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

algo = "original"
#algo = "log"

class attack_BAYCPAAES():

    def getOptions(self):
        #Type, Label/Name, Values
        return {
         "Model": {"Type": 'CB', "Opts": ['Hamming Weight', 'Hamming Distance']},
         "Key Round": {"Type": 'CB', "Opts": ['First', 'Last']}
        }   
    
    def doDPA(self, brange, traces, plaintexts, ciphertexts, progressBar=None, modeltype="Hamming Weight", keyround="First", encodedopts=None):
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
                sumstd = np.zeros(len(traces[0,:]), dtype=np.float64)
                hyp = [0] * len(traces[:,0])           
                
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

                #Mean & STD-DEV of hypothesis
                meanh = np.mean(hyp, dtype=np.float64)
                stddevh = np.std(hyp, dtype=np.float64)

                #Mean & STD-DEV of all points in trace
                meant = np.mean(traces, axis=0, dtype=np.float64)
                stddevt = np.std(traces, axis=0, dtype=np.float64)
                                   
                #For each trace, do the following
                for tnum in range(len(traces[:,0])):
                    #Make both zero-mean
                    hdiff = np.float64(hyp[tnum]) - meanh
                    tdiff = np.array((np.array(traces[tnum,:], dtype=np.float64) - meant), dtype=np.float64)

                    #Scale by std-dev
                    hdiff = hdiff / np.float64(stddevh)
                    tdiff = tdiff / np.array(stddevt, dtype=np.float64)

                    #Calculate error term                   
                    error = (hdiff - tdiff)
                    sumstd = sumstd + pow(error, 2)

                q = len(traces[:,0])

                if algo == "original":
                    #Original Algorithm
                    sumstd = pow(np.sqrt((sumstd/q)), -q)
                elif algo == "log":
                    #LOG Algorithm
                    sumstd = -q * ( (0.5*np.log(sumstd)) - np.log(q))
                else:
                    raise RuntimeError("algo not defined")

                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        break

                diffs[key] = sumstd

            #Gotten all stddevs - now process algorithm

            #Original algorithm
            if algo == "original":
                sums = np.sum(diffs, axis=0, dtype=np.float64)
                for key in range(0, 256):
                    diffs[key] = diffs[key] / sums

            elif algo == "log":
            #Logarithm Algorithm
                summation = np.zeros(len(traces[0,:]), dtype=np.float64)
                for key in range(1, 256):
                    diff = diffs[key] - diffs[0]
                    summation = summation + np.exp(diff)
                summation = summation + 1
                summation = np.log(summation)
                summation = summation + diffs[0]
                           
                for key in range(0, 256):
                    diffs[key] = diffs[key] - summation

            else:
                raise RuntimeError("algo not defined")
                            
            self.all_diffs[bnum] = diffs

            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,


    def getResult(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)

        res = [self.all_diffs[bnum][i] for i in hyprange]
        return res

    def getDiff(self, bnum, hyprange=None):
        if algo == "original":
            return self.getResult(bnum, hyprange)
        elif algo == "log":
            return np.exp(self.getResult(bnum, hyprange))
        else:
            raise RuntimeError("algo not defined")

    def getLog(self, bnum, hyprange=None):
        if algo == "original":
            raise TypeError("Don't call getlog without using log algorithm")
        elif algo == "log":
            return self.getResult(bnum, hyprange)
        else:
            raise RuntimeError("algo not defined") 
