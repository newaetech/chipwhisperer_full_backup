# #    This file is part of chipwhisperer.
# #
# #    chipwhisperer is free software: you can redistribute it and/or modify
# #    it under the terms of the GNU General Public License as published by
# #    the Free Software Foundation, either version 3 of the License, or
# #    (at your option) any later version.
# #
# #    chipwhisperer is distributed in the hope that it will be useful,
# #    but WITHOUT ANY WARRANTY; without even the implied warranty of
# #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# #    GNU Lesser General Public License for more details.
# #
# #    You should have received a copy of the GNU General Public License
# #    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
# #=================================================
# from collections import OrderedDict
# import inspect
# from .base import ModelsBase
# import numpy as np
# from chipwhisperer.common.utils.util import binarylist2bytearray, bytearray2binarylist

# class DESLeakageHelper(object):

#     #Name of DES Model
#     name = 'DES Leakage Model (unnamed)'

#     #c model enumeration value, if a C model exists for this device
#     c_model_enum_value = None
#     c_model_enum_name = None

#     # Based on https://gist.github.com/eigenein/1275094
#     # Author:   Todd Whiteman
#     # Date:     16th March, 2009
#     # Verion:   2.0.0
#     # License:  Public Domain - free to do as you wish
#     # Homepage: http://twhiteman.netfirms.com/des.html

#     sBox = 8*[64*[0]]

#     sBox[0] = (14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
#                 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
#                 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
#                15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13)

#     sBox[1] = (15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
#                 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
#                 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
#                13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9)

#     sBox[2] = (10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
#                13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
#                13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
#                 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12)

#     sBox[3] = ( 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
#                13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
#                10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
#                 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14)

#     sBox[4] = ( 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
#                14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
#                 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
#                11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3)

#     sBox[5] = (12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
#                10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
#                 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
#                 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13)

#     sBox[6] = ( 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
#                13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
#                 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
#                 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12)

#     sBox[7] = (13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
#                 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
#                 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
#                 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11)


#     # Permutation and translation tables for DES
#     __pc1 = [56, 48, 40, 32, 24, 16,  8,
#           0, 57, 49, 41, 33, 25, 17,
#           9,  1, 58, 50, 42, 34, 26,
#          18, 10,  2, 59, 51, 43, 35,
#          62, 54, 46, 38, 30, 22, 14,
#           6, 61, 53, 45, 37, 29, 21,
#          13,  5, 60, 52, 44, 36, 28,
#          20, 12,  4, 27, 19, 11,  3
#     ]

#     __pc1_inv = [
#         7, 15, 23, 55, 51, 43, 35, None,
#         6, 14, 22, 54, 50, 42, 34, None,
#         5, 13, 21, 53, 49, 41, 33, None,
#         4, 12, 20, 52, 48, 40, 32, None,
#         3, 11, 19, 27, 47, 39, 31, None,
#         2, 10, 18, 26, 46, 38, 30, None,
#         1, 9, 17, 25, 45, 37, 29, None,
#         0, 8, 16, 24, 44, 36, 28, None
#     ]

#     # permuted choice key (table 2)
#     __pc2 = [
#         13, 16, 10, 23,  0,  4,
#          2, 27, 14,  5, 20,  9,
#         22, 18, 11,  3, 25,  7,
#         15,  6, 26, 19, 12,  1,
#         40, 51, 30, 36, 46, 54,
#         29, 39, 50, 44, 32, 47,
#         43, 48, 38, 55, 33, 52,
#         45, 41, 49, 35, 28, 31
#     ]

#     __pc2_inv = [
#         4, 23, 6, 15, 5, 9, 19,
#         17, None, 11, 2, 14, 22,
#         0, 8, 18, 1, None, 13, 21,
#         10, None, 12, 3, None, 16,
#         20, 7, 46, 30, 26, 47,
#         34, 40, None, 45, 27, None,
#         38, 31, 24, 43, None, 36,
#         33, 42, 28, 35, 37, 44,
#         32, 25, 41, None, 29, 39
#     ]

#     # initial permutation IP
#     __ip = [57, 49, 41, 33, 25, 17, 9,  1,
#         59, 51, 43, 35, 27, 19, 11, 3,
#         61, 53, 45, 37, 29, 21, 13, 5,
#         63, 55, 47, 39, 31, 23, 15, 7,
#         56, 48, 40, 32, 24, 16, 8,  0,
#         58, 50, 42, 34, 26, 18, 10, 2,
#         60, 52, 44, 36, 28, 20, 12, 4,
#         62, 54, 46, 38, 30, 22, 14, 6
#     ]

