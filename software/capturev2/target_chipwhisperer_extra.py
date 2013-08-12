# -*- coding: utf -*-
# This file is part of ChipWhisperer
# Copyright (C) Colin O'Flynn 2013
# See www.newae.com for details
#
# Released under GPL License

import sys
from functools import partial
import time

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
     
    def __init__(self, showScriptParameter=None):
        #self.cwADV = CWAdvTrigger()
        self.cwEXTRA = CWExtraSettings()        
        self.cwExtraParams = Parameter.create(name='CW Extra', type='group', children=self.cwEXTRA.param)
        ExtendedParameter.setupExtended(self.cwExtraParams, self)
        self.showScriptParameter = showScriptParameter
        
        self.cwUSI = CWUniversalSerial()
        
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setOpenADC(self, oa):
        #self.cwADV.setOpenADC(oa)
        self.cwEXTRA.con(oa.sc)
        self.cwExtraParams.getAllParameters()
        
        self.cwUSI.con(oa.sc)

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

def strToBits(string, startbits=1, stopbits=1, parity="none", guardbits=0):
    """Converts a string into a standard serial pattern"""
    totalpat = []
    for s in string:
        
        try:        
            bp = ord(s)
        except:
            bp = s
            
        if s is None:
            for i in range(0, startbits):
                totalpat.append(1)
                
            for i in range(0, 8):
                totalpat.append(1)
                
            if parity != "none":
                totalpat.append(1)
                
            for i in range(0, stopbits):
                totalpat.append(1)
                
            for i in range(0, guardbits):
                totalpat.append(1)
        else:
                
            #Start bits
            for i in range(0, startbits):
                totalpat.append(0)
             
            onebits = 0
             
            #Following for MSB first:   
            #for i in range(7, -1, -1):
            #Serial protocols LSB first:
            for i in range(0,  8):
                bit = (bp >> i) & 0x01
                if bit:
                    onebits += 1
                totalpat.append(bit)
    
            #Parity bits
            if (onebits % 2) == 0:
                if parity == "even":
                    totalpat.append(0)
                elif parity == "odd":
                    totalpat.append(1)
            else:
                if parity == "even":
                    totalpat.append(1)
                elif parity == "odd":
                    totalpat.append(0)            
    
            #Stop bits
            for i in range(0, stopbits):
                totalpat.append(1)
    
            for i in range(0, guardbits):
                totalpat.append(1)       
                
    return totalpat

def strToPattern(string, startbits=1, stopbits=1):
    totalpat = strToBits(string, startbits, stopbits)
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
   
def bitsToTxPattern(bits, samplesperbit=1, idle=1):
    txpattern = []
    
    #Extend if needed
    for b in bits:
        for i in range(0, samplesperbit):
            txpattern.append(b)
    
    #Extend to make multiple of 8 bits
    if (len(txpattern) % 8) != 0:
        extend = 8 - (len(txpattern) % 8)
        for i in range(0, extend):
            txpattern.append(1)
        
    blist = []
            
    #Group into 8 bits
    for gnum in range(0, len(txpattern)/8):
        start = gnum * 8
        byte = 0
        for b in range(0,8):
            byte |= txpattern[start+b] << b;
            
        blist.append(byte)
        
    return blist   

def averageBits(bits):
    ones = 0
    zeros = 0
    for b in bits:
        if b == 1:
            ones += 1
        else:
            zeros += 1
    
    if ones > zeros:
        return 1
    else:
        return 0

