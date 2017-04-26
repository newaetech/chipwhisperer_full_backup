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
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource, ActiveTraceObserver
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.ui.ProgressBar import *
import numpy as np
import logging

class WaveFormWidget(GraphWidget, ResultsBase, ActiveTraceObserver, Plugin):
    _name = 'Trace Output Plot'
    _description = 'Plots the waveform for a given trace source'

    def __init__(self, name=None):
        GraphWidget.__init__(self)
        if name is not None:
            self._name = name
        ActiveTraceObserver.__init__(self)

        self.params.addChildren([
            {'name':'Redraw after Each', 'type':'bool', 'value':False},
            #{'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0), 'value':(0, 0)},
            {'name':'Trace(s) to Plot', 'key':'tracecmd', 'type':'str', 'value':'0'},
            {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0), 'graphwidget':self},
            {'name':'Downsampling Mode', 'key':'dsmode', 'type':'list', 'values':{'None':None, 'Subsample':'subsample', 'Mean':'mean', 'Peak':'peak'},
                'value':'peak', 'action':self.plotInputTrace},
            {'name':'Y Axis', 'type':'group', 'expanded':False, 'children':[
                {'name':'Unity', 'type':'list', 'values':{"None":"", "Voltage":"V", "Current":"A"}, 'value':"", 'action':self.plotInputTrace},
                {'name':'Scale Factor', 'type':'float', 'limits':(1E-9, 1E9), 'value':1.0, 'action':self.plotInputTrace},
                {'name':'Offset Factor', 'type':'float', 'limits':(-1E9, 1E9), 'value':0.0, 'action':self.plotInputTrace},
            ]},
            {'name':'X Axis', 'type':'list', 'values':{"Sample":"Pts.", "Time":"s"}, 'value':"Pts.", 'action':self.plotInputTrace},
            {'name':'T-Statistic', 'key':'tstat', 'type':'group', 'expanded':True, 'children':[
                {'name':'Enable', 'key': 'enable', 'type': 'bool', 'value': False},
                {'name':'Leakage Type', 'key':'type', 'type':'list', 'values':['Text In', 'Text Out', 'Key', 'None'], 'value':'Text In'},
                {'name':'Trace Range', 'key':'range', 'type':'range', 'limits':(0, 0), 'value':(0, 0)},
            ]},
            {'name':'Redraw', 'type':'action', 'action':self.plotInputTrace},
        ])

        self.findParam('input').setValue(TraceSource.registeredObjects["Trace Management"])
        TraceSource.sigRegisteredObjectsChanged.connect(self.traceSourcesChanged)

        self.resetTraceLimits()
        self.setDefaultYRange(-0.5, 0.5)
        self.YDefault()

    def resetTraceLimits(self):
        if self._traceSource:
            lastTrace = self._traceSource.numTraces()-1
            lastPoint = self._traceSource.numPoints()-1
        else:
            lastTrace = -1
            lastPoint = -1

        #traceRange = self.findParam('tracerng').getValue()
        #self.findParam('tracerng').setLimits((0, lastTrace))
        #self.findParam('tracerng').setValue((max(0, traceRange[0]), min(lastTrace, traceRange[1] if traceRange[1] >= 0 else 7)))
        self.findParam('tracecmd').setValue("0")
        self.findParam('pointrng').setLimits((0, lastPoint))
        self.findParam('pointrng').setValue((0, lastPoint))

        mintrace = min(self.getPlotList())
        maxtrace = max(self.getPlotList())

        self.findParam(['tstat', 'range']).setLimits((0, lastTrace))
        self.findParam(['tstat', 'range']).setValue((max(0, mintrace), min(lastTrace, maxtrace) if maxtrace >= 0 else 7))

    def top_level_split(self, s):
        """
        Split `s` by top-level commas only. Commas within parentheses are ignored.

        Source: http://stackoverflow.com/questions/33527245/python-split-by-comma-skipping-the-content-inside-parentheses
        """

        # Parse the string tracking whether the current character is within
        # parentheses.
        balance = 0
        parts = []
        part = ''

        for c in s:
            part += c
            if c == '(':
                balance += 1
            elif c == ')':
                balance -= 1
            elif c == ',' and balance == 0:
                parts.append(part[:-1].strip())
                part = ''

        # Capture last part
        if len(part):
            parts.append(part.strip())

        return parts

    def getPlotList(self):
        #Get range of traces used curently

        tracecmd = self.findParam("tracecmd").getValue()
        cmdparts = self.top_level_split(tracecmd)

        #Example of valid plots:
        #0
        #2
        #0-10
        #0:10, 5, 9
        #5('r', x=2), 9-10

        plotlist = []
        plotinfo = []
        for cmd in cmdparts:

            cmd_tracenum, sep, cmd_traceplt = cmd.partition("(")
            cmd_traceplt = cmd_traceplt.rstrip(")")

            #Parse range
            cmd_tracenumint = cmd_tracenum.replace("-", " ")
            cmd_tracenumint = cmd_tracenumint.replace(":", " ")
            cmd_tracenumint = cmd_tracenumint.replace("to", " ")
            cmd_tracenumint = cmd_tracenumint.split()

            #Parse command
            cmdtracedict = {'color':None,
                            'xoffset':None}

            if cmd_traceplt:
                cmd_traceplotint = self.top_level_split(cmd_traceplt)
                if cmd_traceplotint:
                    cmdtracedict['color'] = cmd_traceplotint[0]

            # Check if we have a range
            if len(cmd_tracenumint) > 1:
                if len(cmd_tracenumint) != 2:
                    raise ValueError("Failed to parse: %s" % cmd_tracenum)
                start = int(cmd_tracenumint[0])
                end = int(cmd_tracenumint[1])
                plotlist.extend(range(start, end))
                plotinfo.extend([cmdtracedict] * (end-start))
            else:
                plotlist.append(int(cmd_tracenumint[0]))
                plotinfo.append(cmdtracedict)


                # TODO: Parse plot information

        return (plotlist, plotinfo)

    def plotInputTrace(self, _=None):
        #print "Plotting %d-%d for points %d-%d"%(params[0].value(), params[1].value(), params[2].value(), params[3].value())
        initialPersist = self.persistant
        if not self.persistant:
            self.clearPushed()

        plotlist, plotinfo = self.getPlotList()
        pstart = self.findParam('pointrng').getValue()[0]
        pend = self.findParam('pointrng').getValue()[1]
        dsmode = self.findParam('dsmode').getValue()
        yaxisScaleFactor = self.findParam(['Y Axis', 'Scale Factor']).getValue()
        yaxisOffsetFactor = self.findParam(['Y Axis', 'Offset Factor']).getValue()

        tstat_enabled = self.findParam(['tstat', 'enable']).getValue()
        ttstart = self.findParam(['tstat', 'range']).getValue()[0]
        ttend = self.findParam(['tstat', 'range']).getValue()[1]

        #Sanity check
        if pend < pstart or plotlist is None or len(plotlist) == 0:
            return

        try:
            progress_bar = None

            if len(plotlist) > 1 or tstat_enabled:
                self.setPersistance(True)

            yUnit = self.findParam(['Y Axis','Unity']).getValue()
            xUnit = self.findParam('X Axis').getValue()
            xScale = self.findParam('X Axis').getValueKey()
            self.pw.setLabel('bottom', text=xScale, units=xUnit)
            self.pw.setLabel('left', units=yUnit)
            xaxis = range(pstart + self._traceSource.offset(), pend + self._traceSource.offset() + 1)
            if xScale == 'Time':
                tmp = float(self._traceSource.getSampleRate())
                if tmp == 0:
                    raise Warning('X Axis of type "Time" in "%s" can\'t have "Sample Rate" with value=0. '
                                  'Change it to its actual value in the Trace Manager.' % self.getName())
                xaxis = [v/tmp for v in xaxis]

            for i, tnum in enumerate(plotlist):
                trace = self._traceSource.getTrace(tnum)
                if trace is not None:
                    if yaxisScaleFactor != 1.0 or yaxisOffsetFactor != 0.0:
                        trace = [yaxisOffsetFactor + x * yaxisScaleFactor for x in trace]
                    #TODO - Not sure if should add _traceSource.offset() or not?
                    traceclr = plotinfo[i]['color']
                    self.passTrace(trace[pstart:pend+1], pstart + self._traceSource.offset(), idString = str(tnum), xaxis=xaxis, dsmode=dsmode, color = traceclr)

                    if self.findParam('Redraw after Each').getValue():
                        util.updateUI()

            if tstat_enabled:
                ttrace = [0] * (pend - pstart + 1)
                tstat_leakage = self.findParam(['tstat', 'type']).getValue()
                if tstat_leakage == 'Text In':
                    b_list = np.array([self._traceSource.getTextin(tnum)[0] for tnum in range(ttstart, ttend+1)])
                elif tstat_leakage == 'Text Out':
                    b_list = np.array([self._traceSource.getTextout(tnum)[0] for tnum in range(ttstart, ttend + 1)])
                elif tstat_leakage == 'Key':
                    b_list = np.array([self._traceSource.getKnownKey(tnum)[0] for tnum in range(ttstart, ttend + 1)])
                else:
                    raise NotImplementedError("Can't calculate t-statistics against input type %s" % tstat_leakage)
                x_list = np.array([bin(x).count('1') for x in b_list])
                s_x = np.sum(x_list)
                mu_x = s_x / len(x_list)

                progress_bar = ProgressBarGUI("Computing T-Statistics", "Calculating T-Statistic:")
                progress_bar.setStatusMask("Current Point = %d", 0)
                progress_bar.setMaximum(pend+1 - pstart)
                progress_bar.show()
                for i in range(pstart, pend+1):
                    y_list = np.array([self._traceSource.getTrace(tnum)[i] for tnum in range(ttstart, ttend+1)])
                    s_y = np.sum(y_list)
                    mu_y = s_y / len(y_list)
                    s_xx = np.sum(np.dot(x_list, x_list))
                    s_xy = np.sum(np.dot(x_list, y_list))

                    b_1 = (s_xy - mu_y * s_x) / (s_xx - s_x**2 / len(x_list))
                    b_0 = mu_y - b_1 * mu_x

                    y_est = b_0 + b_1 * x_list
                    err = y_est - y_list
                    s_ee = np.sum(np.dot(err, err))
                    se = np.sqrt(s_ee / (len(x_list) - 2) / (s_xx - s_x**2 / len(x_list)))

                    t_stat = b_1 / se
                    ttrace[i] = t_stat

                    prog = i+1 - pstart
                    progress_bar.updateStatus(prog, prog)
                    if progress_bar.wasAborted():
                        break

                self.passTrace(ttrace[pstart:pend + 1], pstart + self._traceSource.offset(), idString='ttest', xaxis=xaxis, dsmode=dsmode, color=0.0)
        except NotImplementedError as e:
            # This happens if we can't get text in/out or key from a trace source
            logging.info("Couldn't plot t-statistic; error message:%s" % e)
        finally:
            self.setPersistance(initialPersist)
            if progress_bar is not None:
                progress_bar.close()

    def processTraces(self):
        self.resetTraceLimits()
        self.plotInputTrace()

    def getWidget(self):
        return self