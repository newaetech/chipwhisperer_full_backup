#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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
#
#=================================================

"""
    This file based on PyQtGraph parameterTypes.py
    It implements new Parameter types needed by CW and fixes some issues in the original PyQtGraph classes
"""

import os

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
from pyqtgraph.parametertree.Parameter import registerParameterType
from pyqtgraph.parametertree.ParameterItem import ParameterItem
from pyqtgraph.parametertree.parameterTypes import WidgetParameterItem, EventProxy, ListParameterItem, Parameter, ActionParameterItem, TextParameterItem, GroupParameterItem, ListParameter
from pyqtgraph import pixmaps
from pyqtgraph.widgets.SpinBox import SpinBox


# User defined Help Window used in the parameters
from chipwhisperer.common.api.settings import Settings

helpwnd = None


def showHelpWindow(curParam):
    """
    Helper to show the help text. If class defines a 'helpwnd' it uses that function, otherwise
    a simple message box is called upon.
    """

    nametext = curParam.param.opts['name']
    helptext = curParam.param.opts['help']
    hdrtext = '-' * len(nametext)
    helptext = helptext.replace('%namehdr%', '%s\n%s\n\n' % (nametext, hdrtext))

    if helpwnd:
        helpwnd(helptext, curParam)
    else:
        # Shitty default window
        if hasattr(curParam, 'widget'):
            wdgt = curParam.widget
        else:
            wdgt = None
        QtGui.QMessageBox.information(wdgt, 'Help: %s' % nametext, helptext, QtGui.QMessageBox.Cancel,
                                      QtGui.QMessageBox.Cancel)


def drawHelpIcon(curParam, layout=None, removeDefault=True):
    """Add a single help icons to a Parameter Item. Also removes the "default" button which isn't really used in our applications"""

    if layout is None:
        layout = curParam.layoutWidget.layout()
        standard_icon = curParam.layoutWidget.style().standardIcon
    else:
        standard_icon = QtGui.qApp.style().standardIcon

    if removeDefault:
        # Bonus: We don't want the default button so delete it here
        numitems = layout.count()
        lastitem = layout.itemAt(numitems - 1)

        if (type(lastitem.widget()) == QtGui.QPushButton) and lastitem.widget().width() == 20:
            lastitem.widget().deleteLater()

    # If help option add the button
    if 'help' in curParam.param.opts:
        buthelp = QtGui.QPushButton()
        buthelp.setFixedWidth(20)
        buthelp.setFixedHeight(20)
        buthelp.setIcon(standard_icon(QtGui.QStyle.SP_TitleBarContextHelpButton))
        buthelp.clicked[bool].connect(lambda ignored: showHelpWindow(curParam))
        layout.addWidget(buthelp)


def ignore_param(a=None, b=None):
    pass

