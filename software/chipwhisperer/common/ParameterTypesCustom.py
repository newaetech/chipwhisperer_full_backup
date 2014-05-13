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
import chipwhisperer.common.qrc_resources

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
        if self.validateLimits():
            self.sigs.sigValueChanged.emit(self)
        
    def svChangingEmit(self, val):
        self.sigs.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()) )
        
    def svLowChanging(self):
        if self.validateLimits("high"):
            self.svChangingEmit(None)
        
    def svHighChanging(self):
        if self.validateLimits("low"):
            self.svChangingEmit(None)
        
    def validateLimits(self, change=0):
        try:
            fixedsize = self.param.opts['fixedsize']
        except KeyError:
            return True
        
        if fixedsize == 0:
            return True
        
        if (self.whigh.value() - self.wlow.value()) != fixedsize:
            
            # Limits wrong! Adjust as needed

            attempts = 0

            while change != "done":
                if change == "low":

                    attempts += 1

                    lowval = self.whigh.value() - fixedsize
                    if self.wlow.valueInRange(lowval):
                        self.wlow.setValue(lowval, update=True, delaySignal=True)
                        change = "done"
                    else:
                        self.wlow.setValue(self.wlow.opts['bounds'][0], update=True, delaySignal=True)
                        change = "high"

                # Prefer to change whigh
                if change == "high":

                    attempts += 1

                    highval = fixedsize + self.wlow.value()
                    if self.whigh.valueInRange(highval):
                        self.whigh.setValue(highval, update=True, delaySignal=True)
                        change = "done"
                    else:
                        self.whigh.setValue(self.whigh.opts['bounds'][1], update=True, delaySignal=True)
                        change = "low"

                # Give up - not possible most likely
                if attempts == 4:
                    return False

            return False
        else:
            return True
        
        
    def makeLayout(self):
        self.sigs = SigStuff()
        opts = self.param.opts        
        defs = {
                'value': 0, 'min': None, 'max': None, 'int': True, 
                'step': 1.0, 'minStep': 1.0, 'dec': False, 'fixedsize':0,
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
        whigh.sigValueChanging.connect(self.svHighChanging)
        wlow.sigValueChanged.connect(self.svChangedEmit)
        wlow.sigValueChanging.connect(self.svLowChanging)        
        
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
        graphIcon.addFile(':/images/wavelimits.png', state=QtGui.QIcon.On)
        graphIcon.addFile(':/images/wavelimitsoff.png', state=QtGui.QIcon.Off)
        
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
        self.pg = opts['graphwidget']
        self.lri = pg.LinearRegionItem(values=(self.wlow.value(), self.whigh.value()))
        self.pg.addPersistantItem(self.lri)
        self.lri.setVisible(False)
        self.lri.sigRegionChanged.connect(self.lriChanged)
        return w
    
    def buttonPressed(self, status):
        self.lri.setVisible(status)
        
    def lriChanged(self):
        new = self.lri.getRegion()
        self.wlow.setValue(new[0])
        self.whigh.setValue(new[1])
        
    def sbChanged(self):
        self.lri.setRegion((self.wlow.value(), self.whigh.value()))
        
    def limitsChanged(self, param, limits):
        """Called when the parameter's limits have changed"""
        # Do we need this? copied from example
        ParameterItem.limitsChanged(self, param, limits)

        self.wlow.setOpts(bounds=limits)
        self.whigh.setOpts(bounds=limits)
        self.lri.setBounds(limits)

    def showEditor(self):
        self.widget.show()
        self.displayLabel.hide()
        self.widget.setFocus(QtCore.Qt.OtherFocusReason)
        self.lri.setVisible(True)

    def hideEditor(self):
        self.widget.hide()
        self.displayLabel.show()
        self.lri.setVisible(self.graphBtn.isChecked())

    
class RangeParameterGraph(Parameter):
    itemClass = RangeParameterGraphItem

registerParameterType('rangegraph', RangeParameterGraph, override=True)
