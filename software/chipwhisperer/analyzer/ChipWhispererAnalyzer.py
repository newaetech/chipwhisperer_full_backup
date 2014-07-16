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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
from datetime import datetime
import random
import os.path
import shlex
from subprocess import Popen, PIPE

import scipy
import numpy as np
import chipwhisperer.common.qrc_resources
from functools import partial

from openadc.ExtendedParameter import ExtendedParameter

try:
    import writer_dpav3
except ImportError:
    writer_dpav3 = None
    writer_dpav3_str = sys.exc_info()

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()
    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None    

from chipwhisperer.common.MainChip import MainChip
#from ResultsDialog import ResultsDialog
from chipwhisperer.common.ProjectFormat import ProjectFormat
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.analyzer.attacks.CPA import CPA
from chipwhisperer.analyzer.attacks.Profiling import Profiling
from chipwhisperer.analyzer.ResultsPlotting import ResultsPlotting
import chipwhisperer.analyzer.preprocessing.Preprocessing as Preprocessing
import chipwhisperer.common.ParameterTypesCustom

#TEMP
from chipwhisperer.analyzer.ResultsPlotting import ResultsPlotData
from chipwhisperer.analyzer.ListAllModules import ListAllModules
# from chipwhisperer.analyzer.utils.Partition import Partition, PartitionDialog
from chipwhisperer.analyzer.utils.TraceExplorerDialog import TraceExplorerDialog


# For MainScriptEditor
import imp
import uuid
import tempfile

class CodeEditor(QTextEdit):

    runFunction = Signal(str)
    assignFunction = Signal(str)

    def contextMenuEvent(self, event):
        menu = self.createStandardContextMenu()
        menu.insertAction(menu.actions()[0], QAction("Run Function", self, triggered=self.rFuncAct))
        # menu.insertAction(menu.actions()[1], QAction("Assign to Toolbar", self, triggered=self.assFuncAct))
        menu.insertSeparator(menu.actions()[1])
        menu.exec_(event.globalPos())

    def rFuncAct(self):
        self.runFunction.emit(self.textCursor().selectedText())

    def assFuncAct(self):
        self.assignFunction.emit(self.textCursor().selectedText())

class MainScriptEditor(QWidget):
    def __init__(self, parent):
        super(MainScriptEditor, self).__init__(parent)

        self.editWindow = CodeEditor()

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.editWindow)

        self.setLayout(mainLayout)
        self.lastLevel = 0
        
        self.tfile = tempfile.NamedTemporaryFile('w', suffix='.py', prefix='cwautoscript_', delete=False)
        self.tfile.close()
        # print self.tfile.name

    def append(self, statement, level=2):
        if self.lastLevel > level:
            self.editWindow.append("")
        self.lastLevel = level
        self.editWindow.append(" "*(level * 4) + statement)
        
    def loadModule(self):
        # Save text editor somewhere
        f = open(self.tfile.name, 'w')
        filecontents = self.editWindow.toPlainText()
        f.write(filecontents)
        f.close()

        modulename = str(uuid.uuid1())
        self.scriptModule = imp.load_source(modulename, self.tfile.name)

        # print self.scriptModule
        return self.scriptModule


