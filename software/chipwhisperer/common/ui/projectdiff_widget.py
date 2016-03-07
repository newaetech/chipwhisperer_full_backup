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
#=================================================

import inspect
from PySide.QtGui import *
from PySide.QtCore import *
from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter, ParameterTree


def delete_keys_from_dict(dict_del, lst_keys):
    for k in lst_keys:
        try:
            del dict_del[k]
        except KeyError:
            pass
    for v in dict_del.values():
        if isinstance(v, dict):
            delete_keys_from_dict(v, lst_keys)

def delete_objects_from_dict(d):
    #todel = []
    for key, value in d.iteritems():
        if isinstance(value, (dict, list, tuple)):
            delete_objects_from_dict(value)

        elif inspect.ismethod(value) or isinstance(value, QObject):
            #todel.append(key)
            #print key
            d[key] = value.__class__.__name__


    #for k in todel:
    #    del d[k]

class ProjectDiffWidget(QWidget):
    """Widget that displays differences between versions of the project file"""

    def __init__(self, parent=None, project=None):
        super(ProjectDiffWidget, self).__init__(parent)
        self._project = project

        hlayout = QHBoxLayout()

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
        self.checkDiff()

    def updateParamTree(self, paramTree, changelist, name):
        paramlist = []
        for k in changelist:
            paramlist.append({'name':k})
        params = Parameter.create(name=name, type='group', children=paramlist)
        ExtendedParameter.reloadParams([params], paramTree)

    def checkDiff(self, ignored=None, updateGUI=False):
        """
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        added, removed, changed = self._project.checkDiff()
        if updateGUI:
            self.updateParamTree(self.changedTree, changed, "Changed Sections")
            self.updateParamTree(self.addedTree, added, "Added Sections (not on disk)")
            self.updateParamTree(self.deletedTree, removed, "Removed Sections (on disk)")

        if (len(added) + len(removed) + len(changed)) == 0:
            return False
        return True

