# from _typeshed import OpenBinaryMode
from multiprocessing.sharedctypes import Value
from ....common.utils.util import dict_to_str, DelayedKeyboardInterrupt
from ....common.utils import util
from ....logging import *
import numpy as np
from .._OpenADCInterface import OpenADCInterface, ClockSettings
from ..cwhardware.ChipWhispererHuskyMisc import ADS4128Settings

import time

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

class CDCI6214:
    """PLL control for the Husky.

    May be merged into scope.clock in the future.

    Basic usage::

        scope = cw.scope() # Husky only
        scope.pll.target_freq = 7.37E6
        scope.pll.adc_mul = 4 # any positive integer within reason that satisfies ADC specs
    """
    def __init__(self, naeusb, mmcm1, mmcm2):
        self.naeusb = naeusb
        self._mmcm1 = mmcm1
        self._mmcm2 = mmcm2
        self._mmcm_muldiv = 0
        self._mmcm_vco_freq = 600e6
        self._mmcm_vco_min = 600e6
        self._mmcm_vco_max = 1200e6
        self.setup()
        self.set_pll_input()
        self.set_outdiv(3, 0)
        self.set_outdiv(1, 0)

        self._input_freq = 12E6 # 12MHz
        self._adc_mul = 4
        self._set_target_freq = 7.37E6
        self.set_prescale(3, 5)
        self.set_prescale(1, 5)
        self._fpga_clk_freq = 48E6
        self._glitch = None
        self._cached_adc_freq = None
        self._max_freq = 300e6
        self._warning_freq = 201e6

        self._old_in_freq = 0
        self._old_target_freq = 0
        self._allow0p5 = False
        self._reset_time = 0.10 # empirically seems to work well; this is a conservative number

    def write_reg(self, addr, data):
        """Write to a CDCI6214 Register over I2C

        Args:
            addr (u8): Address to write to
            data (u16 or list): Data to write. u16 is big endian, list is two elements long,
                so write_reg(0x00, [0x10, 0x20]) is the same as write_reg(0x00, 0x2010)

        data can be a 16-bit integer or a 2 element list
        """
        if not hasattr(data, "__getitem__"):
            tmp = [data & 0xFF, (data >> 8) & 0xFF]
            data = tmp

        self.naeusb.sendCtrl(0x29, data=[1, addr, 0x00, data[0], data[1]])

    def read_reg(self, addr, as_int=False):
        """Read a CDCI6214 Register over I2C

        Args:
            addr (u8): Address to read from
            as_int (bool): If true, return a big endian u16. Otherwise, return a two element list.
        """

        self.naeusb.sendCtrl(0x29, data=[0, addr, 0x00, 0, 0])
        data = self.naeusb.readCtrl(0x29, dlen=3)

        if data[0] != 2:
            raise IOError("PLL/I2C Error, got {}".format(data))

        if as_int is True:
            return (data[1]) | (data[2] << 8)
        return bytearray(data[1:])

    def update_reg(self, addr, bits_to_set, bits_to_clear):
        """Updates a CDCI6214 Register. Reads, clears bits, then sets bits.

        This means bits_to_set will overwrite bits_to_clear. Effectively::

            x = read_reg(addr)
            x &= ~(bits_to_clear) # C bitwise not, illustration purposes only, not implemented like this
            x |= bits_to_set
            write_reg(addr, x)

        Args:
            addr (u8): Address to update
            bits_to_set (u16 or list): Bits to set in the register, Overwrites bits_to_clear.
                            Big endian: 0x2010 == [0x10, 0x20]
            bits_to_clear (u16 or list): Bits to set in the register
                            Big endian: 0x2010 == [0x10, 0x20]


        bits_to_set/bits_to_clear can be a 16-bit integer or a 2 element list.
        The clear is applied first, so you can clear a register with 0xffff to set
        everything after.
        """
        # print(bits_to_set, bits_to_clear)

        if not hasattr(bits_to_set, "__getitem__"):
            tmp = [bits_to_set & 0xFF, (bits_to_set >> 8) & 0xFF]
            bits_to_set = tmp

        if not hasattr(bits_to_clear, "__getitem__"):
            tmp = [bits_to_clear & 0xFF, (bits_to_clear >> 8) & 0xFF]
            bits_to_clear = tmp

        # print(bits_to_set, bits_to_clear)
        reg_val = self.read_reg(addr, as_int=False)
        reg_val[0] &= 0xFF - bits_to_clear[0] # the not we want ;)
        reg_val[1] &= 0xFF - bits_to_clear[1]

        reg_val[0] |= bits_to_set[0]
        reg_val[1] |= bits_to_set[1]

        # print("Writing {} to addr {:02X} pll".format(reg_val, addr))
        self.write_reg(addr, reg_val)


    def setup(self):
        """Do required initial setup.

        Does the following:

         * Sets GPIO1 and 4 to outputs
         * Enables power to whole chip
         * Enable sync
         * Disable glitchless output
         * Disable channel 2 and 4 (unused)
         * Set ref as AC-differential, XIN == xtal
         * Use register to select PLL input instead of pin
        """
        # disable GPIO1/4 as inputs
        self.update_reg(0x00, (1 << 13) | (1 << 12), 0)

        self.update_reg(0x04, (1 << 3) | (1 << 4), 0) # turn off outputs 2 and 4

        self.update_reg(0x05, 0, 0b11111110111) # turn on power to everything

        # enable SYNC on channel 3
        self.update_reg(0x32, 0b1, 1 << 10)

        # enable SYNC on channel 1
        self.update_reg(0x26, 0b1, 1 << 10)

        # disable glitchless on channel 3
        self.update_reg(0x33, 1, 1)

        # disable glitchless on channel 1
        self.update_reg(0x27, 1, 1)

        # Disable channel 2: mute=1, outbuf=off
        self.update_reg(0x2C, (1<<7), (0x7<<2))

        # Disable channel 4: mute-1, outbuf=off
        self.update_reg(0x38, (1<<7), (0x7<<2))

        # Set ref input as AC-differential, XIN to xtal
        self.update_reg(0x1A, (1 << 15) | (0x0B << 8), (1 << 15) | 0b11 | (0xFF << 8))

        # use ref_mux_src bit to select input
        self.update_reg(0x01, 1 << 9, 0b11 << 8)

        # set GPIO1 output to PLL_LOCK
        self.update_reg(0x02, 0, 0b1111)

    def get_pll_input(self):
        """True if XTAL or False if FPGA
        """
        return (self.read_reg(0x01, True) & (1 << 8) == (0))

    def get_outfreq(self, pll_out=3):
        """Get the output frequency of a PLL output.

        Recommended to use :code:`scope.pll.adc_freq` and
        :code:`scope.pll.target_freq` instead
        """
        prescale_lut = [4, 5, 6]
        if pll_out == 3:
            prescale_reg = (self.read_reg(0x1E, True) >> 0) & 0b11
            prescale = prescale_lut[prescale_reg]

            outdiv = self.read_reg(0x31, True)
            #Bypass mode in upper bits
            if outdiv & 0xC000 == 0xC000:
                #Todo - return better value
                return "BYPASS"

            #Div held in lower bits
            outdiv = outdiv & 0x3FFF
            if outdiv == 0:
                return 0

            return self.input_freq / outdiv / prescale
        elif pll_out == 1:
            prescale_reg = (self.read_reg(0x1E, True) >> 2) & 0b11
            prescale = prescale_lut[prescale_reg]

            outdiv = (self.read_reg(0x25, True) & 0x3FFF)
            if outdiv == 0:
                return 0

            return self.input_freq / outdiv / prescale

    def reset(self):
        """Do a reset of the PLL chip. Doesn't reset registers.

        Maybe need to do to lock PLL?
        """
        self.update_reg(0x0, 1 << 2, 0x00)
        # wait enough time for PLL to re-lock (obtained empirically)
        time.sleep(self._reset_time)

    def sync_clocks(self):
        """Send a resync pulse to the internal synchronization blocks.

        Resync clocks.
        """
        self.update_reg(0x00, 1 << 5, 0x00)

    # def reset_pll_lock_detection(self):
    #     """NOTE: Does not relock PLL
    #     """
    #     self.update_reg(0x00, 1 << 6, 0x00)

    def recal(self):
        """Perform a calibration. Typically unneeded.
        """
        self.update_reg(0x0, 1 << 4, 0x00)

    def set_pll_input(self, xtal=True):
        """Set input to PLL and set input to 4MHz

        If xtal, use xtal, otherwise use FPGA clock
        """
        if xtal:
            # set input to xtal
            self.update_reg(0x01, 0, 1 << 8)

            # divide input clock by 3
            self.update_reg(0x1B, 3, 0xFF)
        else:
            # set input to ref (FPGA clock)
            self.update_reg(0x01, 1 << 8, 1 << 8)

            # divide input clock by 12
            self.update_reg(0x1B, 12, 0xFF)

        # set pll f_out = 5*125 fin (2.5GHz out)
        self.update_reg(0x1D, 125 | (1 << 14), 0xFFFF)

        # /4 on both prescaler A and B -> 650MHz out
        self.update_reg(0x1E, 0x00, 0x0F)

    def set_prescale(self, pll_out=3, prescale_val=4):
        """Set prescaler. Uses prescaler A for CH3 out, and prescaler B for CH1 out
        """
        bitshift = 0
        if pll_out == 3:
            bitshift = 0
        elif pll_out == 1:
            bitshift = 2

        if prescale_val == 4:
            self.update_reg(0x1E, 0x00, 0b11 << bitshift)
        elif prescale_val == 5:
            self.update_reg(0x1E, (1 << bitshift), 0b11 << bitshift)
        elif prescale_val == 6:
            self.update_reg(0x1E, (2 << bitshift), 0b11 << bitshift)

    def get_prescale(self, pll_out=3):
        reg = self.read_reg(0x1E, True)
        if pll_out == 3:
            bitshift = 0
        elif pll_out == 1:
            bitshift = 2

        prescales = [4, 5, 6]
        return prescales[(reg >> bitshift) & 0b11]

    def set_outdiv(self, pll_out=3, div=10):
        #todo, do one prescale for both output channels
        if div > 0x3FFF:
            raise ValueError("Div too big")
        if pll_out == 3:
            self.update_reg(0x31, div, 0xFFFF) # set div
            self.update_reg(0x32, (1) | (1 << 2), 0xFF) # LVDS CH3
        elif pll_out == 1:
            self.update_reg(0x25, div, 0xFFFF) # set div, prescaler A
        else:
            raise ValueError("pll_out must be 1 or 3, not {}".format(pll_out))

    def get_outdiv(self, pll_out=3):
        if pll_out == 3:
            return self.read_reg(0x31, True) & 0x3FFF
        elif pll_out == 1:
            return self.read_reg(0x25, True) & 0x3FFF
        return None

    def set_outfreqs(self, input_freq, target_freq, adc_mul, force_recalc=False):
        """Set an output target frequency for the target/adc using input_freq

        Calculates the best PLL/divider settings for a target_freq
        with an output div that can evenly divide adc_mul. Should
        help keep clocks in sync. Recommended to just set scope.pll.target_freq
        and scope.pll.adc_mul

        How the PLL works:

        1. We either get an input clock from the onboard xtal, or from
        the FPGA

        2. This input clock gets divided between 1 and 255 or multiplied by 2
        The resulting clock must be between 1MHz and 100MHz and is the input to
        the PLL

        3. The PLL input clock will be multiplied by a prescale value
        (4, 5, or 6, but we assume 5), then by between 5 and 2**14. The resulting
        clock must be between 2.4GHz and 2.8GHz

        4. The PLL output clock is then divided by a prescale value (we assume 5),
        then by an output division between 1 and 2**14. The resulting output clock
        must be below 200MHz for the ADC clock.

        To get the best output settings, we'll be calculating the output frequency
        and calculating its percent error. The settings that result in the
        lowest error will be used
        """

        # if the target clock is off, turn off both output clocks
        if target_freq == 0:
            # TODO: turn off clocks
            self.set_outdiv(3, 0)
            self.set_outdiv(1, 0)
            return

        # ADC mul must be either 0, or a positive integer
        adc_off = (adc_mul == 0)
        if (adc_mul < 1) or (adc_mul != int(adc_mul)):
            if not adc_off:
                raise ValueError("ADC must be >= 1 and an integer")

        scope_logger.debug("adc_mul: {}".format(adc_mul))

        # Adjust adc_mul if it results in an invalid adc clock
        old_mul = adc_mul
        if not adc_off:
            while (adc_mul * target_freq) > self._max_freq:
                adc_mul -= 1
            while (adc_mul * target_freq) < 1E6:
                adc_mul += 1
        else:
            # since the output div for the target freq
            # needs to be divisible by adc_mul,
            # setting it to 1 removes its effect
            adc_mul = 1

        if old_mul != adc_mul:
            if not adc_off:
                scope_logger.warning("ADC frequency must be between 1MHz and {}MHz - ADC mul has been adjusted to {}".format(self._max_freq, adc_mul))

        if adc_mul * target_freq > self._warning_freq:
            scope_logger.warning("""
                ADC frequency exceeds specification (200 MHz). 
                This may or may not work, depending on temperature, voltage, and luck.
                It may not work reliably.
                You can run scope.adc_test() to check whether ADC data is sampled properly by the FPGA,
                but this doesn't fully verify that the ADC is working properly.
                You can adjust scope.clock.pll._warning_freq if you don't want
                to see this message anymore.
                """)

        # If we're just changing ADC mul, try to avoid touching PLL settings
        # Depending on what frequencies we're dealing with, this may fail, meaning we have to touch the PLL settings
        # Then we need to reset the PLL to lock it, which drops the target clock for a bit
        # This often crashes the target, so the user may need to reset their target
        if (force_recalc is False) and ((input_freq == self._old_in_freq) and (target_freq == self._old_target_freq)):
            scope_logger.info("Input and target frequency unchanged, avoiding PLL changes so as not to drop out target clock")
            old_div = self.get_outdiv(3)
            
            # check if this results in a remainder
            # if it does, we need to recalc clocks
            if (old_div * self.adc_mul) % adc_mul:
                scope_logger.warning(f"Could not adjust adc_mul via output divider alone. Recalcing clocks...")
                scope_logger.warning("Target clock has dropped for a moment. You may need to reset your target")
            else:
                new_div = (old_div * self.adc_mul) // adc_mul
                scope_logger.debug(f"Newdiv {new_div}, OldDiv {old_div}, old adcmul {self.adc_mul}, new adcmul {adc_mul}")
                try:
                    if not self.pll_locked:
                        scope_logger.warning("PLL unlocked after updating frequencies")
                        scope_logger.warning("Target clock has dropped for a moment. You may need to reset your target")
                        self.reset()
                    self.set_outdiv(3, new_div)
                    self._adc_mul = adc_mul
                    return
                except:
                    scope_logger.warning("Could not change adc_mul with current settings, redoing PLL calculations")
                    scope_logger.warning("Target clock has dropped for a moment. You may need to reset your target")

        scope_logger.debug("adc_mul: {}".format(adc_mul))

        # find input divs that will give a clock
        # input to the PLL between 1MHz and 100MHz
        okay_in_divs = list(range(1,256))
        if self._allow0p5:
            # setting the reference divider to 0.5 seems to result in inconsistant phase relationship between the target clock and the
            # ADC sampling clock (the CDCI6214 datasheet hints at this in section 8.3.6); it's prevented by default:
            okay_in_divs.append(0.5)
        okay_in_divs = np.array(okay_in_divs, dtype='int64')
        okay_in_divs = okay_in_divs[(input_freq // okay_in_divs) >= 1E6]
        okay_in_divs = okay_in_divs[(input_freq // okay_in_divs) <= 100E6]
        scope_logger.debug("OK in divs: {}".format(okay_in_divs))

        pll_muls = np.arange(5, 2**14, dtype='int64')

        best_in_div = 0
        best_out_div = 0
        best_pll_mul = 0
        best_prescale = 0
        best_error = float('inf')

        # go through all valid input divisions
        for okay_in_div in okay_in_divs:
            pll_input = input_freq // okay_in_div

            # calculate all valid PLL multiples for the current input division
            okay_pll_muls = np.array(pll_muls, dtype='int64')
            okay_pll_muls = okay_pll_muls[((pll_input * 5 * okay_pll_muls) >= 2400E6)]
            okay_pll_muls = okay_pll_muls[((pll_input * 5 * okay_pll_muls) <= 2800E6)]

            # go through all the valid PLL multiples we calculated
            # and if we find better settings, update our best settings
            scope_logger.info("Ok PLL muls: {}".format(okay_pll_muls))
            for pll_mul in okay_pll_muls:
                for prescale in [4, 5, 6]:
                    output_input = pll_input * pll_mul * 5 // prescale
                    out_div = int((output_input / target_freq) + 0.5)
                    out_div -= out_div % adc_mul

                    real_target_freq = output_input / out_div
                    error = abs(target_freq - real_target_freq) / target_freq
                    scope_logger.debug("Testing settings: in_div {} out_div {} pll_mull {} prescale {} error {} freq {}".\
                        format(okay_in_div, out_div, pll_mul, prescale, error, real_target_freq))
                    if error < best_error:
                        best_in_div = okay_in_div
                        best_out_div = out_div
                        best_pll_mul = pll_mul
                        best_error = error
                        best_prescale = prescale
                        scope_logger.info("New best: in_div {} out_div {} pll_mull {} prescale {} error {} freq {}".\
                            format(best_in_div, best_out_div, best_pll_mul, best_prescale, best_error, real_target_freq))

        if best_error == float('inf'):
            raise ValueError("Could not calculate pll settings for input {}, output {} with mul {}".format(input_freq, target_freq, adc_mul))

        # set the output settings we found
        self.set_prescale(3, best_prescale)
        
        relock = False
        if self.get_input_div() != best_in_div:
            self.set_input_div(best_in_div)
            relock = True
        if self.get_pll_mul() != best_pll_mul:
            self.set_pll_mul(best_pll_mul)
            relock = True
        self.set_outdiv(1, best_out_div)

        if not adc_off:
            self.set_outdiv(3, best_out_div // adc_mul)
        else:
            # if the user wants the ADC clock off, turn it off
            self.set_outdiv(3, 0)

        # reset PLL (needed?)
        if (not self.pll_locked) or relock:
            self.reset()
        self.sync_clocks()

        self._old_in_freq = input_freq
        self._old_target_freq = target_freq
        self._adc_mul = adc_mul

    def set_bypass_adc(self, enable_bypass):
        """Routes FPGA clock input directly to ADC, bypasses PLL.
        """
        if enable_bypass:
            #fpga input
            self.pll_src = "fpga"

            #For output 3 (hard coded):

            # turn on bypass buffer for CH3
            self.update_reg(0x1B, (1<<13), 0)

            # Output divide by 1
            self.update_reg(0x31, 1, 0x3FFF)

            # Output source is REF
            self.update_reg(0x31, 0xC000, 0)
        else:

            # Output source is PSA
            self.update_reg(0x31, 0, 0xC000)

            # turn off bypass buffer for CH3
            self.update_reg(0x1B, 0, (1<<13))

    @property
    def target_delay(self):
        """Delays/phase shifts the target clock to the right (positive phase).
        Can only be used when pll_src is xtal.

        :getter: A 5 bit integer representing the delay

        :setter: A 5 bit integer representing the delay. Must be between 0 and 31
        """
        delay = (self.read_reg(0x26, True) >> 11) & 0b11111
        return delay

    @target_delay.setter
    def target_delay(self, delay):
        if self.pll_src == 'fpga':
            raise ValueError("Cannot set target_delay when scope.clock.clkgen_src is 'extclk'.")
        if (delay > 0b11111) or (delay < 0):
            raise ValueError("Invalid Delay {}, must be between 0 and 31".format(delay))

        self.update_reg(0x26, (delay << 11) | (1 << 10), 0b11111 << 11)
        self.reset() # the change doesn't take until a reset (or recal)

    @property
    def adc_delay(self):
        """Delays/phase shifts the target clock to the right (positive phase)

        :getter: A 5 bit integer representing the delay

        :setter: A 5 bit integer representing the delay. Must be between 0 and 31
        """
        delay = (self.read_reg(0x32, True) >> 11) & 0b11111
        return delay

    @adc_delay.setter
    def adc_delay(self, delay):
        if (delay > 0b11111) or (delay < 0):
            raise ValueError("Invalid Delay {}, must be between 0 and 31".format(delay))

        self.update_reg(0x32, (delay << 11) | (1 << 10), 0b11111 << 11)
        self.reset() # the change doesn't take until a reset (or recal)

    @property
    def pll_src(self):
        """Get/set the PLL src. fpga is typically useful if using an external clock

        :getter: 'xtal' or 'fpga'

        :setter: 'xtal' or 'fpga'
        """
        if self.get_pll_input():
            return "xtal"
        else:
            return "fpga"

    @pll_src.setter
    def pll_src(self, src):
        self._cached_adc_freq = None
        if src == "xtal":
            self.set_pll_input(True)
        elif src == "fpga":
            self.set_pll_input(False)
        else:
            raise ValueError("PLL src must be either 'xtal' or 'fpga'")
        ## update clocks
        self.set_outfreqs(self.input_freq, self._set_target_freq, self._adc_mul, True)

    @property
    def adc_mul(self):
        """The ADC clock output as a multiple of the target clock

        Must be an integer multiple.

        :getter: Last set multiplier

        :setter: Multiplier to set. Recalculates both adc and target clock settings,
            so setting this will result in a short disruption of the clock
            to the target
        """
        return self._adc_mul

    @adc_mul.setter
    def adc_mul(self, adc_mul):
        scope_logger.debug("adc_mul: {}".format(adc_mul))
        self.set_outfreqs(self.input_freq, self._set_target_freq, adc_mul)
        self._adc_mul = adc_mul

    @property
    def target_freq(self):
        """The target clock frequency.

        Due to PLL/adc_mul limitations, the actual value set will differ
        from the requested value

        :getter: The actual calculated target clock frequency that was set

        :setter: The target frequency you want
        """
        indiv = self.get_input_div()
        outdiv = self.get_outdiv(1)
        if not indiv:
            scope_logger.warning("Input divisor not set!")
            return None
        elif not outdiv:
            return 0
        else:
            return ((self.input_freq / indiv) * (self.get_pll_mul()) / outdiv) / (self.get_prescale(3)) * 5

    @target_freq.setter
    def target_freq(self, freq):
        self._cached_adc_freq = None
        self._set_target_freq = freq
        scope_logger.debug("adc_mul: {}".format(self._adc_mul))
        self.set_outfreqs(self.input_freq, self._set_target_freq, self._adc_mul)
        self.update_fpga_vco(self._mmcm_vco_freq)

    @property
    def adc_freq(self):
        """The actual calculated adc_clock freq. Read only
        """
        if True:
            indiv = self.get_input_div()
            outdiv = self.get_outdiv(3)
            if not indiv:
                scope_logger.warning("Input divisor not set!")
                return None
            elif not outdiv:
                return 0
            else:
                self._cached_adc_freq = ((self.input_freq / indiv) * (self.get_pll_mul()) / outdiv) / (self.get_prescale(3)) * 5

        return self._cached_adc_freq


    @property
    def input_freq(self):
        if self.pll_src == "xtal":
            return 12E6
        elif self.pll_src == "fpga":
            return self._fpga_clk_freq

    def set_input_div(self, div):
        okay_divs = [0.5]
        okay_divs.extend(range(1, 256))
        if div not in okay_divs:
            raise ValueError("Invalid input div {}".format(div))

        if div == 0.5:
            scope_logger.warning("""Setting reference divider to 0.5;
            this may result in inconsistent phase relationship between the target clock and the ADC sampling clock.
            This can be prevented by setting scope.clock.pll._allow0p5 to False.""")
            div = 0

        div = int(div)
        self.update_reg(0x1B, div, 0xFF)

    def get_input_div(self):
        div = self.read_reg(0x1B, True) & 0xFF
        if div == 0:
            div = 0.5
        return div

    def set_pll_mul(self, mul):
        okay_pll_muls = range(5, 2**14)
        if mul not in okay_pll_muls:
            raise ValueError("Invalid mul {}".format(mul))
        mul = int(mul)
        self.update_reg(0x1D, mul, 0x3FFF)

    def get_pll_mul(self):
        return self.read_reg(0x1D, True) & 0x3FFF


    def update_fpga_vco(self, vco):
        """Set the FPGA clock glitch PLL's VCO frequency.
        This isn't a property of the CDCI6214 PLL, but it is closely tied, because
        the FPGA VCO frequency depends of this PLL's frequency.
        Allowed range: 600 - 1200 MHz.
        """
        # For clock glitching, FPGA clock glitch MMCMs also need to have their M/D parameters
        # adjusted, in order to keep their VCO frequency in range.
        # Rules of the game:
        # 1. M and (secondary) D are always equal (output frequency = input frequency), and their range is [2, 64]
        # 2. main divider is always set to 1
        # 3. MMCM VCO range is [600, 1200] MHz (default: 600)
        if self.target_freq == 0:
            return
        if vco > self._mmcm_vco_max or vco < self._mmcm_vco_min:
            raise ValueError("Requested VCO out of range")

        # the following changes resets the glitch offset and width setting, but just resetting the internal (Python)
        # phase settings doesn't work as one would expect; resetting the actual FPGA MMCM phases is needed to get consistent
        # results (e.g. glitch shape is the same for a given offset/width after changing the VCO freq)
        if self._glitch.enabled and self._glitch.mmcm_locked:
            self._glitch.offset = 0
            self._glitch.width = 0

        muldiv = int(np.ceil(vco/self.target_freq))
        self._mmcm_vco_freq = vco
        if self.target_freq * muldiv > self._mmcm_vco_max:
            muldiv -= 1

        scope_logger.info("Setting vco {}, muldiv: {}".format(vco, muldiv))
        self._mmcm1.set_mul(muldiv)
        self._mmcm2.set_mul(muldiv)
        self._mmcm1.set_sec_div(muldiv)
        self._mmcm2.set_sec_div(muldiv)
        self._mmcm1.set_main_div(1)
        self._mmcm2.set_main_div(1)
        self._mmcm_muldiv = muldiv


    @property
    def pll_locked(self):
        """ Returns True if the pll is locked, False otherwise
        """
        return (self.read_reg(0x07, True) & (1 << 11)) == (1 << 11)

    def _dict_repr(self):
        rtn = {}
        rtn['pll_src'] = self.pll_src
        rtn['adc_freq'] = self.adc_freq
        rtn['target_freq'] = self.target_freq
        rtn['adc_mul'] = self.adc_mul
        rtn['pll_locked'] = self.pll_locked
        rtn['adc_delay'] = self.adc_delay
        rtn['target_delay'] = self.target_delay
        return rtn

    def __repr__(self):
        return dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

# class ChipWhispererHuskyClockAdv:
#     """todo"""
#     def __init__(self, pll, fpga_clk_settings):
#         self.pll = pll
#         self.fpga_clk_settings = fpga_clk_settings

class ChipWhispererHuskyClock(util.DisableNewAttr):

    def clear_adc_unlock(fn):
        """Use this to decorate methods that can cause the PLL to momentarily unlock. Clears
        the unlock status and then re-enables it. If PLL lock is regained, then the user will
        see the ADC LED turn on for a short time. If the PLL remains unlocked, then the ADC
        LED will turn on, flicker off, then turn back on and stay on.
        We do this because the ADC LED is sticky (i.e. stays on after an unlock event, even
        when the PLL re-locks, until manually cleared).
        """
        def inner(self, *args, **kwargs) :
            fn(self, *args, **kwargs)
            self._adc_error_enabled(False)
            self._adc_error_enabled(True)
        return inner

    def __init__(self, oaiface : OpenADCInterface, fpga_clk_settings : ClockSettings, mmcm1, mmcm2, adc: ADS4128Settings):
        super().__init__()

        # cache ADC freq to improve capture speed
        self._cached_adc_freq = None

        self.oa = oaiface
        self.naeusb = oaiface.serial
        self.pll = CDCI6214(self.naeusb, mmcm1, mmcm2)
        self.adc = adc
        self.fpga_clk_settings = fpga_clk_settings
        self.fpga_clk_settings.freq_ctr_src = "extclk"
        self.adc_phase = 0
        self._extclk_tolerance_cached = 100e3
        self._extclk_tolerance_enabled = True
        # self.adv_settings = ChipWhispererHuskyClockAdv(pll, fpga_clk_settings)
        self.disable_newattr()

    @property
    def clkgen_src(self):
        """The input for the Husky's PLL, which generates clocks
        for the target and the ADC.

        The PLL can receive input from two places:

        - "system" or "internal": An onboard crystal
        - "extclk": An external clock (e.g. generated by the target).

        When clkgen_src is set to "extclk", the external clock frequency is
        measured to set the ADC clock accordingly. If the external clock
        frequency is later modified, then clkgen_src must be re-set to "extclk"
        in order for the frequency change to be recognized. Otherwise, the ADC
        sampling clock will remain tied to the *previous* external clock
        frequency.

        A variant on "extclk" is "extclk_aux_io", when the external clock is
        supplied on the AUX I/O MCX instead of the HS1 pin (scope.io.aux_io_mcx
        must be set to "high_z" in this case).

        :Getter: Return the current PLL input ("system", "extclk" or "extclk_aux_io")

        :Setter: Change the CLKGEN source

        Raises:
            ValueError: if source is not one of the above


        """
        if self.pll.pll_src == "xtal":
            return "system"
        elif self.pll.pll_src == "fpga":
            data = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", maxResp=1)[0]
            if data & 0x03 == 0x03:
                return "extclk"
            elif data & 0x03 == 0x00:
                return "extclk_aux_io"
            else:
                raise ValueError("Unexpected value: %d" % data)

        raise ValueError("Invalid FPGA/PLL settings!") #TODO: print values

    @clkgen_src.setter # type: ignore
    @clear_adc_unlock
    def clkgen_src(self, clk_src):
        self._cached_adc_freq = None
        if clk_src in ["internal", "system"]:
            self.extclk_monitor_enabled = False
            clkgen_freq = self.clkgen_freq
            self.pll.pll_src = "xtal"
            self.fpga_clk_settings.enabled = False # keep things cool
            self.clkgen_freq = clkgen_freq
        elif clk_src in ["extclk", 'extclk_aux_io']:
            data = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", maxResp=1)[0]
            if clk_src == 'extclk':
                #set bits [2:0] to 011:
                data &= 0xf8
                data |= 0x03
            else:
                #set bits [2:0] to 000:
                data &= 0xf8
            self.oa.sendMessage(CODE_WRITE, "CW_EXTCLK_ADDR", [data])
            self.pll.pll_src = "fpga"
            self.fpga_clk_settings.adc_src = "extclk_dir"
            self.fpga_clk_settings.enabled = False # keep things cool
            self.fpga_clk_settings.freq_ctr_src = "extclk"
            self.clkgen_freq = self.fpga_clk_settings.freq_ctr
            self.extclk_monitor_enabled = True
        else:
            raise ValueError("Invalid src settings! Must be 'internal', 'system', 'extclk' or 'extclk_aux_io', not {}".format(clk_src))

    def _update_adc_speed_mode(self, mul, freq):
        """Husky's ADC has a high speed / low speed mode bit.
        When the ADC clock is changed, this automatically sets the appropriate
        speed mode.
        """
        if mul * freq < 80e6:
            self.adc.low_speed = True
        else:
            self.adc.low_speed = False

    @property
    def clkgen_freq(self):
        """The target clock frequency in Hz.

        The PLL takes the input clock frequency and multiplies it/divides to
        match as closely as possible to the set clkgen_freq. If set to 0,
        turns both the target and ADC clocks off.

        Some important notes for setting this value:

        * The minimum output frequency is 500kHz and the maximum is 350MHz
        * The ADC clock output frequency (clkgen_freq * adc_mul) must be
            below 200MHz. Therefore, if you want to use
            a clkgen_freq above 200MHz, you must set adc_mul=0
        * The accuracy of the actual clkgen_freq will depend
            on adc_mul, as the output divisor for the clkgen_freq must divide
            cleanly by adc_mul. For example, if you try to use a clkgen_freq
            of 7.37MHz and and adc_mul of 16, the closest valid clkgen_freq
            will be 7.5MHz.

        :Getter: Return the calculated target clock frequency in Hz

        :Setter: Attempt to set a new target clock frequency in Hz.
            This also blindly clears extclk_error if there is one, but it only
            assumes, and does not verify, that the frequency has been updated
            to the correct value.

        """
        # update pll clk src
        if not (self.clkgen_src in ["internal", "system"]):
            self.pll._fpga_clk_freq = self.fpga_clk_settings.freq_ctr
        return self.pll.target_freq


    @clkgen_freq.setter # type: ignore
    @clear_adc_unlock
    def clkgen_freq(self, freq):
        # update pll clk src
        self._cached_adc_freq = None
        if not (self.clkgen_src in ["internal", "system"]):
            self.pll._fpga_clk_freq = self.fpga_clk_settings.freq_ctr
        if self.clkgen_src == "test":
            self.pll._fpga_clk_freq = freq
        self.pll.target_freq = freq
        self.extclk_error = None
        self._update_adc_speed_mode(self.adc_mul, freq)

    @property
    def adc_mul(self):
        """ Sets a new ADC clock frequency by multiplying this value by clkgen_freq

        Must be a positive integer, or 0. If 0, turns the ADC clock off.

        adc_freq = adc_mul * clkgen_freq

        Note that the value of adc_mul affects how closely clkgen_freq can be matched
        to the requested frequency. See clkgen_freq for more information.

        :Getter: The currently set adc multiplier

        :Setter: Set the adc multiplier
        """
        # self._cached_adc_freq = None
        return self.pll.adc_mul

    @adc_mul.setter # type: ignore
    @clear_adc_unlock
    def adc_mul(self, mul):
        self._cached_adc_freq = None
        self.pll.adc_mul = mul
        self._update_adc_speed_mode(mul, self.clkgen_freq)

    @property
    def adc_freq(self):
        """Calculates the ADC frequency based on clkgen_freq and adc_mul

        Read-only
        """
        if self._cached_adc_freq is None:
            self._cached_adc_freq = self.pll.adc_freq

        return self._cached_adc_freq

        # return self.pll.adc_freq

    @property
    def freq_ctr(self):
        """Reads the frequency of the external input clock
        """
        return self.fpga_clk_settings.freq_ctr

    @property
    def freq_ctr_src(self):
        """The current input to the frequency counter.

        There are two possible inputs to the frequency counter:
        - "clkgen": The CLKGEN DCM output
        - "extclk": The external input clock signal

        :Getter: Return the frequency counter input (one of the above strings)

        :Setter: Set the frequency counter source

        Raises:
           ValueError: if source is not "pll" or "extclk"
        """
        return self.fpga_clk_settings.freq_ctr_src

    @freq_ctr_src.setter
    def freq_ctr_src(self, src):
        self.fpga_clk_settings.freq_ctr_src = src

    @property
    def clkgen_locked(self):
        """Checks if the Husky PLL is locked"""
        return self.pll.pll_locked

    @property
    def adc_phase(self):
        """Changes the phase of the ADC clock relative to the target clock

        Positive values delay the ADC clock compared to the target clock
        and vice versa.

        Negative values are not possible when scope.clock.clkgen_src is
        'extclk'.

        Note: The actual phase is only a 6 bit signed value compared to
        a 9 bit signed value on the Lite/Pro. This is mapped onto
        the same [-255, 255] range, meaning not all phases
        between -255 and 255 are possible.

        :Getter: Gets the current adc_phase

        :Setter: Sets the adc_phase. Must be in the range [-255, 255]
        """
        return int((self.pll.adc_delay - self.pll.target_delay) * 255 / 31)

    @adc_phase.setter # type: ignore
    @clear_adc_unlock
    def adc_phase(self, phase):
        self._cached_adc_freq = None
        if abs(phase) > 255:
            raise ValueError("Max phase +/- 255")
        adj_phase = int((abs(phase) * 31 / 255) + 0.5)

        if phase >= 0:
            self.pll.adc_delay = adj_phase
            if self.clkgen_src == 'system':
                # can't set this otherwise:
                self.pll.target_delay = 0
        else:
            self.pll.target_delay = abs(adj_phase)
            self.pll.adc_delay = 0

    @clear_adc_unlock # type: ignore
    def reset_dcms(self):
        """Reset the lock on the Husky's PLL.
        """
        self.pll.reset()

    @property
    def extclk_monitor_enabled(self):
        """When enabled, any change in the external clock frequency input
        exceeding the amount set in self.extclk_tolerance will trigger an
        error.

        When using an external clock to drive ChipWhisperer (i.e.
        self.clkgen_src == 'extclk'), Husky must know the frequency of that
        clock (by setting scope.clock.clkgen_freq). This clock monitor is a
        convenience to flag when the frequency changes without Husky being
        informed of that change.

        :Getter: Whether the external clock monitor is enabled.

        :Setter: Enable/disable the external clock monitor.
        """
        return self._extclk_tolerance_enabled

    @extclk_monitor_enabled.setter
    def extclk_monitor_enabled(self, en):
        if en:
            self.extclk_tolerance = self._extclk_tolerance_cached
            self._extclk_tolerance_enabled = True
        else:
            self._extclk_tolerance_enabled = False
            self._extclk_tolerance_cached = self.extclk_tolerance
            self.oa.sendMessage(CODE_WRITE, "EXTCLK_MONITOR", [0,0,0,0])

    def _adc_error_enabled(self, en):
        """Enable or disable the front panel red LED labeled "ADC", which (when
        enabled) lights up when the PLL (CDCI6214) is not locked.
        This is not something users are intended to play with; it's used internally
        to mask PLL unlock events when making clock changes.
        """
        if en:
            raw = 0
        else:
            raw = 1
        self.oa.sendMessage(CODE_WRITE, "ADCFREQ_ADDR", [raw])

    @property
    def extclk_error(self):
        """When the external clock is used, a change in clock frequency
        exceeding extclk_error will flag an error. The purpose of this is to
        remind you that you need to set scope.clock.clkgen_freq to the
        frequency of your external clock.

        :Getter: Whether the external clock monitor has flagged an error.

        :Setter: Clear the error.
        """
        raw = self.oa.sendMessage(CODE_READ, "EXTCLK_MONITOR", maxResp=1)[0]
        if raw:
            return True
        else:
            return False

    @extclk_error.setter
    def extclk_error(self, val):
        if self.extclk_monitor_enabled:
            self.extclk_monitor_enabled = False
            self.extclk_monitor_enabled = True


    @property
    def extclk_tolerance(self):
        """Tolerance for external clock frequency change, measured in Hz. If
        the difference between consecutive measurements exceeds this, an error
        is flagged. Defaults to ~100 Hz.

        :Getter: Get the frequency change tolerance [Hz].

        :Setter: Set the frequency change tolerance [Hz].
        """
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))
        return self._extclk_tolerance_cached * samplefreq

    @extclk_tolerance.setter
    def extclk_tolerance(self, freq):
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))
        freq = int(freq/samplefreq)
        self._extclk_tolerance_cached = freq
        self.oa.sendMessage(CODE_WRITE, "EXTCLK_MONITOR", list(int.to_bytes(freq, length=4, byteorder='little')))


    def _dict_repr(self):
        my_dict = {}
        my_dict['clkgen_src'] = self.clkgen_src
        my_dict['clkgen_freq'] = self.clkgen_freq
        my_dict['adc_mul'] = self.adc_mul
        my_dict['adc_freq'] = self.adc_freq
        my_dict['freq_ctr'] = self.freq_ctr
        my_dict['freq_ctr_src'] = self.freq_ctr_src
        my_dict['clkgen_locked'] = self.clkgen_locked
        my_dict['adc_phase'] = self.adc_phase
        my_dict['extclk_monitor_enabled'] = self.extclk_monitor_enabled
        my_dict['extclk_error'] = self.extclk_error
        my_dict['extclk_tolerance'] = self.extclk_tolerance
        return my_dict

    def __repr__(self):
        return dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def adc_src(self):
        """Convenience function for backwards compatibility with how ADC clocks
        are set on CW-lite and CW-pro.

        The ADC can be clocked by one of five possible sources:

         * "clkgen_x1": CLKGEN output via DCM
         * "clkgen_x4": CLKGEN output via DCM with x4 clk multiplier
         * "extclk_x1": External clock input via DCM
         * "extclk_x4": External clock input via DCM with x4 clk multiplier
         * "extclk_dir": External clock input with no DCM

        :Getter: Return the current ADC clock source (one of five strings above)

        :Setter: Set the ADC clock source and reset the ADC DCM to lock it.

        Raises:
           ValueError: if string not in valid settings
        """
        return "For Husky, please use scope.clock.clkgen_src and scope.clock.adc_mul instead."


    @adc_src.setter
    def adc_src(self, src):
        scope_logger.warning("scope.clock.adc_src is provided for backwards compability, but scope.clock.clkgen_src and scope.clock.adc_mul should be used for Husky.")
        self._cached_adc_freq = None

        if src == "clkgen_x4":
            self.adc_mul = 4
            self.clkgen_src = 'system'
        elif src == "clkgen_x1":
            self.adc_mul = 1
            self.clkgen_src = 'system'
        elif src == "extclk_x4":
            self.adc_mul = 4
            self.clkgen_src = 'extclk'
        elif src == "extclk_x1":
            self.adc_mul = 1
            self.clkgen_src = 'extclk'
        elif src == "extclk_dir":
            self.pll.set_bypass_adc(True)
        else:
            raise ValueError("Invalid ADC source (possible values: 'clkgen_x4', 'clkgen_x1', 'extclk_x4', 'extclk_x1', 'extclk_dir'")


    @clear_adc_unlock # type: ignore
    def reset_adc(self):
        """Convenience function for backwards compatibility with how ADC clocks
        are managed on CW-lite and CW-pro.
        """
        self._cached_adc_freq = None
        self.pll.reset()

    @clear_adc_unlock # type: ignore
    def recal_pll(self):
        """Convenience function.
        """
        self.pll.recal()


    @property
    def adc_locked(self):
        """Convenience function for backwards compatibility with how ADC clocks
        are managed on CW-lite and CW-pro.
        """
        return self.pll.pll_locked

    @property
    def fpga_vco_freq(self):
        """Set the FPGA clock glitch PLL's VCO frequency.

        Affects :attr:`scope.glitch.phase_shift_steps <chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.phase_shift_steps>`

        Allowed range: 600 - 1200 MHz.

        :getter: Calculate vco from last set value [Hz]

        :setter: Set the vco frequency [Hz]

        Raises:
            ValueError: set vco out of valid range
        """
        muldiv = self.pll._mmcm_muldiv
        vco = self.pll.target_freq * muldiv
        return vco

    @fpga_vco_freq.setter
    def fpga_vco_freq(self, vco):
        """Set the FPGA clock glitch PLL's VCO frequency.

        Affects scope.glitch.phase_shift_steps

        Allowed range: 600 - 1200 MHz.

        :getter: Calculate vco from last set value [Hz]

        :setter: Set the vco frequency [Hz]

        Raises:
            ValueError: set vco out of valid range
        """
        vco = int(vco)
        self.pll.update_fpga_vco(vco)
