#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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

from .base import ResultsBase
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized


class Scatterplot(GraphWidget, ResultsBase, Parameterized, Plugin):
    _name = 'Scatter Plot'
    _description = 'Use Cartesian coordinates to display values for typically two variables for a set of data'

    def __init__(self, name=None):
        GraphWidget.__init__(self)
        if name is not None:
            self._name = name

        self.getParams().addChildren([
            {'name':'Symbol', 'type':'list', 'values':['o', 's', 't', 'd', '+'], 'value':'o'},
        ])
        self.setDefaultYRange(-0.5, 0.5)

    def plot(self, posX, posY, color, label):
        self.setupPlot(self.pw.plot(posX, posY,pen=None, symbol=self.findParam("Symbol").getValue(), symbolBrush=color), 1, True, label)

    def getWidget(self):
        return self