#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
import logging

from chipwhisperer.common.utils.tracesource import TraceSource, PassiveTraceObserver
from chipwhisperer.common.utils.parameter import setupSetParam
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.util import camel_case_deprecated
from chipwhisperer.common.api.ProjectFormat import Project
from chipwhisperer.common.traces import Trace

from tqdm import trange # type: ignore
class PreprocessingBase(TraceSource, PassiveTraceObserver):
    """
    Base Class for all preprocessing modules
    Derivable Classes work like this:
        - updateScript is called to update the scripts based on the current status of the object
        - the other methods are used by the API to apply the preprocessing filtering
    """
    _name = "None"

    def __init__(self, traceSource=None, name=None):
        self._enabled = False
        PassiveTraceObserver.__init__(self)
        if name is None:
            TraceSource.__init__(self, self.getName())
        else:
            TraceSource.__init__(self, name=name)
        if isinstance(traceSource, Project):
            traceSource = traceSource.trace_manager()
        self.setTraceSource(traceSource, blockSignal=True)
        if traceSource:
            #until new analyzer is implemented

            traceSource.sigTracesChanged.connect(self.sigTracesChanged.emit)  # Forwards the traceChanged signal to the next observer in the chain
        self.getParams().addChildren([
                 {'name':'Enabled', 'key':'enabled', 'type':'bool', 'default':self._getEnabled(), 'get':self._getEnabled, 'set':self._setEnabled}
        ])
        self.findParam('input').hide()

        self.register()
        if __debug__: logging.debug('Created: ' + self._name)

        #Old attribute dict
        self._attrdict = None
        self.enabled = True

    def _getEnabled(self):
        """Return if it is enable or not"""
        return self._enabled

    @setupSetParam("Enabled")
    def _setEnabled(self, enabled):
        """Turn on/off this preprocessing module"""
        self._enabled = enabled

    @property
    def enabled(self):
        """Whether this module is active.

        If False, the module will have no effect on the traces - it will just
        pass through the traces from the previous source.

        Setter raises TypeError if value isn't bool.
        """
        return self._getEnabled()

    @enabled.setter
    def enabled(self, en):
        if not isinstance(en, bool):
            raise TypeError("Expected bool; got %s" % type(en), en)
        self._setEnabled(en)

    def get_trace(self, n):
        """Get trace number n"""
        if self.enabled:
            trace = self._traceSource.get_trace(n)
            # Do your preprocessing here
            return trace
        else:
            return self._traceSource.get_trace(n)

    getTrace = camel_case_deprecated(get_trace)
    def get_textin(self, n):
        """Get text-in number n"""
        return self._traceSource.get_textin(n)

    getTextin = camel_case_deprecated(get_textin)

    def get_textout(self, n):
        """Get text-out number n"""
        return self._traceSource.get_textout(n)

    getTextout = camel_case_deprecated(get_textout)

    def get_known_key(self, n=None):
        """Get known-key number n"""
        return self._traceSource.get_known_key(n)

    getKnownKey = camel_case_deprecated(get_known_key)

    def getSampleRate(self):
        """Get the Sample Rate"""
        return self._traceSource.getSampleRate()

    def init(self):
        """Do any initialization required once all traces are loaded"""
        pass

    def getSegmentList(self):
        return self._traceSource.get_segment_list()

    def getAuxData(self, n, auxDic):
        return self._traceSource.getAuxData(n, auxDic)

    def get_segment(self, n):
        return self._traceSource.getSegment(n)

    getSegment = camel_case_deprecated(get_segment)

    def num_traces(self):
        return self._traceSource.num_traces()

    numTraces = camel_case_deprecated(num_traces)

    def num_points(self):
        return self._traceSource.numPoints()

    numPoints = camel_case_deprecated(num_points)
    def attrSettings(self):
        """Return user-added attributes, used in determining cache settings"""

        if self.__dict__ == self._attrdict:
            return self._attrdict_trimmed

        self._attrdict = self.__dict__.copy()
        attrdict = self.__dict__.copy()

        del attrdict["_attrdict"]
        if hasattr(attrdict, "_attrdict_trimmed"): del attrdict["_attrdict_trimmed"]
        del attrdict["runScriptFunction"]
        del attrdict["sigTracesChanged"]
        del attrdict["_smartstatements"]
        del attrdict["_traceSource"]
        attrdict["params"] = str(attrdict["params"])
        del attrdict["scriptsUpdated"]
        del attrdict["updateDelayTimer"]

        self._attrdict_trimmed = attrdict
        return self._attrdict_trimmed

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

    def _dict_repr(self):
        #raise NotImplementedError("Must define target-specific properties.")
        return {}

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def preprocess(self):
        """Process all traces.

        Returns:
            Project: A new project containing the processed traces.

        .. versionadded: 5.1
            Add preprocess method to Preprocessing module.
        """
        proj = Project()

        for i in trange(self.num_traces()):
            if self.get_trace(i) is None:
                logging.warn("Wave {} ({}) is invalid. Skipping ".format(i, self.get_trace(i)))
                continue
            proj.traces.append(Trace(self.get_trace(i), self.get_textin(i),
                                self.get_textout(i), self.get_known_key(i)))
        return proj
