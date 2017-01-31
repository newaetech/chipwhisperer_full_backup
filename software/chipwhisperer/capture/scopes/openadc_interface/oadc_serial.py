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
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam

import serial
import chipwhisperer.common.utils.serialport as scan


class OpenADCInterface_Serial(Parameterized, Plugin):
    _name = "Serial Port (LX9)"

    def __init__(self, oadcInstance):
        self.portName = ''
        self.ser = None

        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            {'name':'Refresh List', 'type':'action', 'action':self.serialRefresh},
            {'name':'Selected Port', 'type':'list', 'values':[''], 'get':self.getPortName, 'set':self.setPortName},
        ])
        self.scope = oadcInstance

    def getPortName(self):
        return self.portName

    @setupSetParam("Selected Port")
    def setPortName(self, snum):
        self.portName = snum

    def con(self):
        if self.ser is None:
            self.ser = serial.Serial()
            self.ser.port = self.portName
            self.ser.baudrate = 512000
            self.ser.timeout = 2     # 2 second timeout

            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException as e:
                    attempts -= 1
                    self.ser = None
                    if attempts == 0:
                        raise IOError("Could not open %s" % self.ser.name)

        try:
            self.scope.con(self.ser)
            logging.info('OpenADC Found, Connecting')
        except IOError as e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error (Serial Port): %s" % (str(exctype) + str(value)))

    def dis(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None

    def __del__(self):
        if self.ser is not None:
            self.ser.close()

    def serialRefresh(self, _=None):
        serialnames = scan.scan()
        if serialnames is None or len(serialnames) == 0:
            serialnames = [" "]

        p = self.params.getChild('Selected Port')
        p.setLimits(serialnames)
        p.setValue(serialnames[0])
