#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
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

from chipwhisperer.analyzer.attacks.models.aes.funcs import sbox, inv_sbox, subbytes, inv_subbytes, mixcolumns, inv_mixcolumns, shiftrows, inv_shiftrows
from chipwhisperer.common.utils.aes_tables import t_table_hw, t_table_hw_dec

from .base import ModelsBase
from chipwhisperer.analyzer.attacks.models.aes.key_schedule import key_schedule_rounds
from chipwhisperer.common.utils.util import camel_case_deprecated
from typing import Optional

class AESLeakageHelper(object):

    #Name of AES Model
    name = 'AES Leakage Model (unnamed)'

    #c model enumeration value, if a C model exists for this device
    c_model_enum_value : Optional[int]= None
    c_model_enum_name : Optional[str] = None

    INVSHIFT_undo = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]

    def sbox(self, data):
        """Helper function: performs AES sbox on single byte"""
        return sbox(data)

    def inv_sbox(self, data):
        """Helper function: performs AES inv-sbox on single byte"""
        return inv_sbox(data)

    def subbytes(self, state):
        """Helper function: performs AES sbox on all bytes of state"""
        return subbytes(state)

    def inv_subbytes(self, state):
        """Helper function: performs AES inv-sbox on all bytes of state"""
        return inv_subbytes(state)

    def mixcolumns(self, state):
        """Helper function: performs AES mixcols on all bytes of state"""
        return mixcolumns(state)

    def inv_mixcolumns(self, state):
        """Helper function: performs AES inv-mixcols on all bytes of state"""
        return inv_mixcolumns(state)

    def shiftrows(self, state):
        """Helper function: performs AES shiftrows on all bytes of state"""
        return shiftrows(state)

    def inv_shiftrows(self, state):
        """Helper function: performs AES inv-shiftrows on all bytes of state"""
        return inv_shiftrows(state)

    def key_schedule_rounds(self, inputkey, inputround, desiredround):
        """Helper function: takes AES key from one round to another round-eky """
        return key_schedule_rounds(inputkey, inputround, desiredround)

    def xtime(self, a):
        """Helper function: xtime operation (normally used in software AES)"""
        a %= 0x100
        b = 0
        if a & 0x80:
            b = 0x1b
        a <<= 1
        a &= 0xff
        a ^= b
        return a

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

class PtKey_XOR(AESLeakageHelper):
    name = 'HW: AddRoundKey Output, First Round (Enc)'
    def leakage(self, pt, ct, key, bnum):
        return pt[bnum] ^ key[bnum]

class SBox_output(AESLeakageHelper):
    name = 'HW: AES SBox Output, First Round (Enc)'
    c_model_enum_value = 1
    c_model_enum_name = 'LEAK_HW_SBOXOUT_FIRSTROUND'
    def leakage(self, pt, ct, key, bnum):
        return self.sbox(pt[bnum] ^ key[bnum])

class InvSBox_output(AESLeakageHelper):
    name = 'HW: AES Inv SBox Output, First Round (Dec)'
    c_model_enum_value = 6
    c_model_enum_name = 'LEAK_HW_INVSBOXOUT_FIRSTROUND'
    def leakage(self, pt, ct, key, bnum):
        return self.inv_sbox(pt[bnum] ^ key[bnum])

class InvSBox_output_alt(AESLeakageHelper):
    name = 'HW: AES Inv SBox Output, First Round (Dec)'
    c_model_enum_value = 61
    c_model_enum_name = 'LEAK_HW_INVSBOXOUT_FIRSTROUND'
    def leakage(self, pt, ct, key, bnum):
        return self.inv_sbox(pt[bnum] ^ key[bnum])

    def process_known_key(self, inpkey):
        k = key_schedule_rounds(inpkey, 0, 10)
        return k

class LastroundHW(AESLeakageHelper):
    name = 'HW: AES Last-Round State'
    def leakage(self, pt, ct, key, bnum):
        # HD Leakage of AES State between 9th and 10th Round
        # Used to break SASEBO-GII / SAKURA-G
        st10 = ct[self.INVSHIFT_undo[bnum]]
        st9 = inv_sbox(ct[bnum] ^ key[bnum])
        return st9

    def process_known_key(self, inpkey):
        return key_schedule_rounds(inpkey, 0, 10)

class LastroundStateDiff(AESLeakageHelper):
    name = 'HD: AES Last-Round State'
    c_model_enum_value = 2
    c_model_enum_name = 'LEAK_HD_LASTROUND_STATE'
    def leakage(self, pt, ct, key, bnum):
        # HD Leakage of AES State between 9th and 10th Round
        # Used to break SASEBO-GII / SAKURA-G
        st10 = ct[self.INVSHIFT_undo[bnum]]
        st9 = inv_sbox(ct[bnum] ^ key[bnum])
        return (st9 ^ st10)

    def process_known_key(self, inpkey):
        return key_schedule_rounds(inpkey, 0, 10)

