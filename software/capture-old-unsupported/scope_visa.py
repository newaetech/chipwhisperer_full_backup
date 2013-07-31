# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
from PySide.QtCore import *
from PySide.QtGui import *
import random
from time import gmtime, strftime
from visa import *

class scopeLowLevel():
    def setInst(self, inst):
        self.scope = inst
    
    def init(self):
        self.scope.write(self.getDefaultSetup())

    def arm(self):       
        self.scope.write(":DIGitize")

    def read(self, Update=False, N=None):
        self.scope.write(":WAVeform:DATA? %d,%d"%(self.XDisplayedOrigin, self.XDisplayedRange))
        data = self.scope.read_raw()

        #Find '#' which is start of frame
        start = data.find('#')

        if start < 0:
            print "Error in header"
            return

        start = start+1
        hdrlen = data[start]
        hdrlen = int(hdrlen)

        print hdrlen

        start = start+1
        datalen = data[start:(start+hdrlen)]
        datalen = int(datalen)
        print datalen

        start = start+hdrlen

        #Each is two bytes
        wavdata = bytearray(data[start:(start + datalen)])

        self.datapoints = []

        for j in range(0, datalen, 2):
            data = wavdata[j] | (wavdata[j+1] << 8)

            if (data & 0x8000):
                data = -65536 + data

            self.datapoints.append(data) 
        
                       

    def getDefaultSetup(self):
       return \
       ":SYSTem:HEADer OFF\n" + \
       ":CHANnel1:INPut DCFifty\n" + \
       ":CHANnel1:DISPlay ON\n" + \
       ":CHANnel2:DISPLay ON\n" + \
       ":CHANnel2:SCALe 1.0\n" + \
       ":CHANnel2:POSition 0.0\n" + \
       ":TRIGger:MODE EDGE\n" + \
       ":TRIGger:EDGE:SOURce CHANnel2\n" + \
       ":TRIGger:EDGE:SLOPe NEGative\n" + \
       ":TRIGger:LEVel 2.0\n" + \
       ":TRIGger:SWEep TRIGgered\n" + \
       ":WAVeform:SOURce CHANnel1\n" + \
       ":WAVeform:COUPling DCFifty\n" + \
       ":WAVeform:FORMat WORD\n" + \
       ":WAVeform:BYTeorder LSBFirst\n" + \
       ":ACQuire:COMPlete 100\n"

    def getCurrentSettings(self):
        test = self.scope.ask_for_values(":WAVeform:PREamble?")

        if test[4] != 0.0:
            self.XDisplayedOrigin = (test[12] - test[5]) / test[4]
            srange = test[11] / test[4]
        else:
            self.XDisplayedOrigin = 0.0
            srange = 0.0
        
        spoints = test[2]

        if srange < spoints:
            self.XDisplayedRange = srange
        else:
            self.XDisplayedRange = spoints

        self.XScale = self.scope.ask_for_values(":TIMebase:SCALe?")
        self.XScale = self.XScale[0]
        self.XOffset = self.scope.ask_for_values(":TIMebase:POSition?")
        self.XOffset = self.XOffset[0]
        self.YOffset = self.scope.ask_for_values(":CHANnel1:OFFSet?")
        self.YOffset = self.YOffset[0]
        self.YScale = self.scope.ask_for_values(":CHANnel1:SCALe?")
        self.YScale = self.YScale[0]

        print self.YScale

