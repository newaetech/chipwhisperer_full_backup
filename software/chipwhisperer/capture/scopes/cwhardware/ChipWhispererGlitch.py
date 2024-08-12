#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2015, NewAE Technology Inc
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
from ....logging import *
import zipfile
import datetime
import math
from ....capture.scopes.cwhardware import PartialReconfiguration as pr
from ....common.utils import util

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m

class MultiGlitchList(list):
    """Class that behaves like a list, but can set individual elements using a getter/setter

    Useful so that we can do scope.glitch.ext_offset[1] = 5 with Husky multi-glitch
    """
    def __setitem__(self, *args, **kwargs):
        oldval = self._getter()
        oldval[args[0]] = args[1]
        self._setter(oldval)
        pass

    def __repr__(self):
        oldrepr = super().__repr__()
        return f"MultiGlitchList({oldrepr})"

    def __init__(self, *args, **kwargs):
        if "getter" not in kwargs:
            raise KeyError("MultiGlitchList requires a getter")
        if "setter" not in kwargs:
            raise KeyError("MultiGlitchList requires a setter")
        
        self._getter = kwargs.pop("getter")
        self._setter = kwargs.pop("setter")
        super().__init__(*args, **kwargs)
        

class GlitchSettings(util.DisableNewAttr):

    # Output modes, sorted by ID (FPGA value)
    _output_modes = [
        "clock_xor",
        "clock_or",
        "glitch_only",
        "clock_only",
        "enable_only"
    ]

    # Trigger types
    _glitch_triggers = [
        "manual",
        "ext_continuous",
        "continuous",
        "ext_single"
    ]

    def __init__(self, cwglitch):
        super().__init__()
        self.cwg = cwglitch
        self._is_husky = self.cwg.cwtype in ['cwhusky', 'cwhusky-plus']
        self.pll = None
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}

        if self._is_husky:
            rtn['enabled'] = self.enabled
            rtn['num_glitches'] = self.num_glitches
        rtn['clk_src'] = self.clk_src
        rtn['mmcm_locked'] = self.mmcm_locked
        rtn['width'] = self.width
        if not self._is_husky:
            rtn['width_fine'] = self.width_fine
        rtn['offset'] = self.offset
        if not self._is_husky:
            rtn['offset_fine'] = self.offset_fine
        rtn['trigger_src'] = self.trigger_src
        rtn['arm_timing'] = self.arm_timing
        rtn['ext_offset'] = self.ext_offset
        rtn['repeat'] = self.repeat
        rtn['output'] = self.output
        if self._is_husky:
            rtn['phase_shift_steps'] = self.phase_shift_steps
        
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def manual_trigger(self):
        """Manually trigger the glitch output.

        This trigger is most useful in Manual trigger mode, where this is the
        only way to cause a glitch.

        Note that for ChipWhisperer-Husky, this method will only cause a glitch
        in manual mode, while on the Lite/Pro, this method will always insert a glitch.
        """
        self.cwg.glitchManual()
        
    def manualTrigger(self):
        self.manual_trigger()

    def readStatus(self):
        """Read the status of the two glitch DCMs.

        Returns:
            A tuple with 4 elements::

             * phase1: Phase shift of DCM1 (N/A for Husky),
             * phase2: Phase shift of DCM2 (N/A for Husky),
             * lock1: Whether DCM1 is locked,
             * lock2: Whether DCM2 is locked
        """
        return self.cwg.getDCMStatus()

    def resetDCMs(self, keepPhase=True):
        """Reset the two glitch DCMs.

        This is automatically run after changing the glitch width or offset,
        so this function is typically not necessary.
        """
        self.cwg.resetDCMs(keepPhase)

    @property
    def clk_src(self):
        """The clock signal that the glitch DCM is using as input.

        This DCM can be clocked from three different sources:
         * "target": The HS1 clock from the target device (can also be AUX clock for Husky)
         * "clkgen": The CLKGEN DCM output (N/A for Husky)
         * "pll": Husky's on-board PLL clock (Husky only)

        :Getter:
           Return the clock signal currently in use

        :Setter:
           Change the glitch clock source

        Raises:
           ValueError: New value not one of "target", "clkgen" or "pll"
        """
        clk_val = self.cwg.glitchClkSource()
        if clk_val == self.cwg.CLKSOURCE0_BIT:
            return "target"
        elif clk_val == self.cwg.CLKSOURCE1_BIT:
            return "clkgen"
        elif clk_val == self.cwg.CLKSOURCE2_BIT:
            return "pll"
        else:
            raise ValueError("Received unexpected glitch module clock source %s" % (clk_val), clk_val)

    @clk_src.setter
    def clk_src(self, source):
        if source == "target":
            clk_val = self.cwg.CLKSOURCE0_BIT
        elif source == "clkgen":
            if self._is_husky:
                glitch_logger.warning("clkgen unsupported for Husky; using pll instead.")
                clk_val = self.cwg.CLKSOURCE2_BIT
            else:
                clk_val = self.cwg.CLKSOURCE1_BIT
        elif source == 'pll':
            if self._is_husky:
                clk_val = self.cwg.CLKSOURCE2_BIT
            else:
                glitch_logger.warning("pll is only for Husky; using clkgen instead.")
                clk_val = self.cwg.CLKSOURCE1_BIT
        else:
            raise ValueError("Can't set glitch clock source to %s; valid values: ('target', 'pll', 'clkgen')" % source)
        self.cwg.setGlitchClkSource(clk_val)

    @property
    def phase_shift_steps(self):
        """The number of phase shift steps per target clock period.
        Husky only.
        To change, modify clock.pll.update_fpga_vco()

        :Getter: Returns the number of steps.

        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.cwg.getPhaseShiftSteps()


    @property
    def enabled(self):
        """Husky only. Whether the Xilinx MMCMs used to generate glitches are
        powered on or not. 7-series MMCMs are power hungry and are estimated
        to consume half of the FPGA's power. If you run into temperature
        issues and don't require glitching, you can power down these MMCMs.

        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.cwg.getEnabled()

    @enabled.setter
    def enabled(self, enable):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        self.cwg.setEnabled(enable)


    @property
    def mmcm_locked(self):
        """Whether the Xilinx MMCMs (aka DCMs/PLLs) used to generate glitches
        are locked or not.

        """
        return self.cwg.getMMCMLocked()

    @property
    def num_glitches(self):
        """The number of glitch events to generate. CW-Husky only.

        Each glitch event uses the same offset and width settings. 
        Glitch event x uses repeat[x] and ext_offset[x].

        This parameter has no effect when scope.glitch.trigger_src is set to
        "manual" or "continuous".

        .. note:: Subsequent glitches are offset from the previous glitch.

        Raises:
           ValueError: number outside of [1, 32].
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.cwg.getNumGlitches()

    @num_glitches.setter
    def num_glitches(self, num):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        self.cwg.setNumGlitches(num)

    @property
    def actual_num_glitches(self):
        """The number of glitches that were generated during the previous
        glitch event (should equal scope.glitch.num_glitches; for debugging).
        CW-Husky only.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.cwg.getNumActualGlitches()

    @property
    def state(self):
        """Glitch FSM state. CW-Husky only. For debug.
        Writing any value resets the FSM to its idle state.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self.cwg.getState()

    @state.setter
    def state(self, ignored):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        self.cwg.resetState()

    @property
    def width(self):
        """The width of a single glitch pulse.
        
        For CW-Husky, width is expressed as the number of phase shift steps.
        Minimum width is obtained at 0. Maximum width is obtained at
        scope.glitch.phase_shift_steps/2. Negative values are allowed, but -x
        is equivalent to scope.glitch.phase_shift_steps-x. The setting rolls
        over (+x is equivalent to scope.glitch.phase_shift_steps+x). Run the
        notebook in jupyter/demos/husky_glitch.ipynb to visualize glitch
        settings.

        For other capture hardware (CW-lite, CW-pro), width is expressed as a
        percentage of one period. One pulse can range from -49.8% to roughly
        49.8% of a period. The system may not be reliable at 0%. Note that
        negative widths are allowed; these act as if they are positive widths
        on the other half of the clock cycle.

        :Getter: Return an int (Husky) or float (others) with the current
            glitch width.

        :Setter: Update the glitch pulse width. For CW-lite/pro, the value is
            adjusted to the closest possible glitch width.

        Raises:
           UserWarning: Width outside of [-49.8, 49.8]. The value is rounded
               to one of these. (CW-lite/pro only)
        """
        return self.cwg.getGlitchWidth()

    @width.setter
    def width(self, value):
        self.cwg.setGlitchWidth(value)

    @property
    def width_fine(self):
        """The fine adjustment value on the glitch width. N/A for Husky.

        This is a dimensionless number that makes small adjustments to the
        glitch pulses' width. Valid range is [-255, 255].

        .. warning:: This value is write-only. Reads will always return 0.

        :Getter: Returns 0

        :Setter: Update the glitch fine width

        Raises:
           TypeError: offset not an integer
           ValueError: offset is outside of [-255, 255]
        """
        return self.cwg.getGlitchWidthFine()

    @width_fine.setter
    def width_fine(self, value):
        if self._is_husky:
            glitch_logger.error("N/A for Husky")
        try:
            int_val = int(value)
        except ValueError as e:
            raise TypeError("Can't convert %s to integer" % value, value) from e

        if int_val < -255 or int_val > 255:
            raise ValueError("New fine width is outside range [-255, 255]")
        self.cwg.setGlitchWidthFine(int_val)

    @property
    def offset(self):
        """The offset from a rising clock edge to a glitch pulse rising edge.

        For CW-Husky, offset is expressed as the number of phase shift steps.
        Minimum offset is obtained at 0 (rising edge of glitch aligned with
        rising edge of glitch source clock). At
        scope.glitch.phase_shift_steps/2, the glitch rising edge is aligned
        with the glitch source clock falling edge. Negative values are
        allowed, but -x is equivalent to scope.glitch.phase_shift_steps-x. The
        setting rolls over (+x is equivalent to
        scope.glitch.phase_shift_steps+x). Run the notebook in
        jupyter/demos/husky_glitch.ipynb to visualize glitch settings.

        For other capture hardware (CW-lite, CW-pro), offset is expressed 
        as a percentage of one period.
        A pulse may begin anywhere from -49.8% to 49.8% away from a rising
        edge, allowing glitches to be swept over the entire clock cycle.

        .. warning:: very large negative offset <-45 may result in double glitches
            (CW-lite/pro only).

        :Getter: Return an int (Husky) or float (CW-lite/pro) with the current
            glitch offset.

        :Setter: Set the glitch offset. For CW-lite/pro, the new value is
            rounded to the nearest possible offset.

        Raises:
           UserWarning: value outside range [-50, 50] (value is rounded)
               (CW-lite/pro only)
        """
        return self.cwg.getGlitchOffset()

    @offset.setter
    def offset(self, value):
        self.cwg.setGlitchOffset(value)

    @property
    def offset_fine(self):
        """The fine adjustment value on the glitch offset. N/A for Husky.

        This is a dimensionless number that makes small adjustments to the
        glitch pulses' offset. Valid range is [-255, 255].

        .. warning:: This value is write-only. Reads will always return 0.

        :Getter: Returns 0

        :Setter: Update the glitch fine offset

        Raises:
           TypeError: if offset not an integer
           ValueError: if offset is outside of [-255, 255]
        """
        return self.cwg.getGlitchOffsetFine()

    @offset_fine.setter
    def offset_fine(self, value):
        if self._is_husky:
            glitch_logger.error("N/A for Husky")
        try:
            int_val = int(value)
        except ValueError as e:
            raise TypeError("Can't convert %s to integer" % value, value) from e

        if int_val < -255 or int_val > 255:
            raise ValueError("New fine offset is outside range [-255, 255]")
        self.cwg.setGlitchOffsetFine(int_val)

    @property
    def trigger_src(self):
        """The trigger signal for the glitch pulses.

        The glitch module can use four different types of triggers:
         * "continuous": Constantly trigger glitches. The following
            scope.glitch parameters have no bearing in this mode: ext_offset,
            repeat, num_glitches.
         * "manual": Only trigger glitches by calling :code:`manual_trigger()`. The
            following scope.glitch parameters have no bearing in this mode:
            ext_offset, num_glitches. In this mode, calling :code:`scope.arm()` will
            trigger a glitch as well.
         * "ext_single": Use the trigger module. Once the scope is armed, one
            set of glitch events is emitted when the trigger condition is
            satisfied. Subsequent trigger conditions are ignored unless the
            scope is re-armed.
         * "ext_continuous": Use the trigger module. A set of glitch events is
            emitted each time the trigger condition is satisfied, whether or
            not the scope is armed.

         .. warning:: calling :code:`scope.arm()` in manual gitch mode will cause a glitch to trigger.

        :Getter: Return the current trigger source.

        :Setter: Change the trigger source.

        Raises:
           ValueError: value not listed above.
        """
        trig_src = self.cwg.glitchTrigger()
        return self._glitch_triggers[trig_src]

    @trigger_src.setter
    def trigger_src(self, src):
        try:
            trig_idx = self._glitch_triggers.index(src)
        except ValueError as e:
            raise ValueError("Can't set glitch trigger to %s; valid values: %s" % (src, self._glitch_triggers), src) from e

        self.cwg.setGlitchTrigger(trig_idx)

    @property
    def arm_timing(self):
        """When to arm the glitch in single-shot mode.

        If the glitch module is in "ext_single" trigger mode, it must be armed
        when the scope is armed. There are two timings for this event:

         * "no_glitch": The glitch module is not armed. Gives a moderate speedup to capture.
         * "before_scope": The glitch module is armed first.
         * "after_scope": The scope is armed first. This is the default.

        This setting may be helpful if trigger events are happening very early.

        If the glitch module is not in external trigger single-shot mode, this
        setting has no effect.

        :Getter: Return the current arm timing ("before_scope" or "after_scope")

        :Setter: Change the arm timing

        Raises:
           ValueError: if value not listed above
        """
        timing = self.cwg.getArmTiming()
        if timing == 0:
            return "no_glitch"
        elif timing == 1:
            return "before_scope"
        elif timing == 2:
            return "after_scope"
        else:
            raise ValueError("Received unexpected glitch arm timing %s" % (timing), timing)

    @arm_timing.setter
    def arm_timing(self, value):
        if value == "no_glitch":
            int_val = 0
        elif value == "before_scope":
            int_val = 1
        elif value == "after_scope":
            int_val = 2
        else:
            raise ValueError("Can't set glitch arm timing to %s; valid values: ('no_glitch', 'before_scope', 'after_scope')" % value, value)

        self.cwg.setArmTiming(int_val)

    @property
    def ext_offset(self):
        """How long the glitch module waits between a trigger and a glitch.

        After the glitch module is triggered, it waits for a number of clock
        cycles before generating glitch pulses. This delay allows the glitch to
        be inserted at a precise moment during the target's execution to glitch
        specific instructions.

        For CW-Husky when scope.glitch.num_glitches > 1, this parameter is a
        list with scope.glitch.num_glitches elements, each element
        representing the ext_offset value for the corresponding glitch,
        relative to the previous glitch. If ext_offset[i] = j, glitch i will
        be issued 2+j cycles after the start of glitch i-1.

        For CW-Lite/Pro, scope.glitch.num_glitches is not supported so this is
        a simply an integer.

        Has no effect when trigger_src = 'manual' or 'continuous'.

        .. note::
            It is possible to get more precise offsets by clocking the
            glitch module faster than the target board.

        This offset must be in the range [0, 2**32).

        :Getter: Return the current external trigger offset(s). For CW-lite/pro
           or when num_glitches=1, this is an integer (for backwards
           compatibility).  Otherwise, it is a MultiGlitchList, which behaves as a list,
           but allows ext_offset[x] = y to set settings for glitch x.

        :Setter: Set the external trigger offset(s). Integer for CW-lite/pro,
           list of integers for Husky.

        Raises:
           TypeError: if offset not an integer, or list of integers for Husky
           ValueError: if any offset outside of range [0, 2**32)
        """
        return self.cwg.getTriggerOffset()

    @ext_offset.setter
    def ext_offset(self, offset):
        self.cwg.setTriggerOffset(offset)

    @property
    def repeat(self):
        """The number of glitch pulses to generate per trigger.

        When the glitch module is triggered, it produces a number of pulses
        that can be combined with the clock signal. This setting allows for
        the glitch module to produce stronger glitches (especially during
        voltage glitching).

        For CW-Husky when scope.glitch.num_glitches > 1, this parameter is a
        list with scope.glitch.num_glitches elements, each element
        representing the repeat value for the corresponding glitch. The
        maximum legal value for repeat[i] is ext_offset[i+1]+1. If an
        illegal value is specified, the glitch output may be held high for
        up to 8192 cycles.

        For CW-Lite/Pro, scope.glitch.num_glitches is not supported so this is
        a simply an integer.

        Has no effect when trigger_src = 'continuous'.

        Repeat counter must be in the range [1, 8192].

        :Getter: Return the current repeat value. For CW-lite/pro or when
           num_glitches=1, this is an integer (for backwards compatibility).
           Otherwise, it is a list of integers.

        :Setter: Set the repeat counter. Integer for CW-lite/pro, list of
           integers for Husky.

        Raises:
           TypeError: if value not an integer, or list of integers for Husky
           ValueError: if any value outside [1, 8192]
        """
        return self.cwg.getRepeat()

    @repeat.setter
    def repeat(self, value):
        self.cwg.setRepeat(value)

    @property
    def output(self):
        """The type of output produced by the glitch module.

        There are 5 ways that the glitch module can combine the clock with its
        glitch pulses:

         * "clock_only": Output only the original input clock.
         * "glitch_only": Output only the glitch pulses - do not use the clock.
         * "clock_or": Output is high if either the clock or glitch are high.
         * "clock_xor": Output is high if clock and glitch are different.
         * "enable_only": Output is high for glitch.repeat cycles.

        Some of these settings are only useful in certain scenarios:
         * Clock glitching: "clock_or" or "clock_xor"
         * Voltage glitching: "glitch_only" or "enable_only"

        :Getter: Return the current glitch output mode (one of above strings)

        :Setter: Change the glitch output mode.

        Raises:
           ValueError: if value not in above strings
        """
        output_mode = self.cwg.glitchType()
        return self._output_modes[output_mode]

    @output.setter
    def output(self, value):
        try:
            output_idx = self._output_modes.index(value)
        except ValueError as e:
            raise ValueError("Can't set glitch mode to %s; valid values: %s" % (value, self._output_modes), value) from e
        self.cwg.setGlitchType(output_idx)

class ChipWhispererGlitch(object):
    """
    Drives the Glitch Module inside the ChipWhisperer Capture Hardware Rev2, or can be used to drive this FPGA
     module inserted into other systems.
    """
    CLKSOURCE0_BIT = 0b00000000
    CLKSOURCE1_BIT = 0b00000001
    CLKSOURCE2_BIT = 0b00000010
    CLKSOURCE_MASK = 0b00000011
    _name= 'Glitch Module'

    def __init__(self, cwtype, scope, oa):

        # Setup FPGA partial configuration dataZ
        self.prCon = pr.PartialReconfigConnection()
        self.prEnabled = False
        self.oa = oa
        self.cwtype = cwtype
        if cwtype in ['cwhusky', 'cwhusky-plus']:
            self._is_husky = True
        else:
            self._is_husky = False
        self.pll = None

        # Set default glitch width/offset:
        if self._is_husky:
            self._timeout = 1
            self._width = 0
            self._offset = 0
        else:
            # Note that these are ints scaled by 256/100 (these will get set via the setOpenADC() call below)
            self._width = 26
            self._offset = 26

        # These ranges are updated during __init__: see below
        # (N/A for Husky)
        self._min_width = 0
        self._max_width = 100

        self._min_offset = 0
        self._max_offset = 100

        # Single-shot arm timing
        self._ssarm = 2

        self._num_glitches = 1
        self._repeat_bits = 13

        # Check if we've got partial reconfiguration stuff for this scope
        try:
            if cwtype == "cwrev2" or cwtype == "cwcrev2":
                settingprefix = "cwcrev2"
                partialbasename = "s6lx25"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype == "cwlite":
                settingprefix = "cwlite"
                partialbasename = "cwlite"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype == "cw1200":
                settingprefix = "cw1200"
                partialbasename = "cw1200"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype in ["cwhusky", "cwhusky-plus"]:
                settingprefix = "cwhusky"
                partialbasename = "cwhusky"
                self.glitchPR = None
            else:
                raise ValueError("Invalid ChipWhisperer Mode: %s" % cwtype)

            if not self.glitchPR:
                self.prEnabled = False

            elif scope.getFWConfig().loader._release_mode != "debug":
                if scope.getFWConfig().loader._release_mode == "builtin":
                    filelike = scope.getFWConfig().loader._bsBuiltinData
                    zfile = zipfile.ZipFile(filelike)
                elif scope.getFWConfig().loader._release_mode == "zipfile":
                    fileloc = scope.getFWConfig().loader._bsZipLoc
                    if zipfile.is_zipfile(fileloc):
                        zfile = zipfile.ZipFile(fileloc, "r")
                    else:
                        glitch_logger.warning('Partial Reconfiguration DISABLED: no zip-file for FPGA')
                        zfile = None
                else:
                    glitch_logger.warning('Partial Reconfiguration DISABLED: no PR data for FPGA')
                    zfile = None
                    raise ValueError("Unknown FPGA mode: %s"%scope.getFWConfig().loader._release_mode)

                if zfile:
                    self.glitchPR.load(zfile.open("%s-glitchwidth.p" % partialbasename))
                    self.glitchPR.load(zfile.open("%s-glitchoffset.p" % partialbasename))
                    self.prEnabled = True
                else:
                    self.prEnabled = False
            else:
                glitch_logger.warning('Partial Reconfiguration DISABLED: Debug bitstream mode')
                self.prEnabled = False

        # except IOError as e: # same as OSError
        #     glitch_logger.error(str(e))
        #     self.prEnabled = False
        except ValueError as e:
            glitch_logger.error(str(e))
            self.prEnabled = False
        except OSError as e:  # Also catches WindowsError
            glitch_logger.error(str(e))
            self.prEnabled = False

        if self.prEnabled:
            # Enable glitch width, check what we've got access to
            self._min_width = self.glitchPR.limitList[0][0] / 2.55
            self._max_width = self.glitchPR.limitList[0][1] / 2.55
            self._min_offset = self.glitchPR.limitList[1][0] / 2.55
            self._max_offset = self.glitchPR.limitList[1][1] / 2.55

        if not self._is_husky:
            self.setOpenADC(oa)
        self.glitchSettings = GlitchSettings(self)

    def setOpenADC(self, oa):
        self.oa = None
        if self.prEnabled:
            self.prCon.con(oa)

            # Check this is actually working
            if self.prCon.isPresent() is False:
                self.prEnabled = False
                glitch_logger.warning('Partial Reconfiguration block not detected, PR disabled')
                return

            # Reset FPGA back to defaults in case previous bitstreams loaded
            self.updatePartialReconfig()
        self.oa = oa

    def updatePartialReconfig(self, _=None):
        """
        Reads the values set via API calls & updates the hardware settings for partial reconfiguration. Checks that PR
        is enabled with self.prEnabled.
        """

        if not self.prEnabled:
            return

        if self._width == 0:
            glitch_logger.warning('Partial reconfiguration for width = 0 may not work')

        if self._offset == 0:
            glitch_logger.warning('Partial reconfiguration for offset = 0 may not work')

        bs = self.glitchPR.getPartialBitstream([self._width, self._offset])

        if self.prEnabled:
            self.prCon.program(bs)
            if self.oa is not None:
                self.resetDCMs()
                # print "Partial: %d %d" % (widthint, offsetint)

            self.updateGlitchReadBack()

    def updateGlitchReadBack(self, test=False):
        """Updates the readback register in the FPGA with glitch information, used for LCD update on CW1200 hardware."""

        #Don't write if PR disable by accident
        if self.oa is None:
            return

        width = self.getGlitchWidth()
        offset = self.getGlitchOffset()

        cmd = bytearray(8)

        #Integer downloads
        cmd[0] = self._offset & 0xff
        cmd[1] = (self._offset >> 8) & 0xff
        cmd[2] = self._width & 0xff
        cmd[3] = (self._width >> 8) & 0xff

        #Floating point info
        cmd[4] = int(offset) & 0xff
        cmd[5] = int(("%f"%offset).split(".")[1][0:2]) & 0xff

        cmd[6] = int(width) & 0xff
        cmd[7] = int(("%f"%width).split(".")[1][0:2]) & 0xff

        self.oa.sendMessage(CODE_WRITE, "GLITCH_RECONFIG_RB_ADDR", cmd, Validate=False)

    def getPhaseShiftSteps(self):
        """Husky only. Returns number of phase shift steps in one target pll
        cycle.  This is simply 56 times the pll glitch MMCM's multiplier,
        indepedent of the target clock frequency.  (ref: Xilinx UG472 v1.14,
        "Dynamic Phase Shift Interface in the MMCM")
        """
        return self.pll._mmcm_muldiv * 56

    def setEnabled(self, enable):
        if enable:
            val = [0]
        else:
            val = [1]
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_POWERDOWN", val, Validate=False)
        if self._is_husky and enable:
            self.resetDCMs(keepPhase=False)

    def getEnabled(self):
        raw = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_POWERDOWN", Validate=False, maxResp=1)[0]
        if raw == 1:
            return False
        elif raw == 0:
            return True
        else:
            raise ValueError("Unexpected: read %d" % raw)

    def setNumGlitches(self, num):
        if num < 1 or num > 32:
            raise ValueError("Allowed range: 1-32");
        self._num_glitches = num
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_NUM_GLITCHES", [num-1], Validate=False)

    def getNumGlitches(self):
        return self._num_glitches

    def getNumActualGlitches(self):
        return self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_NUM_GLITCHES", Validate=False, maxResp=1)[0]

    def getState(self):
        raw = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_MULTIPLE_STATE", Validate=False, maxResp=1)[0]
        if raw == 0:
            return 'idle'
        elif raw == 1:
            return 'wait'
        elif raw == 2:
            return 'next'
        elif raw == 3:
            return 'done'
        else:
            raise ValueError("Unexpected state value: %d" % raw)

    def resetState(self):
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_MULTIPLE_STATE", [1], Validate=False)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_MULTIPLE_STATE", [0], Validate=False)

    def getMMCMLocked(self):
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=6)
        if ((resp[4] & 0x80) == 0x80) and ((resp[5] & 0x01) == 0x01):
            return True
        else:
            return False

    def setGlitchWidth(self, width):
        if self._is_husky:
            if not (self.glitchSettings.enabled and self.glitchSettings.mmcm_locked):
                raise ValueError("Can't change glitch settings if not enabled and locked.")
            assert type(width) == int
            self._width = width
            current = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
            LSB = width & 0x00FF
            MSB = (width & 0xFF00) >> 8
            current[0] = LSB
            current[1] = MSB
            current[2] = current[2] | 0x02
            self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", current, Validate=False)
            # Large adjustments can take a while so it's important to check if done. It *is* possible to trigger a glitch, following an adjustment,
            # before the adjustment is complete!
            starttime = datetime.datetime.now()
            done = False
            while not done:
                diff = datetime.datetime.now() - starttime
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in phase adjustment. Increase self._timeout. This should not be necessary unless you make *huge* width jumps.')
                    break
                raw = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=5)
                done = (raw[4] >> 6) & 0x01

        else:
            if width < self._min_width or width > self._max_width:
                raise UserWarning("Can't use glitch width %s - rounding into [%s, %s]" % (width, self._min_width, self._max_width))
            if width > self._max_width:
                width = self._max_width
            if width < self._min_width:
                width = self._min_width
            self._width = int(round((width/100.) * 256.))
            self.updatePartialReconfig()

    def getGlitchWidth(self):
        if self._is_husky:
            return self._width
        else:
            return self._width * 100. / 256.

    def setGlitchOffset(self, offset):
        if self._is_husky:
            if not (self.glitchSettings.enabled and self.glitchSettings.mmcm_locked):
                raise ValueError("Can't change glitch settings if not enabled and locked.")
            assert type(offset) == int
            self._offset = offset
            current = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
            LSB = offset & 0x00FF
            MSB = (offset & 0xFF00) >> 8
            current[0] = LSB
            current[1] = MSB
            current[2] = current[2] | 0x01
            self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", current, Validate=False)
            # Large adjustments can take a while so it's important to check if done. It *is* possible to trigger a glitch, following an adjustment,
            # before the adjustment is complete!
            starttime = datetime.datetime.now()
            done = False
            while not done:
                diff = datetime.datetime.now() - starttime
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in phase adjustment. Increase self._timeout. This should not be necessary unless you make *huge* offset jumps.')
                    break
                raw = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=5)
                done = (raw[4] >> 5) & 0x01

        else:
            value = offset
            if value < self._min_offset or value > self._max_offset:
                raise UserWarning("Can't use glitch offset %s - rounding into [%s, %s]" % (value, self._min_offset, self._max_offset))
            if offset > self._max_offset:
                offset = self._max_offset
            if offset < self._min_offset:
                offset = self._min_offset
            self._offset = int(round((offset / 100.) * 256.))
            self.updatePartialReconfig()

    def getGlitchOffset(self):
        if self._is_husky:
            return self._offset
        else:
            return self._offset * 100. / 256.

    def setTriggerOffset(self, offsets):
        """Set offset between trigger event and glitch in clock cycles"""
        if type(offsets) != list:
            offsets = [offsets]
        if len(offsets) > 1 and not self._is_husky:
            raise ValueError("Only Husky supports multiple offsets.")
        raw  = 0
        for i,offset in enumerate(offsets):
            try:
                int_val = int(offset)
            except ValueError as e:
                raise TypeError("Can't convert %s to integer" % offset, offset) from e
            if int_val < 0 or int_val >= 2**32:
                raise ValueError("New trigger offset %d is outside range [0, 2**32)" % int_val)
            raw += int_val * 2**(32*i)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_OFFSET", list(int.to_bytes(raw, length=4*len(offsets), byteorder='little')))

    def readTriggerOffset(self):
        num_glitches = self.getNumGlitches()
        raw = int.from_bytes(self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_OFFSET", maxResp=4*num_glitches), byteorder='little')
        if num_glitches == 1:
            return raw
        else:
            offsets = []
            for i in range(num_glitches):
                offsets.append(raw & (2**32-1))
                raw = raw >> 32
            return offsets
        
    def getTriggerOffset(self):
        """Get offset between trigger event and glitch in clock cycles"""
        offsets = self.readTriggerOffset()
        if type(offsets) is int:
            return offsets
        else:
            return MultiGlitchList(offsets, setter=self.setTriggerOffset, getter=self.readTriggerOffset)


    def setGlitchOffsetFine(self, fine):
        """Set the fine glitch offset adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            glitch_logger.warning('Glitch Module not present?')
            return

        LSB = fine & 0x00FF
        MSB = (fine & 0x0100) >> 8

        current[0] = LSB #7..0
        current[1] = (current[1] & ~0x01) | MSB #15..8

        #Start adjust
        current[2] = current[2] | 0x04  # 23..16
        #assign clockglitch_settings_read[37] = phase1_done_reg;
        #assign clockglitch_settings_read[38] = phase2_done_reg;

        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", current, Validate=False)

    def getGlitchWidthFine(self):
        if self._is_husky:
            glitch_logger.error("N/A for Husky")
        return self.getDCMStatus()[1]

    def setGlitchWidthFine(self, fine):
        """Set the fine glitch width adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            glitch_logger.warning('Glitch Module not present?')
            return

        LSB = fine & 0x00FF
        MSB = (fine & 0x0100) >> 8

        current[1] = (current[1] & 0x01) | ((LSB & 0x7F) << 1)
        current[2] = (current[2] & ~0x03) | ((LSB >> 7) | (MSB << 1))

        #Start adjust
        current[2] = current[2] | 0x04  # 23..16
        #assign clockglitch_settings_read[37] = phase1_done_reg;
        # assign clockglitch_settings_read[38] = phase2_done_reg;
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", current, Validate=False)

    def getGlitchOffsetFine(self):
        if self._is_husky:
            glitch_logger.error("N/A for Husky")
        return self.getDCMStatus()[0]

    def getDCMStatus(self):
        current = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)

        glitch_offset_fine_loaded = current[2] >> 3
        glitch_offset_fine_loaded |= (current[3] & 0x0F) << 5
        glitch_offset_fine_loaded = SIGNEXT(glitch_offset_fine_loaded, 9)

        glitch_width_fine_loaded = (current[3] & 0xF0) >> 4
        glitch_width_fine_loaded |= (current[4] & 0x1F) << 4
        glitch_width_fine_loaded = SIGNEXT(glitch_width_fine_loaded, 9)

        dcm1Lock = False
        dcm2Lock = False

        if current[4] & 0x80:
            dcm1Lock = True

        if current[5] & 0x01:
            dcm2Lock = True

        return (glitch_offset_fine_loaded, glitch_width_fine_loaded, dcm1Lock, dcm2Lock)

    def resetDCMs(self, keepPhase=True):
        """ Reset the DCMs/MMCMs for the Glitch width & Glitch offset. 
        Husky: if keepPhase=True, the previous offset and width settings are automatically re-applied.
        Non-Husky: Required after doing a PR operation
        """
        reset = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        reset[5] |= (1<<1)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", reset, Validate=False)
        reset[5] &= ~(1<<1)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", reset, Validate=False)

        # TODO: should we be doing something with keepPhase if we're not Husky?

        if self._is_husky:
            if keepPhase:
                self.setGlitchOffset(self._offset)
                self.setGlitchWidth(self._width)
            else:
                self._offset = 0
                self._width = 0


    def checkLocked(self, _=None):
        """Check if the DCMs are locked and print results """

        stat = self.getDCMStatus()
        glitch_logger.info('DCM1: Phase %d, Locked %r' % (stat[0], stat[2]))
        glitch_logger.info('DCM2: Phase %d, Locked %r' % (stat[1], stat[3]))

    def setRepeat(self, repeats):
        """Set number of glitches to occur after a trigger"""
        if type(repeats) != list:
            repeats = [repeats]
        if len(repeats) > 1 and not self._is_husky:
            raise ValueError("Only Husky supports multiple offsets.")
        raw = 0
        for i,repeat in enumerate(repeats):
            try:
                int_val = int(repeat)
            except ValueError as e:
                raise TypeError("Can't convert %s to integer" % repeat, repeat) from e
            if int_val < 1 or int_val > 8192:
                raise ValueError("New repeat value %d is outside range [1, 8192]" % int_val)
            int_val = int_val-1
            if i == 0:
                resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
                if resp is None or len(resp) < 8:
                    glitch_logger.warning('Glitch Module not present?')
                    return
                resp[6] = int_val & 0xff #LSB        
                resp[7] = (resp[7] & self.CLKSOURCE_MASK) | ((int_val >> 8) << 2) #5-bit MSB stored in upper bits
                self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)
            else:
                raw += int_val * 2**(self._repeat_bits*(i-1))
        if len(repeats) > 1:
            bytes_to_write = math.ceil((len(repeats)-1)*self._repeat_bits/8)
            self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_REPEATS", list(int.to_bytes(raw, length=bytes_to_write, byteorder='little')))

    def readRepeat(self):
        num_glitches = self.getNumGlitches()
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        num = resp[6]
        num |= ((resp[7] & ~(self.CLKSOURCE_MASK)) >> 2) << 8
        num += 1
        if num_glitches == 1:
            return num
        else:
            repeats = [num]
            bytes_to_read = math.ceil((num_glitches-1)*self._repeat_bits/8)
            raw = int.from_bytes(self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_REPEATS", Validate=False, maxResp=bytes_to_read), byteorder='little')
            for i in range(1, num_glitches):
                repeats.append
                repeats.append((raw & (2**self._repeat_bits-1)) + 1)
                raw = raw >> self._repeat_bits
            return repeats
        

    def getRepeat(self):
        """Get number of glitches to occur after a trigger"""
        repeats = self.readRepeat()
        if type(repeats) is int:
            return repeats
        else:
            return MultiGlitchList(repeats, setter=self.setRepeat, getter=self.readRepeat)
            


    def setGlitchTrigger(self, trigger):
        """Set glitch trigger type (manual, continous, adc-trigger)"""
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        resp[5] = (resp[5] & ~(0x0C)) | (trigger << 2)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)

    def glitchTrigger(self):
        """Get glitch trigger type"""
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        return (resp[5] & 0x0C) >> 2

    def setGlitchType(self, t):
        """Set glitch output type (ORd with clock, XORd with clock, clock only, glitch only)"""
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        resp[5] = (resp[5] & ~(0x70)) | (t << 4)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)

    def glitchType(self):
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        return (resp[5] & 0x70) >> 4

    def glitchManual(self, _=None):
        """
        Cause a single glitch event to occur. Depending on setting of scope.glitch.repeat this may mean
        multiple glitches in a row
        """
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        resp[5] = resp[5] | (1 << 7)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)
        resp[5] = resp[5] & ~(1 << 7)
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)

    def glitchArm(self):
        """If trigger is set to single-shot mode, this must be called before the selected trigger occurs"""
        self.glitchManual()

    def setGlitchClkSource(self, source):
        """Set the source of the glitched clock, either the HS1-In or the CLKGEN Module"""
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        resp[7] = (resp[7] & ~self.CLKSOURCE_MASK) | source
        self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", resp, Validate=False)
        if self._is_husky:
            self.resetDCMs()

    def glitchClkSource(self):
        resp = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
        return (resp[7] & self.CLKSOURCE_MASK)

    def getArmTiming(self):
        return self._ssarm

    def setArmTiming(self, val):
        self._ssarm = val

    def armPreScope(self):
        """Called before scope trigger is armed"""
        if self.getArmTiming() == 1:
            self.glitchArm()

    def armPostScope(self):
        """Called after scope trigger is armed"""
        if self.getArmTiming() == 2:
            self.glitchArm()