def __init___fix(self, param, depth):
    """
    Fix some bugs with PyQtGraph:
    - Disconnect the signals when the widget is gone.
    - Adjusts the parameter height to also take into account the edit widget
    """

    ParameterItem.__init__(self, param, depth)

    self.hideWidget = True  ## hide edit widget, replace with label when not selected
                            ## set this to False to keep the editor widget always visible


    ## build widget into column 1 with a display label and default button.
    w = self.makeWidget()
    self.widget = w

    self.widget.destroyed.connect(lambda: self.param.sigValueChanged.disconnect(self.valueChanged))
    self.widget.destroyed.connect(lambda: self.param.sigChildAdded.disconnect(self.childAdded))
    self.widget.destroyed.connect(lambda: self.param.sigChildRemoved.disconnect(self.childRemoved))
    self.widget.destroyed.connect(lambda: self.param.sigNameChanged.disconnect(self.nameChanged))
    self.widget.destroyed.connect(lambda: self.param.sigLimitsChanged.disconnect(self.limitsChanged))
    self.widget.destroyed.connect(lambda: self.param.sigDefaultChanged.disconnect(self.defaultChanged))
    self.widget.destroyed.connect(lambda: self.param.sigOptionsChanged.disconnect(self.optsChanged))
    self.widget.destroyed.connect(lambda: self.param.sigParentChanged.disconnect(self.parentChanged))

    self.eventProxy = EventProxy(w, self.widgetEventFilter)

    opts = self.param.opts
    if 'tip' in opts:
        w.setToolTip(opts['tip'])

    self.updateDefaultBtn = ignore_param
    self.defaultBtn = ignore_param
    self.defaultBtn.setHidden = ignore_param
    self.defaultBtn.setEnabled = ignore_param

    #No default button
    #self.defaultBtn = QtGui.QPushButton()
    #self.defaultBtn.setEnabled(False)
    #self.defaultBtn.setHidden(True)
    #self.defaultBtn.setFixedWidth(20)
    #self.defaultBtn.setFixedHeight(20)
    #self.defaultBtn.setIcon(QtGui.QIcon(pixmaps.getPixmap('default')))
    #self.defaultBtn.clicked.connect(self.defaultClicked)
    #self.defaultBtn.setHidden = ignore_param
    #self.defaultBtn.setEnabled = ignore_param

    self.displayLabel = QtGui.QLabel()

    layout = QtGui.QHBoxLayout()
    layout.setContentsMargins(0, 0, 0, 0)
    layout.setSpacing(2)
    layout.addWidget(w)
    layout.addWidget(self.displayLabel)

    drawHelpIcon(self, layout, False)

    #No default button
    #layout.addWidget(self.defaultBtn)

    self.layoutWidget = QtGui.QWidget()
    self.layoutWidget.setLayout(layout)
    self.layoutWidget.setMinimumHeight(w.minimumHeight())

    if w.sigChanged is not None:
        w.sigChanged.connect(self.widgetValueChanged)

    if hasattr(w, 'sigChanging'):
        w.sigChanging.connect(self.widgetValueChanging)

    ## update value shown in widget.
    if opts.get('value', None) is not None:
        self.valueChanged(self, opts['value'], force=True)
    else:
        ## no starting value was given; use whatever the widget has
        self.widgetValueChanged()

    #self.updateDefaultBtn()

WidgetParameterItem.__init__ = __init___fix


def __init___fix2(self, param, depth):
    ParameterItem.__init__(self, param, depth)
    self.updateDepth(depth)

    self.addItem = None

    if 'addLoadSave' in param.opts:
        self.loadBtn = QtGui.QPushButton(QtGui.QIcon(":/images/open.png"),"")
        self.loadBtn.setCheckable(False)
        self.loadBtn.setFixedSize(20, 20)
        self.loadBtn.clicked.connect(self.loadBtnClicked)
        self.saveBtn = QtGui.QPushButton(QtGui.QIcon(":/images/save.png"),"")
        self.saveBtn.setCheckable(False)
        self.saveBtn.setFixedSize(20, 20)
        self.saveBtn.clicked.connect(self.saveBtnClicked)

        layout = QtGui.QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(11)

        layout.addStretch()
        layout.addWidget(self.loadBtn)
        layout.addWidget(self.saveBtn)

        self.widget = QtGui.QWidget()
        self.widget.setLayout(layout)

GroupParameterItem.__init__ = __init___fix2


def loadBtnClicked(self):
    fname = QtCore.QSettings().value("Group_"+ self.param.name(), Settings().value("project-home-dir", "/"))
    fname, _ = QtGui.QFileDialog.getOpenFileName(None, 'Load settings from:', fname, "*.cwset")
    if fname:
        self.param.opts['addLoadSave'][0](fname)

GroupParameterItem.loadBtnClicked = loadBtnClicked


def saveBtnClicked(self):
    fname = QtCore.QSettings().value("Group_"+ self.param.name(), Settings().value("project-home-dir", "/"))
    fname, _ = QtGui.QFileDialog.getSaveFileName(None, 'Save settings to:', fname, "*.cwset")
    if fname:
        QtCore.QSettings().setValue("Group_"+ self.param.name(), fname)
        self.param.opts['addLoadSave'][1](fname)

