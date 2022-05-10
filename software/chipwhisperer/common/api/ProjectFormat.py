#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
import logging
import os
import re
import sys
from datetime import datetime
import zipfile
import numpy as np

from chipwhisperer.common.api.TraceManager import TraceManager
from chipwhisperer.common.api.settings import Settings
from chipwhisperer.common.utils.parameter import Parameter, Parameterized, setupSetParam
from chipwhisperer.common.utils import util
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
import copy
from chipwhisperer.common.traces import Trace
from ...logging import *
import shutil

try:
    from configobj import ConfigObj # type: ignore
except ImportError:
    print("ERROR: configobj (https://pypi.python.org/pypi/configobj/) is required for this program")
    sys.exit()

__author__ = "Colin O'Flynn"


def ensure_cwp_extension(path):
    """Ensures that a file path has the '.cwp' extension.

    This can be used to allow specifying project names without
    having to include the '.cwp' extension needed for some of
    the project functionality to work.

    If the file already has the right extension another one
    is not appended.

    Args:
        path (str): A path like string.

    Returns:
        (str) A path that ends with '.cwp'.
    """
    root, ext = os.path.splitext(path)
    if ext != '.cwp':
        return ''.join([root, ext, '.cwp'])
    else:
        return path


class ConfigObjProj(ConfigObj):
    """
    Extends ConfigObj to add a callback feature when something is written, used
    to determine when the project file becomes 'dirty'.
    """

    def __init__(self, callback=None, *args, **kwargs):
        if callback is not None:
            self._callback = util.WeakMethod(callback)
        else:
            self._callback = None

        super(ConfigObjProj, self).__init__(*args, **kwargs)

    def __setitem__(self, key, value, unrepr=False):
        super(ConfigObjProj, self).__setitem__(key, value, unrepr)
        if self._callback:
            self._callback(key)


