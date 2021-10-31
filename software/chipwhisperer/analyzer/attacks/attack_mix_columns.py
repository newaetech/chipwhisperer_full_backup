import logging
try:
    import scared # type: ignore
    import estraces # type: ignore
except:
    raise ImportError("Could not import scared. Scared is currently required for this attack")

import numpy as np
from tqdm.autonotebook import trange # type: ignore
from typing import List

gal1=np.array(range(0,256), dtype='uint8')
gal2=np.array((
0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5), dtype='uint8')

gal3=np.array((
0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a), dtype='uint8')

w=0x00
lut_input_col = [[0, 1, 2, 3],
        [4, 5, 6, 7],
        [8, 9, 10, 11],
        [12, 13, 14, 15]]

lut_input_row = np.transpose(lut_input_col)

def inc_vec(x):
    for i in range(4):
        r = range(4*i, 4*i + 4)
        if x in r:
            x += 1
            if not (x in r):
                x = r[0]

    return x

def wrap_inc(x):
    if x >= 3:
        x = 0
    else:
        x += 1
    return x

base_lut_mix_column_col = [[0, 13, 10, 7],
                     [4, 1, 14, 11],
                     [8, 5, 2, 15],
                     [12, 9, 6, 3]] # lut to find which pt to xor with mixcolumn output

base_lut_mix_column_row = np.transpose(base_lut_mix_column_col) # lut to find which pt to xor with mixcolumn output

# generate luts for figuring out which pt input needs to be xor'd with the mixcol output for HD
import copy
lut_mix_column_col = [copy.deepcopy(base_lut_mix_column_col)]
lut_mix_column_row = [copy.deepcopy(base_lut_mix_column_row)]
for i in range(3):
    tmp = copy.deepcopy(lut_mix_column_col[i])
    tmp2 = copy.deepcopy(lut_mix_column_row[i])
    for j in range(4):
        for k in range(4):
            tmp[j][k] = inc_vec(tmp[j][k])
            tmp2[j][k] = inc_vec(tmp2[j][k])

    lut_mix_column_col.append(tmp)
    lut_mix_column_row.append(tmp2)


# scared leakage generator
# 

gal_lut = [gal2, gal3, gal1, gal1]
def round_gen(plaintext, guesses, cmpgn, lut_in, lut_out, hd, gal):
    global gal_lut
    res = np.empty((plaintext.shape[0], len(guesses), plaintext.shape[1]), dtype='uint8')
    if hd:
        new_pt = np.repeat(plaintext[:,lut_out[cmpgn][0]][:, np.newaxis], 16, axis=1)
        #print(np.shape(new_pt))
        new_pt = np.zeros(np.shape(new_pt), dtype='uint8')
        #print(np.shape(new_pt))
        new_pt[:,lut_in[cmpgn][0]] = plaintext[:,lut_out[cmpgn][0]]
        new_pt[:,lut_in[cmpgn][1]] = plaintext[:,lut_out[cmpgn][1]]
        new_pt[:,lut_in[cmpgn][2]] = plaintext[:,lut_out[cmpgn][2]]
        new_pt[:,lut_in[cmpgn][3]] = plaintext[:,lut_out[cmpgn][3]]
    else:
        new_pt = 0
    for i, guess in enumerate(guesses):
        res[:,i,:] = np.bitwise_xor(new_pt, gal_lut[gal][scared.aes.sub_bytes(np.bitwise_xor(plaintext, guess))])
    return res

# leakage_cmpgns_col : List[]= []
# leakage_cmpgns_row = []

