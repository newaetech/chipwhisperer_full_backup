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
#=================================================

import sys
from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.utils import util
from pyqtgraph.parametertree import Parameter

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None

def getInstance(*args):
    return TargetTemplate(*args)

class TargetTemplate(object):
    params = None

    def __init__(self, showScriptParameter=None):
        """Pass None/None if you don't have/want showScriptParameter"""
        self.showScriptParameter = showScriptParameter
        self.setupParameters()
        self._ConnectionStatus = False
        self.paramListUpdated = util.Signal()
        self.newInputData = util.Signal()
        self.connStatusUpdated = util.Signal()

    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        ssParams = [{'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}]        
        self.params = Parameter.create(name='Smartcard Reader', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
                        
    def setSomething(self):
        """Here you would send value to the reader hardware"""
        pass

    def paramList(self):
        p = [self.params]
        #if self.ser is not None:
        #    for a in self.ser.paramList(): p.append(a)
        return p        
        
    def __del__(self):
        """Close system if needed"""
        self.close()

    def setOpenADC(self, oadc):
        self.oa = oadc

    def getStatus(self):
        return self._ConnectionStatus

    def dis(self):
        """Disconnect from target"""
        self.close()

    def con(self):
        """Connect to target"""

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
        self.key = key

    def loadInput(self, inputtext):
        """Load input plaintext"""
        self.input = inputtext

    def isDone(self):
        """If encryption takes some time after 'go' called, lets user poll if done"""
        return True

    def readOutput(self):        
        """Read result"""
        raise NotImplementedError("TargetTemplate.readOutput()")

    def go(self):
        """Do Encryption"""
        raise NotImplementedError("TargetTemplate.go()")

    def keyLen(self):
        """Length of key system is using"""
        return 16
    
    def getExpected(self):
        """Based on key & text get expected if known, otherwise returns None"""
        
        # e.g. for AES we can do this:
        if AES and hasattr(self, 'key') and hasattr(self, 'input') and self.input and self.key:
            cipher = AES.new(str(self.key), AES.MODE_ECB)
            ct = cipher.encrypt(str(self.input))
            ct = bytearray(ct)
            return ct
        else:
            return None

    def getName(self):
        return "None"