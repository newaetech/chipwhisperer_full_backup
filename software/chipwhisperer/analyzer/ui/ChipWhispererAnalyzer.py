#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
#=================================================


import sys
from chipwhisperer.common.ui.KeyScheduleDialog import KeyScheduleDialog
from functools import partial
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.ui.MainChip import MainChip
#from ResultsDialog import ResultsDialog
from chipwhisperer.analyzer.attacks.CPA import CPA
from chipwhisperer.analyzer.api.CWAnalizerAPI import CWAnalizerAPI
from chipwhisperer.analyzer.ResultsPlotting import ResultsPlotting
from chipwhisperer.analyzer.ListAllModules import ListAllModules
# from chipwhisperer.analyzer.utils.Partition import Partition, PartitionDialog
from chipwhisperer.analyzer.utils.TraceExplorerDialog import TraceExplorerDialog
from chipwhisperer.analyzer.utils.scripteditor import MainScriptEditor
from PySide.QtCore import *
from PySide.QtGui import *
from pyqtgraph.parametertree import Parameter, ParameterTree


class ChipWhispererAnalyzer(MainChip):
    """ Main ChipWhisperer Analyzer GUI Window Class.

    You can run this class from another Python script if you wish to, which gives you the ability
    to drive the system from another Python script, and not be forced to do everything through
    the GUI. Unfortunutly the GUI window still needs to open, as much of the program flow is
    done through PySide signals/slots.
    """

    def __init__(self):
        super(ChipWhispererAnalyzer, self).__init__(CWAnalizerAPI(), name="ChipWhisperer" + u"\u2122" + " Analyzer V2", icon="cwiconA")
        self.cwAPI.setupParameters("", self.showScriptParameter)
        self.results = ResultsPlotting()
        #self.resultsDialog = ResultsDialog(self)
        #self.addShowStats()
        self.addTraceDock("Waveform Display")

        self.plotInputEach = False

        self.addToolbars()
        self.addSettingsDocks()

        self.traceExplorerDialog = TraceExplorerDialog(self)
        self.traceExplorerDialog.scriptsUpdated.connect(self.reloadScripts)
        self.traceExplorerDialog.runScriptFunction.connect(self.runFunc)
        self.cwAPI.utilList.append(self.traceExplorerDialog)

        self.keyScheduleDialog = KeyScheduleDialog(self)

        self.scriptList.append({'widget':MainScriptEditor(self)})
        self.scriptList[0]['filename'] = self.scriptList[0]['widget'].filename
        self.scriptList[0]['dockname'] = 'Auto-Generated'
        self.defaultEditor = self.scriptList[0]
        # autogen = (self.scriptList[0]['dockname'], self.scriptList[0]['filename'])


        for d in self.results.dockList():
            self.addDockWidget(Qt.RightDockWidgetArea, d)
            self.addWindowMenuAction(d.toggleViewAction(), "Results")
            self.enforceMenuOrder()

        self.editorDocks()
        self.restoreDockGeometry()

        self.tabifyDocks([self.settingsNormalDock, self.settingsPreprocessingDock, self.settingsAttackDock,
                          self.settingsPostProcessingDock, self.settingsResultsDock])

        for d in self.results.dockList():
            self.tabifyDockWidget(self.waveformDock, d)

        self.newProject()

        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)
        self.openFile.connect(self.openProject)

        self.manageTraces.tracesChanged.connect(self.tracesChanged)
        cpaTemp = CPA(self, showScriptParameter=self.showScriptParameter)
        self.setAttack(cpaTemp)

        self.setupPreprocessorChain()

        # print self.findParam('attackfilelist').items
        self.cwAPI.signals.projectChanged.connect(lambda: self.traceExplorerDialog.setProject(self.project()))

    def listModules(self):
        """Overload this to test imports"""
        return ListAllModules()

    def editorDocks(self):
        """Ensure we have a script editor window for each referenced analyzer script file"""

        for script in self.scriptList:

            dockname = "Analysis Script: %s" % script['dockname']

            # No previous dock, do setup
            if 'dock' not in script.keys():
                script['widget'].editWindow.runFunction.connect(partial(self.runScriptFunction, filename=script['filename']))
                script['dock'] = self.addDock(script['widget'], name=dockname, area=Qt.RightDockWidgetArea)

            # Dock present, check if name changed
            if script['dock'].windowTitle() != dockname:
                script['dock'].setWindowTitle(dockname)

    def editorControl(self, filename, filedesc, default=False, bringToFront=True):
        """This is the call-back from the script editor file list, which opens editors"""

        # Find filename
        thisEditor = None

        for e in self.scriptList:
            if e['filename'] == filename:
                thisEditor = e
                break

        if thisEditor is None:
            thisEditor = {'widget':MainScriptEditor(parent=self, filename=filename)}
            thisEditor['filename'] = filename
            thisEditor['dockname'] = filedesc
            self.scriptList.append(thisEditor)

        # Update all docks if required
        thisEditor['dockname'] = filedesc
        self.editorDocks()

        if bringToFront:
            thisEditor['dock'].show()
            thisEditor['dock'].raise_()

        if default:
            # Set as default for attacks etc
            self.defaultEditor = thisEditor


    def setPlotInputEach(self, enabled):
        self.plotInputEach = enabled

    def addToolbars(self):
        """Add toolbars and menus to the main window"""

        attack = QAction(QIcon(':/images/attack.png'), 'Start Attack', self)
        attack.triggered.connect(self.doAttack)

        self.AttackToolbar = self.addToolBar('Attack Tools')
        self.AttackToolbar.setObjectName('Attack Toolbar')
        self.AttackToolbar.addAction(attack)
        self.AttackToolbar.show()


        # Add utilities

        self.UtilitiesTraceExplorer = QAction('Trace Explorer', self,
                               statusTip='Get information on traces',
                               triggered=self.traceExplorerDialog.show)

        self.UtilitiesAESSchedule = QAction('AES Key Schedule', self,
                               statusTip='Show AES Key Schedule calculator',
                               triggered=self.keyScheduleDialog.show)

        # self.UtilitiesPartition = QAction('Generate Partitions', self,
        #                       statusTip='Generate Partitions for Template Attacks',
        #                       triggered=self.PartitionDialog.exec_)

        self.toolMenu.addSeparator()
        self.toolMenu.addAction(self.UtilitiesTraceExplorer)
        self.toolMenu.addAction(self.UtilitiesAESSchedule)
        # self.toolMenu.addAction(self.UtilitiesPartition)
        self.toolMenu.addSeparator()

    def reloadParamListPreprocessing(self, list=None):
        """Reload the parameter lists, ensuring GUI is showing correct options to user"""

        plist = []
        for p in self.preprocessingListGUI:
            if p:
                for item in p.paramList():
                    plist.append(item)
        ExtendedParameter.reloadParams(plist, self.preprocessingParamTree)

    def doAttack(self):
        """Called when the 'Do Attack' button is pressed, or can be called via API
        to cause attack to run"""

        #Check if traces enabled
        if self.traces.NumTrace == 0:
            msgBox = QMessageBox(QMessageBox.Warning, "Trace Error", "No traces enabled in project - open Trace Manager?",
                                       QMessageBox.NoButton, self)
            msgBox.addButton("Yes", QMessageBox.AcceptRole)
            msgBox.addButton("No", QMessageBox.RejectRole)
            if msgBox.exec_() == QMessageBox.AcceptRole:
                self.manageTraces.show()
            return
        cwAPI.doAttack()

    def reloadAttackParamList(self, list=None):
        """Reloads parameter tree in GUI when attack changes"""

        ExtendedParameter.reloadParams(self.attack.paramList(), self.attackParamTree)

    def tracesChanged(self):
        """Traces changed due to loading new project or adjustment in trace manager,
        so adjust limits displayed and re-plot the new input trace"""

        self.setTraceLimits(self.manageTraces.iface.NumTrace, self.manageTraces.iface.NumPoint)
        self.plotInputTrace()

    def plotInputTrace(self):
        """Plot the input trace(s) as given by the GUI settings."""

        #print "Plotting %d-%d for points %d-%d"%(params[0].value(), params[1].value(), params[2].value(), params[3].value())
        self.waveformDock.widget().clearPushed()
        self.setupPreprocessorChain()

        tstart = self.findParam('tracerng').value()[0]
        tend = self.findParam('tracerng').value()[1]
        pstart = self.findParam('pointrng').value()[0]
        pend = self.findParam('pointrng').value()[1]

        ttotal = 0

        if tend - tstart > 1:
            self.waveformDock.widget().setPersistance(True)

        for tnum in range(tstart, tend):
            trace = self.traces.getTrace(tnum)

            if trace is None:
                continue

            ttotal += 1
            self.waveformDock.widget().passTrace(trace[pstart:pend], pstart)

            if self.plotInputEach:
                QCoreApplication.processEvents()

        # print ttotal

    #def addShowStats(self):
    #    self.statsShowAct = QAction('&Results Monitor', self, statusTip='Plot/Save PGE etc', triggered=self.resultsDialog.show)
    #    self.projectMenu.addAction(self.statsShowAct)

    def addSettingsDocks(self):
        """Add settings dock to main window"""

        self.setupParametersTree()
        self.settingsNormalDock = self.addSettings(self.paramTree, "General")
        self.settingsPreprocessingDock = self.addSettings(self.preprocessingParamTree, "Preprocessing")
        self.settingsAttackDock = self.addSettings(self.attackParamTree, "Attack")
        self.settingsPostProcessingDock = self.addSettings(self.postprocessingParamTree, "Postprocessing")
        self.settingsResultsDock = self.addSettings(self.resultsParamTree, "Results")

    def setupParametersTree(self):
        """Setup all parameter trees so they can be reloaded later with changes"""

        self.cwParams = self.cwAPI.cwParams.append(
            {'name':'Result Collection', 'type':'group', 'children':[
                    {'name':'Input Trace Plot', 'type':'group', 'children':[
                        {'name':'Enabled', 'type':'bool', 'value':True},
                        {'name':'Redraw after Each (slower)', 'type':'bool', 'value':True, 'set':self.setPlotInputEach},
                        {'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0)},
                        {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'graphwidget':self.waveformDock.widget()},
                        {'name':'Redraw', 'type':'action', 'action':self.plotInputTrace},
                        ]}
                    ]}
        )

        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)
        self.preprocessingParamTree = ParameterTree()
        self.attackParamTree = ParameterTree()
        self.postprocessingParamTree = ParameterTree()
        self.resultsParamTree = ParameterTree()

        self.results.paramListUpdated.connect(self.reloadParamListResults)
        self.reloadParamListResults()

    def reloadParamListResults(self, lst=None):
        """Reload parameter tree for results settings, ensuring GUI matches loaded modules."""

        ExtendedParameter.reloadParams(self.results.paramList(), self.resultsParamTree)

    def reloadParamList(self, lst=None):
        """Reload parameter trees in a given list, ensuring GUI matches loaded modules."""

        ExtendedParameter.reloadParams(self.paramList(), self.paramTree)

    def paramList(self):
        p = []
        p.append(self.params)
        return p

    def newProject(self):
        cwAPI.newProject()
#        self.projectChanged.connect(self.traceExplorerDialog.setProject)


def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Analyzer V2")
    return app

def main():
    # Create the Qt Application
    app = makeApplication()
    # Create and show the form
    window = ChipWhispererAnalyzer()
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
