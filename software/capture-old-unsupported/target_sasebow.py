# -*- coding: cp1252 -*-
# This file is part of AESExplorer
# Copyright (C) Colin O'Flynn 2012
# See www.newae.com for details
#
# Released under LGPL License

import sys
import os
import threading
import time
import logging
import math
import serial

class SASEBOW():
    
    def __init__(self):
        self.ser = None

    def __del__(self):
        self.close()
    
    def connect(self, serport):
        if self.ser == None:
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = serport
            self.ser.baudrate = 9600
            self.ser.timeout  = 3     # 3 second timeout
            self.ser.stopbits = serial.STOPBITS_TWO
            self.ser.parity = serial.PARITY_EVEN
            self.ser.open()

        self.reset()

    def reset(self):
        self.ser.setRTS(True)
        time.sleep(0.2)
        self.ser.setRTS(False)
        time.sleep(0.5)

        data = bytearray(self.ser.read(self.ser.inWaiting()))
        stratr = "ATR = "
        for p in data:
            stratr = stratr + "%2X "%p

        print stratr
        self.stratr = stratr
        return stratr
    
    def disconnect(self):
        self.close()
            
    def close(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        
    def init(self):
        self.reset()

    def getATR(self):
        return self.stratr

    def APDUHeader(self, cla, ins, p1, p2, datalen):
        header = bytearray()
        header.append(cla)
        header.append(ins)
        header.append(p1)
        header.append(p2)
        header.append(datalen)

        self.ser.flushInput()
        self.ser.write(header)
       
        try:
            resp = bytearray(self.ser.read(1))
            if (resp[0] != ins):
                print "SASEBOW: APDUHeader Sync Error %X != %X"%(resp[0], ins)
                return False
        except:
            print "SASEBOW: APDUHeaderSync Error, no response"
            return False

        return True

    def APDUSend(self, cla, ins, p1, p2, data):
        self.APDUHeader(cla, ins, p1, p2, len(data))   
        self.ser.write(bytearray(data))
        
        resp = self.ser.read(2)

        if len(resp) < 2:
            print "SASEBOW: APDUSend Data Error, no response"

        resp = bytearray(resp)

        resp = (resp[0] << 8) | resp[1]

        #print resp

        return resp

    def APDURecv(self, cla, ins, p1, p2, datalen):
        self.APDUHeader(cla, ins, p1, p2, datalen)
        
        resp = self.ser.read(datalen + 2)

        if len(resp) < (datalen + 2):
            print "SASEBOW: APDUResp Data Error, response too short"
            return 0

        resp = bytearray(resp)
        
        return resp[0:datalen]        
                    
      
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        if key != None:
            self.APDUSend(0x80, 0x12, 0x00, 0x00, key)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):
        return self.APDURecv(0x80, 0xC0, 0x00, 0x00, 16)

    def go(self):
        self.APDUSend(0x80, 0x04, 0x04, 0x00, self.input)
   
