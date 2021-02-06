# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the 2-Clause BSD License. See LICENSE
# file which should have came with this code.

import types
from functools import partial
from pyqtgraph.Qt import QtGui, QtCore

#This class adds some  hacks that allow us to have 'get', 'set', and 'linked' methods in the Parameter specification.
#They are especially helpful for the work done here
    
from pyqtgraph.parametertree.parameterTypes import *

class WidgetParameterItemHelp(WidgetParameterItem):
    def __init__(self, *args, **kwargs):
        super(WidgetParameterItemHelp, self).__init__(*args, **kwargs)
        ExtendedParameter.drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass

class ListParameterItemHelp(ListParameterItem):
    def __init__(self, *args, **kwargs):
        super(ListParameterItemHelp, self).__init__(*args, **kwargs)
        ExtendedParameter.drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass

class ActionParameterItemHelp(ActionParameterItem):
    def __init__(self, *args, **kwargs):
        super(ActionParameterItemHelp, self).__init__(*args, **kwargs)
        ExtendedParameter.drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass

class TextParameterItemHelp(TextParameterItem):
    def __init__(self, *args, **kwargs):
        super(TextParameterItemHelp, self).__init__(*args, **kwargs)
        ExtendedParameter.drawHelpIcon(self)

    def updateDefaultBtn(self):
        pass

classmapping = {
 "<class 'pyqtgraph.parametertree.parameterTypes.WidgetParameterItem'>":WidgetParameterItemHelp,
 "<class 'pyqtgraph.parametertree.parameterTypes.ListParameterItem'>":ListParameterItemHelp,
 "<class 'pyqtgraph.parametertree.parameterTypes.ActionParameterItem'>":ActionParameterItemHelp,
  "<class 'pyqtgraph.parametertree.parameterTypes.TextParameterItem'>":TextParameterItemHelp
 }

    
class ExtendedParameter():
    @staticmethod
    def getAllParameters(self, parent=None):
        """Causes the 'get' function for all parameters to be called (if present)."""
        if parent is None:
            parent = self
                              
        if parent.hasChildren():
            for child in parent.children():
                self.getAllParameters(child)
        else:
            if 'get' in parent.opts:
                try:
                    parent.setValue(parent.opts['get']())
                except TypeError:
                    parent.hide()

    @staticmethod
    def findHelpWindowMethod(curParam):
        if hasattr(curParam, 'param'):
            if hasattr(curParam.param, 'opts'):
                if 'helpwnd' in curParam.param.opts:
                    return curParam.param.opts['helpwnd']

        if hasattr(curParam, 'parent'):
            return ExtendedParameter.findHelpWindowMethod(curParam.parent())

        return None

    @staticmethod
    def showHelpWindow(curParam):
        """
        Helper to show the help text. If class defines a 'helpwnd' it uses that function, otherwise
        a simple message box is called upon.
        """

        nametext = curParam.param.opts['name']
        helptext = curParam.param.opts['help']
        hdrtext = '-' * len(nametext)

        helptext = helptext.replace('%namehdr%', '%s\n%s\n\n' % (nametext, hdrtext))

        helpwnd = ExtendedParameter.findHelpWindowMethod(curParam)

        if helpwnd:
            helpwnd(helptext, curParam)
        else:
            #Shitty default window
            if hasattr(curParam, 'widget'):
                wdgt = curParam.widget
            else:
                wdgt = None
            QtGui.QMessageBox.information(wdgt, 'Help: %s' % nametext, helptext, QtGui.QMessageBox.Cancel, QtGui.QMessageBox.Cancel)
       
    @staticmethod
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
            buthelp.clicked[bool].connect(lambda ignored: ExtendedParameter.showHelpWindow(curParam))
            layout.addWidget(buthelp)

    @staticmethod
    def addHelpIcons(curParam):
        """Iterate through parameter tree, and add help icon to each one. Replaces existing class with overloaded class which will generate
           a help icon when the widget is made."""
        if len(curParam.children()) > 0:
            for child in curParam.children():
                ExtendedParameter.addHelpIcons(child)
        else:
            if str(curParam.itemClass) in classmapping.keys():
                curParam.itemClass = classmapping[str(curParam.itemClass)]

    @staticmethod
    def setupExtended(curParam, parent=None):
        """Setup the 'extended' parameters, which is the name given to the OpenADC extensions."""
        curParam.getAllParameters = types.MethodType(ExtendedParameter.getAllParameters, curParam)
        curParam.sigTreeStateChanged.connect(ExtendedParameter.change)
        
        ExtendedParameter.addHelpIcons(curParam)
        
        if parent is not None:
            parent.paramTreeChanged = types.MethodType(ExtendedParameter.paramTreeChanged, parent)
            curParam.sigTreeStateChanged.connect(parent.paramTreeChanged)
            parent.findParam = types.MethodType(ExtendedParameter.findParam, parent)
            
            if not hasattr(parent, 'showScriptParameter'):
                parent.showScriptParameter = None
                         
    ## If anything changes in the tree, print a message
    @staticmethod
    def change(param,  changes):  
        """Monitor changes in the tree"""
        for param, change, data in changes:

            # print change

            # Only trigger on 'value' changes!
            if (change != 'value') and (change != 'activated') and (change != 'limits'):
                return
                                    
            #Call specific 'set' routine associated with data
            if 'set' in param.opts:
                # QtCore.QTimer.singleShot(0, partial(param.opts['set'], data))
                param.opts['set'](data)
                
            if 'linked' in param.opts:
                par = param.parent()
                for link in param.opts['linked']:
                    linked = par
                    if isinstance(link, tuple):
                        for p in link:
                            linked = linked.names[p]                            
                    else:                        
                        linked = par.names[link]
                        
                    #QtCore.QTimer.singleShot(0, partial(linked.setValue, linked.opts['get']()))
                    linked.setValue(linked.opts['get']()) 
                    
            if 'action' in param.opts:                
                #QtCore.QTimer.singleShot(0, param.opts['action'])
                param.opts['action']()
    
    @staticmethod
    def paramTreeChanged(self, param, changes):        
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)
      
    @staticmethod  
    def reloadParams(lst, paramtree, help_window=None):
            """
            Reloads parameters in a paramtree. Hides anything that isn't in the list anymore, adds
            any new parameters. Required to avoid deleting parameters that we actually want to see
            again later, otherwise they are garbage collected.
            """
            
            for i in range(0, paramtree.invisibleRootItem().childCount()):
                refitem = paramtree.invisibleRootItem().child(i)
                if refitem.param in lst:
                    lst.remove(refitem.param)
                    refitem.setHidden(False)
                else:
                    refitem.setHidden(True)                                  
                    
            for p in lst:
                if help_window:
                    for c in p.children():
                        if hasattr(c, 'children'):
                            for k in c.children():
                                # if hasattr(k, 'param'):
                                #    print k.param()
                                if hasattr(k, 'opts'):
                                    k.opts['helpwnd'] = help_window

                paramtree.addParameters(p)
                
      
    @staticmethod
    def findParam(self=None, paramKey=None, params=None):
        """Match parameter based on 'key'"""
        if params is None:
            params = self.params
            
        try:
            if params.opts['key'] == paramKey:
                return params
        except KeyError:
            pass
        
        for t in params.children():
            p = self.findParam(paramKey=paramKey, params=t)
            if p is not None:
                return p
             
        return None
                
