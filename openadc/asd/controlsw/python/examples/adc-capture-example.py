# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the 2-Clause BSD License. See LICENSE
# file which should have came with this code.

from PySide.QtCore import *
from PySide.QtGui import *

import sys
import os
import threading
import time
import logging
import math

try:
    import numpy
except ImportError:
    print "*************************************************"
    print "*************************************************"
    print "Install numpy from http://sourceforge.net/projects/numpy/files/NumPy"
    print "or  http://www.lfd.uci.edu/~gohlke/pythonlibs/#numpy"
    print "*************************************************"
    print "*************************************************"    

import openadc.qt as oaqt
import openadc.scan as scan
import serial


#Non-Critical Imports
try:
    import ftd2xx as ft
except ImportError:
    ft = None
    ft_str = sys.exc_info()
    print ft_str
    print "ftd2xx import failed. Install XXX from XXX for FTDI support"
except WindowsError:
    ft = None
    ft_str = sys.exc_info()
    print ft_str
    print "ftd2xx import failed. Install FTDI D2XX DLL from ftdichip.com if needed"    

try:
    import usb
except ImportError:
    usb = None
    usb_str = sys.exc_info()
    print usb_str
    print "usb import failed. Install pyusb from http://pyusb.sourceforge.net for FX2 Support"

#import numpy as np
#import scipy as sp
#import matplotlib as mp
#import pylab as pl

