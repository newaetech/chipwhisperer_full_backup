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

from chipwhisperer.common.utils import Util, plugin


class ResultsBase(plugin.PluginTemplate):
    name = "Results"

    def __init__(self):
        super(ResultsBase, self).__init__()
        self.attack = None

    def setObservedAttack(self, attack):
        attack.attackStarted.connect(self.attackStarted)
        attack.attackDone.connect(self.attackDone)
        attack.statsUpdated.connect(self.attackStatsUpdated)
        attack.settingsChanged.connect(self.attackSettingsChanged)
        self.attack = attack

    def attackStarted(self):
        pass

    def attackStatsUpdated(self):
        pass

    def attackDone(self):
        pass

    def attackSettingsChanged(self):
        pass

    def highlightedKey(self):
        return self.attack.knownKey()

    def getWidget(self):
        return None


class ResultsWidgetBase(ResultsBase):
    def __init__(self):
        self.visibilityChanged = Util.Signal()
        ResultsBase.__init__(self)

    def setupParameters(self):
        return [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.visibilityChanged.emit}].extend(self._setupParameters())

    def _setupParameters(self):
        # PUT YOUR CUSTOMIZED PARAMETERS HERE
        return []

    def updateVisibility(self, visible):
        """Dock visability changed"""
        self.findParam('show').setValue(visible)

    def getWidget(self):
        return self