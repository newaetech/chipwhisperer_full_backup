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

    
    
