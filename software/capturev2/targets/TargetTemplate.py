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

from PySide.QtCore import *
from PySide.QtGui import *

try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter

class TargetTemplate(QObject):
    paramListUpdated = Signal(list)
    
    def __init__(self, console=None, showScriptParameter=None):
        """Pass None/None if you don't have/want console/showScriptParameter"""
        super(TargetTemplate, self).__init__()        
        self.console = console
        self.showScriptParameter = showScriptParameter                
        self.setupParameters()
                
    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        ssParams = [{'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}]        
        self.params = Parameter.create(name='Smartcard Reader', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
            
    def paramList(self):
        p = [self.params]
        #if self.ser is not None:
        #    for a in self.ser.paramList(): p.append(a)
        return p        
        
    def __del__(self):
        """Close system if needed"""
        self.close()
        
    def log(self, msg):
        if self.console is not None:
            self.console.append(msg)
        else:
            print msg
            
    def setOpenADC(self, oadc):
        self.oa = oadc
        
    
    def dis(self):
        """Disconnect from target"""
        self.close()

    def con(self):   
        """Connect to target"""
        pass

    def flush(self):
        """Flush input/output buffers"""
        pass

    def close(self):
        """Close target"""
        pass

    def init(self):
        """Init Hardware"""
        pass
    
    def reinit(self):
        pass

    def setModeEncrypt(self):
        """Set hardware to do encryption, if supported"""
        pass

    def setModeDecrypt(self):
        """Set hardware to do decryption, if supported"""
        pass

    def checkEncryptionKey(self, key):
        """System 'suggests' encryption key, and target modifies it if required because e.g. hardware has fixed key"""
        return key

    def loadEncryptionKey(self, key):
        """Load desired encryption key"""        
        pass

    def loadInput(self, inputtext):
        """Load input plaintext"""
        pass

    def isDone(self):
        """If encryption takes some time after 'go' called, lets user poll if done"""
        return True

    def readOutput(self):        
        """Read result"""

    def go(self):
        """Do Encryption"""