GroupParameterItem.saveBtnClicked = saveBtnClicked


def treeWidgetChanged_fix(self):
    ParameterItem.treeWidgetChanged(self)
    treeWidget = self.treeWidget()
    if treeWidget is not None:
        treeWidget.setFirstItemColumnSpanned(self, True)

    ## add all widgets for this item into the tree
    if hasattr(self, "widget"):
        tree = self.treeWidget()
        if tree is None:
            return
        tree.setItemWidget(self, 1, self.widget)
        self.selected(False)

GroupParameterItem.treeWidgetChanged = treeWidgetChanged_fix


class WidgetParameterItemHelp(WidgetParameterItem):
    def __init__(self, *args, **kwargs):
        super(WidgetParameterItemHelp, self).__init__(*args, **kwargs)
        drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass


class ListParameterItemHelp(ListParameterItem):
    def __init__(self, *args, **kwargs):
        super(ListParameterItemHelp, self).__init__(*args, **kwargs)
        drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass


class ActionParameterItemHelp(ActionParameterItem):
    def __init__(self, *args, **kwargs):
        super(ActionParameterItemHelp, self).__init__(*args, **kwargs)
        drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass


class TextParameterItemHelp(TextParameterItem):
    def __init__(self, *args, **kwargs):
        super(TextParameterItemHelp, self).__init__(*args, **kwargs)
        drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass

class FileParameterItemHelp(WidgetParameterItemHelp):
    def __init__(self, *args, **kwargs):
        super(FileParameterItemHelp, self).__init__(*args, **kwargs)

        fileButton = QtGui.QPushButton()
        fileButton.setFixedWidth(20)
        fileButton.setFixedHeight(20)
        fileButton.setIcon(self.layoutWidget.style().standardIcon(QtGui.QStyle.SP_FileDialogContentsView))
        fileButton.clicked[bool].connect(self.openFile)
        self.layoutWidget.layout().addWidget(fileButton)
        fname = QtCore.QSettings().value(self.param.opts["name"])
        if fname:
            self.param.setValue(fname)

    def makeWidget(self):
        w = QtGui.QLineEdit()
        w.sigChanged = w.editingFinished
        w.value = w.text
        w.setValue = w.setText
        w.sigChanging = w.textChanged
        return w

    def openFile(self):
        if self.param.opts.get("filter", None) == "dir":
            fname = QtGui.QFileDialog.getExistingDirectory(None, "Open Directory", self.param.value(),
                                                           QtGui.QFileDialog.ShowDirsOnly)
        else:
            fname, _ = QtGui.QFileDialog.getOpenFileName(None, 'Get file path', self.param.value(),
                                                         self.param.opts.get("filter", "*"))
        if fname:
            self.param.setValue(fname)


class FileParameter(Parameter):
    itemClass = FileParameterItemHelp

registerParameterType('file', FileParameter, override=True)

classmapping = {
    "<class 'pyqtgraph.parametertree.parameterTypes.WidgetParameterItem'>": WidgetParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.ListParameterItem'>": ListParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.ActionParameterItem'>": ActionParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.TextParameterItem'>": TextParameterItemHelp
}


class QWidgetWSignals(QtGui.QWidget):
    sigValueChanged = QtCore.Signal(object)  # (self)
    sigValueChanging = QtCore.Signal(object, object)  # (self, value)  sent immediately; no delay.


