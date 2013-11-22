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
    
sys.path.append('../../common')
sys.path.append('../../../openadc/controlsw/python/common')

from ExtendedParameter import ExtendedParameter

try:
    from pyqtgraph.parametertree import Parameter   
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
    import scan
except ImportError:
    serial = None

import ChipWhispererExtra as ChipWhispererExtra

class OpenADCInterface_FTDI(QWidget):    
    paramListUpdated = Signal(list) 
    
    def __init__(self,oadcInstance,console=None,showScriptParameter=None):
        QWidget.__init__(self)
        self.showScriptParameter = showScriptParameter
        
        ftdiParams = [                  
                      {'name':'Refresh Device List', 'type':'action', 'action':self.serialRefresh},
                      {'name':'Serial Number', 'type':'list', 'values':[''], 'value':None, 'set':self.setSerialNumber},
                  ]           
       
        self.console = console
        self.serialNumber = None
       
        if (openadc_qt is None) or (ft is None):               
            self.ser = None
            raise ImportError("Needed imports for FTDI missing")
            return
        else:            
            self.ser = None
            self.scope = oadcInstance
            self.params = Parameter.create(name='OpenADC-FTDI', type='group', children=ftdiParams)
            ExtendedParameter.setupExtended(self.params, self)  
            
        #if target_chipwhisperer_extra is not None:
        #    self.cwAdvancedSettings = target_chipwhisperer_extra.QtInterface()
        #else:
        #    self.cwAdvancedSettings = None
        
    #def paramTreeChanged(self, param, changes):        
    #    if self.showScriptParameter is not None:
    #        self.showScriptParameter(param, changes, self.params)

    def setSerialNumber(self, snum):
        self.serialNumber = snum

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            try:
                self.dev = ft.openEx(str(self.serialNumber), ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
                self.dev.setBitMode(0x00, 0x40)
                self.dev.setTimeouts(500, 500)
                self.dev.setLatencyTimer(2)                
                self.ser = self
            except ft.ftd2xx.DeviceError, e:
                self.ser = None
                raise IOError("Could not open %s: %s"%(self.serialNumber,e))
            
        try:
            self.scope.con(self.ser)
            self.console.append("OpenADC Found, Connecting")            
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            self.console.append("OpenADC Error: %s"%(str(exctype) + str(value)))
            QMessageBox.warning(None, "FTDI Port", str(exctype) + str(value))
            raise IOError(e) 
        
        #if self.cwAdvancedSettings:
        #    self.cwAdvancedSettings.setOpenADC(self.scope)

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        

    def serialRefresh(self):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [" "]

        for p in self.params.children():
            if p.name() == 'Serial Number':
                p.setLimits(serialnames)
                p.setValue(serialnames[0])

        self.paramListUpdated.emit(self.paramList())

    def read(self, N=0, debug=False):
        return bytearray(self.dev.read(N))

    def write(self, data, debug=False):
        return self.dev.write(data)
            
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"
        
    def paramList(self):
        p = [self.params]        
        #if self.cwAdvancedSettings is not None:
        #    for a in self.cwAdvancedSettings.paramList(): p.append(a)            
        return p

class OpenADCInterface_Serial(QWidget):    
    paramListUpdated = Signal(list) 
    
    def __init__(self,oadcInstance,console=None,showScriptParameter=None):
        QWidget.__init__(self)
        self.showScriptParameter = showScriptParameter
        
        ftdiParams = [                  
                      {'name':'Refresh List', 'type':'action', 'action':self.serialRefresh},
                      {'name':'Port', 'type':'list', 'values':[''], 'value':None, 'set':self.setPortName},
                  ]           
       
        self.console = console
        self.ser = None
       
        if (openadc_qt is None) or (serial is None):               
            self.ser = None
            raise ImportError("Needed imports for serial missing")
            return
        else:            
            self.ser = None
            self.scope = oadcInstance
            self.params = Parameter.create(name='OpenADC-Serial', type='group', children=ftdiParams)
            ExtendedParameter.setupExtended(self.params, self)  
            
    def setPortName(self, snum):
        self.portName = snum

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            self.ser = serial.Serial()
            self.ser.port     = self.portName
            self.ser.baudrate = 512000;
            self.ser.timeout  = 2     # 2 second timeout


            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException, e:
                    attempts = attempts - 1
                    self.ser = None
                    if attempts == 0:
                        raise IOError("Could not open %s"%self.ser.name)
            
        try:
            self.scope.con(self.ser)
            self.console.append("OpenADC Found, Connecting")            
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            self.console.append("OpenADC Error: %s"%(str(exctype) + str(value)))
            QMessageBox.warning(None, "Serial Port", str(exctype) + str(value))
            raise IOError(e) 
        

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        

    def serialRefresh(self):
        serialnames = scan.scan()
        if serialnames == None:
            serialnames = [" "]
            
        for p in self.params.children():
            if p.name() == 'Port':
                p.setLimits(serialnames)
                p.setValue(serialnames[0])

        self.paramListUpdated.emit(self.paramList())
            
    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"
        
    def paramList(self):
        p = [self.params]            
        return p

      
            
class OpenADCInterface_ZTEX(QWidget):    
    paramListUpdated = Signal(list) 
    
    def __init__(self,oadcInstance,console=None,showScriptParameter=None):
        QWidget.__init__(self)
        self.showScriptParameter = showScriptParameter
        
        ztexParams = [                  
                      #No Parameters for ZTEX
                  ]           
       
        self.console = console
        self.ser = None
        
       
        if (openadc_qt is None) or (usb is None):               
            raise ImportError("Needed imports for ChipWhisperer missing")
        else:            
            self.scope = oadcInstance
            self.params = Parameter.create(name='OpenADC-ZTEX', type='group', children=ztexParams)
            ExtendedParameter.setupExtended(self.params, self)  
            
            
        #if target_chipwhisperer_extra is not None:
        #    self.cwAdvancedSettings = target_chipwhisperer_extra.QtInterface()
        #else:
        #    self.cwAdvancedSettings = None
        
    #def paramTreeChanged(self, param, changes):
    #    if self.showScriptParameter is not None:
    #        self.showScriptParameter(param, changes, self.params)

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            try:
                dev = usb.core.find(idVendor=0x221A, idProduct=0x0100)
            except IOError, e:
                exctype, value = sys.exc_info()[:2]
                QMessageBox.warning(None, "FX2 Port", str(exctype) + str(value))
                raise IOError(e)

            if dev is None:
                QMessageBox.warning(None, "FX2 Port", "Could not open USB Device")            
                raise IOError("Could not open USB Device")

            dev.set_configuration()            

            self.dev = dev
            self.writeEP = 0x06
            self.readEP = 0x82
            self.interface = 0
            self.ser = self

        try:
            self.scope.con(self.ser)
            self.console.append("OpenADC Found, Connecting")
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            self.console.append("OpenADC Error: %s"%(str(exctype) + str(value)))
            self.console.append("Did you download firmware/FPGA data to ChipWhisperer?")
            QMessageBox.warning(None, "FX2 Port", str(exctype) + str(value))
            raise IOError(e)

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        

    def read(self, N=0, debug=False):
        try:
            data = self.dev.read(self.readEP, N, self.interface, 500)
        except IOError:
            return []
        
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
        
    def paramList(self):
        p = [self.params]                   
        return p

class OpenADCInterface(QObject):
    connectStatus = Signal(bool)
    dataUpdated = Signal(list, int)
    paramListUpdated = Signal(list)    

    def __init__(self, parent=None, console=None, showScriptParameter=None):          
        super(OpenADCInterface, self).__init__(parent)
        self.parent = parent
        self.qtadc = openadc_qt.OpenADCQt(includePreview=False,  setupLayout=False, console=console, showScriptParameter=showScriptParameter)
        self.qtadc.setupParameterTree(False)
        self.qtadc.dataUpdated.connect(self.doDataUpdated)
        self.scopetype = None
        self.datapoints = []
        
        try:
            cwrev2 = OpenADCInterface_ZTEX(self.qtadc, console=console, showScriptParameter=showScriptParameter)
        except ImportError:
            cwrev2 = None
            
        try:
            ftdi = OpenADCInterface_FTDI(self.qtadc, console=console, showScriptParameter=showScriptParameter)
        except ImportError:
            ftdi = None
            
        try:
            cwser = OpenADCInterface_Serial(self.qtadc, console=console, showScriptParameter=showScriptParameter)
        except ImportError:
            cwser = None
            
        self.setCurrentScope(cwrev2, False)
        defscope = cwrev2
        
        cw_cons = {}
        
        if ftdi:
            ftdi.paramListUpdated.connect(self.emitParamListUpdated)
            cw_cons["FTDI (SASEBO-W/SAKURA-G)"] = ftdi
        
        if cwrev2:
            cwrev2.paramListUpdated.connect(self.emitParamListUpdated)
            cw_cons["ChipWhisperer Rev2"] = cwrev2
            
        if cwser:
            cwser.paramListUpdated.connect(self.emitParamListUpdated)
            cw_cons["Serial Port (LX9)"] = cwser
        
        
        
        self.advancedSettings = None
        
        scopeParams = [{'name':'connection', 'type':'list', 'values':cw_cons, 'value':defscope, 'set':self.setCurrentScope},
                       {'name':'Auto-Refresh DCM Status', 'type':'bool', 'value':True, 'set':self.setAutorefreshDCM}
                      ]
        
        self.params = Parameter.create(name='OpenADC Interface', type='group', children=scopeParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        self.setCurrentScope(defscope)
        
        self.refreshTimer = QTimer()  
        self.refreshTimer.timeout.connect(self.dcmTimeout)
        self.refreshTimer.setInterval(1000)
        
    def dcmTimeout(self):
        if self.parent:
            self.parent.setParameter(['OpenADC', 'Clock Setup', 'Refresh Status', None])
            self.parent.setParameter(['OpenADC', 'Trigger Setup', 'Refresh Status', None])
        
    def setAutorefreshDCM(self, enabled):
        if enabled:
            self.refreshTimer.start()
        else:
            self.refreshTimer.stop()
        
    def emitParamListUpdated(self):
        self.paramListUpdated.emit(self.paramList())
        
    #def paramTreeChanged(self, param, changes):        
    #    if self.showScriptParameter is not None:
    #        self.showScriptParameter(param, changes, self.params)

    def setCurrentScope(self, scope, update=True):        
        self.scopetype = scope
        if update:
            self.paramListUpdated.emit(self.paramList())
   
    def con(self):
        if self.scopetype is not None:
            self.scopetype.con()  
            self.refreshTimer.start()
            
            if "ChipWhisper" in self.qtadc.sc.hwInfo.versions()[2]:            
                #For OpenADC: If we have CW Stuff, add that now
                self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(self.showScriptParameter)
                self.advancedSettings.setOpenADC(self.qtadc)
                
                self.paramListUpdated.emit(None)
            
            self.connectStatus.emit(True)
            

    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()  
            self.refreshTimer.stop()
            self.connectStatus.emit(True)

    def doDataUpdated(self,  l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        self.qtadc.arm()

    def capture(self, update=True, NumberPoints=None):
        """Raises IOError if unknown failure, returns 'True' if successful, 'False' if timeout"""
        return self.qtadc.capture(update, NumberPoints)    
        
    def paramList(self):
        p = []       
        p.append(self.params)  
         
        if self.scopetype is not None:
            for a in self.scopetype.paramList(): p.append(a)         
        
        p.append(self.qtadc.params)  
            
        if self.advancedSettings is not None:
            for a in self.advancedSettings.paramList(): p.append(a)    
            
        return p