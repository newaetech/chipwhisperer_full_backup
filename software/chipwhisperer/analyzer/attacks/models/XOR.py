#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, NewAE Technology Inc
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

from collections import OrderedDict
import inspect

from .base import ModelsBase

class XORLeakageHelper(object):

    name = 'XOR Leakage Model (unnamed)'

    def leakage(self, pt, ct, key, bnum):
        """
        Override this function with specific leakage function (S-Box output, HD, etc).

        Args:
            pt: 16-byte plain-text input
            ct: 16-byte cipher-text output.
            key: 16-byte AES key - byte 'bnum' may be a GUESS if key is known. Rest of bytes may/may not be valid too.
            bnum: Byte number we are trying to attack.

        Returns:
            Value that will be presented on the 8-bit bus. Leakage model (such as HW) will map this to leakage itself.
        """
        raise NotImplementedError("ASKLeakageHelper does not implement leakage")

class PtKey_XOR(XORLeakageHelper):
    name = 'HW: Key XOR Plaintext'
    def leakage(self, pt, ct, key, bnum):
        return pt[bnum] ^ key[bnum]


#List of all classes you can use
enc_list = [PtKey_XOR]
dec_list = [] # type: ignore

class XOR_8bit(ModelsBase):
    _name = 'XOR'

    hwModels = OrderedDict((mod.name, mod) for mod in (enc_list+dec_list) )

    def __init__(self, model=PtKey_XOR, bitmask=0xFF):
        ModelsBase.__init__(self, 4, 256, model=model)

        #Override number of subkeys - todo make this work
        #self.numSubKeys = 2
        #self.getParams().addChildren([{'name': 'Number of SubKeys', 'type':'list', 'values':[1,2,4,8,16,32], 'get':self.getNumSubKeys, 'set':self.setNumSubKeys}])
        self._mask = bitmask


    def _updateHwModel(self):
        """" Re-implement this to update leakage model """
        self.modelobj = None

        #Check if they passed an object...
        if isinstance(self.model, XORLeakageHelper):
            self.modelobj = self.model

        #Check if they passed a class...
        elif inspect.isclass(self.model) and issubclass(self.model, XORLeakageHelper):
            self.modelobj = self.model()

        #Otherwise it's probably one of these older keys (kept for backwards-compatability)
        else:
            raise AttributeError("Unknown model")

        if self.modelobj is None:
            raise ValueError("Invalid model: %s" % str(self.model))

    def process_known_key(self, inpkey):
        return inpkey

    def leakage(self, pt, ct, guess, bnum, state):
        try:
            #Make a copy so we don't screw with anything...
            key = list(state['knownkey'])
        except:
            #We don't log due to time-sensitive nature... but if state doesn't have "knownkey" will result in
            #unknown knownkey which causes some attacks to fail. Possibly should make this some sort of
            #flag to indicate we want to ignore the problem?
            key = [None]*16

        #Guess can be 'none' if we want to use original key as-is
        if guess is not None:
            key[bnum] = guess

        #Get intermediate value
        intermediate_value = self.modelobj.leakage(pt, ct, key, bnum)

        #For bit-wise attacks, mask off specific bit value
        intermediate_value = self._mask & intermediate_value

        #Return HW of guess
        return self.HW[intermediate_value]
