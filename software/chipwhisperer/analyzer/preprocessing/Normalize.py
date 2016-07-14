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

from ._base import PreprocessingBase
import numpy as np


class NormBase(object):
    """Base Class for Normalization"""
    UseF1Coeff = False
    UseF2Coeff = False
    UseZSource = False
    f1coeff = None
    f2coeff = None
    zdata = None

    def loadF1File(self, f):
        pass

    def loadF2File(self, f):
        pass

    def loadZFile(self, f):
        pass


class NormMean(NormBase):
    """Normalize by mean (e.g. make traces zero-mean)"""
    def processTrace(self, t, tindex):
        return t - np.mean(t)


class NormMeanStd(NormBase):
    """Normalize by mean & std-dev """
    def processTrace(self, t, tindex):
        return (t - np.mean(t)) / np.std(t)


try:
    from PySide.QtGui import *

    class NormLinFunc(NormBase):
        """Normalize by two polynomial functions based on additional information"""
        UseF1Coeff = True
        UseF2Coeff = True
        UseZSource = True

        def loadF1File(self, f):
            if f is 0:
                self.f1coeff = 0
            elif f is None:
                f, _ = QFileDialog.getOpenFileName(None, 'F1 Coefficients', '.', '*.npy')
                if f:
                    self.f1coeff = np.load(f)
            else:
                self.f1coeff = np.load(f)

        def loadF2File(self, f):
            if f is 1:
                self.f2coeff = 1
            elif f is None:
                f, _ = QFileDialog.getOpenFileName(None, 'F2 Coefficients', '.', '*.npy')
                if f:
                    self.f2coeff = np.load(f)
            else:
                self.f2coeff = np.load(f)

        def loadZFile(self, f):
            if f is None:
                f, _ = QFileDialog.getOpenFileName(None, 'Z Data File', '.', '*.npy')
                if f:
                    self.zdata = np.load(f)
            else:
                self.zdata = np.load(f)

        def processTrace(self, t, tindex):
            if isinstance(self.f1coeff, (int, long)) and self.f1coeff == 0:
                f1 = 0
            else:
                f1 = np.polyval(self.f1coeff, self.zdata[tindex])

            if isinstance(self.f2coeff, (int, long)) and self.f2coeff == 1:
                f2 = 1
            else:
                f2 = np.polyval(self.f2coeff, self.zdata[tindex])

            return (t - f1) / f2
except:
    class NormLinFunc(NormBase):
        pass


class Normalize(PreprocessingBase):
    """
    Normalize traces by a variety of methods
    """
    _name = "Normalize"
    _description = "Normalize by standard deviation."

    def __init__(self, traceSource=None):
        PreprocessingBase.__init__(self, traceSource)
        self.ptStart = 0
        self.ptEnd = 0
        self.importsAppend("from chipwhisperer.analyzer.preprocessing.Normalize import NormMean, NormMeanStd, NormLinFunc")

        self.params.addChildren([
            {'name':'Type', 'key':'type', 'type':'list', 'values':{"y=x-mean(x)":NormMean, "y=(x-mean(x))/stddev(x)":NormMeanStd, "y=(x-f1(z))/f2(z)":NormLinFunc}, 'value':NormMean, 'action':self.updateNormClass},
            {'name':'F1 Coefficients', 'key':'f1coeff', 'type':'list', 'values':{"N/A":None, "Zero":0, "Load from file":5}, 'value':None, 'action':self.updateScript},
            {'name':'F2 Coefficients', 'key':'f2coeff', 'type':'list', 'values':{"N/A":None, "Unity":1, "Load from file":5}, 'value':None, 'action':self.updateScript},
            {'name':'Z Source', 'key':'zsource', 'type':'list', 'values':{"N/A":None, "Load from file":5}, 'action':self.updateScript},
            # {'name':'Point Range', 'key':'ptrange', 'type':'rangegraph', 'graphwidget':ResultsBase.registeredObjects["Trace Output Plot"], 'action':self.updateScript},
        ])
        self.updateScript()

    def updateScript(self, _=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').getValue())
        self.addFunction("init", "setNormFunc", "%s" % self.findParam('type').getValue().__name__)
        
        f1src = self.findParam('f1coeff').getValue()
        if f1src is not None:
            if f1src == 5: f1src = None
            self.addFunction("init", "norm.loadF1File", "%s" % str(f1src))

        f2src = self.findParam('f2coeff').getValue()
        if f2src is not None:
            if f2src == 5: f2src = None
            self.addFunction("init", "norm.loadF2File", "%s" % str(f2src))

        zsrc = self.findParam('zsource').getValue()
        if zsrc is not None:
            if zsrc == 5: zsrc = None
            self.addFunction("init", "norm.loadZFile", "%s" % str(zsrc))

    #    rng = self.findParam('ptrange').getValue()
    #    if rng:
    #        self.addFunction("init", "setPointRange", " (%d, %d) " % (rng[0], rng[1]))


    # def setPointRange(self, r):
    #    self.ptStart = r[0]
    #    self.ptEnd = r[1]

    # def updateF1Coeffs(self, f):
    #    if f is not None:
    #        if f is 5:
    #            self.norm.loadF1File(None)
    #        else:
    #            self.norm.loadF1File(f)

    # def updateF2Coeffs(self, f):
    #    if f is not None:
    #        if f is 5:
    #            self.norm.loadF2File(None)
    #        else:
    #            self.norm.loadF2File(f)

    # def updateZFile(self, f):
    #    if f is not None:
    #        if f is 5:
    #            self.norm.loadZFile(None)
    #        else:
    #            self.norm.loadZFile(f)

    def setNormFunc(self, cl):
        self.norm = cl()

    def updateNormClass(self, p):
        self.norm = p.getValue()()

        if self.norm.UseF1Coeff:
            self.findParam('f1coeff').setValue(0)
            self.findParam('f1coeff').setReadonly(False)
        else:
            self.findParam('f1coeff').setValue(None)
            self.findParam('f1coeff').setReadonly(True)
            self.delFunction('init', 'norm.loadF1File')

        if self.norm.UseF2Coeff:
            self.findParam('f2coeff').setValue(1)
            self.findParam('f2coeff').setReadonly(False)
        else:
            self.findParam('f2coeff').setValue(None)
            self.findParam('f2coeff').setReadonly(True)
            self.delFunction('init', 'norm.loadF2File')

        if self.norm.UseZSource:
            self.findParam('zsource').setValue(None)
            self.findParam('zsource').setReadonly(False)
        else:
            self.findParam('zsource').setValue(None)
            self.findParam('zsource').setReadonly(True)
            self.delFunction('init', 'norm.loadZFile')

        self.updateScript()

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)

            if trace is None:
                return None
            proc = self.norm.processTrace(trace, n)

            return proc
        else:
            return self._traceSource.getTrace(n)

    # def init(self):
    #    if self.ptEnd == 0:
    #        points = np.shape(self.trace().getTrace(0))[0]
    #        self.findParam('ptrange').setLimits((0, points))
    #        self.findParam('ptrange').setValue((0, points))
    #        self.ptStart = 0
    #        self.ptEnd = points
