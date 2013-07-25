#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================


import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
import random
import os.path
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
imagePath = '../common/images/'

from ExtendedParameter import ExtendedParameter

try:
    import writer_dpav3
except ImportError:
    writer_dpav3 = None
    writer_dpav3_str = sys.exc_info()

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

try:
    import openadc_qt
except ImportError:
    print "ERROR: openadc_qt is required for this program"
    sys.exit()

try:
    import ftd2xx as ft
except ImportError:
    ft = None
    ft_str = sys.exc_info()

try:
    import usb
except ImportError:
    usb = None
    usb_str = sys.exc_info()

try:
    import serial
except ImportError:
    serial = None

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None    

try:
    import target_chipwhisperer_integrated
except ImportError:
    target_chipwhisperer_integrated = None
    target_chipwhisperer_integrated_str = sys.exc_info()    

from projectwindow import MainChip

            
class OpenADCInterface_ZTEX(QWidget):    
    def __init__(self,oadcInstance):
        QWidget.__init__(self)
        
        ztexParams = [                  
                      #No Parameters for ZTEX
                  ]           
       
        if (openadc_qt == None) or (usb == None):               
            self.ser = None
        else:            
            self.ser = None
            self.scope = oadcInstance
            self.params = Parameter.create(name='OpenADC-ZTEX', type='group', children=ztexParams)
            ExtendedParameter.setupExtended(self.params)    
            
        

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            try:
                dev = usb.core.find(idVendor=0x221A, idProduct=0x0100)
            except:
                exctype, value = sys.exc_info()[:2]
                QMessageBox.warning(None, "FX2 Port", str(exctype) + str(value))
                return False

            if dev is None:
                QMessageBox.warning(None, "FX2 Port", "Could not open USB Device")            
                return False

            dev.set_configuration()            

            self.dev = dev
            self.writeEP = 0x06
            self.readEP = 0x82
            self.interface = 0
            self.ser = self

        try:
            self.scope.con(self.ser)
        except:
            exctype, value = sys.exc_info()[:2]
            QMessageBox.warning(None, "FX2 Port", str(exctype) + str(value))
            return False
        
        return True

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        

    def read(self, N=0, debug=False):
        data = self.dev.read(self.readEP, N, self.interface, 500)
        data = bytearray(data)
        if debug:
            print "RX: ",
            for b in data:
                print "%02x "%b,
            print ""
        return data

    def write(self, data, debug=False):
        data = bytearray(data)
        if debug:
            print "TX: ",
            for b in data:
                print "%02x "%b,
            print ""
        self.dev.write(self.writeEP, data, self.interface, 500)
            
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def update(self):
        print "update"

class OpenADCInterface(QObject):
    connectStatus = Signal(bool)
    dataUpdated = Signal(list)
    paramListUpdated = Signal(list)    

    def __init__(self, parent=None):          
        super(OpenADCInterface, self).__init__(parent)
        self.qtadc = openadc_qt.OpenADCQt(includePreview=False,  setupLayout=False)
        self.qtadc.setupParameterTree(False)
        self.qtadc.dataUpdated.connect(self.doDataUpdated)
        self.datapoints = self.qtadc.datapoints
        self.scopetype = None
        
        scopeParams = [{'name':'connection', 'type':'list', 'values':{"ChipWhisperer Rev2":OpenADCInterface_ZTEX(self.qtadc),
                                                                     "Serial Port (LX9)":None,
                                                                     "FTDI (SASEBO-W)":None}, 'value':'ChipWhisperer Rev2', 'set':self.setCurrentScope}
                      ]
        
        self.params = Parameter.create(name='OpenADC Interface', type='group', children=scopeParams)
        ExtendedParameter.setupExtended(self.params)
        

    def setCurrentScope(self, scope):        
        self.scopetype = scope
        self.paramListUpdated.emit(self.paramList())
   
    def con(self):
        if self.scopetype is not None:
            self.scopetype.con()  
            self.connectStatus.emit(True)

    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()  
            self.connectStatus.emit(True)

    def doDataUpdated(self,  l):
        self.dataUpdated.emit(l)

    def arm(self):
        self.qtadc.arm()

    def capture(self, update=True, NumberPoints=None):
        self.qtadc.capture(update, NumberPoints)    
        
    def paramList(self):
        p = []
        p.append(self.params)
        if self.scopetype is not None:
            p.append(self.scopetype.params)
        p.append(self.qtadc.params)               
        return p


