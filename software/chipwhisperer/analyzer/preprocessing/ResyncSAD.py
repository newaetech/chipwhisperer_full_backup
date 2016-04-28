#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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

import numpy as np
from ._base import PreprocessingBase
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI


class ResyncSAD(PreprocessingBase):
    """
    Resync by minimizing the SAD.
    """
    name = "Resync: Sum-of-Difference"
    description = "Minimizes the 'Sum of Absolute Difference' (SAD), also known as 'Sum of Absolute Error'. Uses "\
                  "a portion of one of the traces as the 'reference'. This reference is then slid over the 'input "\
                  "window' for each trace, and the amount of shift resulting in the minimum SAD criteria is selected "\
                  "as the shift amount for that trace."

    def _setupParameters(self):
        self.rtrace = 0
        self.debugReturnSad = False
        self.ccStart = 0
        self.ccEnd = 1
        self.wdStart = 0
        self.wdEnd = 1
        return [ {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'value':0, 'set':self.updateScript},
                 {'name':'Reference Points', 'key':'refpts', 'type':'rangegraph', 'graphwidget':lambda: CWCoreAPI.getInstance().getGraphWidget(), 'set':self.updateScript, 'default':(0, 0)},
                 {'name':'Input Window', 'key':'windowpt', 'type':'rangegraph', 'graphwidget':lambda: CWCoreAPI.getInstance().getGraphWidget(), 'set':self.updateScript, 'default':(0, 0)},
                 # {'name':'Valid Limit', 'type':'float', 'value':0, 'step':0.1, 'limits':(0, 10), 'set':self.setValidLimit},
                 # {'name':'Output SAD (DEBUG)', 'type':'bool', 'value':False, 'set':self.setOutputSad},
               ]

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())

        refpt = self.findParam('refpts').value()
        windowpt = self.findParam('windowpt').value()

        if refpt is None: refpt = (0, 0)
        if windowpt is None: windowpt = (0, 0)

        self.addFunction("init", "setReference", "rtraceno=%d, refpoints=(%d,%d), inputwindow=(%d,%d)" %
                         (self.findParam('reftrace').value(), refpt[0], refpt[1], windowpt[0], windowpt[1]))

    def setReference(self, rtraceno=0, refpoints=(0, 0), inputwindow=(0, 0)):
        self.rtrace = rtraceno
        self.wdStart = inputwindow[0]
        self.wdEnd = inputwindow[1]
        self.ccStart = refpoints[0]
        self.ccEnd = refpoints[1]
        self.init()

    def setOutputSad(self, enabled):
        self.debugReturnSad = enabled
   
    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            sad = self.findSAD(trace)
            
            if self.debugReturnSad:
                return sad
            
            if len(sad) == 0:
                return None            
            
            newmaxloc = np.argmin(sad)
            maxval = min(sad)
            #if (maxval > self.refmaxsize * 1.01) | (maxval < self.refmaxsize * 0.99):
            #    return None
            
            if maxval > self.maxthreshold:
                return None
            
            diff = newmaxloc-self.refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace
        else:
            return self._traceSource.getTrace(n)
   
    def init(self):
        try:
            self.calcRefTrace(self.rtrace)
        #Probably shouldn't do this, but deals with user enabling preprocessing
        #before trace management setup
        except ValueError:
            pass
        
    def findSAD(self, inputtrace):
        reflen = self.ccEnd-self.ccStart
        sadlen = self.wdEnd-self.wdStart
        sadarray = np.empty(sadlen)
        
        for ptstart in range(self.wdStart, self.wdEnd):    
            #Find SAD        
            sadarray[ptstart-self.wdStart] = np.sum(np.abs(inputtrace[ptstart:(ptstart+reflen)] - self.reftrace))
            
        return sadarray
        
    def calcRefTrace(self, tnum):
        #If not enabled stop
        if self.enabled == False:
            return
        
        self.reftrace = self._traceSource.getTrace(tnum)[self.ccStart:self.ccEnd]
        sad = self.findSAD(self._traceSource.getTrace(tnum))
        self.refmaxloc = np.argmin(sad)
        self.refmaxsize = min(sad)
        self.maxthreshold = np.mean(sad)
