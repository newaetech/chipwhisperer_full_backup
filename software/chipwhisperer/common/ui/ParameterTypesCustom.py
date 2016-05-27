#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# This file based on PyQtGraph parameterTypes.py
#=================================================

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
from pyqtgraph.parametertree.Parameter import registerParameterType
from pyqtgraph.parametertree.ParameterItem import ParameterItem
from pyqtgraph.parametertree.parameterTypes import WidgetParameterItem, EventProxy, ListParameterItem, Parameter, ActionParameterItem, TextParameterItem, GroupParameterItem
from pyqtgraph.widgets.SpinBox import SpinBox

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

def drawHelpIcon(curParam):
    """Add a single help icons to a Parameter Item. Also removes the "default" button which isn't really used in our applications"""
    layout = curParam.layoutWidget.layout()

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
        buthelp.setIcon(curParam.layoutWidget.style().standardIcon(QtGui.QStyle.SP_TitleBarContextHelpButton))
        buthelp.clicked[bool].connect(lambda ignored: showHelpWindow(curParam))
        layout.addWidget(buthelp)


# This class adds some  hacks that allow us to have 'get', 'set', and 'linked' methods in the Parameter specification.
# They are especially helpful for the work done here

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
        fname, _ = QtGui.QFileDialog.getOpenFileName(None, 'Get file path', QtCore.QSettings().value(self.param.opts["name"]), self.param.opts["filter"])
        if fname:
            self.param.setValue(fname)
            QtCore.QSettings().setValue(self.param.opts["name"], fname)


class FileParameter(Parameter):
    itemClass = FileParameterItemHelp

registerParameterType('file', FileParameter, override=True)

classmapping = {
    "<class 'pyqtgraph.parametertree.parameterTypes.WidgetParameterItem'>": WidgetParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.ListParameterItem'>": ListParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.ActionParameterItem'>": ActionParameterItemHelp,
    "<class 'pyqtgraph.parametertree.parameterTypes.TextParameterItem'>": TextParameterItemHelp
}


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
                'siPrefix': False, 'suffix': '', 'visible':True,
            }
        defs.update(opts)

        wlow = SpinBox()
        wlow.setMaximumWidth(80)
        wlow.setAlignment(QtCore.Qt.AlignRight)
        whigh = SpinBox()
        whigh.setMaximumWidth(80)
        whigh.setAlignment(QtCore.Qt.AlignRight)

        # The following required for pyqtgraph > 0.9.10
        for k in defs:
            wlow.opts[k] = defs[k]
        for k in defs:
            whigh.opts[k] = defs[k]

        if 'limits' in opts:
            defs['bounds'] = opts['limits']

        wlow.setOpts(**defs)
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

        graphIcon = QtGui.QIcon()
        graphIcon.addFile(':/images/wavelimits.png', state=QtGui.QIcon.On)
        graphIcon.addFile(':/images/wavelimitsoff.png', state=QtGui.QIcon.Off)

        self.graphBtn = QtGui.QPushButton()
        self.graphBtn.setToolTip("Allows range selection by draging the interval boundaries on the graph")
        self.graphBtn.setFixedWidth(20)
        self.graphBtn.setFixedHeight(20)
        self.graphBtn.setIcon(graphIcon)
        self.graphBtn.setCheckable(True)
        self.graphBtn.clicked[bool].connect(self.buttonPressed)

        l = self.makeLayout()
        l.addWidget(self.graphBtn)
        l.addWidget(QtGui.QLabel("  "))

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
        self.sigs = SigStuff()
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

    def makeWidget(self):
        """Copy of SpinBox from PyQtGraph 0.9.10 & later, which adds special parameters we hack on"""
        opts = self.param.opts
        defs = {
            'value': 0, 'min': None, 'max': None, 'int': True,
            'step': 1.0, 'minStep': 1.0, 'dec': False,
            'siPrefix': False, 'suffix': ''
        }

        defs.update(opts)
        if 'limits' in opts:
            defs['bounds'] = opts['limits']
        w = SpinBox()
        w.destroyed.connect(lambda: self.param.sigValueChanged.disconnect(self.valueChanged))
        # This hack ensures compatibility between 0.9.10 and later
        for k in opts:
            w.opts[k] = opts[k]

        w.setOpts(**defs)
        w.sigChanged = w.sigValueChanged
        w.sigChanging = w.sigValueChanging

        return w

    def updateDefaultBtn(self):
        pass

 #   def delayedChange(self):
 #       #Modified to always emit change
 #       try:
 #           #if self.val != self.lastValEmitted:
 #           self.emitChanged()
 #       except RuntimeError:
 #           pass  ## This can happen if we try to handle a delayed signal after someone else has already deleted the underlying C++ object.

class SpinBoxWithSet(Parameter):
    itemClass = SpinBoxWithSetItem

registerParameterType('int', SpinBoxWithSet, override=True)


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
#      Useful when hardware reality is != software settings, and need to re-download
#      things.
def setValue_Fix(self, value, blockSignal=None):
    # Fixes the CW requirement to not skip the setValue call when the previous value is the same
    try:
        if blockSignal is not None:
            self.sigValueChanged.disconnect(blockSignal)
        self.opts['value'] = value
        self.sigValueChanged.emit(self, value)
    finally:
        if blockSignal is not None:
            self.sigValueChanged.connect(blockSignal)

Parameter.setValue = setValue_Fix


#Call base class to hide/show group-type parameter item
def optsChanged_Fix(self, param, opts):
    ParameterItem.optsChanged(self, param, opts)
    if 'addList' in opts:
        self.updateAddList()

GroupParameterItem.optsChanged = optsChanged_Fix
