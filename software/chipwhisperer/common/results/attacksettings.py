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

from chipwhisperer.analyzer.attacks._base import AttackObserver
from .base import ResultsBase
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import setupSetParam


class AttackSettings(ResultsBase, AttackObserver, Plugin):
    _name = "Attack Settings"
    _description = "General settings for all the attack widgets"

    def __init__(self, name=None):
        AttackObserver.__init__(self)
        self._overridedKey = [00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00]
        self.params = self.getParams()
        self.params.addChildren([
            {'name':'Highlighted key', 'type':'list', 'values':['Known key from attack', 'Override', 'Rank 0 key', 'None'],
             'value': 'Known key from attack', 'action':lambda p: self.setKnownKeySrc(p.getValue())},
            {'name':'Override with', 'type':'str', 'key':'knownkey', 'value':"00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 'action':lambda p:self.setKnownKey(p.getValue())},
            {'name':'Highlighted key color', 'type':'color', 'value':"F00", 'action':lambda p: self.setHighlightedKeyColor(p.getValue())},
            {'name':'Trace color', 'type':'color', 'value':"0F0", 'action':lambda p: self.setTraceColor(p.getValue())},
            {'name':'Redraw Widgets', 'type':'action', 'action':self.updateAll},
        ])
        self.findParam('knownkey').hide()

    def updateAll(self, _=None):
        for i in self.registeredObjects.itervalues():
            if isinstance(i, AttackObserver):
                i.processAnalysis()

    def getKnownKeySrc(self):
        return self.findParam('Highlighted key')

    def setKnownKeySrc(self, keysrc):
        """Set key as 'attack' or 'override'"""
        self.findParam('knownkey').hide()
        if keysrc == 'Known key from attack':
            AttackObserver._highlightedKeys = self._analysisSource.knownKey
        elif keysrc == 'Override':
            self.findParam('knownkey').show()
            AttackObserver._highlightedKeys = self.getOverridedKey
        elif keysrc == 'Rank 0 key':
            AttackObserver._highlightedKeys = self.getRank0Key
        elif keysrc == 'None':
            AttackObserver._highlightedKeys = self.getNoneKey
        else:
            raise ValueError("Key Source Error")

    def getOverridedKey(self):
        return self._overridedKey

    def getRank0Key(self):
        return [self._analysisSource.getStatistics().maxes[bnum][0]['hyp'] for bnum in range(0, self._numKeys())]

    def getNoneKey(self):
        return [None] * self._numKeys()

    def setKnownKey(self, strkey):
        """Override known key by user selection"""
        try:
            self._overridedKey = util.hexstr2list(strkey)
        except ValueError:
            raise Warning("Key Selection - Could not convert '%s' to hex, key unchanged!" % strkey)

    def setHighlightedKeyColor(self, color):
        AttackObserver.highlightedKeyColor = color.red(), color.green(), color.blue()

    def setTraceColor(self, color):
        AttackObserver.traceColor = color.red(), color.green(), color.blue()