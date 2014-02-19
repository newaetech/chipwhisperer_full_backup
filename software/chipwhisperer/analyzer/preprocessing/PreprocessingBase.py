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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from openadc.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter

class PreprocessingBase(QObject):
    """
    Base Class for all preprocessing modules
    """
    paramListUpdated = Signal(list)

    descrString = "The person who made this module failed to supply a descrString. They made a bad module" \
                  " and should feel bad."

    def __init__(self, parent, console=None, showScriptParameter=None):
        """Pass None/None if you don't have/want console/showScriptParameter"""
        super(PreprocessingBase, self).__init__()
        self.console = console
        self.showScriptParameter = showScriptParameter
        self.parent = parent
        self.NumTrace = 1
        self.enabled = True
        self.setTraceSource(parent.manageTraces.iface)
        self.setupParameters()

    def setupParameters(self):
        """Setup parameters specific to preprocessing module"""
        ssParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                    # PUT YOUR PARAMETERS HERE
                    {'name':'Desc', 'type':'text', 'value':self.descrString}]
        self.params = Parameter.create(name='Name of Module', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)

    def paramList(self):
        """Returns the parameter list"""
        return [self.params]

    def setEnabled(self, enabled):
        """Turn on/off this preprocessing"""
        self.enabled = enabled

    def getTrace(self, n):
        """Get trace number n"""
        if self.enabled:
            trace = self.trace.getTrace(n)

            # Do your preprocessing here

            return trace
        else:
            return self.trace.getTrace(n)

    def getTextin(self, n):
        """Get text-in number n"""
        return self.trace.getTextin(n)

    def getTextout(self, n):
        """Get text-out number n"""
        return self.trace.getTextout(n)

    def getKnownKey(self, n=None):
        """Get known-key number n"""
        return self.trace.getKnownKey()

    def init(self):
        """Do any initilization required once all traces are loaded"""
        pass

    def setTraceSource(self, tmanager):
        """Set the input trace source"""
        self.trace = tmanager