class RangeParameterItem(WidgetParameterItem):
    """
    WidgetParameterItem subclass providing two int selection for range
    """

    def __init__(self, param, depth):
        self.targetValue = None
        WidgetParameterItem.__init__(self, param, depth)

    def svChangedEmit(self):
        if self.validateLimits():
            self.widget.sigValueChanged.emit(self)

    def svChangingEmit(self, val):
        self.widget.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()) )

    def svLowChanging(self):
        if self.validateLimits("high"):
            self.svChangingEmit(None)

    def svHighChanging(self):
        if self.validateLimits("low"):
            self.svChangingEmit(None)

    def validateLimits(self, change=0):
        try:
            fixedsize = self.param.opts['fixedsize']
        except KeyError:
            low = self.wlow.value()
            high = self.whigh.value()
            if high < low and change == "high":
                self.whigh.setValue(low, update=True, delaySignal=True)
            elif low > high and change == "low":
                self.wlow.setValue(high, update=True, delaySignal=True)
            return True

        if fixedsize == 0:
            return True

        if (self.whigh.value() - self.wlow.value()) != fixedsize:

            # Limits wrong! Adjust as needed

            attempts = 0

            while change != "done":
                if change == "low":

                    attempts += 1

                    lowval = self.whigh.value() - fixedsize
                    if self.wlow.valueInRange(lowval):
                        self.wlow.setValue(lowval, update=True, delaySignal=True)
                        change = "done"
                    else:
                        self.wlow.setValue(self.wlow.opts['bounds'][0], update=True, delaySignal=True)
                        change = "high"

                # Prefer to change whigh
                if change == "high":

                    attempts += 1

                    highval = fixedsize + self.wlow.value()
                    if self.whigh.valueInRange(highval):
                        self.whigh.setValue(highval, update=True, delaySignal=True)
                        change = "done"
                    else:
                        self.whigh.setValue(self.whigh.opts['bounds'][1], update=True, delaySignal=True)
                        change = "low"

                # Give up - not possible most likely
                if attempts == 4:
                    return False

            return False
        else:
            return True

    def makeLayout(self):
        opts = self.param.opts
        defs = {
                'value': 0, 'min': None, 'max': None, 'int': True,
                'step': 1.0, 'minStep': 1.0, 'dec': False, 'fixedsize':0,
                'siPrefix': False, 'suffix': '', 'visible':True,
            }
        defs.update(opts)

        wlow = SpinBox()
        wlow.setMaximumWidth(80)
        wlow.setContentsMargins(0,0,0,0)
        wlow.setAlignment(QtCore.Qt.AlignRight)
        whigh = SpinBox()
        whigh.setMaximumWidth(80)
        whigh.setAlignment(QtCore.Qt.AlignRight)
        whigh.setContentsMargins(0,0,0,0)

        # The following required for pyqtgraph > 0.9.10
        for k in defs:
            wlow.opts[k] = defs[k]
        for k in defs:
            whigh.opts[k] = defs[k]

        if 'limits' in opts:
            defs['bounds'] = opts['limits']

        defs["value"] = self.param.opts["value"][0]
        wlow.setOpts(**defs)
        defs["value"] = self.param.opts["value"][1]
        whigh.setOpts(**defs)

        whigh.sigValueChanged.connect(self.svChangedEmit)
        whigh.sigValueChanging.connect(self.svHighChanging)
        wlow.sigValueChanged.connect(self.svChangedEmit)
        wlow.sigValueChanging.connect(self.svLowChanging)

        l = QtGui.QHBoxLayout()
        l.setAlignment(QtCore.Qt.AlignLeft)
        l.setContentsMargins(0,0,0,0)
        l.addWidget(QtGui.QLabel("("))
        l.addWidget(wlow)
        l.addWidget(QtGui.QLabel(","))
        l.addWidget(whigh)
        l.addWidget(QtGui.QLabel(")"))
        l.addStretch()

        self.wlow = wlow
        self.whigh = whigh
        return l

    def makeWidget(self):
        l = self.makeLayout()

        w = QWidgetWSignals()
        w.setLayout(l)

        w.sigChanged = w.sigValueChanged
        w.sigChanging = w.sigValueChanging
        w.value = self.value
        w.setValue = self.setValue
        return w

    def value(self):
        return (self.wlow.value(), self.whigh.value())

    def setValue(self, val):
        self.wlow.setValue(val[0])
        self.whigh.setValue(val[1])

    def limitsChanged(self, param, limits):
        """Called when the parameter's limits have changed"""
        # Do we need this? copied from example
        ParameterItem.limitsChanged(self, param, limits)

        self.wlow.setOpts(bounds=limits)
        self.whigh.setOpts(bounds=limits)


