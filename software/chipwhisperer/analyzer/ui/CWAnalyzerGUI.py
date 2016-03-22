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

from datetime import *
import os.path
from chipwhisperer.common.ui.KeyScheduleDialog import KeyScheduleDialog
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
#from ResultsDialog import ResultsDialog
from chipwhisperer.analyzer.attacks.CPA import CPA
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.analyzer.ResultsPlotting import ResultsPlotting
# from chipwhisperer.analyzer.utils.Partition import Partition, PartitionDialog
from chipwhisperer.analyzer.utils.TraceExplorerDialog import TraceExplorerDialog
from chipwhisperer.analyzer.utils.scripteditor import MainScriptEditor
from PySide.QtCore import *
from PySide.QtGui import *
from pyqtgraph.parametertree import Parameter, ParameterTree
import chipwhisperer.common.ui.ParameterTypesCustom  # DO NOT REMOVE!!
from chipwhisperer.analyzer.attacks.Profiling import Profiling
from functools import partial
from chipwhisperer.analyzer.attacks.CPA import CPA


class CWAnalyzerGUI(CWMainGUI):
    """ Main ChipWhisperer Analyzer GUI Window Class.

    You can run this class from another Python script if you wish to, which gives you the ability
    to drive the system from another Python script, and not be forced to do everything through
    the GUI. Unfortunutly the GUI window still needs to open, as much of the program flow is
    done through PySide signals/slots.
    """

    def __init__(self, rootdir):
        super(CWAnalyzerGUI, self).__init__(CWCoreAPI(rootdir), name="ChipWhisperer" + u"\u2122" + " Analyzer " + CWCoreAPI.__version__, icon="cwiconA")
        self.addTraceDock("Waveform Display")

        self.preprocessingListGUI = [None, None, None, None]
        self.scriptList = []
        self.scriptList.append({'widget':MainScriptEditor(self)})
        self.scriptList[0]['filename'] = self.scriptList[0]['widget'].filename
        self.scriptList[0]['dockname'] = 'Auto-Generated'
        self.defaultEditor = self.scriptList[0]

        self.results = ResultsPlotting()
        self.cwAPI.results = self.results
        #self.resultsDialog = ResultsDialog(self)
        #self.addShowStats()

        self.scriptList = []
        self.plotInputEach = False
        self.traceExplorerDialog = TraceExplorerDialog(self)
        self.traceExplorerDialog.scriptsUpdated.connect(self.reloadScripts)
        self.traceExplorerDialog.runScriptFunction.connect(self.runFunc)
        self.keyScheduleDialog = KeyScheduleDialog(self)
        self.utilList = [self.traceExplorerDialog]
        self.valid_atacks = {'CPA':CPA(), 'Profiling':Profiling(self.traceExplorerDialog)}
        self.valid_preprocessingModules = self.cwAPI.getPreprocessingModules(self.cwAPI.getRootDir() + "/preprocessing", self.cwAPI.getTraceManager(), self.waveformDock.widget())

        self.addToolbars()
        self.addSettingsDocks()

        self.windowMenu.addSeparator()
        for d in self.results.dockList():
            self.addDockWidget(Qt.TopDockWidgetArea, d)
            self.windowMenu.addAction(d.toggleViewAction())
            # self.addWindowMenuAction(d.toggleViewAction(), "Results")
            # self.enforceMenuOrder()

        self.editorDocks()
        self.restoreSettings()

        self.tabifyDocks([self.settingsNormalDock, self.settingsPreprocessingDock, self.settingsAttackDock,
                          self.settingsPostProcessingDock, self.settingsResultsDock])

        for d in self.results.dockList():
            self.tabifyDockWidget(self.waveformDock, d)

        self.newProject()

        self.cwAPI.getTraceManager().tracesChanged.connect(self.tracesChanged)
        self.cwAPI.getTraceManager().tracesChanged.connect(self.reloadScripts)
        # print self.findParam('attackfilelist').items
        self.cwAPI.signals.attackChanged.connect(self.attackChanged)
        self.cwAPI.signals.reloadAttackParamList.connect(self.reloadAttackParamList)

        self.cwAPI.setAttack(self.valid_atacks["CPA"])
        self.reloadScripts()
        self.setupPreprocessorChain()

    def attackChanged(self):
        self.reloadScripts()
        self.cwAPI.getAttack().scriptsUpdated.connect(self.reloadScripts)
        self.cwAPI.getAttack().runScriptFunction.connect(self.runScriptFunction)

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

        mod = self.setupScriptModule()
        self.setupPreprocessorChain(mod)
        self.cwAPI.doAttack(mod)

    def reloadAttackParamList(self, list=None):
        """Reloads parameter tree in GUI when attack changes"""

        ExtendedParameter.reloadParams(self.cwAPI.getAttack().paramList(), self.attackParamTree)

    def tracesChanged(self):
        """Traces changed due to loading new project or adjustment in trace manager,
        so adjust limits displayed and re-plot the new input trace"""

        self.setTraceLimits(self.cwAPI.getTraceManager().NumTrace, self.cwAPI.getTraceManager().NumPoint)
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

        autogen = (self.defaultEditor['dockname'], self.defaultEditor['filename'])

        self.cwParams = [
                {'name':'Attack Script', 'type':'group', 'children':[
                    {'name':'Filename', 'key':'attackfilelist', 'type':'filelist', 'values':{autogen:0}, 'value':0, 'editor':self.editorControl,},# , 'values':self.attackscripts, 'value':None
                    ]},

                {'name':'Pre-Processing', 'type':'group', 'children':
                    [{'name':'Module #%d' % step, 'type':'list', 'value':0, 'values':self.valid_preprocessingModules, 'set':partial(self.setPreprocessing, step)} for step in range(0, len(self.preprocessingListGUI))]},

                {'name':'Attack', 'type':'group', 'children':[
                    {'name':'Module', 'type':'list', 'values':self.valid_atacks, 'value':'CPA', 'set':self.cwAPI.setAttack},
                    ]},

                {'name':'Post-Processing', 'type':'group'},

                {'name':'Result Collection', 'type':'group', 'children':[
                    {'name':'Input Trace Plot', 'type':'group', 'children':[
                        {'name':'Enabled', 'type':'bool', 'value':True},
                        {'name':'Redraw after Each (slower)', 'type':'bool', 'value':True, 'set':self.setPlotInputEach},
                        {'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0)},
                        {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'graphwidget':self.waveformDock.widget()},
                        {'name':'Redraw', 'type':'action', 'action':self.plotInputTrace},
                        ]}
                    ]},
                ]
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

    def setPreprocessing(self, num, module):
        """Insert the preprocessing module selected from the GUI into the list of active modules.

        This ensures that the options for that module are then displayed in the GUI, along with
        writing the auto-generated script.
        """
        self.preprocessingListGUI[num] = module
        if module:
            module.paramListUpdated.connect(self.reloadParamListPreprocessing)
            module.scriptsUpdated.connect(self.reloadScripts)
        self.reloadParamListPreprocessing()
        self.reloadScripts()

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
        self.cwAPI.newProject()
#        self.projectChanged.connect(self.traceExplorerDialog.setProject)

    def runFunc(self, name):
        # TODO: We should be doing this correctly, this hack is bad ;_;
        # name = "TraceExplorerDialog_PartitionDisplay_" + name
        self.runScriptFunction(name)

    def setupScriptModule(self, filename=None):
        """Loads a given script as a module for dynamic run-time insertion.

        Args:
            filename (str): The full filename to open. If None it opens the
                            auto-generated script instead.

        """

        if filename and filename != self.defaultEditor['filename']:
            raise Exception("Script Error: Cannot run script from non-default function")

        mod = self.defaultEditor['widget'].loadModule()

        # Check if we aborted due to conflitcing edit
        if mod is None:
            return None

        script = mod.userScript(self)

        if hasattr(self, "traces") and self.traces:
            script.setTraceManager(self.traces)
        return script

    def runScriptFunction(self, funcname, filename=None):
        """Loads a given script and runs a specific function within it."""

        mod = self.setupScriptModule(filename)

        if mod:
            try:
                eval('mod.%s()' % funcname)
            except AttributeError as e:
                # TODO fix this hack - this function will not exist before the
                # traceexplorer dialog has been opended, but will still be
                # called once
                if funcname == 'TraceExplorerDialog_PartitionDisplay_findPOI':
                    pass
                else:
                    # Continue with exception
                    raise

    def reloadScripts(self):
        """Rewrite the auto-generated analyzer script, using settings from the GUI"""

        # Auto-Generated is always first
        mse = self.scriptList[0]['widget']

        mse.saveSliderPosition()
        mse.editWindow.clear()

        mse.append("# Date Auto-Generated: %s" % datetime.now().strftime('%Y.%m.%d-%H.%M.%S'), 0)

        mse.append("from chipwhisperer.common.api.autoscript import AutoScriptBase", 0)

        # Get imports from preprocessing
        mse.append("#Imports from Preprocessing", 0)
        mse.append("import chipwhisperer.analyzer.preprocessing as preprocessing", 0)
        for p in self.preprocessingListGUI:
            if p:
                imports = p.getImportStatements()
                for i in imports: mse.append(i, 0)

        # Get imports from capture
        mse.append("#Imports from Capture", 0)
        for i in self.cwAPI.getAttack().getImportStatements():
            mse.append(i, 0)

        # Some other imports
        mse.append("#Imports from utilList", 0)
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for i in util.getImportStatements(): mse.append(i, 0)

        mse.append("", 0)

        # Add main class
        mse.append("class userScript(AutoScriptBase):", 0)
        mse.append("preProcessingList = []", 1)

        mse.append("def initProject(self):", 1)
        mse.append("pass")

        mse.append("def initPreprocessing(self):", 1)

        # Get init from preprocessing
        instNames = ""
        for i, p in enumerate(self.preprocessingListGUI):
            if p and p.getName() != "None":
                classname = type(p).__name__
                instname = "self.preProcessing%s%d" % (classname, i)
                mse.append("%s = preprocessing.%s.%s(self.parent)" % (instname, classname, classname))
                for s in p.getStatements('init'):
                    mse.append(s.replace("self.", instname + ".").replace("userScript.", "self."))
                instNames += instname + ","

        mse.append("self.preProcessingList = [%s]" % instNames)
        mse.append("return self.preProcessingList")

        mse.append("def initAnalysis(self):", 1)

        # Get init from analysis
        mse.append('self.attack = %s()' % type(self.cwAPI.getAttack()).__name__)
        for s in self.cwAPI.getAttack().getStatements('init'):
            mse.append(s.replace("self.", "self.attack.").replace("userScript.", "self."))

        mse.append('return self.attack')

        # Get init from reporting

        # Get go command from analysis
        mse.append("def initReporting(self, results):", 1)
        # mse.append("results.clear()")
        mse.append("results.setAttack(self.attack)")
        mse.append("results.setTraceManager(self.traceManager())")
        mse.append("self.results = results")

        mse.append("def doAnalysis(self):", 1)
        mse.append("self.attack.doAttack()")

        # Get other commands from attack module
        for k in self.cwAPI.getAttack()._smartstatements:
            if k == 'init' or k == 'go' or k == 'done':
                pass
            else:
                mse.append("def %s(self):" % k, 1)
                for s in self.cwAPI.getAttack().getStatements(k):
                    mse.append(s.replace("self.", "self.cwAPI.getAttack().").replace("userScript.", "self."))


        # Get other commands from other utilities
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for k in util._smartstatements:
                    util._smartstatements[k].addSelfReplacement("utilList[%d]." % index)
                    util._smartstatements[k].addSelfReplacement("parent.")
                    statements = util.getStatements(k)

                    if len(statements) > 0:
                        mse.append("def %s_%s(self):" % (util.__class__.__name__, k), 1)
                        for s in statements:
                            mse.append(s.replace("userScript.", "self."))

        mse.restoreSliderPosition()

    def setupPreprocessorChain(self, mod=None):
        """Setup the preprocessor chain by chaining the first module input to the source
        traces, the next module input to the previous module output, etc."""

        if mod is None:
            mod = self.setupScriptModule()
        self.preprocessingList = mod.initPreprocessing()

        self.lastoutput = self.cwAPI.getTraceManager()
        for t in self.preprocessingList:
            if t:
                t.setTraceSource(self.lastoutput)
                t.init()
                self.lastoutput = t
        self.traces = self.lastoutput

        mod.setTraceManager(self.traces)

        # self.reloadScripts()

    def setTraceLimits(self, traces=None, points=None, deftrace=1, defpoint=-1):
        if defpoint == -1:
            defpoint = points

        self.cwAPI.setTraceLimits(traces, points)
        # Set local parameters for trace viewer
        if traces is not None:
            self.findParam('tracerng').setLimits((0, traces))
            # TODO: Bug in pyqtgraph maybe - if call with just deftrace &
            #setLimits was called with (0,0), the setValue(1) is ignored which is OK,
            #but then calling setLimits with higher value followed by setValue still
            #has no effect??
            #WORKAROUND: use min(traces,deftrace) to ensure don't set value beyond limit for now
            self.findParam('tracerng').setValue((0, min(traces, deftrace)))

        if points:
            self.findParam('pointrng').setLimits((0, points))
            self.findParam('pointrng').setValue((0, defpoint))


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
    window = CWAnalyzerGUI(os.path.join('chipwhisperer', 'analyzer'))
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
