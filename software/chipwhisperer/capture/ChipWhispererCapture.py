#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
import importlib
from functools import partial

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
from datetime import datetime
import random
import os.path

from openadc.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.qrc_resources

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
    #print pg.systemInfo()
    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None    

from chipwhisperer.capture.scopes.OpenADC import OpenADCInterface as OpenADCInterface
from chipwhisperer.capture.scopes.ChipWhispererFWLoader import FWLoaderConfig

try:
    from  chipwhisperer.capture.scopes.VisaScope import VisaScopeInterface as VisaScopeInterface
except ImportError:
    VisaScopeInterface = None

try:
    from  chipwhisperer.capture.scopes.PicoScope import PicoScopeInterface as PicoScopeInterface
except ImportError:
    PicoScopeInterface = None

try:
    import  chipwhisperer.capture.targets.SimpleSerial as target_SimpleSerial
except ImportError:
    target_SimpleSerial = None
    target_SimpleSerial_str = sys.exc_info()
    
try:
    import  chipwhisperer.capture.targets.SmartCard as target_SmartCard
except ImportError:
    target_SmartCard = None
    target_SmartCard_str = sys.exc_info()
    print "SmartCard targets disabled: %s"%str(target_SmartCard_str)
    
try:
    import  chipwhisperer.capture.targets.SASEBOGII as target_SASEBOGII
except ImportError:
    target_SASEBOGII = None
    target_SASEBOGII_str = sys.exc_info()
    
try:
    import  chipwhisperer.capture.targets.SAKURAG as target_SAKURAG
except ImportError:
    target_SAKURAG = None
    target_SAKURAG_str = sys.exc_info()

try:
    import  chipwhisperer.capture.auxiliary.FrequencyMeasure as aux_FrequencyMeasure
except ImportError:
    aux_FrequencyMeasure = None

from chipwhisperer.common.MainChip import MainChip
from chipwhisperer.common.ProjectFormat import ProjectFormat
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.common.traces.TraceContainerDPAv3 import TraceContainerDPAv3

try:
    from chipwhisperer.common.traces.TraceContainerMySQL import TraceContainerMySQL
except ImportError:
    TraceContainerMySQL = None

from chipwhisperer.capture.ListAllModules import ListAllModules

