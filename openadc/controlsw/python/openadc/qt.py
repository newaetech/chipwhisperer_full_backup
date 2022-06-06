# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the 2-Clause BSD License. See LICENSE
# file which should have came with this code.


#Always import PySide FIRST to force everything to use this, in case both PySide & PyQt installed
from PySide.QtCore import *
from PySide.QtGui import *


import openadc
from ExtendedParameter import ExtendedParameter

try:
    import pyqtgraph as pg
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "*************************************************"
    print "*************************************************"
    print "Install pyqtgraph from http://www.pyqtgraph.org"
    print "*************************************************"
    print "*************************************************"

class previewWindow():
    def __init__(self):
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')

        self.pw = pg.PlotWidget(name="Trace Preview")
        self.pw.setLabel('bottom', 'Sample Number')
        self.pw.setLabel('left', 'Value')
        vb = self.pw.getPlotItem().getViewBox()
        vb.setMouseMode(vb.RectMode)

        self.dock = QDockWidget("Trace Preview")
        self.dock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.dock.setWidget(self.pw)

        self.settings = QGroupBox("Trace Preview Settings");
        layout = QGridLayout()
        self.settings.setLayout(layout)

        self.persistant = QCheckBox("Persistance")
        clearPB = QPushButton("Clear")
        clearPB.clicked.connect(self.pw.clear)
        layout.addWidget(self.persistant, 0, 0)
        layout.addWidget(clearPB, 0, 1)

        self.colour = QSpinBox()
        self.colour.setMinimum(0)
        self.colour.setMaximum(7)
        self.autocolour = QCheckBox("Auto Colour")
        layout.addWidget(QLabel("Colour"), 1, 0)
        layout.addWidget(self.colour, 1, 1)
        layout.addWidget(self.autocolour, 1, 2)

    def addDock(self, mainWindow):
        '''Pass the main window, adds the dock for you'''
        mainWindow.addDockWidget(Qt.BottomDockWidgetArea,self.dock)

    def getSetupWidget(self):
        '''Returns the setup widget for the preview graph'''
        return self.settings

    def updateData(self, data, start=0):
        if self.persistant.isChecked():
            if self.autocolour.isChecked():
                nc = (self.colour.value() + 1) % 8
                self.colour.setValue(nc)
        else:
            self.pw.clear()

        xaxis = range(start, len(data)+start)

        self.pw.plot(xaxis, data, pen=(self.colour.value(),8))

    def hideGraph(self):
        self.dock.close()


class OpenADCQt(QObject):

    dataUpdated = Signal(list, int)

    def __init__(self, MainWindow=None, includePreview=True, setupLayout=True, includeParameters=True, console=None, showScriptParameter=None):
        super(OpenADCQt,  self).__init__()
        self.console = console
        self.offset = 0.5
        self.ser = None
        self.sc = None

        self.datapoints = []

        self.preview = None
        self.adc_settings = None

        self.timerStatusRefresh = QTimer(self)
        self.timerStatusRefresh.timeout.connect(self.statusRefresh)
        self.showScriptParameter = showScriptParameter

        if setupLayout:
            self.setupLayout(MainWindow, includePreview, includeParameters)

    def setEnabled(self, enabled):
        pass

    def statusRefresh(self):
        pass

    def setupLayout(self, MainWindow, includePreview=True,  includeParameters=True):

        vlayout = QVBoxLayout()
        self.tb = QToolBox()
        vlayout.addWidget(self.tb)

        ###### Graphical Preview Window
        if includePreview:
            self.preview = previewWindow()
            self.preview.addDock(MainWindow)
            vlayout.addWidget(self.preview.getSetupWidget())

        if includeParameters:
            self.setupParameterTree()
            self.paramDock = QDockWidget("Settings")
            self.paramDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
            self.paramDock.setWidget(self.paramTree)
            MainWindow.addDockWidget(Qt.LeftDockWidgetArea, self.paramDock)

        self.masterLayout = vlayout

        self.setEnabled(False)

    def getLayout(self):
        return self.masterLayout

    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setupParameterTree(self, makeTree=True):
        if self.adc_settings is None:
            self.adc_settings = openadc.OpenADCSettings(self.console)
            self.params = Parameter.create(name='OpenADC', type='group', children=self.adc_settings.parameters(doUpdate=False))
            #ExtendedParameter.setupExtended(self.params)
            ep = ExtendedParameter()
            ep.setupExtended(self.params, self)

            #todo: this is a somewhat insane way to cut through the layers
            self.adc_settings.setFindParam(self.findParam)

            if makeTree:
                self.paramTree = ParameterTree()
                self.paramTree.setParameters(self.params, showTop=False)


    def reloadParameterTree(self):
        self.adc_settings.setInterface(self.sc)
        self.params.blockTreeChangeSignal()
        self.params.getAllParameters()
        self.params.unblockTreeChangeSignal()

    def processData(self, data):
        fpData = []

        #lastpt = -100;

        if data[0] != 0xAC:
            self.showMessage("Unexpected sync byte: 0x%x"%data[0])
            return None

        for i in range(2, len(data)-1, 2):
            if (0x80 & data[i + 1]) or ((0x80 & data[i + 0]) == 0):
                self.statusBar().showMessage("Error at byte %d"%i)
                print("Bytes: %x %x"%(data[i], data[i+1]))
                return None

            #Convert
            intpt = data[i + 1] | ((data[i+0] & 0x07) << 7)

            #input validation test: uncomment following and use
            #ramp input on FPGA
            ##if (intpt != lastpt + 1) and (lastpt != 0x3ff):
            ##    print "intpt: %x lstpt %x\n"%(intpt, lastpt)
            ##lastpt = intpt;

            fpData.append(float(intpt) / 1024.0 - self.offset)

        return fpData

    def arm(self):
        self.sc.arm()

    def read(self, update=True, NumberPoints=None):
        if NumberPoints == None:
            NumberPoints = self.adc_settings.parm_trigger.maxSamples()

        progress = QProgressDialog("Reading", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)

        try:
            self.datapoints = self.sc.readData(NumberPoints, progress)
        except IndexError, e:
            raise IOError("Error reading data: %s"%str(e))

        self.dataUpdated.emit(self.datapoints, -self.adc_settings.parm_trigger.presamples(True))

        if update & (self.preview is not None):
            self.preview.updateData(self.datapoints, -self.adc_settings.parm_trigger.presamples(True))


    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        timeout = self.sc.capture(waitingCallback=waitingCallback)
        self.read(update, NumberPoints)
        return timeout

    def reset(self):
        self.sc.setReset(True)
        self.reloadParameterTree()

    def test(self):
        self.sc.testAndTime()

    def con(self, ser):
        self.ser = ser

        #See if device seems to be attached
        self.sc = openadc.OpenADCInterface(self.ser, self.console)

        deviceFound = False
        numTries = 0

        #Try a few times
        while(deviceFound == False):

            if self.sc.devicePresent():
                deviceFound = True
                break

            numTries += 1

            if (numTries == 5):
                try:
                    portname = self.ser.name
                except:
                    portname = "UNKNOWN"
                self.ser.close()
                self.ser = None

                raise IOError("Opened port %s but failed to find OpenADC"%portname)
                #self.statusBar().showMessage("Failed to received response from USB Device")

#        self.statusBar().showMessage("Connected to ADC Module on port %s" % self.ser.portstr)

        self.reloadParameterTree()
        self.setEnabled(True)

    def close(self):
        if self.ser:
            self.ser.close()
            self.ser = None

    def closeAndHide(self):
        self.close()
        if self.preview:
            self.preview.hideGraph()

    def __del__(self):
        self.close()


