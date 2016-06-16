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

import os
import re
import sys
from chipwhisperer.common.utils import util
from chipwhisperer.common.api.dictdiffer import DictDiffer
from chipwhisperer.common.api.TraceManager import TraceManager
from chipwhisperer.common.api.settings import Settings

try:
    from configobj import ConfigObj  # import the module
except ImportError:
    print "ERROR: configobj (https://pypi.python.org/pypi/configobj/) is required for this program"
    sys.exit()


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


class ProjectFormat(object):
    untitledFileName = os.path.normpath(os.path.join(Settings().value("project-home-dir"), "tmp/default.cwp"))

    def __init__(self):
        self.sigFilenameChanged = util.Signal()
        self.sigStatusChanged = util.Signal()
        self.dirty = util.Observable(True)

        self.settingsDict = {'Project Name':"Untitled", 'Project File Version':"1.00", 'Project Author':"Unknown"}
        self.paramListList = []        
        self.datadirectory = ""
        self.config = ConfigObjProj(callback=self.configObjChanged)
        self._traceManager = TraceManager().register()
        self._traceManager.dirty.connect(lambda: self.dirty.setValue(self._traceManager.dirty.value() or self.dirty.value()))
        self.setFilename(ProjectFormat.untitledFileName)

    def configObjChanged(self, key):
        self.dirty.setValue(True)

    def isUntitled(self):
        return self.filename == ProjectFormat.untitledFileName

    def traceManager(self):
        return self._traceManager

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
        
    def getFilename(self):
        return self.filename

    def setFilename(self, f):
        self.filename = f
        self.config.filename = f        
        self.datadirectory = os.path.splitext(self.filename)[0] + "_data/"
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
            self.setFilename(f)

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
        """Check a configuration section for various settings"""
        requiredSettings = util.convert_to_str(requiredSettings)
        config = util.convert_to_str(config)
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
            
        self._traceManager.saveProject(self.config, self.filename)

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
        self.sigStatusChanged.emit()
        self.dirty.setValue(False)

    def checkDiff(self):
        """
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        self.saveTraceManager()

        disk = util.convert_to_str(ConfigObjProj(infile=self.filename))
        ram = util.convert_to_str(self.config)
        diff = DictDiffer(ram, disk)

        added = diff.added()
        removed = diff.removed()
        changed = diff.changed() #TODO: bug when comparing projects with template sections. It is returning changes when there is not.

        return added, removed, changed

    def hasDiffs(self):
        if self.dirty.value(): return True

        added, removed, changed = self.checkDiff()
        if (len(added) + len(removed) + len(changed)) == 0:
            return False
        return True

    def consolidate(self, keepOriginals = True):
        for indx, t in enumerate(self._traceManager.traceSets):
            destinationDir = os.path.normpath(self.datadirectory + "traces/")
            config = ConfigObj(t.config.configFilename())
            prefix = config['Trace Config']['prefix']
            tracePath = os.path.normpath(os.path.split(t.config.configFilename())[0])

            if destinationDir == tracePath: continue

            for traceFile in os.listdir(tracePath):
                if traceFile.startswith(prefix):
                    util.copyFile(os.path.normpath(tracePath + "/" + traceFile), destinationDir, keepOriginals)

            util.copyFile(t.config.configFilename(), destinationDir, keepOriginals)
            t.config.setConfigFilename(os.path.normpath(destinationDir + "/" + os.path.split(t.config.configFilename())[1]))
        self.sigStatusChanged.emit()
