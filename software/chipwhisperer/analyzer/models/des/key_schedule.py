#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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
#=================================================

#
# This file performs forward AND backwards key scheduling. Can work from arbitrary
# key locations (i.e. first to last, last to first, etc.)
#
# Currently only support AES-128 and AES-256
#


sBox = 8*[64*[0]]

sBox[0] = (14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
            0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
            4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
           15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13)

sBox[1] = (15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
            3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
            0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
           13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9)

sBox[2] = (10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
           13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
           13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
            1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12)

sBox[3] = ( 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
           13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
           10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
            3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14)

sBox[4] = ( 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
           14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
            4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
           11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3)

sBox[5] = (12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
           10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
            9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
            4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13)

sBox[6] = ( 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
           13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
            1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
            6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12)

sBox[7] = (13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
            1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
            7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
            2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11)




def invsbox(inp):
    s = [0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3,
            0x9e, 0x81, 0xf3, 0xd7, 0xfb , 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f,
            0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb , 0x54,
            0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b,
            0x42, 0xfa, 0xc3, 0x4e , 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24,
            0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25 , 0x72, 0xf8,
            0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d,
            0x65, 0xb6, 0x92 , 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
            0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84 , 0x90, 0xd8, 0xab,
            0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3,
            0x45, 0x06 , 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1,
            0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b , 0x3a, 0x91, 0x11, 0x41,
            0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6,
            0x73 , 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9,
            0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e , 0x47, 0xf1, 0x1a, 0x71, 0x1d,
            0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b ,
            0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0,
            0xfe, 0x78, 0xcd, 0x5a, 0xf4 , 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07,
            0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f , 0x60,
            0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f,
            0x93, 0xc9, 0x9c, 0xef , 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5,
            0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61 , 0x17, 0x2b,
            0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55,
            0x21, 0x0c, 0x7d]
    return s[inp]


rcon = [0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
            0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97,
            0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72,
            0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66,
            0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
            0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
            0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
            0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61,
            0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
            0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
            0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc,
            0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5,
            0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a,
            0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d,
            0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c,
            0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
            0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4,
            0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
            0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08,
            0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
            0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d,
            0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2,
            0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74,
            0xe8, 0xcb ]


def g_func(inp, rcon):
    #Step 1: change order
    newlist = [inp[1], inp[2], inp[3], inp[0]]

    #Step 2: s-box
    newlist = [sbox(t) for t in newlist]

    #Step 3: apply rcon
    newlist[0] ^= rcon

    return newlist


def h_func(inp):
    #Step 1: s-box
    newlist = [sbox(t) for t in inp]

    return newlist


def xor(l1, l2):
    return [l1[i] ^ l2[i] for i in range(0, len(l1))]


# Permutation and translation tables for DES
__pc1 = [56, 48, 40, 32, 24, 16,  8,
      0, 57, 49, 41, 33, 25, 17,
      9,  1, 58, 50, 42, 34, 26,
     18, 10,  2, 59, 51, 43, 35,
     62, 54, 46, 38, 30, 22, 14,
      6, 61, 53, 45, 37, 29, 21,
     13,  5, 60, 52, 44, 36, 28,
     20, 12,  4, 27, 19, 11,  3
]

# permuted choice key (table 2)
__pc2 = [
    13, 16, 10, 23,  0,  4,
     2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7,
    15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54,
    29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,
    45, 41, 49, 35, 28, 31
]

# number left rotations of pc1
__left_rotations = [
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
]

def __String_to_BitList(self, data):
    """Turn the string data, into a list of bits (1, 0)'s"""

    data = [ord(c) for c in data]
    l = len(data) * 8
    result = [0] * l
    pos = 0
    for ch in data:
        i = 7
        while i >= 0:
            if ch & (1 << i) != 0:
                result[pos] = 1
            else:
                result[pos] = 0
            pos += 1
            i -= 1

    return result

def __permutate(self, table, block):
    """Permutate this block with the specified table"""
    return list(map(lambda x: block[x], table))

def keyScheduleRounds(inputkey, inputround, desiredround):
    """Create the 16 subkeys K[1] to K[16] from the given key"""
    key = __permutate(__pc1, __String_to_BitList(inputkey))
    i = 0
    # Split into Left and Right sections
    L = key[:28]
    R = key[28:]
    Kn = [ [0] * 48 ] * 16
    while i < desiredround:
        j = 0
        # Perform circular left shifts
        while j < __left_rotations[i]:
            L.append(L[0])
            del L[0]

            R.append(R[0])
            del R[0]

            j += 1

        # Create one of the 16 subkeys through pc2 permutation
        Kn[i] = __permutate(__pc2, L + R)

        i += 1
    return Kn[i-1]

