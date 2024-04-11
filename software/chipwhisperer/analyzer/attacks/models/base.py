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

#The following is placed here for backwards compatability with imports expected the getHW function
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

    hwModels = {} # type: ignore
    hwModels_toStr = [] # type: ignore

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

    def __init__(self, numSubKeys=None, permPerSubkey=None, model=None):
        self.sigParametersChanged = util.Signal()
        self.numSubKeys = numSubKeys
        self.permPerSubkey = permPerSubkey
        self.numRoundKeys = 0
        self.model = model
        #Update interal models (if required)
        self._updateHwModel()

        if model.name in self.hwModels:
            if self.hwModels[model.name] != model:
                self.hwModels[str(model)] = model
        else:
            self.hwModels[model.name] = model

        self.getParams().addChildren([
            {'name':'Hardware Model', 'type':'list', 'values':self.hwModels, 'get':self.getHwModel, 'set':self.setHwModel, 'addToList':True}
        ])

        if numSubKeys:
            self.getParams().addChildren([{'name':'Number of SubKeys', 'type':'int', 'get':self.getNumSubKeys, 'readonly':True}])

        if permPerSubkey:
            self.getParams().addChildren([{'name':'Number of Permutations', 'type':'int', 'get':self.getPermPerSubkey, 'readonly':True}])

    @classmethod
    def getHW(cls, n):
        return cls.HW[n]

    def process_known_key(self, inpkey):
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
        raise NotImplementedError()

    def getRoundKeys(self, key, iniRound):
        ret = []
        for targetRound in range(0, self.numRoundKeys+1):
            ret.append(self.keyScheduleRounds(key, iniRound, targetRound, returnSubkeys=False))
        return ret

    @setupSetParam("Hardware Model")
    def setHwModel(self, model, kwargs=None):
        self.model = model
        self.model_kwargs = kwargs
        self._updateHwModel()
        self.sigParametersChanged.emit()

    def _updateHwModel(self):
        """" Re-implement this to update leakage model """
        pass

    def _VccToGnd(self, var):
        """Convert from number of 1's to number of 0's... used when shunt inserted in GND path"""
        return 8-var
