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
import logging

from . import _OpenADCInterface as openadc
from chipwhisperer.common.utils import util


class OpenADCQt(object):
    _name= 'OpenADC'

    def __init__(self):
        self.dataUpdated = util.Signal()


        self.offset = 0.5
        self.ser = None
        self.sc = None
        self.parm_hwinfo = None
        self.parm_gain = None
        self.parm_trigger = None
        self.parm_clock = None
        self.parm_ads4128 = None

        self.datapoints = []


    def setEnabled(self, enabled):
        pass

    def statusRefresh(self):
        pass

    def processData(self, data):
        fpData = []

        if data[0] != 0xAC:
            logging.warning("Unexpected sync byte: 0x%x" % data[0])
            return None

        for i in range(2, len(data)-1, 2):
            if (0x80 & data[i + 1]) or ((0x80 & data[i + 0]) == 0):
                logging.error('Error at byte ' + str(i) + '. Bytes: %x %x' % (data[i], data[i+1]))
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

    def startCaptureThread(self):
        self.sc.startCaptureThread()

    def read(self, numberPoints=None, channelNr=0):
        if numberPoints == None:
            numberPoints = self.parm_trigger.samples
        
        logging.debug("Expecting {} points".format(numberPoints))

        try:
            self.datapoints = self.sc.readData(numberPoints)

            # this stuff takes longer than you'd expect
            logging.debug("Read {} datapoints".format(len(self.datapoints)))
            if (self.datapoints is None) or (len(self.datapoints) != numberPoints):
                logging.error("Received fewer points than expected! {} vs {}".format(len(self.datapoints), numberPoints))
                return True #effectively a timeout for now
        except IndexError as e:
            raise IOError("Error reading data: %s" % str(e))

        return False


    def trigger_duration(self):
        return self.parm_trigger.duration()

    def capture(self, offset=None, adc_freq=29.53E6, samples=24400):
        timeout = self.sc.capture(offset, adc_freq, samples)
        timeout2 = self.read()

        return timeout or timeout2

    def reset(self):
        self.sc.setReset(True)

    def test(self):
        self.sc.testAndTime()

    def con(self, ser):
        self.ser = ser
        # See if device seems to be attached
        self.sc = openadc.OpenADCInterface(self.ser)

        self.parm_hwinfo = openadc.HWInformation(self.sc)

        self.parm_gain = openadc.GainSettings(self.sc)

        self.parm_trigger = openadc.TriggerSettings(self.sc)

        self.parm_clock = openadc.ClockSettings(self.sc, hwinfo=self.parm_hwinfo)

        # TODO: what if not Husky? Maybe this is harmless?
        self.parm_ads4128 = openadc.ADS4128Settings(self.sc)

        deviceFound = False
        numTries = 0

        # Try a few times
        while(deviceFound == False):
            if self.sc.devicePresent():
                deviceFound = True
                break

            numTries += 1
            if numTries == 5:
                try:
                    portname = self.ser.name
                except:
                    portname = "UNKNOWN"
                self.close()

                raise IOError("Opened port %s but failed to find OpenADC" % portname)

        self.setEnabled(True)

    def close(self):
        self.ser = None
        self.parm_hwinfo = None

        self.parm_gain = None

        self.parm_trigger = None

        self.parm_clock = None
        self.sc = None

        self.parm_ads4128 = None

    def __del__(self):
        self.close()
