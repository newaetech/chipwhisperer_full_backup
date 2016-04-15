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

import sys
from chipwhisperer.common.utils import plugin
import chipwhisperer.capture.ui.qt as openadc_qt
try:
    import serial
    import chipwhisperer.common.utils.Scan as scan
except ImportError:
    serial = None

class OpenADCInterface_Serial(plugin.PluginTemplate):
    name = "Serial Port (LX9)"

    def __init__(self, oadcInstance):
        self.ser = None

        if (openadc_qt is None) or (serial is None):
            raise ImportError("Needed imports for serial missing")
        else:
            self.scope = oadcInstance

    def setupParameters(self):
        return [
                  {'name':'Refresh List', 'type':'action', 'action':self.serialRefresh},
                  {'name':'Port', 'type':'list', 'values':[''], 'value':None, 'set':self.setPortName},
                ]

    def setPortName(self, snum):
        self.portName = snum

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            self.ser = serial.Serial()
            self.ser.port     = self.portName
            self.ser.baudrate = 512000
            self.ser.timeout  = 2     # 2 second timeout

            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException, e:
                    attempts = attempts - 1
                    self.ser = None
                    if attempts == 0:
                        raise IOError("Could not open %s"%self.ser.name)

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error (Serial Port): %s"%(str(exctype) + str(value)))

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def serialRefresh(self):
        serialnames = scan.scan()
        if serialnames == None:
            serialnames = [" "]

        for p in self.params.children():
            if p.name() == 'Port':
                p.setLimits(serialnames)
                p.setValue(serialnames[0])

        self.paramListUpdated.emit()

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"