class acquisitionController(object):
    def __init__(self, scope, target, writer, label=None, fixedPlain=False, updateData=None, textInLabel=None, textOutLabel=None, textExpectedLabel=None):
        self.target = target
        self.scope = scope
        self.label = label
        self.running = False
        self.fixedPlainText = fixedPlain
        self.maxtraces = 1
        self.updateData = updateData
        self.textInLabel = textInLabel
        self.textOutLabel = textOutLabel
        self.textExpectedLabel = textExpectedLabel

        self.plain = bytearray(16)
        for i in range(0,16):
                   self.plain[i] = random.randint(0, 255)       

    def TargetDoTrace(self, plaintext, key=None):
        if self.target is None:
            return
        
        self.target.loadEncryptionKey(key)      
        self.target.loadInput(plaintext)
        self.target.go()

        #print "DEBUG: Target go()"

        resp = self.target.readOutput()
        #print "DEBUG: Target readOutput()"

        #print "pt:",
        #for i in plaintext:
        #    print " %02X"%i,
        #print ""

        #print "sc:",
        #for i in resp:
        #    print " %02X"%i,
        #print ""

        return resp

    def newPlain(self, textIn=None):     
          
        if textIn:
            self.textin = textIn
        else:
            if not self.fixedPlainText:       
                self.textin = bytearray(16)
                for i in range(0,16):
                    self.textin[i] = random.randint(0, 255)
                    #self.textin[i] = i

        #Do AES if setup
        if AES and (self.textExpectedLabel != None):
            if self.key == None:
                self.textExpectedLabel.setText("")
            else:
                cipher = AES.new(str(self.key), AES.MODE_ECB)
                ct = cipher.encrypt(str(self.textin))
                if self.textExpectedLabel != None:
                    ct = bytearray(ct)
                    self.textExpectedLabel.setText("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"%(ct[0],ct[1],ct[2],ct[3],ct[4],ct[5],ct[6],ct[7],ct[8],ct[9],ct[10],ct[11],ct[12],ct[13],ct[14],ct[15]))


    def doSingleReading(self, update=True, N=None, key=None):
        self.scope.arm()

        if key:
            self.key = key
        else:
            self.key = None

        self.newPlain()

        ## Start target now
        if self.textInLabel != None:
            self.textInLabel.setText("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"%(self.textin[0],self.textin[1],self.textin[2],self.textin[3],self.textin[4],self.textin[5],self.textin[6],self.textin[7],self.textin[8],self.textin[9],self.textin[10],self.textin[11],self.textin[12],self.textin[13],self.textin[14],self.textin[15]))

        
        if self.target is not None:
            #Set mode
            self.target.setModeEncrypt()

            #Load input, start encryption, get output
            self.textout = self.TargetDoTrace(self.textin, self.key)

        try:
            self.textOutLabel.setText("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X"%(self.textout[0],self.textout[1],self.textout[2],self.textout[3],self.textout[4],self.textout[5],self.textout[6],self.textout[7],self.textout[8],self.textout[9],self.textout[10],self.textout[11],self.textout[12],self.textout[13],self.textout[14],self.textout[15]))
        except:
            print "Response failed?"

        #Get ADC reading
        self.scope.capture(update, N)
        

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self):
        self.running = True

        tw = writer_dpav3.dpav3()
        tw.openFiles()
        tw.addKey(self.key)

        while (tw.numtrace < self.maxtraces) and self.running:
            self.doSingleReading(True, None, None)
            tw.addTrace(self.textin, self.textout, self.scope.datapoints, self.key)
  
            if self.updateData:
                self.updateData(self.scope.datapoints)

            if self.label != None:
                self.label.setText("Traces = %d"%tw.numtrace)

        tw.closeAll()

        self.running = False      

class TargetInterface(QObject):

    def __init__(self, parent=None):
        super(TargetInterface, self).__init__(parent)

    def OpenADCConnectChanged(self, status):
        if status == False:
            self.driver.dis()
        #else:
        #    self.setOpenADC(oadc)

    def setOpenADC(self, oadc):
        '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
        self.oadc = oadc
        #print self.OpenADCConnectChanged
        oadc.connectStatus.connect(self.OpenADCConnectChanged)

    def setDriver(self, driver):
        self.driver = driver.QtInterface()
        try:
            self.driver.setOpenADC(self.oadc)
        except:
            pass
         