class PipelineDiff(AESLeakageHelper):
    name = 'HD: AES round 9 between previous and current encryptions'
    def leakage(self, pt, ct, prev_pt, prev_ct, key, bnum):
        curr = inv_sbox(ct[bnum] ^ key[bnum])
        prev = inv_sbox(prev_ct[bnum] ^ key[bnum])
        return curr ^ prev

    def process_known_key(self, inpkey):
        return key_schedule_rounds(inpkey, 0, 10)

class HalfPipelineDiff(AESLeakageHelper):
    name = 'HD: AES previous round 10 and current round 9'
    def leakage(self, pt, ct, prev_pt, prev_ct, key, bnum):
        curr = inv_sbox(ct[bnum] ^ key[bnum])
        prev = prev_ct[self.INVSHIFT_undo[bnum]]
        return curr ^ prev

    def process_known_key(self, inpkey):
        return key_schedule_rounds(inpkey, 0, 10)

class LastroundStateDiffAlternate(AESLeakageHelper):
    name = 'HD: AES Last-Round State Alternate'
    def leakage(self, pt, ct, key, bnum):
        # Alternate leakage
        st10 = ct[bnum]
        st9 = inv_sbox(ct[bnum] ^ key[bnum])
        return (st9 ^ st10)

    def process_known_key(self, inpkey):
        k = key_schedule_rounds(inpkey, 0, 10)
        k = self.shiftrows(k)
        return k

class SBoxInOutDiff(AESLeakageHelper):
    name = 'HD: AES SBox Input to Output'
    c_model_enum_value = 3
    c_model_enum_name = 'LEAK_HD_SBOX_IN_OUT'
    def leakage(self, pt, ct, key, bnum):
        st1 = pt[bnum] ^ key[bnum]
        st2 = self.sbox(st1)
        return st1 ^ st2

class SBoxInputSuccessive(AESLeakageHelper):
    name = 'HD: AES SBox Input i to i+1'
    c_model_enum_value = 4
    c_model_enum_name = 'LEAK_HD_SBOX_IN_SUCCESSIVE'
    def leakage(self, pt, ct, key, bnum):
        st1 = pt[bnum] ^ key[bnum]
        if bnum > 0:
            try:
                st2 = pt[bnum - 1] ^ key[bnum-1]
            except IndexError:
                raise ValueError("Successive requires known key")
        else:
            st2 = 0
        return st1 ^ st2

class SBoxOutputSuccessive(AESLeakageHelper):
    name = 'HD: AES SBox Output i to i+1'
    c_model_enum_value = 5
    c_model_enum_name = 'LEAK_HD_SBOX_OUT_SUCCESSIVE'
    def leakage(self, pt, ct, key, bnum):
        st1 = self.sbox(pt[bnum] ^ key[bnum])
        if bnum > 0:
            try:
                st2 = self.sbox(pt[bnum - 1] ^ key[bnum-1])
            except IndexError:
                raise ValueError("Successive requires known key")
        else:
            st2 = 0
        return st1 ^ st2

class Mixcolumns_output(AESLeakageHelper):
    name = 'HW: AES Mixcolumns Output'
    #This is mostly a nonsense leakage model for now, but added for completeness
    def leakage(self, pt, ct, key, bnum):
        state = [pt[i] ^ key[i] for i in range(0, 16)]
        state = self.subbytes(state)
        state = self.shiftrows(state)
        state = self.mixcolumns(state)
        return state[bnum]

class ShiftColumns_output(AESLeakageHelper):
    name = 'HW: AES ShiftColumns Output'
    def leakage(self, pt, ct, key, bnum):
        state = [pt[i] ^ key[i] for i in range(0, 16)]
        state = self.subbytes(state)
        state = self.shiftrows(state)
        return state[bnum]

class Round1Round2StateDiff_Text(AESLeakageHelper):
    name = 'HD: AES Round1/Round2 State diff for text'
    def leakage(self, pt, ct, key, bnum):
        state1 = pt[:]
        state = [pt[i] ^ key[i] for i in range(0, 16)]
        state = self.subbytes(state)
        state = self.shiftrows(state)
        state = self.mixcolumns(state)
        return state[bnum] ^ state1[bnum]

class Round1Round2StateDiff_KeyMix(AESLeakageHelper):
    name = 'HD: AES Round1/Round2 State diff for key addition'
    def leakage(self, pt, ct, key, bnum):
        state = [pt[i] ^ key[i] for i in range(0, 16)]
        state1 = state[:]
        state = self.subbytes(state)
        state = self.shiftrows(state)
        state = self.mixcolumns(state)

        key2 = self.key_schedule_rounds(key, 0, 1)
        state = [state[i] ^ key2[i] for i in range(0, 16)]

        return state[bnum] ^ state1[bnum]

class Round1Round2StateDiff_SBox(AESLeakageHelper):
    name = 'HD: AES Round1/Round2 State diff for SBox'
    def leakage(self, pt, ct, key, bnum):
        state = [pt[i] ^ key[i] for i in range(0, 16)]
        state = self.subbytes(state)
        state1 = state[:]
        state = self.shiftrows(state)
        state = self.mixcolumns(state)

        key2 = self.key_schedule_rounds(key, 0, 1)
        state = [state[i] ^ key2[i] for i in range(0, 16)]
        state = subbytes(state)
        return state[bnum] ^ state1[bnum]

