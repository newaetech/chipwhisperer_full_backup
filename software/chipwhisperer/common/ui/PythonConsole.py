# Copyright (c) 2009-2010, Cloud Matrix Pty. Ltd.
# All rights reserved; available under the terms of the BSD License.
"""
PySideKick.Console: a simple embeddable python shell
=====================================================


This module provides the call QPythonConsole, a python shell that can be
embedded in your GUI.

"""

import chipwhisperer
import logging
import os, sys, subprocess, shutil
from code import InteractiveConsole as _InteractiveConsole
from PySide import QtCore, QtGui
from chipwhisperer.common.utils.util import requestConsoleBreak, updateUI
from os.path import expanduser
import time
from datetime import timedelta

# Note: we'd like to use cStringIO (it's faster) but we can't subclass it
from StringIO import StringIO

class MyStringIO(StringIO):
    """Custom StringIO class - prints everything to console"""
    def __init__(self, console):
        StringIO.__init__(self)
        self._output = console

    def write(self, s):
        self._output.write(s)
        updateUI()


class QInteractiveLineEdit(QtGui.QLineEdit):
    """Improved QLineEdit for Python console

    Handles tab keys and emits signal for Ctrl-C
    """

    sigCtrlCPressed = QtCore.Signal()

    def __init__(self, *args):
        QtGui.QLineEdit.__init__(self, *args)

    def event(self, event):
        if (event.type() == QtCore.QEvent.KeyPress):
            if event.key() == QtCore.Qt.Key_Tab:
                self.setText(self.text() + "    ")
                return True
            elif event.key() == QtCore.Qt.Key_C and (event.modifiers() & QtCore.Qt.ControlModifier):
                requestConsoleBreak()

        return QtGui.QLineEdit.event(self, event)

class _QPythonConsoleInterpreter(_InteractiveConsole):
    """InteractiveConsole subclass that sends all output to the GUI."""
 
    def __init__(self,ui,locals=None):
        _InteractiveConsole.__init__(self,locals)
        self.ui = ui

    def write(self,data):
        if data:
            # Warning! This function enters C++ land where \0 marks the end of a string!
            # Make sure strings don't have null chars before they end by decoding them
            data_sanitized = data.decode('utf-8')

            self.ui.output.moveCursor(QtGui.QTextCursor.End)
            self.ui.output.insertPlainText(data_sanitized)
            self.ui.output.moveCursor(QtGui.QTextCursor.End)

    def runsource(self,source,filename="<input>",symbol="single"):
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = sys.stderr = collector = MyStringIO(self)
        try:
            more = _InteractiveConsole.runsource(self,source,filename,symbol)
        finally:
            if sys.stdout is collector:
                sys.stdout = old_stdout
            if sys.stderr is collector:
                sys.stderr = old_stderr
        return more


class _QPythonConsoleUI(object):
    """UI layout container for QPythonConsole."""
    def __init__(self,parent):
        if parent.layout() is None:
            parent.setLayout(QtGui.QHBoxLayout())

        # Monospace font
        font = QtGui.QFont("Courier")
        font.setStyleHint(QtGui.QFont.Monospace)

        layout = QtGui.QVBoxLayout()
        layout.setSpacing(0)

        # Output console: a fixed-pitch-font text area
        self.output = QtGui.QPlainTextEdit(parent)
        self.output.setReadOnly(True)
        self.output.setUndoRedoEnabled(False)
        self.output.setMaximumBlockCount(5000)
        self.output.setFont(font)

        layout.addWidget(self.output)
        parent.layout().addLayout(layout)

        # Input console, a prompt displated next to a lineedit
        layout2 = QtGui.QHBoxLayout()
        self.prompt = QtGui.QLabel(parent)
        self.prompt.setText(">>> ")
        layout2.addWidget(self.prompt)
        self.input = QInteractiveLineEdit(parent)
        self.input.setAttribute(QtCore.Qt.WA_MacShowFocusRect, False)
        self.input.setFont(font)
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
        self.interpreter.write(self.ui.prompt.text() + line + '\n')
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
        text = self.ui.input.text()
        self.ui.input.setText("")
        # allow copy and pasting multiple lines
        lines = text.split('\n')
        if len(lines) > 1:
            lines = filter(None, lines)  # removes empty lines
        for line in lines:
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

    def setSelectedPath(self, path):
        self.setCurrentIndex(self.model().index(path))


