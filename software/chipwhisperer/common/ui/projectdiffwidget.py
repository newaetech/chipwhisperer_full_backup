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
from pyqtgraph.parametertree import Parameter, ParameterTree
from PySide.QtGui import *


class ProjectDiffWidget(QWidget):
    """Widget that displays differences between versions of the project file"""

    def __init__(self, parent, project):
        super(ProjectDiffWidget, self).__init__(parent)

        hlayout = QHBoxLayout()
        hlayout.setContentsMargins(0, 0, 0, 0)
        hlayout.setSpacing(5)

        self.changedTree = ParameterTree()
        self.addedTree = ParameterTree()
        self.deletedTree = ParameterTree()

        self.updateParamTree(self.changedTree, [], "Changed Sections")
        self.updateParamTree(self.addedTree, [], "Added Sections")
        self.updateParamTree(self.deletedTree, [], "Removed Sections")

        hlayout.addWidget(self.changedTree)
        hlayout.addWidget(self.addedTree)
        hlayout.addWidget(self.deletedTree)

        self.setLayout(hlayout)
        self.checkDiff(project)

    def updateParamTree(self, paramTree, changelist, name):
        paramlist = []
        for k in changelist:
            paramlist.append({'name':k})
        params = Parameter.create(name=name, type='group', children=paramlist)
        paramTree.clear()
        paramTree.addParameters(params)

    def checkDiff(self, project):
        """
        Check if there is a difference. Udates widget with overview of the
        differences if requested with updateGUI
        """
        added, removed, changed = project.checkDiff()
        self.updateParamTree(self.changedTree, changed, "Changed Sections")
        self.updateParamTree(self.addedTree, added, "Added Sections (not on disk)")
        self.updateParamTree(self.deletedTree, removed, "Removed Sections (on disk)")
