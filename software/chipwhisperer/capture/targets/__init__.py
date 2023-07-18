#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2021, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

"""
Package containing all of the target types that the ChipWhisperer API can connect to:

Targets:
* SimpleSerial - Communication via CW Uart Pins
* SimpleSerial2 - Communication via CW Uart Pins (updated protocol)
* CW305 - USB communication to CW305 (also CW305_ECC, CW305_AES_PIPELINED for extensions for those particular targets)
* CW310 - USB communication to CW310
* CW340 - USB communication to CW340
* SAKURAG - Requires FTDI module, old, untested
* SASEBOGII - Requires FTDI module, old, untested
* SmartCard - Currently unavailable (tied to GUI)
"""
from .SimpleSerial import SimpleSerial
from .CW305 import CW305
from .SimpleSerial2 import SimpleSerial2, SimpleSerial2_CDC
from .CW305_ECC import CW305_ECC
from .CW305_AES_PIPELINED import CW305_AES_PIPELINED
from .CW310 import CW310
from .CW340 import CW340
from typing import Union

# try:
#     from .sakura_g import SakuraG #needs ftdi module
# except ImportError:
#     pass

# from .SmartCard import SmartCard #Tied to GUI
SimpleSerialTypes = Union[SimpleSerial, SimpleSerial2, SimpleSerial2_CDC]
FPGATypes = Union[CW305, CW305_ECC, CW310, CW340]
TargetTypes = Union[FPGATypes, SimpleSerialTypes]