def rxPatternToString(rxpattern, samplesperbit=1, idle=1, startbits=1, stopbits=1, parity="none"):
    bits = []
    
    #Flip bits around
    for b in rxpattern:
        for i in range(0,  8):
            bit = (b >> i) & 0x01
            bits.append(bit)
        
    bytelist = []
    
    print bits
            
    #Find start bit
    bindex = 0
    
    while bindex < len(bits):
        if averageBits(bits[bindex:(bindex+samplesperbit)]) == 0:
            #print "found start bit @ %d"%bindex
            bindex += samplesperbit
            
            for n in range(1, startbits):
                if averageBits(bits[bindex:(bindex+samplesperbit)]) != 0:
                    print "WARNING: USI Missing expected start bit"
                bindex += samplesperbit
            
            byte = 0
            
            onecnt = 0
    
            for b in range(0,8):
                bitstate = averageBits(bits[bindex:(bindex+samplesperbit)])
                onecnt += bitstate
                byte |= bitstate << b
                bindex += samplesperbit
                
            bytelist.append(byte)
            
            if parity != "none":
                #TODO: Check/Validate parity
                paritystate = averageBits(bits[bindex:(bindex+samplesperbit)])
                bindex += samplesperbit
                
                if (onecnt % 2) == 0:
                    #Even number of 1's present
                    if parity == "even":
                        if paritystate == 1:
                            print "WARNING: USI parity error"
                    elif parity == "odd":
                        if paritystate == 0:
                            print "WARNING: USI parity error"
                 
                else:
                    #Even number of 1's present
                    if parity == "even":
                        if paritystate == 0:
                            print "WARNING: USI parity error"
                    elif parity == "odd":
                        if paritystate == 1:
                            print "WARNING: USI parity error"
                            
            for n in range(0, stopbits):       
                if averageBits(bits[bindex:(bindex+samplesperbit)]) != 1:
                    print "WARNING: USI Missing expected stop bit"
                #else:
                #    print "found stop bit"
                bindex += samplesperbit
            
        else:
            bindex += samplesperbit
            
    s = "".join([chr(b) for b in bytelist])    
    return s
    
 
def calcClkDiv(sysclk, targfreq):
    clkdivider = int((sysclk / (2 * targfreq)) + 1)   
    
    actual = sysclk / (2*(clkdivider-1))
    error = (float(actual - targfreq) / targfreq)*100.0        
    
    return (clkdivider, error)
    
    #    desired_freq = 38400 * 3
    #    clk = 30E6
    #    clkdivider = (clk / (2 * desired_freq)) + 1        
    #    self.cwADV.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)

    
   
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
        