class Project(Parameterized):
    """Class describing an open ChipWhisperer project.

    Basic capture usage::

        import chipwhisperer as cw
        proj = cw.create_project("project")
        trace = cw.Trace(trace_data, plaintext, ciphertext, key)
        proj.traces.append(trace)
        proj.save()

    Basic analyzer usage::

        import chipwhisperer as cw
        import chipwhisperer.analyzer as cwa
        proj = cw.open_project("project")
        attack = cwa.cpa(proj)
        #run attack

    Use a trace_manager when analyzing traces, since that allows analyzer to
    work with multiple trace segments.

      *  :attr:`project.location <.Project.location>`
      *  :attr:`project.waves <.Project.waves>`
      *  :attr:`project.textins <.Project.textins>`
      *  :attr:`project.textouts <.Project.textouts>`
      *  :attr:`project.keys <.Project.keys>`
      *  :meth:`project.get_filename <.Project.get_filename>`
      *  :meth:`project.trace_manager <.Project.trace_manager>`
      *  :meth:`project.save <.Project.save>`
      *  :meth:`project.export <.Project.export>`
    """
    untitledFileName = os.path.normpath(os.path.join(Settings().value("project-home-dir"), "tmp", "default.cwp"))

    def __init__(self, prog_name="ChipWhisperer", prog_ver=""):
        self.valid_traces = None
        self._trace_format = None

        self.params = Parameter(name="Project Settings", type="group")
        self.params.addChildren([
            {'name': 'Trace Format', 'type': 'list', 'values': self.valid_traces, 'get': self.get_trace_format, 'set': self.set_trace_format},
        ])

        #self.findParam("Trace Format").setValue(TraceContainerNative(project=self), addToList=True)
        self._trace_format = TraceContainerNative(project=self)

        #self.traceParam = Parameter(name="Trace Settings", type='group', addLoadSave=True).register()
        #self.params.getChild('Trace Format').stealDynamicParameters(self.traceParam)

        self.sigFilenameChanged = util.Signal()
        self.sigStatusChanged = util.Signal()
        self.dirty = util.Observable(True)

        self.settingsDict = {'Project Name':"Untitled", 'Project File Version':"1.00", 'Project Author':"Unknown"}
        self.datadirectory = ""
        self.config = ConfigObjProj(callback=self.configObjChanged)
        self._traceManager = TraceManager().register()
        self._traceManager.dirty.connect(self.__dirtyCallback)
        self.setFilename(Project.untitledFileName)

        self.setProgramName(prog_name)
        self.setProgramVersion(prog_ver)

        self._segments = Segments(self)
        self._traces = Traces(self)
        self._keys = IndividualIterable(self._traceManager.get_known_key, self._traceManager.num_traces)
        self._textins = IndividualIterable(self._traceManager.get_textin, self._traceManager.num_traces)
        self._textouts = IndividualIterable(self._traceManager.get_textout, self._traceManager.num_traces)
        self._waves = IndividualIterable(self._traceManager.get_trace, self._traceManager.num_traces)

        if __debug__:
            logging.debug('Created: ' + str(self))

    def append_segment(self, segment):
        """ Adds a new trace segment to the project

        Args:
            segment (TraceContainer): Trace segment to add to project
        """
        self._traceManager.appendSegment(copy.copy(segment))

    appendSegment = append_segment

    def new_segment(self):
        """ Returns the __class__() of the trace container used to store traces
        in the project

        You probably want get_new_trace_segment()

        Returns:
            __class()__ of the current trace container
        """
        return self._trace_format.__class__()

    newSegment = new_segment

    def get_trace_format(self):
        """ Gets the TraceContainer used to store traces

        Returns:
            The trace container used in the project
        """
        return self._trace_format

    getTraceFormat = get_trace_format

    def get_new_trace_segment(self):
        """Return a new TraceContainer object for the specified format.

        Once you're done working with this segment, you can readd it to the
        project with append_segment()

        Returns:
            a new TraceContainer object
        """
        tmp = copy.copy(self._trace_format)
        tmp.clear()
        starttime = datetime.now()
        prefix = starttime.strftime('%Y.%m.%d-%H.%M.%S') + "_"
        tmp.config.setConfigFilename(os.path.join(self.datadirectory, "traces", "config_" + prefix + ".cfg"))
        tmp.config.setAttr("prefix", prefix)
        tmp.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
        return tmp

    getNewTraceSegment = get_new_trace_segment

    @setupSetParam("Trace Format")
    def set_trace_format(self, trace_format):
        self._trace_format = trace_format

    setTraceFormat = util.camel_case_deprecated(set_trace_format)

    def __dirtyCallback(self):
        self.dirty.setValue(self._traceManager.dirty.value() or self.dirty.value())

    def configObjChanged(self, key):
        self.dirty.setValue(True)

    def isUntitled(self):
        return self.filename == Project.untitledFileName

    def trace_manager(self):
        """ Gets the trace manager for the project

        Returns:
            The trace manager for the project
        """
        return self._traceManager

    traceManager = util.camel_case_deprecated(trace_manager)

    def setProgramName(self, name):
        self.settingsDict['Program Name']=name

    def setProgramVersion(self, ver):
        self.settingsDict['Program Version']=ver

    def setAuthor(self, author):
        self.settingsDict['Project Author']=author

    def setProjectName(self, name):
        self.settingsDict['Project Name']=name

    def setFileVersion(self, ver):
        self.settingsDict['Project File Version']=ver


    def get_filename(self):
        """Gets the filename associated with the project.

        Returns:
            Filename of the project config file ending with ".cwp".
        """
        return self.filename

    getFilename = util.camel_case_deprecated(get_filename)

    def setFilename(self, f):
        self.filename = f
        self.config.filename = f
        self.datadirectory = os.path.splitext(self.filename)[0] + "_data"
        self.createDataDirectory()
        self.sigStatusChanged.emit()

    def createDataDirectory(self):
        # Check if data-directory exists?
        if not os.path.isdir(self.datadirectory):
            os.makedirs(self.datadirectory)

        # Make trace storage directory too
        if not os.path.isdir(os.path.join(self.datadirectory, 'traces')):
            os.mkdir(os.path.join(self.datadirectory, 'traces'))

        # Make analysis storage directory
        if not os.path.isdir(os.path.join(self.datadirectory, 'analysis')):
            os.mkdir(os.path.join(self.datadirectory, 'analysis'))

        # Make glitchresults storage directory
        if not os.path.isdir(os.path.join(self.datadirectory, 'glitchresults')):
            os.mkdir(os.path.join(self.datadirectory, 'glitchresults'))

    def load(self, f = None):
        if f is not None:
            self.setFilename(os.path.abspath(os.path.expanduser(f)))

        if not os.path.isfile(self.filename):
            raise IOError("File " + self.filename + " does not exist or is not a file")

        self.config = ConfigObjProj(infile=self.filename, callback=self.configObjChanged)
        self._traceManager.loadProject(self.filename)
        self.dirty.setValue(False)

    def getDataFilepath(self, filename, subdirectory='analysis'):
        datadir = os.path.join(self.datadirectory, subdirectory)
        fname = os.path.join(datadir, filename)
        relfname = os.path.relpath(fname, os.path.split(self.config.filename)[0])
        fname = os.path.normpath(fname)
        relfname = os.path.normpath(relfname)
        return {"abs":fname, "rel":relfname}

    def convertDataFilepathAbs(self, relativepath):
        return os.path.join(os.path.split(self.filename)[0], relativepath)

    def checkDataConfig(self, config, requiredSettings):
        """Check a configuration section for various settings. Don't use."""
        requiredSettings = util.convert_to_str(requiredSettings)
        config = util.convert_to_str(config)
        return set(requiredSettings.items()).issubset(set(config.items()))

    def getDataConfig(self, sectionName="Aux Data", subsectionName=None, requiredSettings=None):
        """ Don't use.
        Get all configuration sections of data type given in
        __init__() call, and also matching the given sectionName.
        e.g. if dataName='Aux Data' and sectionName='Frequency',
        this would return a list of all sections of the type
        'Aux Data NNNN - Frequency'.
        """
        sections = []

        # Get all section names
        for sname in list(self.config.keys()):
            # Find if starts with 'Aux Data'
            if sname.startswith(sectionName):
                # print "Found %s" % sname
                # Find if module name matches if applicable
                if subsectionName is None or sname.endswith(subsectionName):
                    # print "Found %s" % sname

                    if requiredSettings is None:
                        sections.append(self.config[sname])
                    else:
                        self.checkDataConfig(self.config[sname], requiredSettings)

        return sections

    def addDataConfig(self, settings=None, sectionName="Aux Data", subsectionName=None):
        # Check configuration file to find incrementing number. Don't use
        maxNumber = 0
        for sname in list(self.config.keys()):
            # Find if starts with 'Aux Data'
            if sname.startswith(sectionName):
                maxNumber = int(re.findall(r'\d+', sname)[0]) + 1

        cfgSectionName = "%s %04d" % (sectionName, maxNumber)
        if subsectionName:
            cfgSectionName += " - %s" % subsectionName

        # Generate the configuration section
        self.config[cfgSectionName] = {}

        if settings is not None:
            for k in list(settings.keys()):
                self.config[cfgSectionName][k] = settings[k]

        return self.config[cfgSectionName]

    def saveAllSettings(self, fname=None, onlyVisibles=False):
        """ Save registered parameters to a file, so it can be loaded again latter. Don't use."""
        if fname is None:
            fname = os.path.join(self.datadirectory, 'settings.cwset')
            logging.info('Saving settings to file: ' + fname)
        Parameter.saveRegistered(fname, onlyVisibles)

    def saveTraceManager(self):
        #Waveform list is Universal across ALL types. Don't use.
        if 'Trace Management' not in self.config:
            self.config['Trace Management'] = {}

        self._traceManager.save_project(self.config, self.filename)

    def save(self):
        """Saves the project.

        Writes the project to the disk. Before this is called your data
        is not saved. Filenames for traces are set in this method.
        """
        if self.filename is None:
            return

        self.saveTraceManager()

        #self.config['Waveform List'] = self.config['Waveform List'] + self.waveList

        #Program-Specific Options
        pn = self.settingsDict['Program Name']

        self.config[pn] = {}
        self.config[pn]['General Settings'] =  self.settingsDict

        self.config.write()
        self.sigStatusChanged.emit()
        self.dirty.setValue(False)

    def checkDiff(self):
        """ Don't use.
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        self.saveTraceManager()

        disk = util.convert_to_str(ConfigObjProj(infile=self.filename))
        ram = util.convert_to_str(self.config)

    def hasDiffs(self):
        # if self.dirty.value(): return True

        # #added, removed, changed = self.checkDiff()
        # if (len(added) + len(removed) + len(changed)) == 0:
        #     return False
        return True

    def consolidate(self, keepOriginals = True):
        for indx, t in enumerate(self._traceManager.traceSegments):
            destinationDir = os.path.normpath(os.path.join(self.datadirectory, "traces"))
            config = ConfigObj(t.config.configFilename())
            prefix = config['Trace Config']['prefix']
            tracePath = os.path.normpath(os.path.split(t.config.configFilename())[0])

            if destinationDir == tracePath: continue

            for traceFile in os.listdir(tracePath):
                if traceFile.startswith(prefix):
                    util.copyFile(os.path.normpath(os.path.join(tracePath, traceFile)), destinationDir, keepOriginals)

            util.copyFile(t.config.configFilename(), destinationDir, keepOriginals)
            t.config.setConfigFilename(os.path.normpath(os.path.join(destinationDir, os.path.split(t.config.configFilename())[1])))
        self.sigStatusChanged.emit()

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

    @property
    def location(self):
        """The directory in which the project is located.

        Example::

            print(project.location)
            '/path/to/the/directory/containing/this/project'

        :Getter:
            (str) Returns the file path of the projects parent directory.

        .. versionadded:: 5.1
            Added **location** attribute to project.
        """
        return os.path.dirname(os.path.abspath(self.get_filename()))

    def export(self, file_path, file_type='zip'):
        """Export a chipwhisperer project.

        Saves project before exporting.

        Supported export types:
          *  zip (default)

        Returns:
            (str) Path to the exported file.

        .. versionadded:: 5.1
            Add **export** method to active project.
        """
        self.save()

        _, cwp_file = os.path.split(self.get_filename())
        name, ext = os.path.splitext(cwp_file)
        data_folder = os.path.join(self.location, '_'.join([name, 'data']))

        file_paths = list()
        file_paths.append(os.path.join(self.location, cwp_file))

        for root, directories, files in os.walk(data_folder):
            for filename in files:
                file_paths.append(os.path.join(root, filename))

        if file_type == 'zip':
            file_path = os.path.abspath(file_path)
            with zipfile.ZipFile(file_path, 'w', zipfile.ZIP_DEFLATED) as myzip:
                common_path = os.path.commonpath(file_paths)
                for file in file_paths:
                    relative_path = os.path.relpath(file, common_path)
                    myzip.write(file, arcname=relative_path)
                    export_file_path = os.path.abspath(myzip.filename)
        else:
            raise ValueError('{} not supported'.format(file_type))

        return export_file_path

    def close(self, save=True):
        """Closes the project cleanly.

        Saves by default. Then closes all claimed files.

        Args:
            save (bool): Saves the project before closing.
        """
        if save:
            self.save()

        for seg in self.segments:
            seg.unloadAllTraces()

    def remove(self, i_am_sure=False):
        """Remove a project from disk.

        Args:
            i_am_sure (bool): Are you sure you want to remove the project?
        """
        if not i_am_sure:
            raise RuntimeWarning('Project not removed... i_am_sure not set to True.')

        self.close(save=False)

        try:
            shutil.rmtree(self.datadirectory)
        except FileNotFoundError:
            pass

        _, cwp_file = os.path.split(self.get_filename())
        try:
            os.remove(os.path.join(self.location, cwp_file))
        except FileNotFoundError:
            pass

    @property
    def traces(self):
        """The interface to all traces contained in the project.

        Instance of :class:`.Traces`.
        """
        return self._traces

    @property
    def segments(self):
        """The interface to all segments contained in the project.

        Instance of :class:`.Segments`.
        """
        return self._segments

    @property
    def keys(self):
        """Iterable for working with only the known keys.

        Each item in the iterable is a byte array.

        Supports iterating, indexing, and slicing::

            for key in my_project.keys:
                # do something
        """
        return self._keys

    @property
    def textins(self):
        """Iterable for working with only the text in.

        Each item in the iterable is a byte array.

        Supports iterating, indexing, and slicing::

            for textin in my_project.textins:
                # do something
        """
        return self._textins

    @property
    def textouts(self):
        """Iterable for working with only the text out.

        Each item in the iterable is a byte array.

        Supports iterating, indexing, and slicing::

            for textout in my_project.textouts:
                # do something
        """
        return self._textouts

    @property
    def waves(self):
        """Iterable for working with only the trace data.

        Each item in the iterable is a numpy array.

        Supports iterating, indexing, and slicing::

            for wave in my_project.waves:
                # do something
        """
        return self._waves


class Traces:
    """Contains the trace interface for the project.

    The class adds support for indexing, slicing and iterating
    of project traces. To add traces to the project use
    :meth:`append <.Traces.append>`. ::

        for trace in my_traces:
            my_project.traces.append(trace)

    To iterate through all traces use::

        for trace in my_project.traces:
            print(trace.wave, trace.textin, trace.textout, trace.key)

    Indexing is supported::

        trace_of_interest = my_project.traces[99]

    So is slicing::

        interesting_traces = my_project.traces[20:35]

    Args:
        project: The project class where traces will be stored.
        segment_length (int): The number of traces stored in one file
            before another file is created. This is used for unloading
            and loading traces from disk to keep memory usage reasonable.

    Returns:
        (iterable) of :class:`traces <chipwhisperer.common.traces.Trace>`
        in the project if the traces belongs
        to a segment that is enabled. The iterable contains tuples
        of length four containing the trace, plain text in,
        encrypted text out, and known key.

    Raises:
        IndexError: When the index used for indexing is out of range.
        TypeError: When key used to get item is not a slice or integer.

    .. versionadded:: 5.1
        Added **Traces** class to project interface.
    """

    def __init__(self, project, segment_length=10000):
        self.project = project
        self.tm = project._traceManager

        # segment management
        self.cur_seg = self.project.segments.new()
        self.project.segments.append(self.cur_seg)
        self.cur_trace_num = 0
        self.seg_len = segment_length
        self.seg_ind_max = self.seg_len - 1

    @property
    def max(self):
        """Max index during iteration."""
        return self.tm.num_traces() - 1
    
    @property
    def keys(self):
        return self._keys

    def append(self, trace, dtype=np.double):
        """Append a Trace containing the trace and related operation information.

        Args:
            trace (:class:`Trace <chipwhisperer.common.trace.Trace>`): A captured or created trace.
            dtype: Numpy data type for storing trace.wave

        Raises:
            TypeError: When trying to append something other than a trace.
        """
        if not isinstance(trace, Trace):
            other_logger.error("Invalid type appended to traces. Try appending cw.Trace(trace_data, textin, textout, key)")
            raise TypeError("Expected Trace object, got {}.".format(type(trace)))

        if self.cur_trace_num > self.seg_ind_max:
            self.cur_seg = self.project.segments.new()
            self.project.segments.append(self.cur_seg)
            self.cur_trace_num = 0
        self.cur_seg.add_trace(*trace, dtype=dtype)
        self.cur_trace_num += 1

    def extend(self, iterable):
        """Add all traces in an iterable to the project.

        Args:
            iterable: Any iterable of :class:`Trace <chipwhisperer.common.trace.Trace>` objects.

        Raises:
            TypeError: If any of the object in the iterable are not a trace.
        """
        for item in iterable:
            self.append(item)

    def __len__(self):
        return self.tm.num_traces()

    def __iter__(self):
        self.n = 0
        return self

    def __next__(self):
        if self.n > self.max:
            raise StopIteration

        result = Trace(
            self.tm.get_trace(self.n),
            self.tm.get_textin(self.n),
            self.tm.get_textout(self.n),
            self.tm.get_known_key(self.n),
        )

        self.n += 1
        return result

    def __getitem__(self, item):
        if isinstance(item, int):
            ind = item
            if ind < 0:
                ind = self.max + ind + 1

            if not (0 <= ind <= self.max):
                raise IndexError('Index outside of range ({}, {})'.format(0, self.max))

            result = Trace(
                self.tm.get_trace(ind),
                self.tm.get_textin(ind),
                self.tm.get_textout(ind),
                self.tm.get_known_key(ind)
            )
            return result

        elif isinstance(item, slice):
            indices = item.indices(self.tm.num_traces())
            result = []
            for i in range(*indices):
                result.append(
                    Trace(
                        self.tm.get_trace(i),
                        self.tm.get_textin(i),
                        self.tm.get_textout(i),
                        self.tm.get_known_key(i)
                    )
                )
            return result
        else:
            raise TypeError('Indexing by integer or slice only')

    def __repr__(self):
        _, project_filename = os.path.split(self.project.get_filename())
        abs_path = os.path.join(self.project.location, project_filename)
        return 'Traces(number={}, project={}) for project at {}'.format(len(self), self.project, abs_path)

    def __str__(self):
        result = []
        i = 0
        while i < 10:
            try:
                result.append(self[i])
            except IndexError:
                break
            i += 1
        return '{} \n Result truncated at maximum 10 lines...'.format(str(np.array(result)))


class Segments:
    """Contains the segment interface for a project.

    Segments are used to group traces together. Each segment can be
    enabled/disabled to be included in the traces returned by
    :attr:`.traces`

    .. warning:: For internal use only. This api is not guaranteed to exist
        if our project switches to a different way of storing project
        files.

    Returns:
        (iterable) of segments, usually some sort of trace container.

    """

    def __init__(self, project):
        self.project = project
        self.tm = project._traceManager
        self.trace_container = project._trace_format
        self.data_directory = project.datadirectory

    def __len__(self):
        return len(self.tm.traceSegments)

    def new(self):
        """Used to get a new empty trace container (segment).
        
        Returns:
            (TraceContainer) A new empty instance of a trace container.
        """
        seg = copy.copy(self.trace_container)
        seg.clear()
        return seg

    def append(self, seg):
        self.tm.appendSegment(seg)

    def __iter__(self):
        self.n = 0
        self.max = len(self) - 1
        return self

    def __next__(self):
        if self.n > self.max:
            raise StopIteration
        result = self.tm.traceSegments[self.n]
        self.n += 1
        return result

    def __getitem__(self, item):
        return self.tm.traceSegments[item]

    def __repr__(self):
        _, project_filename = os.path.split(self.project.get_filename())
        abs_path = os.path.join(self.project.location, project_filename)
        return 'Segments(num={}, project={}) for project at {}'.format(len(self), self.project, abs_path)


class IndividualIterable:
    
    def __init__(self, getter_func, trace_num_func):
        self.getter = getter_func
        self.trace_num_func = trace_num_func

    @property
    def max(self):
        return self.trace_num_func() - 1

    def __iter__(self):
        self.n = 0
        return self

    def __next__(self):
        if self.n > self.max:
            raise StopIteration

        self.n += 1
        return self.getter(self.n - 1)
    
    def __len__(self):
        return self.trace_num_func()

    def __getitem__(self, item):
        if isinstance(item, int):
            ind = item
            if ind < 0:
                ind = self.max + ind + 1

            if not (0 <= ind <= self.max):
                raise IndexError('Index outside of range ({}, {})'.format(0, self.max))

            return self.getter(ind)

        elif isinstance(item, slice):
            indices = item.indices(self.max+1)
            result = []
            for i in range(*indices):
                result.append(self.getter(i))
            return result
        else:
            raise TypeError('Indexing by integer or slice only')

    def __array__(self):
        # to make converting to numpy arrays much easier
        if hasattr(self.getter(0), "dtype"):
            dtype = self.getter(0).dtype
        else:
            dtype = 'uint8'
        num_traces = self.trace_num_func()
        len_trace = len(self.getter(0))
        arr = np.zeros((num_traces, len_trace), dtype=dtype)
        arr[:,:] = np.array([self.getter(i) for i in range(num_traces)])[:,:]
        return arr
