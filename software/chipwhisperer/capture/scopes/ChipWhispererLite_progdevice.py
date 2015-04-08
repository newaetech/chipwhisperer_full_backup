# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, NewAE Technology Inc
# All rights reserved.
#
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

# NOTE: This file is currently manually maintained. Eventually it will be automatically created
#      from avrdude.conf, but I'd like to test with a few more devices before doing that.

XMEGAMEM_TYPE_APP = 1
XMEGAMEM_TYPE_BOOT = 2
XMEGAMEM_TYPE_EEPROM = 3
XMEGAMEM_TYPE_FUSE = 4
XMEGAMEM_TYPE_LOCKBITS = 5
XMEGAMEM_TYPE_USERSIG = 6
XMEGAMEM_TYPE_FACTORY_CALIBRATION = 7

class XMEGA128A4U(object):
    signature = [0x1e, 0x97, 0x46]
    name = "XMEGA128A4U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

class XMEGA128D4U(object):
    signature = [0x1e, 0x97, 0x47]
    name = "XMEGA128D4U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }


supported_xmega = [XMEGA128A4U(), XMEGA128D4U()]

class AVRBase(object):
    name = "INVALID DEVICE"
    signature = [0xFF, 0xFF, 0xFF]
    timeout = 200
    stabdelay = 100
    cmdexedelay = 25
    synchloops = 32
    bytedelay = 0
    pollindex = 3
    pollvalue = 0x53
    predelay = 1
    postdelay = 1
    pollmethod = 1

    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }

class ATMega328P(AVRBase):
    name = "ATMega328P"
    signature = [0x1e, 0x95, 0x0f]
    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }

class ATMega328(AVRBase):
    name = "ATMega328"
    signature = [0x1e, 0x95, 0x14]
    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }


class ATMega168PA(AVRBase):
    name = "ATMega168PA"
    signature = [0x1e, 0x94, 0x0B]
    memtypes = {
       "flash":{"offset":0, "size":16384, "pagesize":128},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega168A(AVRBase):
    name = "ATMega168A"
    signature = [0x1e, 0x94, 0x06]
    memtypes = {
       "flash":{"offset":0, "size":16384, "pagesize":128},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega88PA(AVRBase):
    name = "ATMega88PA"
    signature = [0x1e, 0x93, 0x0F]
    memtypes = {
       "flash":{"offset":0, "size":8192, "pagesize":64},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega88A(AVRBase):
    name = "ATMega88A"
    signature = [0x1e, 0x93, 0x0A]
    memtypes = {
       "flash":{"offset":0, "size":8192, "pagesize":64},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega48PA(AVRBase):
    signature = [0x1e, 0x92, 0x0A]
    name = "ATMega48PA"
    memtypes = {
       "flash":{"offset":0, "size":4096, "pagesize":64},
       "eeprom":{"offset":0, "size":256, "pagesize":4}
     }

class ATMega48A(AVRBase):
    signature = [0x1e, 0x92, 0x05]
    name = "ATMega48A"
    memtypes = {
       "flash":{"offset":0, "size":4096, "pagesize":64},
       "eeprom":{"offset":0, "size":256, "pagesize":4}
     }

supported_avr = [ATMega328P(), ATMega328(), ATMega168A(), ATMega168PA(), ATMega88A(), ATMega88PA(), ATMega48A(), ATMega48PA()]
