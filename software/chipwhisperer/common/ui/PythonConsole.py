# Copyright (c) 2009-2010, Cloud Matrix Pty. Ltd.
# All rights reserved; available under the terms of the BSD License.
"""
PySideKick.Console: a simple embeddable python shell
=====================================================


This module provides the call QPythonConsole, a python shell that can be
embedded in your GUI.

"""

import os
import sys
from code import InteractiveConsole as _InteractiveConsole
from PySide import QtCore, QtGui

try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO


class _QPythonConsoleInterpreter(_InteractiveConsole):
    """InteractiveConsole subclass that sends all output to the GUI."""
 
    def __init__(self,ui,locals=None):
        _InteractiveConsole.__init__(self,locals)
        self.ui = ui

    def write(self,data):
        if data:
            if data[-1] == "\n":
                data = data[:-1]
            self.ui.output.appendPlainText(data)

    def runsource(self,source,filename="<input>",symbol="single"):
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = sys.stderr = collector = StringIO()
        try:
            more = _InteractiveConsole.runsource(self,source,filename,symbol)
        finally:
            if sys.stdout is collector:
                sys.stdout = old_stdout
            if sys.stderr is collector:
                sys.stderr = old_stderr
        self.write(collector.getvalue())
        return more


class _QPythonConsoleUI(object):
    """UI layout container for QPythonConsole."""
    def __init__(self,parent):
        if parent.layout() is None:
            parent.setLayout(QtGui.QHBoxLayout())
        layout = QtGui.QVBoxLayout()
        layout.setSpacing(0)
        # Output console: a fixed-pitch-font text area.
        self.output = QtGui.QPlainTextEdit(parent)
        self.output.setReadOnly(True)
        self.output.setUndoRedoEnabled(False)
        self.output.setMaximumBlockCount(5000)
        fmt = QtGui.QTextCharFormat()
        fmt.setFontFixedPitch(True)
        self.output.setCurrentCharFormat(fmt)
        layout.addWidget(self.output)
        parent.layout().addLayout(layout)
        # Input console, a prompt displated next to a lineedit
        layout2 = QtGui.QHBoxLayout()
        self.prompt = QtGui.QLabel(parent)
        self.prompt.setText(">>> ")
        layout2.addWidget(self.prompt)
        self.input = QtGui.QLineEdit(parent)
        self.input.setAttribute(QtCore.Qt.WA_MacShowFocusRect, False)
        layout2.addWidget(self.input)
        layout.addLayout(layout2)


class QPythonConsole(QtGui.QWidget):
    """A simple python console to embed in your GUI.

    This widget provides a simple interactive python console that you can
    embed in your GUI (e.g. for debugging purposes). Use it like so:

    self.debug_window.layout().addWidget(QPythonConsole())

    You can customize the variables that are available in the shell by
    passing a dict as the "locals" argument.
    """

    def __init__(self, parent=None, locals=None):
        super(QPythonConsole,self).__init__(parent)
        self.ui = _QPythonConsoleUI(self)
        self.interpreter = _QPythonConsoleInterpreter(self.ui,locals)
        self.ui.input.returnPressed.connect(self._on_enter_line)
        self.ui.input.installEventFilter(self)
        self.history = []
        self.history_pos = 0

    def _on_enter_line(self):
        line = self.ui.input.text()
        self.ui.input.setText("")
        self.interpreter.write(self.ui.prompt.text() + line)
        more = self.interpreter.push(line)
        if line:
            self.history.append(line)
            self.history_pos = len(self.history)
            while len(self.history) > 100:
                self.history = self.history[1:]
                self.history_pos -= 1
        if more:
            self.ui.prompt.setText("... ")
        else:
            self.ui.prompt.setText(">>> ")
        
    def eventFilter(self, obj, event):
        if hasattr(event, "type") and event.type() == QtCore.QEvent.KeyPress:
            if event.key() == QtCore.Qt.Key_Up:
                self.go_history(-1)
                return True
            elif event.key() == QtCore.Qt.Key_Down:
                self.go_history(1)
                return True
        return False

    def go_history(self,offset):
        if offset < 0:
            self.history_pos = max(0,self.history_pos + offset)
        elif offset > 0:
            self.history_pos = min(len(self.history),self.history_pos + offset)
        try:
            line = self.history[self.history_pos]
        except IndexError:
            line = ""
        self.ui.input.setText(line)
        self.ui.input.end(False)

class QPythonScriptRunner(QtGui.QWidget):
    def __init__(self, parent=None):
        super(QPythonScriptRunner,self).__init__(parent)

        self.file_view = QtGui.QTreeView()

        # Only show users the ChipWhisperer directory
        # TODO: don't hard-code this path
        # TODO: make it clear where the root path is
        scripts_folder = r'C:/chipwhisperer/software/chipwhisperer'
        model = QtGui.QFileSystemModel()
        model.setRootPath(scripts_folder)

        # Only display Python files
        model.setNameFilters(["*.py"])
        model.setNameFilterDisables(False)

        self.file_view.setModel(model)
        self.file_view.setRootIndex(model.index(scripts_folder))

        # Hide size/type/date columns
        self.file_view.hideColumn(1)
        self.file_view.hideColumn(2)
        self.file_view.hideColumn(3)

        self.file_preview = QtGui.QTextEdit()
        self.file_preview.setReadOnly(True)
        # Warning: this signal connection crashes
        #self.file_view.selectionModel().selectionChanged.connect(self.viewScript)

        self.run_button = QtGui.QPushButton("Run")
        self.run_button.clicked.connect(self.runScript)

        self.edit_button = QtGui.QPushButton("Edit")
        self.edit_button.clicked.connect(self.editScript)

        #self.view_button = QtGui.QPushButton("View")
        #self.view_button.clicked.connect(self.viewScript)

        grid_layout = QtGui.QGridLayout(self)
        grid_layout.addWidget(self.file_view, 0, 0, 1, 3)
        grid_layout.addWidget(self.file_preview, 0, 3, 1, 2)
        grid_layout.addWidget(self.run_button, 1, 0)
        grid_layout.addWidget(self.edit_button, 1, 1)
        #grid_layout.addWidget(self.view_button, 1, 2)
        self.setLayout(grid_layout)

    def getSelectedPath(self):
        file_index = self.file_view.currentIndex()
        if not file_index.isValid():
            return None
        return self.file_view.model().filePath(file_index)

    def runScript(self):
        """Run the currently selected script"""
        print "TODO: Run script"
        print "Selected file: %s" % self.getSelectedPath()

    def editScript(self):
        """Edit the currently selected script"""
        path = self.getSelectedPath()
        if path is None:
            return
        if os.path.isfile(path):
            os.startfile(path)

    def viewScript(self, x, y=None):
        """Edit the currently selected script"""
        path = self.getSelectedPath()
        if path is None or not os.path.isfile(path):
            self.file_preview.setText("")
        else:
            with open(path, 'r') as script_file:
                file_contents = script_file.read()
            self.file_preview.setText(file_contents)

class QSplitConsole(QtGui.QSplitter):
    def __init__(self, parent=None, locals=None):
        super(QSplitConsole,self).__init__(parent)
        self.console = QPythonConsole(parent, locals)
        self.addWidget(self.console)

        self.script_runner = QPythonScriptRunner(parent)
        self.addWidget(self.script_runner)

#        self.setLayout(QtGui.QHBoxLayout(self))

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    win = QtGui.QMainWindow()
    win.setCentralWidget(QPythonConsole())
    win.show()
    app.exec_()