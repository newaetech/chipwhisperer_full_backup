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
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.pluginmanager import Plugin

class PGEVsTraceNoGUI(object):
    def __init__(self, stats):
        self._stats = stats
        
    def calculatePGE(self):
        stats = self._stats
        pge = stats.pge_total
        allpge = util.DictType()
        
        for i in pge:
            tnum = i['trace']
            if not tnum in allpge:
                allpge[tnum] = [{'pgesum':0, 'trials':0} for z in range(0,stats.numSubkeys)]

            allpge[tnum][i['subkey']]['pgesum'] += i['pge']
            allpge[tnum][i['subkey']]['trials'] += 1

        for (tnum, plist) in allpge.items():
            for j in plist:
                if j['trials'] > 0:
                    j['pge'] = float(j['pgesum']) / float(j['trials'])
                    # print "%d "%j['trials'],
                else:
                    j['pge'] = None

        return allpge
    
    def update_stats(self, stats):
        self._stats = stats
        
    def getPlotData(self, bnum):
        allpge = self.calculatePGE()
        trace = []
        pge = []
        for (tnum, plist) in allpge.items():
            if plist[bnum]['pge'] is not None:
                trace.append(tnum)
                pge.append(plist[bnum]['pge'])
         
        return [trace, pge]
