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
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../../common')
sys.path.append('../.')
from ExtendedParameter import ExtendedParameter

from utils.SerialProtocols import strToBits as strToBits
from utils.SerialProtocols import CWCalcClkDiv as calcClkDiv

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

ADDR_EXTCLK = 38
ADDR_TRIGSRC = 39
ADDR_TRIGMOD = 40
        
class CWUniversalSerialProcessor(object):
    def bitsToTxPattern(self, bits, samplesperbit=1, idle=1):
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
    
    def averageBits(self, bits):
        ones = 0
        zeros = 0
        for b in bits:
            if b == 1:
                ones += 1
            else:
                zeros += 1
        
        if ones >= zeros:
            return 1
        else:
            return 0
        
    def numOnes(self, bits):
        ones = 0
        for b in bits:
            if b == 1:
                ones += 1

        return ones
    
    def rxPatternToString(self, rxpattern, samplesperbit=1, idle=1, startbits=1, stopbits=1, parity="none"):
        bits = []
        
        #Flip bits around
        for b in rxpattern:
            for i in range(0,  8):
                bit = (b >> i) & 0x01
                bits.append(bit)
            
        bytelist = []
        
        #print bits
                
        #Find start bit
        bindex = 0
        
        #print samplesperbit
        bits[0] = 0
        
        
        while bindex < len(bits):
            
            if self.averageBits(bits[bindex:(bindex+samplesperbit)]) == 0:
            #if self.numOnes(bits[bindex:(bindex+samplesperbit)]) <= 3:
            #if bits[bindex:(bindex+samplesperbit)] == [0]*samplesperbit:                
                #print "found start bit @ %d"%bindex
                #if bindex < samplesperbit:
                #    bindex += 1#samplesperbit
                #else:               
                bindex += samplesperbit
                
                for n in range(1, startbits):
                    if self.averageBits(bits[bindex:(bindex+samplesperbit)]) != 0:
                        print "WARNING: USI Missing expected start bit"
                    bindex += samplesperbit
                
                byte = 0
                
                onecnt = 0
        
                for b in range(0,8):
                    bitstate = self.averageBits(bits[bindex:(bindex+samplesperbit)])
                    onecnt += bitstate
                    byte |= bitstate << b
                    bindex += samplesperbit
                    
                bytelist.append(byte)
                
                if parity != "none":
                    #TODO: Check/Validate parity
                    paritystate = self.averageBits(bits[bindex:(bindex+samplesperbit)])
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
                    if self.averageBits(bits[bindex:(bindex+samplesperbit)]) != 1:
                        print "WARNING: USI Missing expected stop bit @ point=%d,byte=%d"%(bindex,len(bytelist))
                    #else:
                    #    print "found stop bit"
                    bindex += samplesperbit
                                    
            else:
                #TODO: only works with oversampling = 8
                bindex += samplesperbit/4
                
        s = "".join([chr(b) for b in bytelist])    
        return s
        
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
        
        self.oversamplerate=12
        self.guardbits=1
        self.stopbits=1
        self.startbits=1
        self.parity="none"
        
        self.proc = CWUniversalSerialProcessor()
    
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
            return 1
        else:
            return 0
        
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
        tosend = self.proc.bitsToTxPattern(self.proc.strToBits("Hello"), 4)
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
            
        print self.proc.rxPatternToString(result, 4)
        
        
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
            self.maxstored = (totalbits/8)+8
            
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
            
        return self.proc.rxPatternToString(result, self.oversamplerate, startbits=self.startbits, stopbits=1, parity=self.parity) #self.stopbits
        
        
    def write(self, string, wait=True):
        """
        Send a string over the USI.
        """

        self.setRunTx(False)
        self.setIdle(1)        
        tosend = self.proc.bitsToTxPattern(strToBits(string, startbits=self.startbits, stopbits=self.stopbits, parity=self.parity, guardbits=self.guardbits), samplesperbit=self.oversamplerate)
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

