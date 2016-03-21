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

__author__ = "Colin O'Flynn"

import os
import sys
import traceback
from datetime import datetime
from pyqtgraph.parametertree import Parameter, ParameterTree
import chipwhisperer.common.ui.PythonConsole
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.ui.HelpWindow import HelpBrowser
from chipwhisperer.common.ui.TraceManagerDialog import TraceManagerDialog
from chipwhisperer.common.ui.project_text_editor import ProjectTextEditor
from chipwhisperer.common.utils import util

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
        self.setModal(True)
        layout = QVBoxLayout()
        layout.addWidget(QLabel("Save unsaved changes?"))
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Yes | QDialogButtonBox.No | QDialogButtonBox.Cancel)
        layout.addWidget(self.buttonBox)

        self.setWindowTitle("Unsaved Changes Detected")
        self._lastpushed = QDialogButtonBox.RejectRole

        detailedWidget = ProjectDiffWidget(self, project=self.parent().cwAPI.project())
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

class MainChip(QMainWindow):
    """
    This is the base GUI class, used for both the Analyzer and Capture software. It defines a number of
    useful features such as the ability to add docks, setting windows, consoles for logging errors, etc. 
    You can run a demo which shows the basic features, which would look like this:
    
    .. image:: /images/mainchip-demo.png
       
    """
    MaxRecentFiles = 4

    def __init__(self, cwCoreAPI, name="Demo", icon="cwicon"):
        super(MainChip, self).__init__()
        self.cwAPI = cwCoreAPI
        self.settings = QSettings()
        self.name = name
        sys.excepthook = self.exceptionHook
        self.manageTraces = TraceManagerDialog(self)
        self.cwAPI.setTraceManager(self.manageTraces)
        self.projEditWidget = ProjectTextEditor(self)
        self.lastMenuActionSection = None
        self.originalStdout = None
        self.initUI(icon)
        self.setCentralWidget(None)
        self.setDockNestingEnabled(True)
        self.helpbrowser = HelpBrowser(self)
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.cwAPI.signals.parametersChanged.connect(QCoreApplication.processEvents)
        self.cwAPI.signals.newProject.connect(lambda: self.projEditWidget.setProject(self.cwAPI.project()))
        self.cwAPI.signals.newProject.connect(lambda: self.cwAPI.project().signals.statusChanged.connect(self.projectChanged))

    def restoreSettings(self):
        self.restoreGeometry(self.settings.value("geometry"))
        self.restoreState(self.settings.value("windowState"))

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

        if visible == False:
            dock.toggleViewAction()
        
        #Add to "Windows" menu
        if addToWindows:
            self.windowMenu.addAction(dock.toggleViewAction())

        return dock
    
    def addSettings(self, tree, name):
        """Adds a dockwidget designed to store a ParameterTree, also adds to 'Windows' menu"""
        self.cwAPI.paramTrees.append(tree)
        dock = self.addDock(tree, name=name, area=Qt.TopDockWidgetArea)
        dock.setMaximumWidth(560)
        return dock

    def tabifyDocks(self, docks):
        for index in range(1, len(docks)):
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

    def clearAllSettings(self):
        """Clear all saved QSettings(), such as window location etc"""
        self.settings.clear()

    def reset(self):
        self.clearAllSettings()
        sys.exit()

    def exceptionHook(self, etype, value, trace):
        """
        Handler for all unhandled exceptions.
        """
        print "".join(traceback.format_exception(etype, value, trace))
        QMessageBox.information(self, u"Error", unicode(value))

    def closeEvent(self, event):
        """Called when window is closed, attempts to save state/geometry"""
        self.settings.setValue("geometry", self.saveGeometry())
        self.settings.setValue("windowState", self.saveState())

        if self.okToContinue():
            QMainWindow.closeEvent(self, event)
        else:
            event.ignore()

    def createFileActions(self):
        """Add the file actions (open/save/new)"""
        self.openRec = QMenu('Open &Recent')

        self.openAct = QAction(QIcon('open.png'), '&Open', self,
                               shortcut=QKeySequence.Open,
                               statusTip='Open Project File',
                               triggered=self.openProject)

        self.saveAct = QAction(QIcon('save.png'), '&Save', self,
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

        for i in range(MainChip.MaxRecentFiles):
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
        self.fileMenu.addAction(self.saveAct)
#        self.fileMenu.addAction(self.importAct)
        self.fileMenu.addMenu(self.openRec)
        for i in range(MainChip.MaxRecentFiles):
            self.openRec.addAction(self.recentFileActs[i])
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.exitAct)
        
        self.projectMenu = self.menuBar().addMenu("&Project")
        self.traceManageAct = QAction('&Manage Traces', self, statusTip='Add/Remove Traces from Project', triggered=self.manageTraces.show)
        self.projectMenu.addAction(self.traceManageAct)
        self.consolidateAct = QAction('&Consolidate', self, statusTip='Copy trace files to project directory', triggered=self.consolidateDialog)
        self.projectMenu.addAction(self.consolidateAct)
        self.showProjFileAct = QAction('&Project File Editor (Text)', self, statusTip='Edit Project File', triggered=self.projEditDock.show)
        self.projectMenu.addAction(self.showProjFileAct)
            
        self.toolMenu= self.menuBar().addMenu("&Tools")
            
        self.windowMenu = self.menuBar().addMenu("&Windows")        
                
        self.helpMenu = self.menuBar().addMenu("&Help")
        self.helpManualAct = QAction('&Tutorial/User Manual', self, statusTip='Everything you need to know', triggered=self.helpdialog)
        # self.helpListAct = QAction('&List Enabled/Disable Modules', self, statusTip="Check if you're missing modules", triggered=self.listModulesShow)
        self.helpAboutAct = QAction('&About', self, statusTip='About Dialog', triggered=self.aboutdialog)
        self.helpMenu.addAction(self.helpManualAct)
        # self.helpMenu.addAction(self.helpListAct)
        self.helpMenu.addAction(self.helpAboutAct)
            
    def initUI(self, icon="cwicon"):
        """Setup the UI, creating statusbar, setting title, menus, etc"""
        self.statusBar()
        self.setWindowIcon(QIcon(":/images/%s.png" % icon))
        self.recentFileActs = []
        self.createFileActions()
        self.projEditDock = self.addDock(self.projEditWidget, name="Project Text Editor", area=Qt.BottomDockWidgetArea, visible=False, addToWindows=False)
        self.createMenus()
        self.updateRecentFileActions()

        # Project editor dock
        self.paramScriptingDock = self.addConsole("Script Commands", visible=False)
        ExtendedParameter.paramScriptingOutput = self.paramScriptingDock.widget()
        self.consoleDock = self.addConsole()
        self.pythonConsoleDock = self.addPythonConsole()
        self.tabifyDocks([self.projEditDock, self.paramScriptingDock, self.pythonConsoleDock, self.consoleDock])
        self.setBaseSize(800,600)
        self.show()
        
    def updateTitleBar(self):
        """Update filename shown in title bar"""
        fname = os.path.basename(self.cwAPI.project().getFilename())
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

    def projectChanged(self):
        """Add File to recent file list"""
        self.updateTitleBar()
        
        if self.cwAPI.project().isUntitled(): return
        
        files = self.settings.value('recentFileList')
        if files is None:
            files = []

        try:
            files.remove(self.cwAPI.project().getFilename())
        except Exception:
            pass

        files.insert(0, self.cwAPI.project().getFilename())
        numRecentFiles = min(len(files), MainChip.MaxRecentFiles)
        files = files[:numRecentFiles]

        self.settings.setValue('recentFileList', files)
        self.updateRecentFileActions()

    def updateRecentFileActions(self):
        """Update & Load the list of recent files"""
        files = self.settings.value('recentFileList')
        if files is not None:
            files_no = 0
            for f in files:
                text = "&%d %s" % (files_no+1, util.strippedName(f))
                self.recentFileActs[files_no].setText(text)
                self.recentFileActs[files_no].setData(f)
                self.recentFileActs[files_no].setVisible(True)
                files_no = files_no + 1

            for j in range(files_no, MainChip.MaxRecentFiles):
                self.recentFileActs[j].setVisible(False)

    def openRecentFile(self):
        action = self.sender()
        if action:
            self.openProject(action.data())

    def openProject(self, fname = None):
        self.okToContinue()
        if fname is None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open File', './projects/','ChipWhisperer Project (*.cwp)','', QFileDialog.DontUseNativeDialog)
            if not fname: return

        self.cwAPI.openProject(fname)
        self.updateStatusBar("Opening Project: " + fname)

    def saveProject(self):
        fname = self.cwAPI.project().getFilename()
        if self.cwAPI.project().isUntitled():
            fd = QFileDialog(self, 'Save New File', './projects/', 'ChipWhisperer Project (*.cwp)')
            fd.setOption(QFileDialog.DontUseNativeDialog)
            fd.setDefaultSuffix('cwp')
            fd.setAcceptMode(QFileDialog.AcceptSave)
            fd.setViewMode(QFileDialog.Detail)
            if fd.exec_() != QDialog.Accepted:
                return

            fname = fd.selectedFiles()[0]

        self.cwAPI.saveProject(fname)
        self.updateStatusBar("Project Saved")

    def newProject(self):
        self.okToContinue()
        self.cwAPI.newProject()
        self.updateStatusBar("New Project Created")

    def setProject(self, proj):
        self.cwAPI.setProject(proj)

    def okToContinue(self):
        if self.cwAPI.project() is None: return False

        reply = SaveProjectDialog.getSaveProjectDialog(self, self.cwAPI.project())
        if reply == QDialogButtonBox.RejectRole:
            return False
        elif reply == QDialogButtonBox.YesRole:
            self.saveProject()
            return True
        elif reply == QDialogButtonBox.NoRole:
            return True
        else:
            raise AttributeError("Invalid role: %s" % str(reply))

    def consolidateDialog(self):
        msgBox = QMessageBox()
        msgBox.setText("Consolidate will copy all data into the project directory.")
        msgBox.setInformativeText("Do you want to keep the original trace files?")
        msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)
        msgBox.setDefaultButton(QMessageBox.Yes)
        ret = msgBox.exec_()

        if ret == QMessageBox.Yes:
            self.cwAPI.project().consolidate(keepOriginals = True)
        elif ret == QMessageBox.No:
            self.cwAPI.project().consolidate(keepOriginals = False)

    def updateStatusBar(self, message):
        msg = message + " (" +  datetime.now().strftime('%d/%m/%y %H:%M:%S') + ")"
        print "Status: " + msg
        self.statusBar().showMessage(msg)

    def runScript(self, mod):
        if mod is None:
            return
        self.updateStatusBar("Running Script: %s" % mod.name())
        m = mod.userScript(self.cwAPI)
        m.run()
        self.updateStatusBar("Finished Script: %s" % mod.name())

def main():    
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Window Demo")
    ex = MainChip(CWCoreAPI(), app.applicationName())
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()