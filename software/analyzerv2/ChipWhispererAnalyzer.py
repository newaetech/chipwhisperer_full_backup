#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import scipy
import numpy as np

from functools import partial

from ExtendedParameter import ExtendedParameter

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

from MainChip import MainChip
from ProjectFormat import ProjectFormat
from TraceContainerNative import TraceContainerNative
from attacks.CPA import CPA
from ResultsPlotting import ResultsPlotting
import preprocessing.Preprocessing as Preprocessing

#TEMP
from ResultsPlotting import ResultsPlotData

class ChipWhispererAnalyzer(MainChip):
    MaxRecentFiles = 4    
    def __init__(self):
        super(ChipWhispererAnalyzer, self).__init__(name="ChipWhisperer Analyzer V2", imagepath=imagePath)
        self.console = self.addConsole()   
        
        self.results = ResultsPlotting()
        
        numPreprocessingStep = 3
        self.preprocessingList = [None]*numPreprocessingStep
        
        self.cwParams = [
                {'name':'Traces', 'type':'group', 'children':[
                    {'name':'Points', 'type':'int', 'value':0, 'readonly':True},
                    {'name':'Traces', 'type':'int', 'value':0, 'readonly':True}
                    ]},
                    
                {'name':'Pre-Processing', 'type':'group', 'children':#[
                    #{'name':'All Enabled', 'type':'bool', 'value':False},
                    #{'name':'Module #1', 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, 0)},
                    #{'name':'Module #2', 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, 1)},
                    #{'name':'Module #3', 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, 2)},
                    #]},
                    
                    [{'name':'Module #%d'%step, 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, step)} for step in range(0, numPreprocessingStep)]},
                    
                         
                {'name':'Attack', 'type':'group', 'children':[
                    {'name':'Module', 'type':'list', 'values':{'CPA':CPA(self)}, 'value':'CPA', 'set':self.setAttack},                                          
                    ]},
                         
                {'name':'Post-Processing', 'type':'group'},
                
                {'name':'Result Collection', 'type':'group', 'children':[
                    {'name':'Input Trace Plot', 'type':'group', 'children':[
                        {'name':'Enabled', 'type':'bool', 'value':True},
                        {'name':'Redraw after Each (slower)', 'type':'bool', 'value':True, 'set':self.setPlotInputEach},
                        {'name':'Starting Trace', 'type':'int', 'limits':(0,0), 'value':0},
                        {'name':'Ending Trace', 'type':'int', 'limits':(0,0), 'value':0},
                        {'name':'Starting Point', 'type':'int', 'limits':(0,0), 'value':0},
                        {'name':'Ending Point', 'type':'int', 'limits':(0,0), 'value':0},
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
        self.addWaveforms()
        
        for d in self.results.dockList():
            self.addDockWidget(Qt.RightDockWidgetArea, d)
            self.addWindowMenuAction(d.toggleViewAction(), "Results")
            self.enforceMenuOrder()
        
        self.restoreDockGeometry()
        
        #Generate correct tab order now that we've restored
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsPreprocessingDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsAttackDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsPostProcessingDock)
        self.tabifyDockWidget(self.settingsNormalDock, self.settingsResultsDock)
        
        self.newProject()   
        
        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)
        self.openFile.connect(self.openProject)

        self.manageTraces.tracesChanged.connect(self.tracesChanged)
        self.setAttack(CPA(self))
        
        self.setupPreprocessorChain()
        
    def setPlotInputEach(self, enabled):
        self.plotInputEach = enabled
        
    def addToolbars(self):
        attack = QAction(QIcon(imagePath+'attack.png'), 'Start Attack', self)
        attack.triggered.connect(self.doAttack)

        self.AttackToolbar = self.addToolBar('Attack Tools')
        self.AttackToolbar.setObjectName('Attack Tools')
        self.AttackToolbar.addAction(attack)  
        
    def setPreprocessing(self, num, module):
        self.preprocessingList[num] = module
        module.paramListUpdated.connect(self.reloadParamListPreprocessing)
        self.reloadParamListPreprocessing() 
        self.setupPreprocessorChain()   

    def reloadParamListPreprocessing(self, list=None):        
        plist = []
        for p in self.preprocessingList:
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
        
    def doAttack(self):
        self.console.append("Attack Started")
        
        if self.results is not None:
            self.results.setTraceManager(self.traces)
        
        if self.attack is not None:
            self.attack.setTraceManager(self.traces)
            self.attack.doAttack()
            
        self.console.append("Attack Done")
        
    def reloadAttackParamList(self, list=None):
        ExtendedParameter.reloadParams(self.attack.paramList(), self.attackParamTree)
        
    def tracesChanged(self):
        self.setTraceLimits(self.manageTraces.iface.NumTrace, self.manageTraces.iface.NumPoint)
        self.plotInputTrace()

    def setupPreprocessorChain(self):
        self.lastoutput = self.manageTraces.iface
        for t in self.preprocessingList:
            if t:
                t.setTraceManager(self.lastoutput)
                t.init()
                self.lastoutput = t           
        self.traces = self.lastoutput

        
    def plotInputTrace(self):
        #print "Plotting %d-%d for points %d-%d"%(params[0].value(), params[1].value(), params[2].value(), params[3].value())
        params = self.inputTraceSettingParams()
        self.waveformDock.widget().clearPushed()
        self.waveformDock.widget().setPersistance(True)
        
        self.setupPreprocessorChain()
        
        tstart = params[0].value()
        tend = params[1].value()
        pstart = params[2].value()
        pend = params[3].value()
        
        ttotal = 0
        
        for tnum in range(tstart, tend):
            trace = self.traces.getTrace(tnum)
            
            if trace is None:
                continue
                  
            ttotal += 1                
            self.waveformDock.widget().passTrace(trace[pstart:pend])
            
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
            
        #Set local parameters for trace viewer
        params = self.inputTraceSettingParams()
        if traces is not None:
            params[0].setLimits((0, traces))
            params[1].setLimits((0, traces))
            params[0].setValue(0)
            #TODO: Bug in pyqtgraph maybe - if call with just deftrace & 
            #setLimits was called with (0,0), the setValue(1) is ignored which is OK,
            #but then calling setLimits with higher value followed by setValue still
            #has no effect??
            #WORKAROUND: use min(traces,deftrace) to ensure don't set value beyond limit for now
            params[1].setValue(min(traces, deftrace))
            
        
        if points is not None:
            params[2].setLimits((0, points))
            params[3].setLimits((0, points))
            params[2].setValue(0)
            params[3].setValue(defpoint)
        
        
    def inputTraceSettingParams(self):
        """Find parameters dealing with input trace plotting"""
        tracestart = None
        traceend = None
        pointstart = None
        pointend = None
        
        for p in self.params.children():
            if p.name() == 'Result Collection':
                for t in p.children():
                    if t.name() == 'Input Trace Plot':
                        for q in t.children():
                            if q.name() == 'Starting Trace':
                                tracestart = q
                                
                            if q.name() == 'Ending Trace':
                                traceend = q
                                
                            if q.name() == 'Starting Point':
                                pointstart = q
                                
                            if q.name() == 'Ending Point':
                                pointend = q
                                
        return (tracestart, traceend, pointstart, pointend)

    def addWaveforms(self):
        self.waveformDock = self.addTraceDock("Waveform Display")        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault() 
 
        
    def addSettingsDocks(self):      
        self.setupParametersTree()        
        self.settingsNormalDock = self.addSettings(self.paramTree, "General")
        self.settingsPreprocessingDock = self.addSettings(self.preprocessingParamTree, "Preprocessing")
        self.settingsAttackDock = self.addSettings(self.attackParamTree, "Attack")
        self.settingsPostProcessingDock = self.addSettings(self.postprocessingParamTree, "Postprocessing")
        self.settingsResultsDock = self.addSettings(self.resultsParamTree, "Results")
        

    def setupParametersTree(self):
        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params)
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

  
    def newProject(self):        
        #TODO: Move this to MainChip
        self.proj = ProjectFormat()
        self.proj.setProgramName("ChipWhisperer-Analyzer")
        self.proj.setProgramVersion("2.00")
        self.proj.addParamTree(self)    
        self.proj.setTraceManager(self.manageTraces)  
        self.setCurrentFile(None)
  
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
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = makeApplication()
    # Create and show the form
    window = ChipWhispererAnalyzer()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
