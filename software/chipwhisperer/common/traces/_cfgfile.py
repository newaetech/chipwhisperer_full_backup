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
import logging

__author__ = "Colin O'Flynn"

import sys
import os
#import pstats, cProfile #For profiling support (not 100% needed)

try:
    from configobj import ConfigObj  # type: ignore
except ImportError:
    print("ERROR: configobj (https://pypi.python.org/pypi/configobj/) is required for this program")
    sys.exit()


def makeAttrDict(sectionname, modulename, paramSpecs):
    """
    Converts a parameter tree configuration setup into an attribute dictionary. Requires specific setup
    of parameter tree, assumes passed a single-element list, with first element having 'children'. No
    sub-groups allowed. Any element without a 'value' key is skipped.
    """
    
    ps = paramSpecs[0]['children']
    
    attrDict = {
                "sectionName":sectionname,
                "moduleName":modulename,
                "module":None,
                "values":{},
                }    
    order = 0    
    for p in ps:
        try:
            edit = not p["readonly"]
        except KeyError:
            edit = True
            
        try:
            attrDict["values"][p["key"]] = {"order":order, "value":p["value"], "desc":p["name"], "changed":False, "editable":edit}
        except KeyError:
            #Silently skip anything without a 'value' attribute given, which forms the initial value
            pass
        
        order += 1
        
    return attrDict
    
   