def keyScheduleRounds(inputkey, inputround, desiredround):
    """
    inputkey = starting key, 16/32 bytes
    inputround = starting round number (i.e. 0 = first)
    desiredround = desired round number (i.e. 10 = last for 16-byte)

    returns desired round number. Can go forward or backwards.

    When dealing with AES-256, inputkey is 16 bytes and inputround
    indicates round that bytes 0...15 are from.
    """

    #Some constants
    n = len(inputkey)
    if n == 16:
        pass
    elif n == 32:
        desiredfull = desiredround
        desiredround = int(desiredround / 2)

        #Special case for inputround of 13, needed for 'final' round...
        if inputround != 13:
            if inputround % 2 == 1:
                raise ValueError("Input round must be divisible by 2")
            inputround = int(inputround / 2)
        else:
            if inputround <= desiredfull:
                if desiredfull < 13:
                    raise ValueError("Round = 13 only permissible for reverse")

                if desiredfull == 13:
                    return inputkey[0:16]
                else:
                    return inputkey[16:32]

    else:
        raise ValueError("Invalid keylength: %d"%n)

    rnd = inputround
    state = list(inputkey)

    #Check if we are going forward or backwards
    while rnd < desiredround:
        rnd += 1

        #Forward algorithm, thus need at least one round
        state[0:4] = xor(state[0:4], g_func(state[(n-4):n], rcon[rnd]))
        for i in range(4, n, 4):
            if n == 32 and i == 16:
                inp = h_func(state[(i-4):i])
            else:
                inp = state[(i - 4):i]
            state[i:(i+4)] = xor(state[i:(i+4)], inp)

    while rnd > desiredround:
        #For AES-256 final-round is 13 as that includes 32 bytes
        #of key. Convert to round 12 then continue as usual...
        if n == 32 and rnd == 13:
            inputrnd = int(12/2)
            rnd = inputrnd
            oldstate = list(state[16:32])
            state[16:32] = state[0:16]

            for i in range(12, 0, -4):
                state[i:(i+4)] = xor(oldstate[i:(i+4)], oldstate[(i-4):i])
            state[0:4] = xor(oldstate[0:4], g_func(state[(n - 4):n], rcon[7]))

        if rnd == desiredround:
            break

        # Reverse algorithm, thus need at least one round
        for i in range(n-4, 0, -4):
            if n == 32 and i == 16:
                inp = h_func(state[(i-4):i])
            else:
                inp = state[(i - 4):i]
            state[i:(i+4)] = xor(state[i:(i+4)], inp)
        state[0:4] = xor(state[0:4], g_func(state[(n - 4):n], rcon[rnd]))
        rnd -= 1

    #For AES-256, we use half the generated key at once...
    if n == 32:
        if desiredfull % 2:
            state = state[16:32]
        else:
            state = state[0:16]

    #Return answer
    return state


def test():
    #Manual tests right now - need to automate this.

    ##### AES-128 Tests
    print "**********AES-128 Tests***************"

    ik = [0]*16
    for i in range(0, 11):
        result = keyScheduleRounds(ik, 0, i)
        print " ".join(["%2x"%d for d in result])
        ok = result

    # 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    # 62 63 63 63 62 63 63 63 62 63 63 63 62 63 63 63
    # 9b 98 98 c9 f9 fb fb aa 9b 98 98 c9 f9 fb fb aa
    # 90 97 34 50 69 6c cf fa f2 f4 57 33 0b 0f ac 99
    # ee 06 da 7b 87 6a 15 81 75 9e 42 b2 7e 91 ee 2b
    # 7f 2e 2b 88 f8 44 3e 09 8d da 7c bb f3 4b 92 90
    # ec 61 4b 85 14 25 75 8c 99 ff 09 37 6a b4 9b a7
    # 21 75 17 87 35 50 62 0b ac af 6b 3c c6 1b f0 9b
    # 0e f9 03 33 3b a9 61 38 97 06 0a 04 51 1d fa 9f
    # b1 d4 d8 e2 8a 7d b9 da 1d 7b b3 de 4c 66 49 41
    #b4 ef 5b cb 3e 92 e2 11 23 e9 51 cf 6f 8f 18 8e

    print ""

    for i in range(0, 11):  # 10 Rounds
        result = keyScheduleRounds(ok, 10, i)
        print " ".join(["%2x" % d for d in result])

    ##### AES-256 Tests
    print "**********AES-256 Tests***************"

    ik = [0]*32
    for i in range(0, 15):  # 14 Rounds
        result = keyScheduleRounds(ik, 0, i)
        print " ".join(["%02x"%d for d in result])

    # 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    # 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    # 62 63 63 63 62 63 63 63 62 63 63 63 62 63 63 63
    # aa fb fb fb aa fb fb fb aa fb fb fb aa fb fb fb
    # 6f 6c 6c cf 0d 0f 0f ac 6f 6c 6c cf 0d 0f 0f ac
    # 7d 8d 8d 6a d7 76 76 91 7d 8d 8d 6a d7 76 76 91
    # 53 54 ed c1 5e 5b e2 6d 31 37 8e a2 3c 38 81 0e
    # 96 8a 81 c1 41 fc f7 50 3c 71 7a 3a eb 07 0c ab
    # 9e aa 8f 28 c0 f1 6d 45 f1 c6 e3 e7 cd fe 62 e9
    # 2b 31 2b df 6a cd dc 8f 56 bc a6 b5 bd bb aa 1e
    # 64 06 fd 52 a4 f7 90 17 55 31 73 f0 98 cf 11 19
    # 6d bb a9 0b 07 76 75 84 51 ca d3 31 ec 71 79 2f
    # e7 b0 e8 9c 43 47 78 8b 16 76 0b 7b 8e b9 1a 62
    # 74 ed 0b a1 73 9b 7e 25 22 51 ad 14 ce 20 d4 3b
    #10 f8 0a 17 53 bf 72 9c 45 c9 79 e7 cb 70 63 85

    print ""

    ik = [0x74 ,0xed ,0x0b ,0xa1 ,0x73 ,0x9b ,0x7e ,0x25 ,0x22 ,
          0x51 ,0xad ,0x14 ,0xce ,0x20 ,0xd4 ,0x3b ,0x10 ,0xf8 ,
          0x0a ,0x17 ,0x53 ,0xbf ,0x72 ,0x9c ,0x45 ,0xc9 ,0x79 ,
          0xe7 ,0xcb ,0x70 ,0x63, 0x85]

    for i in range(0, 14):
        result = keyScheduleRounds(ik, 13, i)
        print " ".join(["%2x"%d for d in result])

if __name__ == "__main__":
    test()
