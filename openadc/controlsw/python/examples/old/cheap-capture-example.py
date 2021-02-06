# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
from PySide.QtCore import *
from PySide.QtGui import *
import openadc
import openadc_qt
import random
import scan
import serial
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
        # Open serial port if not already
        self.ser = serial.Serial()
        self.ser.port     = "com1"
        self.ser.baudrate = 4800
        self.ser.timeout  = 2     # 2 second timeout
        self.ser.open()
        return True        
    
    def close(self):
        self.ser.close()

    def init(self):
        print "init"      

    def setModeEncrypt(self):
        print "setmode"

    def setModeDecrypt(self):
        print "setmode"

    def loadEncryptionKey(self, key):
        #The APDU for the keyschedule is in hex (80 02 00 00 10 KEY(16Byte) 10)
        self.ser.write("k2b7e151628aed2a6abf7158809cf4f3c\n")
        self.ser.write("k2b7e151628aed2a6abf7158809cf4f3c\n")
        self.ser.write("k2b7e151628aed2a6abf7158809cf4f3c\n")
       
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        response = self.ser.read(33)

        if response == None:
            print "WARNING: No Response"
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
        self.ser.write(cmd)
        
                    
class MainWindow(QMainWindow):

    def settingsToString(self, tw):
        return
        
    def findboards(self):
        self.SASEBOconnect()


        if self.ser == None:        
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = self.serialList.currentText()
            self.ser.baudrate = 512000;
            self.ser.timeout  = 2     # 2 second timeout


            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException, e:
                    attempts = attempts - 1
                    if attempts == 0:
                        raise IOError("Could not open %s"%self.ser.name)
        
        self.oa.ADCconnect(self.ser)
        
    
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

    def serialRefresh(self):
        serialnames = scan.scan()
        for i in range(0, 255):
            self.serialList.removeItem(i)
        self.serialList.addItems(serialnames)
        
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self.ser = None
        self.sasebo = SASEBO()
        self.tw = traceWriter()       
        self.oa = openadc_qt.OpenADCQt()
        
        self.statusBar()
        self.setWindowTitle("AVR / OpenADC Capture App")
        self.title = QLabel("AVR / OpenADC Capture App")
        self.title.setAlignment(Qt.AlignCenter)
        flabel = self.title.font()
        flabel.setPointSize(24)
        self.title.setFont(flabel)

        self.serialList = QComboBox()
        self.connectButton = QPushButton("Connect")
        self.captureButton = QPushButton("Capture One")
        self.readButton = QPushButton("Re-Read")
        self.updateButton = QPushButton("Go")


        self.oa = openadc_qt.OpenADCQt();
        self.connect(self.connectButton, SIGNAL("clicked()"),self, SLOT("findboards()"))
        self.connect(self.captureButton, SIGNAL("clicked()"),self, SLOT("captureone()"))
        self.connect(self.updateButton, SIGNAL("clicked()"),self,SLOT("startCapture()"))
        self.connect(self.readButton, SIGNAL("clicked()"),self,SLOT("ADCread()"))
       
        
        # Create layout and add widgets
        self.mw = QWidget()

        glayout = QGridLayout()
        
        layout = QVBoxLayout()
        
        layout.addWidget(self.title)
        glayout.addWidget(self.serialList, 0, 0)
        glayout.addWidget(self.connectButton, 0, 1)
        glayout.addWidget(self.captureButton, 1, 0)
        glayout.addWidget(self.readButton, 1, 1)
        glayout.addWidget(self.updateButton, 2, 0)

        layout.addLayout(glayout)
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

        self.serialRefresh()
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