#     # Expansion table for turning 32 bit blocks into 48 bits
#     __expansion_table = [
#         31,  0,  1,  2,  3,  4,
#          3,  4,  5,  6,  7,  8,
#          7,  8,  9, 10, 11, 12,
#         11, 12, 13, 14, 15, 16,
#         15, 16, 17, 18, 19, 20,
#         19, 20, 21, 22, 23, 24,
#         23, 24, 25, 26, 27, 28,
#         27, 28, 29, 30, 31,  0
#     ]

#     # number left rotations of pc1
#     __left_rotations = [0, 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1]

#     def __permutate(self, table, block):
#         """Permutate this block with the specified table"""
#         return [block[v] if v is not None else v for i,v in enumerate(table)]

#     def keyScheduleRounds(self, inputkey, inputround, desiredround, returnSubkeys=True):
#         """Create the 16 subkeys K[1] to K[16] from the given key"""
#         if inputround == 0:
#             inputkey = bytearray2binarylist(inputkey,8)
#             key = self.__permutate(self.__pc1, inputkey)
#         else:
#             inputkey = bytearray2binarylist(inputkey,6)
#             key = self.__permutate(self.__pc2_inv, inputkey)
#         i = inputround
#         L = key[:28]
#         R = key[28:]
#         while i < desiredround:
#             i += 1
#             j = 0
#             # Perform circular left shifts
#             while j < self.__left_rotations[i]:
#                 L.append(L[0])
#                 del L[0]

#                 R.append(R[0])
#                 del R[0]

#                 j += 1
#         while i > desiredround:
#             # Perform circular right shifts
#             j = 0
#             while j < self.__left_rotations[i]:
#                 L.insert(0,L[27])
#                 del L[28]

#                 R.insert(0,R[27])
#                 del R[28]

#                 j += 1
#             i -= 1
#         if desiredround==0:
#             key = self.__permutate(self.__pc1_inv, L + R)
#             return binarylist2bytearray(key, 8) if returnSubkeys else key
#         else:
#             key = self.__permutate(self.__pc2, L + R)
#             return binarylist2bytearray(key, 6) if returnSubkeys else key

#     def sbox_in_first_fbox(self, pt, guess, bnum):
#         init=bytearray2binarylist(pt, 8)

#         initPermut = self.__permutate(self.__ip, init)
#         R = initPermut[32:]
#         expR = self.__permutate(self.__expansion_table, R)
#         expR = expR[bnum*6:(1+bnum)*6]
#         guess = np.unpackbits(np.uint8(guess))
#         guess = guess[2:]
#         B = list(map(lambda x, y: x ^ y, expR, guess))
#         return B

#     def sbox_out_first_fbox(self, pt, guess, bnum):
#         B = self.sbox_in_first_fbox(pt, guess, bnum)
#         m = (B[0] << 1) + B[5]
#         n = (B[1] << 3) + (B[2] << 2) + (B[3] << 1) + B[4]
#         # Find the permutation value
#         return self.sBox[bnum][(m << 4) + n]

#     def leakage(self, pt, ct, key, bnum):
#         """
#         Override this function with specific leakage function (S-Box output, HD, etc).

#         Args:
#             pt: 16-byte plain-text input
#             ct: 16-byte cipher-text output.
#             key: 16-byte AES key - byte 'bnum' may be a GUESS if key is known. Rest of bytes may/may not be valid too.
#             bnum: Byte number we are trying to attack.

#         Returns:
#             Value that will be presented on the 8-bit bus. Leakage model (such as HW) will map this to leakage itself.
#         """
#         raise NotImplementedError("ASKLeakageHelper does not implement leakage")

# class SBox_output(DESLeakageHelper):
#     name = 'HW: SBoxes Output, First Round'
#     c_model_enum_value = 0
#     c_model_enum_name = 'LEAK_HW_SBOXOUT_FIRSTROUND'
#     def leakage(self, pt, ct, key, bnum):
#         return self.sbox_out_first_fbox(pt, key[bnum], bnum)

# class SBox_input(DESLeakageHelper):
#     name = 'HW: SBoxes Input, First Round'
#     c_model_enum_value = 1
#     c_model_enum_name = 'LEAK_HW_SBOXIN_FIRSTROUND'
#     def leakage(self, pt, ct, key, bnum):
#         return binarylist2bytearray(self.sbox_in_first_fbox(pt, key[bnum], bnum), 6)[0]

# enc_list = [SBox_output, SBox_input]
# dec_list = []

# class DES(ModelsBase):
#     _name = 'DES'

#     hwModels = OrderedDict((mod.name, mod) for mod in (enc_list + dec_list))

#     def __init__(self, model=SBox_output, bitmask=0xFF):
#         ModelsBase.__init__(self, 8, 64, model=model)
#         self.numRoundKeys = 16
#         self._mask = bitmask