class acquisitionController(QObject):
    traceDone = Signal(int, list, int)
    captureDone = Signal(bool)
    
    def __init__(self, scope, target, writer, aux=None, fixedPlain=None, updateData=None, esm=None, newKeyPerTrace=False):
        super(acquisitionController, self).__init__()

        self.target = target
        self.scope = scope
        self.writer = writer
        self.aux = aux
        self.running = False
        self.fixedPlainText = fixedPlain
        self.maxtraces = 1
        self.updateData = updateData

        self.textInLabel = esm.textInLine
        self.textOutLabel = esm.textOutLine
        self.textExpectedLabel = esm.textOutExpected
        self.textKeyLabel = esm.textEncKey
        self.newKeyPerTrace=newKeyPerTrace

        self.textin = bytearray(16)
        for i in range(0,16):
            self.textin[i] = i  # random.randint(0, 255)

        if self.aux is not None:
            self.aux.captureInit()

    def TargetDoTrace(self, plaintext, key=None):
        if self.target is None:
            return
           
        self.target.loadInput(plaintext)
        self.target.go()
        
        while self.target.isDone() == False:
            continue

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

    def newKey(self):
        newkey = bytearray(self.target.keyLen())
        for i in range(0,self.target.keyLen()):
            newkey[i] = random.randint(0,255)
        return newkey

    def newPlain(self, textIn=None):      

        if textIn:
            self.textin = textIn
        else:
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
                    text = ""
                    for t in ct: text += "%02X "%t                                             
                    self.textExpectedLabel.setText(text)


    def doSingleReading(self, update=True, N=None, key=None):       
        self.key = key
        self.newPlain(self.fixedPlainText)

        if self.key is not None:
            self.key = self.target.checkEncryptionKey(key)
            if (self.target.keyLen() != len(self.key)):
                print "Key length WRONG for given target"
                raise IOError("Key Length Wrong for Given Target, %d != %d"%(self.target.keyLen(), len(self.key)))                                                

        if self.textKeyLabel is not None:
            txtlabel = ""
            for t in self.key: txtlabel += "%02X "%t
            self.textKeyLabel.setText(txtlabel)

        ## Start target now
        if self.textInLabel is not None:
            text = ""
            for t in self.textin: text += "%02X "%t                                             
            self.textInLabel.setText(text)
           
        #Set mode
        if self.target is not None:
            self.target.reinit()
            self.target.setModeEncrypt()
            self.target.loadEncryptionKey(self.key)  
        
        if self.scope is not None:
            self.scope.arm()

        if self.aux is not None:
            self.aux.traceArm()
        
        if self.target is not None:            
            #Load input, start encryption, get output
            self.textout = self.TargetDoTrace(self.textin, key)

            if self.textout is not None:
                if len(self.textout) >= 16:  
                    text = ""
                    for t in self.textout: text += "%02X "%t                                             
                    self.textOutLabel.setText(text)

        #Get ADC reading
        if self.scope is not None:
            try:
                if self.scope.capture(update, N) == True:
                    print "Timeout"
                    return False       
            except IOError,e:
                print "IOError: %s"%str(e)
                return False

        if self.aux is not None:
            self.aux.traceDone()
        
        return True

    def setMaxtraces(self, maxtraces):
        self.maxtraces = maxtraces

    def doReadings(self, addToList=None, key=None):
        self.running = True
        
        self.key = key
        
        if self.writer is not None:
            self.writer.prepareDisk()
            self.writer.setKnownKey(self.key)

        if self.aux is not None:
            self.aux.captureInit()

        nt = 0

        while (nt < self.maxtraces) and self.running:
            if self.newKeyPerTrace:
                self.key = self.newKey()

            if self.doSingleReading(True, None, key=self.key) == True:
                if self.writer is not None:
                    self.writer.addTrace(self.scope.datapoints, self.textin, self.textout, self.key)            
    
                nt = nt + 1
                self.traceDone.emit(nt, self.scope.datapoints, self.scope.offset)
            QCoreApplication.processEvents()
            

        if self.aux is not None:
            self.aux.captureComplete()

        if self.writer is not None:
            self.writer.closeAll()
        
        if addToList is not None:
            if self.writer is not None:
                addToList.append(self.writer)
        
        self.captureDone.emit(self.running)
        
        self.running = False      

