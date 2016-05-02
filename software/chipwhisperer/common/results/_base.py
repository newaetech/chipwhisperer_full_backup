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

from chipwhisperer.common.utils import pluginmanager


class ResultsBase(pluginmanager.Plugin):
    """ Base class for the output widgets """

    def __init__(self, parentParam=None):
        pluginmanager.Plugin.__init__(self, parentParam)

    def getWidget(self):
        return None


class ResultsWidgetBase(ResultsBase):
    """ Base class for the output GUI widgets. You can define a GUI Widget to be inserted in the tools menu """

    def getWidget(self):
        return self