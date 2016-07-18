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
from visa import *
from chipwhisperer.common.utils.parameter import Parameterized, Parameter
from chipwhisperer.common.utils import util


class VisaScope(Parameterized):
    _name= 'Scope Settings'

    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2 uS":2E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "250 mV":250E-3, "100 mV":100E-3, "50 mV":50E-3}

    header = ":SYSTem:HEADer OFF\n"

    def __init__(self):
        self.visaInst = None
        self.dataUpdated = util.Signal()

        self.getParams().addChildren([
            {'name':'X-Scale', 'key':'xscale', 'type':'list', 'values':self.xScales},
            {'name':'Y-Scale', 'key':'yscale', 'type':'list', 'values':self.yScales},
            {'name':'Y-Offset', 'key':'yoffset', 'type':'float', 'step':1E-3, 'siPrefix': True, 'suffix': 'V'},
            {'name':'X-Offset', 'key':'xoffset', 'type':'float', 'step':1E-6, 'siPrefix': True, 'suffix': 'S'},
            {'name':'Download Offset', 'key':'xdisporigin', 'type':'int',  'limits':(0,1E9)},
            {'name':'Download Size', 'key':'xdisprange', 'type':'int', 'limits':(0,1E9)},
        ])

    def con(self, constr):
        self.visaInst = instrument(constr)
        self.visaInst.write("*RST")
        logging.info(self.visaInst.ask("*IDN?"))
        for cmd in self.header:
            self.visaInst.write(cmd)
            logging.info('VISA: %s' % cmd)
            time.sleep(0.1)
        self.updateCurrentSettings()

    def updateCurrentSettings(self):
        self.currentSettings()

        self.findParam('xscale').setValue(self.XScale)
        self.findParam('yscale').setValue(self.YScale)
        self.findParam('xoffset').setValue(self.XOffset)
        self.findParam('yoffset').setValue(self.YOffset)

    def currentSettings(self):
        """You MUST implement this"""
        pass

    def arm(self):
        """Example arm implementation works on most"""
        self.visaInst.write(":DIGitize")

    def capture(self):
        """You MUST implement this"""
        self.dataUpdated.emit(self.datapoints, 0)
