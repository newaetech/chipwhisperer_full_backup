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

__author__ = "Colin O'Flynn"

import sys
import os
import re
import inspect
from chipwhisperer.common.utils import util

#We always import PySide first, to force usage of PySide over PyQt
try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

try:
    from configobj import ConfigObj  # import the module
except ImportError:
    print "ERROR: configobj (https://pypi.python.org/pypi/configobj/) is required for this program"
    sys.exit()

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    # print pg.systemInfo()

except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.api.dictdiffer import DictDiffer

def delete_keys_from_dict(dict_del, lst_keys):
    for k in lst_keys:
        try:
            del dict_del[k]
        except KeyError:
            pass
    for v in dict_del.values():
        if isinstance(v, dict):
            delete_keys_from_dict(v, lst_keys)

def delete_objects_from_dict(d):
    #todel = []
    for key, value in d.iteritems():
        if isinstance(value, (dict, list, tuple)):
            delete_objects_from_dict(value)
        
        elif inspect.ismethod(value) or isinstance(value, QObject):
            #todel.append(key)
            #print key
            d[key] = value.__class__.__name__
            

    #for k in todel:
    #    del d[k]

import collections

def convert_to_str(data):
    """
    Converts all dictionary elements to string type - similar to what ConfigObj will
    be doing when it saves and loads the data.
    """
    if isinstance(data, collections.Mapping):
        return dict(map(convert_to_str, data.iteritems()))
    elif isinstance(data, collections.Iterable) and not isinstance(data, basestring):
        return type(data)(map(convert_to_str, data))
    else:
        return str(data)

class ConfigObjProj(ConfigObj):
    """
    Extends ConfigObj to add a callback feature when something is written, used
    to determine when the project file becomes 'dirty'.
    """

    def __init__(self, callback=None, *args, **kwargs):
        self._callback = callback
        super(ConfigObjProj, self).__init__(*args, **kwargs)

    def setCallback(self, cb):
        self._callback = cb

    def __setitem__(self, key, value, unrepr=False):
        super(ConfigObjProj, self).__setitem__(key, value, unrepr)
        if self._callback:
            self._callback(key)

