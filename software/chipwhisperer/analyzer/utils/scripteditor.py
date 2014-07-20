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

from chipwhisperer.analyzer.utils.syntaxhighlighter import PythonHighlighter

class CodeEditor(QTextEdit):

    runFunction = Signal(str)
    assignFunction = Signal(str)

    def __init__(self, parent=None):
        super(CodeEditor, self).__init__(parent)

        # Use fixed-space font
        font = QFont()
        font.setFamily("Courier")
        font.setStyleHint(QFont.Monospace)
        font.setFixedPitch(True)
        font.setPointSize(10)
        self.setFont(font)

        # Setup code highlighter
        self.highlighter = PythonHighlighter(self.document())

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Tab:
            self.insertPlainText("    ")
        else:
            return QTextEdit.keyPressEvent(self, event)

    def contextMenuEvent(self, event):
        menu = self.createStandardContextMenu()
        menu.insertAction(menu.actions()[0], QAction("Run Function", self, triggered=self.rFuncAct))
        # menu.insertAction(menu.actions()[1], QAction("Assign to Toolbar", self, triggered=self.assFuncAct))
        menu.insertSeparator(menu.actions()[1])
        menu.exec_(event.globalPos())

    def rFuncAct(self):
        self.runFunction.emit(self.textCursor().selectedText())

    def assFuncAct(self):
        self.assignFunction.emit(self.textCursor().selectedText())

class MainScriptEditor(QWidget):
    def __init__(self, parent):
        super(MainScriptEditor, self).__init__(parent)

        self.editWindow = CodeEditor()

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.editWindow)

        self.setLayout(mainLayout)
        self.lastLevel = 0

        self.tfile = tempfile.NamedTemporaryFile('w', suffix='.py', prefix='cwautoscript_', delete=False)
        self.tfile.close()
        # print self.tfile.name
        
        self.saveSliderPosition()
        
    def saveSliderPosition(self):
        self._sliderPosition = self.editWindow.verticalScrollBar().value()
        
    def restoreSliderPosition(self):
        self.editWindow.verticalScrollBar().setValue(self._sliderPosition)

    def append(self, statement, level=2):
        if self.lastLevel > level:
            self.editWindow.append("")
        self.lastLevel = level
        self.editWindow.append(" "*(level * 4) + statement)

    def loadModule(self):
        # Save text editor somewhere
        f = open(self.tfile.name, 'w')
        filecontents = self.editWindow.toPlainText()
        f.write(filecontents)
        f.close()

        modulename = str(uuid.uuid1())
        self.scriptModule = imp.load_source(modulename, self.tfile.name)

        # print self.scriptModule
        return self.scriptModule
