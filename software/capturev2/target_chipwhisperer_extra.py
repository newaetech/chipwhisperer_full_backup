# -*- coding: utf -*-
# This file is part of ChipWhisperer
# Copyright (C) Colin O'Flynn 2013
# See www.newae.com for details
#
# Released under GPL License

import sys
import os
import threading
import time
import logging
import math
import serial

from PySide.QtCore import *
from PySide.QtGui import *

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

ADDR_DATA       = 33
ADDR_LEN        = 34
ADDR_BAUD       = 35

ADDR_TRIGCLKDIV = 36
ADDR_TRIGIOPROG = 37

ADDR_EXTCLK = 38
ADDR_TRIGSRC = 39
ADDR_TRIGMOD = 40

class QtInterface(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()
    
        self.tb = QToolBox()
        layout.addWidget(self.tb)

        genconfig = QWidget()
        genlayout = QVBoxLayout()
        genconfig.setLayout(genlayout)

        self.cwADV = CWAdvTriggerQT()
        self.cwEXTRA = CWExtraSettingsQT()

    def setOpenADC(self, oa):
        self.cwADV.con(oa)
        self.cwEXTRA.con(oa)

    def testPattern(self):
        desired_freq = 38400 * 3
        clk = 30E6
        clkdivider = (clk / (2 * desired_freq)) + 1        
        self.cwADV.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)

    def loadSettings(self, settings):
        self.targset = settings.addGroup("CW Capture", self)
        settings.addGroupItem(self.targset, "Advanced IO Match", self.cwADV)
        settings.addGroupItem(self.targset, "Trigger Settings", self.cwEXTRA)

class CWExtraSettings(object):
    PIN_FPA = 0x01
    PIN_FPB = 0x02
    PIN_RTIO1 = 0x04
    PIN_RTIO2 = 0x08
    PIN_RTIO3 = 0x10
    PIN_RTIO4 = 0x20
    MODE_OR = 0x00
    MODE_AND = 0x01
    
    def __init__(self):
        super(CWExtraSettings, self).__init__()
        self.oa = None
    
    def con(self, oa):
        self.oa = oa
           
    def setPins(self, pins, mode):
        d = bytearray()        
        d.append((mode << 6) | pins)  
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGSRC, d)
        
    def getPins(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGSRC, Validate=False, maxResp=1)
        pins = resp[0] & 0x3F;
        mode = resp[0] >> 6;
        return(pins, mode)
        
class CWExtraSettingsQT(CWExtraSettings,  QWidget):        
    def __init__(self):
        super(CWExtraSettingsQT,  self).__init__()
        adviolayout = QVBoxLayout()
        self.setLayout(adviolayout)

        pins = QHBoxLayout()
        self.pinFPA = QCheckBox('Front Panel A')
        self.pinFPB = QCheckBox('Front Panel B')
        self.pinIO1 = QCheckBox('Target IO1')
        self.pinIO2 = QCheckBox('Target IO2')
        self.pinIO3 = QCheckBox('Target IO3')
        self.pinIO4 = QCheckBox('Target IO4')
        
        pins.addWidget(self.pinFPA)
        pins.addWidget(self.pinFPB)
        pins.addWidget(self.pinIO1)
        pins.addWidget(self.pinIO2)
        pins.addWidget(self.pinIO3)
        pins.addWidget(self.pinIO4)
        
        self.pinFPA.clicked.connect(self.pinsModeChanged)
        self.pinFPB.clicked.connect(self.pinsModeChanged)
        self.pinIO1.clicked.connect(self.pinsModeChanged)
        self.pinIO2.clicked.connect(self.pinsModeChanged)
        self.pinIO3.clicked.connect(self.pinsModeChanged)
        self.pinIO4.clicked.connect(self.pinsModeChanged)
        
        adviolayout.addLayout(pins)         
        
        pinmode = QHBoxLayout()
        self.CBMode = QComboBox()
        self.CBMode.addItem("OR", self.MODE_OR)
        self.CBMode.addItem("AND",  self.MODE_AND)
        pinmode.addWidget(QLabel("Collection Mode:"))
        pinmode.addWidget(self.CBMode)
        self.CBMode.currentIndexChanged.connect(self.pinsModeChanged)     
        adviolayout.addLayout(pinmode)

    def pinsModeChanged(self,  indx=0):
        mode = self.CBMode.itemData(self.CBMode.currentIndex())
        pins = 0
        
        if self.pinFPA.isChecked(): pins |= self.PIN_FPA
        if self.pinFPB.isChecked(): pins |= self.PIN_FPB
        if self.pinIO1.isChecked(): pins |= self.PIN_RTIO1   
        if self.pinIO2.isChecked(): pins |= self.PIN_RTIO2 
        if self.pinIO3.isChecked(): pins |= self.PIN_RTIO3 
        if self.pinIO4.isChecked(): pins |= self.PIN_RTIO4 
        
        self.setPins(pins,  mode)

    def getPins(self):
        (pins, mode) = self.getPins()
        
        self.pinFPA.setChecked(pins & self.PIN_FPA)
        self.pinFPB.setChecked(pins & self.PIN_FPB)
        self.pinIO1.setChecked(pins & self.PIN_RTIO1)
        self.pinIO2.setChecked(pins & self.PIN_RTIO2)
        self.pinIO3.setChecked(pins & self.PIN_RTIO3)
        self.pinIO4.setChecked(pins & self.PIN_RTIO4)
        
        #This will force callback to pinsModeChanged so needs to
        #be last thing
        self.CBMode.setIndex(self.CBMode.findData(mode))

    def setOpenADC(self, oa):
        self.con(oa)

