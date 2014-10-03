#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# This file based on PyQtGraph parameterTypes.py
#=================================================
from pyqtgraph.Qt import QtCore, QtGui
from pyqtgraph.python2_3 import asUnicode
from pyqtgraph.parametertree.Parameter import Parameter, registerParameterType
from pyqtgraph.parametertree.ParameterItem import ParameterItem
from pyqtgraph.parametertree.parameterTypes import WidgetParameterItem
from pyqtgraph.pgcollections import OrderedDict
from pyqtgraph.widgets.SpinBox import SpinBox
import pyqtgraph as pg
import chipwhisperer.common.qrc_resources

class SigStuff(QtGui.QWidget):
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
            self.sigs.sigValueChanged.emit(self)

    def svChangingEmit(self, val):
        self.sigs.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()) )

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
        self.sigs = SigStuff()
        opts = self.param.opts
        defs = {
                'value': 0, 'min': None, 'max': None, 'int': True,
                'step': 1.0, 'minStep': 1.0, 'dec': False, 'fixedsize':0,
                'siPrefix': False, 'suffix': ''
            }
        defs.update(opts)
        if 'limits' in opts:
            defs['bounds'] = opts['limits']
        wlow = SpinBox()
        wlow.setOpts(**defs)

        whigh = SpinBox()
        whigh.setOpts(**defs)

        whigh.sigValueChanged.connect(self.svChangedEmit)
        whigh.sigValueChanging.connect(self.svHighChanging)
        wlow.sigValueChanged.connect(self.svChangedEmit)
        wlow.sigValueChanging.connect(self.svLowChanging)

        l = QtGui.QHBoxLayout()
        l.setContentsMargins(0,0,0,0)

        l.addWidget(wlow)
        l.addWidget(QtGui.QLabel(" : "))
        l.addWidget(whigh)

        self.wlow = wlow
        self.whigh = whigh
        return l

    def makeWidget(self):
        l = self.makeLayout()

        w = QtGui.QWidget()
        w.setLayout(l)

        w.sigChanged = self.sigs.sigValueChanged
        w.sigChanging = self.sigs.sigValueChanging
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
        self.graphBtn.setFixedWidth(20)
        self.graphBtn.setFixedHeight(20)
        self.graphBtn.setIcon(graphIcon)
        self.graphBtn.setCheckable(True)
        self.graphBtn.clicked[bool].connect(self.buttonPressed)

        l.addWidget(self.graphBtn)

        w = QtGui.QWidget()
        w.setLayout(l)

        w.sigChanged = self.sigs.sigValueChanged
        w.sigChanging = self.sigs.sigValueChanging
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

    def buttonPressed(self, status):
        self.lri.setVisible(status)

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
            self.sigs.sigValueChanged.emit(self)

    def svChangingEmit(self, val):
        self.sigs.sigValueChanging.emit(self, (self.wlow.value(), self.whigh.value()))

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
        self.sigs = SigStuff()
        opts = self.param.opts
        # defs = {
        #        'value': 0, 'min': None, 'max': None, 'int': True,
        #        'step': 1.0, 'minStep': 1.0, 'dec': False, 'fixedsize':0,
        #        'siPrefix': False, 'suffix': ''
        #    }
        # defs.update(opts)

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
        buttonAdd.setEnabled(False)
        buttonRemove = QtGui.QPushButton('Remove')
        buttonRemove.setEnabled(False)
        buttonEdit = QtGui.QPushButton('Edit')
        buttonCopy = QtGui.QPushButton('Copy')
        buttonActive = QtGui.QPushButton('Set Active')

        buttonCopy.clicked.connect(self.copyFile)
        buttonEdit.clicked.connect(self.editFile)
        buttonActive.clicked.connect(self.setDefault)

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

        w = QtGui.QWidget()
        w.setLayout(l)

        w.sigChanged = self.sigs.sigValueChanged
        w.sigChanging = self.sigs.sigValueChanging
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


        # self.wlow.setOpts(bounds=limits)
        # self.whigh.setOpts(bounds=limits)

class FilelistParameter(Parameter):
    itemClass = FilelistItem

    def __init__(self, **opts):
    #    self.forward = OrderedDict()  # # name: value
    #    self.reverse = OrderedDict()  # # value: name#

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
