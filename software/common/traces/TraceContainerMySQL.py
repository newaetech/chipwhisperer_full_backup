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
    def __init__(self, openMode=False):
        self.fmt = None
        traceParams = [{'name':'MySQL Configuration', 'type':'group', 'children':[
                        {'name':'Server Address', 'key':'addr', 'type':'str', 'value':'127.0.0.1'},
                        {'name':'Server Port', 'key':'port', 'type':'int', 'value':'3306'},
                        {'name':'Username', 'key':'user', 'type':'str', 'value':'root'},
                        {'name':'Password', 'key':'password', 'type':'str', 'value':'admin'},
                        {'name':'Database', 'key':'database', 'type':'str', 'value':'CWTraces'}
                      ]}]
        
        if openMode == False:
            traceParams[0]['children'].append({'name':'Table Naming', 'key':'tableNameType', 'type':'list', 'values':{'Date/Time Based':'datetime'}, 'value':'datetime'})
        else:
            traceParams[0]['children'].append({'name':'Table Name', 'key':'tableName', 'type':'list', 'values':[]})
            traceParams[0]['children'].append({'name':'List Tables', 'key':'tableListAct', 'type':'action'})
        
        traceParams[0]['children'].append({'name':'Trace Format', 'key':'traceFormat', 'type':'list', 'values':['NumPy Pickle'], 'value':'NumPy Pickle', 'set':self.setFormat})
        self.params = Parameter.create(name='MySQL Settings', type='group', children=traceParams)
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
        self.idOffset = 0
        self.lastId = 0
        
        if params is not None:
            self.getParams = params
            self.getParams.findParam('tableListAct').opts['action'] = self.listAllTables
            
    def makePrefix(self, mode='datetime'):
        return "test"
            
    def prepareDisk(self):
        self.con()        
        #CREATE DATABASE `cwtraces` /*!40100 COLLATE 'utf8_unicode_ci' */
        traceprefix = self.makePrefix(self.getParams.findParam('tableNameType').value())
        
        #Check version as simple validation
        result = self.db.query("SELECT VERSION()")
        print "MySQL Version: %s"%result.rows[0][0]
         
        self.tableName = "test_table"
         
        self.db.query("CREATE TABLE IF NOT EXISTS %s(Id INT PRIMARY KEY AUTO_INCREMENT,\
         Textin VARCHAR(32),\
         EncKey VARCHAR(32),\
         Textout VARCHAR(32),\
         Wave MEDIUMBLOB)"%self.tableName)

    def con(self):
        if self.db is not None:
            self.db.close()
            
        db = sql.Connection()
        
        server = self.getParams.findParam('addr').value()
        port = int(self.getParams.findParam('port').value())
        user = self.getParams.findParam('user').value()
        password = self.getParams.findParam('password').value()
        database = self.getParams.findParam('database').value()
        
        #Connection
        db.connect(server,port,user,password,database)
        
        self.db = db
        
    def listAllTables(self):
        self.con()
        database = self.getParams.findParam('database').value()
        results = self.db.query("SHOW TABLES IN %s"%database)
        tables = []
        for r in results.rows:
            tables.append(r[0])            
        self.getParams.findParam('tableName').setLimits(tables)
        
        
    def updateConfigData(self):
        self.con()
        self.tableName = self.getParams.findParam('tableName').value()        
        res = self.db.query("SELECT COUNT(*) FROM %s"%(self.tableName))
        self._NumTrace = res.rows[0][0]
        self.config.setAttr('numTraces', self._NumTrace)
        
        wav = self.db.query("SELECT Wave FROM %s LIMIT 1 OFFSET %d"%(self.tableName, 0)).rows[0][0]
        self._NumPoint = self.formatWave(wav, read=True).shape[0]
        self.config.setAttr('numPoints', self._NumPoint)
        
    def numTraces(self, update=False):
        if update:
            self.updateConfigData()
        return self._NumTrace
        
    def numPoints(self, update=False):
        if update:
            self.updateConfigData()
        return self._NumPoint
        
    def loadAllTraces(self, path=None, prefix=None):
        self.updateConfigData()
        
        
    def formatWave(self, wave, read=False):
        if self.getParams.format() == "NumPy Pickle":
            if read == False:
                return pickle.dumps(wave, protocol=2)
            else:
                return np.array(pickle.loads(wave))
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

    def getTrace(self, n):
        wv = self.db.query("SELECT Wave FROM %s LIMIT 1 OFFSET %d"%(self.tableName, n)).rows[0][0]
        return self.formatWave(wv, read=True)

    def asc2list(self, asc):
        lst = []
        for i in range(0,len(asc),2):
            lst.append( int(asc[i:(i+2)], 16) )
        return lst            

    def getTextin(self, n):
        asc = self.db.query("SELECT Textin FROM %s LIMIT 1 OFFSET %d"%(self.tableName, n)).rows[0][0]
        return self.asc2list(asc)

    def getTextout(self, n):
        asc = self.db.query("SELECT Textout FROM %s LIMIT 1 OFFSET %d"%(self.tableName, n)).rows[0][0]
        return self.asc2list(asc)

    def getKnownKey(self, n=None):
        if n is None:
            n = 0    
        asc = self.db.query("SELECT EncKey FROM %s LIMIT 1 OFFSET %d"%(self.tableName, n)).rows[0][0]
        return self.asc2list(asc)
