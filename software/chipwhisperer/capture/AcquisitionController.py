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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import random

from openadc.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.qrc_resources
import chipwhisperer.common.ParameterTypesCustom

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    # print pg.systemInfo()

except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from chipwhisperer.common.MainChip import MainChip
from chipwhisperer.common.ProjectFormat import ProjectFormat


class AcquisitionController(QObject):
    traceDone = Signal(int, list, int)
    captureDone = Signal(bool)

    # (key, plaintext, response)
    newTextResponse = Signal(list, list, list, list)

    def __init__(self, scope, target=None, writer=None, aux=None, keyTextPattern=None):
        super(AcquisitionController, self).__init__()

        self.target = target
        self.scope = scope
        self.writer = writer
        self.aux = aux
        self.running = False
        self.setKeyTextPattern(keyTextPattern)

        self.maxtraces = 1

        if self.aux is not None:
            self.aux.captureInit()

    def setKeyTextPattern(self, pat):
        self._keyTextPattern = pat
        if pat:
            self._keyTextPattern.initPair()

    def TargetDoTrace(self, plaintext, key=None):
        if self.target is None:
            return

        if key:
            self.target.loadEncryptionKey(key)
        self.target.loadInput(plaintext)
        self.target.go()

        while self.target.isDone() == False:
            continue

        # print "DEBUG: Target go()"

        resp = self.target.readOutput()
        # print "DEBUG: Target readOutput()"

        # print "pt:",
        # for i in plaintext:
        #    print " %02X"%i,
        # print ""

        # print "sc:",
        # for i in resp:
        #    print " %02X"%i,
        # print ""

        self.newTextResponse.emit(self.key, plaintext, resp, self.target.getExpected())

        return resp

    def doSingleReading(self, update=True, N=None):

        # Get key / plaintext now
        data = self._keyTextPattern.newPair()
        self.key = data[0]
        self.textin = data[1]

        # Set mode
        if self.target is not None:
            self.target.reinit()
            self.target.setModeEncrypt()
            self.target.loadEncryptionKey(self.key)

        if self.aux is not None:
            self.aux.traceArm()

        if self.scope is not None:
            self.scope.arm()

        if self.target is not None:
            # Load input, start encryption, get output
            self.textout = self.TargetDoTrace(self.textin, self.key)

        # Get ADC reading
        if self.scope is not None:
            try:
                if self.scope.capture(update, N, waitingCallback=QApplication.processEvents) == True:
                    print "Timeout"
                    return False
            except IOError, e:
                print "IOError: %s" % str(e)
                return False

        if self.aux is not None:
            self.aux.traceDone()

        return True

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def abortCapture(self, doAbort=True):
        if doAbort:
            self.running = False

    def doReadings(self, addToList=None):
        self.running = True

        self._keyTextPattern.initPair()
        data = self._keyTextPattern.newPair()
        self.key = data[0]
        self.textin = data[1]

        if self.writer is not None:
            self.writer.prepareDisk()
            self.writer.setKnownKey(self.key)

        if self.aux is not None:
            self.aux.captureInit()

        nt = 0

        while (nt < self.maxtraces) and self.running:
            if self.doSingleReading(True, None) == True:
                if self.writer is not None:
                    self.writer.addTrace(self.scope.datapoints, self.textin, self.textout, self.key)

                nt = nt + 1
                self.traceDone.emit(nt, self.scope.datapoints, self.scope.offset)

            QCoreApplication.processEvents()


        if self.aux is not None:
            self.aux.captureComplete()

        if self.writer is not None:
            # Don't clear trace as we re-use the buffer
            self.writer.closeAll(clearTrace=False)

        if addToList is not None:
            if self.writer is not None:
                addToList.append(self.writer)

        self.captureDone.emit(self.running)

        self.running = False

def hexStrToByteArray(hexStr):
    ba = bytearray()
    for s in hexStr.split():
        ba.append(int(s, 16))
    return ba

class AcqKeyTextPattern_Base(QObject):

    paramListUpdated = Signal(list)

    def __init__(self, console=None, showScriptParameter=None, target=None):
        super(AcqKeyTextPattern_Base, self).__init__()
        self.showScriptParameter = showScriptParameter
        self.console = console
        basicParams = self.setupParams()
        self.params = Parameter.create(name='Key/Text Pattern', type='group', children=basicParams)
        ExtendedParameter.setupExtended(self.params, self)
        self._target = target
        self._initPattern()


    def setTarget(self, target):
        self._target = target
        self._initPattern()

    def keyLen(self):
        if self._target:
            return self._target.keyLen()
        else:
            return 16

    def validateKey(self):
        if self._target:
            if len(self._key) != self._target.keyLen():
                raise IOError("Key Length Wrong for Given Target, %d != %d" % (self._target.keyLen(), len(self.key)))

            self._key = self._target.checkEncryptionKey(self._key)

    def setupParams(self):
        basicParams = [
                      {'name':'Do Something', 'type':'bool'},
                  ]

        return basicParams


    def paramList(self):
        p = [self.params]
        return p
    
    def _initPattern(self):
        """Perform any extra init stuff required. Called at the end of main init() & when target changed."""
        pass

    def setInitialKey(self, initialKey, binaryKey=False):
        pass

    def setInitialText(self, initialText, binaryText=False):
        pass
    
    def initPair(self):
        """Called before a capture run, does not return anything"""
        raise AttributeError("This needs to be reimplemented")

    def newPair(self):
        """Called when a new encryption pair is requested"""
        raise AttributeError("This needs to be reimplemented")