class TraceContainerConfig(object):
    """
    This class holds configuration data for a TraceContainer class. This is split into a separate class
    partially for historical reasons, since the old code had this as a separate class.
    
    This class is responsible for reading/writing the .cfg file associated with any traces. Even if we are
    using non-native trace format, the traces will STILL have a ChipWhisperer-specific .cfg file written
    with them.
    
    This choice means you aren't forced to import traces into ChipWhisperer-only format, and can for example
    you may wish to keep them in some MATLAB workspace format instead.
    """   
        
    ## Notes on dictionary:
    # order = order of display, needed since dictionary is unordered
    # value = value of variable
    # desc = description of variable
    # changed = has value changed since being written to disk?
    # headerLabel = short description/name used for header label in trace management dialog
    #

    def __init__(self, configfile=None):
        """If a config file is given, will attempt to load that file"""
        self.attrDictInst = {
                "sectionName":"Trace Config",
                "moduleName":"native",
                "module":None,
                "values":{
                    "format":{"order":0, "value":"native", "desc":"Native Format Type", "changed":False, "headerLabel":"Format", "editable":False},
                    "numTraces":{"order":1, "value":0, "desc":"Number of Traces in File", "changed":False, "headerLabel":"# Traces", "editable":False},
                    "numPoints":{"order":2, "value":0, "desc":"Number of Points per trace, assuming uniform", "changed":False, "headerLabel":"# Points", "editable":False},
                    "date":{"order":3, "value":"1997-01-28 17:05:00", "desc":"Date of Capture YYYY-MM-DD HH:MM:SS Format", "changed":False, "headerLabel":"Date and Time of Capture", "editable":False},
                    "prefix":{"order":4, "value":None, "desc":"Prefix of all files if applicable", "changed":False},
                    "targetHW":{"order":5, "value":"unknown", "desc":"Description of Target (DUT) Hardware", "changed":False, "headerLabel":"Target Hardware", "editable":True},
                    "targetSW":{"order":6, "value":"unknown", "desc":"Description of Target (DUT) Software", "changed":False, "headerLabel":"Target Software", "editable":True},
                    "scopeName":{"order":7, "value":"unknown", "desc":"Scope Model/Description", "changed":False, "headerLabel":"Scope Module Name", "editable":True},
                    "scopeSampleRate":{"order":8, "value":0, "desc":"Sample Rate (s/sec)", "changed":False, "headerLabel":"Sample Rate", "editable":True},
                    "scopeYUnits":{"order":9, "value":0, "desc":"Units of Y Points", "changed":False, "editable":True},
                    "scopeXUnits":{"order":10, "value":0, "desc":"Units of X Points", "changed":False, "editable":True},
                    "notes":{"order":11, "value":"", "desc":"Additional Notes about Capture Setup", "changed":False, "headerLabel":"Notes", "editable":True}                
                    },
                }
    
        self.attrList = [self.attrDictInst]
        self.mappedRange = None
        self.trace = None
        if configfile is not None:
            configfile = os.path.normpath(configfile)
        self._configfile = configfile

        #Attempt load/sync
        self.loadTrace(configfile)
        
    def module(self, attr, moduleName=None):
        """Given an attribute & possibly module name, return reference to module dictionary"""
        module = None
                
        if moduleName == None:
            for i in self.attrList:
                if attr in i["values"]:
                    module = i
                    break
        else:
            for i in self.attrList:
                if (i["moduleName"] == moduleName) | (i["sectionName"] == moduleName):
                    if attr in i["values"]:
                        module = i
                        break
                
        if module == None:
            raise ValueError("Invalid attribute: %s"%attr)
        
        return module
        
    def attrHeaderValues(self):
        """Used to list all values in {"name":name, "header":header title, "desc":description} dictionary"""
        lst = []
        
        for i in self.attrList:
            for p in i["values"].keys():
                try:
                    lst += [{ "name":p, "header":i["values"][p]["headerLabel"], "desc":i["values"][p]["desc"], "order":i["values"][p]["order"] }]
                except KeyError:
                    #If we don't have headerLabel type, we skip this item
                    pass
        
        lst = sorted(lst, key=lambda k: k["order"]) 
        
        return lst
     
    def setConfigFilename(self, fname):
        """Set the config filename, WITHOUT syncronizing internal DB to File. Use load or save for this feature."""
        if fname is not None:
            fname = os.path.normpath(fname)
        self._configfile = fname
        
    def configFilename(self):
        """Get Config Filename"""
        return self._configfile
     
    def attr(self, attr, moduleName=None):
        """Get value of attribute specified from internal DB"""       
        return self.attrDict(attr, moduleName)["value"]
    
    def attrDict(self, attr, moduleName=None):
        """Get dictionary of attribute specified, includes additional info such as flags"""
        mod = self.module(attr,moduleName)        
        return mod["values"][attr]
        
    def setAttr(self, attr, value, moduleName=None):
        """Set value of attribute in internal DB"""
        mod = self.module(attr,moduleName)
        mod["values"][attr]["value"] = value
        mod["values"][attr]["changed"] = True 
               
    def syncFile(self, sectionname=None):
        """
        Sync the internal DB cache to the .cfg file specified already, basically this can be a save or load.
        
        The CHANGED flag is used to decide what to do. If the 'changed' flag is FALSE in the internal DB,
        the disk has priority. Any attributes not in the disk .cfg file will be taken from the internal DB.
        
        If the 'changed' flag of an attribute is TRUE, the internal DB will overwrite the disk file.
        """

        for ad in self.attrList:

            # If user only wishes to update a certain section do that
            if sectionname is not None:
                if ad["sectionName"] != sectionname:
                    continue

            sn = ad["sectionName"]
            
            #Does section exist?
            try:
                self.config[sn]
                logging.debug('Section %s found' % sn)
            except KeyError:
                self.config[sn] = {}
                logging.debug('Section %s not found' % sn)
                
            #Check each item
            for item in ad["values"].keys():
                try:
                    self.config[sn][item]
                    
                    #Check priority
                    if ad["values"][item]["changed"] == False:                        
                        ad["values"][item]["value"] = self.config[sn][item]
                        logging.debug('%s from configfile = %s' % (item, self.config[sn][item]))
                    else:
                        self.config[sn][item] = ad["values"][item]["value"]
                        logging.debug('%s from cache = %s' % (item, ad["values"][item]["value"]))
                
                except KeyError:
                    self.config[sn][item] = ad["values"][item]["value"]
                    logging.debug('%s missing in cfg file' % item)
        
    def loadTrace(self, configfile=None):
        """Load config file. Syncs internal DB to File"""
        if configfile:
            self._configfile = os.path.normpath(configfile)
            if not os.path.isfile(configfile):
                raise Exception("Error: Trace set config file doesn't exists: " + configfile)

        self.config = ConfigObj(self._configfile)
        self.syncFile()
        
    def saveTrace(self, configfile = None):
        """Save internal DB to Config File"""
        self.config.filename = self._configfile
        self.syncFile()     
        self.config.write()

    def checkTraceItem(self, itemname, localitem, changedlist):
        if localitem != self.__dict__[itemname]:                      
            changedlist.append(itemname)
            return True
        return False        

    def checkTrace(self, configfile = None):
        if configfile == None:
            configfile = self.configfile
      
        changed = True
        
        #TODO: THIS

        return changed