class TargetInterface(QObject):
    """This is a standard target interface, which controls various supported lower-level hardware interfaces"""
    paramListUpdated = Signal(list)
    targetUpdated = Signal(bool)
    connectStatus = Signal(bool)
    
    def __init__(self, parent=None, log=None,showScriptParameter=None):
        super(TargetInterface, self).__init__(parent)
        valid_targets = {"None":None}
        self.driver = None
        self.log=log
        self.showScriptParameter = showScriptParameter
                
        if target_SimpleSerial is not None:
            valid_targets["Simple Serial"] = target_SimpleSerial.SimpleSerial(self.log, showScriptParameter=showScriptParameter)
            
        if target_SmartCard is not None:
            valid_targets["Smart Card"] = target_SmartCard.SmartCard(self.log, showScriptParameter=showScriptParameter)
            
        if target_SASEBOGII is not None:
            valid_targets["SASEBO GII"] = target_SASEBOGII.SaseboGII(self.log, showScriptParameter=showScriptParameter)
            
        if target_SAKURAG is not None:
            valid_targets["SAKURA G"] = target_SAKURAG.SakuraG(self.log, showScriptParameter=showScriptParameter)
        
        self.toplevel_param = {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':valid_targets["None"], 'set':self.setDriver}     

    def setOpenADC(self, oadc):
        '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
        self.oadc = oadc.scope.sc
        self.driver.setOpenADC(self.oadc)
        
    def con(self):
        if self.driver is not None:
            self.driver.con()
            self.connectStatus.emit(True)
        
    def dis(self):
        if self.driver is not None:
            self.driver.dis()
            self.connectStatus.emit(False)

    def setDriver(self, driver):
        self.driver = driver
        try:
            self.driver.setOpenADC(self.oadc)
        except:
            pass        
        
        if self.driver is None:
            self.paramListUpdated.emit(None)
            self.targetUpdated.emit(False)
        else:
            self.driver.paramListUpdated.connect(self.paramListUpdated.emit)
            self.paramListUpdated.emit(self.driver.paramList())
            self.targetUpdated.emit(True)
            
    def paramList(self):
        if self.driver is None:
            return [None]
        else:
            return self.driver.paramList()

class EncryptionStatusMonitor(QDialog):
    def __init__(self, parent):
        super(EncryptionStatusMonitor, self).__init__(parent)
        self.textResultsLayout = QGridLayout()
        self.textInLine = QLineEdit()
        self.textOutLine = QLineEdit()
        self.textResultsLayout.addWidget(QLabel("Text In "), 0, 0)
        self.textInLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textInLine, 0, 1)
        self.textResultsLayout.addWidget(QLabel("Text Out"), 1, 0)
        self.textOutLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textOutLine, 1, 1)
        self.textResultsLayout.addWidget(QLabel("Expected"), 2, 0)
        self.textOutExpected = QLineEdit()
        self.textOutExpected.setReadOnly(True)        
        self.textResultsLayout.addWidget(self.textOutExpected, 2, 1)

        self.textResultsLayout.addWidget(QLabel("Enc. Key"), 3, 0)
        self.textEncKey = QLineEdit()
        self.textEncKey.setReadOnly(True)        
        self.textResultsLayout.addWidget(self.textEncKey, 3, 1)

        self.setLayout(self.textResultsLayout)  
        self.hide()
               

