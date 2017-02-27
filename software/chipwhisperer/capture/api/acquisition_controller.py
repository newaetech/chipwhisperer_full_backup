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
import logging
import time
from chipwhisperer.common.utils import util


class AcquisitionController:

    def __init__(self, scope, target=None, writer=None, auxList=None, keyTextPattern=None):
        self.sigTraceDone = util.Signal()
        self.sigNewTextResponse = util.Signal()

        self.currentTrace = 0
        self.maxtraces = 1

        self.key = [0]
        self.textin = [0]
        self.textout = [0]

        self.target = target
        self.scope = scope
        self.writer = writer
        self.auxList = auxList
        self.keyTextPattern = keyTextPattern
        self.keyTextPattern.setTarget(target)

        if self.auxList is not None:
            for aux in auxList:
                if aux:
                    aux.captureInit()

    def targetDoTrace(self):
        if self.target is None or self.target.getName() == "None":
            return []

        self.target.go()
        timeout = 50
        while self.target.isDone() is False and timeout:
            timeout -= 1
            time.sleep(0.01)

        if timeout == 0:
            logging.warning('Target timeout')

        self.textout = self.target.readOutput()
        logging.debug("PlainText: " + ''.join(format(x, '02x') for x in self.textin))
        logging.debug("CipherText: " + ''.join(format(x, '02x') for x in self.textout))

    def doSingleReading(self):
        # Set mode
        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        if self.target:
            self.target.reinit()

        if self.scope:
            self.scope.arm()

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArmPost()

        if self.target:
            # Get key / plaintext now
            self.key, self.textin = self.keyTextPattern.newPair()

            self.target.setModeEncrypt()
            self.target.loadEncryptionKey(self.key)
            self.target.loadInput(self.textin)
            # Load input, start encryption, get output
            self.targetDoTrace()
            self.sigNewTextResponse.emit(self.key, self.textin, self.textout, self.target.getExpected())

        # Get ADC reading
        if self.scope:
            try:
                ret = self.scope.capture()
                if ret:
                    logging.debug('Timeout happened during acquisition.')
                return not ret
            except IOError as e:
                logging.error('IOError: %s' % str(e))
                return False

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceDone()

        return True

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self, channelNumbers=None, tracesDestination=None, progressBar=None):
        if channelNumbers is None:
            channelNumbers = [0]

        self.keyTextPattern.initPair()

        if self.writer:
            self.writer.prepareDisk()

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        if self.target:
            self.target.init()

        self.currentTrace = 0
        while self.currentTrace < self.maxtraces:
            if self.doSingleReading():
                try:
                    if self.writer:
                        self.writer.setKnownKey(self.key)
                        for channelNum in channelNumbers:
                            self.writer.addTrace(self.scope.channels[channelNum].getTrace(), self.textin, self.textout,
                                                 self.key, channelNum=channelNum)
                except ValueError as e:
                    logging.warning('Exception caught in adding trace %d, trace skipped.' % self.currentTrace)
                    logging.debug(str(e))
                self.sigTraceDone.emit()
                self.currentTrace += 1
            else:
                util.updateUI()  # Check if it was aborted

            if progressBar is not None:
                if progressBar.wasAborted():
                    break

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.captureComplete()

        if self.writer and self.writer.numTraces() > 0:
            # Don't clear trace as we re-use the buffer
            self.writer.config.setAttr("scopeSampleRate", self.scope.channels[channelNumbers[0]].getSampleRate())
            self.writer.closeAll(clearTrace=False)
            if tracesDestination:
                tracesDestination.appendSegment(self.writer)