class CWPythonRecentTable(QtGui.QTableWidget):
    """A table view that stores a list of recently run files

    Also allows files to be pinned
    """

    num_scripts = 10
    regular_color = QtGui.QColor(255, 255, 255)
    pinned_color = QtGui.QColor(160, 220, 240)
    pinned_setting = 'pinned-scripts'
    unpinned_setting = 'unpinned-scripts'

    def __init__(self, parent=None):
        super(CWPythonRecentTable, self).__init__(parent)

        self.pinned = QtCore.QSettings().value(self.pinned_setting)
        self.unpinned = QtCore.QSettings().value(self.unpinned_setting)

        if self.pinned is None or not isinstance(self.pinned, list) \
            or self.unpinned is None or not isinstance(self.unpinned, list):
            self.pinned = []
            self.unpinned = []

        self.setRowCount(self.num_scripts)
        self.setColumnCount(1)
        self.setHorizontalHeaderLabels([
            'Recent Files',
        ])
        self.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.horizontalHeader().setResizeMode(0, QtGui.QHeaderView.Stretch)

        self._listsUpdated()

    def _listsUpdated(self, selected_idx=None):
        """Helper function for whenever the file lists are updated.

        Saves the new lists to persistent settings and redraws the table.

        If <selected_idx> is not None, change the selected row to this one
        """
        QtCore.QSettings().setValue(self.pinned_setting, self.pinned)
        QtCore.QSettings().setValue(self.unpinned_setting, self.unpinned)

        num_pinned = len(self.pinned)
        num_unpinned = len(self.unpinned)
        num_empty = self.num_scripts - num_pinned - num_unpinned
        for row in range(self.num_scripts):
            if row < num_pinned:
                idx = row
                bg_color = self.pinned_color
                path = self.pinned[idx]
            elif row < num_pinned + num_unpinned:
                idx = row - num_pinned
                bg_color = self.regular_color
                path = self.unpinned[idx]
            else:
                bg_color = self.regular_color
                path = ""

            short_path = os.path.basename(path)
            item = QtGui.QTableWidgetItem(short_path)
            item.setFlags(item.flags() & ~QtCore.Qt.ItemIsEditable)
            item.setToolTip(path)
            item.setBackground(bg_color)
            self.setItem(row, 0, item)

        if selected_idx is not None:
            self.selectRow(selected_idx)

    def addScript(self, path):
        """Add a script path to the recent file list.

        3 possible cases:
        1. Script is pinned. Do nothing.
        2. Script is unpinned. Put it at the top of the unpinned list.
        3. Script is not in lists. Put it at the top of the unpinned list and remove an old script if needed.
        Redraw the table and save settings if necessary.
        """
        selected_idx = None
        if path in self.pinned:
            selected_idx = self.pinned.index(path)

        elif path in self.unpinned:
            self.unpinned.remove(path)
            self.unpinned.insert(0, path)
            selected_idx = len(self.pinned)
        else:
            self.unpinned.insert(0, path)
            if len(self.pinned) + len(self.unpinned) > self.num_scripts:
                del self.unpinned[-1]

            # Select this row only if it wasn't removed immediately
            if len(self.unpinned) > 0:
                selected_idx = len(self.pinned)

        self._listsUpdated(selected_idx)

    def _pinScript(self, idx):
        """Take an unpinned script and add it to the list of pinned scripts

        Cases:
        1. Script is unpinned. Move it from unpinned list to pinned.
        2. Script is not unpinned. Do nothing.
        Redraw the table and save settings if necessary.
        """
        unpinned_idx = idx - len(self.pinned)
        if unpinned_idx < 0 or unpinned_idx >= len(self.unpinned):
            selected_idx = idx
        else:
            path = self.unpinned[unpinned_idx]
            del self.unpinned[unpinned_idx]
            self.pinned.append(path)
            selected_idx = len(self.pinned) - 1
        self._listsUpdated(selected_idx)

    def _unpinScript(self, idx):
        """Take a pinned script and move it to the unpinned script list.

        Cases:
        1. Script is not pinned. Do nothing.
        2. Script is pinned. Remove from pinned list and add to top of unpinned list.
        Redraw the table and save settings if necessary.
        """
        if idx < len(self.pinned):
            path = self.pinned[idx]
            del self.pinned[idx]
            self.unpinned.insert(0, path)
            selected_idx = len(self.pinned)
        else:
            selected_idx = idx

        self._listsUpdated(selected_idx)

    def _removeScript(self, idx):
        """Remove a script from the list.

        Find the script in one of the lists and remove it. Don't worry that
        this will leave an empty space at the end of the table.
        """
        if idx < len(self.pinned):
            del self.pinned[idx]
        else:
            unpinned_idx = idx - len(self.pinned)
            if unpinned_idx < len(self.unpinned):
                del self.unpinned[unpinned_idx]

        self._listsUpdated(None)

    def contextMenuEvent(self, event):
        idx = self.rowAt(event.pos().y())
        num_pinned = len(self.pinned)
        num_unpinned = len(self.unpinned)

        if idx < num_pinned:
            item_pinned = True
        elif idx < num_pinned + num_unpinned:
            item_pinned = False
        else:
            return

        menu = QtGui.QMenu(self)
        if item_pinned:
            action_name = "Unpin From Top"
        else:
            action_name = "Pin To Top"

        modify_pin = menu.addAction(action_name)
        remove_item = menu.addAction("Remove From List")
        action = menu.exec_(self.mapToGlobal(event.pos()))

        if action == modify_pin:
            if item_pinned:
                self._unpinScript(idx)
            else:
                self._pinScript(idx)
        elif action == remove_item:
            self._removeScript(idx)

    def getSelectedPath(self):
        idx_list = self.selectedIndexes()
        if len(idx_list) == 0:
            return None
        row = idx_list[0].row()
        if row < len(self.pinned):
            return self.pinned[row]
        elif row < len(self.pinned) + len(self.unpinned):
            return self.unpinned[row - len(self.pinned)]
        return None


