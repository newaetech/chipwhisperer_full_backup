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
import sys
import chipwhisperer.capture.scopes._qt as openadc_qt
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
try:
    import ftd2xx as ft
except:
    ft = None


class OpenADCInterface_FTDI(Parameterized, Plugin):
    _name = "FTDI (SASEBO-W/SAKURA-G)"

    def __init__(self, oadcInstance):
        self.serialNumber = ''
        self._serialnumbers = ['']

        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            {'name':'Refresh Device List', 'type':'action', 'action':self.serialRefresh},
            {'name':'Device Serial Number', 'key':'snum', 'type':'list', 'values':[''], 'get':self.getSerialNumber, 'set':self.setSelectedDevice},
        ])
        self.ser = None

        if (openadc_qt is None) or (ft is None):
            raise ImportError("Needed imports for FTDI missing")
        else:
            self.scope = oadcInstance

    def getSerialNumber(self):
        return self.serialNumber

    @setupSetParam("Device Serial Number")
    def setSelectedDevice(self, snum):
        self.serialNumber = snum

    def con(self):
        if self.ser is None:
            try:
                self.dev = ft.openEx(str(self.serialNumber), ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
                self.dev.setBitMode(0x00, 0x40)
                self.dev.setTimeouts(500, 500)
                self.dev.setLatencyTimer(2)
                self.ser = self
            except ft.ftd2xx.DeviceError, e:
                self.ser = None
                raise IOError("Could not open %s: %s" % (self.serialNumber, e))

        try:
            self.scope.con(self.ser)
            logging.info('OpenADC Found, Connecting')
        except IOError as e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error: %s" % (str(exctype) + str(value)) + " - " + e.message)

    def dis(self):
        self.ser = None
        if hasattr(self, 'dev'):
            self.dev.close()
            del self.dev

    def __del__(self):
        self.dis()

    def serialRefresh(self, _=None):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [""]
        self.setSerialNumberLimits(serialnames)
        self.findParam('snum').setValue(serialnames[0])

    def setSerialNumberLimits(self, newitems):
        for s in newitems:
            if s not in self._serialnumbers:
                self._serialnumbers.append(s)

        self.findParam('snum').setLimits(self._serialnumbers)

    def read(self, N=0, debug=False):
        return bytearray(self.dev.read(N))

    def write(self, data, debug=False):
        return self.dev.write(data)
