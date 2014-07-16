#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
from pyqtgraph.parametertree import Parameter, ParameterTree
from chipwhisperer.common.GraphWidget import GraphWidget

#Setup scripts/examples
from chipwhisperer.analyzer.utils.TraceExplorerScripts.PartitionDisplay import PartitionDisplay

from chipwhisperer.common.autoscript import AutoScript

class TraceExplorerDialog(QDialog, AutoScript):
    """Open dialog to explore trace properties, data graphs, etc"""

    def __init__(self, parent):
        super(TraceExplorerDialog, self).__init__(parent)
        self.autoScriptInit()

        self.setWindowTitle("Trace Explorer")
        self.setObjectName("Trace Explorer")

        self.qwindow = QMainWindow()
        self.parent = parent

        self._tmanager = None
        self._project = None

        # We want to use dock widgets, but need a QMainWindow for this to work
        # thus we place a QMainWindow inside the QDialog. Great Success.
        layout = QVBoxLayout()
        layout.addWidget(self.qwindow)
        self.setLayout(layout)

        # Add example scripts to this list
        self.exampleScripts = [PartitionDisplay(self)]

        # Add Scripts
        self.setupCommonScripts()

        self.qwindow.setParent(self)

        self.graphDockList = []
        self.getGraphWidgets(["Basic Plot"])

        self.progressBar = QProgressDialog(self)
        self.progressBar.setWindowModality(Qt.WindowModal)


    def setupCommonScripts(self):
        # Setup parameter tree
        
        self.commonScriptParams = []
        
        for example in self.exampleScripts:
            self.commonScriptParams.append({'name':example.name, 'type':'group', 'children':example.params})
            example.scriptsUpdated.connect(self.updateScripts)
            example.runScriptFunction.connect(self.runScriptFunction.emit)
        

        self.paramCommonScripts = Parameter.create(name='Common Scripts', type='group', children=self.commonScriptParams)
        ExtendedParameter.setupExtended(self.paramCommonScripts, self)
        self.paramTreeCommonScripts = ParameterTree()
        self.paramTreeCommonScripts.setParameters(self.paramCommonScripts, showTop=False)

        self.commonScriptsDock = self.addDock(self.paramTreeCommonScripts, "Common Scripts")

        self.params = self.paramCommonScripts

        self.updateScripts()

    def addDock(self, dockWidget, name="Settings", area=Qt.LeftDockWidgetArea, allowedAreas=Qt.TopDockWidgetArea | Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea | Qt.LeftDockWidgetArea):
        """Add a dockwidget to the main window"""
        # Configure dock
        dock = QDockWidget(name)
        dock.setAllowedAreas(allowedAreas)
        dock.setWidget(dockWidget)
        dock.setObjectName(name)
        self.qwindow.addDockWidget(area, dock)
        return dock

    def addTraceDock(self, name):
        """Add a new GraphWidget in a dock, you can get the GW with .widget() property of returned QDockWidget"""
        gw = GraphWidget()
        return self.addDock(gw, name=name, area=Qt.RightDockWidgetArea)


    def setTraceSource(self, tmanager):
        """Set the input trace source"""
        self._tmanager = tmanager
        
    def setProject(self, project):
        self._project = project

    def project(self):
        return self._project

####COMMON SCRIPTING STUFF

    def traceManager(self):
        """Get the trace information"""
        return self._tmanager
    
    def getGraphWidgets(self, nameList=["Unknown"]):
        """Setup graph widgets (e.g. graphs) in the Window, and return a reference to them"""

        # Close/Clear all widgets
        for gd in self.graphDockList:
            gd.close()
            gd.widget().clearPushed()
            gd.widget().clearCustomWidgets()
            gd.widget().setDefaults()

        for i in range(0, min(len(self.graphDockList), len(nameList))):
            self.graphDockList[i].show()

        while len(self.graphDockList) < len(nameList):
            self.graphDockList.append(self.addTraceDock("TempName"))

        for i, name in enumerate(nameList):
            self.graphDockList[i].setObjectName(name)
            self.graphDockList[i].setWindowTitle(name)

        widgetList = []
        for gd in self.graphDockList:
            widgetList.append(gd.widget())

        return widgetList

    def getProgressIndicator(self):
        return self.progressBar

    def updateScripts(self):
        for index, example in enumerate(self.exampleScripts):
            if hasattr(example, "_smartstatements"):
                for k in example._smartstatements:
                    statements = example.getStatements(k)                    
                    if len(statements) > 0:
                        prefix = example.__class__.__name__ + "_"
                        self._smartstatements[prefix + k] = example._smartstatements[k]
                        self._smartstatements[prefix + k].addSelfReplacement("exampleScripts[%d]." % index)
                        
                for k in example.getImportStatements():
                    self.importsAppend(k)

        self.scriptsUpdated.emit()
        
        

                
        


