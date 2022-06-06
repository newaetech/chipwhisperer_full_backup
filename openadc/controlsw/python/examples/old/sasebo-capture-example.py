# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
import ftd2xx as ft
from PySide.QtCore import *
from PySide.QtGui import *
import openadc
import openadc_qt
import random
from time import gmtime, strftime

class traceWriter():

    def __init__(self):
        self.dir = "."
        self.numtrace = 0

    def setDirectory(self, directory):
        self.dir = directory;

        os.mkdir(directory)

    def openFiles(self):
        self.startTime = gmtime()
        self.setDirectory("capture-%s/"%strftime("%Y.%m.%d-%H.%M.%S", self.startTime))
        
        if os.path.exists(self.dir + "text_in.txt"):
            print "Textin File exists!"
            return

        if os.path.exists(self.dir + "text_out.txt"):
            print "Textout File exists!"
            return

        if os.path.exists(self.dir + "wave.txt"):
            print "Wave File exists!"
            return

        if os.path.exists(self.dir + "key.txt"):
            print "Key file exists!"
            return
        
        self.inf = open(self.dir + "text_in.txt", "w")
        self.outf = open(self.dir + "text_out.txt", "w")
        self.wavef = open(self.dir + "wave.txt", "w")
        self.keyf = open(self.dir + "key.txt", "w")

    def addTrace(self, textin, textout, wave, key=None):
        self.wavelen = len(wave)
        self.numtrace = self.numtrace + 1

        for i in textin:
            self.inf.write('%2X '%i)
        self.inf.write('\n')

        for i in textout:
            self.outf.write('%2X '%i)
        self.outf.write('\n')

        for i in wave:
            iint = i * float(2**16)
            self.wavef.write('%d '%int(iint))
        self.wavef.write('\n')

        if key:
            for i in key:
                self.keyf.write('%2X '%i)
            self.keyf.write('\n')

    def writeInfo(self):
        infofile = open(self.dir + "info.xml", "w")

        infofile.write('<?xml version="1.0" encoding="utf-8"?>\n')
        infofile.write('<WaveformInfo xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">\n')
        infofile.write('  <Date>%s</Date>'%strftime("%d/%m%Y %H:%M:%S", self.startTime))
        infofile.write('  <Operator>Colin O\'Flynn</Operator>\n')
        infofile.write('  <WaveType>PowerTrace</WaveType>\n')
        infofile.write('  <WaveFormat>System.Single[]</WaveFormat>\n')
        infofile.write('  <Instrument>OpenADC</Instrument>\n')
        infofile.write('  <Module>AES</Module>\n')
        infofile.write('  <Cipher>AES</Cipher>\n')
        infofile.write('  <KeyLength>128</KeyLength>\n')
        infofile.write('  <TextWidth>128</TextWidth>\n')
        infofile.write('  <NumTrace>%d</NumTrace>\n'%self.numtrace)
        infofile.write('  <NumPoint>%d</NumPoint>\n'%self.wavelen)
        infofile.write('</WaveformInfo>\n')
        infofile.close()

    def writeSettings(self, settings):
        settingsfile = open(self.dir + "settings.txt", "w")
        settingsfile.write("OpenADC Capture Settings:\n")
        for s in settings:
            settingsfile.write("%s\n"%s)       
        settingsfile.close()

    def closeAll(self):
        self.writeInfo()
        self.inf.close()
        self.outf.close()
        self.wavef.close()
        self.keyf.close()
        
class doAcq(object):
    def __init__(self, sasebo, openadc, label=None, newKeyPerTrace=False, fixedPlain=False):
        self.sasebo = sasebo
        self.openadc = openadc
        self.label = label
        self.running = False
        self.newKeyPerTrace = newKeyPerTrace
        self.fixedPlainText = fixedPlain
        self.maxtraces = 1

        self.plain = bytearray(16)
        for i in range(0,16):
                   self.plain[i] = random.randint(0, 255)
        

    def SASEBOdotrace(self, plaintext, key=None):
        self.sasebo.loadEncryptionKey(key)      
        self.sasebo.loadInput(plaintext)
        self.sasebo.go()

        while(self.sasebo.isDone() == False):
            time.sleep(0.05)
            continue

        resp = self.sasebo.readOutput()

        return resp

    def doSingleReading(self, update=True, N=None, textIn=None, key=None):
        self.openadc.ADCarm()

        if textIn:
            self.textin = textIn
        else:
            self.textin = bytearray(16)
            for i in range(0,16):
                self.textin[i] = random.randint(0, 255)

        if key:
            self.key = key
        else:
            if self.newKeyPerTrace:
                self.key = bytearray(16)
                for i in range(0,16):
                    self.key[i] = random.randint(0, 255)
            else:
                self.key = None
        
        ## Start SASEBO now
        

        #Set mode
        self.sasebo.setModeEncrypt()

        #Load input, start encryption, get output
        self.textout = self.SASEBOdotrace(self.textin, self.key)

        #Get ADC reading
        self.openadc.ADCcapture(update, N)


    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self):
        self.running = True
        tw = traceWriter()
        tw.openFiles()

        if self.fixedPlainText:
            plain = self.plain
        else:
            plain = None

        while (tw.numtrace < self.maxtraces) and self.running:
            self.doSingleReading(True, self.openadc.preview.xmax.value(), plain)
            tw.addTrace(self.textin, self.textout, self.openadc.datapoints[int(self.openadc.preview.xmin.value()) : int(self.openadc.preview.xmax.value())], self.key)

            self.label.setText("Traces = %d"%tw.numtrace)

        tw.closeAll()

        self.running = False
        
        
              
