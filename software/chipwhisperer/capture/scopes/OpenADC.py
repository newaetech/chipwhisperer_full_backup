#!/usr/bin/python
# HIGHLEVEL_CLASSLOAD_FAIL_FUNC_WARN
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
import logging
from usb import USBError
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererDecodeTrigger as ChipWhispererDecodeTrigger
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererDigitalPattern as ChipWhispererDigitalPattern
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra as ChipWhispererExtra
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD as ChipWhispererSAD
from . import _qt as openadc_qt
from .base import ScopeTemplate
from chipwhisperer.capture.scopes.openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from chipwhisperer.common.utils import util, timer, pluginmanager
from chipwhisperer.common.utils.parameter import Parameter, setupSetParam
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.util import dict_to_str
from collections import OrderedDict

class OpenADC(ScopeTemplate, Plugin, util.DisableNewAttr):
    """OpenADC scope object.

    This class contains the public API for the OpenADC hardware, including the
    ChipWhisperer Lite/CW1200/Rev 2 boards. It includes specific settings for
    each of these devices.

    To connect to one of these devices, the easiest method is

    >>> import chipwhisperer as cw
    >>> scope = cw.scope()

    This code will automatically detect an attached ChipWhisperer device and
    connect to it.

    For more help about scope settings, try help() on each of the ChipWhisperer
    scope submodules:
        scope.gain
        scope.adc
        scope.clock
        scope.io
        scope.trigger
        scope.glitch
    """

    _name = "ChipWhisperer/OpenADC"

    def __init__(self):
        ScopeTemplate.__init__(self)

        self.qtadc = openadc_qt.OpenADCQt()
        self.qtadc.dataUpdated.connect(self.newDataReceived)
        # Bonus Modules for ChipWhisperer
        self.advancedSettings = None
        self.advancedSAD = None
        self.digitalPattern = None

        self._is_connected = False

        self.scopetype = OpenADCInterface_NAEUSBChip(self.qtadc)
        self.params.init()
        self.params.append(self.qtadc.getParams())

    def dcmTimeout(self):
        if self.connectStatus.value():
            try:
                self.qtadc.sc.getStatus()
                # The following happen with signals, so a failure will likely occur outside of the try...except
                # For this reason we do the call to .getStatus() to verify USB connection first
                Parameter.setParameter(['OpenADC', 'Clock Setup', 'Refresh Status', None], blockSignal=True)
                Parameter.setParameter(['OpenADC', 'Trigger Setup', 'Refresh Status', None], blockSignal=True)
            except USBError:
                self.dis()
                self.scope_disconnected_signal.emit()
                raise Warning("Error in the scope. It may have been disconnected.")
            except Exception as e:
                self.dis()
                raise e

    def getCurrentScope(self):
        return self.scopetype

    def setCurrentScope(self, scope):
        self.scopetype = scope

    def _getCWType(self):
        """Find out which type of ChipWhisperer this device is.

        Returns:
            One of the following:
            - ""
            - "cwlite"
            - "cw1200"
            - "cwrev2"
        """
        hwInfoVer = self.qtadc.sc.hwInfo.versions()[2]
        if "ChipWhisperer" in hwInfoVer:
            if "Lite" in hwInfoVer:
                return "cwlite"
            elif "CW1200" in hwInfoVer:
                return "cw1200"
            else:
                return "cwrev2"
        return ""

    def _con(self):
        if self.scopetype is not None:
            self.scopetype.con()

            # TODO Fix this hack
            if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
                self.qtadc.sc.usbcon = self.scopetype.ser._usbdev

            cwtype = self._getCWType()
            if cwtype != "":

                #For OpenADC: If we have CW Stuff, add that now
                #TODO FIXME - this shouldn't be needed, but if you connect/disconnect you can no longer
                #             use self.api.setParameter(...) with CWExtra-specific settings. The OpenADC
                #             settings will work, but not CWExtra ones? For now this works, but doesn't let
                #             you change the OpenADC type.
                #if self.advancedSettings is None:
                self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(cwtype, self.scopetype, self.qtadc.sc)
                self.params.append(self.advancedSettings.getParams())

                util.chipwhisperer_extra = self.advancedSettings

                if cwtype == "cwrev2" or cwtype == "cw1200":
                    self.advancedSAD = ChipWhispererSAD.ChipWhispererSAD(self.qtadc.sc)
                    self.params.append(self.advancedSAD.getParams())

                if cwtype == "cw1200":
                    self.decodeIO = ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger(self.qtadc.sc)
                    self.params.append(self.decodeIO.getParams())

                if cwtype == "cwcrev2":
                    self.digitalPattern = ChipWhispererDigitalPattern.ChipWhispererDigitalPattern(self.qtadc.sc)
                    self.params.append(self.digitalPattern.getParams())

            self.adc = self.qtadc.parm_trigger
            self.gain = self.qtadc.parm_gain
            self.clock = self.qtadc.parm_clock

            if self.advancedSettings:
                self.io = self.advancedSettings.cwEXTRA.gpiomux
                self.trigger = self.advancedSettings.cwEXTRA.triggermux
                self.glitch = self.advancedSettings.glitch.glitchSettings

            self.disable_newattr()
            self._is_connected = True
            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            if self.advancedSettings is not None:
                self.advancedSettings.getParams().remove()
                self.advancedSettings = None
                util.chipwhisperer_extra = None

            if self.advancedSAD is not None:
                self.advancedSAD.getParams().remove()
                self.advancedSAD = None

            if self.digitalPattern is not None:
                self.digitalPattern.getParams().remove()
                self.digitalPattern = None

        # TODO Fix this hack
        if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
            self.qtadc.sc.usbcon = None

        self.enable_newattr()
        self._is_connected = False
        return True

    def arm(self):
        if self.connectStatus.value() is False:
            raise Warning("Scope \"" + self.getName() + "\" is not connected. Connect it first...")

        try:
            if self.advancedSettings:
                self.advancedSettings.armPreScope()

            self.qtadc.arm()

            if self.advancedSettings:
                 self.advancedSettings.armPostScope()

            self.qtadc.startCaptureThread()
        except Exception:
            self.dis()
            raise

    def capture(self):
        """Raises IOError if unknown failure, returns 'True' if timeout, 'False' if no timeout"""
        ret = self.qtadc.capture()
        return ret

    def getLastTrace(self):
        """Return the last trace captured with this scope.
        """
        return self.qtadc.datapoints


    def _dict_repr(self):
        dict = OrderedDict()
        dict['gain']    = self.gain._dict_repr()
        dict['adc']     = self.adc._dict_repr()
        dict['clock']   = self.clock._dict_repr()
        dict['trigger'] = self.trigger._dict_repr()
        dict['io']      = self.io._dict_repr()
        dict['glitch']  = self.glitch._dict_repr()

        return dict

    def __repr__(self):
        # Add some extra information about ChipWhisperer type here
        if self._is_connected:
            ret = "%s Device\n" % self._getCWType()
            return ret + dict_to_str(self._dict_repr())
        else:
            ret = "ChipWhisperer/OpenADC device (disconnected)"
            return ret

    def __str__(self):
        return self.__repr__()