#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2017, NewAE Technology Inc
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

from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.parameter import Parameterized

class SPIFlashTemplate(Parameterized):


    _name= 'SPI Flash Programmer'

    def __init__(self):
        self.connectStatus = util.Observable(False)
        self.getParams()

    def selectionChanged(self):
        pass

    def close(self):
        pass

    def con(self, scope=None):
        """Connect to target"""
        self.connectStatus.setValue(True)

    def dis(self):
        """Disconnect from target"""
        self.close()
        self.connectStatus.setValue(False)

    def spi_command(self, command_in, num_data_to_read=0):
        """Do a SPI command & return just the result"""
        padded_data_out = command_in + [0]*num_data_to_read
        response = self.hw_spi_write(padded_data_out)
        return list(response[len(command_in):])

    def hw_spi_write(self, data_to_send):
        """
        TX a certain number of bytes (in data_to_send) & also record state of input pin.

        Returns:
            list: Received data bytes over SPI
        """
        raise NotImplementedError

