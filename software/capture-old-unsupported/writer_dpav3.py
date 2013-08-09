# -*- coding: cp1252 -*-
import sys
import os
import threading
import time
import logging
import math
import random
from time import gmtime, strftime

class dpav3():
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

    def addKey(self, key):
        if key:
            for i in key:
                self.keyf.write('%2X '%i)
            self.keyf.write('\n')
        

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
        infofile.write('  <Operator>Smooth</Operator>\n')
        infofile.write('  <WaveType>PowerTrace</WaveType>\n')
        infofile.write('  <WaveFormat>System.Single[]</WaveFormat>\n')
        infofile.write('  <Instrument>Something</Instrument>\n')
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
