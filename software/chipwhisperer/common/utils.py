#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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


def hexstrtolist(data):
        data = str(data)

        newdata = data.lower()
        newdata = newdata.replace("0x", "")
        newdata = newdata.replace(",", "")
        newdata = newdata.replace(" ", "")
        newdata = newdata.replace("[", "")
        newdata = newdata.replace("]", "")
        newdata = newdata.replace("(", "")
        newdata = newdata.replace(")", "")
        newdata = newdata.replace("{", "")
        newdata = newdata.replace("}", "")
        newdata = newdata.replace(":", "")
        newdata = newdata.replace("-", "")

        datalist = [int(newdata[i:(i + 2)], 16) for i in range(0, len(newdata), 2)]

        return datalist

