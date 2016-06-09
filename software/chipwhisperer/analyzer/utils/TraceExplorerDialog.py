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

from chipwhisperer.analyzer.utils.TraceExplorerScripts.PartitionDisplay import PartitionDisplay
from chipwhisperer.analyzer.utils.TraceExplorerScripts.TextDisplay import TextDisplay
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam


class TraceExplorerDialog(AutoScript, Parameterized):
    """Open dialog to explore trace properties, data graphs, etc"""
    _name = "Trace Explorer"
    def __init__(self, parent):
        AutoScript.__init__(self)
        self.enabled = False
        self.autoScriptInit()

        # Add example scripts to this list
        self.exampleScripts = [PartitionDisplay(self), TextDisplay(self)]

        # Add Scripts
        self.setupCommonScripts()

        self.progressBar = ProgressBar(show=False)

    def setupCommonScripts(self):
        # Setup parameer tree

        self.getParams().addChildren([
                 {'name':'Enabled', 'key':'enabled', 'type':'bool', 'default':self.getEnabled(), 'get':self.getEnabled, 'set':self.setEnabled}
        ])
        self.commonScriptParams = []
        self.paramCommonScripts = Parameter(name='Common Scripts', type='group', children=self.commonScriptParams)
        for example in self.exampleScripts:
            self.paramCommonScripts.append(example.getParams())
            example.scriptsUpdated.connect(self.updateScripts)
            example.runScriptFunction.connect(self.runScriptFunction.emit)
        self.getParams().append(self.paramCommonScripts)
        self.paramCommonScripts.hide()

        self.updateScripts()

    def getEnabled(self):
        return self.enabled

    @setupSetParam("Enabled")
    def setEnabled(self, enabled):
        self.enabled = enabled
        self.paramCommonScripts.show(enabled)
        self.updateChildren()

####COMMON SCRIPTING STUFF

    def getProgressIndicator(self):
        return self.progressBar

    def updateChildren(self):
        for example in self.exampleScripts:
            if hasattr(example, 'updateScript'):
                example.updateScript("traceexplorer_show")
        self.updateScripts()

    def updateScripts(self):
        for index, example in enumerate(self.exampleScripts):
            if hasattr(example, "_smartstatements"):
                for k in example._smartstatements:
                    statements = example.getStatements(k)
                    if len(statements) > 0:
                        prefix = example.__class__.__name__ + "_"
                        self._smartstatements[prefix + k] = example._smartstatements[k]
                        self._smartstatements[prefix + k].addSelfReplacement("exampleScripts[%d]." % index)

                for k in example.getImportStatements():
                    self.importsAppend(k)

        self.scriptsUpdated.emit()
