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


from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from collections import OrderedDict
import inspect
import textwrap
import linecache

class AuxListObject(Parameterized):
    def __init__(self, func, parent, id):
        super(AuxListObject, self).__init__()
        self._func = func
        self._parent = parent
        self._id = id

        self._name = "Aux Module %d: %s.%s" % (id, func.__module__, func.__name__)
        self._enabled = True

        self.getParams().addChildren([
            {'name':'ID', 'type':'str', 'key':'id', 'value':str(id), 'readonly':True},
            {'name':'Enabled', 'type':'bool', 'key':'enabled', 'get':self.getEnabled, 'set':self.setEnabled},
            {'name':'Preview', 'type':'text', 'key':'code_str', 'get':self._getCodePreview, 'readonly':True},
            {'name':'Remove', 'type':'action', 'action':self.remove}
        ])

    def __str__(self):
        return self._name

    def __repr__(self):
        return self.__str__()

    def _getCodePreview(self):
        try:
            # TODO: maybe there is a better way
            # Important!, the linecache.clearcache call is needed to avoid
            # a very bad situation where the function, and aux module is changed
            # but the inspection of the source uses a cache where the old version of
            # the source code is accessed instead of the new source code
            linecache.clearcache()
            return textwrap.dedent(inspect.getsource(self._func))
        except IOError:
            return "?"

    @property
    def enabled(self):
        return self.getEnabled()

    @enabled.setter
    def enabled(self, val):
        self.setEnabled(val)

    def getEnabled(self):
        return self._enabled

    @setupSetParam("Enabled")
    def setEnabled(self, val):
        self._enabled = val

    @property
    def function(self):
        return self._func

    @property
    def id(self):
        return self._id

    def remove(self, _):
        self._parent._removeObject(self._id)


class AuxList(Parameterized):
    """Class for controlling aux modules within an acquisition controller.

    Possible timings for auxiliary functions are:
    - "set_prefix": At the very start of a Capture Many run
    - "before_capture": At the start of a capture campaign
    - "before_trace": Before each trace, before setting up scope/target
    - "before_arm": Before arming the scope, but after setting up the target
    - "after_arm": After arming the scope, but before capturing any data
    - "after_trace": After recording each trace
    - "after_capture": At the end of a capture campaign

    Most aux functions must accept 3 arguments: the scope object in use, the
    currently connected target, and the project storing the results. Feel free
    to use any features of these 3 objects.

    The one special case is aux functions under the "set_prefix" timing - these
    are only provided with a timestamp describing the start of the capture.
    This function slot is helpful for recording timestamps on auxiliary files.
    """

    # Dict of "API name": "GUI name"
    _valid_timings = OrderedDict([
        ("set_prefix", "Set Prefix"),
        ("before_capture", "Before Capture"),
        ("before_trace", "Before Trace"),
        ("before_arm", "Before Arm"),
        ("after_arm", "After Arm"),
        ("after_trace", "After Trace"),
        ("after_capture", "After Capture")
    ])

    _name = "Aux Settings"

    def __init__(self):
        super(AuxList, self).__init__()

        self._next_id = 1

        # List aux modules and parameter groups for each timing
        self._aux_items = OrderedDict()
        self._param_groups = {}
        for t in list(self._valid_timings.keys()):
            self._aux_items[t] = []
            self._param_groups[t] = Parameter(name=self._valid_timings[t], type='group')
            self.getParams().append(self._param_groups[t])

        self.getParams().addChildren([
            {'name':'Help', 'type':'action', 'action':self._showHelp}
        ])
        self.getParams().refreshAllParameters()

    def _findItem(self, id):
        """Attempt to find a function with this ID.

        Returns: a tuple of (timing, index) such that
            self._aux_items[timing][index].id == id

        Raises: ValueError if no function has this ID
        """
        for t in self._valid_timings:
            for i in range(len(self._aux_items[t])):
                if self._aux_items[t][i].id == id:
                    return (t, i)

        raise ValueError("Couldn't find registered function with this ID", id)

    def _removeObject(self, id):
        """Remove an aux list object from the lists
        """

        (timing, idx) = self._findItem(id)
        aux_obj = self._aux_items[timing][idx]

        aux_obj.getParams().delete()
        del self._aux_items[timing][idx]

    def _showHelp(self, _):
        print("New aux modules can't be added directly from the GUI - see chipwhisperer/capture/scripts for "
                          "examples of aux module setup.")

    def register(self, func, timing, override_class=True):
        """Register a function call with the aux module list.
           By default overrides an existing one with same preview.

        Returns the ID of the registered function.

        Raises:
            ValueError if timing not recognized
            TypeError if func not a function
        """
        if not callable(func):
            raise TypeError("Provided function is not callable" % func)
        if timing not in list(self._valid_timings.keys()):
            raise ValueError("Invalid timing provided: expected one of %s" % list(self._valid_timings.keys()), timing)

        existing_item = None
        if override_class:
            for groups in self._aux_items:
                for item in self._aux_items[groups]:
                    if item:
                        new_funct_string = func.__self__.__class__.__name__ + '.' + func.__name__
                        found_item_string = item.function.__self__.__class__.__name__ + '.' + item.function.__name__

                        if new_funct_string == found_item_string and timing == groups:
                            existing_item = item

        if existing_item is None:
            new_item = AuxListObject(func, self, self._next_id)
            self._next_id += 1

            self._aux_items[timing].append(new_item)
            self._param_groups[timing].append(new_item.getParams())
        else:
            new_item = existing_item
            existing_item._func = func
            self.getParams().refreshAllParameters()

        return new_item.id

    def enable(self, id):
        """Enable an auxiliary function call.

        <id> must be the ID of a registered function call

        Raises:
            ValueError if no registered functions have this ID
        """
        (timing, idx) = self._findItem(id)
        self._aux_items[timing][idx].enabled = True

    def disable(self, id):
        """Temporarily disable an auxiliary function.

        Raises:
            ValueError if no registered functions have this ID
        """
        (timing, idx) = self._findItem(id)
        self._aux_items[timing][idx].enabled = False

    def change_timing(self, id, new_timing):
        """Move an aux module to a different time.

        Raises:
            ValueError if no registered functions have this ID or if new_timing
            is not a valid timing
        """
        if new_timing not in self._valid_timings:
            raise ValueError("Invalid timing provided: expected one of %s" % list(self._valid_timings.keys()), new_timing)

        (timing, idx) = self._findItem(id)

        item = self._aux_items[timing][idx]
        del self._aux_items[timing][idx]
        self._aux_items[new_timing].append(item)
        self._param_groups[new_timing].append(item.getParams())

    def remove(self, id):
        """Permanently remove an auxiliary function.

        Raises:
            ValueError if no registered functions have this ID
        """
        self._removeObject(id)

    def getDict(self, only_enabled=True):
        """Return a dictionary of lists of all registered functions.

        If only_enabled is True, only return functions that are currently
        enabled. Otherwise, return a list of all functions, enabled or not.
        """
        ret = OrderedDict()
        for t in list(self._valid_timings.keys()):
            ret[t] = []
            for item in self._aux_items[t]:
                append_item = True
                if only_enabled:
                    append_item = item.enabled

                if append_item:
                    ret[t].append(item.function)
        return ret