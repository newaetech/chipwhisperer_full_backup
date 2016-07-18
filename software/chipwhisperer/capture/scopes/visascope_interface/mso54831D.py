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


class VisaScopeInterface_MSO54831D(VisaScope):
    _name = "Agilent MSO 54831D"

    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2uS":2E-6, "1 uS":1E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "200 mV":200E-3, "100 mV":100E-3,
               "50 mV":50E-3, "20 mV":20E-3, "10 mV":10E-3, "5 mV":5E-3}

    header = [":SYSTem:HEADer OFF",
                ":CHANnel1:INPut DCFifty",
                ":CHANnel1:DISPlay ON",
                ":CHANnel2:INPut DC"
                ":CHANnel2:DISPLay ON",
                ":CHANnel2:SCALe 1.0",
                ":TRIGger:MODE EDGE",
                ":TRIGger:EDGE:SOURce CHANnel2",
                ":TRIGger:EDGE:SLOPe NEGative",
                ":TRIGger:LEVel CHANnel2,2.0",
                ":TRIGger:SWEep TRIGgered",
                ":WAVeform:FORMat WORD",
                ":WAVeform:BYTeorder LSBFirst",
                ":WAVeform:SOURce CHANnel1",
                ":ACQuire:COMPlete 100"
                ]

    def currentSettings(self):
        self.visaInst.write(":TRIG:SWE AUTO")
        self.visaInst.write(":RUN")
        time.sleep(0.5)
        test = self.visaInst.ask_for_values(":WAVeform:PREamble?")
        self.visaInst.write(":TRIG:SWE TRIG")

        if test[4] != 0.0:
            self.XDisplayedOrigin = (test[12] - test[5]) / test[4]
            srange = test[11] / test[4]
        else:
            self.XDisplayedOrigin = 0.0
            srange = 0.0

        spoints = test[2]

        if srange < spoints:
            self.XDisplayedRange = srange
        else:
            self.XDisplayedRange = spoints

        self.XScale = self.visaInst.ask_for_values(":TIMebase:SCALe?")
        self.XScale = self.XScale[0]
        self.XOffset = self.visaInst.ask_for_values(":TIMebase:POSition?")
        self.XOffset = self.XOffset[0]
        self.YOffset = self.visaInst.ask_for_values(":CHANnel1:OFFSet?")
        self.YOffset = self.YOffset[0]
        self.YScale = self.visaInst.ask_for_values(":CHANnel1:SCALe?")
        self.YScale = self.YScale[0]

    def arm(self):
        self.visaInst.write(":DIGitize")

    def capture(self):
        xdstart = self.findParam("xdisporigin").getValue()
        xdrange = self.findParam("xdisprange").getValue()

        command = ":WAVeform:DATA?"

        if (xdstart != 0) or (xdrange != 0):
            command += " %d" % xdstart

            if xdrange != 0:
                command += ",%d" % xdrange

        #print command
        self.visaInst.write(command)
        data = self.visaInst.read_raw()

        #Find '#' which is start of frame
        start = data.find('#')

        if start < 0:
            raise IOError('Error in header')

        start += 1
        hdrlen = data[start]
        hdrlen = int(hdrlen)

        #print hdrlen

        start += 1
        datalen = data[start:(start+hdrlen)]
        datalen = int(datalen)
        #print datalen

        start += hdrlen

        #Each is two bytes
        wavdata = bytearray(data[start:(start + datalen)])

        self.datapoints = []

        for j in range(0, datalen, 2):
            data = wavdata[j] | (wavdata[j+1] << 8)

            if (data & 0x8000):
                data = -65536 + data

            self.datapoints.append(data)

        self.dataUpdated.emit(self.datapoints, 0)
        return False
