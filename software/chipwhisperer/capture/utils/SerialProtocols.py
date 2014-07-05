
def CWCalcClkDiv(sysclk, targfreq):
    clkdivider = int((sysclk / (2 * targfreq)) + 1)   
    
    try:
        actual = sysclk / (2 * (clkdivider - 1))
    except ZeroDivisionError:
        actual = 0
    error = (float(actual - targfreq) / targfreq)*100.0        
    
    return (clkdivider, error)
    
    #    desired_freq = 38400 * 3
    #    clk = 30E6
    #    clkdivider = (clk / (2 * desired_freq)) + 1        
    #    self.cwADV.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)


def strToBits(string, startbits=1, stopbits=1, parity="none", guardbits=0):
    """Converts a string into a standard serial pattern"""
    totalpat = []
    for s in string:
        
        try:        
            bp = ord(s)
        except:
            bp = s
            
        if s is None:
            for i in range(0, startbits):
                totalpat.append(1)
                
            for i in range(0, 8):
                totalpat.append(1)
                
            if parity != "none":
                totalpat.append(1)
                
            for i in range(0, stopbits):
                totalpat.append(1)
                
            for i in range(0, guardbits):
                totalpat.append(1)
        else:
                
            #Start bits
            for i in range(0, startbits):
                totalpat.append(0)
             
            onebits = 0
             
            #Following for MSB first:   
            #for i in range(7, -1, -1):
            #Serial protocols LSB first:
            for i in range(0,  8):
                bit = (bp >> i) & 0x01
                if bit:
                    onebits += 1
                totalpat.append(bit)
    
            #Parity bits
            if (onebits % 2) == 0:
                if parity == "even":
                    totalpat.append(0)
                elif parity == "odd":
                    totalpat.append(1)
            else:
                if parity == "even":
                    totalpat.append(1)
                elif parity == "odd":
                    totalpat.append(0)            
    
            #Stop bits
            for i in range(0, stopbits):
                totalpat.append(1)
    
            for i in range(0, guardbits):
                totalpat.append(1)       
                
    return totalpat