class AttackMixColumns:
    """ Class for attacking after MixColumns using a variable vector plaintext

    Will also successfully attack ARK1.

    Please see https://eprint.iacr.org/2016/249.pdf for details behind the attack.

    Runs 32 single bit CPA attacks on the mix columns outputs for a given pt/key byte
    (i.e. all bits of 2sbox(pt^key), sbox(pt^key), 3sbox(pt^key)). If using hamming
    distance, assumes a leakage between ARK0 and MixColumns/ARK1

    Requires capture using cw.ktp.VarVec or equivalent. vec_type must
    be the same as was used during the capture campaigns.

    Args:
        project (ChipWhisperer Projects): List of 4 projects for each capture campaign.
                                            To skip analysis for a project, set as None in the list
        vec_type (str): 'column' for variable column, or 'row' for variable row. Must be same as was used
                        in the capture campaign. Defaults to column
        hd (bool): Use hamming distance between pt input and mix columns/ARK output. Otherwise use hamming weight of mix columns/ARK output

    Example hd attack::

        # assuming projects captured as in cw.ktp.VarVec documentation
        from chipwhisperer.analyzer.attacks.attack_mix_columns import AttackMixColumns
        attack = AttackMixColumns(projects, hd=False)
        results = attack.run()
        print(bytearray(results['guess']))
    """
    def __init__(self, projects, vec_type='column', hd=True):
        if vec_type not in ["column", "row"]:
            raise ValueError(f"Invalid vector type {vec_type}")
        if vec_type == 'column':
            lut_in = lut_input_col
            lut_out = lut_mix_column_col
        else:
            lut_in = lut_input_row
            lut_out = lut_mix_column_row

        leakage_cmpgns = []
        for campaign in range(4):
            tmp=[]
            def make_leakage(j, campaign):
                return lambda plaintext, guesses: round_gen(plaintext, guesses, campaign, lut_in, lut_out[j], hd, j)
            for j in range(4):
                tmp.append(\
                    scared.attack_selection_function(make_leakage(j, campaign)))
            leakage_cmpgns.append(tmp)
        self.projects = projects
        self.leakage_cmpgns = leakage_cmpgns
        self.vec_type = vec_type

    def run(self, n_traces=None, trace_slice=None):
        """ Run attack on projects

        Args:
            n_traces (int): Number of traces to use. Leave as None if you want to use the full trace set. Defaults to None
            trace_slice (slice): Slice of trace wave to use. Useful for windowing, since ghost peaks can be a problem. If None,
                                use full trace. Defaults to None.

        Returns:
            Dict with {'corr': correlation_data, '}
        """
        results = {}
        projects = self.projects
        cs = [None] * 16
        key_guess = [None] * 16
        if trace_slice is None:
            trace_slice = slice(None, None)
        for campaign in trange(4):
            b = None
            if projects[campaign] is None:
                continue
            cw_traces = estraces.read_ths_from_ram(np.array(projects[campaign].waves)[:n_traces, trace_slice],
                        plaintext=np.array([textin for textin in projects[campaign].textins], dtype='uint8')[:n_traces])
            for t in trange(4, leave=False):
                for i in trange(8, leave=False):
                    container = scared.Container(cw_traces)
                    a = scared.CPAAttack(selection_function=self.leakage_cmpgns[campaign][t], 
                                        model=scared.Monobit(i), discriminant=scared.maxabs)
                    a.run(container)
                    if b is None:
                        b = abs(a.results)
                    else:
                        b += abs(a.results)
            r = None
            if self.vec_type == "column":
                r = range(0+4*campaign, 4+4*campaign)
            else:
                r = range(campaign, 16, 4)
            for i in r:
                c = np.nan_to_num(b[:,i,:])
                cs[i] = c
                maxes = np.max(c, axis=1)
                guess = np.argsort(maxes)[-1]
                key_guess[i] = guess
        results["corr"] = cs
        results["guess"] = key_guess
        return results

    def run_alt(self, n_traces=None, trace_slice=None, campaign=0):
        results = {}
        projects = self.projects
        cs = [None] * 4
        key_guess = [0] * 16
        if trace_slice is None:
            trace_slice = slice(None, None)
        b = None
        cw_traces = estraces.read_ths_from_ram(np.array(projects[campaign].waves)[:n_traces, trace_slice],
                    plaintext=np.array([textin for textin in projects[campaign].textins], dtype='uint8')[:n_traces])
        for t in trange(4, leave=False):
            for i in trange(8, leave=False):
                container = scared.Container(cw_traces)
                a = scared.CPAAttack(selection_function=self.leakage_cmpgns[campaign][t], 
                                    model=scared.Monobit(i), discriminant=scared.maxabs)
                a.run(container)
                if b is None:
                    b = abs(a.results)
                else:
                    b += abs(a.results)
        r = None
        if self.vec_type == "column":
            r = range(0+4*campaign, 4+4*campaign)
        else:
            r = range(campaign, 16, 4)
        x = 0
        for i in r:
            c = np.nan_to_num(b[:,i,:])
            cs[x] = c
            x += 1
            maxes = np.max(c, axis=1)
            guess = np.argsort(maxes)[-1]
            key_guess[i] = guess

        # get byte with highest correlation
        cs = np.array(cs)
        maxes = np.max(cs, axis=1)
        maxes2 = np.max(maxes, axis=1)
        best_byte = np.argmax(maxes2)

        # get guess with best correlation
        maxes = np.max(cs[best_byte], axis=1)
        guess = np.argsort(maxes)[-1]

        # select best point to do analysis
        best_point = np.argmax(cs[best_byte][guess])


        corr_signs = np.zeros([4, 4, 8], dtype='uint8')

        for t in trange(4, leave=False):
            for i in trange(8, leave=False):
                container = scared.Container(cw_traces)
                a = scared.CPAAttack(selection_function=self.leakage_cmpgns[campaign][t], 
                                    model=scared.Monobit(i), discriminant=scared.maxabs)
                a.run(container)
                if self.vec_type == "column":
                    r = range(0+4*campaign, 4+4*campaign)
                else:
                    r = range(campaign, 16, 4)
                x = 0
                for key_byte in r:
                    # if > 0, correlation flipped by constant XOR
                    corr_signs[x, t, i] = a.results[key_guess[key_byte], key_byte, best_point] > 0
                    x += 1

                print("")
        results["corr"] = cs
        results["guess"] = key_guess
        return results, corr_signs


def test_lut():
    assert lut_mix_column_col[0][0] == 0


# if __name__ == "__main__":
#     #print(lut_mix_column_col)
#     for i in lut_mix_column_col:
#         print(i)

#         print("--------")

#     for i in lut_mix_column_row:
#         print(i)

#         print("--------")