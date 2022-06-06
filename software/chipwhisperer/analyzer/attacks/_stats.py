#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
#=================================================

import numpy as np
from chipwhisperer.common.utils.util import camel_case_deprecated
from collections import OrderedDict


class Results(object):
    """
    Results type used for attacks generating peaks indicating the 'best' success. Examples include
    standard DPA & CPA attacks.
    """

    def __init__(self, numSubkeys=16, numPerms=256):
        self.numSubkeys = numSubkeys
        self.numPerms = numPerms
        self.known_key = None
        self.clear()

    def key_guess(self):
        """ Get best guess for key in a list.

        Returns:
            Guessed key as a list
        """
        best_guesses = self.best_guesses()
        return [byte['guess'] for byte in best_guesses]

    def best_guesses(self):
        """ Gets best subkey guesses from attack results

        Returns:
            List of OrderedDicts with keys 'guess', 'correlation' and 'pge'.
        """
        guess_list = []
        stats = self.find_maximums()
        for i, subkey in enumerate(stats):
            guess = {}
            guess['guess'] = subkey[0][0]
            guess['correlation'] = subkey[0][2]
            guess['pge'] = self.simple_PGE(i)
            guess_list.append(guess)

        return guess_list

    def __str__(self):
        ret = ""
        ret += "Subkey KGuess Correlation\n"
        guesses = self.best_guesses()
        for i,subkey in enumerate(guesses):
            ret += "  {:02d}    0x{:02X}    {:7.5f}\n".format(i, subkey['guess'], subkey['correlation'])
        return ret

    def clear(self):
        #Diffs from CPA/DPA Attack
        self.diffs = [None]*self.numSubkeys

        #Maximum diff & location of maximum
        self.maxes = [0]*self.numSubkeys
        for i in range(0, self.numSubkeys):
            self.maxes[i] = np.zeros(self.numPerms, dtype=[('hyp', 'i2'), ('point', 'i4'), ('value', 'f8')])

        #If maximum diffs are valid & sorted correctly
        self.maxValid = [False]*self.numSubkeys
        self.pge = [255]*self.numSubkeys
        self.diffs_tnum = [None]*self.numSubkeys
        self.pge_total = []
        self.maxes_list = [list() for i in range(0, self.numSubkeys)]

        #TODO: Ensure this gets called by attack algorithms when rerunning

    def calc_PGE(self, bnum):
        if self.known_key is None:
            raise ValueError("Set result.known_key before running this method!")
        result = self.find_maximums()[bnum]
        for i in range(len(result)):
            if self.known_key[bnum] == result[i][0]:
                return i

    def simple_PGE(self, bnum):
        """Returns the partial guessing entropy of subkey."""
        if self.pge[bnum] == 255:
            # actually have to do the calculation
            if self.known_key is None:
                return 255
            return self.calc_PGE(bnum)
        if self.maxValid[bnum] == False:
            #TODO: should sort
            return 1
        return self.pge[bnum]

    simplePGE = camel_case_deprecated(simple_PGE)

    def set_known_key(self, known_key):
        """Sets the known encryption key."""
        self.known_key = known_key

    setKnownkey = camel_case_deprecated(set_known_key)

    def update_subkey(self, bnum, data, copy=True, force_update=False, tnum=None):
        """Update the specific subkey.

        Args:
            bnum (int): The index of the subkey.
            data (int): The new subkey byte.
            copy (int):
        """
        if (id(data) != id(self.diffs[bnum])) or force_update:
            self.maxValid[bnum] = False

            if data is not None and copy:
                self.diffs[bnum] = data[:]
                self.diffs_tnum[bnum] = tnum
            else:
                self.diffs[bnum] = data
                self.diffs_tnum[bnum] = tnum

    updateSubkey = camel_case_deprecated(update_subkey)

    def find_key(self, use_absolute=True):
        """ Find the best guess for the key from the attack.

        Args:
            use_absolute (bool, optional): Use the absolute value of the
                correlation during analysis.

        Returns:
            The best guess for a key from the attack as a list.
        """
        res = self.find_maximums(use_absolute=use_absolute)
        return [subkey[0][0] for subkey in res]

    def find_maximums(self, bytelist=None, use_absolute=True, use_single=False):
        """Information from the attack:

        Args:
            bytelist (list): Iterable of subkeys to compute and organize results
                for.
            use_absolute (bool): Use absolute value of correlation to find highest
                correlation.
            use_single (bool): All table values are taken from the same point the
                maximum is taken from.


        Returns:
            list: Ordered by subkey index::

                [subkey0_data, subkey1_data, subkey2_data, ...]

            *subkey0_data* is another list containing guesses ordered by strength
            of correlation::

                [guess0, guess1, guess2, ...]

            *guess0* is a tuple containing::

                (key_guess, location_of_max, correlation)

        For example, if you want to print the correlation of the best guess
        of the 4th subkey, you would run::

            print(attack_results.find_maximums()[4][0][2])

        Note the "point location of the max" is normally not calculated/tracked,
        and thus returns as a 0.
        """
        if bytelist is None:
            bytelist = list(range(0, self.numSubkeys))

        # print useAbsolute

        for i in bytelist:
            if self.diffs[i] is None:
                self.maxValid[i] = False
                continue

            if self.maxValid[i] == False:
                for hyp in range(0, self.numPerms):
                    if use_absolute:
                        v = np.nanmax(np.fabs(self.diffs[i][hyp]))
                    else:
                        v = np.nanmax(self.diffs[i][hyp])

                    mvalue = v

                    #Get maximum value for this hypothesis
                    mindex = np.nanargmax(np.fabs(self.diffs[i][hyp]))
                    self.maxes[i][hyp]['hyp'] = hyp
                    self.maxes[i][hyp]['point'] = mindex
                    self.maxes[i][hyp]['value'] = mvalue

                #TODO: why does this fail?
                #self.maxes[i][np.isnan(self.maxes[i]['value'])]['value'] = 0
                #TODO: workaround for PGE, as NaN's get ranked first
                numnans = np.isnan(self.maxes[i]['value']).sum()

                if use_single:
                    #All table values are taken from same point MAX is taken from
                    where = self.maxes[i][0]['point']
                    for j in range(0, self.numPerms):
                        self.maxes[i][j]['point'] = where
                        self.maxes[i][j]['value'] = self.diffs[i][self.maxes[i][j]['hyp']][where]

                self.maxes[i][::-1].sort(order='value') # sorts nunpy array in place and in reverse order
                self.maxValid[i] = True

                if self.known_key is not None:
                    try:
                        self.pge[i] = np.where(self.maxes[i]['hyp'] == self.known_key[i])[0][0] - numnans
                        # print(self.pge)
                        if self.pge[i] < 0:
                            self.pge[i] = self.numPerms/2
                    except IndexError as e:
                        print("AHHH " + str(e))
                        self.pge[i] = self.numPerms-1

            tnum = self.diffs_tnum[i]
            self.pge_total.append({'trace':tnum, 'subkey':i, 'pge':self.pge[i]})

            if len(self.maxes_list[i]) == 0 or self.maxes_list[i][-1]['trace'] != tnum:
                self.maxes_list[i].append({'trace':tnum, 'maxes':np.array(self.maxes[i])})

        return self.maxes

    findMaximums = camel_case_deprecated(find_maximums)
