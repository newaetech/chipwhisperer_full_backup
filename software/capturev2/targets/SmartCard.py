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
#=================================================
import sys
import serial

from PySide.QtCore import *
from PySide.QtGui import *

import time

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter

class ReaderChipWhisperer(QObject):
    paramListUpdated = Signal(list)
    
    def __init__(self, console=None, showScriptParameter=None):
        super(ReaderChipWhisperer, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        
        self.ser = None
        ssParams = [{'name':'Baud', 'type':int, 'value':9600, 'set':self.setBaud}                                                            
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)       

    def sendAPDU(self):
        pass
    
    def getResponse(self):
        pass
    
class ProtocolSASEBOWCardOS(QObject):
    paramListUpdated = Signal(list)
    
    def __init__(self, console=None, showScriptParameter=None):
        super(ProtocolSASEBOWCardOS, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        
        self.ser = None
        ssParams = [{'name':'Baud', 'type':int, 'value':9600, 'set':self.setBaud}                                                            
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)       

    def sendAPDU(self):
        pass
    
    def getResponse(self):
        pass
               
class SmartCard(QObject):   
    paramListUpdated = Signal(list) 
     
    def __init__(self, console=None, showScriptParameter=None):
        super(SmartCard, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        
        self.ser = None
        ssParams = [{'name':'Reader Hardware', 'type':'list', 'values':{"ChipWhisperer-Connected":None, "PC/SC Reader":None, "Cheapskate-Serial":None}, 'value':None, 'set':self.setConnection},
                    #"BasicCard v7.5 (INCOMPLETE"):None, 
                    #"Custom (INCOMPLETE)":None, "DPAContestv4 (INCOMPLETE)":None
                    {'name':'SmartCard Protocol', 'type':'list', 'values':{"SASEBO-W SmartCard OS":None, "ChipWhisperer-Dumb":None}, 'value':None, 'set':self.setProtocol}                    
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        

    def __del__(self):
        self.close()
        
    def log(self, msg):
        if self.console is not None:
            self.console.append(msg)
        else:
            print msg
            
    def setOpenADC(self, oadc):
        try:
            self.ser.setOpenADC(oadc)
        except:
            pass

    def setConnection(self, con):
        self.ser = con        
        self.paramListUpdated.emit(self.paramList)
        
    def setProtocol(self, con):
        return
        
    def paramList(self):
        p = [self.params]
        if self.ser is not None:
            for a in self.ser.paramList(): p.append(a)
        return p
    
    def con(self):        
        self.ser.flush()

    def dis(self):
        self.close()
    
    def close(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        return
        
    def init(self):
        return
      
    def loadEncryptionKey(self, key):
        pass
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        return ""

    def go(self):
        pass        