class scopeQt(scopeLowLevel):
    def __init__(self):
        self.setupLayout()

    def XStuffChanged(self, data=None):
        self.XDisplayedOrigin = self.XDisplayedOriginSB.value()
        self.XDisplayedRange = self.XDisplayedRangeSB.value()

    def connect(self):
        self.setInst(instrument(self.visaName.currentText()))
        self.getCurrentSettings()

        self.XDisplayedOriginSB.setValue(self.XDisplayedOrigin)
        self.XDisplayedRangeSB.setValue(self.XDisplayedRange)

        for j in range(0, self.XRangeCB.count()):
            if float(self.XRangeCB.itemText(j)) == self.XScale:
                self.XRangeCB.setCurrentIndex(j)
                break

        for j in range(0, self.YRangeCB.count()):
            if float(self.YRangeCB.itemText(j)) == self.YScale:
                self.YRangeCB.setCurrentIndex(j)
                break

        self.YOffsetSB.setValue(self.YOffset)

    def setupLayout(self):                
            glayout = QGridLayout()

            layout = QHBoxLayout()
            
            
            self.scopeSetupCommands = QTextEdit()
            self.scopeSetupCommands.setPlainText(self.getDefaultSetup())

            self.visaName = QComboBox()

            self.visaName.setEditable(True)
            self.visaName.addItem("TCPIP0::192.168.2.100::inst0::INSTR")

            visaLabel = QLabel("VISA Name:")
            visaLabel.setAlignment(Qt.AlignRight)
            line = 0
            glayout.addWidget(visaLabel, line, 0)
            glayout.addWidget(self.visaName, line, 1)

            self.waitSB = QSpinBox()
            self.waitSB.setMinimum(-1000000)
            self.waitSB.setMaximum(1000000)
            line = line + 1
            glayout.addWidget(self.waitSB, line, 1)
            glayout.addWidget(QLabel("Wait"), line,0)

            self.XDisplayedOriginSB = QSpinBox()
            self.XDisplayedOriginSB.setMinimum(-1000000)
            self.XDisplayedOriginSB.setMaximum(1000000)
            self.XDisplayedOriginSB.valueChanged.connect(self.XStuffChanged)
            line = line + 1
            glayout.addWidget(self.XDisplayedOriginSB, line, 1)
            glayout.addWidget(QLabel("XDisplayedOrigin"), line,0)

            self.XDisplayedRangeSB = QSpinBox()
            self.XDisplayedRangeSB.setMinimum(-1000000)
            self.XDisplayedRangeSB.setMaximum(100000)
            self.XDisplayedRangeSB.valueChanged.connect(self.XStuffChanged)
            line = line + 1
            glayout.addWidget(self.XDisplayedRangeSB, line, 1)
            glayout.addWidget(QLabel("XDisplayedRange"), line,0)

            self.XOffsetSB = QSpinBox()
            self.XOffsetSB.setMinimum(-1000000)
            self.XOffsetSB.setMaximum(1000000)
            line = line + 1
            glayout.addWidget(self.XOffsetSB, line, 1)
            glayout.addWidget(QLabel("XOffset"), line,0)

            self.XRangeCB = QComboBox()
            self.XRangeCB.addItem("500E-3")
            self.XRangeCB.addItem("200E-3")
            self.XRangeCB.addItem("100E-3")
            self.XRangeCB.addItem("50E-3")
            self.XRangeCB.addItem("20E-3")
            self.XRangeCB.addItem("10E-3")
            self.XRangeCB.addItem("5E-3")
            self.XRangeCB.addItem("2E-3")
            self.XRangeCB.addItem("1E-3")
            self.XRangeCB.addItem("500E-6")
            self.XRangeCB.addItem("200E-6")
            self.XRangeCB.addItem("100E-6")
            self.XRangeCB.addItem("50E-6")
            self.XRangeCB.addItem("20E-6")
            self.XRangeCB.addItem("10E-6")
            self.XRangeCB.addItem("5E-6")
            self.XRangeCB.addItem("2E-6")
            self.XRangeCB.addItem("1E-6")
            self.XRangeCB.setEditable(True)
            line = line + 1
            glayout.addWidget(self.XRangeCB, line, 1)
            glayout.addWidget(QLabel("XRangeCB"), line,0)

            self.YOffsetSB = QDoubleSpinBox()
            self.YOffsetSB.setMinimum(-10)
            self.YOffsetSB.setMaximum(10)
            self.YOffsetSB.setDecimals(5)
            line = line + 1
            glayout.addWidget(self.YOffsetSB, line, 1)
            glayout.addWidget(QLabel("YOffset"), line,0)

            self.YRangeCB = QComboBox()
            self.YRangeCB.addItem("10")
            self.YRangeCB.addItem("5")
            self.YRangeCB.addItem("2")
            self.YRangeCB.addItem("500E-3")
            self.YRangeCB.addItem("200E-3")
            self.YRangeCB.addItem("100E-3")
            self.YRangeCB.addItem("50E-3")
            self.YRangeCB.addItem("20E-3")
            self.YRangeCB.addItem("10E-3")
            self.YRangeCB.addItem("5E-3")
            self.YRangeCB.setEditable(True)
            line = line + 1
            glayout.addWidget(self.YRangeCB, line, 1)
            glayout.addWidget(QLabel("YRange"), line,0)
            
            layout.addWidget(self.scopeSetupCommands)
            layout.addItem(glayout)
                  
            self.masterLayout = layout
                        
    def getLayout(self):
            return self.masterLayout
