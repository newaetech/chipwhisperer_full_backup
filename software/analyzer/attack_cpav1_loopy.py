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

class attack_CPAAESv1():

    def __init__(self):
        self.setOptions(None, range(0,16))

    def getOptions(self):
        #Type, Label/Name, Values
        return {
         "Model": {"Type": 'CB', "Opts": ['Hamming Weight', 'Hamming Distance']},
         "Key Round": {"Type": 'CB', "Opts": ['First', 'Last']}
        }

    def setOptions(self, encodedopts, brange):
        self.modeltype="Hamming Weight"
        self.keyround="First"
        self.brange=brange
        if encodedopts != None:
            self.modeltype = encodedopts['Model']
            self.keyround = encodedopts['Key Round']
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange
                                                                   
        traces_all = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        numtraces = len(traces_all[:,0])

        #For all bytes of key
        for bnum in brange:

            diffs = [0]*256

            if pointRange == None:
                traces = traces_all
                padbefore = 0
                padafter = 0
            else:
                traces = np.array(traces_all[:, pointRange[bnum][0] : pointRange[bnum][1]])
                padbefore = pointRange[bnum][0]
                padafter = len(traces_all[0,:]) - pointRange[bnum][1]
                #print "%d - %d (%d %d)"%( pointRange[bnum][0],  pointRange[bnum][1], padbefore, padafter)

            #For each 0..0xFF possible value of the key byte
            for key in range(0, 256):                
                #Initialize arrays & variables to zero
                sumnum = np.zeros(len(traces[0,:]))
                sumden1 = np.zeros(len(traces[0,:]))
                sumden2 = np.zeros(len(traces[0,:]))

                hyp = [0] * numtraces

                #Formula for CPA & description found in "Power Analysis Attacks"
                # by Mangard et al, page 124, formula 6.2.                         
                
                #Generate hypotheticals
                for tnum in range(numtraces):

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
                for tnum in range(numtraces):
                    hdiff = (hyp[tnum] - meanh)
                    tdiff = traces[tnum,:] - meant

                    sumnum = sumnum + (hdiff*tdiff)
                    sumden1 = sumden1 + hdiff*hdiff 
                    sumden2 = sumden2 + tdiff*tdiff             

                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        break

                diffs[key] = sumnum / np.sqrt( sumden1 * sumden2 )

                if padafter > 0:
                    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

                if padbefore > 0:
                    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])                    
            
            self.all_diffs[bnum] = diffs

            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
