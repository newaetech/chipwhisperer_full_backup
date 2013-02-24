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

### WARNING THIS FILE IS INCOMPLETE

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

    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange
                                                                   
        traces = np.array(traces)
        numpoints = len(traces[0,:])
        numtraces = len(traces[:,0])
        all_diffs = np.zeros([16, 256, numpoints], dtype='double')

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        hyp = np.zeros(numtraces, dtype='u1')
        
        #For all bytes of key
        for bnum in brange:

            #For each 0..0xFF possible value of the key byte
            for key in xrange(0, 256):                
                #Formula for CPA & description found in "Power Analysis Attacks"
                # by Mangard et al, page 124, formula 6.2.                         
                
                #Generate hypotheticals
                for tnum in xrange(len(traces[:,0])):

                    if len(plaintexts) > 0:
                        pt = plaintexts[tnum]
                    else:
                        pt = None

                    if len(ciphertexts) > 0:
                        ct = ciphertexts[tnum]
                    else:
                        ct = None

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

                #Mean of hypothesis
                self.sumh = self.sumh + np.sum(hyp, dtype='double')

                #Mean of all points in trace
                self.sumt = self.sumt + np.sum(traces, axis=0, dtype='double')

                meanh = self.sumh / self.totaltraces
                meant = self.sumt / self.totaltraces

                hdiff = hyp - meanh
                tdiff = traces - meant
                                   
                #For each trace, calculate num/denom
                sumnum = np.sum( hdiff * np.transpose(tdiff), axis=1, dtype='double')
                sumden1 = np.sum( hdiff * hdiff, dtype='double')
                sumden2 = np.sum( tdiff * tdiff, axis=0, dtype='double')
                
                all_diffs[bnum][key] = sumnum / np.sqrt( sumden1 * sumden2 )

                if progressBar:
                    if  (pbcnt % 5) == 0:
                        progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        break

            self.all_diffs = all_diffs


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
