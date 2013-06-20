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
#

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

try:
    import openadc_qt
except ImportError:
    print "ERROR: openadc_qt is required for this program"
    sys.exit()

class PreviewTab(QWidget):
    def __init__(self):
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')

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
        #setupLayout.addLayout(hl)
        

        #hl = QHBoxLayout()
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
        #setupLayout.addLayout(hl)

        #hl = QHBoxLayout()
        hl.addWidget(QLabel("Points: "))
        hl.addWidget(self.startPointPrint, )
        hl.addWidget(QLabel(" to "))
        hl.addWidget(self.endPointPrint)
        hl.addStretch()
        setupLayout.addLayout(hl)
        
        layout.addWidget(setupGB)

        #Setup plot widget
        self.pw = pg.PlotWidget(name="Power Trace View")
        self.pw.setLabel('top', 'Power Trace View')
        self.pw.setLabel('bottom', 'Samples')
        self.pw.setLabel('left', 'Data')
        vb = self.pw.getPlotItem().getViewBox()
        vb.setMouseMode(vb.RectMode)
        
        layout.addWidget(self.pw)        
        self.setLayout(layout)

    def passTrace(self, trace):
        return   

    def redrawPushed(self):
        return

    def clearPushed(self):
        self.pw.clear()
    

class SettingsTree(QTreeWidget):
    def __init__(self, parent=None):
        super(SettingsTree, self).__init__(parent)

        self.widgetList = []

        #self.setItemDelegate(VariantDelegate(self))

        self.setHeaderLabels(["Setting", "Summary"])
        self.header().setResizeMode(0, QHeaderView.Stretch)
        self.header().setResizeMode(1, QHeaderView.Stretch)

        self.groupIcon = QIcon()
        self.groupIcon.addPixmap(self.style().standardPixmap(QStyle.SP_DirClosedIcon),
                QIcon.Normal, QIcon.Off)
        self.groupIcon.addPixmap(self.style().standardPixmap(QStyle.SP_DirOpenIcon),
                QIcon.Normal, QIcon.On)
        self.keyIcon = QIcon()
        self.keyIcon.addPixmap(self.style().standardPixmap(QStyle.SP_FileIcon))

    def addItem(self, text, parent, index=0, widget=None):
        after = None

        if index != 0:
            after = self.childAt(parent, index - 1)

        if parent is not None:
            item = QTreeWidgetItem(parent, after)
        else:
            item = QTreeWidgetItem(self, after)

        item.setText(0, text)
        item.setFlags(item.flags()) # | Qt.ItemIsEditable

        item.setData(0, Qt.UserRole, widget)
        return item

class CWSettings(QObject):

    widgetChanged = Signal(QWidget)
    
    def __init__(self, parent=None):
        super(CWSettings, self).__init__()
        self.st = SettingsTree(parent)
        self.st.itemSelectionChanged.connect(self.itemChanged)
        self.widgetList = []

    def getWidget(self):
        return self.st

    def addGroup(self, groupname, widget=None):
        #Some error, cannot set the data to a widget, or the
        #system crashes. Instead use a reference & keep a list
        #of all widgets.
        self.widgetList.append(widget)
        return self.st.addItem(groupname, None, widget=(len(self.widgetList)-1))

    def addGroupItem(self, group, itemname, itemwidget=None):
        #Some error, cannot set the data to a widget, or the
        #system crashes. Instead use a reference & keep a list
        #of all widgets.
        self.widgetList.append(itemwidget)
        return self.st.addItem(itemname, parent=group, widget=(len(self.widgetList)-1))

    def itemChanged(self):
        itm = self.st.currentItem()
        itmWidget = itm.data(0, Qt.UserRole)
        itmWidget=self.widgetList[itmWidget]
        if itmWidget != None:
            self.widgetChanged.emit(itmWidget)
            

class OpenADCInterface():
    def __init__(self):
        self.qtadc = openadc_qt.OpenADCQt(setupLayout=False)
        self.qtadc.setupWidgets()

    def loadSettings(self, settings):
        oaset = settings.addGroup("OpenADC Settings")
        settings.addGroupItem(oaset, "Gain", self.qtadc.gainWidget)
        settings.addGroupItem(oaset, "Trigger", self.qtadc.triggerWidget)
        settings.addGroupItem(oaset, "Samples", self.qtadc.samplesWidget)
        settings.addGroupItem(oaset, "Clock", self.qtadc.clockWidget)
         
class MainWindow(QMainWindow):           
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self.da = None
        self.key = None
       
        self.statusBar()
        self.setWindowTitle("Chip Whisperer: Capturev2")
        self.setWindowIcon(QIcon("../common/cwicon.png"))

        # Create layout and add widgets
        self.mw = QWidget()
        
        layout = QVBoxLayout()

        self.settings = CWSettings()
        layout.addWidget(self.settings.getWidget())
        self.initSettings()

        #layout.addStretch()
             
        # Set dialog layout
        self.setLayout(layout)       
        self.mw.setLayout(layout)
        self.setCentralWidget(self.mw)


        dock = QDockWidget("Channel 1", self)
        dock.setAllowedAreas(Qt.RightDockWidgetArea)
        dock.setWidget(PreviewTab())
        self.addDockWidget(Qt.RightDockWidgetArea, dock)

        self.configdock = QDockWidget("Config Options", self)
        self.configdock.setAllowedAreas(Qt.RightDockWidgetArea|Qt.BottomDockWidgetArea)
        self.addDockWidget(Qt.RightDockWidgetArea, self.configdock)

        self.addToolbars()

    def initSettings(self):
        #ChipWhisperer-Capture Settings
        gg = self.settings.addGroup("General Settings", None)

        #OpenADC Settings
        self.scope = OpenADCInterface()
        self.scope.loadSettings(self.settings)        

        self.settings.widgetChanged.connect(self.setConfigWidget)

    def setConfigWidget(self, widget):
        self.configdock.setWidget(widget)

    def addToolbars(self):
        self.addGraphTools()

    def addGraphTools(self):
        xLockedAction = QAction(QIcon('images/xlock.png'), 'Lock all X Axis', self)
        #xLockedAction.triggered.connect(self.xLocked)
        yAutoScale = QAction(QIcon('images/yauto.png'), 'Autoscale Y Axis', self)
        xAutoScale = QAction(QIcon('images/xauto.png'), 'Autoscale X Axis', self)
        
        self.toolbar = self.addToolBar('Graph Tools')
        self.toolbar.addAction(xLockedAction)
        self.toolbar.addAction(xAutoScale)
        self.toolbar.addAction(yAutoScale)


    def closeEvent(self, event):
        return
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
