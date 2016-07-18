# Authors: Colin O'Flynn
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
from _base import VisaScope
from chipwhisperer.common.utils import util


class VisaScopeInterface_DSO1024A(VisaScope):
    _name = "Agilent DSO 1024A"

    # TODO: What scales & ranges are allowed on the DSO1024A?
    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2uS":2E-6, "1 uS":1E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "200 mV":200E-3, "100 mV":100E-3,
               "50 mV":50E-3, "20 mV":20E-3, "10 mV":10E-3, "5 mV":5E-3}

    header = [  ":CHANnel1:PROBe 1X",
                ":CHANnel1:DISPlay ON",
                ":CHANnel1:COUPling DC",
                ":CHANnel2:PROBe 10X",
                ":CHANnel2:SCALe 1",
                ":CHANnel2:OFFSet 0",
                ":CHANnel2:DISPLay ON",
                ":TRIGger:MODE EDGE",
                ":TRIGger:EDGE:SOURce CHANnel2",
                ":TRIGger:EDGE:SLOPe NEGative",
                ":TRIGger:EDGE:LEVel 2.0",
                ":TRIGger:EDGE:SWEep NORMal",
                ":WAVeform:SOURce CHANnel1",
                ":WAVeform:FORMat WORD",
                ]

    def currentSettings(self):
        # TODO: Delete these?
        self.XScale = self.visaInst.ask_for_values(":TIMebase:SCALe?")
        self.XScale = self.XScale[0]
        self.XOffset = self.visaInst.ask_for_values(":TIMebase:POSition?")
        self.XOffset = self.XOffset[0]
        self.YOffset = self.visaInst.ask_for_values(":CHANnel1:OFFSet?")
        self.YOffset = self.YOffset[0]
        self.YScale = self.visaInst.ask_for_values(":CHANnel1:SCALe?")
        self.YScale = self.YScale[0]

    def arm(self):
        self.visaInst.write(":RUN\n")
        result = "fake"
        while (result.startswith("WAIT") == False) and (result.startswith("RUN") == False):
            result = self.visaInst.ask(":TRIGger:STATus?")
            time.sleep(0.1)
            logging.info('1Waiting...')

    def capture(self):
        # Wait?
        while self.visaInst.ask("*OPC?\n") != "1":
            time.sleep(0.1)
            util.updateUI()
            logging.info('2Waiting...')

        # print command
        self.visaInst.write(":WAVeform:DATA?")
        data = self.visaInst.read_raw()

        # Find '#' which is start of frame
        start = data.find('#')

        if start < 0:
            raise IOError('Error in header')

        start += 1
        hdrlen = data[start]
        hdrlen = int(hdrlen)

        # print hdrlen

        start += 1
        datalen = data[start:(start + hdrlen)]
        datalen = int(datalen)
        # print datalen

        start += hdrlen

        # Each is two bytes
        wavdata = bytearray(data[start:(start + datalen)])

        self.datapoints = []

        for j in range(0, datalen, 2):
            data = wavdata[j] | (wavdata[j + 1] << 8)

            if (data & 0x8000):
                data += -65536

            self.datapoints.append(data)

        self.dataUpdated.emit(0, self.datapoints, 0, 0)
        return False
