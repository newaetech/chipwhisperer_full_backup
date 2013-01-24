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
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
import os
import threading
import time
import logging
import math
os.environ["QT_API"] = "pyside"
import matplotlib
matplotlib.use("Qt4Agg")
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PySide.QtCore import *
from PySide.QtGui import *
import serial
import random

sys.path.append('../../openadc/controlsw/python/common')
import scan

try:
    import ftd2xx as ft
except ImportError:
    ft = None
    ft_str = sys.exc_info()
    print ft_str
    
try:
    import openadc_qt
except ImportError:
    openadc_qt = None
    openadc_qt_str = sys.exc_info()
    print openadc_qt_str

try:
    import target_smartcard
except ImportError:
    target_smartcard = None
    target_smartcard_str = sys.exc_info()
    print target_smartcard_str

try:
    import target_simpleserial
except ImportError:
    target_simpleserial = None
    target_simpleserial_str = sys.exc_info()
    print target_simpleserial_str

try:
    import target_sasebogii
except ImportError:
    target_sasebogii = None
    target_sasebogii_str = sys.exc_info()
    print target_sasebogii_str

try:
    import target_smartcardserial
except ImportError:
    target_smartcardserial = None
    target_smartcardserial_str = sys.exc_info()
    print target_smartcardserial_sasebow_str

try:
    import target_sasebow
except ImportError:
    target_sasebow = None
    target_sasebow_str = sys.exc_info()
    print target_sasebow_str

try:
    import writer_dpav3
except ImportError:
    writer_dpav3 = None
    writer_dpav3_str = sys.exc_info()
    print writer_dpav3_str

try:
    import target_sasebow_integrated
except ImportError:
    target_sasebow_integrated = None
    target_sasebow_integrated_str = sys.exc_info()
    print target_sasebow_integrated_str


class pysideGraph():
    def __init__(self, name="", xmin=0, xmax=1, ymin=-1.0, ymax=1.0, xfigsize=600, yfigsize=600):
        self.gb = QGroupBox("Results Preview")
        self.fig = Figure(figsize=(xfigsize,yfigsize), dpi=72)
        self.fig_ax =  self.fig.add_subplot(111)
        self.fig_ax.plot([0,0])
        clocklayout = QVBoxLayout()
        canvas = FigureCanvas(self.fig)
        layout = QVBoxLayout()
        layout.addWidget(canvas)
        settingsLayout = QGridLayout()
      
        self.xmin = QSpinBox()
        self.xmin.setMinimum(xmin)
        self.xmin.setMaximum(xmax)
        self.xmax = QSpinBox()
        self.xmax.setMinimum(xmin)
        self.xmax.setMaximum(xmax)
        self.ymin = QDoubleSpinBox()
        self.ymin.setMinimum(ymin)
        self.ymin.setMaximum(ymax)
        self.ymin.setDecimals(5)
        self.ymax = QDoubleSpinBox()
        self.ymax.setMinimum(ymin)
        self.ymax.setMaximum(ymax)
        self.ymin.setValue(ymin)
        self.ymax.setValue(ymax)
        self.xmin.setValue(xmin)
        self.xmax.setValue(xmax)
        self.ymax.setDecimals(5)

        self.persistant = QCheckBox("Persistance")

        settingsLayout.addWidget(QLabel("X Limits:"), 0, 0)
        settingsLayout.addWidget(self.xmin, 0, 1)
        settingsLayout.addWidget(self.xmax, 0, 2)
        settingsLayout.addWidget(QLabel("Y Limits:"), 1, 0)
        settingsLayout.addWidget(self.ymin, 1, 1)
        settingsLayout.addWidget(self.ymax, 1, 2)
        settingsLayout.addWidget(self.persistant, 1, 3)
        layout.addLayout(settingsLayout)       
        self.gb.setLayout(layout)

        self.xmin.valueChanged.connect(self.updateAxis)
        self.xmax.valueChanged.connect(self.updateAxis)
        self.ymin.valueChanged.connect(self.updateAxis)
        self.ymax.valueChanged.connect(self.updateAxis)

    def getWidget(self):
        return self.gb

    def updateAxis(self):
        self.fig_ax.axis([self.xmin.value(), self.xmax.value(), self.ymin.value(), self.ymax.value()])
        self.redraw()

    def redraw(self):
        self.fig.canvas.draw()
            
    def updateData(self, data=None):
        if data:
            if self.persistant.isChecked() == False:
                self.fig_ax.cla()
                
            self.fig_ax.plot(data)

        if self.xmin.value() == self.xmax.value():
            cursettings = self.fig_ax.axis()
            self.xmin.setValue(cursettings[0])
            self.xmax.setValue(cursettings[1])
            self.ymin.setValue(cursettings[2])
            self.ymax.setValue(cursettings[3])
        else:
            self.updateAxis()

        self.redraw()


