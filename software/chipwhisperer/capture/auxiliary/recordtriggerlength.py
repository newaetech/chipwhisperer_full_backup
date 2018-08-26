#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at colinoflynn.com - this file is part of the chipwhisperer
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
import copy
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from ._base import AuxiliaryTemplate

class RecordTriggerLength(object):
    attrDictTriggerLength = {
        "sectionName":"Trigger Length",
        "moduleName":"TriggerLength",
        "module":None,
        "values":{
            "filename": {"value": None, "desc": "Trigger Length File", "changed": False, "headerLabel": "Trigger Length Data"},
        },
    }

    def __init__(self):
        self.trig_list = []

    def traceDone(self, scope, target, project):
        trig_length = scope.adc.trig_count
        self.trig_list.append(trig_length)

        print(trig_length)

    def captureComplete(self, scope, target, project):
        updatedDict = project.addAuxDataConfig(copy.deepcopy(self.attrDictTriggerLength))
        project.saveAuxData(self.trig_list, updatedDict)
        self.trig_list = []
        