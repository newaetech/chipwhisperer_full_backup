#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
#
# ChipWhisperer is a trademark of NewAE Technology Inc.
#===========================================================

import sys
from ._base import AttackBaseClass


class Profiling(AttackBaseClass):
    """Profiling Power Analysis Attack"""
    _name = "Profiling"

    def __init__(self):
        self._algos = None
        self._analysisAlgorithm = self._algos["Template Attack"]
        AttackBaseClass.__init__(self)
        self.useAbs = False # Do not use absolute

    def updateScript(self, _=None):
        AttackBaseClass.updateScript(self)

        analysAlgoStr = sys.modules[self._analysisAlgorithm.__class__.__module__].__name__ + '.' + self._analysisAlgorithm.__class__.__name__
        model_path = sys.modules[self.findParam('Crypto Algorithm').getValue().__class__.__module__].__name__
        cryptoalg = model_path + '.' + self.findParam('Crypto Algorithm').getValue().__class__.__name__
        hwmodel = model_path + '.' + self.findParam('Crypto Algorithm').getValue().getHwModel().__name__
        self.addVariable("init", "leakage_object", "%s(%s)" % (cryptoalg, hwmodel))
