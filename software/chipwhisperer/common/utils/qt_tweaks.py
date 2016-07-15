#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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

import PySide.QtCore as qtc
import PySide.QtGui as qtg


class QLineEdit(qtg.QLineEdit):
    """Fixes a bug with Mac OS where the Frame would flicker the second time you call show()"""
    def __init__(self, *args, **kwargs):
        super(QLineEdit, self).__init__(*args, **kwargs)
        self.setAttribute(qtc.Qt.WA_MacShowFocusRect, False)

class QDialog(qtg.QDialog):
    """Makes it easy to show and raise the window at the same time"""
    def show(self, *args, **kwargs):
        super(QDialog, self).show(*args, **kwargs)
        self.raise_()

class QTextBrowser(qtg.QTextBrowser):
    def write(self, text):
        self.moveCursor(qtg.QTextCursor.End)
        self.insertPlainText(text)
        self.moveCursor(qtg.QTextCursor.End)
