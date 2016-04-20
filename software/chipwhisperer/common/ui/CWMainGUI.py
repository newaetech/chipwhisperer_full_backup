#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
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
import chipwhisperer.common.utils.pluginmanager

__author__ = "Colin O'Flynn"

import os
import sys, traceback
from functools import partial
from datetime import datetime
from pyqtgraph.parametertree import Parameter, ParameterTree
import chipwhisperer.common.ui.PythonConsole
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.ui.HelpWindow import HelpBrowser
from chipwhisperer.common.ui.TraceManagerDialog import TraceManagerDialog
from chipwhisperer.common.ui.ProjectTextEditor import ProjectTextEditor
from chipwhisperer.common.utils import pluginmanager, Util
import chipwhisperer.common.ui.qrc_resources

#We always import PySide first, to force usage of PySide over PyQt
try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

try:
    import pyqtgraph as pg
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()


class ProjectDiffWidget(QWidget):
    """Widget that displays differences between versions of the project file"""

    def __init__(self, parent=None, project=None):
        super(ProjectDiffWidget, self).__init__(parent)
        self._project = project

        hlayout = QHBoxLayout()

        self.changedTree = ParameterTree()
        self.addedTree = ParameterTree()
        self.deletedTree = ParameterTree()

        self.updateParamTree(self.changedTree, [], "Changed Sections")
        self.updateParamTree(self.addedTree, [], "Added Sections")
        self.updateParamTree(self.deletedTree, [], "Removed Sections")

        hlayout.addWidget(self.changedTree)
        hlayout.addWidget(self.addedTree)
        hlayout.addWidget(self.deletedTree)

        self.setLayout(hlayout)
        self.checkDiff()

    def updateParamTree(self, paramTree, changelist, name):
        paramlist = []
        for k in changelist:
            paramlist.append({'name':k})
        params = Parameter.create(name=name, type='group', children=paramlist)
        ExtendedParameter.reloadParams([params], paramTree)

    def checkDiff(self, ignored=None, updateGUI=False):
        """
        Check if there is a difference - returns True if so, and False
        if no changes present. Also updates widget with overview of the
        differences if requested with updateGUI
        """
        added, removed, changed = self._project.checkDiff()
        if updateGUI:
            self.updateParamTree(self.changedTree, changed, "Changed Sections")
            self.updateParamTree(self.addedTree, added, "Added Sections (not on disk)")
            self.updateParamTree(self.deletedTree, removed, "Removed Sections (on disk)")


class SaveProjectDialog(QDialog):

    def __init__(self, parent):
        super(SaveProjectDialog, self).__init__(parent)
        self.setWindowTitle("Unsaved Changes Detected")
        self.setModal(True)
        layout = QVBoxLayout()
        layout.addWidget(QLabel("Save unsaved changes?"))
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Yes | QDialogButtonBox.No | QDialogButtonBox.Cancel)
        layout.addWidget(self.buttonBox)

        detailedWidget = ProjectDiffWidget(self, project=self.parent().api.project())
        detailedWidget.checkDiff(updateGUI=True)
        detailedLayout = QVBoxLayout()
        detailedLayout.addWidget(detailedWidget)

        detailedHidableWidget = QWidget()
        detailedHidableWidget.setLayout(detailedLayout)
        detailedHidableWidget.hide()

        pbShowDetails = QPushButton("Show Details")
        pbShowDetails.clicked.connect(detailedHidableWidget.show)
        detailpblayout = QHBoxLayout()
        detailpblayout.addWidget(pbShowDetails)
        detailpblayout.addStretch()
        layout.addLayout(detailpblayout)
        layout.addWidget(detailedHidableWidget)
        
        self.setLayout(layout)

        self._lastpushed = QDialogButtonBox.RejectRole
        self.buttonBox.clicked.connect(self.setValue)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

    def setValue(self, but):
        self._lastpushed = self.buttonBox.buttonRole(but)

    def value(self):
        return self._lastpushed

    @staticmethod
    def getSaveProjectDialog(parent, project):
        if not project.hasDiffs():
            return QDialogButtonBox.NoRole
        dialog = SaveProjectDialog(parent)
        dialog.exec_()
        return dialog.value()


