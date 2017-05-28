#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
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
from _base import TargetTemplate
from chipwhisperer.common.utils import pluginmanager
from chipwhisperer.common.utils.parameter import setupSetParam

class SPIFlashTarget(TargetTemplate):
    _name = "SPI Flash Reprogrammer"


    supported_chips = [
        {'name':'W25Q32', 'Description':'Winbond 4Mb FLASH', 'ID':[0xEF, 0x40, 0x16], 'erasecmd':[0x20], 'erasesize':4096, 'pagesize':256, 'addrbytes':3},
                       ]

    def __init__(self):
        TargetTemplate.__init__(self)

        spi_cons = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets.spiflash_programmers", True,
                                                             True)

        self.spi = None
        for c in spi_cons:
            if c is not None:
                self.spi = c
                break

        self.input = ""
        self.textlength = 16

        chip_names = []
        for c in self.supported_chips:
            chip_names.append(c['name'])

        self._chip = self.supported_chips[0]

        self.params.addChildren([
            {'name': 'Connection', 'type': 'list', 'key': 'con', 'values': spi_cons, 'get': self.getConnection,
             'set': self.setConnection},
            {'name': 'Input Length (Bytes)', 'type': 'int', 'range': (1, 512), 'default': 16, 'get': self.textLen,
             'set': self.setTextLen},
            {'name': 'Write Address (Hex)', 'key':'addr', 'type':'str', 'value':'0x1000'},
            {'name': 'Flash Chip', 'type':'list', 'values':chip_names, 'get':self.chipname, 'set':self.setChipname},
        ])

        if self.spi:
            self.setConnection(self.spi, blockSignal=True)


    @setupSetParam("Input Length")
    def setTextLen(self, tlen):
        """ Set plaintext length. tlen given in bytes """
        self.textlength = tlen

    def textLen(self):
        """ Return plaintext length in bytes """
        return self.textlength


    def getConnection(self):
        return self.spi

    @setupSetParam("Connection")
    def setConnection(self, con):
        self.spi = con

        if self.spi:
            self.params.append(self.spi.getParams())
            self.spi.connectStatus.setValue(False)
            self.spi.connectStatus.connect(self.connectStatus.emit)
            self.spi.selectionChanged()

    def chipname(self):
        return self._chip['name']

    @setupSetParam("Flash Chip")
    def setChipname(self, chip):
        for c in self.supported_chips:
            if c['name'] == chip:
                self._chip = c
                return

        self._chip = None
        raise ValueError("Unknown chip %s"%chip)

    def _con(self, scope=None):
        self.spi.con()

    def close(self):
        if self.spi != None:
            self.spi.close()
        self.spi = None

    def loadInput(self, inputtext):
        self.input = inputtext

        #TODO: Select when 'do_program' happens
        self.do_program()

    def go(self):
        return

    def readOutput(self):
        return [0]*16

    def do_program(self):
        addr = int(self.findParam('addr').getValue(), 16)
        self.erase_cmd(addr)
        self.write_cmd(addr, list(self.input))

    def addr_bytes(self, address):
        """Return address in format suitable for sending over SPI bus (MSB=first)"""
        num_bytes = self._chip['addrbytes']

        addrdata = [0]*num_bytes

        j = num_bytes
        for i in range(0, num_bytes):
            j -= 1
            addrdata[i] = (address >> (8*j)) & 0xff

        return addrdata

    def erase_cmd(self, address, verify=False):
        self.spi_command([0x06])
        self.spi_command(self._chip['erasecmd'] + self.addr_bytes(address))
        self.wait_for_done()

        if verify:
            raise NotImplemented("Blank Check Not Implemented")

    def write_cmd(self, address, data, verify=True):
        self.spi_command([0x06])
        self.spi_command([0x02] + self.addr_bytes(address) + data)
        self.wait_for_done()

        if verify:
            readdata = self.read_cmd(address, len(data))
            if readdata == data:
                logging.debug("Verified %d bytes at %x"%(len(data), address))
            else:
                logging.debug("FLASH Wrote: %s"%str(data))
                logging.debug("FLASH Read: %s"%str(readdata))
                raise IOError("SPI Flash verify failed of %d bytes at %x"%(len(data), address))


    def read_cmd(self, address, datalen):
        data = self.spi_command([0x03] + self.addr_bytes(address), datalen)
        return data


    def wait_for_done(self):
        busy = 0x01
        timeout = 0
        while busy:
            sr = self.spi_command([0x05], 1)
            busy = sr[0] & 0x01
            time.sleep(0.01)

            if timeout > 5000:
                raise IOError("Possible timeout waiting for BUSY=0")

    def spi_command(self, *args, **kwargs):
        return self.spi.spi_command(*args, **kwargs)