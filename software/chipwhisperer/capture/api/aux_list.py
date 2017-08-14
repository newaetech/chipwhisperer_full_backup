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


from collections import OrderedDict

# Probably the CWCoreAPI object should store one of these
class AuxList(object):
    """Class for controlling aux modules within an acquisition controller.

    Possible timings for auxiliary functions are:
    - "before_capture": At the start of a capture campaign
    - "before_arm": Before arming the scope
    - "after_arm": After arming the scope, but before capturing any data
    - "after_trace": After recording a trace
    - "after_capture": At the end of a capture campaign
    """

    valid_timings = [
        "before_capture",
        "before_arm",
        "after_arm",
        "after_trace",
        "after_capture"
    ]

    def __init__(self):
        self._func_list = OrderedDict()
        for t in self.valid_timings:
            self._func_list[t] = []

    def register(self, func, timing):
        """Register a function call with the aux module list.

        Raises:
            ValueError if timing not recognized
            TypeError if func not a function (TODO)
        """
        if timing in valid_timings:
            self._func_list[t].append(func)
        else:
            raise ValueError("Invalid timing provided: expected one of %s" % self.valid_timings, timing)

    def enable(self, func, timing):
        """Enable an auxiliary function call.
        """
        return "todo"

    def disable(self, func, timing):
        """Temporarily disable an auxiliary function.
        """
        return "todo"

    def remove(self, func, timing):
        """Permanently remove an auxiliary function.
        """
        return "todo"

    def list_functions(self):
        """Return a list of all registered auxiliary functions."""
        return "todo"