if __name__ == '__main__':
    import pyqtgraph as pg
    from pyqtgraph.Qt import QtCore, QtGui

    app = QtGui.QApplication([])
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    
    class submodule(QtCore.QObject):
        paramListUpdated = QtCore.Signal(list)
        
        def __init__(self):
            super(submodule, self).__init__()
            moreparams = [                  
                      {'name':'Value', 'type':'list', 'values':['2','3','4'], 'set':self.set}
                  ]           
            self.params = Parameter.create(name='Test', type='group', children=moreparams)
            ExtendedParameter.setupExtended(self.params)
            
        def set(self, value):
            print "set %s"%value
            #self.paramListUpdated.emit(self.paramList())
            
        def paramList(self):
            p = [self.params]                
            return p

    
    class module(QtCore.QObject):
        paramListUpdated = QtCore.Signal(list)
        
        def __init__(self):
            super(module, self).__init__()
            moreparams = [                  
                      {'name':'SubModule', 'type':'list', 'values':{'sm1':submodule(), 'sm2':submodule(), 'sm3':submodule()}, 'set':self.setSubmodule}
                  ]           
            self.params = Parameter.create(name='Test', type='group', children=moreparams)
            ExtendedParameter.setupExtended(self.params)
            self.sm = None
            
        def paramList(self):
            p = [self.params]        
            if self.sm is not None:
                for a in self.sm.paramList(): p.append(a)            
            return p
            
        def setSubmodule(self, sm):
            self.sm = sm  
            self.paramListUpdated.emit(self.paramList())
    
    class maintest(QtCore.QObject):
        paramListUpdated = QtCore.Signal(list)
        
        def __init__(self):
            super(maintest, self).__init__()
            p = [
                        {'name': 'Basic parameter data types', 'type': 'group', 'helpwnd':self.printhelp, 'children': [
                            {'name': 'Module 1', 'type': 'list', 'values':{'module 1':module(), 'module 2':module(), 'module 3':module()},
                              'set':self.setmodule, 'help':'%namehdr%Boatload of text is possible here. Can use markup too with external help window.'},
                            {'name':'Rocks to Skips', 'type':'int', 'help':'Another help example', 'helpwnd':None}
                    ]}]
            self.params = Parameter.create(name='Test', type='group', children=p)
            ExtendedParameter.setupExtended(self.params)
            self.module = None
            
            self.t = ParameterTree()    
            self.reloadParams()        
            
        def printhelp(self, msg, obj):
            print msg

        def setmodule(self, module):
            print "Changing Module"
            self.module = module            
            self.module.paramListUpdated.connect(self.reloadParams)
            self.paramListUpdated.emit(self.paramList())
            self.reloadParams()
            
        def reloadParams(self):
            ExtendedParameter.reloadParams(self.paramList(), self.t)
            
        def reloadParamsBad(self):
            """The following is kept as a reminder you must NOT do this, or else objects get deleted"""
            
            #Notes about my wasted day to report a bug later when I have more time:
            # *Calling the .clear() causes stuff to be deleted
            # *In file parameterTypes.py at line 180 you have the following:
            #    self.widget.sigChanged.disconnect(self.widgetValueChanged)            
            #  If self.widget has been deleted, which will happen once the ParameterTree is cleared, the call
            #  to .disconnect causes a bad crash (sometimes). However - you can insert a call to 'self.widget.value()'
            #  before the .disconnect() call. The self.widget.value() actually checks if the object was deleted, and if so
            #  it raises a nice exception with a handy error message.
            
            lst = self.paramList()
            self.t.clear()            
            for p in lst:
                self.t.addParameters(p)            
            
        def paramList(self):
            p = [self.params]        
            if self.module is not None:
                for a in self.module.paramList(): p.append(a)            
            return p        

    m = maintest()
    
    t = m.t
    t.show()
    t.setWindowTitle('pyqtgraph example: Parameter Tree')
    t.resize(400,800)

    QtGui.QApplication.instance().exec_()
                    
