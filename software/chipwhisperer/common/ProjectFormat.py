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

import inspect

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
    if isinstance(data, collections.Mapping):
        return dict(map(convert_to_str, data.iteritems()))
    elif isinstance(data, collections.Iterable):
        return type(data)(map(str, data))
    else:
        return str(data)

class ProjectFormat(QObject):
    def __init__(self):
        super(ProjectFormat, self).__init__()
        self.settingsDict = {'Project Name':"Untitled", 'Project File Version':"1.00", 'Project Author':"Unknown"}
        self.paramListList = []        
        self.filename = "untitled.cwp"
        self.directory = "."
        self.datadirectory = "default-data-dir/"
        self.config = ConfigObj()
        self.traceManager = None
        self.checkDataDirectory()
        
        self.dataDirIsDefault = True

        
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

        self.config = ConfigObj(self.filename)
            
        #TODO: readings????
        
    def saveparam(self, p):
        #delete_objects_from_dict(p)
        
        if 'Settings' not in self.config[self.settingsDict['Program Name']]:
            self.config[self.settingsDict['Program Name']]['Settings'] = {}
        
        self.config[self.settingsDict['Program Name']]['Settings'][p['name']] = p

    def getDataFilepath(self, filename, subdirectory='analysis'):
        datadir = os.path.join(self.datadirectory, 'analysis')
        fname = os.path.join(datadir, filename)
        relfname = os.path.relpath(fname, os.path.split(self.config.filename)[0])
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
        
    def save(self):
        if self.filename is None:
            return

        #Waveform list is Universal across ALL types
        if 'Trace Management' not in self.config:
            self.config['Trace Management'] = {}
            
        if self.traceManager:
            self.traceManager.saveProject(self.config, self.filename)
            
        #self.config['Waveform List'] = self.config['Waveform List'] + self.waveList

        #Program-Specific Options
        pn = self.settingsDict['Program Name']
       
        self.config[pn] = {}
        self.config[pn]['General Settings'] =  self.settingsDict
                
        for p in self.paramListList:
            if p is not None:
                for a in p.paramList():
                    if a is not None:
                        self.saveparam( a.saveState() )
        
                
        self.config.write()        
    
