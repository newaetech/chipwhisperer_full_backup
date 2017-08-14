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
    """High level class for controlling captures.

    An acquisition controller coordinates all of the smaller pieces to allow
    power traces to be recorded. The sequence in a typical capture is:
    - Key and input text are calculated using a key-text pattern
    - Scope is armed
    - Target is sent key and input text
    - Output text is recorded from target
    - Power trace is read from scope
    - Input and output data are saved into project

    Also, an acquisition controller tracks a number of auxiliary functions
    which can perform any other tasks during a capture, such as resetting
    """

    def __init__(self, scope, target=None, writer=None, auxList=None, keyTextPattern=None):
        # TODO: use project objects instead of writers?
        self.sigTraceDone = util.Signal()
        self.sigNewTextResponse = util.Signal()

        self.currentTrace = 0
        self.maxtraces = 0

        self.key = [0]
        self.textin = [0]
        self.textout = [0]

        self._target = target
        self._scope = scope
        self._writer = writer
        self.auxList = auxList
        self._pattern = keyTextPattern
        self._pattern.setTarget(target)
        self._pattern.initPair(self.maxtraces)

        if self.auxList is not None:
            for aux in auxList:
                if aux:
                    aux.captureInit()

    def targetDoTrace(self):
        if self._target is None or self._target.getName() == "None":
            return []

        self._target.go()
        timeout = 50
        while self._target.isDone() is False and timeout:
            timeout -= 1
            time.sleep(0.01)

        if timeout == 0:
            logging.warning('Target timeout')

        self.textout = self._target.readOutput()
        try:
            logging.debug("PlainText: " + ''.join(format(x, '02x') for x in self.textin))
        except:
            pass

        try:
            logging.debug("CipherText: " + ''.join(format(x, '02x') for x in self.textout))
        except:
            pass



    def doSingleReading(self):
        # Set mode
        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        if self._target:
            self._target.reinit()

        ##if self.scope:
        ##    self.scope.arm()

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArmPost()

        if self._target:
            # Get key / plaintext now
            self.key, self.textin = self._pattern.newPair()

            self._target.setModeEncrypt()
            self._target.loadEncryptionKey(self.key)
            self._target.loadInput(self.textin)

        if self._scope:
            self._scope.arm()

        if self._target:
            # Load input, start encryption, get output
            self.targetDoTrace()
            self.sigNewTextResponse.emit(self.key, self.textin, self.textout, self._target.getExpected())

        # Get ADC reading
        if self._scope:
            try:
                ret = self._scope.capture()
                if ret:
                    logging.debug('Timeout happened during acquisition.')
                capture_ok = not ret
            except IOError as e:
                logging.error('IOError: %s' % str(e))
                capture_ok = False

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceDone()

        return capture_ok

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self, channelNumbers=None, tracesDestination=None, progressBar=None):
        if channelNumbers is None:
            channelNumbers = [0]

        self._pattern.initPair(self.maxtraces)

        if self._writer:
            self._writer.prepareDisk()

        if self.auxList:
            for aux in self.auxList:
                if aux:
                    aux.traceArm()

        if self._target:
            self._target.init()

        self.currentTrace = 0
        while self.currentTrace < self.maxtraces:
            if self.doSingleReading():
                try:
                    if self._writer:
                        self._writer.setKnownKey(self.key)
                        for channelNum in channelNumbers:
                            self._writer.addTrace(self._scope.channels[channelNum].getTrace(), self.textin, self.textout,
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
                    aux.captureComplete(self._writer)

        if self._writer and self._writer.numTraces() > 0:
            # Don't clear trace as we re-use the buffer
            self._writer.config.setAttr("scopeSampleRate", self._scope.channels[channelNumbers[0]].getSampleRate())
            self._writer.closeAll(clearTrace=False)
            if tracesDestination:
                tracesDestination.appendSegment(self._writer)

    @property
    def last_key(self):
        """The last key sent to the target. Read-only.
        """
        return "todo"

    @property
    def last_textin(self):
        """The last input text sent to the target. Read-only.
        """
        return "todo"

    @property
    def last_textout(self):
        """The last output text received from the target. Read-only.
        """
        return "todo"
