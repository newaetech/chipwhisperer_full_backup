#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
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
        self.datadirectory = os.path.basename(self.filename) + "/"
        
    def load(self, f=None):
        if f is not None:
            self.setFilename(f)
            
        #TODO: readings????
        
    def saveparam(self, p):
        #delete_objects_from_dict(p)
        
        if 'Settings' not in self.config[self.settingsDict['Program Name']]:
            self.config[self.settingsDict['Program Name']]['Settings'] = {}
        
        self.config[self.settingsDict['Program Name']]['Settings'][p['name']] = p
                
        
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
    