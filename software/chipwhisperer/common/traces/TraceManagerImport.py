#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

__author__ = "Colin O'Flynn"

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path

import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
from openadc.ExtendedParameter import ExtendedParameter
    
from TraceContainerNative import TraceContainerNative
from TraceContainerConfig import TraceContainerConfig
import TraceContainerTypes

from functools import partial

try:
    from traces.TraceContainerMySQL import TraceContainerMySQL
except ImportError:
    TraceContainerMySQL = None
    
class TraceManagerImport(QDialog):
    def __init__(self, parent=None):        
        super(TraceManagerImport, self).__init__(parent)
        self.parent = parent

        layout = QVBoxLayout()

        # Set dialog layout
        self.setLayout(layout)
        self.setWindowTitle("Add Existing Trace")

        self.openCfg = QPushButton("Load .cfg File")
        self.openCfg.clicked.connect(self.loadCfg)

        layout.addWidget(self.openCfg)
        
        self.modName = QComboBox()
        self.modName.addItem("Select format for manual mode only...")
        self.modName.addItem("ChipWhisperer/Native")
        self.modName.addItem("DPAContestv3")
        if TraceContainerMySQL is not None:        
            self.modName.addItem("MySQL", TraceContainerMySQL)
        self.modName.currentIndexChanged.connect(self.traceTypeChanged)
        
        layout.addWidget(self.modName)

        self.paramTree = ParameterTree()
        layout.addWidget(self.paramTree)

        buts = QHBoxLayout()
        cancel = QPushButton("Screw This")
        cancel.clicked.connect(self.abort)
        ok = QPushButton("Import")
        ok.clicked.connect(self.accept)
        buts.addWidget(ok)
        buts.addWidget(cancel)
        
        layout.addLayout(buts)
        
        self.tmanager = None
        
    def abort(self):
        self.tmanager = None
        self.reject()        

    def traceTypeChanged(self, newindx):
        self.openCfg.setEnabled(False)
        newTT = self.modName.itemData(newindx)        
        self.tmanagerParams = newTT.getParamsClass(openMode=True)   
        self.tmanager = newTT(self.tmanagerParams)
        ExtendedParameter.reloadParams(self.tmanagerParams.paramList(), self.paramTree)
        
    def getTrace(self):
        return self.tmanager
    
    def updateConfigData(self):
        if self.tmanager is not None:
            self.tmanager.updateConfigData()
            
    def loadCfg(self, fname=None):
	if fname is None:
	        fname, _ = QFileDialog.getOpenFileName(self, 'Open file',QSettings().value("trace_last_file"),'*.cfg')

        if fname:
            QSettings().setValue("trace_last_file", fname)

        if fname:
            self.modName.setEnabled(False)
            
            #Figure out what format this is in
            tc = TraceContainerConfig(fname)
            fmt = tc.attr("format")
            
            #Generate a temp class for getting parameters from
            fmtclass = TraceContainerTypes.TraceContainerFormatList[fmt]
            
            #Use temp class to finally initilize our "good" version
            self.tmanager = fmtclass( fmtclass.getParamsClass(openMode=True) )
            self.tmanager.config.loadTrace(fname)
            self.tmanager.loadAllConfig()
            ExtendedParameter.reloadParams(self.tmanager.getParams.paramList(), self.paramTree)
            
            
