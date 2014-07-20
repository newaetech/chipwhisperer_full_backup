#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

import imp
import uuid
import tempfile

from PySide.QtCore import *
from PySide.QtGui import *

class ProjectEditor(QTextEdit):

    def __init__(self, parent=None):
        super(ProjectEditor, self).__init__(parent)

        # Use fixed-space font
        font = QFont()
        font.setFamily("Courier")
        font.setStyleHint(QFont.Monospace)
        font.setFixedPitch(True)
        font.setPointSize(10)
        self.setFont(font)

        # Setup code highlighter
        self.highlighter = ProjectHighlighter(self.document())

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Tab:
            self.insertPlainText("    ")
        else:
            return QTextEdit.keyPressEvent(self, event)

    # def contextMenuEvent(self, event):
    #    menu = self.createStandardContextMenu()
    #    menu.insertAction(menu.actions()[0], QAction("Run Function", self, triggered=self.rFuncAct))
    #    # menu.insertAction(menu.actions()[1], QAction("Assign to Toolbar", self, triggered=self.assFuncAct))
    #    menu.insertSeparator(menu.actions()[1])
    #    menu.exec_(event.globalPos())


class ProjectTextEditor(QWidget):
    def __init__(self, parent=None):
        super(ProjectTextEditor, self).__init__(parent)

        self.editWindow = ProjectEditor()

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.editWindow)

        self.setLayout(mainLayout)
        self.lastLevel = 0

        self.saveSliderPosition()

    def setFilename(self, fname):
        self._filename = fname
        self.readFromDisk()

    def readFromDisk(self):
        self.editWindow.clear()
        try:
            with open(self._filename) as fp:
                for line in fp:
                    self.editWindow.moveCursor(QTextCursor.End)
                    self.editWindow.insertPlainText(line)
                    self.editWindow.moveCursor(QTextCursor.End)
        except IOError as e:
            self.editWindow.append("Failed to Open File, Exception:\n%s" % e)

    def writeToDisk(self):
        pass

    def saveSliderPosition(self):
        self._sliderPosition = self.editWindow.verticalScrollBar().value()

    def restoreSliderPosition(self):
        self.editWindow.verticalScrollBar().setValue(self._sliderPosition)


def fontformat(color, style=''):
    """Return a QTextCharFormat with the given attributes.
    """
    _color = QColor()
    _color.setNamedColor(color)

    _format = QTextCharFormat()
    _format.setForeground(_color)
    if 'bold' in style:
        _format.setFontWeight(QFont.Bold)
    if 'italic' in style:
        _format.setFontItalic(True)

    return _format


# Syntax styles that can be shared by all languages
STYLES = {
    'operator': fontformat('blue'),
    'defclass': fontformat('black', 'bold'),
    'string': fontformat('darkMagenta'),
    'string2': fontformat('darkMagenta'),
    'comment': fontformat('darkGreen', 'italic'),
    'class1': fontformat('red', 'bold'),
    'class2': fontformat('magenta', 'bold'),
    'class3': fontformat('magenta', 'bold'),
    'class4': fontformat('magenta', 'bold'),
    'numbers': fontformat('brown'),
}


class ProjectHighlighter (QSyntaxHighlighter):
    """Syntax highlighter for the Python language.
    """
    # Python operators
    operators = [
        '=',
    ]

    def __init__(self, document):
        QSyntaxHighlighter.__init__(self, document)

        # Multi-line strings (expression, flag, style)
        # FIXME: The triple-quotes in these two lines will mess up the
        # syntax highlighting from this point onward
        self.tri_single = (QRegExp("'''"), 1, STYLES['string2'])
        self.tri_double = (QRegExp('"""'), 2, STYLES['string2'])

        rules = []

        # Keyword, operator, and brace rules
        rules += [(r'%s' % o, 0, STYLES['operator'])
            for o in ProjectHighlighter.operators]

        # All other rules
        rules += [
            # Double-quoted string, possibly containing escape sequences
            # (r'"[^"\\]*(\\.[^"\\]*)*"', 0, STYLES['string']),
            # Single-quoted string, possibly containing escape sequences
            # (r"'[^'\\]*(\\.[^'\\]*)*'", 0, STYLES['string']),

            # 'def' followed by an identifier
            # (r'\bdef\b\s*(\w+)', 1, STYLES['defclass']),
            # 'class' followed by an identifier
            # (r'\bclass\b\s*(\w+)', 1, STYLES['defclass']),

            # From '#' until a newline
            (r'#[^\n]*', 0, STYLES['comment']),

            # Numeric literals
            (r'\b[+-]?[0-9]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b', 0, STYLES['numbers']),

            # Top-Level Setting
            (r'^\[(.*)\]', 0, STYLES['class1']),  # \[(.*?)\] #^\[+$
            (r'^\[\[(.*)\]\]', 0, STYLES['class2']),  # \[(.*?)\] #^\[+$
            (r'^\[\[\[(.*)\]\]\]', 0, STYLES['class3']),  # \[(.*?)\] #^\[+$
            (r'^\[\[\[(.*)\]\]\]\]', 0, STYLES['class4']),  # \[(.*?)\] #^\[+$

        ]

        # Build a QRegExp for each pattern
        self.rules = [(QRegExp(pat), index, fmt)
            for (pat, index, fmt) in rules]


    def highlightBlock(self, text):
        """Apply syntax highlighting to the given block of text.
        """
        # Do other syntax formatting
        for expression, nth, format in self.rules:
            index = expression.indexIn(text, 0)

            while index >= 0:
                # We actually want the index of the nth match
                index = expression.pos(nth)
                length = len(expression.cap(nth))
                self.setFormat(index, length, format)
                index = expression.indexIn(text, index + length)

        self.setCurrentBlockState(0)

        # Do multi-line strings
        in_multiline = self.match_multiline(text, *self.tri_single)
        if not in_multiline:
            in_multiline = self.match_multiline(text, *self.tri_double)


    def match_multiline(self, text, delimiter, in_state, style):
        """Do highlighting of multi-line strings. ``delimiter`` should be a
        ``QRegExp`` for triple-single-quotes or triple-double-quotes, and
        ``in_state`` should be a unique integer to represent the corresponding
        state changes when inside those strings. Returns True if we're still
        inside a multi-line string when this function is finished.
        """
        # If inside triple-single quotes, start at 0
        if self.previousBlockState() == in_state:
            start = 0
            add = 0
        # Otherwise, look for the delimiter on this line
        else:
            start = delimiter.indexIn(text)
            # Move past this match
            add = delimiter.matchedLength()

        # As long as there's a delimiter match on this line...
        while start >= 0:
            # Look for the ending delimiter
            end = delimiter.indexIn(text, start + add)
            # Ending delimiter on this line?
            if end >= add:
                length = end - start + add + delimiter.matchedLength()
                self.setCurrentBlockState(0)
            # No; multi-line string
            else:
                self.setCurrentBlockState(in_state)
                length = len(text) - start + add
            # Apply formatting
            self.setFormat(start, length, style)
            # Look for the next match
            start = delimiter.indexIn(text, start + length)

        # Return True if still inside a multi-line string, False otherwise
        if self.currentBlockState() == in_state:
            return True
        else:
            return False



