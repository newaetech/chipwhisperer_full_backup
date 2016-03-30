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

import copy
import re
import numpy as np
import TraceContainerConfig
from chipwhisperer.common.api.config_parameter import ConfigParameter


def getClass():
    return TraceContainer


class Parameters(object):
    def __init__(self, openMode=False):
        self.fmt = None
        traceParams = [{'name':'Trace Configuration', 'type':'group', 'children':[
                        {'name':'Config File', 'key':'cfgfile', 'type':'str', 'readonly':True, 'value':''},
                        {'name':'Format', 'key':'format', 'type':'str', 'readonly':True, 'value':''},
                      ]}]
        self.params = ConfigParameter.create_extended(self, name='Trace Configuration', type='group', children=traceParams)
        self.traceParams = traceParams
        
    def paramList(self):
        return [self.params]


class TraceContainer(object):
    """
    TraceContainer holds traces for the system to operate on. This can include both reading in traces for analysis, and
    writing traces to disk.
    
    This class is normally used as a base class for some specific format. For example the 'TraceFormatNative' class
    adds functions for reading/storing data in the 'native' ChipWhisperer format.
    """
    
    getParams = Parameters()
    getParamsClass = Parameters
    
    def __init__(self, params=None, configfile=None):
        self.config = TraceContainerConfig.TraceContainerConfig(configfile=configfile)
        self.textins = []
        self.textouts = []
        self.keylist = []
        self.knownkey = None
        self.directory = None
        self.prefix = None
        self.dirty = False
        self.tracedtype = np.double  
        self.traces = None
        self.tracehint = 1
        self.pointhint = 0
        self._numTraces = 0
        self._isloaded = False
        
        if params is not None:
            self.getParams = params

    def setDirty(self, dirty):
        self.dirty = dirty
        
    def updateConfigData(self):
        return

    def numPoints(self):
        try:        
            return self.traces.shape[1]
        except:
            return 0
        
    def setTraceBuffer(self, tracebuffer):
        """Reuse a trace buffer allocated elsewhere"""
        self.traces = tracebuffer

    def setTraceHint(self, traces):
        self.tracehint = traces
        
    def setPointHint(self, points):
        self.pointhint = points

    def numTraces(self):
        cfint = int(self.config.attr("numTraces"))
        if cfint != self._numTraces:
            self._numTraces = max(cfint, self._numTraces)
        return self._numTraces

    def addTrace(self, trace, textin, textout, key, dtype=np.double):
        self.addWave(trace, dtype)
        self.addTextin(textin)
        self.addTextout(textout)
        self.addKey(key)

    def writeDataToConfig(self):
        self.config.setAttr("numTraces", self.numTraces())
        self.config.setAttr("numPoints", self.numPoints())      

    def addWave(self, trace, dtype=None):
        try:
            if self.traces is None:
                if dtype is None:
                    dtype = np.double
                self.tracedtype = dtype
                self.traces = np.zeros((self.tracehint, len(trace)), dtype=dtype)
                self.traces[self._numTraces][:] = trace
            else:
                # Check can fit this
                if self.traces.shape[0] <= self._numTraces:
                    if self._numTraces >= self.tracehint:
                        # Tracehint wrong - increase by 25
                        self.tracehint += 25

                    print self.tracehint

                    # Do a resize now to allocate more memory
                    self.traces.resize((self.tracehint, self.traces.shape[1]))

                self.traces[self._numTraces][:] = trace
        except MemoryError:
            raise MemoryError("Failed to allocate/resize array for %d x %d, if you have sufficient memory it may be fragmented. Use smaller segments and retry." % (self.tracehint, self.traces.shape[1]))
            
        self._numTraces += 1
        self.setDirty(True)
        self.writeDataToConfig()

    def setKnownKey(self, key):
        self.knownkey = key

    def addKey(self, key):
        self.keylist.append(key)

    def addTextin(self, data):
        self.textins.append(data)
        
    def addTextout(self, data):
        self.textouts.append(data)
        
    def getTrace(self, n):
        data = self.traces[n]

        #Following line will normalize all traces relative to each
        #other by mean & standard deviation
        #data = (data - np.mean(data)) / np.std(data)
        return data

    def getTextin(self, n):
        return self.textins[n]

    def getTextout(self, n):
        return self.textouts[n]

    def getKnownKey(self, n=0):
        if hasattr(self, 'keylist'):
            if self.keylist is not None:
                return self.keylist[n]

        return self.knownkey
    
    def getAuxDataConfig(self, newmodule):
        """
        Get auxilary data section in config file, searches based on both 'modname'
        and 'uniquedict'. Checks file itself & NOT the local database, since the
        auxilary data is not loaded into database.
        """

        # Get all section names
        for sname in self.config.config.keys():
            # Find if starts with 'Aux Data'
            if sname.startswith("Aux Data"):

                # print "Found %s" % sname

                # Find if module name matches
                if sname.endswith(newmodule["sectionName"]):

                    # print "Found %s" % sname

                    # Finally confirm unique dictionary values
                    for k in newmodule["values"].keys():
                        try:
                            if newmodule["values"][k]["definesunique"]:
                                try:
                                    if str(self.config.config[sname][k]) != str(newmodule["values"][k]["value"]):
                                        return None
                                except KeyError:
                                    return None
                        except KeyError:
                            pass

                    return self.config.config[sname]
        return None

    def addAuxDataConfig(self, newmodule):
        """Add a new module to the config file, place in aux data"""
        # newmodule is a dict of data

        # sectionName will be prepended with "Aux Data N - ", where NNNN
        # is a sequential number. The original section name will be called
        # .originalSectionName, which can be used when searching the system.
        # the NNNN will be also written as an integer to the .auxNumber member

        newdict = copy.deepcopy(newmodule)
        
        #Check dictionary
        maxNumber = 0
        for ad in self.config.attrList:
            if hasattr(ad, "auxNumber"):
                maxNumber = max(maxNumber, ad.auxNumber + 1)

        #Check configuration file
        for sname in self.config.config.keys():
            # Find if starts with 'Aux Data'
            if sname.startswith("Aux Data"):
                maxNumber = max(int(re.findall(r'\d+', sname)[0]) + 1, maxNumber)

        newdict["auxNumber"] = maxNumber
        newdict["originalSectionName"] = newdict["sectionName"]
        newdict["sectionName"] = "Aux Data %04d - " % maxNumber + newdict["sectionName"]

        self.config.attrList.append(newdict)
        self.config.syncFile(sectionname=newdict["sectionName"])

        return newdict

    def prepareDisk(self):
        """Placeholder called after creating a new file setup, but before actually writing traces to it"""
        
        #if self.traces is None:
        #    self.traces = np.zeros((self.tracehint, self.pointhint))
        
    def loadAllConfig(self):
        """Placeholder for loading configuration data ONLY. May not be required."""
        pass
    
    def loadAllTraces(self, directory=None, prefix=""):
        """Placeholder for load command. May not actually read everything into memory depending on format."""
        self._isloaded = True
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)

    def unloadAllTraces(self):
        """Placeholder for unload command. If loadAllTraces unloaded into memory, this may drop from memory. May not be implemented."""
        self._isloaded = False

    def saveAuxData(self, extraname, data):
        """Placeholder for command to save auxiliary data into some location which follows traces (e.g. same database/folder with same prefix.)"""
        raise AttributeError("%s doesn't have this method implemented" % self.__class__.__name__)
    
    def saveAllTraces(self, directory, prefix=""):
        """Placeholder for save command."""
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
    
    def copyTo(self, srcTraces=None, srcFormat=None):
        """Placeholder for copy/import command. Different from load as copies data INTO this classes format, possibly from another format"""
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
    
    def closeAll(self):
        """Writer is done, can close/save any files."""               
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)

    def validateSettings(self):
        """Check settings, log any messages to special setup window"""
        return []

    def isLoaded(self):
        """Returns true if you can use getTrace, getTextin, etc methods"""
        return self._isloaded

    @staticmethod
    def getName():
        return "None"
        
        
if __name__ == "__main__":
    test = TraceContainer()
    wave = [1,2,3,4,5,6]
    test.addTrace(wave, None, None, None)
    test.addTrace(wave, None, None, None)
    test.addTrace(wave, None, None, None)
    print test.numTraces()
    print test.numPoints()
