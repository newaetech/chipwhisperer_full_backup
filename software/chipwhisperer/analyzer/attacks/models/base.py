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
        self.numSubKeys = numSubKeys
        self.permPerSubkey = permPerSubkey
        self.model = model
        self.getParams().addChildren([
            {'name':'Hardware Model', 'key':'hw_model', 'type':'list', 'values':self.hwModels, 'get':self.getHwModel, 'set':self.setHwModel},
            {'name':'Number of SubKeys', 'key':'hw_model', 'type':'int', 'get':self.getNumSubKeys, 'readonly':True},
            {'name':'Number of Permutations', 'key':'hw_model', 'type':'int', 'get':self.getPermPerSubkey, 'readonly':True},
        ])

    def processKnownKey(self, inpkey):
        pass

    def leakage(self, pt, ct, guess, bnum, state):
        pass

    def getNumSubKeys(self):
        return self.numSubKeys

    def getPermPerSubkey(self):
        return self.permPerSubkey

    def getHwModel(self):
        return self.model

    @setupSetParam("hw_model")
    def setHwModel(self, model):
        self.model = model

    def getHwModelString(self):
        return sys.modules[self.__class__.__module__].__name__  + '.' + self.__class__.__name__  + '.' + self.hwModels_toStr[self.model]