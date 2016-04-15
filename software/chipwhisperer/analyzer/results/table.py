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


class ResultsTable(QTableWidget, ResultsWidgetBase):
    """Table of results, showing all guesses based on sorting output of attack"""
    name = 'Results Table'

    def __init__(self, subkeys=16, permPerSubkey=256, useAbs=True):
        ResultsWidgetBase.__init__(self)
        QTableWidget.__init__(self, permPerSubkey+1, subkeys)

        self.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.horizontalHeader().setMinimumSectionSize(51)
        self.horizontalHeader().setResizeMode(QHeaderView.Stretch)
        self.setRowCount(permPerSubkey+1)
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self.useSingle = False
        self.updateMode = self.findParam('updateMode').value()

        stdCell = QTableWidgetItem("")
        stdCell.setFlags(stdCell.flags() ^ Qt.ItemIsEditable)
        stdCell.setTextAlignment(Qt.AlignCenter)

        for x in range(0, self.numKeys):
            self.setHorizontalHeaderItem(x, QTableWidgetItem("%d" % x))
            cell = stdCell.clone()
            cell.setText("-")
            cell.setBackground(QBrush(QColor(253, 255, 205)))
            self.setItem(0, x, cell)
            for y in range(1, self.numPerms+1):
                self.setItem(y, x, stdCell.clone())

        self.setVerticalHeaderItem(0, QTableWidgetItem("PGE"))
        for y in range(1, self.numPerms+1):
            self.setVerticalHeaderItem(y, QTableWidgetItem("%d" % (y-1)))

    def _setupParameters(self):
        return [{'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},
                {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.setSingleMode},
                {'name':'Update Mode', 'key':'updateMode', 'type':'list', 'values':{'Entire Table (Slow)':'all', 'PGE Only (faster)':'pge'}, 'set':self.setUpdateMode},
                ]

    def clearTableContents(self):
        for x in range(0, self.numKeys):
            self.item(0, x).setText("-")
            for y in range(1, self.numPerms+1):
                self.item(y, x).setText("")

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

    def updateTable(self, everything=False):
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
                if everything:
                    for j in range(0, self.numPerms):
                        cell = self.item(j+1, bnum)
                        cell.setText("%02X\n%.4f" % (maxes[j]['hyp'], maxes[j]['value']))
                        if maxes[j]['hyp'] == highlightValue:
                            cell.setForeground(QBrush(Qt.red))
                        else:
                            cell.setForeground(QBrush(Qt.black))
            else:
                self.setColumnHidden(bnum, True)
        self.setVisible(True)


    def attackStarted(self):
        self.clearTableContents()

    def attackStatsUpdated(self):
        """New attack statistics available, replot/redraw graphs"""
        self.setBytesEnabled(self.attack.bytesEnabled())
        self.updateTable(everything=(self.updateMode == 'all'))

    def attackDone(self):
        self.setBytesEnabled(self.attack.bytesEnabled())
        self.updateTable(everything=True)

    def attackSettingsChanged(self):
        """Attack settings have changed"""
        self.setAbsoluteMode(self.attack.useAbs)
