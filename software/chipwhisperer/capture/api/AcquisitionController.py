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

import time
from chipwhisperer.common.utils import util


class AcquisitionController():

    def __init__(self, scope, target=None, writer=None, auxList=None, keyTextPattern=None):
        self.sigTraceDone = util.Signal()
        self.sigCaptureDone = util.Signal()
        self.sigNewTextResponse = util.Signal()

        self.target = target
        self.scope = scope
        self.writer = writer
        self.auxList = auxList
        self.running = False
        self.setKeyTextPattern(keyTextPattern)
        keyTextPattern.setTarget(target)

        self.maxtraces = 1

        if self.auxList is not None:
            for aux in auxList:
                if aux:
                    aux.captureInit()

    def setKeyTextPattern(self, pat):
        self._keyTextPattern = pat
        if pat:
            self._keyTextPattern.initPair()

    def targetDoTrace(self, plaintext, key=None):
        if self.target is None or self.target.getName()== "None":
            return []

        if key:
            self.target.loadEncryptionKey(key)
        self.target.loadInput(plaintext)
        self.target.go()

        timeout = 50
        while self.target.isDone() == False and timeout:
            timeout -= 1
            time.sleep(0.01)
            
        if timeout == 0:
            print "WARNING: Target timeout"

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

        self.sigNewTextResponse.emit(self.key, plaintext, resp, self.target.getExpected())
        return resp

    def doSingleReading(self, update=True, N=None):
        # Set mode
        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        if self.scope:
            self.scope.arm()

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArmPost()

        if self.target:
            # Get key / plaintext now
            data = self._keyTextPattern.newPair()
            self.key = data[0]
            self.textin = data[1]

            self.target.reinit()
            self.target.setModeEncrypt()
            self.target.loadEncryptionKey(self.key)
            # Load input, start encryption, get output. Key was set already, don't resend
            self.textout = self.targetDoTrace(self.textin, key=None)
        else:
            self.textout = [0]

        # Get ADC reading
        if self.scope:
            try:
                if self.scope.capture(update, N) == True:
                    print "Timeout"
                    return False
            except IOError as e:
                print "IOError: %s" % str(e)
                return False

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceDone()

        return True

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def abortCapture(self, doAbort=True):
        if doAbort:
            self.running = False

    def doReadings(self, tracesDestination=None):
        self.running = True

        self._keyTextPattern.initPair()
        data = self._keyTextPattern.newPair()
        self.key = data[0]
        self.textin = data[1]

        if self.writer:
            self.writer.prepareDisk()
            self.writer.setKnownKey(self.key)

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        self.currentTrace = 0
        while (self.currentTrace < self.maxtraces) and self.running:
            if self.doSingleReading(True, None) == True:
                try:
                    if self.writer:
                        self.writer.addTrace(self.scope.datapoints, self.textin, self.textout, self.key)
                except ValueError, e:
                    print "WARNING: Exception caught in adding trace %d, trace skipped."%self.currentTrace
                    print "         Exception info: %s"%str(e)
                self.sigTraceDone.emit()
                self.currentTrace = self.currentTrace + 1

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.captureComplete()

        if self.writer:
            # Don't clear trace as we re-use the buffer
            self.writer.closeAll(clearTrace=False)
            if tracesDestination:
                tracesDestination.appendTraceSet(self.writer)

        self.sigCaptureDone.emit(self.running)
        self.running = False
