#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

from chipwhisperer.analyzer.preprocessing.ResyncCrossCorrelation import ResyncCrossCorrelation
from chipwhisperer.analyzer.preprocessing.ResyncPeakDetect import ResyncPeakDetect
from chipwhisperer.analyzer.preprocessing.ResyncSAD import ResyncSAD
from chipwhisperer.analyzer.preprocessing.Filter import Filter
from chipwhisperer.analyzer.preprocessing.AddNoiseJitter import AddNoiseJitter
from chipwhisperer.analyzer.preprocessing.Normalize import Normalize

try:
    from chipwhisperer.analyzer.preprocessing.DecimationClockRecovery import DecimationClockRecovery
except ImportError:
    DecimationClockRecovery = None
# from chipwhisperer.analyzer.preprocessing.DecimationFixed import DecimationFixed

def listAll(parent):
    valid_targets = {}
    valid_targets["Resync: Cross Correlation"] = ResyncCrossCorrelation(parent)
    valid_targets["Resync: Peak Detect"] = ResyncPeakDetect(parent)
    valid_targets["Resync: Sum-of-Difference"] = ResyncSAD(parent)
    valid_targets["Digital Filter"] = Filter(parent)
    valid_targets["Normalize"] = Normalize(parent)
    valid_targets["Add Noise: Jitter"] = AddNoiseJitter(parent)
    if DecimationClockRecovery:
        valid_targets["Decimation: Clock Recovery"] = DecimationClockRecovery(parent)
    # valid_targets["Decimation: Fixed"] = DecimationFixed(parent)
    valid_targets["Disabled"] = 0

    return valid_targets