class ProjectDiffWidget(QWidget):
    """Widget that displays differences between versions of the project file"""

    def __init__(self, parent=None, project=None):
        super(ProjectDiffWidget, self).__init__(parent)

        self._project = project

        hlayout = QHBoxLayout()

        self.changedTree = ParameterTree()
        self.addedTree = ParameterTree()
        self.deletedTree = ParameterTree()

        self.updateParamTree(self.changedTree, [], "Changed Sections")
        self.updateParamTree(self.addedTree, [], "Added Sections")
        self.updateParamTree(self.deletedTree, [], "Removed Sections")

        hlayout.addWidget(self.changedTree)
        hlayout.addWidget(self.addedTree)
        hlayout.addWidget(self.deletedTree)

        self.setLayout(hlayout)

    def setProject(self, proj):
        self._project = proj
        # self._project.valueChanged.connect(self.doDiff)
        
    def updateParamTree(self, paramTree, changelist, name):
        paramlist = []
        for k in changelist:
            paramlist.append({'name':k})
        params = Parameter.create(name=name, type='group', children=paramlist)
        ExtendedParameter.reloadParams([params], paramTree)

    def checkDiff(self, ignored=None, updateGUI=False):
        """
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        if self._project.traceManager:
            self._project.saveTraceManager()

        disk = convert_to_str(ConfigObjProj(infile=self._project.filename))
        ram = convert_to_str(self._project.config)
        diff = DictDiffer(ram, disk)

        added = diff.added()
        removed = diff.removed()
        changed = diff.changed()

        if updateGUI:
            self.updateParamTree(self.changedTree, changed, "Changed Sections")
            self.updateParamTree(self.addedTree, added, "Added Sections (not on disk)")
            self.updateParamTree(self.deletedTree, removed, "Removed Sections (on disk)")

        if (len(added) + len(removed) + len(changed)) == 0:
            return False
        return True


class ProjectFormat(object):

    # Filename changed
    filenameChanged = util.Signal()

    # File changed on disk but perhaps not yet updated
    fileChangedOnDisk = util.Signal()

    # Project settings changed but NOT saved anywhere yet
    valueChanged = util.Signal()

    def __init__(self, parent=None):
        self.settingsDict = {'Project Name':"Untitled", 'Project File Version':"1.00", 'Project Author':"Unknown"}
        self.paramListList = []        
        self.filename = "untitled.cwp"
        self.directory = "."
        self.datadirectory = "default-data-dir/"
        self.config = ConfigObjProj(callback=self.configObjChanged)
        self.traceManager = None
        self.checkDataDirectory()
        self.diffWidget = ProjectDiffWidget(parent, project=self)

        self.dataDirIsDefault = True
        
    def configObjChanged(self, key):
        self.valueChanged.emit(key)

    def hasFilename(self):
        if self.filename == "untitled.cwp":
            return False
        else:
            return True

    def setTraceManager(self, manager):
        self.traceManager = manager
    
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
    
    def addParamTree(self, pt):
        self.paramListList.append(pt)
        
    def addWave(self, configfile):
        return       
        
    def setFilename(self, f):
        self.filename = f
        self.config.filename = f        
        self.datadirectory = os.path.splitext(self.filename)[0] + "_data/"
        self.checkDataDirectory()
        self.dataDirIsDefault = False
        self.filenameChanged.emit(self.filename)
        
    def checkDataDirectory(self):
        # Check if data-directory exists?
        if not os.path.isdir(self.datadirectory):
            os.mkdir(self.datadirectory)

        # Make trace storage directory too
        if not os.path.isdir(os.path.join(self.datadirectory, 'traces')):
            os.mkdir(os.path.join(self.datadirectory, 'traces'))

        # Make analysis storage directory
        if not os.path.isdir(os.path.join(self.datadirectory, 'analysis')):
            os.mkdir(os.path.join(self.datadirectory, 'analysis'))

    def load(self, f=None):
        if f is not None:
            self.setFilename(f)

        self.config = ConfigObjProj(infile=self.filename, callback=self.configObjChanged)

        #TODO: readings????
        

    def getDataFilepath(self, filename, subdirectory='analysis'):
        datadir = os.path.join(self.datadirectory, 'analysis')
        fname = os.path.join(datadir, filename)
        relfname = os.path.relpath(fname, os.path.split(self.config.filename)[0])
        fname = os.path.normpath(fname)
        relfname = os.path.normpath(relfname)
        return {"abs":fname, "rel":relfname}

    def convertDataFilepathAbs(self, relativepath):
        return os.path.join(os.path.split(self.filename)[0], relativepath)
                
    def checkDataConfig(self, config, requiredSettings):
        """Check a configuration section for various settings"""
        requiredSettings = convert_to_str(requiredSettings)
        config = convert_to_str(config)
        return set(requiredSettings.items()).issubset(set(config.items()))

    def getDataConfig(self, sectionName="Aux Data", subsectionName=None, requiredSettings=None):
        """
        Get all configuration sections of data type given in
        __init__() call, and also matching the given sectionName.
        e.g. if dataName='Aux Data' and sectionName='Frequency',
        this would return a list of all sections of the type
        'Aux Data NNNN - Frequency'.
        """

        sections = []

        # Get all section names
        for sname in self.config.keys():
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
        # Check configuration file to find incrementing number
        maxNumber = 0
        for sname in self.config.keys():
            # Find if starts with 'Aux Data'
            if sname.startswith(sectionName):
                maxNumber = int(re.findall(r'\d+', sname)[0]) + 1

        cfgSectionName = "%s %04d" % (sectionName, maxNumber)
        if subsectionName:
            cfgSectionName += " - %s" % subsectionName

        # Generate the configuration section
        self.config[cfgSectionName] = {}

        if settings is not None:
            for k in settings.keys():
                self.config[cfgSectionName][k] = settings[k]

        return self.config[cfgSectionName]
        
    def saveTraceManager(self):
        #Waveform list is Universal across ALL types
        if 'Trace Management' not in self.config:
            self.config['Trace Management'] = {}
            
        if self.traceManager:
            self.traceManager.saveProject(self.config, self.filename)

    def save(self):
        if self.filename is None:
            return

        self.saveTraceManager()
            
        #self.config['Waveform List'] = self.config['Waveform List'] + self.waveList

        #Program-Specific Options
        pn = self.settingsDict['Program Name']
       
        self.config[pn] = {}
        self.config[pn]['General Settings'] =  self.settingsDict

        self.config.write()
        self.fileChangedOnDisk.emit()
    
