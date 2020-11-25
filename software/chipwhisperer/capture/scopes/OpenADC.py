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
from .cwhardware import ChipWhispererDecodeTrigger, ChipWhispererDigitalPattern, ChipWhispererExtra, ChipWhispererSAD
from . import _qt as openadc_qt
from .base import ScopeTemplate
from .openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.util import dict_to_str, DelayedKeyboardInterrupt
from collections import OrderedDict
import time

class OpenADC(ScopeTemplate, util.DisableNewAttr):
    """OpenADC scope object.

    This class contains the public API for the OpenADC hardware, including the
    ChipWhisperer Lite/ CW1200 Pro boards. It includes specific settings for
    each of these devices.

    To connect to one of these devices, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope(type=scopes.OpenADC)

    Some sane default settings are available via::

        scope.default_setup()

    This code will automatically detect an attached ChipWhisperer device and
    connect to it.

    For more help about scope settings, try help() on each of the ChipWhisperer
    scope submodules (scope.gain, scope.adc, scope.clock, scope.io,
    scope.trigger, and scope.glitch):

     *  :attr:`scope.gain <.OpenADC.gain>`
     *  :attr:`scope.adc <.OpenADC.adc>`
     *  :attr:`scope.clock <.OpenADC.clock>`
     *  :attr:`scope.io <.OpenADC.io>`
     *  :attr:`scope.trigger <.OpenADC.trigger>`
     *  :attr:`scope.glitch <.OpenADC.glitch>`
     *  :meth:`scope.default_setup <.OpenADC.default_setup>`
     *  :meth:`scope.con <.OpenADC.con>`
     *  :meth:`scope.dis <.OpenADC.dis>`
     *  :meth:`scope.arm <.OpenADC.arm>`
     *  :meth:`scope.get_last_trace <.OpenADC.get_last_trace>`

    If you have a CW1200 ChipWhisperer Pro, you have access to some additional features:

     * :attr:`scope.SAD <.OpenADC.SAD>`
     * :attr:`scope.DecodeIO <.OpenADC.DecodeIO>`
     * :attr:`scope.adc.stream_mode (see scope.adc for more information)`
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

    @property
    def latest_fw(self):
        cw_type = self._getCWType()
        if cw_type == "cwlite":
            from chipwhisperer.hardware.firmware.cwlite import fwver
        elif cw_type == "cw1200":
            from chipwhisperer.hardware.firmware.cw1200 import fwver
        
        ret = OrderedDict()
        return {"major": fwver[0], "minor": fwver[1]}
    @property
    def fw_version(self):
        a = self.qtadc.sc.serial.readFwVersion()
        return {"major": a[0], "minor": a[1], "debug": a[2]}

    @property
    def sn(self):
        return self.scopetype.ser.snum

    def _getNAEUSB(self):
        return self.scopetype.dev._cwusb

    def default_setup(self):
        """Sets up sane capture defaults for this scope

         *  45dB gain
         *  5000 capture samples
         *  0 sample offset
         *  rising edge trigger
         *  7.37MHz clock output on hs2
         *  4*7.37MHz ADC clock
         *  tio1 = serial rx
         *  tio2 = serial tx

        .. versionadded:: 5.1
            Added default setup for OpenADC
        """
        self.gain.db = 25
        self.adc.samples = 5000
        self.adc.offset = 0
        self.adc.basic_mode = "rising_edge"
        self.clock.clkgen_freq = 7.37e6
        self.trigger.triggers = "tio4"
        self.io.tio1 = "serial_rx"
        self.io.tio2 = "serial_tx"
        self.io.hs2 = "clkgen"

        self.clock.adc_src = "clkgen_x4"

        count = 0
        while not self.clock.clkgen_locked:            
            self.clock.reset_dcms()
            time.sleep(0.05)
            count += 1

            if count == 5:
                logging.info("Could not lock clock for scope. This is typically safe to ignore. Reconnecting and retrying...")
                self.dis()
                time.sleep(0.25)
                self.con()
                time.sleep(0.25)
                self.gain.db = 25
                self.adc.samples = 5000
                self.adc.offset = 0
                self.adc.basic_mode = "rising_edge"
                self.clock.clkgen_freq = 7.37e6
                self.trigger.triggers = "tio4"
                self.io.tio1 = "serial_rx"
                self.io.tio2 = "serial_tx"
                self.io.hs2 = "clkgen"
                self.clock.adc_src = "clkgen_x4"

            if count > 10:
                raise OSError("Could not lock DCM. Try rerunning this function or calling scope.clock.reset_dcms(): {}".format(self))
    def dcmTimeout(self):
        if self.connectStatus:
            try:
                self.qtadc.sc.getStatus()
            except USBError:
                self.dis()
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
             -  ""
             -  "cwlite"
             -  "cw1200"
             -  "cwrev2"
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

    def get_name(self):
        """ Gets the name of the attached scope

        Returns:
            'ChipWhisperer Lite' if a Lite, 'ChipWhisperer Pro' if a Pro
        """
        name = self._getCWType()
        if name == "cwlite":
            return "ChipWhisperer Lite"
        elif name == "cw1200":
            return "ChipWhisperer Pro"

    def _con(self, sn=None):
        if self.scopetype is not None:
            self.scopetype.con(sn)

            if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
                self.qtadc.sc.usbcon = self.scopetype.ser._usbdev
            #self.qtadc.sc.usbcon = self.scopetype.ser._usbdev

            cwtype = self._getCWType()
            if cwtype != "":
                self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(cwtype, self.scopetype, self.qtadc.sc)

                util.chipwhisperer_extra = self.advancedSettings

                if cwtype == "cwrev2" or cwtype == "cw1200":
                    self.SAD = ChipWhispererSAD.ChipWhispererSAD(self.qtadc.sc)

                if cwtype == "cw1200":
                    self.decode_IO = ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger(self.qtadc.sc)
                    #self.advancedSettings.cwEXTRA.triggermux._set_is_pro(True)

                if cwtype == "cwcrev2":
                    self.digitalPattern = ChipWhispererDigitalPattern.ChipWhispererDigitalPattern(self.qtadc.sc)

            self.adc = self.qtadc.parm_trigger
            self.gain = self.qtadc.parm_gain
            self.clock = self.qtadc.parm_clock

            if cwtype == "cw1200":
                self.adc._is_pro = True
            if self.advancedSettings:
                self.io = self.advancedSettings.cwEXTRA.gpiomux
                self.trigger = self.advancedSettings.cwEXTRA.triggermux
                self.glitch = self.advancedSettings.glitch.glitchSettings
                if cwtype == "cw1200":
                    self.trigger = self.advancedSettings.cwEXTRA.protrigger


            self.disable_newattr()
            self._is_connected = True

            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            if self.advancedSettings is not None:
                self.advancedSettings = None
                util.chipwhisperer_extra = None

            if self.advancedSAD is not None:
                self.advancedSAD = None

            if self.digitalPattern is not None:
                self.digitalPattern = None

        # TODO Fix this hack
        if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
            self.qtadc.sc.usbcon = None

        self.enable_newattr()
        self._is_connected = False
        return True

    def arm(self):
        """Setup scope to begin capture/glitching when triggered.

        The scope must be armed before capture or glitching (when set to
        'ext_single') can begin.

        Raises:
           OSError: Scope isn't connected.
           Exception: Error when arming. This method catches these and
               disconnects before reraising them.
        """
        if self.connectStatus is False:
            raise OSError("Scope is not connected. Connect it first...")
        with DelayedKeyboardInterrupt():
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
        """Captures trace. Scope must be armed before capturing.

        Returns:
           True if capture timed out, false if it didn't.

        Raises:
           IOError: Unknown failure.
        """
        # need adc offset, adc_freq, samples cached
        with DelayedKeyboardInterrupt():
            if not self.adc.stream_mode:
                return self.qtadc.capture(self.adc.offset, self.clock.adc_freq, self.adc.samples)
            else:
                return self.qtadc.capture(None)

    def get_last_trace(self):
        """Return the last trace captured with this scope.

        Returns:
           Numpy array of the last capture trace.
        """
        return self.qtadc.datapoints

    getLastTrace = util.camel_case_deprecated(get_last_trace)

    def _dict_repr(self):
        dict = OrderedDict()
        dict['sn'] = self.sn
        dict['fw_version'] = self.fw_version
        dict['gain']    = self.gain._dict_repr()
        dict['adc']     = self.adc._dict_repr()
        dict['clock']   = self.clock._dict_repr()
        dict['trigger'] = self.trigger._dict_repr()
        dict['io']      = self.io._dict_repr()
        dict['glitch']  = self.glitch._dict_repr()
        if self._getCWType() == "cw1200":
            dict['SAD'] = self.SAD._dict_repr()
            dict['decode_IO'] = self.decode_IO._dict_repr()

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