def strToPattern(string, startbits=1, stopbits=1):
    totalpat = []
    for s in string:
        bp = ord(s)
        #Start bits
        for i in range(0, startbits):
            totalpat.append(0)
            
        for i in range(7, -1, -1):
            bit = (bp >> i) & 0x01
            totalpat.append(bit)

        #Stop bits
        for i in range(0, stopbits):
            totalpat.append(1)

    return bitsToPattern(totalpat)

def processBit(state, cnt, first=False, last=False, var=1, osRate=3):
    cnt = osRate * cnt
    low = cnt - var
    high = cnt + var

    if low < 1:
        low = 1

    if high > 509:
        high = 509

    if first:
        high = 'wait'

    if last:
        high = 'now'    

    return [state, low, high]    

def bitsToPattern(bits):
    pattern = []
    lastbit = 2
    bitcnt = 0
    first = True
    for b in bits:
        if b == lastbit:
            bitcnt = bitcnt + 1
        else:
            if bitcnt > 0:
                #print "%d %d"%(lastbit, bitcnt)
                pattern.append(processBit(lastbit, bitcnt, first=first))
            lastbit = b
            bitcnt = 1

        first = False
    pattern.append(processBit(lastbit, bitcnt, last=True))
    return pattern
   
class CWAdvTrigger(object):
    def __init__(self):
        super(CWAdvTrigger, self).__init__()
        self.oa = None
    
    def con(self, oa):
        self.oa = oa

    def setExtPin(self, line):
        return

    def setIOPattern(self, pattern, form='units', clkdiv=None):
        ''' Setup the IO trigger pattern.'''
        "Form can be 'units' or 'seconds'. If set to 'units' you"
        "must also set the clkdiv parameter properly"        

        #Pattern format:
        #[state, low, high]
        #state = 1/0
        #low = time in seconds/units
        #high = time in seconds/units
        #Special high variables:
        #Set to 'now' means next state after low time passes
        #high = 'now'        
        #Set to 'wait' means no upper limit, but wait for transition
        #of IO line before changing state
        #high = 'wait'

        addr = 0        
        for p in pattern:
            state = p[0]
            low = p[1]
            high = p[2]        
            #TODO: support seconds
            if form == 'seconds':
                print "Not Supported"

            if high == 'now':
                high = 511

            if high == 'wait':
                high = 510
                
            self.writeOnePattern(addr, state, low, high)
            addr = addr + 1

        #Set done marker
        self.writeOnePattern(addr, 1, 255, 511)

        if clkdiv:
            self.writeClockDiv(clkdiv)

    def writeClockDiv(self, clkdiv):
        clkdiv = int(clkdiv)
        d = bytearray()        
        d.append(clkdiv & 0xff)
        d.append((clkdiv >> 8) & 0xff)
        d.append((clkdiv >> 16) & 0xff)        
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGCLKDIV, d)

    def reset(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGCLKDIV, Validate=False, maxResp=3)
        #Assert Reset
        resp[2] = resp[2] | 0x80
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGCLKDIV, resp)

        #Deassert Reset
        resp[2] = resp[2] & 0x7F;
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGCLKDIV, resp)

    def writeOnePattern(self, addr, state, low, high):
        d = bytearray()
        d.append(low)
        d.append(high & 0xff)
        d.append((0x01 & (high >> 8)) | (state << 1))
        d.append(addr)        
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGIOPROG, d)          
      
    
class CWAdvTriggerQT(CWAdvTrigger,  QWidget):        
    def __init__(self):
        super(CWAdvTriggerQT,  self).__init__()
        adviolayout = QVBoxLayout()
        self.setLayout(adviolayout)

        pbTest = QPushButton("Test Pattern")
        pbTest.clicked.connect(self.testPattern)
        adviolayout.addWidget(pbTest)        

    def setOpenADC(self, oa):
        self.con(oa)

    def testPattern(self):
        desired_freq = 38400 * 3
        clk = 30E6
        clkdivider = (clk / (2 * desired_freq)) + 1        
        self.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)
        
