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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
try:
    import pyqtgraph as pg
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

class ColorDialog(QDialog):
    def __init__(self, color,  auto):
        super(ColorDialog, self).__init__()
        layout = QVBoxLayout()
        self.setLayout(layout)
                
        self.cbAuto = QCheckBox("Auto-Increment Persistant Colours")
        self.cbAuto.setChecked(auto)
        
        layout.addWidget(self.cbAuto)
        
        clayout = QHBoxLayout()
        self.cbColor = QComboBox()
        self.cbColor.addItem("Red",  0)
        self.cbColor.addItem("Yellow",  1)
        self.cbColor.addItem("Chartreuse",  2)
        self.cbColor.addItem("Green",  3)
        self.cbColor.addItem("Cyan",  4)
        self.cbColor.addItem("Blue",  5)
        self.cbColor.addItem("Purple",  6)
        self.cbColor.addItem("Magenta",  7)          
        self.cbColor.currentIndexChanged.connect(self.currentIndexChanged)
        self.cbColor.setCurrentIndex(color)
        
        clayout.addWidget(QLabel("Color: "))
        clayout.addWidget(self.cbColor)
        clayout.addStretch()
        self.color = color        
        
        layout.addLayout(clayout)
        
        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)
        layout.addWidget(buttonBox)
        
    def currentIndexChanged(self, indx):
        self.color = self.cbColor.itemData(indx)
        
    def getValues(self):
        return (self.color,  self.cbAuto.isChecked())

class GraphWidget(QWidget):
    def __init__(self, imagepath='images/'):
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.imagepath = imagepath

        QWidget.__init__(self)
        layout = QVBoxLayout()

        setupGB = QGroupBox("View Options")
        setupLayout = QVBoxLayout()
        setupGB.setLayout(setupLayout)

        hl = QHBoxLayout()
        pbRedraw = QPushButton("Redraw")
        pbRedraw.clicked.connect(self.redrawPushed)
        hl.addWidget(pbRedraw)

        pbClear = QPushButton("Clear All")
        pbClear.clicked.connect(self.clearPushed)
        hl.addWidget(pbClear)
        hl.addStretch()
    
        self.startTracePrint = QSpinBox()        
        self.startTracePrint.setMinimum(0)
        self.endTracePrint = QSpinBox()        
        self.endTracePrint.setMinimum(0)
        self.startPointPrint = QSpinBox()        
        self.startPointPrint.setMinimum(0)
        self.endPointPrint = QSpinBox()        
        self.endPointPrint.setMinimum(0)

        hl.addWidget(QLabel("Traces: "))
        hl.addWidget(self.startTracePrint)
        hl.addWidget(QLabel(" to "))
        hl.addWidget(self.endTracePrint)
        hl.addStretch()

        hl.addWidget(QLabel("Points: "))
        hl.addWidget(self.startPointPrint, )
        hl.addWidget(QLabel(" to "))
        hl.addWidget(self.endPointPrint)
        hl.addStretch()
        setupLayout.addLayout(hl)
        
        layout.addWidget(setupGB)

        self.pw = pg.PlotWidget(name="Power Trace View")
        self.pw.setLabel('top', 'Power Trace View')
        self.pw.setLabel('bottom', 'Samples')
        self.pw.setLabel('left', 'Data')
        vb = self.pw.getPlotItem().getViewBox()
        vb.setMouseMode(vb.RectMode)
        
        ###Toolbar
        xLockedAction = QAction(QIcon(self.imagepath+'xlock.png'), 'Lock X Axis', self)
        xLockedAction.setCheckable(True)
        xLockedAction.triggered[bool].connect(self.xLocked)

        yLockedAction = QAction(QIcon(self.imagepath+'ylock.png'), 'Lock Y Axis', self)
        yLockedAction.setCheckable(True)
        xLockedAction.triggered[bool].connect(self.yLocked)

        yAutoScale = QAction(QIcon(self.imagepath+'yauto.png'), 'Autoscale Y Axis', self)
        yAutoScale.triggered[bool].connect(self.yAutoScale)
        xAutoScale = QAction(QIcon(self.imagepath+'xauto.png'), 'Autoscale X Axis', self)
        xAutoScale.triggered[bool].connect(self.xAutoScale)
        
        persistance = QAction(QIcon(self.imagepath+'persistance.png'), 'Enable Persistance',  self)
        persistance.setCheckable(True)
        persistance.triggered[bool].connect(self.setPersistance)
        
        setColour = QAction(QIcon(self.imagepath+'wavecol.png'),  'Set Colour',  self)
        setColour.triggered[bool].connect(self.setColour)
        
        self.GraphToolbar = QToolBar('Graph Tools')
        self.GraphToolbar.addAction(xLockedAction)
        self.GraphToolbar.addAction(yLockedAction)
        self.GraphToolbar.addAction(xAutoScale)
        self.GraphToolbar.addAction(yAutoScale)
        self.GraphToolbar.addAction(persistance)
        self.GraphToolbar.addAction(setColour)
        layout.addWidget(self.GraphToolbar)
        
        layout.addWidget(self.pw)        
        self.setLayout(layout)
        
        self.persistant = False
        self.color = 0
        self.autocolor = True

    def setPersistance(self, enabled):
        self.persistant = enabled
        
    def setColour(self, enabled):
        cd = ColorDialog(self.color, self.autocolor)
        if cd.exec_():        
            data = cd.getValues()
            self.color = data[0]
            self.acolor = data[0]
            self.autocolor = data[1]
        
    def yAutoScale(self, enabled):
        pass
        
    def xAutoScale(self, enabled):
        pass
        
    def yLocked(self, enabled):
        pass
        
    def xLocked(self, enabled):
        pass

    def passTrace(self, trace):
        if self.persistant:
            if self.autocolor:
                nc = (self.acolor + 1) % 8
                self.acolor = nc
            else:
                self.acolor = self.color
        else:
            self.acolor = self.color
            self.pw.clear()
        self.pw.plot(trace, pen=(self.acolor,8)) 

    def redrawPushed(self):
        return

    def clearPushed(self):
        self.pw.clear()