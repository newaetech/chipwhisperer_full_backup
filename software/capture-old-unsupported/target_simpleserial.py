# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
import serial

class SimpleSerial():
    
    def __init__(self):
        self.ser = None

    def __del__(self):
        self.close()
    
    def connect(self, serport, baud=38400):
        if self.ser == None:
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = serport
            self.ser.baudrate = baud
            self.ser.timeout  = 2     # 2 second timeout
            self.ser.open()

        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
        self.ser.flush()

    def disconnect(self):
        self.close()
    
    def close(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        return
        
    def init(self):
        return
      
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        if key != None:            
            cmd = "k"
            for b in key:
                cmd = cmd + "%2x"%b
            cmd = cmd + "\n"
            self.ser.flushInput()
            self.ser.write(cmd)
            #self.ser.read(1)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        response = self.ser.read(33)

        if len(response) < 33:
            print "WARNING: Response too short!"
            return None

        if response[0] != 'r':
            print "Sync Error"
            return None

        data = bytearray(16)

        for i in range(0,16):
            data[i] = int(response[(i*2+1):(i*2+3)], 16)

        return data

    def go(self):
        self.ser.flushInput()
        cmd = "p"
        for b in self.input:
            cmd = cmd + "%2x"%b
        cmd = cmd + "\n"
        self.ser.flushInput()
        self.ser.write(cmd)
        #self.ser.read(1)
   
