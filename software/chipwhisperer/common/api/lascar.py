try:
    import lascar # type: ignore
    from lascar.container import Container, TraceBatchContainer # type: ignore
    from lascar import * # type: ignore
    from lascar.tools.aes import sbox, inv_sbox # type: ignore
except ImportError as e:
    print("Unable to import LASCAR")

import numpy as np
class CWContainer(Container):
    def __init__(self, project, values, start=None, end=None, **kwargs):
        
        v = [val for val in values]
        self.values = np.array(v)
        if start is None:
            start = 0
        if end is None:
            end = len(project.waves[0])
        self.leakages = np.array([wave[start:end] for wave in project.waves])    
        Container.__init__(self, **kwargs)
        
    def __getitem__(self, key):
        return TraceBatchContainer.__getitem__(self, key)
    
    def __setitem__(self, key, value):
        TraceBatchContainer.__setitem__(self, key, value)


#The following leakage models copied from /chipwhisperer/analyzer/attacks/models/AES128_8bit.py and
# massaged into Lascar Version

def sbox_HW_gen(byte):
    # selection_with_guess function must take 2 arguments: value and guess
    def selection_with_guess(value, guess):
        return hamming(sbox[value[byte] ^ guess])
    return selection_with_guess

def sboxInOut_HD_gen(byte):
    # selection_with_guess function must take 2 arguments: value and guess
    def selection_with_guess(value, guess):
        return hamming(sbox[value[byte] ^ guess] ^ value[byte])
    return selection_with_guess

def lastround_HD_gen(byte):
    # selection_with_guess function must take 2 arguments: value and guess
    def selection_with_guess(value, guess):
        INVSHIFT_undo = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]
        st10 = value[INVSHIFT_undo[byte]]
        st9 = inv_sbox[value[byte] ^ guess]
        return hamming(st9 ^ st10)
    return selection_with_guess

import pandas as pd # type: ignore
from IPython.display import clear_output # type: ignore
import chipwhisperer as cw
from chipwhisperer.analyzer.attacks._stats import Results
import chipwhisperer.analyzer as cwa

class LascarDisplay:   
    key_schedule_rounds = cwa.aes_funcs.key_schedule_rounds
    def __init__(self, cpa_engines, highlight_key=None):  
        dt = Results()
        for i in range(len(cpa_engines)):
            results = cpa_engines[i].finalize()
            dt.update_subkey(i, results)

        self.dt = dt
        self.hlk = highlight_key
        self.dt.known_key = self.hlk
        
    def format_stat(self, stat):
        return str("{:02X}<br>{:.3f}".format(stat[0], stat[2]))

    def color_corr_key(self, row):
        ret = [""] * 16
        for i,bnum in enumerate(row):
            if bnum[0] == self.hlk[i]:
                ret[i] = "color: red"
            else:
                ret[i] = ""
        return ret
    
    def show_pge(self):
        stat_data = self.dt.find_maximums()
        df = pd.DataFrame(stat_data).transpose()
        return df.head(255).style.format(self.format_stat).apply(self.color_corr_key, axis=1)
        
    def results(self): #getStatistics
        """CW Interfae Function"""
        self.dt.known_key = self.hlk
        return self.dt
    
    def known_key(self): #knownKey
        """CW Interface Function"""
        if self.hlk is None: return [0]*16
        
        return self.hlk
    
    def getReportingInterval(self):
        """CW Interface Function"""
        return 0
    