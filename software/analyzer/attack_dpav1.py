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

import numpy
import attack_aesmodel as model

class attack_DPAAESv1():
    
    def doDPA(self, brange, traces, plaintexts, ciphertexts, progressBar=None, modeltype="hw", keyround="first", targetbit=0):

        traces =numpy.array(traces)
        plaintexts =numpy.array(plaintexts)

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
                mean1 = numpy.zeros(len(traces[0,:]))
                mean0 = numpy.zeros(len(traces[0,:]))
                num1 = 0
                num0 = 0

                #For each trace, do the following
                for tnum in range(len(traces[:,0])):
                    if len(plaintexts) > 0:
                        pt = plaintexts[tnum]

                    if len(ciphertexts) > 0:
                        ct = ciphertexts[tnum]

                    if keyround == "first":
                        ct = None
                    elif keyround == "last":
                        pt = None
                    else:
                        raise ValueError("keyround invalid")
                        
                    
                    #Generate the output of the SBOX
                    if modeltype == "hw":
                        Hyp = model.HypHW(pt, ct, key, bnum);
                    elif modeltype == "hd":
                        Hyp = model.HypHD(None, ct, key, bnum);
                    else:
                        raise ValueError("modeltype invalid")

                    #Is target bit 1 or target bit 0?
                    if (Hyp & (1 << targetbit)) != 0:
                        #Bit is 1, so add this trace to the 1 partition
                        mean1 = numpy.add(mean1, traces[tnum,:])
                        num1 = num1 + 1
                    else:
                        #Bit is 0, so add this trace to the 0 partition
                        mean0 = numpy.add(mean0, traces[tnum,:])
                        num0 = num0 + 1
                    
                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        break
                        
                #Average
                mean1 = mean1 / num1
                mean0 = mean0 / num0

                #Find the difference between the two means
                diffs[key] = numpy.subtract(mean1, mean0)

                #Find the biggest difference for this specific key & store
                #diffs[key] = max(numpy.fabs(diff))

            
            self.all_diffs[bnum] = diffs

            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
            #print "%2x "%foundbyte,


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
        
        

        


