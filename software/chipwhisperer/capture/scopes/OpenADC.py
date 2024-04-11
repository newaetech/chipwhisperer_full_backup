#!/usr/bin/python
# HIGHLEVEL_CLASSLOAD_FAIL_FUNC_WARN
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2022, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com
#
#=================================================
from chipwhisperer.logging import *
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
from ...hardware.naeusb.serial import USART
from .cwhardware import ChipWhispererDecodeTrigger, ChipWhispererExtra, \
     ChipWhispererSAD, ChipWhispererHuskyClock
from .cwhardware.ChipWhispererHuskyMisc import XilinxDRP, XilinxMMCMDRP, LEDSettings, HuskyErrors, \
        USERIOSettings, XADCSettings, LASettings, ADS4128Settings
from ._OpenADCInterface import OpenADCInterface, HWInformation, GainSettings, TriggerSettings, ClockSettings
from ..api.cwcommon import ChipWhispererSAMErrors

try:
    from ..trace import TraceWhisperer
    from ..trace.TraceWhisperer import UARTTrigger
except Exception as e:
    tracewhisperer_logger.info("Could not import TraceWhisperer: {}".format(e))
    TraceWhisperer = None # type: ignore

from .cwhardware.ChipWhispererSAM3Update import SAMFWLoader
from .openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from ...common.utils import util
from ...common.utils.util import dict_to_str, DelayedKeyboardInterrupt
import time
import numpy as np
from ..api.cwcommon import ChipWhispererCommonInterface

from typing import List, Dict, Any

CODE_READ              = 0x80
CODE_WRITE             = 0xC0

