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

from _smartcard_gui import SmartCardGUICard
from _base import TargetTemplate
from chipwhisperer.common.utils import pluginmanager


class SmartCard(TargetTemplate):
    _name = "Smart Card"

    def __init__(self, parentParam=None):
        TargetTemplate.__init__(self, parentParam)
        self.driver = None
        self.window = None

        self.setupActiveParams([lambda: self.lazy(self.driver)])
        readers = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets.smartcard_readers", True, False, self)

        self.setupActiveParams([lambda: self.lazy(self.protocol)])
        protocols = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets.smartcard_protocols", True, False, self)

        from chipwhisperer.capture.targets.smartcard_readers.chipwhisperer_ser import ReaderChipWhispererSER
        self.params.addChildren([
            {'name':'Reader Hardware', 'type':'list', 'values':readers, 'value':readers[ReaderChipWhispererSER._name], 'set':self.setConnection},
            {'name':'SmartCard Protocol', 'type':'list', 'values':protocols , 'value':None, 'set':self.setProtocol},
            {'name':'SmartCard Explorer', 'type':'action', 'action':lambda: self.scgui.show()}
        ])

    def __del__(self):
        self.close()

    def keyLen(self):
        """ Return key length in BYTES """
        return 16

    def setConnection(self, con):
        self.driver = con        
        self.paramListUpdated.emit()
        self.scgui.setConnection(con)
        
    def setProtocol(self, con):
        self.protocol = con
        self.paramListUpdated.emit()
        self.protocol.setReaderHardware(self.driver)

    def con(self, scope = None):
        self.driver.con(scope)
        self.driver.flush()
        self.protocol.setReaderHardware(self.driver)
        self.connectStatus.setValue(True)

    def close(self):
        if self.driver != None:
            self.driver.close()
            # self.driver = None
        return
        
    def init(self):
        if not self.protocol: raise Warning("No protocol selected.")
        self.protocol.init()

    def reinit(self):
        if not self.protocol: raise Warning("No protocol selected.")
        self.protocol.reinit()
      
    def loadEncryptionKey(self, key):
        self.protocol.loadEncryptionKey(key)
        self.key = key
      
    def loadInput(self, inputtext):
        self.protocol.loadInput(inputtext)
        self.input = inputtext

    def isDone(self):
        return self.protocol.isDone()

    def readOutput(self):        
        return self.protocol.readOutput()

    def go(self):
        self.protocol.go()

    def setModeEncrypt(self):
        pass
    
    def checkEncryptionKey(self, key):
        return key

    def setupGuiActions(self, mainWindow):
        if not hasattr(self, 'scgui'):
            self.scgui = SmartCardGUICard(mainWindow)
        return [['SmartCard Explorer','', self.scgui.show],]
