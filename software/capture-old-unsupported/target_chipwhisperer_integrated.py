# -*- coding: cp1252 -*-
# This file is part of AESExplorer
# Copyright (C) Colin O'Flynn 2012
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

class CWQt(QWidget):
    def __init__(self, mw=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        self.parent = mw
    
        self.connectButton = QPushButton("Connect")
        self.disconnectButton = QPushButton("Disconnect")
        self.resetButton = QPushButton("Reset")
        self.statusButton = QPushButton("Updated Status")           
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
        layout.addWidget(self.Statuslabel)

        self.disconnectButton.setEnabled(False)
        self.resetButton.setEnabled(False)
        self.statusButton.setEnabled(False)

        self.target = CWSimpleSerial()


        self.tb = QToolBox()
        layout.addWidget(self.tb)

        genconfig = QWidget()
        genlayout = QGridLayout()
        genconfig.setLayout(genlayout)
        self.tb.addItem(genconfig, "General Configuration")

        self.setLayout(layout)

    def __del__(self):
        if self.target != None:
            self.dis()

    def update(self):
        self.Statuslabel.setText("don't look at me")

    def con(self):
        try:
            self.target.connect(self.parent.tw.widget(1).scope.sc)
            self.disconnectButton.setEnabled(True)
            self.connectButton.setEnabled(False)
            self.resetButton.setEnabled(True)
            self.statusButton.setEnabled(True)
        except:
            print "Unexpected error:", sys.exc_info()

    def res(self):
        self.target.reset()
                    
    def dis(self):
        self.disconnectButton.setEnabled(False)
        self.connectButton.setEnabled(True)
        self.resetButton.setEnabled(False)
        self.statusButton.setEnabled(False)
        self.target.disconnect()

class CWSmartcard():
    def __init__(self):
        self.oa = None
    
    def connect(self, oa):
        self.oa = oa 

class CWSimpleSerial():    
    def __init__(self):
        self.oa = None

    def reset(self):
        return
        
    def connect(self, oa):        
        self.oa = oa
        #data = bytearray([0x62])
        #self.oa.sendMessage(CODE_WRITE, ADDR_DATA, data, Validate=False)

        #data = bytearray([0x66])
        #self.oa.sendMessage(CODE_WRITE, ADDR_DATA, data, Validate=False)

        #resp = self.oa.sendMessage(CODE_READ, ADDR_LEN, Validate=False, maxResp=2)
        #print "%d %d"%(resp[0],resp[1])

        #if resp[1] > 0:
        #    print "%d bytes waiting"%resp[1]
        #    data = self.oa.sendMessage(CODE_READ, ADDR_DATA, Validate=False)
        #    print "%x"%data[0]

        self.write("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
        self.flush()
        self.write("p12121212121212121212121212121212\n")
        #Give time for response
        time.sleep(0.05)
        
        resp = self.read(num=33)
        print resp

    def write(self, string):
        for s in string:
            d = bytearray(s)
            #print "%x"%d[0]
            self.oa.sendMessage(CODE_WRITE, ADDR_DATA, d, Validate=False)

    def bytesReadWaiting(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_LEN, Validate=False, maxResp=2)
        resp = resp[1]

        #print "%d waiting"%resp
        
        return resp

    def read(self, num=0, timeout=100):
        waiting = self.bytesReadWaiting()
        data = bytearray()

        #TODO: why is this needed? Some garbage at front...
        num = num + 1

        while (len(data) < num) and (timeout > 1):
            if waiting > 0:
                resp = self.oa.sendMessage(CODE_READ, ADDR_DATA, Validate=False, maxResp=1)
                if resp:
                    data.append(resp[0])
            else:
                time.sleep(0.01)
                timeout = timeout - 1
            waiting = self.bytesReadWaiting()

        #TODO: fix removing garbage at front
        return data[1:len(data)]

    def flush(self):
        waiting = self.bytesReadWaiting()
        while waiting > 0:
            resp = self.oa.sendMessage(CODE_READ, ADDR_DATA, Validate=False, maxResp=1)
            waiting = self.bytesReadWaiting()  
        
    def readAll(self):
        return

    def readPayload(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_PLDADDR, Validate=False, maxResp=18)
        return bytearray(resp)
        
    def disconnect(self):
        self.oa = None
            
    def close(self):
        self.oa = None
        
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
            self.write(cmd)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):
        response = str(self.read(num=33))

        if len(response) < 33:
            print "WARNING: Response (len=%d) too short!"%len(response)
            return None

        if response[0] != 'r':
            print "Sync Error"
            return None

        data = bytearray(16)

        for i in range(0,16):
            data[i] = int(response[(i*2+1):(i*2+3)], 16)

        return data

    def go(self):
        self.flush()
        cmd = "p"
        for b in self.input:
            cmd = cmd + "%2x"%b
        cmd = cmd + "\n"
        self.write(cmd)