class CWUniversalSerial(object):
    
    ADDR_USICLKDIV = 44
    ADDR_USIPROG = 45
    ADDR_USISTATUS = 46
    
    MASK_IDLE = 0x01
    MASK_TXDONE = 0x02
    MASK_TXGO = 0x04
    MASK_RXDONE = 0x08
    MASK_RXGO = 0x10
    MASK_WREN = 0x80
    
    
    def __init__(self):
        super(CWUniversalSerial, self).__init__()
        self.oa = None
        
        self.oversamplerate=4
        self.guardbits=1
        self.stopbits=1
        self.startbits=1
        self.parity="none"
    
    def con(self, oa):
        self.oa = oa

            
    def setIdle(self, idle):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        resp[0] = (resp[0] & 0xFE)
         
        if idle:
            resp[0] |= self.MASK_IDLE
            
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USISTATUS, resp, Validate=False)
        
    def idle(self):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        if (resp[0] & self.MASK_IDLE):
            return True
        else:
            return False
        
    def setRunTx(self, stat):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        resp[0] &= ~(self.MASK_TXGO)
        if stat:
            resp[0] |= self.MASK_TXGO
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USISTATUS, resp, Validate=False) 
        
    def setRunRx(self, stat):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        resp[0] &= ~(self.MASK_RXGO)
        if stat:
            resp[0] |= self.MASK_RXGO
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USISTATUS, resp, Validate=False)        
        
    def doneTx(self):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        if (resp[0] & self.MASK_TXDONE):
            return True
        return False
    
    def doneRx(self):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        if (resp[0] & self.MASK_RXDONE):
            return True
        return False
        
    def setMaxTx(self, maxStates):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        resp[1] &= 0x0f;
        resp[1] |= (maxStates >> 4) & 0xf0;
        resp[3] = maxStates & 0xff;
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USISTATUS, resp, Validate=False)
        
    def setMaxRx(self, maxStates):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USISTATUS, Validate=False, maxResp=4)
        resp[1] &= 0xf0;
        resp[1] |= (maxStates >> 8) & 0x0f;
        resp[2] = maxStates & 0xff;
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USISTATUS, resp, Validate=False)
        
    def writeClockDiv(self, clkdiv):
        clkdiv = int(clkdiv)
        d = bytearray()        
        d.append(clkdiv & 0xff)
        d.append((clkdiv >> 8) & 0xff)
        d.append((clkdiv >> 16) & 0xff)        
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USICLKDIV, d)

    def reset(self):
        resp = self.oa.sendMessage(CODE_READ, self.ADDR_USICLKDIV, Validate=False, maxResp=3)
        #Assert Reset
        resp[2] = resp[2] | 0x80
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USICLKDIV, resp)

        #Deassert Reset
        resp[2] = resp[2] & 0x7F;
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USICLKDIV, resp)          

    def writeSequence(self, addr, data):
        d = bytearray()
        d.append(data)
        d.append((addr) & 0xff)
        d.append((addr >> 8) & 0xff)
        d.append((addr >> 16) & 0x7f | 0x80)
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USIPROG, d, Validate=False)
        
    def readSequence(self, addr):
        d = bytearray()
        d.append(0)
        d.append((addr) & 0xff)
        d.append((addr >> 8) & 0xff)
        d.append((addr >> 16) & 0xff)
        self.oa.sendMessage(CODE_WRITE, self.ADDR_USIPROG, d, Validate=False)
        rv = self.oa.sendMessage(CODE_READ, self.ADDR_USIPROG, Validate=False, maxResp=4)
        return rv[0]
        
    def test(self):
        self.setRunTx(False)
        self.setRunRx(False)
        self.writeClockDiv(65)#3125
        tosend = bitsToTxPattern(strToBits("Hello"), 4)
        for i in range(0, len(tosend)):
            self.writeSequence(i, tosend[i])
        self.setMaxTx(len(tosend))
        self.setIdle(1)
        
        self.setMaxRx(50)
        self.setRunRx(True)
        
        self.setRunTx(True)
        
        while(self.doneRx() == False):
            continue
        
        result = []
        for t in range(0,50):
            result.append(self.readSequence(t))
        #    print "%02x "%result[t],
        #print ""
            
        print rxPatternToString(result, 4)
        
        
    def setBaud(self, baud=9600, oversamplerate=None):        
        if oversamplerate:
            self.oversamplerate = oversamplerate
        resp = calcClkDiv(self.oa.hwInfo.sysFrequency(), baud*self.oversamplerate)
        self.error = resp[1]
        self.writeClockDiv(resp[0])
        return self.error        
    
    def setStartbits(self, sb):
        self.startbits = sb
        
    def setStopbits(self, sb):
        self.stopbits = sb
        
    def setParity(self, par):
        if (par != "none") & (par != "even") & (par != "odd"):
            raise ValueError("Invalid settings for parity: %s"%par)
        
        
        self.parity = par
        
    def read(self, maxbytes=10, timeout=0.5, startonly=False, waitonly=False):
        """
        Wait for the RX USI to start capturing. Will record the state of the serial line such that
        up to maxBytes could be recorded. Must have set startbit,stopbit,guardbits correctly. Note
        this doesn't wait for maxbytes - the hardware is incapable of knowing how many bytes have
        been received, it just records a certain amount of time
        """
        if waitonly == False:
            self.setIdle(1)  
            self.setRunRx(False)        
                   
            if self.parity == "none":
                pbits = 0
            else:
                pbits = 1
                   
            totalbits = (8+self.startbits+self.stopbits+self.guardbits+pbits)*maxbytes*self.oversamplerate
            self.maxstored = (totalbits/8)+24
            
            self.setMaxRx(self.maxstored)
            self.setRunRx(True)
        
        if startonly:
            return
        
        
        while(self.doneRx() == False):
            time.sleep(0.01)
            timeout -= 0.01
            if timeout <= 0:
                raise IOError("Timeout in readString")
        
        result = []
        for t in range(0,self.maxstored):
            result.append(self.readSequence(t))
            
        self.setRunRx(False)
            
        return rxPatternToString(result, self.oversamplerate, startbits=self.startbits, stopbits=1, parity=self.parity) #self.stopbits
        
        
    def write(self, string, wait=True):
        """
        Send a string over the USI.
        """

        self.setRunTx(False)
        self.setIdle(1)        
        tosend = bitsToTxPattern(strToBits(string, startbits=self.startbits, stopbits=self.stopbits, parity=self.parity, guardbits=self.guardbits), samplesperbit=self.oversamplerate)
        for i in range(0, len(tosend)):
            self.writeSequence(i, tosend[i])
        self.setMaxTx(len(tosend))        
        self.setRunTx(True)
                
        if wait:
            while(self.doneTx() == False):
                time.sleep(0.01)
            
            self.setRunTx(False)     
    
    def test2(self):
        self.setBaud(9600)
        self.setParity("even")        
        self.setStopbits(2)
        self.write([0x80,0x04, 0x04, 0x00, 0x10, None, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01])
        
    def test3(self):
        self.read(16, startonly=True)
        self.write([0x80,0xC0, 0x00, 0x00, 0x10])
        p = bytearray(self.read(16, waitonly=True))
        for t in p:
            print "%2x "%t,
        
    
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
        