# class ModuleListDialog(QDialog):
#     def __init__(self, lmFunc):
#         super(ModuleListDialog, self).__init__()
#         self.setWindowTitle("Enabled Modules")
#
#         modules = lmFunc()
#
#         table = QTableWidget(len(modules), 3, self)
#         table.setHorizontalHeaderLabels(["Module", "Enabled", "Details"])
#
#         for indx,itm in enumerate(modules):
#             table.setItem(indx, 0, QTableWidgetItem(itm[0]))
#             table.setItem(indx, 1, QTableWidgetItem(str(itm[1])))
#             table.setItem(indx, 2, QTableWidgetItem(itm[2]))
#
#         layout = QVBoxLayout()
#         layout.addWidget(table)
#         self.setLayout(layout)

class OutLog:
    def __init__(self, edit, out=None, color=None, origStdout=None):
        """(edit, out=None, color=None) -> can write stdout, stderr to a
        QTextEdit.
        edit = QTextEdit
        out = alternate stream ( can be the original sys.stdout )
        color = alternate color (i.e. color stderr a different color)
        """
        self.edit = edit
        self.out = None
        self.color = color
        self.origStdout = origStdout

    def write(self, m):
        # Still redirect to original STDOUT

        tc = self.edit.textColor()
        if self.color:
            self.edit.setTextColor(self.color)

        self.edit.moveCursor(QTextCursor.End)
        self.edit.insertPlainText(m)

        if self.color:
            self.edit.setTextColor(tc)

        if self.out:
            self.out.write(m)

        if self.origStdout:
            self.origStdout.write(m)

