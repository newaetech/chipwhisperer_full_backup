#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
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
__author__ = "Colin O'Flynn"

import TraceContainerNative
try:
    import TraceContainerMySQL
except ImportError:
    TraceContainerMySQL = None

import TraceContainerDPAv3

TraceContainerFormatList = {"native":TraceContainerNative.TraceContainerNative, "dpav3":TraceContainerDPAv3.TraceContainerDPAv3 }
if TraceContainerMySQL is not None:
    TraceContainerFormatList["mysql"] = TraceContainerMySQL.TraceContainerMySQL