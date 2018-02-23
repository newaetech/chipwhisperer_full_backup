#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Colin O'Flynn <coflynn@newae.com>
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

import time
from subprocess import call
from _base import AuxiliaryTemplate


class ResetAVR(object):
    def __init__(self, path):
        self._stk_path = path
        self._part = "atmega328p"
        
    def reset(scope, target, project):
        ret = call([self._stk_path, "-d%s" % self._part, "-s", "-cUSB"])

        if int(ret) != 0:
            raise IOError("Error Calling Stk500.exe")

        time.sleep(1)
