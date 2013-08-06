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
from TraceFormatNative import TraceFormatNative

class MainWindow(MainChip):
    MaxRecentFiles = 4    
    def __init__(self):
        super(MainWindow, self).__init__(name="ChipWhisperer Analyzer V2", imagepath=imagePath)
        self.console = self.addConsole()
    
        
        self.cwParams = [            
                ]
        
        self.da = None
        self.numTraces = 100

        #self.addToolbars()
        self.addSettingsDocks()
        self.addWaveforms()
        
        self.restoreDockGeometry()
        self.newProject()   
        
        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)

        
    def addWaveforms(self):
        self.waveformDock = self.addTraceDock("Waveform Display")        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault()       
        
    def addSettingsDocks(self):      
        self.setupParametersTree()        
        self.settingsNormalDock = self.addSettings(self.paramTree, "General Settings")

    def setupParametersTree(self):
        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)
        
    def reloadParamList(self, lst=None):
        self.paramTree.clear()                             
        for p in self.paramList(): self.paramTree.addParameters(p)           
        
        
    def paramList(self):
        p = []
        p.append(self.params)     
        return p        

  
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
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Analyzer V2")
    
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
