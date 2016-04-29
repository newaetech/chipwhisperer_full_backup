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

# To be uses in conjunct with chipwhisperer.common.utils.analysissource/tracesource

from chipwhisperer.common.utils import util, pluginmanager


class ResultsBase(pluginmanager.Plugin):
    """ Base class for the output widgets """

    def __init__(self, parentParam=None):
        super(ResultsBase, self).__init__(parentParam)

    def setupParameters(self):
        ret = [{'name':'Description', 'type':'text', 'value':self.description, 'readonly':True}]
        ret.extend(self._setupParameters())
        return ret

    def _setupParameters(self):
        # PUT YOUR CUSTOMIZED PARAMETERS HERE
        return []

    def getWidget(self):
        return None


class ResultsWidgetBase(ResultsBase):
    """ Base class for the output GUI widgets. You can define a GUI Widget to be inserted in the tools menu """

    def __init__(self, parentParam=None):
        self.visibilityChanged = util.Signal()
        ResultsBase.__init__(self, parentParam)

    def setupParameters(self):
        ret = [{'name':'Description', 'type':'text', 'value':self.description, 'readonly':True},
               {'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.visibilityChanged.emit}
               ]
        ret.extend(self._setupParameters())
        return ret

    def updateVisibility(self, visible):
        """Dock visability changed"""
        self.findParam('show').setValue(visible)

    def getWidget(self):
        return self