#List of all classes you can use
enc_list = [SBox_output, PtKey_XOR, SBoxInputSuccessive, SBoxInOutDiff, LastroundStateDiff, LastroundStateDiffAlternate, PipelineDiff, HalfPipelineDiff, SBoxOutputSuccessive, ShiftColumns_output, Mixcolumns_output, Round1Round2StateDiff_Text, Round1Round2StateDiff_KeyMix, Round1Round2StateDiff_SBox]
dec_list = [InvSBox_output]


class AES128_8bit(ModelsBase):
    """Leakage model for AES128 attacks.

    Make sure to set the actual leakage model (i.e. SBox_Output)
    """
    _name = 'AES 128'

    hwModels = OrderedDict((mod.name, mod) for mod in (enc_list+dec_list) ) # type: ignore

    hw_models = OrderedDict((mod.__name__, mod) for mod in (enc_list+dec_list)) # type: ignore

    _has_prev = False

    def __init__(self, model=SBox_output, bitmask=0xFF):
        ModelsBase.__init__(self, 16, 256, model=model)
        self.numRoundKeys = 10
        self._mask = bitmask

    def _updateHwModel(self):
        """" Re-implement this to update leakage model """
        self.modelobj = None

        #Check if they passed an object...
        if isinstance(self.model, AESLeakageHelper):
            self.modelobj = self.model

        #Check if they passed a class...
        elif inspect.isclass(self.model) and issubclass(self.model, AESLeakageHelper):
            self.modelobj = self.model()

        #Otherwise it's probably one of these older keys (kept for backwards-compatability)
        else:
            for mod in self.hwModels:
                if (mod.c_model_enum_value == self.model) or (mod.name == self.model):
                    self.modelobj = mod()
                    break

        if self.modelobj is None:
            raise ValueError("Invalid model: %s" % str(self.model))

    def process_known_key(self, inpkey):
        if hasattr(self.modelobj, 'process_known_key'):
            return self.modelobj.process_known_key(inpkey)
        return inpkey

    def leakage(self, pt, ct, guess, bnum, state):
        """ Leakage as set by model

        Args:
            pt (list): Plaintext/textin
            ct (list): Ciphertext/textout
            guess (list): Key guess
            bnum (list): Subkey Byte Number
            state (list): The state of the key finding

        Returns:
            A hamming weight (int)
        """
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

    def key_schedule_rounds(self, inputkey, inputround, desiredround):
        """Changes the round of inputkey from inputround to desiredround

        Args:
            inputkey (list): key that you want to change the round of
            inputround (int): Round that inputkey is currently in
            desiredround (int): Round that you want inputkey to be in

        Returns:
            desired key (list)
        """
        return key_schedule_rounds(inputkey, inputround, desiredround)

    keyScheduleRounds = camel_case_deprecated(key_schedule_rounds)


class AES128_ttable(AES128_8bit):
    _has_prev = False
    def leakage(self, pt, ct, guess, bnum, state):
        """ Leakage as set by model

        Args:
            pt (list): Plaintext/textin
            ct (list): Ciphertext/textout
            guess (list): Key guess
            bnum (list): Subkey Byte Number
            state (list): The state of the key finding

        Returns:
            A hamming weight (int)
        """
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
        return t_table_hw[intermediate_value]

class AES128_ttable_dec(AES128_8bit):
    _has_prev = False
    def leakage(self, pt, ct, guess, bnum, state):
        """ Leakage as set by model

        Args:
            pt (list): Plaintext/textin
            ct (list): Ciphertext/textout
            guess (list): Key guess
            bnum (list): Subkey Byte Number
            state (list): The state of the key finding

        Returns:
            A hamming weight (int)
        """
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
        return t_table_hw_dec[intermediate_value]


class AES128_prev(AES128_8bit):
    """Extension of AES128_8bit which adds prev_pt and prev_ct parameters, for when the
    leakage is tied to both the current and previous ciphertext and/or plaintext.
    """
    _has_prev = True

    def leakage(self, pt, ct, prev_pt, prev_ct, guess, bnum, state):
        """ Leakage as set by model

        Args:
            pt (list): Plaintext/textin
            ct (list): Ciphertext/textout
            prev_pt (list): Plaintext/textin for the previous encryption
            prev_ct (list): Ciphertext/textout for the previous encryption
            guess (list): Key guess
            bnum (list): Subkey Byte Number
            state (list): The state of the key finding

        Returns:
            A hamming weight (int)
        """
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
        intermediate_value = self.modelobj.leakage(pt, ct, prev_pt, prev_ct, key, bnum)

        #For bit-wise attacks, mask off specific bit value
        intermediate_value = self._mask & intermediate_value

        #Return HW of guess
        return self.HW[intermediate_value]

