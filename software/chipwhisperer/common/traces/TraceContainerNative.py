#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import os
import numpy as np
import TraceContainer

class TraceContainerNative(TraceContainer.TraceContainer):

    def copyTo(self, srcTraces=None):
        self.NumTrace = srcTraces.NumTrace
        self.NumPoint = srcTraces.NumPoint
        self.knownkey = srcTraces.knownkey

        self.textins = np.zeros([self.NumTrace, 16], dtype=np.uint8)
        for n in range(0, self.NumTrace):
            tin = srcTraces.textins[n]
            self.textins[n] = map(int, tin, [16]*len(tin))
        

        self.textouts = np.zeros([self.NumTrace, 16], dtype=np.uint8)
        for n in range(0, self.NumTrace):
            tout = srcTraces.textouts[n]
            self.textouts[n] = map(int, tout, [16]*len(tout))

        if srcTraces.tracedtype:
            userdtype = srcTraces.tracedtype
        else:
            userdtype = np.float
            
        self.traces = np.array(srcTraces.traces, dtype=userdtype)

        #Traces copied in means not saved
        self.setDirty(True)
        
        
    def loadAllTraces(self, directory=None, prefix=""):
        self.traces = np.load(directory + "/%straces.npy"%prefix, mmap_mode='r')
        self.textins = np.load(directory + "/%stextin.npy"%prefix)
        self.textouts = np.load(directory + "/%stextout.npy"%prefix)
        self.knownkey = np.load(directory + "/%sknownkey.npy"%prefix)

	#These should come from config file
        #self.NumTrace = self.traces.shape[0]
        #self.NumPoint = self.traces.shape[1]

        #Traces loaded means saved
        self.setDirty(False)

    def saveAllTraces(self, directory, prefix=""):
        self.config.saveTrace()
        np.save(directory + "/%straces.npy"%prefix, self.traces)
        np.save(directory + "/%stextin.npy"%prefix, self.textins)
        np.save(directory + "/%stextout.npy"%prefix, self.textouts)
        np.save(directory + "/%skeylist.npy"%prefix, self.keylist)    
        np.save(directory + "/%sknownkey.npy"%prefix, self.knownkey)
        self.setDirty(False)
        
    def closeAll(self):        
        self.saveAllTraces( os.path.dirname(self.config.configFilename()), prefix=self.config.attr("prefix"))
        
        # Release memory associated with data
        self.traces = None
        self.textints = None
        self.textouts = None
        self.keylist = None
        self.knownkey = None