class SASEBO():
    def connect(self):
       # connect to port   
        try:
            self.sasebo = ft.openEx("FTSZ1IONB")
        except ft.ftd2xx.DeviceError, e:
            self.sasebo = None
            return False

        self.sasebo.setTimeouts(1000, 1000)
        return True

    def flush(self):
        num = self.sasebo.getQueueStatus()
        if num > 0:
            self.sasebo.read(num)

    def write(self, address, MSB, LSB):
        msg = bytearray(5)

        msg[0] = 0x01;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        msg[3] = MSB;
        msg[4] = LSB;

        strmsg = str(msg);

        #msg = bytearray(strmsg)
        #print "Write: %x %x %x %x %x"%(msg[0],msg[1],msg[2],msg[3],msg[4])

        self.sasebo.write(strmsg)

    def read(self, address):
        self.flush()
        msg = bytearray(3)
        msg[0] = 0x00;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        self.sasebo.write(str(msg))
        #print "Write: %x %x %x"%(msg[0],msg[1],msg[2]),
        msg = self.sasebo.read(2)
        msg = bytearray(msg)

        #print " Read: %x %x"%(msg[0],msg[1])

        #Order = MSB, LSB
        return msg

    def read128(self, address):
        self.flush()
        msg = bytearray(3*8)
        for i in range(0, 8):
            msg[i*3] = 0x00;
            msg[i*3+1] = (address >> 8) & 0xFF;
            msg[i*3+2] = (address & 0xFF) + (i*2);
        self.sasebo.write(str(msg))
        msg = self.sasebo.read(16)        
        return bytearray(msg)

    def close(self):
        self.sasebo.close()

    def init(self):
        #Select AES
        self.write(0x0004, 0x00, 0x01)
        self.write(0x0006, 0x00, 0x00)

        #Reset AES module
        self.write(0x0002, 0x00, 0x04)
        self.write(0x0002, 0x00, 0x00)

        #Select AES output
        self.write(0x0008, 0x00, 0x01)
        self.write(0x000A, 0x00, 0x00)

    def setModeEncrypt(self):
        self.write(0x000C, 0x00, 0x00)

    def setModeDecrypt(self):
        self.write(0x000C, 0x00, 0x01)

    def loadEncryptionKey(self, key):
        """Encryption key is bytearray"""

        if key:     
            self.write(0x0100, key[0], key[1])
            self.write(0x0102, key[2], key[3])
            self.write(0x0104, key[4], key[5])
            self.write(0x0106, key[6], key[7])
            self.write(0x0108, key[8], key[9])
            self.write(0x010A, key[10], key[11])
            self.write(0x010C, key[12], key[13])
            self.write(0x010E, key[14], key[15])

        #I don't know what this is but we need it? Not documented?
        self.write(0x0002, 0x00, 0x02)

        while self.isDone() == False:
            continue

    def loadInput(self, inputtext):
        self.write(0x0140, inputtext[0], inputtext[1])
        self.write(0x0142, inputtext[2], inputtext[3])
        self.write(0x0144, inputtext[4], inputtext[5])
        self.write(0x0146, inputtext[6], inputtext[7])
        self.write(0x0148, inputtext[8], inputtext[9])
        self.write(0x014A, inputtext[10], inputtext[11])
        self.write(0x014C, inputtext[12], inputtext[13])
        self.write(0x014E, inputtext[14], inputtext[15])

    def isDone(self):
        result = self.read(0x0002)

        if result[0] == 0x00 and result[1] == 0x00:
            return True
        else:
            return False

    def readOutput(self):        
        return self.read128(0x0180)

    def setMode(self, mode):
        if mode == "encryption":
            self.write(0x000C, 0x00, 0x00)
        elif mode == "decryption":
            self.write(0x000C, 0x00, 0x01)
        else:
            print "Wrong mode!!!!"

    def go(self):
        self.write(0x0002, 0x00, 0x01)
        
