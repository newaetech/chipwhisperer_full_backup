#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# This file based on PyQtGraph parameterTypes.py
#=================================================
from pyqtgraph.Qt import QtCore, QtGui
from pyqtgraph.python2_3 import asUnicode
from pyqtgraph.parametertree.Parameter import Parameter, registerParameterType
from pyqtgraph.parametertree.ParameterItem import ParameterItem
from pyqtgraph.parametertree.parameterTypes import WidgetParameterItem
from pyqtgraph.pgcollections import OrderedDict
from pyqtgraph.widgets.SpinBox import SpinBox
import pyqtgraph as pg

imagepath = '../common/images/'

class SigStuff(QtGui.QWidget):
    sigValueChanged = QtCore.Signal(object)  # (self)
    sigValueChanging = QtCore.Signal(object, object)  # (self, value)  sent immediately; no delay.    

class RangeParameterItem(WidgetParameterItem):
    """
    WidgetParameterItem subclass providing two int selection for range    
    """
   
    def __init__(self, param, depth):
        self.targetValue = None
        WidgetParameterItem.__init__(self, param, depth)

    def svChangedEmit(self):
        self.sigs.sigValueChanged.emit(self)
        
    def svChangingEmit(self, val):
        self.sigs.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()) )
        
    def makeLayout(self):
        self.sigs = SigStuff()
        opts = self.param.opts        
        defs = {
                'value': 0, 'min': None, 'max': None, 'int': True, 
                'step': 1.0, 'minStep': 1.0, 'dec': False, 
                'siPrefix': False, 'suffix': ''
            }
        defs.update(opts)
        if 'limits' in opts:
            defs['bounds'] = opts['limits']
        wlow = SpinBox()
        wlow.setOpts(**defs)
        
        whigh = SpinBox()
        whigh.setOpts(**defs)
        
        whigh.sigValueChanged.connect(self.svChangedEmit)
        whigh.sigValueChanging.connect(self.svChangingEmit)
        wlow.sigValueChanged.connect(self.svChangedEmit)
        wlow.sigValueChanging.connect(self.svChangingEmit)        
        
        l = QtGui.QHBoxLayout()
        l.setContentsMargins(0,0,0,0)
           
        l.addWidget(wlow)
        l.addWidget(QtGui.QLabel(" : "))
        l.addWidget(whigh)      
        
        self.wlow = wlow
        self.whigh = whigh
        return l
    
    def makeWidget(self):
        l = self.makeLayout()

        w = QtGui.QWidget()
        w.setLayout(l)
        
        w.sigChanged = self.sigs.sigValueChanged
        w.sigChanging = self.sigs.sigValueChanging
        w.value = self.value
        w.setValue = self.setValue        
        return w

    def value(self):
        return (self.wlow.value(), self.whigh.value())
            
    def setValue(self, val):
        self.wlow.setValue(val[0])
        self.whigh.setValue(val[1])
        print val

    def limitsChanged(self, param, limits):
        """Called when the parameter's limits have changed"""
        # Do we need this? copied from example
        ParameterItem.limitsChanged(self, param, limits)

        self.wlow.setOpts(bounds=limits)
        self.whigh.setOpts(bounds=limits)


class RangeParameter(Parameter):
    itemClass = RangeParameterItem

registerParameterType('range', RangeParameter, override=True)


class RangeParameterGraphItem(RangeParameterItem):
    def makeWidget(self):
        l = self.makeLayout()
        
        graphIcon = QtGui.QIcon()
        graphIcon.addFile(imagepath+'wavelimits.png', state=QtGui.QIcon.On)
        graphIcon.addFile(imagepath+'wavelimitsoff.png', state=QtGui.QIcon.Off)
        
        self.graphBtn = QtGui.QPushButton()
        self.graphBtn.setFixedWidth(20)
        self.graphBtn.setFixedHeight(20)
        self.graphBtn.setIcon(graphIcon)
        self.graphBtn.setCheckable(True)
        self.graphBtn.clicked[bool].connect(self.buttonPressed)
        
        l.addWidget(self.graphBtn)

        w = QtGui.QWidget()
        w.setLayout(l)
        
        w.sigChanged = self.sigs.sigValueChanged
        w.sigChanging = self.sigs.sigValueChanging
        w.value = self.value
        w.setValue = self.setValue
        
        self.wlow.sigValueChanged.connect(self.sbChanged)
        self.whigh.sigValueChanged.connect(self.sbChanged)
                
        opts = self.param.opts
        self.pw = opts['plotwidget']
        self.lri = pg.LinearRegionItem(values=(self.wlow.value(), self.whigh.value()))
        self.addLri()
        self.lri.setVisible(False)
        self.lri.sigRegionChanged.connect(self.lriChanged)        
        return w
    
    def addLri(self):
        if self.lri not in self.pw.items():
            self.pw.addItem(self.lri)
    
    def buttonPressed(self, status):
        self.addLri()
        self.lri.setVisible(status)
        
    def lriChanged(self):
        self.addLri()
        new = self.lri.getRegion()
        self.wlow.setValue(new[0])
        self.whigh.setValue(new[1])
        
    def sbChanged(self):
        self.lri.setRegion((self.wlow.value(), self.whigh.value()))
        
    
class RangeParameterGraph(Parameter):
    itemClass = RangeParameterGraphItem

registerParameterType('rangegraph', RangeParameterGraph, override=True)
