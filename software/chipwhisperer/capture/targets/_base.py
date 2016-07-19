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

from chipwhisperer.capture.api.programmers import Programmer
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized, Parameter

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None


class TargetTemplate(Parameterized, Plugin):
    _name = 'Target Connection'

    def __init__(self):
        self.newInputData = util.Signal()
        self.connectStatus = util.Observable(False)
        self.getParams().register()

    def setSomething(self):
        """Here you would send value to the reader hardware"""
        pass

    def __del__(self):
        """Close system if needed"""
        self.close()

    def getStatus(self):
        return self.connectStatus.value()

    def dis(self):
        """Disconnect from target"""
        self.close()
        self.connectStatus.setValue(False)

    def con(self, scope=None):
        """Connect to target"""
        Programmer.lastFlashedFile = "unknown"
        try:
            self.connectStatus.setValue(True)
            self._con(scope)
        except:
            self.dis()
            raise


    def _con(self, scope=None):
        raise NotImplementedError

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

    def checkPlaintext(self, text):
        """System suggests plaintext; target modifies as required"""
        return text

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
        raise NotImplementedError("Target \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".readOutput()")

    def go(self):
        """Do Encryption"""
        raise NotImplementedError("Target \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".go()")

    def keyLen(self):
        """Length of key system is using"""
        return 16

    def textLen(self):
        """Length of the plaintext used by the system"""
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

    def validateSettings(self):
        # return [("warn", "Target Module", "You can't use module \"" + self.getName() + "\"", "Specify other module", "57a3924d-3794-4ca6-9693-46a7b5243727")]
        return []