#     def _updateHwModel(self):
#         """" Re-implement this to update leakage model """
#         self.modelobj = None

#         #Check if they passed an object...
#         if isinstance(self.model, DESLeakageHelper):
#             self.modelobj = self.model

#         #Check if they passed a class...
#         elif inspect.isclass(self.model) and issubclass(self.model, DESLeakageHelper):
#             self.modelobj = self.model()

#         #Otherwise it's probably one of these older keys (kept for backwards-compatability)
#         else:
#             for mod in self.hwModels:
#                 if (mod.c_model_enum_value == self.model) or (mod.name == self.model):
#                     self.modelobj = mod()
#                     break

#         if self.modelobj is None:
#             raise ValueError("Invalid model: %s" % str(self.model))

#     def process_known_key(self, inpkey):
#         return self.modelobj.keyScheduleRounds(inpkey, 0, 1)

#     def leakage(self, pt, ct, guess, bnum, state):
#         try:
#             # Make a copy so we don't screw with anything...
#             key = list(state['knownkey'])
#         except:
#             # We don't log due to time-sensitive nature... but if state doesn't have "knownkey" will result in
#             # unknown knownkey which causes some attacks to fail. Possibly should make this some sort of
#             # flag to indicate we want to ignore the problem?
#             key = [None] * 8

#         # Guess can be 'none' if we want to use original key as-is
#         if guess is not None:
#             key[bnum] = guess

#         # Get intermediate value
#         intermediate_value = self.modelobj.leakage(pt, ct, key, bnum)

#         # For bit-wise attacks, mask off specific bit value
#         intermediate_value = self._mask & intermediate_value

#         # Return HW of guess
#         return self.HW[intermediate_value]

#     def compare(self, correctKey, guessedKey):
#         """Return the bits that are unknown and differ between the guessed key and the correct key"""
#         unknown=[]
#         wrong=[]
#         for i in range(len(correctKey)):
#             if guessedKey[i]!=correctKey[i] and i%8 != 7:
#                 if guessedKey[i] is None:
#                     unknown.append(i)
#                 else:
#                     wrong.append(i)
#         return (unknown, wrong)

#     def keyScheduleRounds(self, inputkey, inputround, desiredround, returnSubkeys=True):
#         return self.modelobj.keyScheduleRounds(inputkey, inputround, desiredround, returnSubkeys)


# if __name__ == "__main__":
#     model = DES()
#     # rk = [[0,0,0,1,0,0,1,1,0,0,1,1,0,1,0,0,0,1,0,1,0,1,1,1,0,1,1,1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,1],
#     #     [0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0],
#     #     [0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1],
#     #     [0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1],
#     #     [0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1],
#     #     [0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0],
#     #     [0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1],
#     #     [1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0],
#     #     [1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1],
#     #     [1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1],
#     #     [1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1],
#     #     [0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0],
#     #     [0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1],
#     #     [1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1],
#     #     [0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0],
#     #     [1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0],
#     #     [1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1]
#     # ]
#     # for roundNum in range(17):
#     #     print model.keyScheduleRounds(rk[0], 0, roundNum)
#     #
#     # for roundNum in range(0,17):
#     #     tmp = model.keyScheduleRounds(rk[roundNum], roundNum, 0)
#     #     f=0
#     #     for i in range(64):
#     #         if tmp[i]!=rk[0][i] and i%8 != 7:
#     #             if tmp[i] is None:
#     #                 f += 1
#     #             else:
#     #                 print "Error in bit %d in round %d" % (i, roundNum)
#     #     print "%d unrecoverable bits when reverting round key %d" % (f, roundNum)
#     #
#     # pt = [0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]
#     # for permNum in range(8):
#     #     v =  rk[1][permNum*6+0]*32
#     #     v +=  rk[1][permNum*6+1]*16
#     #     v +=  rk[1][permNum*6+2]*8
#     #     v +=  rk[1][permNum*6+3]*4
#     #     v +=  rk[1][permNum*6+4]*2
#     #     v +=  rk[1][permNum*6+5]*1
#     #     print model.sbox_in_first_fbox(pt, v, permNum)

# #22 10 30 21 32 38 07 3F
#     guessedkey = [0x22, 0x10, 0x30, 0x21, 0x32, 0x38, 0x07, 0x3F]
#     originalkey = [0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6]
#     print((str(model.array_of_bytes_to_bin(originalkey, 8))))
#     print((str(model.array_of_bytes_to_bin(guessedkey, 6))))

#     print(("Unknown bits = %s, Wrong bit guesses = %s" % model.compare(model.array_of_bytes_to_bin(originalkey, 8), model.keyScheduleRounds(guessedkey, 1, 0, returnSubkeys=False))))