class MainWindow(MainChip):
    MaxRecentFiles = 4
    
    def __init__(self):
        super(MainWindow, self).__init__(name="ChipWhisperer Capture V2", imagepath=imagePath)
    
        valid_scopes = {"None":None, "ChipWhisperer/OpenADC":OpenADCInterface()}
        valid_targets = {"None":None}
        valid_traces = {"None":None, "DPA Contest v3": writer_dpav3.dpav3()}    
        
        self.cwParams = [
                {'name':'Scope Module', 'type':'list', 'values':valid_scopes, 'value':valid_scopes["None"], 'set':self.scopeChanged},
                {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':valid_targets["None"]},
                {'name':'Trace Format', 'type':'list', 'values':valid_traces, 'value':valid_traces["None"], 'set':self.traceChanged},
                ]
        
        self.da = None
        self.key = None
        
        self.scope = None
        self.target = None

        self.addToolbars()
        self.addSettingsDocks()
        self.addWaveforms()
        
    def addWaveforms(self):
        self.waveformDock = self.addTraceDock("Capture Waveform (Channel 1)")       
        
    def addSettingsDocks(self):      
        self.setupParametersTree()        
        self.settingsNormalDock = self.addSettings(self.paramTree, "General Settings")
        self.settingsScopeDock = self.addSettings(self.scopeParamTree, "Scope Settings")
        self.settingsTargetDock = self.addSettings(self.targetParamTree, "Target Settings")

    def setupParametersTree(self):
        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)
        
        self.scopeParamTree = ParameterTree()
        self.targetParamTree = ParameterTree()
        
    def reloadParamList(self, lst=None):
        self.paramTree.clear()
        self.scopeParamTree.clear()
        self.targetParamTree.clear()        
        for p in self.paramList(): self.paramTree.addParameters(p)
        if self.scope is not None:     
            for p in self.scope.paramList(): self.scopeParamTree.addParameters(p)            
        if self.target is not None:
            for p in self.target.paramList(): self.targetParamTree.addParameters(p)
        
    def paramList(self):
        p = []
        p.append(self.params)     
        return p        

    def initSettings(self):
        #ChipWhisperer-Capture Settings
        self.genConfig = self.generalConfig()
        gg = self.settings.addGroup("General Settings", self.genConfig)          

        #OpenADC Settings
        self.scope = OpenADCInterface()
        #self.scope.loadSettings(self.settings)
        self.scope.dataUpdated.connect(self.newScopeData)
        self.scope.connectStatus.connect(self.connected)
        
        self.target = TargetInterface()
        self.target.setOpenADC(self.scope)

        self.settings.widgetChanged.connect(self.setConfigWidget)

        self.cbTargetChanged(0)

    def newScopeData(self,  data=None):
        self.channelDocks[0].passTrace(data)

    def setConfigWidget(self, widget):
        self.configdock.setWidget(widget)

    def addToolbars(self):
        self.addCaptureTools()

    def addCaptureTools(self):
        capture1 = QAction(QIcon(imagePath+'play1.png'), 'Capture 1', self)
        capture1.triggered.connect(self.capture1)
        captureM = QAction(QIcon(imagePath+'playM.png'), 'Capture Multi', self)
        captureM.triggered.connect(self.captureM)
        
        self.captureStatus = QToolButton()
        self.captureStatusActionDis = QAction(QIcon(imagePath+'status_disconnected.png'),  'Status: Disconnected',  self)
        self.captureStatusActionDis.triggered.connect(self.doCon)
        self.captureStatusActionCon = QAction(QIcon(imagePath+'status_connected.png'),  'Status: Connected',  self)
        self.captureStatusActionDis.triggered.connect(self.doDis)
        self.captureStatus.setDefaultAction(self.captureStatusActionDis)

        self.CaptureToolbar = self.addToolBar('Capture Tools')
        self.CaptureToolbar.addAction(capture1)
        self.CaptureToolbar.addAction(captureM)
        self.CaptureToolbar.addWidget(self.captureStatus)
        #self.CaptureToolbar.setEnabled(False)

    def connected(self, status=True, text=None):
        #self.CaptureToolbar.setEnabled(status)
        
        if status:
            self.captureStatus.setDefaultAction(self.captureStatusActionCon)
        else:
            self.captureStatus.setDefaultAction(self.captureStatusActionDis)


    def doCon(self):
        if self.scope is not None:
            self.scope.con()
        
        if self.target is not None:
            self.target.con()
    
    def doDis(self):
        self.scope.dis()
        self.target.dis()

    def capture1(self):
        if self.target:
            target = self.target.driver.target
        else:
            target = None
        ac = acquisitionController(self.scope, target, self.writer)
        ac.doSingleReading()

    def captureM(self):
        print "capture M"
        
    def scopeChanged(self, newscope):
        self.scope = newscope
        if self.scope is not None:
            self.scope.paramListUpdated.connect(self.reloadParamList)            
        self.reloadParamList()
        
    def traceChanged(self, newtrace):
        self.trace = newtrace
        #TODO: Reload?
        
    def targetChanged(self, newtarget):
        self.target = newtarget
        if self.target is not None:
            self.target.paramListUpdated.connect(self.reloadParamList)
        self.reloadParamList()

    def closeEvent(self, event):
        return
  
if __name__ == '__main__':
    
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Capture V2")
    
    # Create and show the form
    window = MainWindow()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
