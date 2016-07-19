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
import logging

from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes
import pyqtgraph as pg


class ColorDialog(QtFixes.QDialog):
    """
    Simple dialog to pick colours for the trace data.
    """
    def __init__(self, colorInt=None, auto=None):
        super(ColorDialog, self).__init__()
        layout = QVBoxLayout()
        self.setLayout(layout)
                
        if colorInt is None:
            colorInt = 0

        if auto is None:
            auto = True

        self.cbAuto = QCheckBox("Auto-Increment Persistent Colours")
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
        self.cbColor.setCurrentIndex(colorInt)
        
        clayout.addWidget(QLabel("Color: "))
        clayout.addWidget(self.cbColor)
        clayout.addStretch()

        self.colorInt = colorInt
        
        layout.addLayout(clayout)
        
        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)
        layout.addWidget(buttonBox)
        
    def currentIndexChanged(self, indx):
        self.colorInt = self.cbColor.itemData(indx)
        
    def getValues(self):
        return (self.colorInt, self.cbAuto.isChecked())


class GraphWidget(QWidget):
    """
    This GraphWidget holds a pyqtgraph PlotWidget, and adds a toolbar for the user to control it.
    """    
    
    xRangeChanged = Signal(int, int)
    dataChanged = Signal(list, int)

    def __init__(self):
        QWidget.__init__(self)
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        
        self.imagepath = ":/images/"
        self.selectedTrace = None
        self.selectedTraceId = None

        #Ghost trace items
        self.lastStartOffset = 0
        self.lastTraceData = []

        self.persistantItems = []
        self._customWidgets = []

        self.colorDialog = ColorDialog()

        self.pw = pg.PlotWidget(name="Power Trace View")
        # self.pw.setTitle(title= 'Power Trace View')
        self.pw.setLabel('top', 'Power Trace View')
        self.pw.getAxis('top').enableAutoSIPrefix(enable=False)
        self.pw.getAxis('top').setStyle(showValues=False)
        self.pw.setLabel('bottom', 'Samples')
        self.pw.setLabel('left', 'Data')
        self.pw.getPlotItem().setContentsMargins(5,5,10,1)
        vb = self.pw.getPlotItem().getViewBox()
        vb.setMouseMode(vb.RectMode)
        vb.sigStateChanged.connect(self.VBStateChanged)
        vb.sigXRangeChanged.connect(self.VBXRangeChanged)

        self.proxysig = pg.SignalProxy(self.pw.plotItem.vb.scene().sigMouseMoved, rateLimit=10, slot=self.mouseMoved)

        self.vLine = pg.InfiniteLine(angle=90, movable=False)
        self.hLine = pg.InfiniteLine(angle=0, movable=False)
        vb.addItem(self.vLine, ignoreBounds=True)
        vb.addItem(self.hLine, ignoreBounds=True)

        ###Toolbar
        xLockedAction = QAction(QIcon(self.imagepath+'xlock.png'), 'Lock X Axis', self)
        xLockedAction.setCheckable(True)
        xLockedAction.triggered[bool].connect(self.xLocked)
        self.XLockedAction = xLockedAction

        yLockedAction = QAction(QIcon(self.imagepath+'ylock.png'), 'Lock Y Axis', self)
        yLockedAction.setCheckable(True)
        yLockedAction.triggered[bool].connect(self.yLocked)
        self.YLockedAction = yLockedAction

        yAutoScale = QAction(QIcon(self.imagepath+'yauto.png'), 'Autoscale Y Axis', self)
        yAutoScale.triggered[bool].connect(self.yAutoScale)
        xAutoScale = QAction(QIcon(self.imagepath+'xauto.png'), 'Autoscale X Axis', self)
        xAutoScale.triggered[bool].connect(self.xAutoScale)
        
        yDefault = QAction(QIcon(self.imagepath+'ydefault.png'), 'Default Y Axis', self)
        yDefault.triggered.connect(self.YDefault)
        
        self.actionPersistance = QAction(QIcon(self.imagepath+'persistance.png'), 'Enable Persistance',  self)
        self.actionPersistance.setCheckable(True)
        self.actionPersistance.triggered[bool].connect(self.setPersistance)
        
        setColour = QAction(QIcon(self.imagepath+'wavecol.png'),  'Set Colour',  self)
        setColour.triggered[bool].connect(self.colorPrompt)
        
        clear = QAction(QIcon(self.imagepath+'clear.png'), 'Clear Display', self)
        clear.triggered.connect(self.clearPushed)

        self.crossHair = QAction(QIcon(self.imagepath+'crosshair.png'), 'Show Crosshairs', self)
        self.crossHair.setCheckable(True)
        self.crossHair.setChecked(False)
        self.setCrossHairs(self.crossHair.isChecked())
        self.crossHair.triggered.connect(lambda: self.setCrossHairs(self.crossHair.isChecked()))

        grid = QAction(QIcon(self.imagepath+'grid.png'), 'Show Grid', self)
        grid.setCheckable(True)
        grid.triggered.connect(lambda: self.pw.showGrid(grid.isChecked(), grid.isChecked(), 0.1))

        mouseMode = QAction(QIcon(self.imagepath+'hand.png'), 'Move', self)
        mouseMode.setCheckable(True)
        mouseMode.triggered.connect(lambda: vb.setMouseMode(
            pg.ViewBox.PanMode if mouseMode.isChecked() else pg.ViewBox.RectMode))

        help = QAction(QIcon(self.imagepath+'help.png'), 'Help', self)
        help.triggered.connect(lambda: QMessageBox.information(self, "Help",
                                "Right click or check the Results Settings for more options.\n\n"
                                "Draw types:\n"
                                "- Fast: Group traces into a min/max area;\n"
                                "- Normal: Plot all traces continuously;\n"
                                "- Detailed: Plot all traces individually.\n\n"
                                "Only highlighted traces can be selected in fast/normal."))

        self.GraphToolbar = QToolBar('Graph Tools')
        self.GraphToolbar.addAction(xLockedAction)
        self.GraphToolbar.addAction(yLockedAction)
        self.GraphToolbar.addAction(xAutoScale)
        self.GraphToolbar.addAction(yAutoScale)
        self.GraphToolbar.addAction(yDefault)
        self.GraphToolbar.addAction(self.actionPersistance)
        self.GraphToolbar.addAction(setColour)
        self.GraphToolbar.addAction(clear)
        self.GraphToolbar.addAction(self.crossHair)
        self.GraphToolbar.addAction(grid)
        self.GraphToolbar.addAction(mouseMode)
        self.GraphToolbar.addAction(help)
        self.GraphToolbar.addSeparator()
        self.selection = QLabel("Selected Trace: None")
        self.GraphToolbar.addWidget(self.selection)
        self.GraphToolbar.addSeparator()
        self.pos = QLabel("Position: (-, -)")
        self.GraphToolbar.addWidget(self.pos)

        layout = QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.setSpacing(0)
        layout.addWidget(self.GraphToolbar)
        layout.addWidget(self.pw)        
        self.setLayout(layout)
        self.setDefaults()

    def setDefaults(self):
        self.setPersistance(False)
        self.color = 0
        self.acolor = self.seedColor = 0
        self.autocolor = True
        self.defaultYRange = None

    def setPersistance(self, enabled):
        """Enable Persistance mode, which means display NOT cleared before new traces added"""
        self.actionPersistance.setChecked(enabled)
        self.persistant = enabled
        
    def setColorInt(self, colorint, numcolors=16):

        # See http://www.pyqtgraph.org/documentation/functions.html#pyqtgraph.mkColor
        self.color = pg.intColor(colorint, hues=numcolors)

    def colorPrompt(self, enabled):
        """Prompt user to set colours"""

        if self.colorDialog.exec_():
            data = self.colorDialog.getValues()
            self.setColorInt(data[0], 9)
            self.acolor = self.seedColor = data[0]
            self.autocolor = data[1]
        
    def VBStateChanged(self, obj):
        """Called when ViewBox state changes, used to sync X/Y AutoScale buttons"""
        arStatus = self.pw.getPlotItem().getViewBox().autoRangeEnabled()
        
        #X Axis
        if arStatus[0]:
            self.XLockedAction.setChecked(False)
        else:
            self.XLockedAction.setChecked(True)            
            
        #Y Axis
        if arStatus[1]:
            self.YLockedAction.setChecked(False)
        else:
            self.YLockedAction.setChecked(True) 
            
    def VBXRangeChanged(self, vb, range):
        """Called when X-Range changed"""
        self.xRangeChanged.emit(range[0], range[1])
        
    def xRange(self):
        """Returns the X-Range"""
        return self.pw.getPlotItem().getViewBox().viewRange()[0]
            
    def YDefault(self, extraarg=None):
        """Copy default Y range axis to active view"""
        if self.defaultYRange is not None:
            self.setYRange(self.defaultYRange[0], self.defaultYRange[1])
          
    def setDefaultYRange(self, lower, upper):
        """Set default Y-Axis range, for when user clicks default button"""
        self.defaultYRange = [lower, upper]
          
    def setXRange(self, lower, upper):
        """Set the X Axis to extend from lower to upper"""
        self.pw.getPlotItem().getViewBox().setXRange(lower, upper)
        
    def setYRange(self, lower, upper):
        """Set the Y Axis to extend from lower to upper"""
        self.pw.getPlotItem().getViewBox().setYRange(lower, upper)
          
    def xAutoScale(self, enabled):
        """Auto-fit X axis to data"""
        vb = self.pw.getPlotItem().getViewBox()
        bounds = vb.childrenBoundingRect(None)
        # print bounds
        vb.setXRange(bounds.left(), bounds.right())
        
    def yAutoScale(self, enabled):
        """Auto-fit Y axis to data"""
        vb = self.pw.getPlotItem().getViewBox()
        bounds = vb.childrenBoundingRect(None)
        vb.setYRange(bounds.top(), bounds.bottom())
        
    def xLocked(self, enabled):
        """Lock X axis, such it doesn't change with new data"""
        self.pw.getPlotItem().getViewBox().enableAutoRange(pg.ViewBox.XAxis, not enabled)
        
    def yLocked(self, enabled):
        """Lock Y axis, such it doesn't change with new data"""
        self.pw.getPlotItem().getViewBox().enableAutoRange(pg.ViewBox.YAxis, not enabled)
        
    def passTrace(self, trace, startoffset=0, ghostTrace=False, pen=None, idString = "", xaxis=None):
        """Plot a new trace, where X-Axis is simply 'sample number' (e.g. integer counting 0,1,2,...N-1).
        
        :param startoffset: Offset of X-Axis, such that zero point is marked as this number
        :type startoffset: int
        
        :param ghostTrace: By default the last plotted trace is stored for use with stuff such as an overlay that
                           selects data off the graph. If ghostTrace is set to 'true' the passed data is NOT stored.
        :type ghostTrace: bool
        """

        if ghostTrace is False:
            self.lastTraceData = trace
            self.lastStartOffset = startoffset

        if self.persistant:
            if self.autocolor:
                nc = (self.acolor + 1) % 8
                self.acolor = nc
            else:
                self.acolor = self.color
        else:
            self.acolor = self.color
            self.pw.clear()
            
        if xaxis is None:
            xaxis = range(startoffset, len(trace)+startoffset)

        if pen is None:
            pen = pg.mkPen(self.acolor)

        p = self.pw.plot(x=xaxis, y=trace, pen=pen)
        self.setupPlot(p, 0, True, idString)

        if ghostTrace is False:
            self.dataChanged.emit(trace, startoffset)

        # TODO: This was commented out, why?
        self.checkPersistantItems()
        return p

    def clearPushed(self):
        """Clear display"""
        self.pw.clear()
        self.checkPersistantItems()
        self.acolor = self.seedColor

    def addPersistantItem(self, item):
        self.persistantItems.append(item)
        self.checkPersistantItems()

    def checkPersistantItems(self):
        for t in self.persistantItems:
            if t not in self.pw.items():
                self.pw.addItem(t)

    def addWidget(self, widget):
        self._customWidgets.append(widget)
        self.layout().addWidget(widget)

    def clearCustomWidgets(self):
        for wid in self._customWidgets:
            wid.setVisible(False)
            self.layout().removeWidget(wid)
            del wid
        self._customWidgets = []

    def setLabels(self, top=None, xaxis=None, yaxis=None):
        if top:
            self.pw.setLabel('top', top)

        if xaxis:
            self.pw.setLabel('bottom', xaxis)

        if yaxis:
            self.pw.setLabel('left', yaxis)

    def setCrossHairs(self, enabled):
        self.vLine.setVisible(enabled)
        self.hLine.setVisible(enabled)
        self.crossHair.setChecked(enabled)

    def selectTrace(self, trace):
        if self.selectedTrace:
            self.selectedTrace.setShadowPen(None)
        if self.selectedTrace == trace:  # Deselects if the trace was already selected
            self.selectedTrace = None
            self.selection.setText("Selected Trace: None")
        else:
            self.selectedTrace = trace
            if self.selectedTrace.xData.size > 25000:
                logging.warning("Trace highlighting (shadow pen) disabled: Trace is too large (>25k points).")
            else:
                self.selectedTrace.setShadowPen(pg.mkPen(0.5, width=2, style=Qt.SolidLine))
            self.selection.setText("Selected Trace: %s" % (self.selectedTrace.id if hasattr(self.selectedTrace, "id") else ""))

    def mouseMoved(self, evt):
        mousePoint = evt[0]
        if self.pw.plotItem.vb.sceneBoundingRect().contains(mousePoint):
            pos = self.pw.plotItem.vb.mapSceneToView(mousePoint)
            self.pos.setText("Position: (%f, %f)" % (pos.x(), pos.y()))
            if self.vLine.isVisible():
                self.vLine.setPos(pos.x())
                self.hLine.setPos(pos.y())

    def setupPlot(self, plot, zOrdering, clickable, id):
        plot.setZValue(zOrdering)
        plot.id = id
        if clickable:
            plot.curve.setClickable(clickable)
            plot.sigClicked.connect(self.selectTrace)
        return plot
