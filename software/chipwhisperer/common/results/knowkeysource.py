#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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

from ._base import ResultsBase
from chipwhisperer.common.utils import util
from chipwhisperer.analyzer.attacks._base import AttackObserver


class KnowKeySource(ResultsBase, AttackObserver):
    _name = "Knownkey Source"
    _description = "Modifies the knownkey to be highlighted in other AnalysisObservers."

    def __init__(self, parentParam=None, name=None):
        ResultsBase.__init__(self, parentParam, name)
        AttackObserver.__init__(self)
        self._knowKey = []

        self.params.addChildren([
            {'name':'Knownkey Source', 'type':'list', 'values':{'Attack Module':'attack', 'Override':'override'},
             'value':'attack', 'set':self.setKnownKeySrc},
            {'name':'Override Key', 'type':'str', 'key':'knownkey', 'value':'', 'set':self.setKnownKey, 'readonly':True}
        ])

    def setKnownKeySrc(self, keysrc):
        """Set key as 'attack' or 'override'"""
        if keysrc == 'attack':
            self.findParam('knownkey').setReadonly(True)
            ResultsBase._highlightedKey = self._analysisSource.knownKey
        elif keysrc == 'override':
            self.findParam('knownkey').setReadonly(False)
            ResultsBase._highlightedKey = self.knowKey
        else:
            raise ValueError("Key Source Error")

    def knowKey(self):
        return self._knowKey

    def setKnownKey(self, strkey):
        """Override known key by user selection"""
        try:
            self._knowKey = util.hexstr2list(strkey)
        except ValueError:
            raise Warning("Key Selection - Could not convert '%s' to hex, key unchanged!" % strkey)
