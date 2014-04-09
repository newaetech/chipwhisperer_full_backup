#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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
#=================================================

import pickle
import os.path
import time

class PartialReconfigData(object):   
    def load(self, fname):
        self.configData = pickle.load(open(fname, 'rb'))        
        klist = list(self.configData['values'].keys())
        self.limits = (min(klist), max(klist))

    def getParamSpec(self, name):
        spec = {}        
        spec['name'] = name
        spec['type'] = 'int'
        spec['limits'] = self.limits
        spec['reconfigData'] = self.configData
        
    def getPartialBitstream(self, indx):
        diffs = self.configData['values'][indx]        
        data = list(self.configData['base'])
        
        #Update base
        for diff in diffs:
            data[diff[0]] = diff[1]
        return data

class PartialReconfigDataMulti(object):
    def __init__(self):
        self.dataList = []
        self.limitList = []
       
    def load(self, fname):

        #TODO: Should use something else, hack for now since user does't generate partial files
        ptime = os.path.getmtime(fname)
        # print "Partial Data Created: %s" % time.ctime(ptime)


        data =  pickle.load(open(fname, 'rb')) 
        self.dataList.append(data )        
        klist = list(data['values'].keys())
        self.limitList.append((min(klist), max(klist)))

        return ptime
        
    def getPartialBitstream(self, indxlst):
        
        data = list(self.dataList[0]['base'])
        
        diffs = []
        for i, cfg in enumerate(self.dataList):
            diff = cfg['values'][indxlst[i]]
                        
            diffs.append( diff )        
        
            #Update base
            for d in diff:
                data[d[0]] = d[1]             
                
        return data

class PartialReconfigConnection(object):
    reconfig = 52        
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0        
    
    def __init__(self):
        self.oa = None
    
    def con(self, oadc):
        self.oa = oadc
    
    def dis(self):
        self.oa = None
    
    def program(self, cfgdata):
       
        if self.oa is None:            
            return

        # Reset PR System
        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, [0x20], Validate=False)
        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, [0x00], Validate=False)
       
        # Load Data
        dataarray = [0x00]
       
        # print len(cfgdata)
        if len(cfgdata) > 512:
            raise IOError("PR Data too long!")

        for data in cfgdata:
            #data = int(t, 2)
            msb = data >> 8;
            lsb = data & 0xff;    
            dataarray.append(msb)
            dataarray.append(lsb)


        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, dataarray, Validate=False)

        # stat = self.oa.sendMessage(self.CODE_READ, self.reconfig, Validate=False, maxResp=1)
        # print "%02x" % stat[0]

        # Write Data
        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, [0x1A], Validate=False)
        
        # Check PR Loaded OK
        stat = self.oa.sendMessage(self.CODE_READ, self.reconfig, Validate=False, maxResp=1)

        if stat[0] != 0x00:
            raise IOError("Failed to load PR Data - Status Register = 0x%02x, should be 0x00" % stat[0])
        # print "%02x" % stat[0]
