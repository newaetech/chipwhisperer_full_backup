# -*- coding: utf -*-
# This file is part of ChipWhisperer
# Copyright (C) Colin O'Flynn 2013
# See www.newae.com for details
#
# Released under GPL License

import sys
from functools import partial

from PySide.QtCore import *
from PySide.QtGui import *

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
from ExtendedParameter import ExtendedParameter

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

class QtInterface:
    paramListUpdated = Signal(list)
     
    def __init__(self):
        #self.cwADV = CWAdvTrigger()
        self.cwEXTRA = CWExtraSettings()        
        self.cwExtraParams = Parameter.create(name='CW Extra', type='group', children=self.cwEXTRA.param)
        ExtendedParameter.setupExtended(self.cwExtraParams)

    def setOpenADC(self, oa):
        #self.cwADV.setOpenADC(oa)
        self.cwEXTRA.con(oa.sc)
        self.cwExtraParams.getAllParameters()

    def paramList(self):
        p = []
        p.append(self.cwExtraParams)            
        return p

    #def testPattern(self):
    #    desired_freq = 38400 * 3
    #    clk = 30E6
    #    clkdivider = (clk / (2 * desired_freq)) + 1        
    #    self.cwADV.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)

    
    

class CWExtraSettings(object):
    PIN_FPA = 0x01
    PIN_FPB = 0x02
    PIN_RTIO1 = 0x04
    PIN_RTIO2 = 0x08
    PIN_RTIO3 = 0x10
    PIN_RTIO4 = 0x20
    MODE_OR = 0x00
    MODE_AND = 0x01
    
    MODULE_BASIC = 0x00
    MODULE_ADVPATTERN = 0x01
    
    def __init__(self):
        super(CWExtraSettings, self).__init__()
        self.oa = None
    
        self.name = "CW Extra Settings"        
        self.param = [{'name': 'CW Extra Settings', 'type': 'group', 'children': [
                {'name': 'Trigger Pins', 'type':'group', 'children':[
                    {'name': 'Front Panel A', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_FPA), 'set':partial(self.setPin, pin=self.PIN_FPA)},
                    {'name': 'Front Panel B', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_FPB), 'set':partial(self.setPin, pin=self.PIN_FPB)},
                    {'name': 'Target IO1 (Serial TXD)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO1), 'set':partial(self.setPin, pin=self.PIN_RTIO1)},
                    {'name': 'Target IO2 (Serial RXD)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO2), 'set':partial(self.setPin, pin=self.PIN_RTIO2)},
                    {'name': 'Target IO3 (SmartCard Serial)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO3), 'set':partial(self.setPin, pin=self.PIN_RTIO3)},
                    {'name': 'Target IO4 (Trigger Line)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO4), 'set':partial(self.setPin, pin=self.PIN_RTIO4)},
                    {'name': 'Collection Mode', 'type':'list', 'values':{"OR":self.MODE_OR, "AND":self.MODE_AND}, 'value':"OR", 'get':self.getPinMode, 'set':self.setPinMode }                                      
                    ]},
                {'name': 'Trigger Module', 'type':'list', 'values':{"Basic (Edge/Level)":self.MODULE_BASIC, "Digital Pattern Matching":self.MODULE_ADVPATTERN}, 'value':self.MODULE_BASIC, 'set':self.setModule, 'get':self.getModule},
                {'name': 'Trigger Out on FPA', 'type':'bool', 'value':False, 'set':self.setTrigOut},
                ]}]
    
    def con(self, oa):
        self.oa = oa
           
    def setPin(self, enabled, pin):
        current = self.getPins()
        
        pincur = current[0] & ~(pin)
        if enabled:
            pincur = pincur | pin
            
        self.setPins(pincur, current[1])
        
            
    def getPin(self, pin):
        current = self.getPins()        
        current = current[0] & pin
        if current == 0:
            return False
        else:
            return True
    
    def setPinMode(self, mode):
        current = self.getPins()
        self.setPins(current[0], mode)
    
    def getPinMode(self):
        current = self.getPins()
        return current[1]
           
    def setPins(self, pins, mode):
        d = bytearray()        
        d.append((mode << 6) | pins)  
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGSRC, d)
        
    def getPins(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGSRC, Validate=False, maxResp=1)
        pins = resp[0] & 0x3F
        mode = resp[0] >> 6
        return(pins, mode)
        
    def setModule(self,  module):        
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)        
        resp[0] = resp[0] & 0xF8
        resp[0] = resp[0] | module      
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)
        
    def getModule(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return resp[0] 
        
    def setTrigOut(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)        
        resp[0] = resp[0] & 0xE7    
        if enabled:
            resp[0] = resp[0] | 0x08
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp) 

def strToPattern(string, startbits=1, stopbits=1):
    totalpat = []
    for s in string:
        bp = ord(s)
        #Start bits
        for i in range(0, startbits):
            totalpat.append(0)
         
        #Following for MSB first:   
        #for i in range(7, -1, -1):
        #Serial protocols LSB first:
        for i in range(0,  8):
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
        
