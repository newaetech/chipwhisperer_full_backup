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

import pickle

import numpy as np

import TraceContainer
from TraceContainerConfig import makeAttrDict
from chipwhisperer.common.api.config_parameter import ConfigParameter
try:
    import umysql as sql
except ImportError, e:
    # This isn't really needed, no need to bother users
    # print "umysql required: https://pypi.python.org/pypi/umysql"
    raise ImportError(e)

def getClass():
    return TraceContainerMySQL

class parameters(object):
    def __init__(self, openMode=False):
        self.fmt = None
        traceParams = [{'name':'MySQL Configuration', 'type':'group', 'children':[
                        {'name':'Server Address', 'key':'addr', 'type':'str', 'value':'127.0.0.1'},
                        {'name':'Server Port', 'key':'port', 'type':'int', 'value':'3306'},
                        {'name':'Username', 'key':'user', 'type':'str', 'value':'root'},
                        {'name':'Password', 'key':'password', 'type':'str', 'value':'admin'},
                        {'name':'Database', 'key':'database', 'type':'str', 'value':'CWTraces'},
                        {'name':'Table Name', 'key':'tableName', 'type':'str', 'value':'', 'readonly':True}
                      ]}]

        if openMode == False:
            traceParams[0]['children'].append({'name':'Table Naming', 'key':'tableNameType', 'type':'list', 'values':{'Auto-Prefix':'prefix'}, 'value':'prefix'})
        else:
            traceParams[0]['children'].append({'name':'Relist Tables', 'key':'tableListAct', 'type':'action'})
            traceParams[0]['children'].append({'name':'Table List', 'key':'tableNameList', 'type':'list', 'values':[], 'value':'', 'linked':['Table Name']})

        traceParams[0]['children'].append({'name':'Trace Format', 'key':'traceFormat', 'type':'list', 'values':['NumPy Pickle'], 'value':'NumPy Pickle', 'set':self.setFormat})

        self.traceParams = traceParams

        self.params = ConfigParameter.create_extended(self, name='MySQL Settings', type='group', children=traceParams)

    def setFormat(self, fmt):
        self.fmt = fmt

    def format(self):
        if self.fmt is None:
            self.fmt = self.findParam('traceFormat').value()

        return self.fmt

    def paramList(self):
        return [self.params]

class TraceContainerMySQL(TraceContainer.TraceContainer):
    getParamsClass = parameters
    getParams = parameters()

    def __init__(self, params=None):
        super(TraceContainerMySQL, self).__init__()
        self.db = None
        self.idOffset = 0
        self.lastId = 0

        if params is not None:
            self.getParams = params

        #Connect actions if applicable
        try:
            self.getParams.findParam('tableListAct').opts['action'] = self.listAllTables
        except AttributeError:
            pass

        self.getParams.findParam('tableName').opts['get'] = self._getTableName

        #Save extra configuration options
        self.attrDict = makeAttrDict("MySQL Config", "mysql", self.getParams.traceParams)
        self.config.attrList.append(self.attrDict)

        #Format name must agree with names from TraceContainerFormatList
        self.config.setAttr("format", "mysql")

    def makePrefix(self, mode='prefix'):
        if mode == 'prefix':
            prefix = self.config.attr('prefix')
            if prefix == "" or prefix is None:
                raise AttributeError("Prefix attribute not set in trace config")

            #Drop everything but underscore (_) for table name
            prefix = "tracedb_" + prefix
            prefix = prefix.replace("-","_")
            prefix = ''.join(c for c in prefix if c.isalnum() or c in("_"))

            return prefix

        raise ValueError("Invalid mode: %s"%mode)

    def prepareDisk(self):
        self.con()
        #CREATE DATABASE `cwtraces` /*!40100 COLLATE 'utf8_unicode_ci' */
        traceprefix = self.makePrefix(self.getParams.findParam('tableNameType').value())

        #Check version as simple validation
        result = self.db.query("SELECT VERSION()")
        print "MySQL Version: %s"%result.rows[0][0]

        self.tableName = traceprefix

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

    def _getTableName(self):
        return self.getParams.findParam('tableNameList').value()

    def listAllTables(self):
        self.con()
        database = self.getParams.findParam('database').value()
        results = self.db.query("SHOW TABLES IN %s"%database)
        tables = []
        for r in results.rows:
            tables.append(r[0])
        self.getParams.findParam('tableNameList').setLimits(tables)


    def updatePointsTraces(self):
        res = self.db.query("SELECT COUNT(*) FROM %s" % self.tableName)
        self._numTraces = res.rows[0][0]

        wav = self.db.query("SELECT Wave FROM %s LIMIT 1 OFFSET %d" % (self.tableName, 0)).rows[0][0]
        self._numPoints = self.formatWave(wav, read=True).shape[0]

    def updateConfigData(self):
        self.con()

        self.tableName = self.getParams.findParam('tableName').value()
        res = self.db.query("SELECT COUNT(*) FROM %s" % self.tableName)
        self._numTraces = res.rows[0][0]
        self.config.setAttr('numTraces', self._numTraces)

        wav = self.db.query("SELECT Wave FROM %s LIMIT 1 OFFSET %d"%(self.tableName, 0)).rows[0][0]
        self._numPoints = self.formatWave(wav, read=True).shape[0]
        self.config.setAttr('numPoints', self._numPoints)

    def numTraces(self, update=False):
        if update:
            self.updateConfigData()
        return self._numTraces


    def numPoints(self, update=False):
        if update:
            self.updateConfigData()
        return self._numPoints

    def loadAllConfig(self):
        for p in self.getParams.traceParams[0]['children']:
            try:
                val = self.config.attr(p["key"], "mysql")
                self.getParams.findParam(p["key"]).setValue(val)
            except ValueError:
                pass
            #print "%s to %s=%s"%(p["key"], val, self.getParams.findParam(p["key"]).value())

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
            strTextout += "%02X" % t

        if key is not None:
            strKey = ""
            for t in key:
                strKey += "%02X"%t
        else:
            strKey = ""

        self.db.query("INSERT INTO %s(Textin, Textout, EncKey, Wave) VALUES('%s', '%s', '%s', "%(self.tableName, strTextin, strTextout,
                                                                                                strKey) + "%s)", (self.formatWave(trace),))

    def saveAll(self):
        #Save attributes from config settings
        for t in self.getParams.traceParams[0]['children']:
            self.config.setAttr(t["key"],  self.getParams.findParam(t["key"]).value() ,"mysql")

        #Save table name/prefix too
        self.config.setAttr("tableName", self.tableName, "mysql")
        self.config.saveTrace()

    def closeAll(self, clearTrace=True, clearText=True, clearKeys=True):
        # self.saveAllTraces(os.path.dirname(self.config.configFilename()), prefix=self.config.attr("prefix"))

        # Release memory associated with data in case this isn't deleted
        if clearTrace:
            self.traces = None

        if clearText:
            self.textins = None
            self.textouts = None

        if clearKeys:
            self.keylist = None
            self.knownkey = None

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

    @staticmethod
    def getName():
        return "MySQL"

