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
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#=================================================

# from ...capture.api.programmers import Programmer
from ...common.utils import util


class TargetTemplate:
    _name = 'Target Connection'
    connectStatus=False

    def __init__(self):
        pass

    def getName(self):
        return ""

    def setSomething(self):
        """Here you would send value to the reader hardware"""
        pass

    def _dis(self):
        pass

    def __del__(self):
        """Close system if needed"""
        self.close()

    def getStatus(self):
        return self.connectStatus

    def dis(self):
        """Disconnect from target"""
        # self.close()
        self._dis()
        self.connectStatus = False


    def con(self, scope=None, **kwargs):
        """Connect to target"""
        # Programmer.lastFlashedFile = "unknown"
        try:
            self.connectStatus = True
            self._con(scope, **kwargs)
        except:
            self.dis()
            raise


    @property
    def output_len(self):
        """The length of the output expected from the crypto algorithm (in bytes)"""
        return 16

    @output_len.setter
    def output_len(self, length):
        return 16

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
        return None
        # """if AES and hasattr(self, 'key') and hasattr(self, 'input') and self.input and self.key:
        #     cipher = AES.new(bytes(self.key), AES.MODE_ECB)
        #     ct = cipher.encrypt(bytes(self.input))
        #     ct = bytearray(ct)
        #     return ct
        # else:
        #     return None"""

    def validateSettings(self):
        # return [("warn", "Target Module", "You can't use module \"" + self.getName() + "\"", "Specify other module", "57a3924d-3794-4ca6-9693-46a7b5243727")]
        return []

    def _dict_repr(self):
        raise NotImplementedError("Must define target-specific properties.")

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()
