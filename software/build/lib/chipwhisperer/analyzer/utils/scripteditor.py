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
import os.path, time

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
    def __init__(self, parent, filename=None):
        super(MainScriptEditor, self).__init__(parent)

        self.editWindow = CodeEditor()
        self.editWindow.textChanged.connect(self.textChanged)

        mainLayout = QHBoxLayout()
        mainLayout.addWidget(self.editWindow)

        self.setLayout(mainLayout)
        self.lastLevel = 0

        # If no file, we generate a temporary file
        if filename is None:
            self.tfile = tempfile.NamedTemporaryFile('w', suffix='.py', prefix='cwautoscript_', delete=False)
            self.tfile.close()
            self.filename = self.tfile.name
        else:
            self.filename = filename
            self.reloadFile()

        self.fileLastTime = os.path.getmtime(self.filename)
        self.saveSliderPosition()

    def textChanged(self):
        self.dirty = True

    def reloadFile(self):
        # todo: check if changed locally?
        self.saveSliderPosition()
        self.editWindow.clear()

        with open(self.filename) as f:
            content = f.readlines()
            for line in content:
                self.editWindow.append(line.rstrip())

        self.restoreSliderPosition()

        # Save time file was last modified
        self.markClean()

    def saveSliderPosition(self):
        self._sliderPosition = self.editWindow.verticalScrollBar().value()

    def restoreSliderPosition(self):
        self.editWindow.verticalScrollBar().setValue(self._sliderPosition)

    def append(self, statement, level=2):
        if self.lastLevel > level:
            self.editWindow.append("")
        self.lastLevel = level
        self.editWindow.append(" "*(level * 4) + statement)

    def markClean(self):
        self.dirty = False
        self.fileLastTime = os.path.getmtime(self.filename)

    def loadModule(self):

        # Check if file was modified outside of us
        if self.fileLastTime != os.path.getmtime(self.filename):

            # If dirty we've got a problem
            if self.dirty:
                msgBox = QMessageBox(self)
                msgBox.setWindowTitle("Warning: Conflicting Edits")
                msgBox.setText("File modified outside of ChipWhisperer Editor")
                msgBox.setInformativeText("Filename: %s" % self.filename)
                msgBox.addButton('Reload File from Disk, discard local changes', QMessageBox.YesRole)
                msgBox.addButton('Overwrite file on disk with local changes', QMessageBox.NoRole)
                msgBox.addButton('Cancel operation (do not discard any changes)', QMessageBox.RejectRole)

                msgBox.exec_()
                ret = msgBox.buttonRole(msgBox.clickedButton())

                if ret == QMessageBox.YesRole:
                    self.reloadFile()
                elif ret == QMessageBox.NoRole:
                    pass
                else:
                    return None
            else:
                self.reloadFile()

        # Save text editor somewhere
        f = open(self.filename, 'w')
        filecontents = self.editWindow.toPlainText()
        f.write(filecontents)
        f.close()

        self.markClean()

        modulename = str(uuid.uuid1())
        try:
            return imp.load_source(modulename, self.filename)
        except SyntaxError as e:
            raise Warning("Could not execute the python code (%s) in the Script Editor:\n%s in line %s" % (self.filename, e.msg, e.lineno))
