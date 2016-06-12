#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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

import os

class Settings(object):

    #Default settings all in one handy location
    _settings_dict = {
        "project-home-dir":os.path.join(os.path.expanduser('~'), 'chipwhisperer_projects'),
    }

    _backend = None

    def value(self, name, default=None):
        """Get the value from the settings, if not present return default"""

        #Try the backend first (if available)
        val = None
        if self._backend:
            val = self._backend.value(name, None)

        #Try our local copy next
        if val is None:
            try:
                val = self._settings_dict[name]
            except KeyError:
                val = default

        return val

    def setValue(self, name, value):
        """Set the value"""

        self._settings_dict[name] = value

        #Backend as well
        if self._backend:
            self._backend.setValue(name, value)

    def setBackend(self, settings_backend):
        self._backend = settings_backend