class CWMainGUI(QMainWindow):
    """
    This is the base GUI class, used for both the Analyzer and Capture software. It defines a number of
    useful features such as the ability to add docks, setting windows, consoles for logging errors, etc. 
    You can run a demo which shows the basic features, which would look like this:
    
    .. image:: /images/mainchip-demo.png
       
    """
    MaxRecentFiles = 4

    def __init__(self, api, name="Demo", icon="cwicon"):
        super(CWMainGUI, self).__init__()
        self.api = api
        Util.setUIupdateFunction(QCoreApplication.processEvents)
        self.api.setHelpWidget(HelpBrowser(self).showHelp)
        self.name = name
        sys.excepthook = self.exceptionHandlerDialog
        self.traceManagerDialog = TraceManagerDialog(self)
        self.projEditWidget = ProjectTextEditor(self)
        self.lastMenuActionSection = None
        self.originalStdout = None
        self.initUI(icon)
        self.setCentralWidget(None)
        self.setDockNestingEnabled(True)
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.api.signals.newProject.connect(self.projectChanged)
        self.api.signals.guiActionsUpdated.connect(self.reloadGuiActions)
        self.api.newProject()
        CWMainGUI.instance = self

    def projectChanged(self):
        self.traceManagerDialog.setTraceManager(self.api.project().traceManager())
        self.projEditWidget.setProject(self.api.project())
        self.api.project().signals.statusChanged.connect(self.projectStatusChanged)
        self.projectStatusChanged()

    def addDock(self, dockWidget, name="Settings", area=Qt.TopDockWidgetArea,
                allowedAreas=Qt.AllDockWidgetAreas,
                visible=True, addToWindows=True):
        """Add a dockwidget to the main window, which also adds it to the 'Windows' menu"""
        #Configure dock
        dock = QDockWidget(name)
        dock.setWidget(dockWidget)
        dock.setAllowedAreas(allowedAreas)
        dock.setObjectName(name)
        self.addDockWidget(area, dock)
        if(hasattr(dockWidget,"visibilityChanged")):
            dockWidget.visibilityChanged.connect(dock.setVisible)
            dock.visibilityChanged.connect(lambda: dockWidget.updateVisibility(dock.isVisible()))

        if visible == False:
            dock.toggleViewAction()
        
        #Add to "Windows" menu
        if addToWindows:
            self.windowMenu.addAction(dock.toggleViewAction())

        return dock
    
    def addSettings(self, tree, name):
        """Adds a dockwidget designed to store a ParameterTree, also adds to 'Windows' menu"""
        dock = self.addDock(tree, name=name, area=Qt.TopDockWidgetArea)
        dock.setMaximumWidth(560)
        return dock

    def tabifyDocks(self, docks):
        for index in range(1, len(docks)):
            docks[index].show()
            self.tabifyDockWidget(docks[index-1], docks[index])
        docks[0].raise_()

    def addTraceDock(self, name):
        """Add a new GraphWidget in a dock, you can get the GW with .widget()"""
        self.waveformDock = self.addDock(GraphWidget(), name=name, area=Qt.TopDockWidgetArea)
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault()
        return self.waveformDock
        
    def addConsole(self, name="Debug Logging", visible=True, redirectStdOut=True):
        """Add a QTextBrowser, used as a console/debug window"""
        console = QTextBrowser()
        if redirectStdOut:
            if self.originalStdout is None:
                self.originalStdout = sys.stdout
            sys.stdout = OutLog(console, sys.stdout, origStdout=self.originalStdout)
            sys.stderr = OutLog(console, sys.stderr, QColor(255, 0, 0), origStdout=self.originalStdout)

        return self.addDock(console, name, area=Qt.BottomDockWidgetArea, visible=visible)
    
    def addPythonConsole(self, name="Python Console", visible=False):
        """Add a python console, inside which you can access the Python interpreter"""
        wid = chipwhisperer.common.ui.PythonConsole.QPythonConsole(self, locals())
        return self.addDock(wid, name, area=Qt.BottomDockWidgetArea, visible=visible)

    def reloadGuiActions(self):
        # Remove all old actions that don't apply for new selection
        if hasattr(self,"_ToolMenuItems"):
            for act in self._ToolMenuItems:
                self.toolMenu.removeAction(act)

        self._ToolMenuItems = []
        self._ToolMenuItems.append(self.toolMenu.addSeparator())
        for act in self.api.guiActions(self):
            self._ToolMenuItems.append(QAction(act[0], self, statusTip=act[1], triggered=act[2]))

        for act in self._ToolMenuItems:
            self.toolMenu.addAction(act)

    def clearAllSettings(self):
        """Clear all saved QSettings(), such as window location etc"""
        QSettings().clear()

    def reset(self):
        self.clearAllSettings()
        sys.exit()

    def restoreSettings(self):
        self.restoreGeometry(QSettings().value("geometry"))
        self.restoreState(QSettings().value("windowState"))

    def closeEvent(self, event):
        """Called when window is closed, attempts to save state/geometry"""
        QSettings().setValue("geometry", self.saveGeometry())
        QSettings().setValue("windowState", self.saveState())

        if self.okToContinue():
            QMainWindow.closeEvent(self, event)
        else:
            event.ignore()

    def createFileActions(self):
        """Add the file actions (open/save/new)"""
        self.openRec = QMenu('Open &Recent')

        self.openAct = QAction(QIcon('open.png'), '&Open...', self,
                               shortcut=QKeySequence.Open,
                               statusTip='Open Project File',
                               triggered=self.openProject)

        self.saveAct = QAction(QIcon('save.png'), '&Save...', self,
                               shortcut=QKeySequence.Save,
                               statusTip='Save current project to Disk',
                               triggered=self.saveProject)

        self.newAct = QAction(QIcon('new.png'), '&New', self,
                               shortcut=QKeySequence.New,
                               statusTip='Create new Project',
                               triggered=self.newProject)

        self.exitAct = QAction("E&xit", self, shortcut="Ctrl+Q",
                statusTip="Exit the application",
                triggered=self.close)

        for i in range(CWMainGUI.MaxRecentFiles):
            self.recentFileActs.append(QAction(self, visible=False, triggered=self.openRecentFile))

    def helpdialog(self):
        """Helps the User"""
        QMessageBox.about(self, 'Link to Documentation', 'See <a href="http://www.newae.com/sidechannel/cwdocs">newae.com/sidechannel/cwdocs</a> for Tutorials and '
                                                            'Documentation. If you are using an official release, this documentation should have also been present with '
                                                            'your release.<br><br>'
                                                            'See <a href="http://www.chipwhisperer.com">chipwhisperer.com</a> for Wiki, GIT Code, community information.'
                                                            '<br><br>See the About dialog for Copyright, Trademark, and Authorship information'
                                                            )

    def aboutdialog(self):
        """Tells the User"""
        QMessageBox.about(self, 'About', '<h3>ChipWhisperer' + u'\u2122' + '</h3>'
                                         '<h4>Copyright Information</h4>'
                                         'Copyright ' + u'\u00A9' + ' NewAE Technology Inc., 2013-2016. <br>'
                                         'Copyright ' + u'\u00A9' + ' Colin O\'Flynn, 2012-2016.'
                                         '<h4>License Information</h4>'
                                         'Released under the GPLv3 License, see <a href="http://www.gnu.org/copyleft/gpl.html">License Details</a>.<br>'
                                         'Various parts of this project may be released under additional open-source licenses such as the BSD License '
                                         'or LGPL license. See source code for details of specific licenses.'
                                         '<h4>Projects used in ChipWhisperer</h4>'
                                         'ChipWhisperer is built on or otherwise uses a number of open-source projects. Many thanks are given to the following '
                                         'projects, which may also have additional license restrictions or information: '
                                         '<ul>'
                                         '<li><a href="https://www.python.org/">Python</a></li>'
                                         '<li><a href="http://qt-project.org/">Qt</a></li>'
                                         '<li><a href="http://qt-project.org/wiki/pyside">PySide</a></li>'
                                         '<li><a href="http://www.pyqtgraph.org/">PyQtGraph</a></li>'
                                         '<li><a href="http://www.numpy.org/">NumPy</a></li>'
                                         '<li><a href="http://www.scipy.org/">SciPy</a></li>'
                                         '<li><a href="http://sourceforge.net/apps/trac/pyusb/">PyUSB</a></li>'
                                         '<li><a href="http://www.fourwalledcubicle.com/LUFA.php">LUFA USB Library</a></li>'
                                         '<li><a href="http://www.ztex.de/downloads/#firmware_kit">ZTEX EZ-USB SDK</a></li>'
                                         '<li><a href="http://www.libusb.org/wiki/libusb-1.0">libusb-1.0</a></li>'
                                         '<li><a href="http://winavr.sourceforge.net/">WinAVR</a></li>'
                                         '</ul>'
                                         'Some projects may be missing from the above list - please let us know, any omission is a mistake!'
                                         '<h4>Trademark Information</h4>'
                                         'ChipWhisperer is a Trademark of NewAE Technology Inc.'
                                         ''
                                         )

    def createMenus(self):
        """Create all menus (File, Window, etc)"""
        self.fileMenu= self.menuBar().addMenu("&File")
        self.fileMenu.addAction(self.newAct)
        self.fileMenu.addAction(self.openAct)
        self.fileMenu.addMenu(self.openRec)
        self.fileMenu.addAction(self.saveAct)