class OpenADC_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if openadc_qt == None:
            layout.addWidget(QLabel("OpenADC Import Failed"))
            self.ser = None
        else:            
            self.serialList = QComboBox()
            self.connectButton = QPushButton("Connect")
            self.disconnectButton = QPushButton("Disconnect")
            self.disconnectButton.setEnabled(False)
            self.resetButton = QPushButton("Reset")
            self.resetButton.setEnabled(False)
            self.connectButton.clicked.connect(self.con)
            self.disconnectButton.clicked.connect(self.dis)           
            self.testButton = QPushButton("Speed Test")
            self.testButton.setEnabled(False)
            
            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(QLabel("Serial Port:"), 0, 0)
            connlayout.addWidget(self.serialList, 0, 1)
            connlayout.addWidget(self.connectButton, 0, 2)
            connlayout.addWidget(self.disconnectButton, 0, 3)
            connlayout.addWidget(self.resetButton, 0, 4)
            connlayout.addWidget(self.testButton, 0, 5)
            layout.addWidget(connection)

            self.ser = None
            self.serialRefresh()            

            self.scope = openadc_qt.OpenADCQt(self, False)
            layout.addItem(self.scope.getLayout())
            self.resetButton.clicked.connect(self.scope.reset)
            self.testButton.clicked.connect(self.scope.test)

        self.setLayout(layout)

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:        
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = self.serialList.currentText()
            self.ser.baudrate = 512000;
            self.ser.timeout  = 2     # 2 second timeout


            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException, e:
                    attempts = attempts - 1
                    self.ser = None
                    if attempts == 0:
                        raise IOError("Could not open %s"%self.ser.name)
        
        self.scope.ADCconnect(self.ser)
        self.connectButton.setEnabled(False)
        self.disconnectButton.setEnabled(True)
        self.resetButton.setEnabled(True)
        self.testButton.setEnabled(True)

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
            self.scope.setEnabled(False)
            self.disconnectButton.setEnabled(False)
            self.resetButton.setEnabled(False)
            self.connectButton.setEnabled(True)
            self.testButton.setEnabled(False)
        
    def serialRefresh(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

class OpenADC_ftdi_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if (openadc_qt == None) or (ft == None):
            if openadc_qt == None:
                layout.addWidget(QLabel("OpenADC Import Failed"))

            if ft == None:
                layout.addWidget(QLabel("ftd2xx Import Failed"))
                
            self.ser = None
        else:            
            self.serialList = QComboBox()
            self.connectButton = QPushButton("Connect")
            self.disconnectButton = QPushButton("Disconnect")
            self.disconnectButton.setEnabled(False)
            self.resetButton = QPushButton("Reset")
            self.resetButton.setEnabled(False)
            self.refreshButton = QPushButton("Refresh List")
            
            self.connectButton.clicked.connect(self.con)
            self.disconnectButton.clicked.connect(self.dis)
            self.refreshButton.clicked.connect(self.serialRefresh)
            self.testButton = QPushButton("Speed Test")
            self.testButton.setEnabled(False)

            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(QLabel("FTDI Device:"), 0, 0)
            connlayout.addWidget(self.serialList, 0, 1)
            connlayout.addWidget(self.connectButton, 0, 2)
            connlayout.addWidget(self.disconnectButton, 0, 3)
            connlayout.addWidget(self.resetButton, 0, 4)
            connlayout.addWidget(self.refreshButton, 0, 5)
            connlayout.addWidget(self.testButton, 0, 5)
            layout.addWidget(connection)

            self.ser = None
            self.serialRefresh()            

            self.scope = openadc_qt.OpenADCQt(self, False)
            layout.addItem(self.scope.getLayout())
            self.resetButton.clicked.connect(self.scope.reset)
            self.testButton.clicked.connect(self.scope.test)

        self.setLayout(layout)

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            snum = self.serialList.currentText();
            try:
                self.ser = ft.openEx(str(snum), ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
                self.ser.setBitMode(0x00, 0x40)
                self.ser.setTimeouts(500, 500)
                self.ser.setLatencyTimer(2)
            except ft.ftd2xx.DeviceError, e:
                self.ser = None
                print e
                raise IOError("Could not open %s"%snum)
       
        self.scope.ADCconnect(self.ser)
        self.connectButton.setEnabled(False)
        self.disconnectButton.setEnabled(True)
        self.resetButton.setEnabled(True)
        self.refreshButton.setEnabled(False)
        self.testButton.setEnabled(True)

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
            self.scope.setEnabled(False)
            self.disconnectButton.setEnabled(False)
            self.resetButton.setEnabled(False)
            self.connectButton.setEnabled(True)
            self.refreshButton.setEnabled(True)
            self.testButton.setEnabled(False)
        
    def serialRefresh(self):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [" "]
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

class Smartcard_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if target_smartcard == None:
            layout.addWidget(QLabel("target_smartcard Import Failed"))
            layout.addWidget(QLabel(str(target_smartcard_str)))
            self.target = None
        else:
            self.target = target_smartcard.SmartCard()

        self.setLayout(layout)

class SimpleSerial_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if target_simpleserial == None:
            layout.addWidget(QLabel("SimpleSerial Import Failed"))
            layout.addWidget(QLabel(str(target_simpleserial_str)))
            self.target = None
        else:            
            self.serialList = QComboBox()
            connectButton = QPushButton("Connect")
            disconnectButton = QPushButton("Disconnect")

            connectButton.clicked.connect(self.con)
            disconnectButton.clicked.connect(self.dis)            

            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(QLabel("Serial Port:"), 0, 0);
            connlayout.addWidget(self.serialList, 0, 1);
            connlayout.addWidget(connectButton, 1, 0);
            connlayout.addWidget(disconnectButton, 1, 1);
            layout.addWidget(connection)

            self.target = target_simpleserial.SimpleSerial()
            self.serialRefresh()            

        self.setLayout(layout)

    def __del__(self):
        if self.target != None:
            self.dis()

    def con(self):
        self.target.connect(self.serialList.currentText())
                    
    def dis(self):
        self.target.disconnect()
        
    def serialRefresh(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

class SASEBOW_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if target_sasebow == None:
            layout.addWidget(QLabel("SASEBO-W Import Failed"))
            layout.addWidget(QLabel(str(target_sasebow_str)))
            self.target = None
        else:            
            self.serialList = QComboBox()
            self.connectButton = QPushButton("Connect")
            self.disconnectButton = QPushButton("Disconnect")
            self.resetButton = QPushButton("Reset")
            self.ATRlabel = QLineEdit("ATR = ?")
            self.ATRlabel.setReadOnly(True)

            self.connectButton.clicked.connect(self.con)
            self.disconnectButton.clicked.connect(self.dis)
            self.resetButton.clicked.connect(self.res)
            

            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(QLabel("Serial Port:"), 0, 0);
            connlayout.addWidget(self.serialList, 0, 1);
            connlayout.addWidget(self.connectButton, 1, 0);
            connlayout.addWidget(self.disconnectButton, 1, 1);
            connlayout.addWidget(self.resetButton, 1, 2)            
            layout.addWidget(connection)
            layout.addWidget(self.ATRlabel)

            self.disconnectButton.setEnabled(False)
            self.resetButton.setEnabled(False)

            self.target = target_sasebow.SASEBOW()
            self.serialRefresh()            

        self.setLayout(layout)

    def __del__(self):
        if self.target != None:
            self.dis()

    def con(self):
        try:
            self.target.connect(self.serialList.currentText())
            self.disconnectButton.setEnabled(True)
            self.connectButton.setEnabled(False)
            self.resetButton.setEnabled(True)
            self.ATRlabel.setText(self.target.getATR())
        except:
            print "Unexpected error:", sys.exc_info()

    def res(self):
        self.target.reset()
        self.ATRlabel.setText(self.target.getATR())
                    
    def dis(self):
        self.disconnectButton.setEnabled(False)
        self.connectButton.setEnabled(True)
        self.resetButton.setEnabled(False)
        self.target.disconnect()
        
    def serialRefresh(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

class SASEBOW_integrated_tab(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        self.parent = parent

        if target_sasebow_integrated == None:
            layout.addWidget(QLabel("SASEBO-W Integrated Import Failed"))
            layout.addWidget(QLabel(str(target_sasebow_integrated_str)))
            self.target = None
        else:            
            self.serialList = QComboBox()
            self.connectButton = QPushButton("Connect")
            self.disconnectButton = QPushButton("Disconnect")
            self.resetButton = QPushButton("Reset")
            self.statusButton = QPushButton("Updated Status")
            self.ATRlabel = QLineEdit("ATR = ?")
            self.ATRlabel.setReadOnly(True)
            self.Statuslabel = QLineEdit("Status = ?")
            self.Statuslabel.setReadOnly(True)

            self.connectButton.clicked.connect(self.con)
            self.disconnectButton.clicked.connect(self.dis)
            self.resetButton.clicked.connect(self.res)
            self.statusButton.clicked.connect(self.update)
            
            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(self.connectButton, 1, 0)
            connlayout.addWidget(self.disconnectButton, 1, 1)
            connlayout.addWidget(self.resetButton, 1, 2)
            connlayout.addWidget(self.statusButton, 1, 3)
            layout.addWidget(connection)
            layout.addWidget(self.ATRlabel)
            layout.addWidget(self.Statuslabel)

            self.disconnectButton.setEnabled(False)
            self.resetButton.setEnabled(False)
            self.statusButton.setEnabled(False)

            self.target = target_sasebow_integrated.SASEBOW_Integrated()
        self.setLayout(layout)

    def __del__(self):
        if self.target != None:
            self.dis()

    def update(self):
        self.Statuslabel.setText("Status = 0x%02x"%self.target.scGetStatus())

    def con(self):
        try:
            self.target.connect(self.parent.tw.widget(1).scope.sc)
            self.disconnectButton.setEnabled(True)
            self.connectButton.setEnabled(False)
            self.resetButton.setEnabled(True)
            self.statusButton.setEnabled(True)
            self.ATRlabel.setText(self.target.getATR())
        except:
            print "Unexpected error:", sys.exc_info()

    def res(self):
        self.target.reset()
        self.ATRlabel.setText(self.target.getATR())
                    
    def dis(self):
        self.disconnectButton.setEnabled(False)
        self.connectButton.setEnabled(True)
        self.resetButton.setEnabled(False)
        self.statusButton.setEnabled(False)
        self.target.disconnect()

class SASEBOGII_tab(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        self.parent = parent

        if target_sasebogii == None:
            layout.addWidget(QLabel("target_sasebogii Import Failed"))
            layout.addWidget(QLabel(str(target_sasebogii_str)))
            self.target = None
        else:            
            connectButton = QPushButton("Connect")
            disconnectButton = QPushButton("Disconnect")
            self.target = target_sasebogii.SASEBOGII()
            connectButton.clicked.connect(self.con)
            disconnectButton.clicked.connect(self.dis)

            layout.addWidget(connectButton)
            layout.addWidget(disconnectButton)
            
        self.setLayout(layout)

    def con(self):
        if self.target.connect():
            if self.parent != None:
                self.parent.statusBar().showMessage("Connected to SASEBO-GII Board")
        else:
            if self.parent != None:
                self.parent.statusBar().showMessage("SASEBO-GII Board Not Found")    

    def dis(self):
        self.target.disconnect()

class SmartcardSerial_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        if target_smartcardserial == None:
            layout.addWidget(QLabel("SmartcardSerial Import Failed"))
            layout.addWidget(QLabel(str(target_smartcardserial_str)))
            self.target = None
        else:            
            self.serialList = QComboBox()
            self.connectButton = QPushButton("Connect")
            self.disconnectButton = QPushButton("Disconnect")
            self.resetButton = QPushButton("Reset")
            self.ATRlabel = QLineEdit("ATR = ?")
            self.ATRlabel.setReadOnly(True)

            self.connectButton.clicked.connect(self.con)
            self.disconnectButton.clicked.connect(self.dis)
            self.resetButton.clicked.connect(self.res)
            

            connection = QGroupBox("Connection")
            connlayout = QGridLayout()
            connection.setLayout(connlayout)
            connlayout.addWidget(QLabel("Serial Port:"), 0, 0);
            connlayout.addWidget(self.serialList, 0, 1);
            connlayout.addWidget(self.connectButton, 1, 0);
            connlayout.addWidget(self.disconnectButton, 1, 1);
            connlayout.addWidget(self.resetButton, 1, 2)            
            layout.addWidget(connection)
            layout.addWidget(self.ATRlabel)

            self.disconnectButton.setEnabled(False)
            self.resetButton.setEnabled(False)

            self.target = target_smartcardserial.SmartcardSerial()
            self.serialRefresh()            

        self.setLayout(layout)

    def __del__(self):
        if self.target != None:
            self.dis()

    def con(self):
        try:
            self.target.connect(self.serialList.currentText())
            self.disconnectButton.setEnabled(True)
            self.connectButton.setEnabled(False)
            self.resetButton.setEnabled(True)
            self.ATRlabel.setText(self.target.getATR())
        except:
            print "Unexpected error:", sys.exc_info()

    def res(self):
        self.target.reset()
        self.ATRlabel.setText(self.target.getATR())
                    
    def dis(self):
        self.disconnectButton.setEnabled(False)
        self.connectButton.setEnabled(True)
        self.resetButton.setEnabled(False)
        self.target.disconnect()
        
    def serialRefresh(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)


class DPAV3_tab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        self.writer = writer_dpav3.dpav3()
        self.setLayout(layout)

class GeneralConfig(QWidget):
    def __init__(self, parent, scopelist, scopecb, targetlist, targetcb, tracelist, tracecb):
        QWidget.__init__(self)
        layout = QGridLayout()

        self.scopetype = QComboBox()
        for scope in scopelist:
            self.scopetype.addItem(scope)
        self.scopetype.currentIndexChanged.connect(scopecb)
        
        self.targettype = QComboBox()
        for target in targetlist:
            self.targettype.addItem(target)
        self.targettype.currentIndexChanged.connect(targetcb)
        
        self.tracetype = QComboBox()
        for tracewr in tracelist:            
            self.tracetype.addItem(tracewr)
        self.tracetype.currentIndexChanged.connect(tracecb)

        layout.addWidget(QLabel("Scope:"), 1, 0)
        layout.addWidget(self.scopetype, 1, 1)
        layout.addWidget(QLabel("Target:"), 2, 0)
        layout.addWidget(self.targettype, 2, 1)
        layout.addWidget(QLabel("Trace Format:"), 3, 0)
        layout.addWidget(self.tracetype, 3, 1)

        self.keysettings = QGroupBox("Key Settings")
        self.keylayout = QGridLayout()
        self.keysettings.setLayout(self.keylayout)
        self.keyText = QLineEdit()
        self.keyCB = QCheckBox("Use Key")
        self.keyCB.setChecked(True)
        self.keyNewPB = QPushButton("Generate")
        self.keyText.setInputMask(">HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH")
        self.keyText.setText("2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c")
        self.keyText.setMaxLength(47)
        self.keylayout.addWidget(QLabel("Encryption Key:"), 0, 0)
        self.keylayout.addWidget(self.keyText, 1, 0)
        self.keylayout.addWidget(self.keyCB, 2, 0)
        self.keylayout.addWidget(self.keyNewPB, 2, 1)
        layout.addWidget(self.keysettings, 4, 0)

        self.keyCB.stateChanged.connect(parent.keyChanged)
        self.keyText.textChanged.connect(parent.keyChanged)
        
        self.capturesettings = QGroupBox("Capture Settings")
        self.caplayout = QGridLayout()
        self.capturesettings.setLayout(self.caplayout)
        self.caplayout.addWidget(QLabel("Number Traces"), 0, 0)
        self.numTraces = QSpinBox()
        self.numTraces.setMinimum(0)
        self.numTraces.setMaximum(1000000)
        self.caplayout.addWidget(self.numTraces, 0, 1)
        self.startCapturePB = QPushButton("Start Capture")
        self.cancelCapturePB = QPushButton("Cancel Capture")

        self.textResults = QGroupBox("Results")
        self.textResultsLayout = QGridLayout()
        self.textInLine = QLineEdit()
        self.textOutLine = QLineEdit()
        self.textResultsLayout.addWidget(QLabel("Text In "), 0, 0)
        self.textInLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textInLine, 0, 1)
        self.textResultsLayout.addWidget(QLabel("Text Out"), 1, 0)
        self.textOutLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textOutLine, 1, 1)
        self.textResults.setLayout(self.textResultsLayout)

        layout.addWidget(self.textResults, 5, 0)

        self.counter = QLabel("Traces = 0")
        self.caplayout.addWidget(self.counter, 0, 2)
        self.caplayout.addWidget(self.startCapturePB, 1, 0)
        self.caplayout.addWidget(self.cancelCapturePB, 1, 1)
        self.startCapturePB.clicked.connect(parent.startCapture)
        self.cancelCapturePB.clicked.connect(parent.stopCapture)
        layout.addWidget(self.capturesettings, 4, 1)
        
        self.setLayout(layout)

class doAcq(object):
    def __init__(self, scope, target, writer, label=None, fixedPlain=False, updateData=None, textInLabel=None, textOutLabel=None):
        self.target = target
        self.scope = scope
        self.label = label
        self.running = False
        self.fixedPlainText = fixedPlain
        self.maxtraces = 1
        self.updateData = updateData
        self.textInLabel = textInLabel
        self.textOutLabel = textOutLabel

        self.plain = bytearray(16)
        for i in range(0,16):
                   self.plain[i] = random.randint(0, 255)
        

    def TargetDoTrace(self, plaintext, key=None):
        self.target.loadEncryptionKey(key)      
        self.target.loadInput(plaintext)
        self.target.go()

        resp = self.target.readOutput()

        #print "pt:",
        #for i in plaintext:
        #    print " %02X"%i,
        #print ""

        #print "sc:",
        #for i in resp:
        #    print " %02X"%i,
        #print ""

        return resp

    def doSingleReading(self, update=True, N=None, textIn=None, key=None):
        self.scope.ADCarm()

        if textIn:
            self.textin = textIn
        else:
            self.textin = bytearray(16)
            for i in range(0,16):
                self.textin[i] = random.randint(0, 255)

        if key:
            self.key = key
        else:
        #    if self.newKeyPerTrace:
        #        self.key = bytearray(16)
        #        for i in range(0,16):
        #            self.key[i] = random.randint(0, 255)
        #    else:
            self.key = None
        
        ## Start target now
        if self.textInLabel != None:
            self.textInLabel.setText("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"%(self.textin[0],self.textin[1],self.textin[2],self.textin[3],self.textin[4],self.textin[5],self.textin[6],self.textin[7],self.textin[8],self.textin[9],self.textin[10],self.textin[11],self.textin[12],self.textin[13],self.textin[14],self.textin[15]))

        #Set mode
        self.target.setModeEncrypt()

        #Load input, start encryption, get output
        self.textout = self.TargetDoTrace(self.textin, self.key)

        try:
            self.textOutLabel.setText("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"%(self.textout[0],self.textout[1],self.textout[2],self.textout[3],self.textout[4],self.textout[5],self.textout[6],self.textout[7],self.textout[8],self.textout[9],self.textout[10],self.textout[11],self.textout[12],self.textout[13],self.textout[14],self.textout[15]))
        except:
            print "Response failed?"

        #Get ADC reading
        self.scope.ADCcapture(update, N)


    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self):
        self.running = True

        tw = writer_dpav3.dpav3()
        tw.openFiles()
        tw.addKey(self.key)

        if self.fixedPlainText:
            plain = self.plain
        else:
            plain = None

        while (tw.numtrace < self.maxtraces) and self.running:
            self.doSingleReading(True, None, plain)
            tw.addTrace(self.textin, self.textout, self.scope.datapoints, self.key)

            if self.updateData:
                self.updateData(self.scope.datapoints)

            if self.label != None:
                self.label.setText("Traces = %d"%tw.numtrace)

        tw.closeAll()

        self.running = False     
                    
class MainWindow(QMainWindow):
    def captureOne(self):
        da = doAcq(self.tw.widget(1).scope, self.tw.widget(2).target, self.tw.widget(3).writer, label=self.tw.widget(0).counter, fixedPlain=self.fixedPlainCB.isChecked(), textInLabel=self.tw.widget(0).textInLine, textOutLabel=self.tw.widget(0).textOutLine)
        da.doSingleReading(key=self.key)
        self.preview.updateData(self.tw.widget(1).scope.datapoints)    

    def startCapture(self):
        self.da = doAcq(self.tw.widget(1).scope, self.tw.widget(2).target, self.tw.widget(3).writer, label=self.tw.widget(0).counter, updateData=self.preview.updateData)
        self.da.setMaxtraces(self.tw.widget(0).numTraces.value())
        self.da.doSingleReading(key=self.key)
        daThread = threading.Thread(target = self.da.doReadings)

        # Start the stream
        daThread.start()

    def stopCapture(self):
        if self.da != None:
            self.da.running = False            
        
    def scopeChanged(self, index):
        self.tw.removeTab(1)

        if index==0:
            self.tw.insertTab(1, OpenADC_tab(), "&OpenADC-Serial")

        elif index==1:
            self.tw.insertTab(1, OpenADC_ftdi_tab(), "&OpenADC-FTDI")

        else:
            print "Invalid scope index"          
           

    def targetChanged(self, index):
        self.tw.removeTab(2)

        if index==0:
            self.tw.insertTab(2, SimpleSerial_tab(), "&Simple Serial")

        elif index==1:
            self.tw.insertTab(2, Smartcard_tab(), "&SmartCard Reader")

        elif index==2:
            self.tw.insertTab(2, SmartcardSerial_tab(), "&SmartCard Serial Reader")

        elif index==3:
            self.tw.insertTab(2, SASEBOGII_tab(self), "&SASEBO-GII")

        elif index==4:
            self.tw.insertTab(2, SASEBOW_tab(), "&SASEBOW")

        elif index==5:
            self.tw.insertTab(2, SASEBOW_integrated_tab(self), "&SASEBOW_Integrated")
            
        else:
            print "Invalid target index"     

    def traceChanged(self, index):
        self.tw.removeTab(3)

        if index==0:
            self.tw.addTab(DPAV3_tab(), "&DPAContestV3")
        else:
            print "Invalid trace index"

    def keyChanged(self, arg):
        if self.tw.widget(0).keyCB.isChecked() == True:
            keystr = self.tw.widget(0).keyText.text().split()
            self.key = bytearray()
            for s in keystr:
                self.key.append(int(s, 16))
        else:
            self.key = None
            
        #for s in self.key:
        #    print "%X "%s,

    def curTabChange(self, index):
        for i in range(self.tw.count()):
            if i == index:
                self.tw.widget(i).setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
            else:
                self.tw.widget(i).setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Ignored)
            
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self.da = None
        self.key = None
       
        self.statusBar()
        self.setWindowTitle("Chip Whisperer Capture Application")
        self.title = QLabel("Chip Whisperer Capture Application")
        self.title.setAlignment(Qt.AlignCenter)
        flabel = self.title.font()
        flabel.setPointSize(14)
        self.title.setFont(flabel)

        # Create layout and add widgets
        self.mw = QWidget()
        
        layout = QVBoxLayout()
        layout.addWidget(self.title)

        self.tw = QTabWidget()
        self.tw.currentChanged.connect(self.curTabChange)        
        self.gctab = GeneralConfig(self, ["OpenADC-Serial", "OpenADC-FTDI"], self.scopeChanged,
                           ["Simple Serial", "SmartCard", "SmartCard Serial", "SASEBO-GII", "SASEBOW Serial", "SASEBOW Integrated"], self.targetChanged,
                           ["DPAContestV3"], self.traceChanged)
        self.tw.addTab(self.gctab, "&General")
        
        #Defaults
        self.scopeChanged(0)
        self.targetChanged(0)
        self.traceChanged(0)

        layout.addWidget(self.tw)
        self.curTabChange(0)

        #Update key
        self.keyChanged(0)

        self.preview = pysideGraph("Preview", 0, 100000, -0.5, 0.5)
        layout.addWidget(self.preview.getWidget())


        self.testcapture = QGroupBox("Test Capture")
        self.caplayout = QGridLayout()
        self.testcapture.setLayout(self.caplayout)
        self.startCapturePB = QPushButton("Capture 1")
        self.fixedPlainCB = QCheckBox("Fixed Plaintext")
        self.caplayout.addWidget(self.startCapturePB)
        self.caplayout.addWidget(self.fixedPlainCB)
        layout.addWidget(self.testcapture)

        self.startCapturePB.clicked.connect(self.captureOne)
             
        # Set dialog layout
        self.setLayout(layout)       
        self.mw.setLayout(layout)
        self.setCentralWidget(self.mw)

    def closeEvent(self, event):
        self.stopCapture()
        self.tw.widget(1).scope.close()
        self.tw.widget(2).target.close()
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