class QPythonScriptBrowser(QtGui.QWidget):
    """A script browser with 3 tabs to help find Python files:
    1. ChipWhisperer directory
    2. Root of file system
    3. Recent file list with options to pin files
    """

    # Note that this signal needs to be defined at the class level
    # See https://stackoverflow.com/a/2971426/3817091 for details
    sigSelectionChanged = QtCore.Signal()
    sigSelectionConfirmed = QtCore.Signal()

    def __init__(self, parent=None, default="capture"):
        super(QPythonScriptBrowser,self).__init__(parent)

        self.tab_bar = QtGui.QTabBar()
        self.tab_bar.addTab("ChipWhisperer")
        self.tab_bar.addTab("Project")
        self.tab_bar.addTab("File System")
        self.tab_bar.addTab("Recent")
        self.tab_bar.currentChanged.connect(self.tabChanged)

        cwroot = os.path.dirname(os.path.dirname(chipwhisperer.__file__))
        home_user_dir = expanduser("~")
        user_chipwhisperer = os.path.join(home_user_dir, 'chipwhisperer')
        
        
        if default == "capture":
            scripts_folder = os.path.join(cwroot, 'chipwhisperer', 'capture', 'scripts')
        elif default == "analyzer":
            scripts_folder = os.path.join(cwroot, 'chipwhisperer', 'analyzer', 'scripts')
        else:
            scripts_folder = cwroot

        self.file_view_cw = CWPythonFileTree(scripts_folder)
        self.file_view_project = CWPythonFileTree(user_chipwhisperer)
        self.file_view_all = CWPythonFileTree()
        self.file_view_recent = CWPythonRecentTable()

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.tab_bar)
        layout.addWidget(self.file_view_cw)
        layout.addWidget(self.file_view_project)
        layout.addWidget(self.file_view_all)
        layout.addWidget(self.file_view_recent)

        self.setLayout(layout)

        self.tabChanged(0)

        # New style signals causes crashes on at least some platforms, so need to use old style
        self.connect(self.file_view_cw.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))
        self.connect(self.file_view_project.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))
        self.connect(self.file_view_all.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))
        self.connect(self.file_view_recent.selectionModel(), QtCore.SIGNAL("selectionChanged(QItemSelection , QItemSelection )"),
                 self, QtCore.SLOT("selectionChanged(QItemSelection, QItemSelection)"))

        self.file_view_cw.activated.connect(self.selectionConfirmed)
        self.file_view_all.activated.connect(self.selectionConfirmed)
        self.file_view_recent.activated.connect(self.selectionConfirmed)

    def tabChanged(self, newTab):
        self.file_view_cw.hide()
        self.file_view_project.hide()
        self.file_view_all.hide()
        self.file_view_recent.hide()
        if newTab == 0: # ChipWhisperer
            self.file_view_cw.show()
        elif newTab == 1: # Project
            self.file_view_project.show()
        elif newTab == 2: # File system
            self.file_view_all.show()
        else: # Recent
            self.file_view_recent.show()

        self.selectionChanged()

    def getSelectedPath(self):
        active_tab = self.tab_bar.currentIndex()
        if active_tab == 0: # ChipWhisperer
            return self.file_view_cw.getSelectedPath()
        elif active_tab == 1: # Project
            return self.file_view_project.getSelectedPath()
        elif active_tab == 2: # File system
            return self.file_view_all.getSelectedPath()
        elif active_tab == 3: # Recent
            return self.file_view_recent.getSelectedPath()
        else:
            return None

    def setSelectedPath(self, basename):
        active_tab = self.tab_bar.currentIndex()
        if active_tab == 0: # ChipWhisperer
            return self.file_view_cw.setSelectedPath(basename)
        elif active_tab == 1: # Project
            return self.file_view_project.setSelectedPath(basename)
        elif active_tab == 2: # File system
            return self.file_view_all.setSelectedPath(basename)
        #Recent tab works differently - not sure it should be selected automatically in this case anyway
        #elif active_tab == 2: # Recent
        #    return self.file_view_recent.setSelectedPath(basename)
        else:
            return None

    def selectionChanged(self, x=None, y=None):
        self.sigSelectionChanged.emit()

    def selectionConfirmed(self):
        path = self.getSelectedPath()
        if not os.path.isdir(path):
            self.sigSelectionConfirmed.emit()

    def addRecentFile(self, path):
        self.file_view_recent.addScript(path)