class AcqKeyTextPattern_Basic(AcqKeyTextPattern_Base):

    def setupParams(self):
        
        self._fixedPlain = False
        self._fixedKey = True
        
        basicParams = [
                      {'name':'Key', 'type':'list', 'values':['Random', 'Fixed'], 'value':'Fixed', 'set':self.setKeyType},
                      {'name':'Plaintext', 'type':'list', 'values':['Random', 'Fixed'], 'value':'Random', 'set':self.setPlainType},
                      {'name':'Fixed Encryption Key', 'key':'initkey', 'type':'str', 'set':self.setInitialKey},
                      {'name':'Fixed Plaintext Key', 'key':'inittext', 'type':'str', 'set':self.setInitialText},
                  ]
        return basicParams
    
    def setKeyType(self, t):
        if t == 'Fixed':
            self._fixedKey = True
        elif t == 'Random':
            self._fixedKey = False
        else:
            raise ValueError("Invalid value for Key Type: %s" % t)
        
    def setPlainType(self, t):
        if t == 'Fixed':
            self._fixedPlain = True
        elif t == 'Random':
            self._fixedPlain = False
        else:
            raise ValueError("Invalid value for Text Type: %s" % t)

    def _initPattern(self):
        self.setInitialKey('2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c')
        self.setInitialText('00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F')

    def setInitialKey(self, initialKey, binaryKey=False):
        if initialKey:
            if binaryKey:
                keyStr = ''
                for s in initialKey:
                    keyStr += '%02x ' % s
                self._key = bytearray(initialKey)
            else:
                keyStr = initialKey                  
                self._key = hexStrToByteArray(initialKey)

            self.findParam('initkey').setValue(keyStr)

    def setInitialText(self, initialText, binaryText=False):
        if initialText:
            if binaryText:
                textStr = ''
                for s in initialText:
                    textStr += '%02x ' % s
                self._textin = bytearray(initialText)
            else:
                textStr = initialText
                self._textin = hexStrToByteArray(initialText)

            self.findParam('inittext').setValue(textStr)
            
    def initPair(self):
        self.setInitialKey(self.findParam('initkey').value())
        self.setInitialText(self.findParam('inittext').value())
   
    def newPair(self):
        if self._fixedKey is False:
            self._key = bytearray(self.keyLen())
            for i in range(0, self.keyLen()):
                self._key[i] = random.randint(0, 255)

        if self._fixedPlain is False:
            self._textin = bytearray(16)
            for i in range(0, 16):
                self._textin[i] = random.randint(0, 255)

        # Check key works with target
        self.validateKey()

        return (self._key, self._textin)

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None

class AcqKeyTextPattern_CRITTest(AcqKeyTextPattern_Base):

    def setupParams(self):

        self._fixedPlain = False
        self._fixedKey = True

        basicParams = [
                      # {'name':'Key', 'type':'list', 'values':['Random', 'Fixed'], 'value':'Fixed', 'set':self.setKeyType},
                  ]
        return basicParams

    def _initPattern(self):
        pass

    def initPair(self):
        if self.keyLen() == 16:
            self._key = hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0")
        elif self.keyLen() == 24:
            self._key = hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0 23 45 67 89 ab cd ef 01")
        elif self.keyLen() == 32:
            self._key = hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0 23 45 67 89 ab cd ef 01 34 56 78 9a bc de f0 12")
        else:
            raise ValueError("Invalid key length: %d bytes" % self.keyLen())
        
        self._textin1 = hexStrToByteArray("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")
        
        if self.keyLen() == 16:
            self._textin2 = hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90")
        elif self.keyLen() == 24:
            self._textin2 = hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 88")
        elif self.keyLen() == 32:
            self._textin2 = hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 95")
        else:
            raise ValueError("Invalid key length: %d bytes" % self.keyLen())

        self.group1 = True

    def newPair(self):

        if self.group1:
            self.group1 = False
            self._textin = self._textin1

            if AES:
                cipher = AES.new(str(self._key), AES.MODE_ECB)
                self._textin1 = bytearray(cipher.encrypt(str(self._textin1)))
            else:
                print "No AES Module, Using rand() instead!"
                self._textin1 = bytearray(16)
                for i in range(0, 16):
                    self._textin1[i] = random.randint(0, 255)

        else:
            self.group1 = True
            self._textin = self._textin2

        # Check key works with target
        self.validateKey()

        return (self._key, self._textin)