class SerialADCLayout():
    def __init__(self):
        self.gb = QGroupBox("Connection Settings");
        layout = QGridLayout()

        self.serialList = QComboBox()
        layout.addWidget(QLabel("Port:"), 0, 0)
        layout.addWidget(self.serialList, 0, 1)

        self.baud = QComboBox()
        self.baud.addItem("512000")
        self.baud.addItem("115200")
        layout.addWidget(QLabel("Baud:"), 1, 0)
        layout.addWidget(self.baud, 1, 1)
        self.gb.setLayout(layout)

        self.ser = None

    def setDisabled(self, disable):
        self.serialList.setDisabled(disable)
        self.baud.setDisabled(disable)
        
    def connect(self):
         # Open serial port if not already
         self.ser = serial.Serial()
         self.ser.port     = self.serialList.currentText()
         self.ser.baudrate = int(self.baud.currentText())
         self.ser.timeout  = 1.0     # 0.5 second timeout

         attempts = 1
         while attempts > 0:
            #if attempts > 2:
            #    self.ser.baudrate = 115200
            try:
                 self.ser.open()
                 attempts = 0
                 self.setDisabled(True)
                 return True
            except serial.SerialException, e:
                 attempts = attempts - 1
                 if attempts == 0:                     
                     QMessageBox.warning(None, "Serial Port",
                                         "Could not open %s"%self.ser.name)
                     self.ser.close()
                     self.ser = None
                     return False
            except ValueError, s:
                QMessageBox.warning(None, "Serial Port",
                                         "Invalid Settings for Port")
                self.ser.close()
                self.ser = None
                return False
         return False
        
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def disconnect(self):
        self.setDisabled(False)
        if self.ser:
            self.ser.close()
            self.ser = None

    def update(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

    def __del__(self):
        self.ser.close()

class FTDIADCLayout():
    def __init__(self):
        self.gb = QGroupBox("Connection Settings");
        layout = QGridLayout()

        self.serialList = QComboBox()
        layout.addWidget(QLabel("Serial Num:"), 0, 0)
        layout.addWidget(self.serialList, 0, 1)

        self.mode = QComboBox()
        self.mode.addItem("Sync (Dual-Channel)")
        self.mode.addItem("ASync (Single-Channel)")
        layout.addWidget(QLabel("FIFO Mode:"), 1, 0)
        layout.addWidget(self.mode, 1, 1)
        self.gb.setLayout(layout)

        self.ser = None

    def setDisabled(self, disable):
        self.serialList.setDisabled(disable)
        self.mode.setDisabled(disable)
        
    def connect(self):
        snum = self.serialList.currentText();
        try:
            self.ser = ft.openEx(str(snum), ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
            #Sync FIFO Requires extra work
            if self.mode.currentIndex() == 0:
                self.ser.setBitMode(0x00, 0x40)
            self.ser.setTimeouts(500, 500)
            self.ser.setLatencyTimer(2)
            return True
        except ft.ftd2xx.DeviceError, e:
            self.ser = None
            print e
            QMessageBox.warning(None, "FTDI Port", "Could not open %s"%snum)
            return False
        
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def disconnect(self):
        self.setDisabled(False)
        if self.ser:
            self.ser.close()
            self.ser = None

    def update(self):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [" "]
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)

    def __del__(self):
        self.ser.close()

class FX2ADCLayout():
    def __init__(self):
        self.gb = QGroupBox("Connection Settings");
        layout = QGridLayout()

        #self.serialList = QComboBox()
        #layout.addWidget(QLabel("Serial Num:"), 0, 0)
        #layout.addWidget(self.serialList, 0, 1)

        #self.mode = QComboBox()
        #self.mode.addItem("Sync (Dual-Channel)")
        #self.mode.addItem("ASync (Single-Channel)")
        #layout.addWidget(QLabel("FIFO Mode:"), 1, 0)
        #layout.addWidget(self.mode, 1, 1)
        #self.gb.setLayout(layout)

        self.dev = None

    def setDisabled(self, disable):
        #self.serialList.setDisabled(disable)
        #self.mode.setDisabled(disable)
        return
        
    def connect(self):
        #snum = self.serialList.currentText();

        dev = usb.core.find(idVendor=0x221A, idProduct=0x0100)

        if dev is None:
            QMessageBox.warning(None, "FX2 Port", "Could not open USB Device")            
            return False
        
         # set the active configuration. With no arguments, the first
        # configuration will be the active one
        dev.set_configuration()

        self.dev = dev
        self.writeEP = 0x06
        self.readEP = 0x82
        self.interface = 0

        self.ser = self

        #TODO: Check extended descriptor!!!

        return True

    def read(self, N=0, debug=False):
        data = self.dev.read(self.readEP, N, self.interface, 500)
        data = bytearray(data)
        if debug:
            print "RX: ",
            for b in data:
                print "%02x "%b,
            print ""
        return data

    def write(self, data, debug=False):
        data = bytearray(data)
        if debug:
            print "TX: ",
            for b in data:
                print "%02x "%b,
            print ""
        self.dev.write(self.writeEP, data, self.interface, 500)
            
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def disconnect(self):
        self.setDisabled(False)

    def update(self):
        print "update"
 
class MainWindow(QMainWindow):                    
    def ADCcapture(self):
        self.oa.arm()
        self.oa.capture()

    def ADCupdate(self):
        #self.oa.update()
        pass

    def ADCread(self):
        self.oa.read();

    def ADCloop(self):
        if self.startButton.isChecked():
            self.startButton.setText("Stop Capture Loop")
        else:
            self.startButton.setText("Start Capture Loop")
        
        while self.startButton.isChecked():
            self.ADCcapture()
            QCoreApplication.processEvents()
            time.sleep(0.1)

    def conUpdate(self):
        if self.adccon:
            self.adccon.update()

    def conModeChange(self, index):
        if index == 0:
            self.adccon = self.OADC_Ser
        elif index == 1:
            self.adccon = self.OADC_FX2
        elif index == 2:
            self.adccon = self.OADC_FTDI

        self.conSettings.setCurrentIndex(index)
        self.adccon.update()

    def connectClicked(self):
        if self.connectButton.isChecked():
            self.connectButton.setText("Disconnect")
            self.mode.setDisabled(True)
            self.captureButton.setDisabled(False)
            self.startButton.setDisabled(False)
            self.updateButton.setDisabled(False)

            #Failed to open port
            if self.adccon.connect() == False:
                self.connectButton.setChecked(False)
                self.connectButton.setText("Connect")
                return

            #Find OpenADC
            try:
                self.oa.con(self.adccon.ser)
            except IOError as detail:
                QMessageBox.warning(None, "OpenADC",
                        "Failed to find OpenADC Connected\nCheck Console for Detail")
                print detail
                self.connectButton.setChecked(False)
                self.connectButton.setText("Connect")
                return
        else:
            self.connectButton.setText("Connect")
            self.mode.setDisabled(False)
            self.captureButton.setDisabled(True)
            self.startButton.setDisabled(True)
            self.updateButton.setDisabled(True)
            self.adccon.disconnect()
        
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        settings = QSettings('NewAE', 'openadc-example')
        self.restoreGeometry(settings.value("geometry"))

        self.ser = None
        self.base = None
        self.plot = None

        #General stuff
        self.statusBar()
        self.setWindowTitle("OpenADC Capture App")
        self.title = QLabel("OpenADC Capture App")
        self.title.setAlignment(Qt.AlignCenter)
        flabel = self.title.font()
        flabel.setPointSize(24)
        self.title.setFont(flabel)

        ## Get an OpenADC Instance
        self.oa = oaqt.OpenADCQt(self);

        ##Get Connection Instances
        self.OADC_Ser = SerialADCLayout()
        self.OADC_FX2 = FX2ADCLayout()
        self.OADC_FTDI = FTDIADCLayout()

        #Update the list for default widget
        self.OADC_Ser.update()

        ## Setup connection
        self.conSettings = QStackedWidget()

        #Add each group box
        self.conSettings.addWidget(self.OADC_Ser.gb)
        self.conSettings.addWidget(self.OADC_FX2.gb)
        self.conSettings.addWidget(self.OADC_FTDI.gb)

        self.mode = QComboBox()
        self.mode.addItem("Serial")
        self.mode.addItem("FX2 (Ztex)")
        self.mode.addItem("FTDI FIFO Mode")
        self.mode.currentIndexChanged.connect(self.conModeChange)
        
        self.connectButton = QPushButton("Connect")
        self.connectButton.setCheckable(True)
        self.connectButton.clicked.connect(self.connectClicked)

        self.refreshListButton = QPushButton("Refresh List")
        self.refreshListButton.clicked.connect(self.conUpdate)
        
        self.captureButton = QPushButton("Capture One")
        self.updateButton = QPushButton("Get Status")
        self.startButton = QPushButton("Start Capture Loop")
        self.startButton.setCheckable(True)

        
        self.connect(self.captureButton, SIGNAL("clicked()"),self, SLOT("ADCcapture()"))
        self.connect(self.updateButton, SIGNAL("clicked()"),self,SLOT("ADCupdate()"))
        self.connect(self.startButton, SIGNAL("clicked()"),self,SLOT("ADCloop()"))
        
        # Create layout and add widgets
        self.mw = QWidget()

        glayout = QGridLayout()
        
        layout = QVBoxLayout()
        
        layout.addWidget(self.title)

        layoutCon = QGridLayout()
        layoutCon.addWidget(QLabel("Mode:"), 0, 0)
        layoutCon.addWidget(self.mode, 0, 1)
        layoutCon.addWidget(self.connectButton, 1, 1)
        layoutCon.addWidget(self.refreshListButton, 2, 1)
        
        glayout.addWidget(self.conSettings, 0, 0)
        glayout.addLayout(layoutCon, 0, 1)
        glayout.addWidget(self.captureButton, 1, 0)
        glayout.addWidget(self.updateButton, 1, 1)
        glayout.addWidget(self.startButton, 2, 0)

        layout.addLayout(glayout)
        layout.addLayout(self.oa.getLayout())
              
        # Set dialog layout
        self.setLayout(layout)       
        self.mw.setLayout(layout)
        self.setCentralWidget(self.mw)

        self.trigmode = 0
        self.hilowmode = 0

        #Force defaults we want
        self.conModeChange(0)
        self.connectClicked()

    def closeEvent(self, event):
        settings = QSettings('NewAE', 'openadc-example')
        settings.setValue("geometry", self.saveGeometry());
        QMainWindow.closeEvent(self, event)

  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
