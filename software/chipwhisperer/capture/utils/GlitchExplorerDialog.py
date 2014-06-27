#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
from PySide.QtCore import *
from PySide.QtGui import *
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
from openadc.ExtendedParameter import ExtendedParameter

class GlitchExplorerDialog(QDialog):
    def __init__(self, parent):
        super(GlitchExplorerDialog, self).__init__(parent)

        self.setWindowTitle("Glitch Explorer")

        self.parent = parent

        self.mainLayout = QVBoxLayout()
        
        #Add default table
        self.table = QTableWidget(1,1)
        self.mainLayout.addWidget(self.table)
        
        self.glitchParams =[{'name':'Clear Output Table', 'type':'action', 'action':self.clearTable},
                            {'name':'Tuning Parameters ', 'key':'numtune', 'type':'int', 'value':0},
                            {'name':'Normal Response', 'type':'str', 'key':'normalresp', 'value':'s.startswith("Bad")'},
                            {'name':'Successful Response', 'type':'str', 'key':'successresp', 'value':'s.startswith("Welcome")'},
                            ]
        
        self.glitchTuneParamTemplate = [
            {'name':'Name', 'type':'str', 'key':'humanname', 'name':''},
            {'name':'Script Command', 'type':'str', 'key':'script'},
            {'name':'Range','type':'float'},
            {'name':'Step','type':'float','value':1.0},
            {'name':'Repeat', 'type':'int','value':1},
            {'name':'Mode', 'type':'list', 'values':["Linear"]},
            ]
        
        self.params = Parameter.create(name='Glitch Explorer', type='group', children=self.glitchParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)
        
        self.mainLayout.addWidget(self.paramTree)

        self.setLayout(self.mainLayout)
        self.hide()
    
    def clearTable(self, ignored=None):
        pass
    
    def updateParameters(self, ignored=None):
        
        numparams = self.findParam('numtune').value()
        
        #Did number change? Adjust if needed
        
    def updateTableHeaders(self, ignored=None):
        
        headerlist = ["Sent", "Received"]
        
        
    def addResponse(self, resp):
        
        
        normeval = self.findParam('normalresp').value()
        succeval = self.findParam('successresp').value()
        
        if len(normeval) > 0:
            #Check if Normal
            normresult = eval(normeval, {'s':resp}, {})
        else:
            normresult = False
        
        #Check if Successful
        if len(normeval) > 0:
            #Check if Normal
            succresult = eval(normeval, {'s':resp}, {})
        else:
            succresult = False
        
        #Check ?
        if isinstance(normresult, bool) == False:
            raise ValueError("Result of 'normal' eval() not a bool, got %s (result: %s)"%(type(normresult), normresult))
        
        if isinstance(succresult, bool) == False:
            raise ValueError("Result of 'success' eval() not a bool, got %s (result: %s)"%(type(succresult), succresult))
        
        
        if normresult and succresult:
            print "WARNING: Both normresult and succresult True!"
            
            
        


def main():
    # Create the Qt Application
    app = QApplication(sys.argv)

    std = GlitchExplorerDialog(None)
    std.show()

    app.exec_()

if __name__ == '__main__':
    main()
