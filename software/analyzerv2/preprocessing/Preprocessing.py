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
#=================================================


import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
from datetime import datetime
import random
import os.path
import shlex
from subprocess import Popen, PIPE
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import scipy
import numpy as np


from ExtendedParameter import ExtendedParameter

try:
    import writer_dpav3
except ImportError:
    writer_dpav3 = None
    writer_dpav3_str = sys.exc_info()

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()
    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

try:
    from Crypto.Cipher import AES
except ImportError:
    AES = None    

from preprocessing.CrossCorrelationResync import CrossCorrelationResync

def listAll(parent):
    valid_targets = {"Disabled":0}
    valid_targets["Cross Correlation Resync"] = CrossCorrelationResync(parent)
    return valid_targets

class Preprocessing(QObject):
    """This is a standard target interface, which controls various supported lower-level hardware interfaces"""
    paramListUpdated = Signal(list)
    
    def __init__(self, parent=None, log=None,showScriptParameter=None):
        super(Preprocessing, self).__init__(parent)
        valid_targets = {"None":None}
        self.parent = parent
        self.driver = None
        self.log=log
        self.showScriptParameter = showScriptParameter
        self.enabled = False
                
        #if preprocessing_CrossCorrResync is not None:
        valid_targets["Cross Correlation Resync"] = CrossCorrelationResync(parent)
        
        self.toplevel_param = {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':valid_targets["None"], 'set':self.setDriver}     

    def setEnabled(self, status):
        self.enabled = status
   
    def setDriver(self, driver):
        self.driver = driver
      
        if self.driver is None:
            self.paramListUpdated.emit(None)
        else:
            self.driver.paramListUpdated.connect(self.paramListUpdated.emit)
            self.paramListUpdated.emit(self.driver.paramList())
            
    def paramList(self):
        if self.driver is None:
            return [None]
        else:
            return self.driver.paramList()
