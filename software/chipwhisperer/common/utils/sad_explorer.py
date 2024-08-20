#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024, NewAE Technology Inc
# All rights reserved.
#
# Authors: Jean-Pierre Thibault
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
import numpy as np
import math
import datetime
import re

from bokeh.palettes import inferno
from bokeh.plotting import figure, show
from bokeh.resources import INLINE
from bokeh.io import push_notebook
from bokeh.models import Span, Legend, LegendItem
from ipywidgets import interact_manual, Layout, widgets

import itertools
from chipwhisperer.common.utils import util
from chipwhisperer.logging import *
import chipwhisperer as cw


class SADExplorer(util.DisableNewAttr):
    """Class to help understand SAD and tune its parameters.
    For usage and tips, see the companion Jupyter notebook.

    Example::

        explorer = cw.SADExplorer(scope, target, reftrace.wave, refstart, max_segments)
    """

    _name = 'Husky SAD Explorer Module'

    def __init__(self, scope, target, reftrace, refstart, max_segments=1, plot_width=2000, plot_height=600, plot_tools='pan, box_zoom, hover, reset, save', capture_function=None):
        super().__init__()
        self.scope = scope
        self.target = target
        self.SAD = scope.SAD
        self.reftrace = reftrace
        self.refstart = refstart
        self.max_segments = max_segments
        self._max_legend_segments = 10
        self.capture_function = capture_function
        self.p = figure(plot_width=plot_width, plot_height=plot_height, tools=plot_tools)

        self.samples = 100 # initial value really doesn't matter
        xrange = range(self.samples)
        colors = itertools.cycle(inferno(self.max_segments))
        self.S = []
        for i in range(self.max_segments):
            self.S.append(self.p.line(xrange, [0]*self.samples, color=next(colors)))

        # TODO: to indicate which samples are disabled:
        self.quads = []

        # intial legend shows what to do:
        self.legend = self.create_legend()
        self.p.add_layout(self.legend)

        # show interval threshold:
        self.Rm = self.p.line(xrange, [0]*self.samples, line_color='black', line_width=1, line_dash='dotted')
        self.Rp = self.p.line(xrange, [0]*self.samples, line_color='black', line_width=1, line_dash='dotted')
        self.Rf = self.p.varea(x=xrange, y1=[0]*self.samples, y2=[0]*self.samples, fill_color='black', fill_alpha=0.1)

        # vertical line to indicate end of SAD reference:
        self.V = Span(location=self.SAD.sad_reference_length, dimension='height', line_color='black', line_width=2)
        self.p.renderers.extend([self.V])

        self.textout = widgets.Output(layout={'border': '1px solid black'})
        self.captureout = widgets.Output(layout={'border': '1px solid black'})

        show(self.p, notebook_handle=True)

        style= {'description_width': '300px'}
        layout=Layout(width='600px')

        my_interact_manual = interact_manual.options(manual_name="run SAD capture")
        my_interact_manual(self.update_plot, 
                           refstart =           widgets.Text(value=str(refstart), description='reference start sample', style=style, layout=layout),
                           samples =            widgets.Text(value=str(scope.SAD.sad_reference_length), description='scope.adc.samples', style=style, layout=layout),
                           segments =           widgets.Text(value=str(max_segments), description='scope.adc.segments', style=style, layout=layout),
                           threshold =          widgets.Text(value=str(scope.SAD.threshold), description='scope.SAD.threshold', style=style, layout=layout),
                           interval_threshold = widgets.Text(value=str(scope.SAD.interval_threshold), description='scope.SAD.interval_threshold', style=style, layout=layout),
                           exclude =            widgets.Text(value='', description='excluded samples', style=style, layout=layout),
                           emode =              widgets.Checkbox(value=scope.SAD.emode, description='scope.SAD.emode', style=style, layout=layout), 
                           always_armed =       widgets.Checkbox(value=scope.SAD.always_armed, description='scope.SAD.always_armed', style=style, layout=layout), 
                           show_diffs =         widgets.Checkbox(value=False, description='show diff', style=style, layout=layout),
                           show_text_legend =   widgets.Checkbox(value=False, description='show text legend (slow)', style=style, layout=layout),
                           show_plot_legend =   widgets.Checkbox(value=False, description='show plot legend (slower)', style=style, layout=layout),
                           legend_sad_stats =   widgets.Checkbox(value=False, description='include SAD stats in legend (slowest)', style=style, layout=layout))

        display(self.textout)
        display(self.captureout)
        self.disable_newattr()

    @property
    def legend_segments(self):
        return min(self._max_legend_segments, self.max_segments)


    def calc_sad(self, wave):
        sad = 0
        exceeds = []
        #return 1,2
        # TODO: is the astype(int) here what's slowing things down?
        for r,w in zip(self.reftrace[self.refstart:self.refstart+self.SAD.sad_reference_length].astype(int), wave[:self.SAD.sad_reference_length]):
        #for r,w in zip([0]*self.SAD.sad_reference_length, wave[:self.SAD.sad_reference_length]):
            diff = abs(r-w)
            exceeds.append(diff)
            if diff > self.SAD.interval_threshold:
                sad += 1
        return sad, max(exceeds)


    def get_legend_items(self, segments, sad_stats):
        items = []
        ttimes = self.scope.trigger.get_trigger_times()
        sads = []
        for i in range(min(len(segments), self.legend_segments)):
        #for i in range(self.legend_segments):
            if sad_stats:
                sad, max_exceeds = self.calc_sad(segments[i])
                sads.append(sad)
            if i == 0:
                delta = 0
            elif ttimes:
                delta = ttimes[i-1]
            else:
                delta = 0
            if sad_stats:
                items.append('segment %d: SAD=%d, max over ref=%d, delta=%d' % (i, sad, max_exceeds, delta))
            else:
                items.append('segment %d: delta=%d' % (i, delta))
        if sad_stats:
            items.append('SADs: %d / %d / %d' % (min(sads), max(sads), int(np.average(sads))))
        else:
            items.append('SADs: N/A')

        if ttimes:
            items.append('trigger times: %d / %d / %d' % (min(ttimes), max(ttimes), int(np.average(ttimes))))
        else:
            items.append('trigger times: N/A')
        items.append('scope.SAD.num_triggers_seen: %d' % self.scope.SAD.num_triggers_seen)
        items.append('scope.adc.errors: %s' % self.scope.adc.errors)
        return items
                         

    def create_legend(self):
        legend_items = [ LegendItem(label='', renderers=[]) ]*(self.legend_segments + 4)
        return Legend(items=legend_items)


    def update_plot(self, 
                    refstart='', 
                    samples='', 
                    segments='', 
                    threshold='', 
                    interval_threshold='', 
                    exclude='', 
                    emode=False, 
                    always_armed=False, 
                    show_diffs=False, 
                    show_text_legend=False, 
                    show_plot_legend=False,
                    legend_sad_stats=False):
        segments = int(segments)
        threshold = int(threshold)
        interval_threshold = int(interval_threshold)
        self.scope.adc.segments = segments
        self.SAD.threshold = threshold
        self.SAD.interval_threshold = interval_threshold
        self.SAD.emode = emode
        self.SAD.always_armed = always_armed

        # visually indicate that a new capture has started::
        self.p.background_fill_color = 'yellow'
        self.p.background_fill_alpha = 0.3
        push_notebook()

        samples = int(samples)
        self.samples = samples
        refstart = int(refstart)
        if self.refstart != refstart: # no need to update if it hasn't changed
            self.refstart = refstart
            with self.captureout:
                self.SAD.reference = self.reftrace[refstart:]

        presamples = self.SAD.sad_reference_length + self.SAD.latency
        samples = max(samples, presamples+2)
        samples = samples + 3 - (samples%3)

        self.scope.adc.presamples = 0
        self.scope.adc.samples = samples
        self.scope.adc.presamples = presamples

        self.captureout.clear_output()
        trace = None
        with self.captureout:
            if self.capture_function is None:
                trace = cw.capture_trace(self.scope, self.target, bytearray(16), bytearray(16), as_int=True)
            else:
                trace = self.capture_function()
            if self.scope.adc.errors:
                print('scope.adc.errors: %s' % self.scope.adc.errors)

        if show_diffs:
            self.Rp.data_source.data = {'y': [interval_threshold]*samples,
                                        'x': range(samples)}
            self.Rm.data_source.data = {'y': [0]*samples,
                                        'x': range(samples)}
            self.Rf.data_source.data = {'y1': [0]*samples,
                                        'y2': [interval_threshold]*samples,
                                        'x': range(samples)}

        else:
            # prevent over/underflow!
            top = self.reftrace[refstart:refstart+samples] + interval_threshold
            bottom = self.reftrace[refstart:refstart+samples] - interval_threshold

            top[top < self.reftrace[refstart:refstart+samples]] = 255
            bottom[bottom > self.reftrace[refstart:refstart+samples]] = 0

            self.Rp.data_source.data = {'y': top,
                                        'x': range(samples)}
            self.Rm.data_source.data = {'y': bottom,
                                        'x': range(samples)}
            self.Rf.data_source.data = {'y1': bottom,
                                        'y2': top,
                                        'x': range(samples)}

        if trace is None:
            self.p.background_fill_color = 'red'
            self.p.background_fill_alpha = 0.7
            push_notebook()
            segments = []

        else:
            self.p.background_fill_color = 'green'
            push_notebook()
            segments = []
            for i in range(self.scope.adc.segments):
                segments.append(trace.wave[i*self.scope.adc.samples:(i+1)*self.scope.adc.samples])

            for i in range(self.scope.adc.segments):
                if show_diffs:
                    self.S[i].data_source.data = {'y': abs(segments[i][:samples].astype(int) - self.reftrace[refstart:refstart+samples].astype(int)),
                                                  'x': range(samples)}
                else:
                    self.S[i].data_source.data = {'y': segments[i][:samples],
                                                  'x': range(samples)}

            if self.scope.adc.segments < self.max_segments:
                for i in range(self.scope.adc.segments, self.max_segments):
                    self.S[i].data_source.data = {'y': [0]*samples,
                                                  'x': range(samples)}

            self.p.renderers.remove(self.V)
            self.V = Span(location=self.SAD.sad_reference_length, dimension='height', line_color='black', line_width=2)
            self.p.renderers.extend([self.V])

            self.textout.clear_output()
            if show_text_legend or show_plot_legend:
                items = self.get_legend_items(segments, legend_sad_stats)

            if show_text_legend:
                with self.textout:
                    for i in items:
                        print(i)

            if show_plot_legend:
                self.p.legend.visible = True
                for i in range(self.legend_segments):
                    if i < self.scope.adc.segments:
                        self.p.legend.items[i] = LegendItem(label=items[i], renderers=[self.S[i]])
                    else:
                        self.p.legend.items[i] = (LegendItem(label='', renderers=[]))
                # update the legend min/max lines:
                self.p.legend.items[-4] = LegendItem(label=items[-4], renderers=[])
                self.p.legend.items[-3] = LegendItem(label=items[-3], renderers=[])
                self.p.legend.items[-2] = LegendItem(label=items[-2], renderers=[])
                self.p.legend.items[-1] = LegendItem(label=items[-1], renderers=[])
            else:
                self.p.legend.visible = False


        # TODO- re-add
        #if len(segments) > 0:
        #    eindices = parse_list_of_ints(exclude)    
        #    loy = min(segments[0])
        #    hiy = max(segments[0])
        #    # clear any previous quads... not pretty, but it works
        #    while self.quads:
        #        q = self.quads.pop()
        #        #q.visible = False
        #        self.p.renderers.remove(q)
        #        #q.data_source.data['top'] = q.data_source.data['bottom']
        #        #q.data_source.data['alpha'] = 0
        #        #q.destroy()
        #    for i in eindices:
        #        self.quads.append(self.p.quad(left=i[0], bottom=loy, right=i[1], top=hiy, color='grey', alpha=0.4))


        if trace is not None:
            self.p.background_fill_color = 'white'

        push_notebook()
        if self.scope.adc.errors:
            self.scope.errors.clear()


    def parse_list_of_ints(l):
        range_regex = re.compile(r'(\d+):(\d+)')
        a = []
        if l != '':
            splits = l.split(',')
            for s in splits:
                try:
                    n = int(s)
                    a.append([n,n+1])
                except ValueError:
                    range_match = range_regex.search(s)
                    if range_match:
                        start = int(range_match.group(1))
                        stop  = int(range_match.group(2))
                        if stop <= start:
                            raise ValueError('Cannot parse %s' % s)
                        a.append([start,stop])
                    else:
                        raise ValueError('Cannot parse %s' % s)
        return a
