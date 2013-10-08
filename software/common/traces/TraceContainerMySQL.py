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

import os
import pickle

import sys
sys.path.append('../common')

import numpy as np
from TraceContainer import TraceContainer
from pyqtgraph.parametertree import Parameter

import umysql as sql

from ExtendedParameter import ExtendedParameter

class parameters(object):
    def __init__(self):
        self.fmt = None
        traceParams = [{'name':'MySQL Configuration', 'type':'group', 'children':[
                        {'name':'Server Address', 'key':'addr', 'type':'str', 'value':'127.0.0.1'},
                        {'name':'Server Port', 'key':'port', 'type':'int', 'value':'3306'},
                        {'name':'Username', 'key':'user', 'type':'str', 'value':'root'},
                        {'name':'Password', 'key':'password', 'type':'str', 'value':'admin'},
                        {'name':'Database', 'key':'database', 'type':'str', 'value':'CWTraces'},
                        {'name':'Table Naming', 'key':'tableNameType', 'type':'list', 'values':{'Date/Time Based':'datetime'}, 'value':'datetime'},
                        {'name':'Trace Format', 'key':'traceFormat', 'type':'list', 'values':['NumPy Pickle'], 'value':'NumPy Pickle', 'set':self.setFormat}
                        ]},             
                      ]

        self.params = Parameter.create(name='Attack Settings', type='group', children=traceParams)
        ExtendedParameter.setupExtended(self.params, self)
        
    def setFormat(self, fmt):
        self.fmt = fmt
        
    def format(self):
        if self.fmt is None:
            self.fmt = self.findParam('traceFormat').value()
            
        return self.fmt
        
    def paramList(self):
        return [self.params]

class TraceContainerMySQL(TraceContainer):
    
    getParams = parameters
    
    def __init__(self, params=None):
        super(TraceContainerMySQL, self).__init__()
        self.db = None
        
        if params is not None:
            self.getParams = params
            
    def makePrefix(self, mode='datetime'):
        return "test"
            
    def prepareDisk(self):
        db = sql.Connection()
        
        server = self.getParams.findParam('addr').value()
        port = int(self.getParams.findParam('port').value())
        user = self.getParams.findParam('user').value()
        password = self.getParams.findParam('password').value()
        database = self.getParams.findParam('database').value()
        traceprefix = self.makePrefix(self.getParams.findParam('tableNameType').value())
        
        #Connection
        db.connect(server,port,user,password,database)
        
        #CREATE DATABASE `cwtraces` /*!40100 COLLATE 'utf8_unicode_ci' */
        
        
        #Check version as simple validation
        result = db.query("SELECT VERSION()")
        print "MySQL Version: %s"%result.rows[0][0]
         
        self.tableName = "test_table"
         
        db.query("CREATE TABLE IF NOT EXISTS %s(Id INT PRIMARY KEY AUTO_INCREMENT,\
         Textin VARCHAR(32),\
         EncKey VARCHAR(32),\
         Textout VARCHAR(32),\
         Wave MEDIUMBLOB)"%self.tableName)
         
        self.db = db
        
    def formatWave(self, wave):
        if self.getParams.format() == "NumPy Pickle":
            return pickle.dumps(wave, protocol=2)            
        else:
            raise AttributeError("Invalid Format for MySQL")
        
    def addTrace(self, trace, textin, textout, key, dtype=np.double):
               
        strTextin = ""
        for t in textin:
            strTextin += "%02X"%t

        strTextout = ""
        for t in textout:
            strTextout += "%02X"%t            
        
        if key is not None:
            strKey = ""
            for t in key:
                strKey += "%02X"%t
        else:
            strKey = ""            
            
        self.db.query("INSERT INTO %s(Textin, Textout, EncKey, Wave) VALUES('%s', '%s', '%s', "%(self.tableName, strTextin, strTextout,
                                                                                                strKey) + "%s)", (self.formatWave(trace),))
        
    def closeAll(self):
        if self.db is not None:
            self.db.close()
        
        self.db = None