class ChipWhispererAnalyzer(MainChip):
    MaxRecentFiles = 4    
    def __init__(self):
        super(ChipWhispererAnalyzer, self).__init__(name="ChipWhisperer" + u"\u2122" + " Analyzer V2")
        self.console = self.addConsole()   
        
        self.results = ResultsPlotting()
        #self.resultsDialog = ResultsDialog(self)
        #self.addShowStats()
        
        self.addWaveforms()
        
        numPreprocessingStep = 4
        self.preprocessingListGUI = [None] * numPreprocessingStep
        
        
        self.utilList = []
        self.traceExplorerDialog = TraceExplorerDialog(self)
        self.utilList.append(self.traceExplorerDialog)

        self.cwParams = [
                {'name':'Traces', 'type':'group', 'children':[
                    {'name':'Points', 'type':'int', 'value':0, 'readonly':True},
                    {'name':'Traces', 'type':'int', 'value':0, 'readonly':True}
                    ]},
                    
                {'name':'Pre-Processing', 'type':'group', 'children':
                    [{'name':'Module #%d' % step, 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, step)} for step in range(0, numPreprocessingStep)]},
                         
                {'name':'Attack', 'type':'group', 'children':[
                    {'name':'Module', 'type':'list', 'values':{'CPA':CPA(self, console=self.console, showScriptParameter=self.showScriptParameter),
                                                               'Profiling':Profiling(self, console=self.console, showScriptParameter=self.showScriptParameter),
                                                               }, 'value':'CPA', 'set':self.setAttack},
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
        
        self.plotInputEach = False
        
        self.da = None
        self.numTraces = 100
        
        self.traceLimits = 0
        self.pointLimits = 0

        self.addToolbars()
        self.addSettingsDocks()
        
        for d in self.results.dockList():
            self.addDockWidget(Qt.RightDockWidgetArea, d)
            self.addWindowMenuAction(d.toggleViewAction(), "Results")
            self.enforceMenuOrder()


        self.mse = MainScriptEditor(self)
        self.mse.editWindow.runFunction.connect(self.runScriptFunction)
        self.addDock(self.mse, name="Scripting", area=Qt.RightDockWidgetArea)
        
        self.restoreDockGeometry()
        
        #Generate correct tab order now that we've restored
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsPreprocessingDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsAttackDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsPostProcessingDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsResultsDock)
        
        for d in self.results.dockList():
            self.tabifyDockWidget(self.waveformDock, d)

        self.newProject()   
        
        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)
        self.openFile.connect(self.openProject)

        self.manageTraces.tracesChanged.connect(self.tracesChanged)
        cpaTemp = CPA(self, console=self.console, showScriptParameter=self.showScriptParameter)
        self.setAttack(cpaTemp)
        
        self.setupPreprocessorChain()
        
    def listModules(self):
        """Overload this to test imports"""
        return ListAllModules()
     
        
    def setPlotInputEach(self, enabled):
        self.plotInputEach = enabled
        
    def addToolbars(self):
        attack = QAction(QIcon(':/images/attack.png'), 'Start Attack', self)
        attack.triggered.connect(self.doAttack)

        self.AttackToolbar = self.addToolBar('Attack Tools')
        self.AttackToolbar.setObjectName('Attack Tools')
        self.AttackToolbar.addAction(attack)  
        

        # Add utilities

        self.UtilitiesTraceExplorer = QAction('Trace Explorer', self,
                               statusTip='Get information on traces',
                               triggered=self.traceExplorerDialog.show)

        # self.UtilitiesPartition = QAction('Generate Partitions', self,
        #                       statusTip='Generate Partitions for Template Attacks',
        #                       triggered=self.PartitionDialog.exec_)

        self.toolMenu.addSeparator()
        self.toolMenu.addAction(self.UtilitiesTraceExplorer)
        # self.toolMenu.addAction(self.UtilitiesPartition)
        self.toolMenu.addSeparator()

    def setPreprocessing(self, num, module):
        self.preprocessingListGUI[num] = module
        if module:
            module.paramListUpdated.connect(self.reloadParamListPreprocessing)
            module.scriptsUpdated.connect(self.reloadScripts)
        self.reloadParamListPreprocessing() 
        self.reloadScripts()

    def reloadScripts(self):
        self.mse.editWindow.clear()

        self.mse.append("# Date Auto-Generated: Yes Please", 0)

        self.mse.append("from chipwhisperer.common.autoscript import AutoScriptBase", 0)

        # Get imports from preprocessing
        self.mse.append("#Imports from Preprocessing", 0)
        self.mse.append("import chipwhisperer.analyzer.preprocessing as preprocessing", 0)
        for p in self.preprocessingListGUI:
            if p:
                imports = p.getImportStatements()
                for i in imports: self.mse.append(i, 0)

        # Get imports from capture
        self.mse.append("#Imports from Capture", 0)
        for i in self.attack.getImportStatements():
            self.mse.append(i, 0)

        # Some other imports
        self.mse.append("#Imports from utilList", 0)
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements'):
                for i in util.getImportStatements(): self.mse.append(i, 0)
            

        self.mse.append("", 0)

        # Add main class
        self.mse.append("class userScript(AutoScriptBase):", 0)
        self.mse.append("preProcessingList = []", 1)

        self.mse.append("def initProject(self):", 1)
        self.mse.append("pass")


        self.mse.append("def initPreprocessing(self):", 1)

        # Get init from preprocessing
        instNames = ""
        for i, p in enumerate(self.preprocessingListGUI):
            if p:
                classname = type(p).__name__
                instname = "self.preProcessing%s%d" % (classname, i)
                self.mse.append("%s = preprocessing.%s.%s(self.parent)" % (instname, classname, classname))
                for s in p.getStatements('init'):
                    self.mse.append(s.replace("self.", instname + ".").replace("userScript.", "self."))
                instNames += instname + ","

        self.mse.append("self.preProcessingList = [%s]" % instNames)
        self.mse.append("return self.preProcessingList")


        self.mse.append("def initAnalysis(self):", 1)

        # Get init from analysis
        self.mse.append('self.attack = %s(self.parent, console=self.console, showScriptParameter=self.showScriptParameter)' % type(self.attack).__name__)
        for s in self.attack.getStatements('init'):
            self.mse.append(s.replace("self.", "self.attack.").replace("userScript.", "self."))

        self.mse.append('return self.attack')

        # Get init from reporting

        # Get go command from analysis
        self.mse.append("def initReporting(self, results):", 1)
        # self.mse.append("results.clear()")
        self.mse.append("results.setAttack(self.attack)")
        self.mse.append("results.setTraceManager(self.traceManager())")
        self.mse.append("self.results = results")

        self.mse.append("def doAnalysis(self):", 1)
        self.mse.append("self.attack.doAttack()")

        # Get other commands from attack module
        for k in self.attack._smartstatements:
            if k == 'init' or k == 'go' or k == 'done':
                pass
            else:
                self.mse.append("def %s(self):" % k, 1)
                for s in self.attack.getStatements(k):
                    self.mse.append(s.replace("self.", "self.attack.").replace("userScript.", "self."))


        # Get other commands from other utilities
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements'):
                for k in util._smartstatements:
                    util._smartstatements[k].addSelfReplacement("utilList[%d]." % index)
                    util._smartstatements[k].addSelfReplacement("parent.")
                    statements = util.getStatements(k)
                    
                    if len(statements) > 0:
                        self.mse.append("def %s_%s(self):" % (util.__class__.__name__, k), 1)
                        for s in statements:
                            self.mse.append(s.replace("userScript.", "self."))
                            
                    



    def reloadParamListPreprocessing(self, list=None):        
        plist = []
        for p in self.preprocessingListGUI:
            if p:
                for item in p.paramList():
                    plist.append(item)
        ExtendedParameter.reloadParams(plist, self.preprocessingParamTree)

    def setAttack(self, attack):
        self.attack = attack
        self.reloadAttackParamList()
        self.results.setAttack(self.attack)
        self.attack.paramListUpdated.connect(self.reloadAttackParamList)
        self.attack.setTraceLimits(self.traceLimits, self.pointLimits)
        
        # Sometimes required
        if hasattr(self, "traces") and self.traces:
            self.attack.setTraceManager(self.traces)

        self.attack.setProject(self.proj)
        self.attack.scriptsUpdated.connect(self.reloadScripts)
        self.reloadScripts()

    def setupScriptModule(self):
        mod = self.mse.loadModule().userScript(self, self.console, self.showScriptParameter)
        if hasattr(self, "traces") and self.traces:
            mod.setTraceManager(self.traces)
        return mod

    def runScriptFunction(self, funcname):
        mod = self.setupScriptModule()
        eval('mod.%s()' % funcname)

    def doAttack(self):
        #Check if traces enabled
        if self.traces.NumTrace == 0:
            msgBox = QMessageBox(QMessageBox.Warning, "Trace Error", "No traces enabled in project - open Trace Manager?",
                                       QMessageBox.NoButton, self)
            msgBox.addButton("Yes", QMessageBox.AcceptRole)
            msgBox.addButton("No", QMessageBox.RejectRole)
            if msgBox.exec_() == QMessageBox.AcceptRole:
                self.manageTraces.show()
            return
        
        self.console.append("Loading...")
        mod = self.setupScriptModule()
        # mod.initProject()

        # Setup trace sources etc, this calls the
        # .initPreprocessing itself
        # it also resets the setTraces in the passed 'mod',
        # which is REQUIRED for proper functioning!
        self.setupPreprocessorChain(mod)

        mod.initAnalysis()
        mod.initReporting(self.results)
        
        mod.doAnalysis()

        mod.doneAnalysis()
        mod.doneReporting()

        # self.console.append("Attack Started")
        # if self.results:
        #    self.results.setTraceManager(self.traces)
        #
        # if self.attack:
        #    self.attack.setTraceManager(self.traces)
        #    self.attack.doAttack()
            
        # self.console.append("Attack Done")
        
    def reloadAttackParamList(self, list=None):
        ExtendedParameter.reloadParams(self.attack.paramList(), self.attackParamTree)
        
    def tracesChanged(self):
        self.setTraceLimits(self.manageTraces.iface.NumTrace, self.manageTraces.iface.NumPoint)
        self.plotInputTrace()

    def setupPreprocessorChain(self, mod=None):
        if mod is None:
            mod = self.setupScriptModule()
        self.preprocessingList = mod.initPreprocessing()

        self.lastoutput = self.manageTraces.iface
        for t in self.preprocessingList:
            if t:
                t.setTraceSource(self.lastoutput)
                t.init()
                self.lastoutput = t           
        self.traces = self.lastoutput

        for item in self.utilList:
            item.setTraceSource(self.traces)

        mod.setTraceManager(self.traces)

        # self.reloadScripts()
        
    def plotInputTrace(self):
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

        print ttotal
        
    def setTraceLimits(self, traces=None, points=None, deftrace=1, defpoint=-1):
        """When traces is loaded, Tell everything default point/trace range"""
        if defpoint == -1:
            defpoint = points
            
        #Set parameters for attack
        self.traceLimits = traces
        self.pointLimits = points
        self.attack.setTraceLimits(traces, points)
            
        # Set local parameters for trace viewer
        if traces is not None:
            self.findParam('tracerng').setLimits((0, traces))
            #TODO: Bug in pyqtgraph maybe - if call with just deftrace & 
            #setLimits was called with (0,0), the setValue(1) is ignored which is OK,
            #but then calling setLimits with higher value followed by setValue still
            #has no effect??
            #WORKAROUND: use min(traces,deftrace) to ensure don't set value beyond limit for now
            self.findParam('tracerng').setValue((0, min(traces, deftrace)))
            
        
        if points:
            self.findParam('pointrng').setLimits((0, points))
            self.findParam('pointrng').setValue((0, defpoint))

    def addWaveforms(self):
        self.waveformDock = self.addTraceDock("Waveform Display")        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault()
 
    #def addShowStats(self):
    #    self.statsShowAct = QAction('&Results Monitor', self, statusTip='Plot/Save PGE etc', triggered=self.resultsDialog.show)
    #    self.projectMenu.addAction(self.statsShowAct)
        
    def addSettingsDocks(self):      
        self.setupParametersTree()        
        self.settingsNormalDock = self.addSettings(self.paramTree, "General")
        self.settingsPreprocessingDock = self.addSettings(self.preprocessingParamTree, "Preprocessing")
        self.settingsAttackDock = self.addSettings(self.attackParamTree, "Attack")
        self.settingsPostProcessingDock = self.addSettings(self.postprocessingParamTree, "Postprocessing")
        self.settingsResultsDock = self.addSettings(self.resultsParamTree, "Results")
        

    def setupParametersTree(self):
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
        ExtendedParameter.reloadParams(self.results.paramList(), self.resultsParamTree)
        
    def reloadParamList(self, lst=None):
        ExtendedParameter.reloadParams(self.paramList(), self.paramTree)  
        
    def paramList(self):
        p = []
        p.append(self.params)     
        return p        
    
    def openProject(self, fname):
        self.proj = ProjectFormat()
        self.proj.setProgramName("ChipWhisperer-Analyzer")
        self.proj.setProgramVersion("2.00")
        self.proj.setTraceManager(self.manageTraces)  
        self.setCurrentFile(fname)
        self.proj.setFilename(fname)
        self.proj.load()
        
        #Open project file & read in everything
        self.proj.traceManager.loadProject(fname)

        # Ensure attack knows about this project
        self.attack.setProject(self.proj)
        self.traceExplorerDialog.setProject(self.proj)

  
    def newProject(self):        
        #TODO: Move this to MainChip
        self.proj = ProjectFormat()
        self.proj.setProgramName("ChipWhisperer-Analyzer")
        self.proj.setProgramVersion("2.00")
        self.proj.addParamTree(self)    
        self.proj.setTraceManager(self.manageTraces)  
        self.setCurrentFile(None)
        self.traceExplorerDialog.setProject(self.proj)
  
    def saveProject(self):
        #TODO: Move to MainChip
        if self.proj.hasFilename() == False:
            fname, _ = QFileDialog.getSaveFileName(self, 'Save New File','.','*.cwp')
            
            if fname is None:
                return
            
            self.proj.setFilename(fname)
            self.setCurrentFile(fname)
            
        self.proj.save()
        self.dirty = False
        self.updateTitleBar()
        self.statusBar().showMessage("Project Saved")
  
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
