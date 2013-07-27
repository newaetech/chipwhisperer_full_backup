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
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import tracereader_dpacontestv3
import tracereader_native
import re
import numpy as np
from scipy import signal

#For copying files when adding existing traces
import shutil
import glob

#For profiling support (not 100% needed)
import pstats, cProfile

#Reading trace config files
import ConfigParser

class TraceContainerConfig():
    def __init__(self, configfile=None):
        self.enabled = False
        self.mappedRange = None
        self.numTraces = 0
        self.date = ""
        self.prefix = ""
        self.points = 0
        self.targetHW = ""
        self.targetSW = ""
        self.scope = ""
        self.samplerate = 0
        self.yscale = 1
        self.yunits = "digits"
        self.notes = ""

        self.trace = None

        self.configfile = None

    def loadTrace(self, configfile=None):

        if configfile:
            self.configfile = configfile

        config = ConfigParser.RawConfigParser()
        config.read(self.configfile)
        
        sname = "Trace Config"
        
        self.numTraces = config.getint(sname, 'NumTraces')
        self.date = config.get(sname, 'Date')
        self.prefix = config.get(sname, 'Prefix')
        self.points = config.getint(sname, 'Points')
        self.targetHW = config.get(sname, 'TargetHW')
        self.targetSW = config.get(sname, 'TargetSW')
        self.scope = config.get(sname, 'ScopeName')
        self.samplerate = config.get(sname, 'ScopeSampleRate')
        self.yscale = config.getfloat(sname, 'ScopeYScale')
        self.yunits = config.get(sname, 'ScopeYUnits')
        self.notes = config.get(sname, 'Notes')

    def saveTrace(self, configfile = None):
        if configfile:
            self.configfile = configfile

        config = ConfigParser.RawConfigParser()
        sname = "Trace Config"
        
        config.add_section(sname)
        config.set(sname, 'NumTraces', self.numTraces)
        config.set(sname, 'Date', self.date)
        config.set(sname, 'Prefix', self.prefix)
        config.set(sname, 'Points', self.points)
        config.set(sname, 'TargetHW', self.targetHW)
        config.set(sname, 'TargetSW', self.targetSW)
        config.set(sname, 'ScopeName', self.scope)
        config.set(sname, 'ScopeSampleRate', self.samplerate)
        config.set(sname, 'ScopeYScale', self.yscale)
        config.set(sname, 'ScopeYUnits', self.yunits)
        config.set(sname, 'Notes', self.notes)

        configfile = open(self.configfile, 'wb')
        config.write(configfile)
        configfile.flush()
        configfile.close()

    def checkTraceItem(self, itemname, localitem, changedlist):
        if localitem != self.__dict__[itemname]:                      
            changedlist.append(itemname)
            return True
        return False        

    def checkTrace(self, configfile = None):
        if configfile == None:
            configfile = self.configfile

        config = ConfigParser.RawConfigParser()
        config.read(configfile)
        
        sname = "Trace Config"

        numTraces = config.getint(sname, 'NumTraces')
        date = config.get(sname, 'Date')
        prefix = config.get(sname, 'Prefix')
        points = config.getint(sname, 'Points')
        targetHW = config.get(sname, 'TargetHW')
        targetSW = config.get(sname, 'TargetSW')
        scope = config.get(sname, 'ScopeName')
        samplerate = config.get(sname, 'ScopeSampleRate')
        yscale = config.getfloat(sname, 'ScopeYScale')
        yunits = config.get(sname, 'ScopeYUnits')
        notes = config.get(sname, 'Notes')

        #Compare memory to sections
        changed = False
        changedName = []
        changed = changed or self.checkTraceItem("numTraces", numTraces, changedName)
        changed = changed or self.checkTraceItem("date", date, changedName)
        changed = changed or self.checkTraceItem("prefix", prefix, changedName)
        changed = changed or self.checkTraceItem("points", points, changedName)
        changed = changed or self.checkTraceItem("targetHW", targetHW, changedName)
        changed = changed or self.checkTraceItem("targetSW", targetSW, changedName)
        changed = changed or self.checkTraceItem("scope", scope, changedName)
        changed = changed or self.checkTraceItem("samplerate", samplerate, changedName)
        changed = changed or self.checkTraceItem("yscale", yscale, changedName)
        changed = changed or self.checkTraceItem("yunits", yunits, changedName)
        changed = changed or self.checkTraceItem("notes", notes, changedName)

        #print changed
        #print changedName

        #TODO: Actually save or anything else
        
        return changed