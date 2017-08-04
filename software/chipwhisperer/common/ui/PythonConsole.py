# Copyright (c) 2009-2010, Cloud Matrix Pty. Ltd.
# All rights reserved; available under the terms of the BSD License.
"""
PySideKick.Console: a simple embeddable python shell
=====================================================


This module provides the call QPythonConsole, a python shell that can be
embedded in your GUI.

"""

import logging
import os
import subprocess
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

    def runLine(self, line):
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

    def _on_enter_line(self):
        line = self.ui.input.text()
        self.ui.input.setText("")
        self.runLine(line)
        
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

class CWPythonFileTree(QtGui.QTreeView):
    """Customized QTreeView that only displays Python files.

    Optional parameter in __init__: root sets up base directory for file tree
    """

    def __init__(self, root=None, parent=None):
        super(CWPythonFileTree, self).__init__(parent)
        model = QtGui.QFileSystemModel()
        if root is not None:
            model.setRootPath(root)
        else:
            # TODO: unsure if this works on all operating systems.
            # Amazingly, it works on Windows.
            model.setRootPath('/')

        # Only display Python files
        model.setNameFilters(["*.py"])
        model.setNameFilterDisables(False)

        self.setModel(model)
        if root is not None:
            self.setRootIndex(model.index(root))
        else:
            # It seems like this is unnecessary. Not sure why...
            # self.setRootIndex(model.index('/'))
            pass

        self.hideColumn(1)
        self.hideColumn(2)
        self.hideColumn(3)

    def getSelectedPath(self):
        file_index = self.currentIndex()
        if not file_index.isValid():
            return None
        return self.model().filePath(file_index)

class QPythonScriptBrowser(QtGui.QWidget):
    """A script browser with 3 tabs to help find Python files:
    1. ChipWhisperer directory
    2. Root of file system
    3. Recent file list with options to pin files
    """

    # Note that this signal needs to be defined at the class level
    # See https://stackoverflow.com/a/2971426/3817091 for details
    sigSelectionChanged = QtCore.Signal()

    def __init__(self, parent=None):
        super(QPythonScriptBrowser,self).__init__(parent)

        self.tab_bar = QtGui.QTabBar()
        self.tab_bar.addTab("ChipWhisperer")
        self.tab_bar.addTab("File System")
        self.tab_bar.addTab("Recent")
        self.tab_bar.currentChanged.connect(self.tabChanged)

        # TODO: don't hard-code this path
        scripts_folder = r'C:/chipwhisperer/software/chipwhisperer'
        self.file_view_cw = CWPythonFileTree(scripts_folder)
        self.file_view_all = CWPythonFileTree()
        # TODO: implement this table
        self.file_view_recent = QtGui.QTableView()

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.tab_bar)
        layout.addWidget(self.file_view_cw)
        layout.addWidget(self.file_view_all)
        layout.addWidget(self.file_view_recent)

        self.setLayout(layout)

        self.tabChanged(0)

        # New style signals causes crashes on at least some platforms, so need to use old style
        self.connect(self.file_view_cw.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))
        self.connect(self.file_view_all.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))

    def tabChanged(self, newTab):
        self.file_view_cw.hide()
        self.file_view_all.hide()
        self.file_view_recent.hide()
        if newTab == 0: # ChipWhisperer
            self.file_view_cw.show()
        elif newTab == 1: # File system
            self.file_view_all.show()
        else: # Recent
            self.file_view_recent.show()

        self.selectionChanged()

    def getSelectedPath(self):
        active_tab = self.tab_bar.currentIndex()
        if active_tab == 0: # ChipWhisperer
            return self.file_view_cw.getSelectedPath()
        elif active_tab == 1: # File system
            return self.file_view_all.getSelectedPath()
        elif active_tab == 2: # Recent
            return None
            # TODO
            #return self.file_view_recent.getSelectedPath()
        else:
            return None

    def selectionChanged(self, x=None, y=None):
        self.sigSelectionChanged.emit()

class QPythonScriptRunner(QtGui.QWidget):
    def __init__(self, console, parent=None):
        super(QPythonScriptRunner,self).__init__(parent)
        self.console = console
        self.api = parent.api

        self.browser = QPythonScriptBrowser()

        self.file_preview = QtGui.QTextEdit()
        self.file_preview.setReadOnly(True)

        self.browser.sigSelectionChanged.connect(self.viewScript)

        self.run_button = QtGui.QPushButton("Run")
        self.run_button.clicked.connect(self.runScript)

        self.edit_button = QtGui.QPushButton("Edit")
        self.edit_button.clicked.connect(self.editScript)

        # TODO
        self.hide_button = QtGui.QPushButton(">")

        #self.view_button = QtGui.QPushButton("View")
        #self.view_button.clicked.connect(self.viewScript)

        grid_layout = QtGui.QGridLayout(self)
        grid_layout.addWidget(self.browser, 0, 0, 1, 3)
        grid_layout.addWidget(self.file_preview, 0, 3, 1, 2)
        grid_layout.addWidget(self.run_button, 1, 0)
        grid_layout.addWidget(self.edit_button, 1, 1)
        #grid_layout.addWidget(self.view_button, 1, 2)
        self.setLayout(grid_layout)

    def runScript(self):
        """Run the currently selected script"""
        path = self.browser.getSelectedPath()
        if path is None or not os.path.isfile(path):
            error_dialog = QtGui.QMessageBox()
            error_dialog.warning(
                self,
                "Python Console",
                "Error in Python Console: Selected path %s is not a file" % path,
                QtGui.QMessageBox.Ok,
                QtGui.QMessageBox.NoButton
            )
        else:
            self.console.runLine("execfile('%s')" % path)

    def editScript(self):
        """Edit the currently selected script"""
        path = self.browser.getSelectedPath()
        if path is None:
            return
        if os.path.isfile(path):
            text_editor = self.api.settings.value('text-editor')
            open_with_default = False

            if len(text_editor) == 0:
                open_with_default = True
            else: # len > 0\
                if not os.path.isfile(text_editor):
                    logging.warning("Python Console: Can't open text files with %s - using system default instead" % text_editor)
                    open_with_default = True
                else:
                    try:
                        subprocess.Popen([text_editor, path])
                    # TODO: I'm not sure which exceptions to catch here to detect a failed Popen?
                    # On Windows, WindowsError works.
                    # Need to test on Linux/Mac
                    except BaseException as e:
                        logging.warning("Python Console: Failed to open text file with %s - using system default instead" % text_editor)
                        open_with_default = True

            if open_with_default:
                os.startfile(path)

    def viewScript(self):
        """Edit the currently selected script"""
        path = self.browser.getSelectedPath()
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

        self.script_runner = QPythonScriptRunner(self.console, parent)
        self.addWidget(self.script_runner)

#        self.setLayout(QtGui.QHBoxLayout(self))

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    win = QtGui.QMainWindow()
    win.setCentralWidget(QPythonConsole())
    win.show()
    app.exec_()