#        self.fileMenu.addAction(self.importAct)
        for i in range(CWMainGUI.MaxRecentFiles):
            self.openRec.addAction(self.recentFileActs[i])
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.exitAct)
        
        self.projectMenu = self.menuBar().addMenu("&Project")
        self.traceManageAct = QAction('&Manage Traces', self, statusTip='Add/Remove Traces from Project', triggered=self.traceManagerDialog.show)
        self.projectMenu.addAction(self.traceManageAct)
        self.consolidateAct = QAction('&Consolidate', self, statusTip='Copy trace files to project directory', triggered=self.consolidateDialog)
        self.projectMenu.addAction(self.consolidateAct)
        self.showProjFileAct = QAction('&Project File Editor (Text)', self, statusTip='Edit Project File', triggered=self.projEditDock.show)
        self.projectMenu.addAction(self.showProjFileAct)
            
        self.toolMenu = self.menuBar().addMenu("&Tools")

        self.windowMenu = self.menuBar().addMenu("&Windows")        
                
        self.helpMenu = self.menuBar().addMenu("&Help")
        # self.helpListAct = QAction('&List Enabled/Disable Modules', self, statusTip="Check if you're missing modules", triggered=self.listModulesShow)
        self.helpMenu.addAction(QAction('&Clear All Settings', self, statusTip='Restore All Settings to Default Values', triggered=self.clearAllSettings))
        self.helpMenu.addAction(QAction('&Tutorial/User Manual', self, statusTip='Everything you need to know', triggered=self.helpdialog))
        self.helpMenu.addAction(QAction('&About', self, statusTip='About Dialog', triggered=self.aboutdialog))

    def initUI(self, icon="cwicon"):
        """Setup the UI, creating statusbar, setting title, menus, etc"""
        self.statusBar()
        self.setWindowIcon(QIcon(":/images/%s.png" % icon))
        self.recentFileActs = []
        self.createFileActions()
        self.projEditDock = self.addDock(self.projEditWidget, name="Project Text Editor", area=Qt.BottomDockWidgetArea, visible=False, addToWindows=False)
        self.createMenus()
        self.updateRecentFileActions()
        self.addExampleScripts(pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scripts", False, False, self))

        # Project editor dock
        self.paramScriptingDock = self.addConsole("Script Commands", visible=False)
        ExtendedParameter.paramScriptingOutput = self.paramScriptingDock.widget()
        self.consoleDock = self.addConsole()
        self.pythonConsoleDock = self.addPythonConsole()
        self.tabifyDocks([self.projEditDock, self.paramScriptingDock, self.pythonConsoleDock, self.consoleDock])
        self.setBaseSize(800,600)
        
    def addExampleScripts(self, scripts):
        self.exampleScriptAct = QAction('&Example Scripts', self, statusTip='Predefined Scripts')
        self.projectMenu.addSeparator()
        self.projectMenu.addAction(self.exampleScriptAct)
        subMenu = QMenu("Submenu", self)

        for name, script in scripts.iteritems():
            subMenu.addAction(QAction(name, self, statusTip=script.description, triggered=partial(self.runScript, script)))

        self.exampleScriptAct.setMenu(subMenu)

    def updateTitleBar(self):
        """Update filename shown in title bar"""
        fname = os.path.basename(self.api.project().getFilename())
        self.setWindowTitle("%s - %s[*]" %(self.name, fname))
        self.setWindowModified(True)
        
    # def listModulesShow(self):
    #     """Opens the Dialog which shows loaded/unloaded modules"""
    #     ml = ModuleListDialog(self.listModules)
    #     ml.exec_()
    #
    # def listModules(self):
    #     """Should return a list of all possible imports, used to test which modules are missing"""
    #     return [["MainChip", True, ""]]

    def projectStatusChanged(self):
        """Add File to recent file list"""
        self.updateTitleBar()
        
        if self.api.project().isUntitled(): return
        
        files = QSettings().value('recentFileList')
        if files is None:
            files = []

        try:
            files.remove(self.api.project().getFilename())
        except Exception:
            pass

        files.insert(0, self.api.project().getFilename())
        numRecentFiles = min(len(files), CWMainGUI.MaxRecentFiles)
        files = files[:numRecentFiles]

        QSettings().setValue('recentFileList', files)
        self.updateRecentFileActions()

    def updateRecentFileActions(self):
        """Update & Load the list of recent files"""
        files = QSettings().value('recentFileList')
        if files is not None:
            files_no = 0
            for f in files:
                text = "&%d %s" % (files_no + 1, Util.strippedName(f))
                self.recentFileActs[files_no].setText(text)
                self.recentFileActs[files_no].setData(f)
                self.recentFileActs[files_no].setVisible(True)
                files_no = files_no + 1

            for j in range(files_no, CWMainGUI.MaxRecentFiles):
                self.recentFileActs[j].setVisible(False)

    def openRecentFile(self):
        action = self.sender()
        if action:
            self.openProject(action.data())

    def openProject(self, fname = None):
        if not self.okToContinue():
            return
        if fname is None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open File', './projects/','ChipWhisperer Project (*.cwp)','', QFileDialog.DontUseNativeDialog)
            if not fname: return

        self.api.openProject(fname)
        self.updateStatusBar("Opening Project: " + fname)

    def saveProject(self):
        fname = self.api.project().getFilename()
        if self.api.project().isUntitled():
            fd = QFileDialog(self, 'Save New File', './projects/', 'ChipWhisperer Project (*.cwp)')
            fd.setOption(QFileDialog.DontUseNativeDialog)
            fd.setDefaultSuffix('cwp') # Will not append the file extension if using the static file dialog
            fd.setAcceptMode(QFileDialog.AcceptSave)
            fd.setViewMode(QFileDialog.Detail)
            if fd.exec_() != QDialog.Accepted:
                return

            fname = fd.selectedFiles()[0]

        self.api.saveProject(fname)
        self.updateStatusBar("Project Saved")

    def newProject(self):
        self.okToContinue()
        self.api.newProject()
        self.updateStatusBar("New Project Created")

    def setProject(self, proj):
        self.api.setProject(proj)

    def okToContinue(self):
        if self.api.project() is None: return False

        reply = SaveProjectDialog.getSaveProjectDialog(self, self.api.project())
        if reply == QDialogButtonBox.RejectRole:
            return False
        elif reply == QDialogButtonBox.YesRole:
            self.saveProject()
            return True
        elif reply == QDialogButtonBox.NoRole:
            return True
        else:
            raise AttributeError("Internal Error: Invalid role: %s" % str(reply))

    def consolidateDialog(self):
        msgBox = QMessageBox()
        msgBox.setText("Consolidate will copy all data into the project directory.")
        msgBox.setInformativeText("Do you want to keep the original trace files?")
        msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
        msgBox.setDefaultButton(QMessageBox.Yes)
        ret = msgBox.exec_()

        if ret == QMessageBox.Yes:
            self.api.project().consolidate(keepOriginals = True)
        elif ret == QMessageBox.No:
            self.api.project().consolidate(keepOriginals = False)

    def updateStatusBar(self, message):
        msg = message + " (" +  datetime.now().strftime('%d/%m/%y %H:%M:%S') + ")"
        print "Status: " + msg
        self.statusBar().showMessage(msg)

    def runScript(self, mod):
        self.updateStatusBar("Running Script: %s" % mod.name)
        m = mod(self.api)
        m.run()
        self.updateStatusBar("Finished Script: %s" % mod.name)

    def exceptionHandlerDialog(self, etype, value, trace):
        """ Handler for uncaught exceptions (for unknown Errors only - fix when you find one)."""

        if issubclass(etype, KeyboardInterrupt): # So program can exit with Ctrl + C.
            self.close()
            return

        if issubclass(etype, Warning):
            print str(value)
            QMessageBox.warning(self, "Warning", str(value))
            return

        details = "".join(traceback.format_exception(etype, value, trace))
        print details
        dialog = QMessageBox(QMessageBox.Critical, "Error",
                    "An error has occurred:<br>%s<br><br>It is usually safe to continue, but save your work just in case.<br>"
                    "If the error occurs again, please create a new ticket <a href='https://www.assembla.com/spaces/chipwhisperer/tickets'>here</a> informing the details bellow." % value, QMessageBox.Close, self)
        dialog.setTextFormat(Qt.RichText) # this is what makes the links clickable
        dialog.setDetailedText(details)
        dialog.exec_()

    @staticmethod
    def getInstance():
        return CWMainGUI.instance

def main():    
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Window Demo")
    ex = CWMainGUI(CWCoreAPI(), app.applicationName())
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()