class MainWindow(QMainWindow):

    def settingsToString(self, tw):
        return
        
    def findboards(self):
        self.SASEBOconnect()
        self.oa.ADCconnect()
    
    def SASEBOconnect(self):        
        # connect to port
        if self.sasebo.connect() == False:
            self.statusBar().showMessage("Failed to connect to SASEBO-GII")
            return

        self.sasebo.init()
        self.statusBar().showMessage("Connected to SASEBO-GII Board")             

    def captureone(self):       
        da = doAcq(self.sasebo, self.oa, None, self.newKeyCB.isChecked(), self.fixedPlainCB.isChecked())
        da.doSingleReading(textIn = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])

    def startCapture(self):       
        self.da = doAcq(self.sasebo, self.oa, self.counter, self.newKeyCB.isChecked(), self.fixedPlainCB.isChecked())
        self.da.setMaxtraces(self.numTraces.value())       
        daThread = threading.Thread(target = self.da.doReadings)

        # Start the stream
        daThread.start()

    def stopCapture(self):
        self.da.running = False

    def startTuning(self):
        self.tuningResults = []

        da = doAcq(self.sasebo, self.oa)

        pointsToAverage = 20

        phase = -255

        plain = bytearray(16)

        while phase <= 255:
            self.oa.sc.setPhase(phase)

            #Wait for change to happen
            while self.oa.sc.getPhase() != phase:
                time.sleep(0.05)
                continue

            maxpt = 0.0
            minpt = 0.0
            for i in range(0, pointsToAverage):
                da.doSingleReading(True, 100, plain)
                cropped = self.oa.datapoints[self.tunStart.value() : self.tunStop.value()]
                maxpt = maxpt + max(cropped)
                minpt = minpt + min(cropped)

            maxpt = maxpt / float(pointsToAverage)
            minpt = minpt / float(pointsToAverage)

            print "Phase %d: %6f %6f"%(phase, maxpt, minpt)

            #Next phase point
            phase = phase + 1

        self.oa.sc.setPhase(0)
        
    def __del__(self):
        if self.sasebo:
            self.sasebo.close()        
        
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self.sasebo = SASEBO()
        self.tw = traceWriter()       
        self.oa = openadc_qt.OpenADCQt()
        
        self.statusBar()
        self.setWindowTitle("SASEBO-GII / OpenADC Capture App")
        self.title = QLabel("SASEBO-GII / OpenADC Capture App")
        self.title.setAlignment(Qt.AlignCenter)
        flabel = self.title.font()
        flabel.setPointSize(24)
        self.title.setFont(flabel)
   
        self.connectButton = QPushButton("Connect")
        captureOne = QPushButton("Capture One")
        self.updateButton = QPushButton("Get Status")
       
        self.connect(self.connectButton, SIGNAL("clicked()"),self, SLOT("findboards()"))
        self.connect(captureOne, SIGNAL("clicked()"),self, SLOT("captureone()"))
       
        # Create layout and add widgets
        self.mw = QWidget()
        layout = QVBoxLayout()
        
        layout.addWidget(self.title)
        layout.addWidget(self.connectButton)
        layout.addWidget(captureOne)
        layout.addWidget(self.updateButton)

        layout.addLayout(self.oa.getLayout())


        self.capturesettings = QGroupBox("Capture Settings")
        self.caplayout = QGridLayout()
        self.capturesettings.setLayout(self.caplayout)
        self.caplayout.addWidget(QLabel("Number Traces"), 0, 0)
        self.numTraces = QSpinBox()
        self.numTraces.setMinimum(0)
        self.numTraces.setMaximum(1000000)
        self.caplayout.addWidget(self.numTraces, 0, 1)

        self.newKeyCB = QCheckBox("New Key")
        self.fixedPlainCB = QCheckBox("Fixed Plaintext")

        self.startCapturePB = QPushButton("Start Capture")
        self.cancelCapturePB = QPushButton("Cancel Capture")

        self.counter = QLabel("Traces = 0")
        self.caplayout.addWidget(self.counter, 0, 2)
        self.caplayout.addWidget(self.startCapturePB, 1, 0)
        self.caplayout.addWidget(self.cancelCapturePB, 1, 1)
        self.caplayout.addWidget(self.newKeyCB, 1, 2)
        self.caplayout.addWidget(self.fixedPlainCB, 1, 3)
        self.startCapturePB.clicked.connect(self.startCapture)
        self.cancelCapturePB.clicked.connect(self.stopCapture)
        layout.addWidget(self.capturesettings)


        self.tuning = QGroupBox("Phase Tuning")
        self.tunlayout = QGridLayout()
        self.tuning.setLayout(self.tunlayout)
        self.tunlayout.addWidget(QLabel("Point range:"), 0, 0)
        self.tunStart = QSpinBox()
        self.tunStop = QSpinBox()
        self.tunlayout.addWidget(self.tunStart, 0, 1)       
        self.tunlayout.addWidget(self.tunStop, 0, 2)
        self.tunStartPB = QPushButton("Start Tuning")
        self.tunStartPB.clicked.connect(self.startTuning)
        self.tunlayout.addWidget(self.tunStartPB, 0, 3)
        layout.addWidget(self.tuning)
              
        # Set dialog layout
        self.setLayout(layout)       
        self.mw.setLayout(layout)
        self.setCentralWidget(self.mw)

        self.trigmode = 0
        self.hilowmode = 0
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