class OpenADC(util.DisableNewAttr, ChipWhispererCommonInterface):
    DEFAULT_GAIN_DB = 25
    DEFAULT_ADC_SAMPLES = 5000
    DEFAULT_CLOCKGEN_FREQ = 7.37e6
    DEFAULT_ADC_MUL = 4

    """OpenADC scope object.

    This class contains the public API for the OpenADC hardware, including the
    ChipWhisperer Lite/ CW1200 Pro boards. It includes specific settings for
    each of these devices.

    To connect to one of these devices, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope(scope_type=cw.scopes.OpenADC)

    Some sane default settings are available via::

        scope.default_setup()

    This code will automatically detect an attached ChipWhisperer device and
    connect to it.

    For more help about scope settings, try help() on each of the ChipWhisperer
    scope submodules (scope.gain, scope.adc, scope.clock, scope.io,
    scope.trigger, and scope.glitch):

     *  :attr:`scope.gain <chipwhisperer.capture.scopes._OpenADCInterface.GainSettings>`
     *  :attr:`scope.adc <chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings>`
     *  :attr:`scope.clock <chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings>`
     *  :attr:`scope.io <chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings>`
     *  :attr:`scope.trigger <chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.TriggerSettings>`
     *  :attr:`scope.glitch (Lite/Pro) <chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings>`
     *  :meth:`scope.default_setup <.OpenADC.default_setup>`
     *  :meth:`scope.con <.OpenADC.con>`
     *  :meth:`scope.dis <.OpenADC.dis>`
     *  :meth:`scope.arm <.OpenADC.arm>`
     *  :meth:`scope.get_last_trace <.OpenADC.get_last_trace>`
     *  :meth:`scope.get_serial_ports <.ChipWhispererCommonInterface.get_serial_ports>`

    If you have a CW1200 ChipWhisperer Pro, you have access to some additional features:

     * :attr:`scope.SAD <chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD.ChipWhispererSAD>`
     * :attr:`scope.DecodeIO <chipwhisperer.capture.scopes.cwhardware.ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger>`
     * :attr:`scope.adc.stream_mode <chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.stream_mode>`

    If you have a CW-Husky, you have access to even more additional features:
     * :attr:`scope.SAD <chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD.HuskySAD>`
     * :attr:`scope.LA <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.LASettings>`
     * :attr:`scope.trace <chipwhisperer.capture.trace.TraceWhisperer>`
     * :attr:`scope.UARTTrigger <chipwhisperer.capture.trace.UARTTrigger>`
     * :attr:`scope.userio <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.USERIOSettings>`
     * :attr:`scope.errors <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.HuskyError>`
     * :attr:`scope.XADC <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.XADCSettings>`
     * :attr:`scope.ADS4128 <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.ADS4128Settings>`
     * :attr:`scope.LEDs <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.LEDSettings>`

    Inherits from :class:`chipwhisperer.capture.api.cwcommon.ChipWhispererCommonInterface`
    """

    _name = "ChipWhisperer/OpenADC"

    # TODO: This should reside in a base clock class if common code is refactored out of HW specific
    # classes.
    def try_wait_clkgen_locked(self, count, delay=0):
        """Tries to wait for clkgen to lock.

        Return:
            True if clkgen locked within the timeout, else False for a timeout.
        """
        while not self.clock.clkgen_locked:
            if count <= 0:
                return False
            self.clock.reset_dcms()
            count -= 1
            if delay:
                time.sleep(delay)
        return True

    def __init__(self):
        # self.qtadc = openadc_qt.OpenADCQt()
        # self
        super().__init__()
        self.enable_newattr()

        # Bonus Modules for ChipWhisperer
        self.advancedSettings = None
        self.advancedSAD = None
        self.digitalPattern = None

        self._is_connected = False
        self.data_points = []
        self._is_husky = False
        self._is_husky_plus = False

        # self.scopetype = OpenADCInterface_NAEUSBChip(self.qtadc)
        self.connectStatus = True
        # self.disable_newattr()

    def _getFWPy(self):
        cw_type = self._getCWType()
        if cw_type == "cwlite":
            from ...hardware.firmware.cwlite import fwver
        elif cw_type == "cw1200":
            from ...hardware.firmware.cw1200 import fwver # type: ignore
        elif cw_type in ["cwhusky", "cwhusky-plus"]:
            from ...hardware.firmware.cwhusky import fwver # type: ignore
        else:
            raise ValueError('Unknown cw_type: %s' % cw_type)
        return fwver

    def reload_fpga(self, bitstream=None, reconnect=True, prog_speed=1E6):
        """(Re)loads a FPGA bitstream (even if already configured).

        Will cause a reconnect event, all settings become default again.
        If no bitstream specified default is used based on current
        configuration settings.

        Args:
            bitstream (str or None): Path to new bitstream file. Optional, defaults to None
            reconnect (True): Whether or not to reconnect to the scope
            prog_speed (int): Speed at which to program the FPGA
        """
        self.scopetype.reload_fpga(bitstream, prog_speed=prog_speed)
        self.dis()
        self.con(self._saved_sn)

    def _getNAEUSB(self) -> NAEUSB:
        return self.scopetype.ser


    def enable_MPSSE(self, enable=True, husky_userio=None, scope_default_setup=True):
        """Enable/disable MPSSE mode. Results in a :code:`default_setup()` and scope disconnection

        Args:
            enable (bool): Enable or disable. Optional, defaults to True
            husky_userio (str or None): Enables communication using the Husky's user IO pins.
                If "jtag", route jtag over those pins. If "swd", route swd. If None, do not route.
                Optional, defaults to None
            scope_default_setup (bool): Calls `default_setup()` before enabling JTAG mode (resets clock,
                IOs, etc to default). Useful when working with standard targets, but set this to `False`
                if you had non-standard setup.
        """
        sn = self.sn
        if scope_default_setup:
            self.default_setup()
        if enable:
            self.io.cwe.setAVRISPMode(1)
        else:
            self.io.cwe.setAVRISPMode(0)

        if self._is_husky:
            if husky_userio:
                if husky_userio == "jtag":
                    self.userio.mode = "target_debug_jtag"
                elif husky_userio == "swd":
                    self.userio.mode = "target_debug_swd"
                else:
                    raise ValueError("Invalid husky userio mode: {}".format(husky_userio))
            self._getNAEUSB().set_husky_tms_wr(1)
        super().enable_MPSSE(enable)

        if enable and (not self._is_husky):
            # non husky needs to be setup after MPSSE is setup
            for i in range(10):
                time.sleep(0.50)
                try:
                    self.con(sn=sn)
                    break
                except:
                    pass
            try:
                self.default_setup()
            except Exception as exc:
                raise IOError("Could not reconnect to ChipWhisperer. \
                    Try connecting manually and running \
                        scope.default_setup(); scope.io.cwe.setAVRISPMode(1)") from exc
            self.io.cwe.setAVRISPMode(1)
            self.dis()

    def _get_usart(self) -> USART:
        return self.scopetype.usart
    
    def finish_mpsse_setup(self, set_defaults=True):
        if set_defaults:
            self.default_setup()
        self.io.cwe.setAVRISPMode(1)

    def _glitch_default(self, glitch_output):
        """Sets all the default glitch settings.
        """
        if self._is_husky:
            self.adc.disable_clip_and_lo_gain_errors(True)
        else:
            self.glitch.clk_src = 'clkgen'

        self.glitch.output = glitch_output
        self.glitch.trigger_src = 'ext_single'

        if self._is_husky:
            self.glitch.enabled = True
            time.sleep(0.1)
            self.glitch.clk_src = 'pll'

    def glitch_disable(self):
        """Disables glitch and glitch outputs
        """
        # Help extend mosfet lifespan and clear first
        self.io.vglitch_disable()

        if self._is_husky:
            self.glitch.enabled = False
            self.adc.disable_clip_and_lo_gain_errors(False)

        self.io.hs2 = 'clkgen'

    def cglitch_setup(self, default_setup=True):
        """Sets up sane defaults for clock glitching

        * glitch clk_src = clkgen
        * output = clock_xor
        * trigger_src = ext_single
        * hs2 = glitch
        * LP and HP glitch disabled
        """
        if default_setup:
            self.default_setup()

        self.io.vglitch_disable()
        self._glitch_default('clock_xor')
        self.io.hs2 = 'glitch'

    def vglitch_setup(self, glitcht, default_setup=True):
        """Sets up sane defaults for voltage glitch

        * glitch clk_src = clkgen
        * output = glitch_only
        * trigger_src = ext_single
        * hs2 = clkgen
        * LP glitch if glitcht = 'lp' or 'both'
        * HP glitch if glitcht = 'hp' or 'both'
        """
        if default_setup:
            self.default_setup()

        self.io.hs2 = 'clkgen'
        self._glitch_default('glitch_only')
        self.io.vcc_glitcht = glitcht

    def _recurse_scope_diff(self, string0, item0, string1, item1):
        if isinstance(item0, dict):
            for i,j in zip(item0.items(), item1.items()):
                self._recurse_scope_diff(string0 + '.' + i[0], i[1], string1 + '.' + j[0], j[1])
        else:
            if item0 != item1 and (('scope.XADC' not in string0) or (string0 == 'scope.XADC.status')):
                print('%-40s changed from %-25s to %-25s' % (string0, item0, item1))

    def scope_diff(self, scope_dict1, scope_dict2):
        """ Reports differences between two sets of scope settings.

        Args:
            scope_dict1, scope_dict2: dictionaries of scope settings (obtained
                with scope._dict_repr())

        """
        for a,b in zip(scope_dict1.items(), scope_dict2.items()):
            self._recurse_scope_diff('scope.' + a[0], a[1], 'scope.' + b[0], b[1])

    def _default_setup(self):
        """Sets all the default hardware configuration settings.
        """
        self.gain.db = self.DEFAULT_GAIN_DB
        self.adc.samples = self.DEFAULT_ADC_SAMPLES
        self.adc.offset = 0
        self.adc.basic_mode = 'rising_edge'
        self.clock.clkgen_freq = self.DEFAULT_CLOCKGEN_FREQ
        self.trigger.triggers = 'tio4'
        self.io.tio1 = self.io.GPIO_MODE_SERIAL_RX
        self.io.tio2 = self.io.GPIO_MODE_SERIAL_TX
        self.io.tio4 = self.io.GPIO_MODE_HIGHZ
        self.io.hs2 = 'clkgen'

        if self._is_husky:
            self.clock.clkgen_src = 'system'
            self.clock.clkgen_freq = self.DEFAULT_CLOCKGEN_FREQ
            self.clock.adc_mul = self.DEFAULT_ADC_MUL
        else:
            self.clock.adc_src = 'clkgen_x4'

    def default_setup(self, verbose=True):
        """Sets up sane capture defaults for this scope

         *  25dB gain
         *  5000 capture samples
         *  0 sample offset
         *  rising edge trigger
         *  7.37MHz clock output on hs2
         *  4*7.37MHz ADC clock
         *  tio1 = serial rx
         *  tio2 = serial tx
         *  tio4 = highZ
         *  CDC settings change off

        .. versionadded:: 5.1
            Added default setup for OpenADC
        """
        if verbose:
            scope_dict_pre = self._dict_repr()

        self._default_setup()
        self.io.cdc_settings = 0

        if self._is_husky:
            if not self.try_wait_clkgen_locked(10):
                raise OSError("Could not lock PLL. Try rerunning this function or calling scope.pll.reset(): {}".format(self))

            # these are the power-up defaults, but just in case e.g. test script left these on:
            self.adc.test_mode = False
            self.ADS4128.mode = 'normal'
            self.glitch.enabled = False
            self.LA.enabled = False
            self.userio.mode = 'normal'
            self.trace.capture.use_husky_arm = False
            self.trace.capture.trigger_source = 'firmware trigger'
            self.adc.segments = 1

        else:
            if not self.try_wait_clkgen_locked(5, 0.05):
                scope_logger.info("Could not lock clock for scope. This is typically safe to ignore. Reconnecting and retrying...")
                self.dis()
                time.sleep(0.25)
                self.con()
                time.sleep(0.25)
                self._default_setup()

                if not self.try_wait_clkgen_locked(5, 0.05):
                    raise OSError("Could not lock DCM. Try rerunning this function or calling scope.clock.reset_dcms(): {}".format(self))

        if verbose:
            scope_dict_post = self._dict_repr()
            self.scope_diff(scope_dict_pre, scope_dict_post)

    def dcmTimeout(self):
        if self._is_connected:
            try:
                self.sc.getStatus()
            except Exception as e:
                self.dis()
                raise e

    def getCurrentScope(self) -> OpenADCInterface_NAEUSBChip:
        return self.scopetype

    def setCurrentScope(self, scope : OpenADCInterface_NAEUSBChip):
        self.scopetype = scope

    def _getCWType(self) -> str:
        """Find out which type of ChipWhisperer this device is.

        Returns:
            One of the following:
             -  ""
             -  "cwlite"
             -  "cw1200"
             -  "cwrev2"
        """
        hwInfoVer = self.sc.hwInfo.versions()[2]
        if "ChipWhisperer" in hwInfoVer:
            if "Lite" in hwInfoVer:
                return "cwlite"
            elif "CW1200" in hwInfoVer:
                return "cw1200"
            elif "Husky-Plus" in hwInfoVer:
                return "cwhusky-plus"
            elif "Husky" in hwInfoVer:
                return "cwhusky"
            else:
                return "cwrev2"
        scope_logger.error("Unknown hwInfoVer: {}".format(hwInfoVer))
        return ""

    def get_name(self):
        """ Gets the name of the attached scope

        Returns:
            'ChipWhisperer Lite' if a Lite, 'ChipWhisperer Pro' if a Pro, 'ChipWhisperer Husky' if a Husky
        """
        name = self._getCWType()
        if name == "cwlite":
            return "ChipWhisperer Lite"
        elif name == "cw1200":
            return "ChipWhisperer Pro"
        elif name == "cwhusky":
            return "ChipWhisperer Husky"

    def adc_test(self, samples=131070, reps=3, verbose=False):
        """Run a series of ADC sampling tests on CW-Husky.

        Useful when pushing the ADC sampling frequency, to get an idea (but
        not a guarantee!) of whether Husky is able to sample properly at
        this frequency. Officially, Husky supports a maximum sampling clock
        of 200 MHz. In practice, sampling rates exceeding 300 MHz have been
        seen to work.

        Runs three different tests. For each test, we capture the sample
        test data and verify that it's what it should be:

        1. The internal test does not involve the ADC; it only verifies
        whether the FPGA sampling circuitry is functioning correctly, by
        generating a ramp pattern inside the FPGA itself.

        2. The ADC ramp test uses an ADC-generated ramp pattern which is
        then sampled by the FPGA.

        3. The ADC alternating test uses an ADC-generated alternating
        pattern (0x555 / 0xaaa). which is then sampled by the FPGA. The
        purpose of this test is that the ADC value changes every clock
        cycle, whereas in the ADC ramp test, the ADC value changes every *4*
        clock cycles.

        Note that this test does nothing to validate that the ADC's analog
        front-end is working properly!

        Args:
            samples (int): number of ADC samples per test.
            reps (int): number of times each test is run.
            verbose (bool)

        Returns:
            "pass" / "fail"

        .. versionadded:: 5.6.1

        :meta private:

        """

        if not self._is_husky:
            scope_logger.error("Only Husky supports scope.adc_test()")
            return
        # we're going to have to change some scope.adc settings, so save the
        # current values, to restore them later:
        saved_samples = self.adc.samples
        saved_stream_mode = self.adc.stream_mode
        saved_segments = self.adc.segments
        saved_bits_per_sample = self.adc.bits_per_sample
        saved_clip_errors_disabled = self.adc.clip_errors_disabled

        self.adc.samples = samples
        self.adc.stream_mode = False
        self.adc.segments = 1
        self.adc.bits_per_sample = 12
        self.adc.clip_errors_disabled = True
        mod=2**self.adc.bits_per_sample
        errors = 0
        first_error = None

        for i in range(reps):
            # 1. internal test (internally-generated ramp, ADC not involved)
            self.adc.test_mode = True
            self.ADS4128.mode = 'normal'
            self.sc.arm(False)
            self.arm()
            self.sc.triggerNow()
            self.sc.arm(False)
            assert self.capture() == False
            raw = self.get_last_trace(True)
            current_count = raw[0]
            for i, byte in enumerate(raw[1:]):
                if byte != (current_count+1)%mod:
                    if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%mod, byte))
                    errors += 1
                    if not first_error:
                        first_error = i
                    current_count = byte
                else:
                    current_count += 1
                    if (i+2) % samples == 0:
                        current_count = (current_count - samples) % mod
            if errors:
                scope_logger.error("%d errors in internal test. First error on sample #%d" % (errors, first_error))
                return "fail"

            # 2. ADC ramp test (ADC-generated ramp)
            self.ADS4128.mode = 'test ramp'
            self.adc.test_mode = False
            self.sc.arm(False)
            self.arm()
            self.sc.triggerNow()
            self.sc.arm(False)
            assert self.capture() == False
            raw = self.get_last_trace(True)
            current_count = raw[0]
            started = False
            for i, byte in enumerate(raw[1:]):
                if started:
                    if count4 < 3:
                        if byte != current_count:
                            if verbose: print("Byte %d: expected %d got %d" % (i, current_count, byte))
                            errors += 1
                            if not first_error:
                                first_error = i
                            started = False
                            current_count = byte
                        count4 += 1
                    else:
                        count4 = 0
                        if byte != (current_count+1)%mod:
                            if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%mod, byte))
                            errors += 1
                            if not first_error:
                                first_error = i
                        current_count = byte
                    if (i+2) % samples == 0:
                        current_count = (current_count - (samples)//4) % mod
                elif byte != current_count:
                    started = True
                    count4 = 0
                    current_count = byte
            if errors:
                scope_logger.error("%d errors in internal test. First error on sample #%d" % (errors, first_error))
                return "fail"

            # 3. alternating pattern test (ADC-generated)
            self.ADS4128.mode = 'test alternating'
            self.adc.test_mode = False
            self.sc.arm(False)
            self.arm()
            self.sc.triggerNow()
            self.sc.arm(False)
            assert self.capture() == False
            raw = self.get_last_trace(True)
            current_count = raw[0]
            for i, byte in enumerate(raw[1:]):
                if current_count == 0xaaa:
                    current_count = 0x555
                elif current_count == 0x555:
                    current_count = 0xaaa
                else:
                    errors += 1
                    if not first_error:
                        first_error = i
                    if verbose: print("Byte %d: unexpected value %0x" % current_count)
                if byte != current_count:
                    errors += 1
                    if not first_error:
                        first_error = i
                    if verbose: print("Byte %d: unexpected value %0x" % current_count)
            if errors:
                scope_logger.error("%d errors in internal test. First error on sample #%d" % (errors, first_error))
                return "fail"

        # restore previous settings:
        self.adc.samples = saved_samples
        self.adc.stream_mode = saved_stream_mode
        self.adc.segments = saved_segments
        self.adc.bits_per_sample = saved_bits_per_sample
        self.adc.clip_errors_disabled = saved_clip_errors_disabled
        self.adc.test_mode = False
        self.ADS4128.mode = 'normal'
        return "pass"


    @property
    def fpga_buildtime(self):
        """When the FPGA bitfile was generated. Husky only.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.sc.hwInfo.get_fpga_buildtime()

    def reset_fpga(self):
        """Reset Husky FPGA. This causes all FPGA-based settings to return to their default values.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        self.sc.reset_fpga()
        self.adc._clear_caches()
        self.sc._clear_caches()
        self.gain._clear_caches()
        self.ADS4128.set_defaults()

    def _get_fpga_programmer(self):
        return self.scopetype.fpga

    def con(self, sn=None, idProduct=None, bitstream=None, force=False, prog_speed=10E6, **kwargs):
        """Connects to attached chipwhisperer hardware (Lite, Pro, or Husky)

        Args:
            sn (str): The serial number of the attached device. Does not need to
                be specified unless there are multiple devices attached.
            idProduct (int): The product ID of the ChipWhisperer. If None, autodetects product ID. Optional.
            bitstream (str): Path to bitstream to program. If None, programs default bitstream. Optional.
            force (bool): Force reprogramming of bitstream. If False, only program bitstream if no bitstream
                is currently programmed. Optional.

        Returns:
            True if connection is successful, False otherwise

        .. versionchanged:: 5.5
            Added idProduct, bitstream, and force parameters.
        """
        self._read_only_attrs = []
        self._saved_sn = sn

        self.scopetype = OpenADCInterface_NAEUSBChip()

        self.scopetype.con(sn, idProduct, bitstream, force, prog_speed, **kwargs)
        self.sc = OpenADCInterface(self.scopetype.ser, self.scopetype.registers) # important to instantiate this before other FPGA components, since this does an FPGA reset
        self.hwinfo = HWInformation(self.sc)
        cwtype = self._getCWType()
        if cwtype in ["cwhusky", "cwhusky-plus"]:
            self.sc._is_husky = True
        self.sc._setReset(True)
        self.sc._setReset(False)

        self.adc = TriggerSettings(self.sc)
        self.gain = GainSettings(self.sc, self.adc)

        self.pll = None
        self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(cwtype, self.scopetype, self.sc)
        self.glitch_drp1 = None
        self.glitch_drp2 = None
        self.la_drp = None
        self.glitch_mmcm1 = None
        self.glitch_mmcm2 = None
        self.la_mmcm = None
        self.trace = None

        util.chipwhisperer_extra = self.advancedSettings

        if cwtype == "cw1200":
            self.SAD = ChipWhispererSAD.ChipWhispererSAD(self.sc)
            self.decode_IO = ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger(self.sc)

        if cwtype in ["cwhusky", "cwhusky-plus"]:
            # self.pll = ChipWhispererHuskyClock.CDCI6214(self.sc)
            self._fpga_clk = ClockSettings(self.sc, hwinfo=self.hwinfo, is_husky=True)
            self.glitch_drp1 = XilinxDRP(self.sc, "CG1_DRP_DATA", "CG1_DRP_ADDR", "CG1_DRP_RESET")
            self.glitch_drp2 = XilinxDRP(self.sc, "CG2_DRP_DATA", "CG2_DRP_ADDR", "CG2_DRP_RESET")
            self.la_drp = XilinxDRP(self.sc, "LA_DRP_DATA", "LA_DRP_ADDR", "LA_DRP_RESET")
            self.glitch_mmcm1 = XilinxMMCMDRP(self.glitch_drp1)
            self.glitch_mmcm2 = XilinxMMCMDRP(self.glitch_drp2)
            self.la_mmcm = XilinxMMCMDRP(self.la_drp)
            self.ADS4128 = ADS4128Settings(self.sc)
            self.clock = ChipWhispererHuskyClock.ChipWhispererHuskyClock(self.sc, \
                self._fpga_clk, self.glitch_mmcm1, self.glitch_mmcm2, self.ADS4128)
            self.XADC = XADCSettings(self.sc)
            self.LEDs = LEDSettings(self.sc)
            self.LA = LASettings(oaiface=self.sc, mmcm=self.la_mmcm, scope=self)
            if TraceWhisperer:
                try:
                    trace_reg_select = self.sc._address_str2int('TW_TRACE_REG_SELECT')
                    main_reg_select = self.sc._address_str2int('TW_MAIN_REG_SELECT')
                    self.trace = TraceWhisperer(husky=True, target=None, scope=self, trace_reg_select=trace_reg_select, main_reg_select=main_reg_select)
                    self.UARTTrigger = UARTTrigger(scope=self, trace_reg_select=3, main_reg_select=2)
                except Exception as e:
                    scope_logger.info("TraceWhisperer unavailable " + str(e))
            self.userio = USERIOSettings(self.sc, self.trace)
            self.SAD = ChipWhispererSAD.HuskySAD(self.sc)
            self.errors = HuskyErrors(self.sc, self.XADC, self.adc, self.clock, self.trace)
            self._is_husky = True
            self.adc._is_husky = True
            self.gain._is_husky = True
            self._fpga_clk._is_husky = True
            self.sc._is_husky = True
            self.adc.bits_per_sample = 12
            if cwtype == "cwhusky-plus":
                self._is_husky_plus = True
                self.LA._is_husky_plus = True
        else:
            self.clock = ClockSettings(self.sc, hwinfo=self.hwinfo)
            self.errors = ChipWhispererSAMErrors(self._getNAEUSB())

        if cwtype == "cw1200":
            self.adc._is_pro = True
        if cwtype == "cwlite":
            self.adc._is_lite = True
        if self.advancedSettings:
            self.io = self.advancedSettings.cwEXTRA.gpiomux
            self.trigger = self.advancedSettings.cwEXTRA.triggermux
            self.glitch = self.advancedSettings.glitch.glitchSettings
            if cwtype in ['cwhusky', 'cwhusky-plus']:
                # TODO: cleaner way to do this?
                self.glitch.pll = self.clock.pll
                self.clock.pll._glitch = self.glitch
                self.advancedSettings.glitch.pll = self.clock.pll
                self.trigger = self.advancedSettings.cwEXTRA.huskytrigger
            if cwtype == "cw1200":
                self.trigger = self.advancedSettings.cwEXTRA.protrigger

        if cwtype in ["cwhusky", "cwhusky-plus"]:
            # these are the power-up defaults, but just in case e.g. test script left these on:
            self.adc.test_mode = False
            self.ADS4128.mode = 'normal'
            self.glitch.enabled = False
            self.LA.enabled = False

        self._get_usart().init() # init serial port on connection

        module_list = [x for x in self.__dict__ if isinstance(self.__dict__[x], util.DisableNewAttr)]
        self.add_read_only(module_list)
        self.disable_newattr()
        self._is_connected = True
        self.connectStatus = True

        return True

    def dis(self):
        """Disconnects the current scope object.

        Returns:
            True if the disconnection was successful, False otherwise.
        """
        self._read_only_attrs = [] # disable read only stuff
        if self.scopetype is not None:
            self.scopetype.dis()
            if self.advancedSettings is not None:
                self.advancedSettings = None
                util.chipwhisperer_extra = None

            if self.advancedSAD is not None:
                self.advancedSAD = None

            if self.digitalPattern is not None:
                self.digitalPattern = None

        if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
            self.sc.usbcon = None

        self.enable_newattr()
        self._is_connected = False
        self.connectStatus = False
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
        if self._is_connected is False:
            raise OSError("Scope is not connected. Connect it first...")
        # with DelayedKeyboardInterrupt():
        try:
            self.advancedSettings.armPreScope()

            self.sc.arm()

            self.advancedSettings.armPostScope()

            # For Husky, scope.adc parameters must be cached before startCaptureThread turns on fast read mode,
            # because we won't be able to read them from the FPGA once fast read mode is turned on:
            if self._is_husky:
                self.adc._update_caches()

            self.sc.startCaptureThread()
        except Exception:
            self.dis()
            raise

    def _capture_read(self, num_points=None):
        if num_points is None:
            num_points = self.adc.samples
        scope_logger.debug("Expecting {} points".format(num_points))

        self.data_points = self.sc.readData(num_points)

        scope_logger.debug("Read {} datapoints".format(len(self.data_points)))
        if (self.data_points is None) or (len(self.data_points) != num_points):
            scope_logger.error("Received fewer points than expected: {} vs {}".format(len(self.data_points), num_points))
            return True
        return False


    def capture(self, poll_done : bool =False) -> bool:
        """Captures trace. Scope must be armed before capturing.

        Blocks until scope triggered (or times out),
        then disarms scope and copies data back.

        Read captured data out with :code:`scope.get_last_trace()`

        Args:
            poll_done: Supported by Husky only. Poll
                Husky to find out when it's done capturing, instead of
                calculating the capture time based on the capture parameters.
                Can result in slightly faster captures when the number of
                samples is high. Defaults to False.
        Returns:
           True if capture timed out, false if it didn't.

        Raises:
           IOError: Unknown failure.

        .. versionchanged:: 5.6.1
            Added poll_done parameter for Husky

        """
        if self._is_husky and self.adc.segments > 1 and self.adc.presamples and self.adc.samples % 3:
            raise ValueError('When using segments with presamples, the number of samples per segment (scope.adc.samples) must be a multiple of 3.')

        if self._is_husky and (self.adc.decimate > 1) and (self.adc.presamples or self.adc.segments > 1):
            raise ValueError('When decimate (%d) is used, presamples or segments cannot be used.' % self.adc.decimate)

        if self._is_husky and (self.adc.segments > 1) and (self.adc.samples * self.adc.segments > self.adc.oa.hwMaxSegmentSamples) and (not self.adc.stream_mode):
            raise ValueError('When using segments and stream mode is disabled, the maximum total number of samples is %d.' % self.adc.oa.hwMaxSegmentSamples)

        if self._is_husky and (self.adc.samples - self.adc.presamples < 2):
            raise ValueError('The number of samples (%d) must be at least 2 more than the number of presamples (%d).' % (self.adc.samples, self.adc.presamples))

        if self.adc.stream_mode and (not self._is_husky):
            a = self.sc.capture(None)
        else:
            a = self.sc.capture(self.adc.offset, self.clock.adc_freq, self.adc.samples, self.adc.segments, self.adc.segment_cycles, poll_done)

        # _capture_read() must be given the total number of samples to read; in the case of Husky, self.adc.samples
        # is the number of samples *per segment*, so adjust accordingly:
        if self._is_husky:
            samples = self.adc.samples * self.adc.segments
        else:
            samples = self.adc.samples
        b = self._capture_read(samples)
        return a or b

    def get_last_trace(self, as_int : bool=False) -> np.ndarray:
        """Return the last trace captured with this scope.

        Can return traces as floating point values (:code:`as_int=False`)
        or as integers.

        Floating point values are scaled and shifted to be between -0.5 and 0.5.

        Integer values are raw readings from the ChipWhisperer ADC. The ChipWhisperer-Lite
        has a 10-bit ADC, the Nano has an 8-bit ADC, and the Husky can read either
        8-bits or 12-bits of ADC data.

        Args:
            as_int: If False, return trace as a float. Otherwise, return as an int.

        Returns:
           Numpy array of the last capture trace.

        .. versionchanged:: 5.6.1
            Added as_int parameter
        """
        if as_int:
            if self._is_husky:
                # for Husky this is always appropriately sized (also there would be # of segments to consider)
                return self.sc._int_data
            else:
                return self.sc._int_data[:self.adc.samples]
        return self.data_points

    getLastTrace = util.camel_case_deprecated(get_last_trace)

    def capture_segmented(self):
        """Captures trace in segment mode, returns as many segments as buffer holds.

        Timeouts not handled yet properly (function will lock). Be sure you are generating
        enough triggers for segmented mode.

        Returns:
           True if capture timed out, false if it didn't.

        Raises:
           IOError: Unknown failure.

        .. versionadded:: 5.5
            Added segmented capture (requires custom bitstream)
        """

        if self.adc.fifo_fill_mode != "segment":
            raise IOError("ADC is not in 'segment' mode - aborting.")

        if self._is_husky:
            scope_logger.warning("Not intended for Husky -- just use a regular capture.")

        with DelayedKeyboardInterrupt():
            max_fifo_size = self.adc.oa.hwMaxSamples
            #self.adc.offset should maybe be ignored - passing for now but untested
            timeout = self.sc.capture(self.adc.offset, self.clock.adc_freq, max_fifo_size)
            timeout2 = self._capture_read(max_fifo_size-256)

            return timeout or timeout2

    def get_last_trace_segmented(self):
        """Return last trace assuming it was captued with segmented mode.

        NOTE: The length of each returned trace is 1 less sample than requested.

        Returns:
            2-D numpy array of the last captured traces.

        .. versionadded:: 5.5
            Added segmented capture (requires custom bitstream)
        """

        seg_len = self.adc.samples-1
        num_seg = int(len(self.data_points) / seg_len)

        return np.reshape(self.data_points[:num_seg*seg_len], (num_seg, seg_len))

    def _dict_repr(self) -> dict:
        rtn : Dict[str, Any] = {}
        rtn['sn'] = self.sn
        if self._is_husky:
            rtn['fpga_buildtime'] = self.fpga_buildtime
        rtn['fw_version'] = self.fw_version
        rtn['gain']    = self.gain._dict_repr()
        rtn['adc']     = self.adc._dict_repr()
        rtn['clock']   = self.clock._dict_repr()
        rtn['trigger'] = self.trigger._dict_repr()
        rtn['io']      = self.io._dict_repr()
        rtn['glitch']  = self.glitch._dict_repr()
        if self._getCWType() == "cw1200":
            rtn['SAD'] = self.SAD._dict_repr()
            rtn['decode_IO'] = self.decode_IO._dict_repr()
        if self._is_husky:
            rtn['SAD'] = self.SAD._dict_repr()
            rtn['ADS4128'] = self.ADS4128._dict_repr()
            if self.LA.present:
                rtn['LA'] = self.LA._dict_repr()
            if self.trace and self.trace.present:
                rtn['trace'] = self.trace._dict_repr()
            rtn['XADC'] = self.XADC._dict_repr()
            rtn['userio'] = self.userio._dict_repr()
            rtn['LEDs'] = self.LEDs._dict_repr()
            rtn['errors'] = self.errors._dict_repr()

        return rtn

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

    # def upgrade_firmware(self, fw_path=None):
    #     """Attempt a firmware upgrade. See https://chipwhisperer.readthedocs.io/en/latest/firmware.html for more information.

    #     .. versionadded:: 5.6.1
    #         Improved programming interface
    #     """
    #     prog = SAMFWLoader(self)
    #     prog.auto_program(fw_path)

    def fpga_reg_read(self, addr, numbytes):
        """Convenience method to read an FPGA register. Intended for debug/development.
        Args:
            addr (int): FPGA address to read.
            numbytes (int): number of bytes to read.

        Returns:
            read result: list of <numbytes> bytes.

        .. versionadded:: 5.6.1

        :meta private:

        """
        return list(self.sc.sendMessage(CODE_READ, addr, maxResp=numbytes))

    def fpga_reg_write(self, addr, listofbytes):
        """Convenience method to write an FPGA register. Intended for debug/development.
        Args:
            addr (int): FPGA address to write.
            listofbytes (int array): list of bytes to write.

        .. versionadded:: 5.6.1

        :meta private:

        """
        return self.sc.sendMessage(CODE_WRITE, addr, listofbytes)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.dis()
