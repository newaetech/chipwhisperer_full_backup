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

from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.qtFixes as qtFixes

imagePath = '../common/images/'

class AttackProgressDialog(qtFixes.QDialog):
    def __init__(self, parent=None):
        super(AttackProgressDialog, self).__init__(parent)
        self.min = 0
        self.max = 10
        self.abort = False
        self.skip = False
        
        #Qt.WindowCloseButtonHint | 
        self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint | Qt.WindowStaysOnTopHint) & ~(Qt.WindowContextHelpButtonHint))
        self.setWindowTitle("Analysis in Progress")
        self.setWindowIcon(QIcon(imagePath+"attack_transp.png"))
        
        layout = QVBoxLayout()
        clayout = QHBoxLayout()
        skip = QPushButton("Next Byte")
        self.skipPB = skip
        cancel = QPushButton("Abort")
        cancel.clicked.connect(self.setCanceled)
        skip.clicked.connect(self.setSkipped)
        clayout.addStretch()
        clayout.addWidget(skip)
        clayout.addWidget(cancel)        
        self.pbar = QProgressBar()
        
        statusInfo = QVBoxLayout()
        self.byteNum = QLabel("Current Subkey = ?")
        self.traceNum = QLabel("Current Traces = ?") 
        statusInfo.addWidget(self.byteNum)
        statusInfo.addWidget(self.traceNum)       
        
        layout.addLayout(statusInfo)        
        layout.addWidget(self.pbar)
        layout.addLayout(clayout)
        
        self.setLayout(layout)
        
        self.skipPB.setVisible(False)
        
        self.offset = 0
        
    def updateStatus(self, trace, byte):
        self.byteNum.setText("Current Subkey = %d"%byte)
        try:
            self.traceNum.setText("Current Trace = %d-%d"%(trace[0]+self.offset, trace[1]+self.offset))
        except:    
            self.traceNum.setText("Current Trace = %d"%(trace+self.offset))        
        
    def supportSkipByte(self, sup):
        self.skipPB.setVisible(sup)
        
    def setMinimumDuration(self, duration):
        pass
        
    def setMinimum(self, mv):
        self.pbar.setMinimum(mv)
    
    def setMaximum(self, mv):
        self.max = mv
        self.pbar.setMaximum(mv)
    
    def setValue(self, val):
        if self.isVisible() == False:
            self.show()
        
        self.pbar.setValue(val)
        if val == self.max:
            self.close()
            
        QApplication.processEvents()
    
    def setCanceled(self):
        self.abort = True
        
    def wasCanceled(self):
        return self.abort
        
    def setSkipped(self):
        self.skip = True
    
    def clearSkipped(self):
        self.skip = False
    
    def wasSkipped(self):        
        return self.skip
