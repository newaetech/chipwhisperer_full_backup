#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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

import numpy as np
import attack_aesmodel as model
import sys

sys.path.append('../common')
import aes_tables
import aes_cipher

class attack_CPAAESit():

    def getOptions(self):
        #Type, Label/Name, Values
        return {
         "Model": {"Type": 'CB', "Opts": ['Hamming Weight', 'Hamming Distance']},
         "Key Round": {"Type": 'CB', "Opts": ['First', 'Last']}
        }


    def findHighestHyp(self):
        #Find most likely byte
        mostlikely = []
        for bnum in range(0,16):
            diffs = self.getDiff(bnum)             
            maxes = np.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

            for hyp in range(0, 256):
                #Get maximum value for this hypothesis
                mvalue = np.amax(np.fabs(diffs[hyp]))
                mindex = np.amin(np.where(np.fabs(diffs[hyp]) == mvalue))
                maxes[hyp] = (hyp,mindex,mvalue)

            maxes.sort(order='value')
            maxes = maxes[::-1]
            mostlikely.append(maxes[0]['hyp'])

        return mostlikely

    def prepareNextRoundPT(self):        
        #Process S-Boxes
        newstate = aes._sub_bytes(state)

        #Process shift
        newstate = aes._shift_rows(state)
        newprob = aes._shift_rows(prob)
        

    def doDPA(self, brange, traces, plaintexts, ciphertexts, progressBar=None, modeltype="Hamming Weight", keyround="First", encodedopts=None):
        #Do one DPA Attack
        self.doDPAOne(brange, traces, plaintexts, ciphertexts, progressBar, modeltype, keyround, encodedopts)

        aes = aes_cipher.AESCipher([1])

        #Get keys
        newkey = self.findHighestHyp()

        print("Round 0 Keys: "),
        print ['%02x' % val for val in newkey]

        np = [[0] * 16] * len(plaintexts)

        for trace_index, t in enumerate(plaintexts):
            for byte_index, b in enumerate(t):
                if trace_index == 0:
                    np[trace_index][byte_index] = 0xFF ^ newkey[byte_index]
                else:
                    np[trace_index][byte_index] = b ^ newkey[byte_index]
            state = list(np[trace_index])
            #if trace_index == 0:
            #    print ['%02x' % val for val in state]
            aes._sub_bytes(state)
            #if trace_index == 0:
            #    print ['%02x' % val for val in state]
            aes._shift_rows(state)
            #if trace_index == 0:
            #    print ['%02x' % val for val in state]
            aes._mix_columns(state, False)
            #if trace_index == 0:
            #    print ['%02x' % val for val in state]
            np[trace_index] = list(state)


        print ['%02x' % val for val in np[0]]
        self.doDPAOne(brange, traces, np, [], progressBar, modeltype, keyround="First")

        newkey = self.findHighestHyp()

        print("Round 1 Keys: "),
        print ['%02x' % val for val in newkey]
            
    
    def doDPAOne(self, brange, traces, plaintexts, ciphertexts, progressBar=None, modeltype="Hamming Weight", keyround="First", encodedopts=None):
        #If encoded options present parse them & overwrite
        if encodedopts != None:
            modeltype = encodedopts['Model']
            keyround = encodedopts['Key Round']
                                                                   
        traces = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        #For all bytes of key
        for bnum in brange:

            diffs = [0]*256

            #For each 0..0xFF possible value of the key byte
            for key in range(0, 256):                
                #Initialize arrays & variables to zero
                sumnum = np.zeros(len(traces[0,:]))
                sumden1 = np.zeros(len(traces[0,:]))
                sumden2 = np.zeros(len(traces[0,:]))

                hyp = [0] * len(traces[:,0])

                #Formula for CPA & description found in "Power Analysis Attacks"
                # by Mangard et al, page 124, formula 6.2.                         
                
                #Generate hypotheticals
                for tnum in range(len(traces[:,0])):

                    if len(plaintexts) > 0:
                        pt = plaintexts[tnum]

                    if len(ciphertexts) > 0:
                        ct = ciphertexts[tnum]

                    if keyround == "First":
                        ct = None
                    elif keyround == "Last":
                        pt = None
                    else:
                        raise ValueError("keyround invalid")
                    
                    #Generate the output of the SBOX
                    if modeltype == "Hamming Weight":
                        hypint = model.HypHW(pt, ct, key, bnum);
                    elif modeltype == "Hamming Distance":
                        hypint = model.HypHD(pt, ct, key, bnum);
                    else:
                        raise ValueError("modeltype invalid")
                    hyp[tnum] = model.getHW(hypint)
                    
                hyp = np.array(hyp)                    

                #Mean of hypothesis
                meanh = np.mean(hyp, dtype=np.float64)

                #Mean of all points in trace
                meant = np.mean(traces, axis=0, dtype=np.float64)
                                   
                #For each trace, do the following
                for tnum in range(len(traces[:,0])):
                    hdiff = (hyp[tnum] - meanh)
                    tdiff = traces[tnum,:] - meant

                    sumnum = sumnum + (hdiff*tdiff)
                    sumden1 = sumden1 + pow(hdiff, 2)
                    sumden2 = sumden2 + pow(tdiff, 2)               

                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        break

                diffs[key] = sumnum / np.sqrt( sumden1 * sumden2 )
            
            self.all_diffs[bnum] = diffs

            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
