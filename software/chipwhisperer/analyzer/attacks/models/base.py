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
from chipwhisperer.common.utils.parameter import Parameterized, setupSetParam
from chipwhisperer.common.utils import util

class ModelsBase(Parameterized):
    _name = 'Crypto Model'

    hwModels = util.DictType()

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