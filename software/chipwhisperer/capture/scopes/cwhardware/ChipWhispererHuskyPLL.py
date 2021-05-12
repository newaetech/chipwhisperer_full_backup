from chipwhisperer.common.utils.util import dict_to_str, DelayedKeyboardInterrupt
from chipwhisperer.logging import *
import time
import numpy as np


class CDCI6214:
    """PLL control for the Husky.

    May be merged into scope.clock in the future.

    Basic usage::

        scope = cw.scope() # Husky only
        scope.pll.target_freq = 7.37E6 
        scope.pll.adc_mul = 4 # any positive integer within reason that satisfies ADC specs
    """
    def __init__(self, scope):
        self._scope = scope
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

        self._scope._getNAEUSB().sendCtrl(0x29, data=[1, addr, 0x00, data[0], data[1]])
        
    def read_reg(self, addr, as_int=False):
        """Read a CDCI6214 Register over I2C

        Args:
            addr (u8): Address to read from
            as_int (bool): If true, return a big endian u16. Otherwise, return a two element list.
        """

        self._scope._getNAEUSB().sendCtrl(0x29, data=[0, addr, 0x00, 0, 0])    
        data = self._scope._getNAEUSB().readCtrl(0x29, dlen=3)

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

    def sync_clocks(self):
        """Send a resync pulse to the internal synchronization blocks.
        
        Resync clocks.
        """
        self.update_reg(0x00, 1 << 5, 0x00)
        
    def recal(self):
        """Perform a calibration. Typically unneeded.
        """
        self.update_reg(0x0, 1 << 5, 1 << 5)
        
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
        
    def set_outdiv(self, pll_out=3, div=10):
        #todo, do one prescale for both output channels
        if div > 0x3FFF:
            raise ValueError("Div too big")
        if pll_out == 3:
            self.update_reg(0x31, div, 0xFFFF) # set div
            self.update_reg(0x32, (1) | (1 << 2), 0xFF) # LVDS CH3
            self.reset()
        elif pll_out == 1:
            self.update_reg(0x25, div, 0xFFFF) # set div, prescaler A
            self.reset()
        else:
            raise ValueError("pll_out must be 1 or 3, not {}".format(pll_out))

    def get_outdiv(self, pll_out=3):
        if pll_out == 3:
            return self.read_reg(0x31, True) & 0x3FFF
        elif pll_out == 1:
            return self.read_reg(0x25, True) & 0x3FFF

    def set_outfreqs(self, input_freq, target_freq, adc_mul):
        """Set an output target frequency for the target/adc using input_freq

        Calculates the best PLL/divider settings for a target_freq
        with an output div that can evenly divide adc_mul. Should
        help keep clocks in sync. Recommended to just set scope.pll.target_freq
        and scope.pll.adc_mul
        """

        """
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
        must be between 10MHz and 200MHz for the ADC clock.

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
        if not adc_off:
            while (adc_mul * target_freq) > 200E6:
                adc_mul -= 1
            while (adc_mul * target_freq) < 10E6:
                adc_mul += 1
        else:
            # since the output div for the target freq
            # needs to be divisible by adc_mul,
            # setting it to 1 removes its effect
            adc_mul = 1


        scope_logger.debug("adc_mul: {}".format(adc_mul))

        # find input divs that will give a clock
        # input to the PLL between 1MHz and 100MHz
        okay_in_divs = [0.5]
        okay_in_divs.extend(range(1, 256))
        okay_in_divs = np.array(okay_in_divs)
        okay_in_divs = okay_in_divs[(input_freq // okay_in_divs) >= 1E6]
        okay_in_divs = okay_in_divs[(input_freq // okay_in_divs) <= 100E6]
        
        pll_muls = np.arange(5, 2**14)

        best_in_div = 0
        best_out_div = 0
        best_pll_mul = 0
        best_error = float('inf')
        
        # go through all valid input divisions
        for okay_in_div in okay_in_divs:
            pll_input = input_freq // okay_in_div

            # calculate all valid PLL multiples for the current input division
            okay_pll_muls = np.array(pll_muls)
            okay_pll_muls = okay_pll_muls[((pll_input * 5 * okay_pll_muls) >= 2400E6)]
            okay_pll_muls = okay_pll_muls[((pll_input * 5 * okay_pll_muls) <= 2800E6)]
            
            # go through all the valid PLL multiples we calculated
            # and if we find better settings, update our best settings
            for pll_mul in okay_pll_muls:
                output_input = pll_input * pll_mul
                out_div = int((output_input / target_freq) + 0.5)
                out_div -= out_div % adc_mul
                
                real_target_freq = output_input / out_div
                error = abs(target_freq - real_target_freq) / target_freq
                if error < best_error:
                    best_in_div = okay_in_div
                    best_out_div = out_div
                    best_pll_mul = pll_mul
                    best_error = error
                    
        if best_error == float('inf'):
            raise ValueError("Could not calculate pll settings for input {} with mul {}".format(target_freq, adc_mul))

        # set the output settings we found
        self.set_prescale(3, 5)
        self.set_input_div(best_in_div)
        self.set_pll_mul(best_pll_mul)
        self.set_outdiv(1, best_out_div)

        if not adc_off:
            self.set_outdiv(3, best_out_div // adc_mul)
        else:
            # if the user wants the ADC clock off, turn it off
            self.set_outdiv(3, 0)

        # reset PLL (needed?)
        self.reset()
            
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
        """Delays/phase shifts the target clock to the right (positive phase)

        :getter: A 5 bit integer representing the delay
        
        :setter: A 5 bit integer representing the delay. Must be between 0 and 31
        """
        delay = (self.read_reg(0x26, True) >> 11) & 0b11111
        return delay

    @target_delay.setter
    def target_delay(self, delay):
        if (delay > 0b11111) or (delay < 0):
            raise ValueError("Invalid Delay {}, must be between 0 and 31".format(delay))

        self.update_reg(0x26, (delay << 11) | (1 << 10), 0b11111 << 11)

    @property
    def adc_delay(self):
        """Delays/phase shifts the target clock to the right (positive phase)

        :getter: A 5 bit integer representing the delay
        
        :setter: A 5 bit integer representing the delay. Must be between 0 and 31
        """
        delay = (self.read_reg(0x32, True) >> 11) & 0b1111
        return delay

    @adc_delay.setter
    def adc_delay(self, delay):
        if (delay > 0b11111) or (delay < 0):
            raise ValueError("Invalid Delay {}, must be between 0 and 31".format(delay))

        self.update_reg(0x32, (delay << 11) | (1 << 10), 0b11111 << 11)
        
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
        if src == "xtal":
            self.set_pll_input(True)
        elif src == "fpga":
            self.set_pll_input(False)
        else:
            raise ValueError("Pll src must be either 'xtal' or 'fpga'")
        ## update clocks
        self.set_outfreqs(self.input_freq, self._set_target_freq, self._adc_mul)

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
            return ((self.input_freq / indiv) * (self.get_pll_mul()) / outdiv)

    @property
    def adc_freq(self):
        """The actual calculated adc_clock freq. Read only
        """
        indiv = self.get_input_div()
        outdiv = self.get_outdiv(3)
        if not indiv:
            scope_logger.warning("Input divisor not set!")
            return None
        elif not outdiv:
            return 0
        else:
            return ((self.input_freq / indiv) * (self.get_pll_mul()) / outdiv)


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

    @target_freq.setter
    def target_freq(self, freq):
        self._set_target_freq = freq
        scope_logger.debug("adc_mul: {}".format(self._adc_mul))
        self.set_outfreqs(self.input_freq, self._set_target_freq, self._adc_mul)

    @adc_mul.setter
    def adc_mul(self, adc_mul):
        self._adc_mul = adc_mul
        scope_logger.debug("adc_mul: {}".format(adc_mul))
        self.set_outfreqs(self.input_freq, self._set_target_freq, self._adc_mul)

    @property
    def pll_locked(self):
        """ Returns True if the pll is locked, False otherwise
        """
        return (self.read_reg(0x07, True) & (1 << 11)) == (1 << 11)

    def _dict_repr(self):
        dict = {}
        dict['pll_src'] = self.pll_src
        dict['adc_freq'] = self.adc_freq
        dict['target_freq'] = self.target_freq
        dict['adc_mul'] = self.adc_mul
        dict['pll_locked'] = self.pll_locked
        dict['adc_delay'] = self.adc_delay
        dict['target_delay'] = self.target_delay
        return dict

    def __repr__(self):
        return dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()