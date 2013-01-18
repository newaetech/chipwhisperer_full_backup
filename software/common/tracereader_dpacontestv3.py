#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the authors nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COLIN O'FLYNN BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import numpy
import os
import xml.etree.ElementTree as ET

class tracereader_dpacontestv3:
    def __init__(self):
        self.NumTrace = None
        self.NumPoint = None
        self.textints = None
        self.textouts = None
        self.knownkey = None
        self.directory = None

    def loadInfo(self, directory=None):
        if directory == None:
            directory = self.directory
        else:
            self.directory = directory
            
        tree = ET.parse(os.path.join(directory, 'info.xml'))
        root = tree.getroot()
        NumTrace = int(root.findall('NumTrace')[0].text)
        NumPoint = int(root.findall('NumPoint')[0].text)

        self.NumTrace = NumTrace
        self.NumPoint = NumPoint
        
    def loadAllTraces(self, directory=None):
        if directory == None:
            directory = self.directory
        else:
            self.directory = directory
            
        if (self.NumTrace == None):
            self.loadInfo(directory)

        self.traces = numpy.loadtxt(os.path.join(directory, 'wave.txt'), ndmin=2)

        try:
            self.textins = numpy.loadtxt(os.path.join(directory, 'text_in.txt'), dtype='|S2',  ndmin=2)
        except IOError:
            self.textins = None

        try:
            self.textouts = numpy.loadtxt(os.path.join(directory, 'text_out.txt'), dtype='|S2',  ndmin=2)
        except IOError:
            self.textouts = None

        try:
           knownkey = numpy.loadtxt(os.path.join(directory, 'key.txt'), dtype='|S2',  ndmin=2)
        except IOError:
            knownkey = None

        try:
            knownkey = knownkey[0]
            self.knownkey = []
            for r in knownkey:
                self.knownkey.append(int(r, 16))
        except:
            self.knownkey = None

    def numPoints(self):
        return self.NumPoint

    def numTraces(self):
        return self.NumTrace

    def getTrace(self, n):
        return self.traces[n]

    def getTextin(self, n):
        return self.textins[n]

    def getKnownKey(self):
        return self.knownkey

#Test Example
if __name__ == "__main__":
    tr = tracereader_dpacontestv3()
    tr.loadAllTraces("capture-2012.11.30-19.22.58")

    
    
