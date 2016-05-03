#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
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

import _OpenADCInterface as openadc
from chipwhisperer.common.utils.parameters import Parameterized
from chipwhisperer.common.utils import util, timer


class OpenADCQt(Parameterized):
    name='OpenADC'

    def __init__(self, parentParam=None):
        Parameterized.__init__(self, parentParam)

        self.dataUpdated = util.Signal()
        self.adc_settings = openadc.OpenADCSettings()
        self.params.addChildren(self.adc_settings.parameters(doUpdate=False))

        self.offset = 0.5
        self.ser = None
        self.sc = None

        self.datapoints = []

        self.timerStatusRefresh = timer.Timer()
        self.timerStatusRefresh.timeout.connect(self.statusRefresh)
        self.adc_settings.setFindParam(self.findParam) #todo: this is a somewhat insane way to cut through the layers

    def setEnabled(self, enabled):
        pass

    def statusRefresh(self):
        pass

    def reloadParameterTree(self):
        self.adc_settings.setInterface(self.sc)
        self.params.blockTreeChangeSignal()
        self.params.getAllParameters()
        self.params.unblockTreeChangeSignal()

    def processData(self, data):
        fpData = []

        #lastpt = -100;

        if data[0] != 0xAC:
            print "Unexpected sync byte: 0x%x" % data[0]
            return None

        for i in range(2, len(data)-1, 2):
            if (0x80 & data[i + 1]) or ((0x80 & data[i + 0]) == 0):
                print "Error at byte %d" % i
                print("Bytes: %x %x"%(data[i], data[i+1]))
                return None

            #Convert
            intpt = data[i + 1] | ((data[i+0] & 0x07) << 7)

            #input validation test: uncomment following and use
            #ramp input on FPGA
            ##if (intpt != lastpt + 1) and (lastpt != 0x3ff):
            ##    print "intpt: %x lstpt %x\n"%(intpt, lastpt)
            ##lastpt = intpt;

            fpData.append(float(intpt) / 1024.0 - self.offset)

        return fpData

    def arm(self):
        self.sc.arm()

    def read(self, update=True, NumberPoints=None):
        if NumberPoints == None:
            NumberPoints = self.adc_settings.parm_trigger.maxSamples()

        try:
            self.datapoints = self.sc.readData(NumberPoints)
        except IndexError, e:
            raise IOError("Error reading data: %s"%str(e))

        self.dataUpdated.emit(self.datapoints, -self.adc_settings.parm_trigger.presamples(True))

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        timeout = self.sc.capture(waitingCallback=waitingCallback)
        self.read(update, NumberPoints)
        return timeout

    def reset(self):
        self.sc.setReset(True)
        self.reloadParameterTree()

    def test(self):
        self.sc.testAndTime()

    def con(self, ser):
        self.ser = ser
        #See if device seems to be attached
        self.sc = openadc.OpenADCInterface(self.ser)

        deviceFound = False
        numTries = 0

        #Try a few times
        while(deviceFound == False):

            if self.sc.devicePresent():
                deviceFound = True
                break

            numTries += 1

            if (numTries == 5):
                try:
                    portname = self.ser.name
                except:
                    portname = "UNKNOWN"
                self.ser.close()
                self.ser = None

                raise IOError("Opened port %s but failed to find OpenADC" % portname)

        self.reloadParameterTree()
        self.setEnabled(True)

    def close(self):
        if self.ser:
            self.ser.close()
            self.ser = None

    def __del__(self):
        self.close()
