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

from PySide.QtCore import *
from PySide.QtGui import *
from _base import ResultsWidgetBase


def getClass():
    """"Returns the Main Class in this Module"""
    return ResultsTable


class ResultsTable(QTableWidget, ResultsWidgetBase):
    """Table of results, showing all guesses based on sorting output of attack"""
    group = 'Ranked Table'
    name = 'Results Table'

    def __init__(self, subkeys=16, permPerSubkey=256, useAbs=True):
        ResultsWidgetBase.__init__(self)
        QTableWidget.__init__(self, permPerSubkey+1, subkeys)

        self.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.horizontalHeader().setMinimumSectionSize(51)
        self.horizontalHeader().setResizeMode(QHeaderView.Stretch)

        for i in range(0, subkeys):
            self.setHorizontalHeaderItem(i, QTableWidgetItem("%d" % i))

        for i in range(0, subkeys):
            cell = QTableWidgetItem("-")
            cell.setBackground(QBrush(QColor(253, 255, 205)))
            cell.setFlags(cell.flags() ^ Qt.ItemIsEditable)
            cell.setTextAlignment(Qt.AlignCenter)
            self.setItem(0, i, cell)

        self.setVerticalHeaderItem(0, QTableWidgetItem("PGE"))
        for i in range(1, permPerSubkey+1):
            self.setVerticalHeaderItem(i, QTableWidgetItem("%d" % (i-1)))

        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self.useSingle = False
        self.updateMode = self.findParam('updateMode').value()

    def setupParameters(self):
        return [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.visibilityChanged.emit},
                {'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},
                {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.setSingleMode},
                {'name':'Update Mode', 'key':'updateMode', 'type':'list', 'values':{'Entire Table (Slow)':'all', 'PGE Only (faster)':'pge'}, 'set':self.setUpdateMode},
                ]

    def setUpdateMode(self, mode):
        """Set if we update entire table or just PGE on every statistics update"""
        self.updateMode = mode

    def setBytesEnabled(self, enabledbytes):
        """Set what bytes to include in table"""
        self.enabledBytes = enabledbytes

    def setAbsoluteMode(self, enabled):
        """If absolute mode is enabled, table is sorted based on absolute value of statistic"""
        self.useAbs = enabled

    def setSingleMode(self, enabled):
        """Single mode uses the same point across all traces, not useful normally"""
        self.useSingle = enabled

    def updateTable(self, attackDone=False):
        """Resort data and redraw the table. If update-mode is 'pge' we only redraw entire table
        when 'attackDone' is True."""

        attackStats = self.attack.getStatistics()
        attackStats.setKnownkey(self.attack.knownKey())
        attackStats.findMaximums(useAbsolute=self.useAbs)
        highlights = self.highlightedKey()

        for bnum in range(0, self.numKeys):
            highlightValue = highlights[bnum] if bnum < len(highlights) else None
            if bnum in self.enabledBytes and attackStats.maxValid[bnum]:
                self.setColumnHidden(bnum, False)
                maxes = attackStats.maxes[bnum]

                self.item(0, bnum).setText("%d" % attackStats.pge[bnum])

                if (self.updateMode == 'all') or attackDone:
                    for j in range(0, self.numPerms):
                        cell = QTableWidgetItem("%02X\n%.4f" % (maxes[j]['hyp'],maxes[j]['value']))
                        cell.setFlags(cell.flags() ^ Qt.ItemIsEditable)
                        cell.setTextAlignment(Qt.AlignCenter)
                        self.setItem(j+1, bnum, cell)

                        if maxes[j]['hyp'] == highlightValue:
                            itm = self.item(j+1, bnum)
                            itm.setForeground(QBrush(Qt.red))
            else:
                self.setColumnHidden(bnum, True)
        self.setVisible(True)

    def attackStatsUpdated(self):
        """New attack statistics available, replot/redraw graphs"""
        self.setBytesEnabled(self.attack.bytesEnabled())
        self.updateTable()

    def attackDone(self):
        self.setBytesEnabled(self.attack.bytesEnabled())
        self.updateTable(attackDone=True)

    def attackSettingsChanged(self):
        """Attack settings have changed"""
        self.setAbsoluteMode(self.attack.useAbs)