class RangeParameter(Parameter):
    itemClass = RangeParameterItem

registerParameterType('range', RangeParameter, override=True)


class RangeParameterGraphItem(RangeParameterItem):
    def makeWidget(self):

        l = self.makeLayout()

        graphIcon = QtGui.QIcon()
        graphIcon.addFile(':/images/wavelimits.png', state=QtGui.QIcon.On)
        graphIcon.addFile(':/images/wavelimitsoff.png', state=QtGui.QIcon.Off)

        self.graphBtn = QtGui.QPushButton()
        self.graphBtn.setToolTip("Allows range selection by draging the interval boundaries on the graph")
        self.graphBtn.setFixedWidth(20)
        self.graphBtn.setFixedHeight(20)
        self.graphBtn.setIcon(graphIcon)
        self.graphBtn.setCheckable(True)

        l.addWidget(QtGui.QLabel("  "))
        l.addWidget(self.graphBtn)
        l.addWidget(QtGui.QLabel("  "))

        w = QWidgetWSignals()
        w.setLayout(l)
        w.setMinimumHeight(33)

        w.sigChanged = w.sigValueChanged
        w.sigChanging = w.sigValueChanging
        w.value = self.value
        w.setValue = self.setValue

        self.wlow.sigValueChanged.connect(self.sbChanged)
        self.whigh.sigValueChanged.connect(self.sbChanged)

        opts = self.param.opts
        self.pg = opts['graphwidget']
        self.lri = pg.LinearRegionItem(values=(self.wlow.value(), self.whigh.value()))
        self.pg.addPersistantItem(self.lri)
        self.lri.setVisible(False)
        self.lri.sigRegionChanged.connect(self.lriChanged)
        return w

    def lriChanged(self):
        new = self.lri.getRegion()
        self.wlow.setValue(new[0])
        self.whigh.setValue(new[1])

    def sbChanged(self):
        self.lri.setRegion((self.wlow.value(), self.whigh.value()))

    def limitsChanged(self, param, limits):
        """Called when the parameter's limits have changed"""
        # Do we need this? copied from example
        ParameterItem.limitsChanged(self, param, limits)

        self.wlow.setOpts(bounds=limits)
        self.whigh.setOpts(bounds=limits)
        self.lri.setBounds(limits)

    def showEditor(self):
        self.widget.show()
        self.displayLabel.hide()
        self.widget.setFocus(QtCore.Qt.OtherFocusReason)
        self.lri.setVisible(True)

    def hideEditor(self):
        self.widget.hide()
        self.displayLabel.show()
        self.lri.setVisible(self.graphBtn.isChecked())


class RangeParameterGraph(Parameter):
    itemClass = RangeParameterGraphItem

registerParameterType('rangegraph', RangeParameterGraph, override=True)


