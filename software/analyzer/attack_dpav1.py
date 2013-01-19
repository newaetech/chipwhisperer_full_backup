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
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.

import numpy

#S-Box used in generating our hypothesis
SBOX = [99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, \
202, 130, 201, 125, 250, 89, 71, 240, 173, 212, 162, 175, 156, 164, 114, 192,       \
183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21,       \
4, 199, 35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117,       \
9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 041, 227, 47, 132,      \
83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207,       \
208, 239, 170, 251, 67, 77, 51, 133, 69, 249, 2, 127, 80, 60, 159, 168,\
81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210,\
205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115,\
96, 129, 79, 220, 34, 42, 144, 136, 70, 238, 184, 20, 222, 94, 11, 219,\
224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121,\
231, 200, 55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8,\
186, 120, 37, 46, 28, 166, 180, 198, 232, 221, 116, 31, 75, 189, 139, 138,\
112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158,\
225, 248, 152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223,\
140, 161, 137, 13, 191, 230, 66, 104, 65, 153, 45, 15, 176, 84, 187, 22];


class attack_DPAAESv1():
    def doDPA(self, targetbit, brange, traces, plaintexts, progressBar=None):

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
                    #Generate the output of the SBOX
                    Hyp = SBOX[int(plaintexts[tnum, bnum], 16) ^ key]

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
#            print "%2x "%foundbyte,


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
        
        

        