class DialogWithCheckBox(QtGui.QMessageBox):
    """Reimplementation of QMessageBox that adds checkbox"""

    def __init__(self, parent= None):
        super(DialogWithCheckBox, self).__init__()

        self.checkbox = QtGui.QCheckBox("Don't show me this crap")
        #Access the Layout of the MessageBox to add the Checkbox
        layout = self.layout()
        layout.addWidget(self.checkbox, 3,1)

    def exec_(self, *args, **kwargs):
        """
        Override the exec_ method so you can return the value of the checkbox
        """
        return QtGui.QMessageBox.exec_(self, *args, **kwargs), self.checkbox.isChecked()

class QPythonScriptRunner(QtGui.QWidget):
    def __init__(self, console, parent=None):
        super(QPythonScriptRunner,self).__init__(parent)
        self.console = console
        self.api = parent.api

        parenttype = str(parent)
        if 'Capture' in parenttype:
            sccmdtype = "capture"
        elif 'Analyzer' in parenttype:
            sccmdtype = "analyzer"
        else:
            sccmdtype = None
        self.browser = QPythonScriptBrowser(default=sccmdtype)

        self.file_preview = QtGui.QTextEdit()
        self.file_preview.setReadOnly(True)

        self.browser.sigSelectionChanged.connect(self.viewScript)

        self.run_button = QtGui.QPushButton("Run")
        self.run_button.clicked.connect(self.runScript)
        self.browser.sigSelectionConfirmed.connect(self.runScript)

        self.edit_button = QtGui.QPushButton("Edit")
        self.edit_button.clicked.connect(self.editScript)

        self.editcopy_button = QtGui.QPushButton("Edit Copy")
        self.editcopy_button.clicked.connect(self.editCopyScript)

        self.hide_button = QtGui.QPushButton("<")
        self.hide_button.setFixedWidth(20)
        self.hide_button.setSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Expanding)
        self.hide_button.clicked.connect(self.toggleBrowser)

        button_layout = QtGui.QHBoxLayout()
        button_layout.addWidget(self.run_button)
        button_layout.addSpacing(20)
        button_layout.addWidget(self.edit_button)
        button_layout.addSpacing(20)
        button_layout.addWidget(self.editcopy_button)
        button_layout.addStretch()

        preview_layout = QtGui.QVBoxLayout()
        preview_layout.addWidget(self.file_preview)
        preview_layout.addSpacing(10)
        preview_layout.addItem(button_layout)
        preview_box = QtGui.QGroupBox("Script Preview (Read Only)")
        preview_box.setLayout(preview_layout)

        layout = QtGui.QHBoxLayout()
        layout.addWidget(self.browser)
        layout.addWidget(self.hide_button)
        layout.addWidget(preview_box)
        self.setLayout(layout)

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
            logging.info('Script %s is being executed' % path)
            start = time.time()
            self.console.runLine("execfile('%s')" % path)
            end = time.time()
            exec_time = timedelta(seconds=end-start)
            logging.info('Script %s has finished executing' % path)
            logging.info('Execution time: %s' % exec_time)
            self.browser.addRecentFile(path)

    def editCopyScript(self):
        """Make a copy of the script, then edit that script"""
        path = self.browser.getSelectedPath()
        if path is None:
            return
        if os.path.isfile(path):
            # Ask user for new filename
            base = os.path.basename(path)
            result = QtGui.QInputDialog.getText(self, "Copy Script " + base,
                                       "Enter new script name:", text = ("COPY_" + base))
            if result[1]:
                #User hit OK

                #Make new filename, check if exists
                basename = result[0]
                rpath = os.path.dirname(path)
                newfname = os.path.join(rpath, basename)

                #Check if file exists already
                if os.path.exists(newfname):
                    raise Warning("New filename (%s) exists, copy aborted" % basename)

                shutil.copy(path, newfname)

                #Make active, then edit
                self.browser.setSelectedPath(newfname)
                self.openEditor(newfname)


    def editScript(self):
        """Edit the currently selected script"""
        path = self.browser.getSelectedPath()
        if path is None:
            return

        self.openEditor(path)

    def openEditor(self, path):
        """Open selected text editor"""
        if os.path.isfile(path):
            text_editor = self.api.settings.value('text-editor')
            open_with_default = False
            open_editor_error = False

            if len(text_editor) == 0:
                open_with_default = True
            else:  # len > 0\
                if not os.path.isfile(text_editor):
                    logging.warning(
                        "Python Console: Can't open text files with %s" % text_editor)
                    open_editor_error = True
                else:
                    try:
                        subprocess.Popen([text_editor, path])
                    # Catching BaseException here might be a bit dangerous
                    # On Windows, WindowsError works.
                    # Need to test on Linux/Mac
                    except BaseException as e:
                        logging.warning(
                            "Python Console: Failed to open text file with %s" % text_editor)
                        open_editor_error = True

            # this message box is presented to the user because the debug logging screen is not
            # in view when user is using the python console with the default view. This makes
            # the error more clear and the details can be found in the debug logging to
            # differentiate between file not existing and the text editor file failing to open the
            # file for editing
            if open_editor_error:
                errorMsgBox = QtGui.QMessageBox()
                errorMsgBox.setWindowTitle("Requested Editor Not Valid")
                errorMsgBox.setText("The editor requested is not valid")
                errorMsgBox.setInformativeText("The path to your preferred text editor is invalid, please set "
                                               "a valid text editor path using 'Help-->Preferences' and "
                                               "changing the path value for the 'External Text Editor' field.")
                errorMsgBox.setIcon(QtGui.QMessageBox.Warning)
                errorMsgBox.addButton(QtGui.QMessageBox.Ok)
                errorMsgBox.setDefaultButton(QtGui.QMessageBox.Ok)
                user_response = errorMsgBox.exec_()
                if user_response == QtGui.QMessageBox.Ok:
                    return

            if open_with_default:
                self.editWithDefault(path)

    def editWithDefault(self, path):
        """Edit file with OS default, checking with user first time this happens"""

        text_editor_dontcheck = bool(self.api.settings.value('text-editor-dont-check'))
        if text_editor_dontcheck != True:
            # QDialog for user
            msgbox = DialogWithCheckBox()
            msgbox.setWindowTitle("System Editor Not Configured")
            msgbox.setText("Using default system editor for file " + os.path.basename(path))
            msgbox.setInformativeText("If your system defaults are to run these files (and not edit), please "
                                      "goto 'Help-->Preferences' and configure your preferred text "
                                      "editor for script files instead. If you hit 'OK' here I will attempt "
                                      "to open the named script file now.")
            msgbox.setIcon(QtGui.QMessageBox.Question)
            msgbox.addButton(QtGui.QMessageBox.Ok)
            msgbox.addButton(QtGui.QMessageBox.Cancel)
            msgbox.setDefaultButton(QtGui.QMessageBox.Ok)
            user_resp = msgbox.exec_()

            if user_resp[1] == True:
                self.api.settings.setValue('text-editor-dont-check', True)

            if user_resp[0] == QtGui.QMessageBox.Cancel:
                return

        if sys.platform == "win32":
            os.startfile(path)
        else:
            opener = "open" if sys.platform == "darwin" else "xdg-open"
            subprocess.call([opener, path])

    def viewScript(self):
        """Edit the currently selected script"""
        path = self.browser.getSelectedPath()
        if path is None or not os.path.isfile(path):
            self.file_preview.setText("")
        else:
            with open(path, 'r') as script_file:
                file_contents = script_file.read()
            self.file_preview.setText(file_contents)

    def toggleBrowser(self):
        hidden = self.browser.isHidden()
        if hidden:
            self.browser.show()
            button_text = "<"
        else:
            self.browser.hide()
            button_text = ">"
        self.hide_button.setText(button_text)

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