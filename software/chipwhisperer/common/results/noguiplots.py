#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2018, NewAE Technology Inc
# All rights reserved.
#
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, https://github.com/newaetech/chipwhisperer
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

class NoGUIPlots(object):
    def __init__(self, attack_results=None):
        self._results = attack_results

    def set_results(self, attack_results):
        """ sets results to attack_results
        """
        self._results = attack_results

    setResults = set_results

    def corr_vs_trace(self, bnum):
        """ Gets plot data of correlation for each possible value of subkey
        vs. trace number for a given subkey

        Args:
            bnum (int): Subkey byte to get plot for.

        Returns:
            List of ::
                [
                    xrange
                    [
                        correlation for each possible subkey
                    ]
                ]
        """
        attack_results = self._results
        data = attack_results.maxes_list
        maxdata = data[bnum]
        tlist = []
        for m in maxdata:
            tlist.append(m['trace'])
        maxlist = np.zeros((len(attack_results.diffs[bnum]), len(tlist)))
        for i, m in enumerate(maxdata):
            for j in range(0, len(attack_results.diffs[bnum])):
                maxlist[m['maxes'][j][0], i] = m['maxes'][j][2]

        return [tlist, maxlist]

    corrVsTrace = corr_vs_trace

    def pge_vs_trace(self, bnum):
        """ Gets plot data of partial guessing entropy vs. trace number for a
        given subkey.

        Args:
            bnum (int): Subkey byte to get plot for

        Returns:
            List of ::
                [
                    xrange,
                    pge
                ]
        """
        attack_results = self._results
        pge = attack_results.pge_total
        allpge = {}

        for i in pge:
            tnum = i['trace']
            if not tnum in allpge:
                allpge[tnum] = [{'pgesum':0, 'trials':0} for z in range(0,attack_results.numSubkeys)]

            allpge[tnum][i['subkey']]['pgesum'] += i['pge']
            allpge[tnum][i['subkey']]['trials'] += 1

        for (tnum, plist) in allpge.items():
            for j in plist:
                if j['trials'] > 0:
                    j['pge'] = float(j['pgesum']) / float(j['trials'])
                    # print "%d "%j['trials'],
                else:
                    j['pge'] = None

        trace = []
        pge = []
        for (tnum, plist) in allpge.items():
            if plist[bnum]['pge'] is not None:
                trace.append(tnum)
                pge.append(plist[bnum]['pge'])

        return [trace, pge]

    pgeVsTrace = pge_vs_trace

    def output_vs_time(self, bnum):
        """ Gets plot data of output vs. time (time data is the adc samples)

        Args:
            bnum (int): Subkey byte to get plot for

        Returns:
            List of ::
                [
                    xrange,
                    correlation of correct byte,
                    largest +ve correlation,
                    largest -ve correlation
                ]
        """
        if self._results is None:
            return None

        attack_results = self._results

        key = attack_results.known_key[bnum]
        data = np.array(attack_results.diffs[bnum])

        xrangelist = range(0, len(data[0]))
        wrong_results = data[[i != key for i in range(256)]]

        return [xrangelist, data[key], np.amax(wrong_results, 0), np.amin(wrong_results, 0)]

    def plot_output_vs_time(self, bnum_it=None):
        import holoviews as hv # type: ignore
        from holoviews.operation import decimate # type: ignore
        import pandas as pd, numpy as np # type: ignore
        def byte_to_color(idx):
            return hv.Palette.colormaps['Category20'](idx/16.0)

        if bnum_it is None:
            bnum_it = range(16)
        a = []
        b = []
        hv.extension('bokeh')
        for i in range(16):
            data = self.output_vs_time(i)
            a.append(np.array(data[1]))
            b.append(np.array(data[2]))
            b.append(np.array(data[3]))
            
        #pda = pd.DataFrame(a).transpose().rename(str, axis='columns')
        #pdb = pd.DataFrame(b).transpose().rename(str, axis='columns')
        curve = hv.Curve(b[i], "Sample").options(color='black').options(title="Correlation Vs. Time")
        for i in range(1, len(b)):
            curve *= hv.Curve(b[i]).options(color='black', width=900, height=600)
            
        for i in bnum_it:
            curve *= hv.Curve(a[i]).options(color=byte_to_color(i), width=900, height=600)
        return decimate(curve)

    def plot_pge_vs_trace(self, bnum_it=None):
        import holoviews as hv
        from holoviews.operation import decimate
        import pandas as pd, numpy as np
        hv.extension('bokeh')
        def byte_to_color(idx):
            return hv.Palette.colormaps['Category20'](idx/16.0)

        if bnum_it is None:
            bnum_it = range(16)
        ret = self.pge_vs_trace(bnum_it[0])
        curve = hv.Curve((ret[0],ret[1]), "Traces Used in Calculation", "Partial Guessing Entrop of Byte").options(title="PGE Vs. Traces")
        for bnum in bnum_it[1:]:
            ret = self.pge_vs_trace(bnum)
            curve *= hv.Curve((ret[0],ret[1])).opts(color=byte_to_color(bnum)).opts(width=900, height=600)
        return curve

    def plot_corr_vs_trace(self, bnum_it=None):
        import holoviews as hv
        from holoviews.operation import decimate
        import pandas as pd, numpy as np
        hv.extension('bokeh')
        def byte_to_color(idx):
            return hv.Palette.colormaps['Category20'](idx/16.0)

        if bnum_it is None:
            bnum_it = range(16)

        curve = None
        for bnum in bnum_it:
            data = np.array(self.corr_vs_trace(bnum))[1]
            xrangelist = self.corr_vs_trace(bnum)[0]
            key = self._results.known_key[bnum]
            wrong_results = data[[i != key for i in range(256)]]
            plt = hv.Curve((xrangelist, data[key]), "Traces Used in Calculation", "Correlation").opts(color=byte_to_color(bnum)).\
                        opts(width=900, height=600).options(title="Correlation Vs. Traces")

            if curve is None:
                curve = plt
            else:
                curve *= plt

            curve *= hv.Curve((xrangelist, np.amax(wrong_results, 0))).opts(color='black')

        return curve

    outputVsTime = output_vs_time