class FilelistItem(WidgetParameterItem):
    """
    WidgetParameterItem subclass providing list of files with copy/add/remove
    """

    def __init__(self, param, depth):
        self.targetValue = None
        self.oldbackgrounds = []
        WidgetParameterItem.__init__(self, param, depth)
        self.hideWidget = False


    def svChangedEmit(self):
        if self.validateLimits():
            self.widget.sigValueChanged.emit(self)

    def svChangingEmit(self, val):
        self.widget.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()))

    def setRows(self, rows):
        self.table.setRowCount(rows)

        height = 0
        for i in range(0, self.table.rowCount()):
            height += self.table.rowHeight(i)

        self.table.setMaximumHeight(height + 10)
        self.table.setMinimumHeight(height + 10)

    def addFileToList(self, desc, filename, fixedName=False):
        rnum = self.table.rowCount()
        self.setRows(rnum + 1)

        descitem = QtGui.QTableWidgetItem(desc)
        descitem.setFlags(descitem.flags() & ~QtCore.Qt.ItemIsSelectable)
        if fixedName:
            descitem.setFlags(descitem.flags() & ~QtCore.Qt.ItemIsEditable)
        self.table.setItem(rnum, 0, descitem)

        fnameitem = QtGui.QTableWidgetItem(filename)
        fnameitem.setFlags(fnameitem.flags() & ~QtCore.Qt.ItemIsEditable)
        fnameitem.setFlags(fnameitem.flags() & ~QtCore.Qt.ItemIsSelectable)
        self.table.setItem(rnum, 1, fnameitem)

        self.oldbackgrounds.append(self.table.item(rnum, 0).background())
        self.table.resizeColumnsToContents()

        if hasattr(self, 'widget'):
            self.treeWidgetChanged()

    def copyFile(self):
        if self.table.currentRow() < 0:
            QtGui.QMessageBox.warning(None, "Copy Selection", "No item selected")
        else:
            rnum = self.table.currentRow()
            desc = self.table.item(rnum, 0).text() + " Copy"
            oldfname = self.table.item(rnum, 1).text()

            # Get new filename
            fname, _ = QtGui.QFileDialog.getSaveFileName(None, 'Save Copy as', oldfname, '*.py')

            if fname and len(fname) > 0:

                # New files
                newfile = open(fname, 'w')
                oldfile = open(oldfname, 'r')

                # Copy text data
                newfile.write(oldfile.read())

                # Close files
                newfile.close()
                oldfile.close()

                # Add to list
                self.addFileToList(desc, fname)

    def editFile(self):
        if self.table.currentRow() < 0:
            QtGui.QMessageBox.warning(None, "Edit Selection", "No item selected")
        else:
            rnum = self.table.currentRow()
            desc = self.table.item(rnum, 0).text()
            fname = self.table.item(rnum, 1).text()

            if self.editor:
                self.editor(filename=fname, filedesc=desc)

    def removeFile(self):
        if self.table.currentRow() < 0:
            QtGui.QMessageBox.warning(None, "Remove File", "No item selected")
        else:
            rnum = self.table.currentRow()
            self.table.removeRow(rnum)

    def addFile(self):
        fname, _ = QtGui.QFileDialog.getOpenFileName(None, 'Add File to Project', '.', '*.py')

        if fname is not None:
            self.addFileToList("Default Name", fname)

    def setDefault(self, rnum=None):
        if rnum is None and self.table.currentRow() < 0:
            QtGui.QMessageBox.warning(None, "Set Default", "No item selected")
        else:
            if rnum is None:
                rnum = self.table.currentRow()
            else:
                self.table.setCurrentCell(rnum, 0)

            highlight = QtGui.QBrush(QtGui.QColor(0, 255, 0))

            for row in range(0, self.table.rowCount()):
                for col in range(0, self.table.columnCount()):
                    item = self.table.item(row, col)
                    if row == rnum:
                        self.table.item(row, col).setBackground(highlight)
                        # item.setFlags(item.flags() | QtCore.Qt.ItemIsSelectable)
                    else:
                        self.table.item(row, col).setBackground(self.oldbackgrounds[row])
                        # item.setFlags(item.flags() & ~QtCore.Qt.ItemIsSelectable)

            if self.editor:
                desc = self.table.item(rnum, 0).text()
                fname = self.table.item(rnum, 1).text()
                self.editor(filename=fname, filedesc=desc, default=True)

    def makeLayout(self):
        opts = self.param.opts

        if 'editor' in opts.keys():
            self.editor = opts['editor']
        else:
            self.editor = None

        self.table = QtGui.QTableWidget()
        self.table.setColumnCount(2)
        self.table.horizontalHeader().hide()
        self.table.verticalHeader().hide()
        self.table.horizontalHeader().setStretchLastSection(True)
        self.table.setSelectionBehavior(QtGui.QAbstractItemView.SelectRows)
        # self.table.horizontalHeader().setResizeMode(QtGui.QHeaderView.Interactive)

        self.setRows(0)

        buttonAdd = QtGui.QPushButton('Add')
        buttonRemove = QtGui.QPushButton('Remove')
        buttonEdit = QtGui.QPushButton('Edit')
        buttonCopy = QtGui.QPushButton('Copy')
        buttonActive = QtGui.QPushButton('Set Active')

        buttonCopy.clicked.connect(self.copyFile)
        buttonEdit.clicked.connect(self.editFile)
        buttonActive.clicked.connect(self.setDefault)
        buttonAdd.clicked.connect(self.addFile)
        buttonRemove.clicked.connect(self.removeFile)

        buttonL = QtGui.QGridLayout()
        buttonL.addWidget(buttonAdd, 0, 0)
        buttonL.addWidget(buttonRemove, 0, 1)
        buttonL.addWidget(buttonEdit, 0, 2)
        buttonL.addWidget(buttonCopy, 1, 0)
        buttonL.addWidget(buttonActive, 1, 1, 1, 2)
        # buttonL.addStretch()

        l = QtGui.QVBoxLayout()
        l.setContentsMargins(0, 0, 0, 0)

        l.addWidget(self.table)
        l.addLayout(buttonL)
        self.layout = l
        return l

    def makeWidget(self):
        l = self.makeLayout()

        w = QWidgetWSignals()
        w.setLayout(l)

        w.sigChanged = w.sigValueChanged
        w.sigChanging = w.sigValueChanging

        w.value = self.value
        w.setValue = self.setValue

        self.limitsChanged(self.param, self.param.opts['limits'])

        return w

    def value(self):
        return 0

    def setValue(self, val):
        pass

    def limitsChanged(self, param, limits):
        """Called when the parameter's limits have changed"""
        # Do we need this? copied from example
        ParameterItem.limitsChanged(self, param, limits)

        for l in limits:
            self.addFileToList(l[0], l[1])

        self.setDefault(0)

