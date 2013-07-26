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

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter
    

class SimpleSerial_serial(QObject):
    paramListUpdated = Signal(list) 
    
    def __init__(self):
        super(SimpleSerial_serial, self).__init__()
        

class SimpleSerial_ChipWhisperer(QObject):
    paramListUpdated = Signal(list) 
    
    def __init__(self):
        super(SimpleSerial_ChipWhisperer, self).__init__()
        ssParams = [{'name':'baud', 'type':'list', 'values':['38400'], 'value':'38400', 'get':self.baud, 'set':self.setBaud}]
        self.params = Parameter.create(name='Serial Port Settings', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params)        

    def setBaud(self, baud):
        return
    
    def baud(self):
        return 38400

    def paramList(self):
        return [self.params]
    
    def setOpenADC(self):
        return

class SimpleSerial(QObject):   
    paramListUpdated = Signal(list) 
     
    def __init__(self):
        super(SimpleSerial, self).__init__()
        
        self.ser = None
        ssParams = [{'name':'connection', 'type':'list', 'values':{"System Serial Port":SimpleSerial_serial(), "ChipWhisperer":SimpleSerial_ChipWhisperer()}, 'value':"System Serial Port", 'set':self.setConnection}]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params)

    def __del__(self):
        self.close()
    
    def setOpenADC(self, oadc):
        try:
            self.ser.setOpenADC(oadc)
        except:
            pass

    def setConnection(self, con):
        self.ser = con        
        self.paramListUpdated.emit(self.paramList)
        
    def paramList(self):
        p = [self.params]
        if self.ser is not None:
            for a in self.ser.paramList(): p.append(a)
        return p
    
    def con(self):        
        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
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
      
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        if key != None:            
            cmd = "k"
            for b in key:
                cmd = cmd + "%2x"%b
            cmd = cmd + "\n"
            self.ser.flushInput()
            self.ser.write(cmd)
            #self.ser.read(1)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        response = self.ser.read(33)

        if len(response) < 33:
            print "WARNING: Response too short!"
            return None

        if response[0] != 'r':
            print "Sync Error"
            return None

        data = bytearray(16)

        for i in range(0,16):
            data[i] = int(response[(i*2+1):(i*2+3)], 16)

        return data

    def go(self):
        self.ser.flushInput()
        cmd = "p"
        for b in self.input:
            cmd = cmd + "%2x"%b
        cmd = cmd + "\n"
        self.ser.flushInput()
        self.ser.write(cmd)
        #self.ser.read(1)
   
