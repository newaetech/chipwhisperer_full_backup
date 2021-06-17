#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, NewAE Technology Inc
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

from .IntelHex import IntelHex

def FileReader(filename):
    if filename.lower().endswith(".hex") or filename.lower().endswith(".ihex"):
        f = IntelHex(filename)
        fdata = f.tobinarray(0)
        fsize = f.maxaddr() - f.minaddr()
    elif filename.lower().endswith(".bin"):
        f = open(filename, "rb")
        fdata = bytearray(f.read())
        fsize = len(fdata)
        f.close()
    else:
        raise IOError("Unknown file extension for file %s"%filename)

    return fdata, fsize