class FilelistParameter(Parameter):
    itemClass = FilelistItem

    def __init__(self, **opts):
        # # Parameter uses 'limits' option to define the set of allowed values
        if 'values' in opts:
            opts['limits'] = opts['values']
        if opts.get('limits', None) is None:
            opts['limits'] = []
        Parameter.__init__(self, **opts)
        self.setLimits(opts['limits'])

    def setLimits(self, limits):
        Parameter.setLimits(self, limits)

registerParameterType('filelist', FilelistParameter, override=True)


class SpinBoxWithSetItem(WidgetParameterItem):

    """ Spin-box with ability to have set/get functions """
    
    def __init__(self, *args, **kwargs):
        super(SpinBoxWithSetItem, self).__init__(*args, **kwargs)
        drawHelpIcon(self)

    def get_widget_defs(self):
        return {
            'value': 0, 'min': None, 'max': None, 'int': True,
            'step': 1.0, 'minStep': 1.0, 'dec': False,
            'siPrefix': False, 'suffix': '',
            'decimals': 10
        }

    def makeWidget(self):
        """Copy of SpinBox from PyQtGraph 0.9.10 & later, which adds special parameters we hack on"""
        opts = self.param.opts
        defs = self.get_widget_defs()

        defs.update(opts)
        if 'limits' in opts:
            defs['bounds'] = opts['limits']
        w = SpinBox()
        # This hack ensures compatibility between 0.9.10 and later
        for k in opts:
            w.opts[k] = opts[k]

        w.setOpts(**defs)
        w.sigChanged = w.sigValueChanged
        w.sigChanging = w.sigValueChanging

        return w

    def updateDefaultBtn(self):
        pass


class SpinBoxWithSet(Parameter):
    itemClass = SpinBoxWithSetItem

registerParameterType('int', SpinBoxWithSet, override=True)


