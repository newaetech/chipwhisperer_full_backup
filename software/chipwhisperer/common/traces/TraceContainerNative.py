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

def getClass():
    return TraceContainerNative

class TraceContainerNative(TraceContainer.TraceContainer):

    def copyTo(self, srcTraces=None):
        self.NumTrace = srcTraces.NumTrace
        self.NumPoint = srcTraces.NumPoint
        self.knownkey = srcTraces.knownkey

        self.textins = np.zeros([self.NumTrace, 16], dtype=np.uint8)
        for n in range(0, self.NumTrace):
            tin = srcTraces.textins[n]
            self.textins[n] = map(int, tin, [16] * len(tin))


        self.textouts = np.zeros([self.NumTrace, 16], dtype=np.uint8)
        for n in range(0, self.NumTrace):
            tout = srcTraces.textouts[n]
            self.textouts[n] = map(int, tout, [16] * len(tout))

        if srcTraces.tracedtype:
            userdtype = srcTraces.tracedtype
        else:
            userdtype = np.float

        self.traces = np.array(srcTraces.traces, dtype=userdtype)

        # Traces copied in means not saved
        self.setDirty(True)


    def loadAllTraces(self, directory=None, prefix=""):
        """Load all traces into memory"""

        if directory is None:
            directory = self.directory

        if prefix is None:
            prefix = self.prefix

        self.traces = np.load(directory + "/%straces.npy" % prefix, mmap_mode='r')
        self.textins = np.load(directory + "/%stextin.npy" % prefix)
        self.textouts = np.load(directory + "/%stextout.npy" % prefix)

        try:
            self.knownkey = np.load(directory + "/%sknownkey.npy" % prefix)
        except IOError:
            self.knownkey = None

        # OK if this fails
        try:
            self.keylist = np.load(directory + "/%skeylist.npy" % prefix)
        except IOError:
            self.keylist = None

        # Traces loaded means saved
        self.setDirty(False)
        self._isloaded = True

    def unloadAllTraces(self):
        """Drop traces from memory to save space """
        self.traces = None
        self.textins = None
        self.textouts = None
        self.knownkey = None
        self.keylist = None
        self._isloaded = False

    def saveAuxData(self, data, configDict, filenameKey="filename"):
        path = os.path.dirname(self.config.configFilename())
        prefix = self.config.attr("prefix")
        fname = "%s%s_aux_%04d.npy" % (prefix, configDict["moduleName"], configDict["auxNumber"])
        np.save(path + "/" + fname, data)

        # if filenameKey is not None:
        configDict["values"][filenameKey]["value"] = fname
        configDict["values"][filenameKey]["changed"] = True
        self.config.syncFile(sectionname=configDict["sectionName"])
        self.config.saveTrace()

        return fname

    def loadAuxData(self, fname):
        path = os.path.dirname(self.config.configFilename())
        # prefix = self.config.attr("prefix")
        # fname = "%s%s.npy" % (prefix, extraname)
        return np.load(path + "/" + fname)

    def saveAllTraces(self, directory, prefix=""):
        self.config.saveTrace()
        np.save(directory + "/%straces.npy" % prefix, self.traces)
        np.save(directory + "/%stextin.npy" % prefix, self.textins)
        np.save(directory + "/%stextout.npy" % prefix, self.textouts)
        np.save(directory + "/%skeylist.npy" % prefix, self.keylist)
        np.save(directory + "/%sknownkey.npy" % prefix, self.knownkey)
        self.setDirty(False)

    def closeAll(self, clearTrace=True, clearText=True, clearKeys=True):
        print "unloading"
        self.saveAllTraces(os.path.dirname(self.config.configFilename()), prefix=self.config.attr("prefix"))

        # Release memory associated with data in case this isn't deleted
        if clearTrace:
            self.traces = None

        if clearText:
            self.textins = None
            self.textouts = None

        if clearKeys:
            self.keylist = None
            self.knownkey = None

    @staticmethod
    def getName():
        return "ChipWhisperer/Native"