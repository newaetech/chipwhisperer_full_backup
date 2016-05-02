#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

import chipwhisperer.capture.scopes.cwhardware.ChipWhispererDigitalPattern as ChipWhispererDigitalPattern
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra as ChipWhispererExtra
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD as ChipWhispererSAD
import _qt as openadc_qt
from _base import ScopeTemplate
from chipwhisperer.capture.scopes.openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils import util, timer, pluginmanager


#TODO - Rename this or the other OpenADCInterface - not good having two classes with same name
class OpenADCInterface(ScopeTemplate):
    name = "ChipWhisperer/OpenADC"

    def __init__(self, parentParam=None):
        super(OpenADCInterface, self).__init__(parentParam)

        self.scopetype = None
        self.qtadc = openadc_qt.OpenADCQt()
        self.qtadc.dataUpdated.connect(self.doDataUpdated)

        scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes.openadc_interface", True, False, self, self.qtadc)
        self.params.addChildren([
            {'name':'Connection', 'key':'con', 'type':'list', 'values':scopes, 'value':scopes[OpenADCInterface_NAEUSBChip.name], 'set':self.setCurrentScope},
            {'name':'Auto-Refresh DCM Status', 'type':'bool', 'value':True, 'set':self.setAutorefreshDCM}
        ])
        self.setupActiveParams([lambda: self.lazy(self), lambda: self.lazy(self.scopetype), lambda: self.lazy(self.qtadc),
                                lambda: self.lazy(self.advancedSettings), lambda: self.lazy(self.advancedSAD),
                                lambda: self.lazy(self.digitalPattern)])

        # Bonus Modules for ChipWhisperer
        self.advancedSettings = None
        self.advancedSAD = None
        self.digitalPattern = None
        self.refreshTimer = timer.runTask(self.dcmTimeout, 1)
        self.setCurrentScope(self.findParam('con').value())

    def dcmTimeout(self):
        try:
            self.qtadc.sc.getStatus()
            # The following happen with signals, so a failure will likely occur outside of the try...except
            # For this reason we do the call to .getStatus() to verify USB connection first
            CWCoreAPI.getInstance().setParameter(['OpenADC', 'Clock Setup', 'Refresh Status', None])
            CWCoreAPI.getInstance().setParameter(['OpenADC', 'Trigger Setup', 'Refresh Status', None])
        except Exception:
            self.dis()
            raise

    def setAutorefreshDCM(self, enabled):
        if enabled:
            self.refreshTimer.start()
        else:
            self.refreshTimer.stop()

    def setCurrentScope(self, scope, update=True):
        self.scopetype = scope
        if update:
            self.paramListUpdated.emit()

    def _con(self):
        if self.scopetype is not None:
            
            self.scopetype.con()
            self.refreshTimer.start()

            # TODO Fix this hack
            if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
                self.qtadc.sc.usbcon = self.scopetype.ser._usbdev

            if "ChipWhisperer" in self.qtadc.sc.hwInfo.versions()[2]:

                if "Lite" in self.qtadc.sc.hwInfo.versions()[2]:
                    cwtype = "cwlite"
                else:
                    cwtype = "cwrev2"

                #For OpenADC: If we have CW Stuff, add that now
                self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(self, cwtype, self.scopetype)
                self.advancedSettings.setOpenADC(self.qtadc)

                util.chipwhisperer_extra = self.advancedSettings

                if "Lite" not in self.qtadc.sc.hwInfo.versions()[2]:
                    self.advancedSAD = ChipWhispererSAD.ChipWhispererSAD()
                    self.advancedSAD.setOpenADC(self.qtadc)

                    self.digitalPattern = ChipWhispererDigitalPattern.ChipWhispererDigitalPattern()
                    self.digitalPattern.setOpenADC(self.qtadc)

                self.paramListUpdated.emit()

            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.refreshTimer.stop()
            self.scopetype.dis()
        return True

    def doDataUpdated(self, l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        if self.connectStatus.value() is False:
            raise Warning("Scope \"" + self.getName() + "\" is not connected. Connect it first...")
        # self.advancedSettings.glitch.resetDCMs()
        if self.advancedSettings:
            self.advancedSettings.armPreScope()

        try:
            self.qtadc.arm()
        except Exception as e:
            self.dis()
            raise e

        if self.advancedSettings:
            self.advancedSettings.armPostScope()

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        """Raises IOError if unknown failure, returns 'True' if timeout, 'False' if no timeout"""
        return self.qtadc.capture(update, NumberPoints, waitingCallback)
