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
import sys

from chipwhisperer.common.utils.parameter import Parameterized, setupSetParam
from chipwhisperer.common.utils import util
import numpy as np

_HW = [0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3,
       4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4,
       4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2,
       3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
       4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4,
       5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
       3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2,
       3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
       4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,
       6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5,
       5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6,
       7, 7, 8]

def getHW(byte):
    return _HW[byte]

class ModelsBase(Parameterized):
    _name = 'Crypto Model'

    hwModels = util.DictType()
    hwModels_toStr = []

    ##Generate this table with:
    #HW = []
    #for n in range(0, 256):
    #    HW = HW + [bin(n).count("1")]
    HW = [0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3,
          4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4,
          4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2,
          3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
          4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4,
          5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
          3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2,
          3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
          4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,
          6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5,
          5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6,
          7, 7, 8]

    def __init__(self, numSubKeys, permPerSubkey, model=None):
        self.sigParametersChanged = util.Signal()
        self.numSubKeys = numSubKeys
        self.permPerSubkey = permPerSubkey
        self.numRoundKeys = 0
        self.model = model
        self.getParams().addChildren([
            {'name':'Hardware Model', 'type':'list', 'values':self.hwModels, 'get':self.getHwModel, 'set':self.setHwModel},
            {'name':'Number of SubKeys', 'type':'int', 'get':self.getNumSubKeys, 'readonly':True},
            {'name':'Number of Permutations', 'type':'int', 'get':self.getPermPerSubkey, 'readonly':True},
        ])

    @classmethod
    def getHW(cls, n):
        return cls.HW[n]

    def processKnownKey(self, inpkey):
        return inpkey

    def leakage(self, pt, ct, guess, bnum, state):
        pass

    def getNumSubKeys(self):
        return self.numSubKeys

    def getPermPerSubkey(self):
        return self.permPerSubkey

    def getNumRoundKeys(self):
        return self.numRoundKeys

    def getHwModel(self):
        return self.model

    def keyScheduleRounds(self, inputkey, inputround, desiredround, returnSubkeys=True):
        pass

    def getRoundKeys(self, key, iniRound):
        ret = []
        for targetRound in range(0, self.numRoundKeys+1):
            ret.append(self.keyScheduleRounds(key, iniRound, targetRound, returnSubkeys=False))
        return ret

    @setupSetParam("Hardware Model")
    def setHwModel(self, model):
        self.model = model
        self.sigParametersChanged.emit()

    def getHwModelString(self):
        return sys.modules[self.__class__.__module__].__name__  + '.' + self.__class__.__name__  + '.' + self.hwModels_toStr[self.model]

    def binary_list_to_subkeys(self, bitlist, nrBits):
        ret = []
        pos = 0
        while pos <= len(bitlist) - nrBits:
            out = 0
            for bit in range(nrBits):
                out = (out << 1) | bitlist[pos+bit]
            ret.append(out)
            pos += nrBits
        return ret

    def array_of_bytes_to_bin(self, bytes, nrBits):
        init=np.array([], dtype=bool)
        for byte in bytes:
            init = np.concatenate((init, np.unpackbits(np.uint8(byte))[8-nrBits:]), axis=0)
        return init

    def _VccToGnd(self, var):
        """Convert from number of 1's to number of 0's... used when shunt inserted in GND path"""
        return 8-var