class ChipWhispererCapture(MainChip):
    MaxRecentFiles = 4    
    def __init__(self):
        super(ChipWhispererCapture, self).__init__(name="ChipWhisperer Capture V2")
        self.console = self.addConsole()
    
        self.scope = None        
        self.trace = None
        self.aux = None
        self.setKey('2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c')
        self.setPlaintext('00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F')
        #self.setKey('9B A5 A3 14 40 32 37 C8 CD 06 13 AA 88 62 49 6A')
        self.target = TargetInterface(log=self.console, showScriptParameter=self.showScriptParameter)        
        self.target.paramListUpdated.connect(self.reloadTargetParamList)
    
        valid_scopes = {"None":None, "ChipWhisperer/OpenADC":OpenADCInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)}        
        valid_traces = {"None":None, "ChipWhisperer/Native":TraceContainerNative, "DPAContestv3":TraceContainerDPAv3}    
        valid_aux = {"None":None}
        
        # If you want to add a 'hacked-in' module, you can do that in the 'aux' system. The aux system is designed to make
        # it very easy to add some code that does something like measure an external instrument, or control some other
        # system. Useful if you are wanting to do something like script different core voltages, frequencies, or otherwise
        # control some external device for every capture run.

        if aux_FrequencyMeasure is not None:
            valid_aux["Frequency Counter"] = aux_FrequencyMeasure.FrequencyMeasure(console=self.console, showScriptParameter=self.showScriptParameter)

        if TraceContainerMySQL is not None:
            valid_traces["MySQL"] = TraceContainerMySQL
            
        if VisaScopeInterface is not None:
            valid_scopes["VISA Scope"] = VisaScopeInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)

        if PicoScopeInterface is not None:
            valid_scopes["PicoScope"] = PicoScopeInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)
        
        self.esm = EncryptionStatusMonitor(self)
        
        self.cwParams = [
                {'name':'Scope Module', 'type':'list', 'values':valid_scopes, 'value':valid_scopes["None"], 'set':self.scopeChanged},
                self.target.toplevel_param,
                {'name':'Trace Format', 'type':'list', 'values':valid_traces, 'value':valid_traces["None"], 'set':self.traceChanged},
                
                {'name':'Auxilary Module', 'type':'list', 'values':valid_aux, 'value':valid_aux["None"], 'set':self.auxChanged },

                {'name':'Key Settings', 'type':'group', 'children':[
                        {'name':'Encryption Key', 'type':'str', 'value':self.textkey, 'set':self.setKey},
                        {'name':'Send Key to Target', 'type':'bool', 'value':True},
                        {'name':'New Encryption Key/Trace', 'key':'newKeyAlways', 'type':'bool', 'value':False},
                    ]},   
                         
                {'name':'Acquisition Settings', 'type':'group', 'children':[
                        {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E6), 'value':100, 'set':self.setNumTraces, 'get':self.getNumTraces},
                        {'name':'Open Monitor', 'type':'action', 'action':self.esm.show},
                        {'name':'Fixed Plaintext', 'type':'bool', 'value':False, 'set':self.setFixedPlain },
                        {'name':'Fixed Plaintext Value', 'type':'str', 'value':self.plaintextStr, 'set':self.setPlaintext},
                    ]}             
                ]
        
        self.da = None
        self.numTraces = 100

        self.addToolbars()
        self.addSettingsDocks()
        self.addWaveforms()
        self.addToolMenu()

        self.addExampleScripts()
        
        self.restoreDockGeometry()
        self.dockifySettings()
        self.settingsAuxDock.setVisible(False)

        self.newProject()   
        
        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)   
        
        self.fixedPlain = False 
        self.target.targetUpdated.connect(self.TargetToolbar.setEnabled)
        self.target.connectStatus.connect(self.targetStatusChanged)
  
    def listModules(self):
        """Overload this to test imports"""
        return ListAllModules()
     
    def setFixedPlain(self, x):
        self.fixedPlain = x
        
    def getNumTraces(self):
        return self.numTraces
        
    def setNumTraces(self, t):
        self.numTraces = t      

    def setKey(self, key, binary=False, updateParamList=False):
        
        if binary:
            self.textkey = ''
            for s in key:
                self.textkey += '%02x '%s
            self.key = bytearray(key)
        else:        
            self.textkey = key       
            self.key = bytearray()
            for s in key.split():
                self.key.append(int(s, 16))

    def setPlaintext(self, key, binary=False, updateParamList=False):
        
        if binary:
            self.plaintextStr = ''
            for s in key:
                self.plaintextStr += '%02x '%s
            self.plaintext = bytearray(key)
        else:        
            self.plaintextStr = key       
            self.plaintext = bytearray()
            for s in key.split():
                self.plaintext.append(int(s, 16))

        
    def FWLoaderConfig(self):
        self.CWFirmwareConfig.show()
    
    def FWLoaderGo(self):
        self.CWFirmwareConfig.loadRequired()
        
    def addExampleScripts(self):
        self.exampleScriptAct = QAction('&Example Scripts', self, statusTip='Predefined Scripts')
        
        self.projectMenu.addSeparator()
        self.projectMenu.addAction(self.exampleScriptAct)
        
        subMenu = QMenu("Submenu", self)
              
        self.scriptList = []
              
        if os.path.isdir("scripts"):
            for fn in os.listdir('scripts/.'):
                fnfull = 'scripts/'+fn
                if os.path.isfile(fnfull) and fnfull.lower().endswith('.py'):
                    try:
                        m = importlib.import_module('scripts.'+os.path.splitext(fn)[0])
                        m.name()
                        self.scriptList.append(m)
                    except ImportError, e:
                        #for debugging uncomment this:
                        #print str(e)
                        pass
                    
                    except AttributeError,e:
                        #for debugging uncomment this:
                        #print str(e)
                        pass
                    
        for t in self.scriptList:
            subMenu.addAction(QAction(t.name(), self, statusTip=t.tip(), triggered=partial(self.runScript, t)))
            
        self.exampleScriptAct.setMenu(subMenu)
        
    def runScript(self, mod):
        self.console.append( "****Running Script: %s"%mod.name() )
        m = mod.userScript(self)
        m.run()
        self.console.append( "****Finished Script: %s"%mod.name() )
        

    def addToolMenu(self):
        self.CWFirmwareConfig = FWLoaderConfig(self, console=self.console)
        
        self.CWFirmwareConfigAct = QAction('Config CW Firmware', self,                               
                               statusTip='Configure ChipWhisperer FW Paths',
                               triggered=self.FWLoaderConfig)

        self.CWFirmwareGoAct = QAction('Download CW Firmware', self,                               
                               statusTip='Download Firmware+FPGA To Hardware',
                               triggered=self.FWLoaderGo)
        
        self.toolMenu.addAction(self.CWFirmwareConfigAct)
        self.toolMenu.addAction(self.CWFirmwareGoAct)
        self.toolMenu.addSeparator()

        
    def addWaveforms(self):
        self.waveformDock = self.addTraceDock("Capture Waveform (Channel 1)")
        
        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault()       
        
    def addSettingsDocks(self):      
        self.setupParametersTree()        
        self.settingsNormalDock = self.addSettings(self.paramTree, "General Settings")
        self.settingsScopeDock = self.addSettings(self.scopeParamTree, "Scope Settings")
        self.settingsTargetDock = self.addSettings(self.targetParamTree, "Target Settings")
        self.settingsTraceDock = self.addSettings(self.traceParamTree, "Trace Settings")
        self.settingsAuxDock = self.addSettings(self.auxParamTree, "Aux Settings")
        self.settingsAuxDock.setVisible(False)

    def setupParametersTree(self):
        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)
        
        self.scopeParamTree = ParameterTree()
        self.targetParamTree = ParameterTree()
        self.traceParamTree = ParameterTree()
        self.auxParamTree = ParameterTree()

    def reloadScopeParamList(self, lst=None): 
        if self.scope is not None:
            ExtendedParameter.reloadParams(self.scope.paramList(), self.scopeParamTree)
        
    def reloadTargetParamList(self, lst=None):
        if self.target is not None:
            ExtendedParameter.reloadParams(self.target.paramList(), self.targetParamTree)
        
    def reloadTraceParamList(self, lst=None):
        if self.traceparams is not None:
            try:
                ExtendedParameter.reloadParams(self.traceparams.paramList(), self.traceParamTree)
            except AttributeError:
                #Some trace writers have no configuration options
                pass

    def reloadAuxParamList(self, lst=None):
        if self.aux is not None:
            try:
                ExtendedParameter.reloadParams(self.aux.paramList(), self.auxParamTree)
            except AttributeError:
                # Some trace writers have no configuration options
                pass
        
    def reloadParamList(self, lst=None):
        ExtendedParameter.reloadParams(self.paramList(), self.paramTree)
        
    def paramList(self):
        p = []
        p.append(self.params)     
        return p   

    def newScopeData(self,  data=None, offset=0):
        self.waveformDock.widget().passTrace(data, offset)

    def setConfigWidget(self, widget):
        self.configdock.setWidget(widget)

    def addToolbars(self):
        self.addCaptureTools()

    def addCaptureTools(self):
        capture1 = QAction(QIcon(':/images/play1.png'), 'Capture 1', self)
        capture1.triggered.connect(self.capture1)
        captureM = QAction(QIcon(':/images/playM.png'), 'Capture Multi', self)
        captureM.triggered.connect(self.captureM)
        
        self.captureStatus = QToolButton()
        self.captureStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Master: Disconnected', self)
        self.captureStatusActionDis.triggered.connect(self.doConDis)
        self.captureStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Master: Connected', self)
        self.captureStatus.setDefaultAction(self.captureStatusActionDis)

        self.CaptureToolbar = self.addToolBar('Capture Tools')
        self.CaptureToolbar.setObjectName('Capture Tools')
        self.CaptureToolbar.addAction(capture1)
        self.CaptureToolbar.addAction(captureM)
        self.CaptureToolbar.addWidget(QLabel('Master:'))
        self.CaptureToolbar.addWidget(self.captureStatus)
        #self.CaptureToolbar.setEnabled(False)
        
        # Scope Toolbar
        self.scopeStatus = QToolButton()
        self.scopeStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Scope: Disconnected', self)
        self.scopeStatusActionDis.triggered.connect(self.doConDisScope)
        self.scopeStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Scope: Connected', self)
        self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        self.ScopeToolbar = self.addToolBar('Scope Toolbar')
        self.ScopeToolbar.setObjectName('Scope Toolbar')
        self.ScopeToolbar.addWidget(QLabel('Scope:'))
        self.ScopeToolbar.addWidget(self.scopeStatus)
        self.ScopeToolbar.setEnabled(False)

        # Target Toolbar
        self.targetStatus = QToolButton()
        self.targetStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Target: Disconnected', self)
        self.targetStatusActionDis.triggered.connect(self.doConDisTarget)
        self.targetStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Target: Connected', self)
        self.targetStatus.setDefaultAction(self.targetStatusActionDis)
        
        self.TargetToolbar = self.addToolBar('Target Toolbar')
        self.TargetToolbar.setObjectName('Target Toolbar')
        self.TargetToolbar.addWidget(QLabel('Target:'))
        self.TargetToolbar.addWidget(self.targetStatus)
        self.TargetToolbar.setEnabled(False)


    def masterStatusChanged(self):
        # Deal with multiple

        if self.scopeStatus.defaultAction() == self.scopeStatusActionCon:
            scopeStat = True
        else:
            scopeStat = False

        if self.targetStatus.defaultAction() == self.targetStatusActionCon:
            targetStat = True
        else:
            targetStat = False

        if targetStat or scopeStat:
            self.captureStatus.setDefaultAction(self.captureStatusActionCon)
        else:
            self.captureStatus.setDefaultAction(self.captureStatusActionDis)

    def scopeStatusChanged(self, status=True, text=None):
        """Callback when scope connection successful"""
        # self.CaptureToolbar.setEnabled(status)

        if status:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionCon)
        else:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        self.masterStatusChanged()

    def targetStatusChanged(self, status=True, text=None):
        """Callback when target connection successful"""
        #self.CaptureToolbar.setEnabled(status)
        
        if status:
            self.targetStatus.setDefaultAction(self.targetStatusActionCon)
        else:
            self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        self.masterStatusChanged()

    def doConDisTarget(self, con=None):
        """Toggle connect button pushed (target), alternatively can use via API by setting 'con' to True or False"""

        if self.target is None:
            return
        
        #Triggered from GUI
        if con is None:
            if self.targetStatus.defaultAction() == self.targetStatusActionDis:
                con = True
            else:
                con = False
        
        #Triggered from API
        try:
            if con:
                self.target.con()
                self.statusBar().showMessage("Target Connected")
            else:
                self.target.dis()
        except IOError, e:
            self.console.append("Target Error: %s"%str(e))
            
        
    def doConDisScope(self, con=None):
        """Toggle connect button pushed (scope), alternatively can use via API by setting 'con' to True or False"""
        if self.scope is None:
            return

        # Triggered from GUI
        if con is None:
            if self.scopeStatus.defaultAction() == self.scopeStatusActionDis:
                con = True
            else:
                con = False
        
        # Triggered from API
        try:
            if con:
                self.scope.con()
                self.statusBar().showMessage("Scope Connected")
                #Pass to target if required
                try:
                    self.target.setOpenADC(self.scope.qtadc.ser)
                except:
                    pass      
            else:
                self.scope.dis()
        except IOError, e:
            self.console.append("Target Error: %s" % str(e))


    def doConDis(self):
        """Toggle connect button pushed (master): attempts both target & scope connection"""
        if self.captureStatus.defaultAction() == self.captureStatusActionDis:      
            self.doConDisScope(True)
            self.doConDisTarget(True)
        else:
            self.doConDisScope(False)
            self.doConDisTarget(False)

    def capture1(self):
        if self.target.driver:
            target = self.target.driver
        else:
            target = None
                     
        #Check key with target
        if target:
            self.setKey(target.checkEncryptionKey(self.key), True, True)
                     
        if self.fixedPlain:
            ptInput = self.plaintext
        else:
            ptInput = None

        ac = acquisitionController(self.scope, target, writer=None, aux=self.aux, fixedPlain=ptInput, esm=self.esm)
        ac.doSingleReading(key=self.key)
        self.statusBar().showMessage("One Capture Complete")

    def printTraceNum(self, num, data, offset=0):
        self.statusBar().showMessage("Trace %d done"%num)
        #self.newScopeData(data, offset)        

    def captureM(self):
        if self.target.driver:
            target = self.target.driver
        else:
            target = None
            
        #Check key with target
        if target:
            self.setKey(target.checkEncryptionKey(self.key), True, True)
            
        starttime = datetime.now()  
        baseprefix = starttime.strftime('%Y.%m.%d-%H.%M.%S')
        prefix = baseprefix + "_"
            
        #Load trace writer        
        if self.trace is not None:
            writer = self.trace(self.traceparams)
            writer.config.setAttr("prefix", prefix)
            writer.config.setConfigFilename(self.proj.datadirectory + "traces/config_" + prefix + ".cfg")
            writer.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
        else:
            writer = None
                    
        if self.fixedPlain:
            ptInput = self.plaintext
        else:
            ptInput = None

        rndKey = self.findParam('newKeyAlways').value()

        if self.aux is not None:
            self.aux.setPrefix(baseprefix)

        ac = acquisitionController(self.scope, target, writer, aux=self.aux,
                                   esm=self.esm, fixedPlain=ptInput,
                                   newKeyPerTrace=rndKey)

        ac.traceDone.connect(self.printTraceNum)
        tn = self.numTraces
        ac.setMaxtraces(tn)        
        ac.doReadings(addToList=self.manageTraces, key=self.key)
        self.statusBar().showMessage("%d Captures Completed"%tn)
        
        stoptime = datetime.now()
        
        self.console.append("Capture delta time: %s"%str(stoptime-starttime))
        
        return writer
        
    def scopeChanged(self, newscope):        
        self.scope = newscope
        if self.scope is not None:
            self.scope.paramListUpdated.connect(self.reloadScopeParamList)                     
            self.scope.dataUpdated.connect(self.newScopeData)
            self.scope.connectStatus.connect(self.scopeStatusChanged)
            self.reloadScopeParamList()
            self.ScopeToolbar.setEnabled(True)
        else:
            self.ScopeToolbar.setEnabled(False)

    def traceChanged(self, newtrace):
        self.trace = newtrace
        try:
            self.traceparams = newtrace.getParams()
        except AttributeError:
            self.traceparams = None
        except TypeError:
            self.traceparams = None
            
        self.reloadTraceParamList()

    def auxChanged(self, newaux):
        self.aux = newaux
        try:
            newaux.paramList()
            newaux.paramListUpdated.connect(self.reloadAuxParamList)
            self.reloadAuxParamList()
            self.settingsAuxDock.setVisible(True)
        except AttributeError:
            self.settingsAuxDock.setVisible(False)
        except TypeError:
            self.settingsAuxDock.setVisible(False)
  
    def newProject(self):        
        self.proj = ProjectFormat()
        self.proj.setProgramName("ChipWhisperer-Capture")
        self.proj.setProgramVersion("2.00")
        self.proj.addParamTree(self)
        self.proj.addParamTree(self.scope)
        self.proj.addParamTree(self.target)      
        self.proj.setTraceManager(self.manageTraces)  
        self.setCurrentFile(None)
  
    def saveProject(self):
        if self.proj.hasFilename() == False:
            fname, _ = QFileDialog.getSaveFileName(self, 'Save New File','.','*.cwp')
            
            if fname is None:
                return
            
            self.proj.setFilename(fname)
            self.setCurrentFile(fname)
            
        self.proj.save()
        self.dirty = False
        self.updateTitleBar()
        self.statusBar().showMessage("Project Saved")
  
def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Capture V2")
    return app
  
if __name__ == '__main__':
    app = makeApplication()
    
    # Create and show the form
    window = ChipWhispererCapture()
    window.show()
   
    # Run the main Qt loop
    sys.exit(app.exec_())
