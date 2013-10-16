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

__author__ = "Colin O'Flynn"

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')

import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
from ExtendedParameter import ExtendedParameter
    
from TraceContainerNative import TraceContainerNative
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

        self.modName = QComboBox()
        self.modName.addItem("ChipWhisperer/Native")
        self.modName.addItem("DPAContestv3")
        if TraceContainerMySQL is not None:        
            self.modName.addItem("MySQL", TraceContainerMySQL)
        self.modName.currentIndexChanged.connect(self.traceTypeChanged)
        
        layout.addWidget(self.modName)
        
        
        #self.params = Parameter.create(name='Generic Settings', type='group', children=[])
        #ExtendedParameter.setupExtended(self.params)
        self.paramTree = ParameterTree()
        #self.paramTree.setParameters(self.params, showTop=False)
        
        layout.addWidget(self.paramTree)
        

    def traceTypeChanged(self, newindx):
        newTT = self.modName.itemData(newindx)        
        self.tmanagerParams = newTT.getParams(openMode=True)   
        self.tmanager = newTT(self.tmanagerParams)
        ExtendedParameter.reloadParams(self.tmanagerParams.paramList(), self.paramTree)
        
    def getTrace(self):
        return self.tmanager
    
    def updateConfigData(self):
        if self.tmanager is not None:
            self.tmanager.updateConfigData()

