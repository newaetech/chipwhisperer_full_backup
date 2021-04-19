from chipwhisperer.common.utils.util import dict_to_str, DelayedKeyboardInterrupt
import time

class CDCI6214:
    def __init__(self, scope):
        self._scope = scope
        self.setup()
        self.set_pll_input()
        self.set_outdiv(3, 0)
        self.set_outdiv(1, 0)

        self._pll_output_frequency = 2500E6 # 2.5GHz

        
    def write_reg(self, addr, data):
        """data can be a 16-bit integer or a 2 element list
        """
        if not hasattr(data, "__getitem__"):
            tmp = [data & 0xFF, (data >> 8) & 0xFF]
            data = tmp

        self._scope._getNAEUSB().sendCtrl(0x29, data=[1, addr, 0x00, data[0], data[1]])
        
    def read_reg(self, addr, as_int=False):
        """If as_int is True, return as a 16-bit integer
        
        Else return as a bytearray
        """

        self._scope._getNAEUSB().sendCtrl(0x29, data=[0, addr, 0x00, 0, 0])    
        data = self._scope._getNAEUSB().readCtrl(0x29, dlen=3)

        if data[0] != 2:
            raise IOError("PLL/I2C Error")

        if as_int is True:
            return (data[1]) | (data[2] << 8)
        return bytearray(data[1:])
    
    def update_reg(self, addr, data, mask):
        """data/mask can be a 16-bit integer or a 2 element list
        
        Ands the register data with mask, then ORs data
        """
        
        if not hasattr(data, "__getitem__"):
            tmp = [data & 0xFF, (data >> 8) & 0xFF]
            data = tmp
            
        if not hasattr(mask, "__getitem__"):
            tmp = [mask & 0xFF, (mask >> 8) & 0xFF]
            mask = tmp
            
        reg_val = self.read_reg(addr, as_int=False)
        reg_val[0] &= 0xFF - mask[0] # the not we want ;)
        reg_val[1] &= 0xFF - mask[1]
        
        reg_val[0] |= data[0]
        reg_val[1] |= data[1]
        
        self.write_reg(addr, reg_val)

        
    def setup(self):
        """Do required initial setup
        """
        # disable GPIO1/4 as inputs
        self.update_reg(0x00, (1 << 13) | (1 << 12), 0)
        
        self.update_reg(0x04, (1 << 3) | (1 << 4), 0) # turn off outputs 2 and 4
        
        self.update_reg(0x05, 0, 0b11111110111) # turn on power to everything
        
        self.update_reg(0x3B, 1 << 4, 0) # Set output driver to high speed
        
        # disable SYNC on channel 3
        self.update_reg(0x32, 0 << 10, 1 << 10)
        
        # disable SYNC on channel 1
        self.update_reg(0x26, 0 << 10, 1 << 10)

        # disable glitchless on channel 3
        self.update_reg(0x33, 0, 1)

        # disable glitchless on channel 1
        self.update_reg(0x27, 0, 1)

        # Disable channel 2: mute=1, outbuf=off
        self.update_reg(0x2C, (1<<7), (1<<7) | (0x7<<2))

        # Disable channel 4: mute-1
        self.update_reg(0x38, (1<<7), (1<<7) | (0x7<<2))

        # Set ref input as AC-differential, XIN to xtal
        self.update_reg(0x1A, (1 << 15) | (0x0B << 8), (1 << 15) | 0b11 | (0xFF << 8))
        
        # use ref_mux_src bit to select input
        self.update_reg(0x01, 1 << 9, 0b11 << 8)

    def get_pll_input(self):
        """True if XTAL or False if FPGA
        """
        return (self.read_reg(0x01, True) & (1 << 8) == (0))

    def get_outfreq(self, pll_out=3):
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

            return self._pll_output_frequency / outdiv / prescale
        elif pll_out == 1:
            prescale_reg = (self.read_reg(0x1E, True) >> 2) & 0b11
            prescale = prescale_lut[prescale_reg]

            outdiv = (self.read_reg(0x25, True) & 0x3FFF)
            if outdiv == 0:
                return 0

            return self._pll_output_frequency / outdiv / prescale
            
    def reset(self):
        """Do a reset of the PLL chip. Doesn't reset registers
        """
        self.update_reg(0x0, 1 << 2, 0x00)
        
    def recal(self):
        self.update_reg(0x0, 1 << 5, 1 << 5)
        
    def set_pll_input(self, xtal=True):
        """Set input to PLL and set input to 4MHz
        
        If xtal, use xtal, otherwise use FPGA clock
        """
        if xtal:
            # set input to xtal
            self.update_reg(0x01, 0 << 8, 1 << 8)
            
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
            self.update_reg(0x31, div, 0x3FFF) # set div
            self.update_reg(0x32, (1) | (1 << 2), 0xFF) # LVDS CH3
            self.reset()
        elif pll_out == 1:
            self.update_reg(0x25, (1 << 14) | div, 0xFFFF) # set div, prescaler B
            self.reset()
        else:
            raise ValueError("pll_out must be 1 or 3, not {}".format(pll_out))
            
    def set_outfreq(self, pll_out=3, freq=50E6):
        base_freq = self._pll_output_frequency
        if not freq:
            self.set_outdiv(pll_out, 0)
        if (freq < 100E3) or (freq > 250E6):
            raise ValueError("Max freq = 250MHz, min freq = 100kHz, given {}".format(freq))
            
        best_error = float('inf') #infinite error
        best_prescale = 4
        best_div = 100
        for prescale in range(4, 7):
            fin = base_freq / prescale
            div = int((fin / freq) + 0.5)
            actual_freq = fin / div
            error = abs(freq - actual_freq) / freq
            if error < best_error:
                best_error = error
                best_prescale = prescale
                best_div = div
                
            #print(error, prescale, div)
            
            
        #print("Found div {} prescale {}".format(best_div, best_prescale))
        self.set_prescale(pll_out, best_prescale)
        self.set_outdiv(pll_out, best_div)
    
    def set_bypass_adc(self, enable_bypass):
        """Routes FPGA clock input directly to ADC, bypasses PLL.
        """
        if enable_bypass:       
            #fpga input
            self.pll_src = "fpga"

            #For output 3 (hard coded):

            # turn on bypass buffer for CH3
            self.update_reg(0x1B, (1<<13), (1<<13))

            # Output divide by 1
            self.update_reg(0x31, 1, 0x3FFF)

            # Output source is REF
            self.update_reg(0x31, 0xC000, 0xC000)
        else:

            # Output source is PSA
            self.update_reg(0x31, 0, 0xC000)

            # turn off bypass buffer for CH3
            self.update_reg(0x1B, 0, (1<<13))

        
    @property
    def pll_src(self):
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

    @property
    def adc_freq(self):
        return self.get_outfreq(pll_out=3)

    @adc_freq.setter
    def adc_freq(self, freq):
        self.set_outfreq(pll_out=3, freq=freq)

    @property
    def target_freq(self):
        return self.get_outfreq(pll_out=1)

    @target_freq.setter
    def target_freq(self, freq):
        self.set_outfreq(pll_out=1, freq=freq)

    def _dict_repr(self):
        dict = {}
        dict['pll_src'] = self.pll_src
        dict['adc_freq'] = self.adc_freq
        dict['target_freq'] = self.target_freq
        return dict

    def __repr__(self):
        return dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()