class SpinBoxFloatWithSetItem(WidgetParameterItem):
    def get_widget_defs(self):
        return {
                'value': 0, 'min': None, 'max': None, 'int': False,
                'step': 1.0, 'minStep': 0.01, 'dec': False,
                'siPrefix': False, 'suffix': '',
                'decimals': 10
            }

class SpinBoxFloatWithSet(Parameter):
    itemClass = SpinBoxFloatWithSetItem

registerParameterType('float', SpinBoxFloatWithSet, override=True)


class LabelParameterItem(WidgetParameterItem):
    """ Class used for description of an item. Spans both columns. """

    def __init__(self, param, depth):
        ParameterItem.__init__(self, param, depth)
        self.setFirstColumnSpanned(True)
        self.hideWidget = True  ## hide edit widget, replace with label when not selected
        ## set this to False to keep the editor widget always visible

        ## build widget into column 1 with a display label and default button.
        w = self.makeWidget()
        self.widget = w
        self.eventProxy = EventProxy(w, self.widgetEventFilter)

        opts = self.param.opts
        if 'tip' in opts:
            w.setToolTip(opts['tip'])

        self.displayLabel = QtGui.QLabel()
        self.displayLabel.hide()

        self.defaultBtn = QtGui.QPushButton("")
        self.defaultBtn.hide()

        layout = QtGui.QHBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(w)
        layout.addWidget(self.displayLabel)
        layout.addWidget(self.defaultBtn)
        self.layoutWidget = QtGui.QWidget()
        self.layoutWidget.setLayout(layout)

        if w.sigChanged is not None:
            w.sigChanged.connect(self.widgetValueChanged)

        if hasattr(w, 'sigChanging'):
            w.sigChanging.connect(self.widgetValueChanging)

        ## update value shown in widget.
        if opts.get('value', None) is not None:
            self.valueChanged(self, opts['value'], force=True)
        else:
            ## no starting value was given; use whatever the widget has
            self.widgetValueChanged()

    def makeWidget(self):
        self.textBox = QtGui.QLabel()
        self.textBox.setSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.MinimumExpanding)
        self.textBox.setWordWrap(True)
        self.textBox.setStyleSheet("QLabel { color : gray; }")
        self.textBox.value = lambda: str(self.textBox.text())
        self.textBox.setValue = self.textBox.setText
        self.textBox.sigChanged = self.textBox.linkActivated
        return self.textBox

    def treeWidgetChanged(self):
        self.treeWidget().setFirstItemColumnSpanned(self, True)
        self.treeWidget().setItemWidget(self, 0, self.textBox)
        self.displayLabel = self.textBox


class LabelParameter(Parameter):
    """Editable string; displayed as large text box in the tree."""
    itemClass = LabelParameterItem

registerParameterType('label', LabelParameter, override=True)


#TODO: Fix parameter's to not skip setting same value, requires more effort than this.
def setValue_Fix(self, value, blockSignal=None):
    """
    Fixes the CW requirement to not skip the setValue call when the previous value is the same
    Useful when hardware reality is != software settings, and need to re-download things.
    """
    try:
        if blockSignal is not None:
            self.sigValueChanged.disconnect(blockSignal)
        self.opts['value'] = value
        self.sigValueChanged.emit(self, value)
    finally:
        if blockSignal is not None:
            self.sigValueChanged.connect(blockSignal)

Parameter.setValue = setValue_Fix


def optsChanged_Fix(self, param, opts):
    """Fixes a PyQtGraph bug. Call base class to hide/show group-type parameter item"""
    ParameterItem.optsChanged(self, param, opts)
    if 'addList' in opts:
        self.updateAddList()

GroupParameterItem.optsChanged = optsChanged_Fix


def setLimits(self, limits):
    """
    Fixes an incompatibility bug between CW and PyQtGraph Parameter classes.
    Prevents setValue being called when the current value is not within the new limits.
    """
    self.forward, self.reverse = self.mapping(limits)
    Parameter.setLimits(self, limits)

ListParameter.setLimits = setLimits