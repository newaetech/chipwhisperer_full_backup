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
#=================================================

__author__ = "Colin O'Flynn"

from datetime import *
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
import chipwhisperer.analyzer.preprocessing.Preprocessing as Preprocessing
from chipwhisperer.analyzer.attacks.Profiling import Profiling
from functools import partial
from chipwhisperer.analyzer.attacks.CPA import CPA


class CWAnalizerAPI(CWCoreAPI):

    def __init__(self):
        super(CWAnalizerAPI, self).__init__()
        self.numPreprocessingStep = 4
        self.preprocessingListGUI = [None] * self.numPreprocessingStep

    def setupParameters(self, rootdir, showScriptParameter):
        valid_atacks = {'CPA':CPA(self, showScriptParameter=showScriptParameter),
         'Profiling':Profiling(self, showScriptParameter=showScriptParameter)}

        self.utilList = []
        self.scriptList = []


        self.cwParams = [
                {'name':'Attack Script', 'type':'group', 'children':[
                    # {'name':'Filename', 'key':'attackfilelist', 'type':'filelist', 'values':{autogen:0}, 'value':0, 'editor':self.editorControl,},# , 'values':self.attackscripts, 'value':None
                    ]},

                {'name':'Traces', 'type':'group', 'children':[
                    {'name':'Points', 'type':'int', 'value':0, 'readonly':True},
                    {'name':'Traces', 'type':'int', 'value':0, 'readonly':True}
                    ]},

                {'name':'Pre-Processing', 'type':'group', 'children':
                    [{'name':'Module #%d' % step, 'type':'list', 'value':0, 'values':Preprocessing.listAll(self), 'set':partial(self.setPreprocessing, step)} for step in range(0, self.numPreprocessingStep)]},

                {'name':'Attack', 'type':'group', 'children':[
                    {'name':'Module', 'type':'list', 'values':valid_atacks, 'value':'CPA', 'set':self.setAttack},
                    ]},

                {'name':'Post-Processing', 'type':'group'},
                ]

    def setPreprocessing(self, num, module):
        """Insert the preprocessing module selected from the GUI into the list of active modules.

        This ensures that the options for that module are then displayed in the GUI, along with
        writing the auto-generated script.
        """
        self.preprocessingListGUI[num] = module
        if module:
            module.paramListUpdated.connect(self.reloadParamListPreprocessing)
            module.scriptsUpdated.connect(self.reloadScripts)
        self.reloadParamListPreprocessing()
        self.reloadScripts()

    def setAttack(self, attack):
        """Set the attack module, reloading GUI and connecting appropriate signals"""

        self.attack = attack
        self.reloadAttackParamList()
        self.results.setAttack(self.attack)
        self.attack.paramListUpdated.connect(self.reloadAttackParamList)
        self.attack.setTraceLimits(self.traceLimits, self.pointLimits)

        # Sometimes required
        if hasattr(self, "traces") and self.traces:
            self.attack.setTraceManager(self.traces)

        self.attack.setProject(self.project())
        self.attack.scriptsUpdated.connect(self.reloadScripts)
        self.attack.runScriptFunction.connect(self.runScriptFunction)
        self.reloadScripts()

    def setupScriptModule(self, filename=None):
        """Loads a given script as a module for dynamic run-time insertion.

        Args:
            filename (str): The full filename to open. If None it opens the
                            auto-generated script instead.

        """

        if filename and filename != self.defaultEditor['filename']:
            raise Exception("Script Error: Cannot run script from non-default function")

        mod = self.defaultEditor['widget'].loadModule()

        # Check if we aborted due to conflitcing edit
        if mod is None:
            return None

        script = mod.userScript(self, self.showScriptParameter)
        if hasattr(self, "traces") and self.traces:
            script.setTraceManager(self.traces)
        return script

    def runFunc(self, name):
        # TODO: We should be doing this correctly, this hack is bad ;_;
        # name = "TraceExplorerDialog_PartitionDisplay_" + name
        self.runScriptFunction(name)

    def runScriptFunction(self, funcname, filename=None):
        """Loads a given script and runs a specific function within it."""

        mod = self.setupScriptModule(filename)

        if mod:
            try:
                eval('mod.%s()' % funcname)
            except AttributeError as e:
                # TODO fix this hack - this function will not exist before the
                # traceexplorer dialog has been opended, but will still be
                # called once
                if funcname == 'TraceExplorerDialog_PartitionDisplay_findPOI':
                    pass
                else:
                    # Continue with exception
                    raise

    def doAttack(self):
        """Called when the 'Do Attack' button is pressed, or can be called via API
        to cause attack to run"""

        #Check if traces enabled
        if self.traces.NumTrace == 0:
            return

        print "Loading..."
        mod = self.setupScriptModule()
        # mod.initProject()

        # Setup trace sources etc, this calls the
        # .initPreprocessing itself
        # it also resets the setTraces in the passed 'mod',
        # which is REQUIRED for proper functioning!
        self.setupPreprocessorChain(mod)

        mod.initAnalysis()
        mod.initReporting(self.results)
        mod.doAnalysis()
        mod.doneAnalysis()
        mod.doneReporting()

        # print "Attack Started"
        # if self.results:
        #    self.results.setTraceManager(self.traces)
        #
        # if self.attack:
        #    self.attack.setTraceManager(self.traces)
        #    self.attack.doAttack()

        # print "Attack Done"

    def setTraceLimits(self, traces=None, points=None, deftrace=1, defpoint=-1):
        """When traces is loaded, Tell everything default point/trace range"""

        if defpoint == -1:
            defpoint = points

        #Set parameters for attack
        self.traceLimits = traces
        self.pointLimits = points
        self.attack.setTraceLimits(traces, points)

        # Set local parameters for trace viewer
        if traces is not None:
            self.findParam('tracerng').setLimits((0, traces))
            # TODO: Bug in pyqtgraph maybe - if call with just deftrace &
            #setLimits was called with (0,0), the setValue(1) is ignored which is OK,
            #but then calling setLimits with higher value followed by setValue still
            #has no effect??
            #WORKAROUND: use min(traces,deftrace) to ensure don't set value beyond limit for now
            self.findParam('tracerng').setValue((0, min(traces, deftrace)))

        if points:
            self.findParam('pointrng').setLimits((0, points))
            self.findParam('pointrng').setValue((0, defpoint))

    def setupPreprocessorChain(self, mod=None):
        """Setup the preprocessor chain by chaining the first module input to the source
        traces, the next module input to the previous module output, etc."""

        if mod is None:
            mod = self.setupScriptModule()
        self.preprocessingList = mod.initPreprocessing()

        self.lastoutput = self.manageTraces.iface
        for t in self.preprocessingList:
            if t:
                t.setTraceSource(self.lastoutput)
                t.init()
                self.lastoutput = t
        self.traces = self.lastoutput

        for item in self.utilList:
            item.setTraceSource(self.traces)

        mod.setTraceManager(self.traces)

        # self.reloadScripts()

    def openProject(self, fname):
        super(CWAnalizerAPI, self).openProject(fname)
        # Ensure attack knows about this project
        self.attack.setProject(self.project())
        self.traceExplorerDialog.setProject(self.project())

    def reloadScripts(self):
        """Rewrite the auto-generated analyzer script, using settings from the GUI"""

        # Auto-Generated is always first
        mse = self.scriptList[0]['widget']

        mse.saveSliderPosition()
        mse.editWindow.clear()

        mse.append("# Date Auto-Generated: %s" % datetime.now().strftime('%Y.%m.%d-%H.%M.%S'), 0)

        mse.append("from chipwhisperer.common.autoscript import AutoScriptBase", 0)

        # Get imports from preprocessing
        mse.append("#Imports from Preprocessing", 0)
        mse.append("import chipwhisperer.analyzer.preprocessing as preprocessing", 0)
        for p in self.preprocessingListGUI:
            if p:
                imports = p.getImportStatements()
                for i in imports: mse.append(i, 0)

        # Get imports from capture
        mse.append("#Imports from Capture", 0)
        for i in self.attack.getImportStatements():
            mse.append(i, 0)

        # Some other imports
        mse.append("#Imports from utilList", 0)
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for i in util.getImportStatements(): mse.append(i, 0)

        mse.append("", 0)

        # Add main class
        mse.append("class userScript(AutoScriptBase):", 0)
        mse.append("preProcessingList = []", 1)

        mse.append("def initProject(self):", 1)
        mse.append("pass")


        mse.append("def initPreprocessing(self):", 1)

        # Get init from preprocessing
        instNames = ""
        for i, p in enumerate(self.preprocessingListGUI):
            if p:
                classname = type(p).__name__
                instname = "self.preProcessing%s%d" % (classname, i)
                mse.append("%s = preprocessing.%s.%s(self.parent)" % (instname, classname, classname))
                for s in p.getStatements('init'):
                    mse.append(s.replace("self.", instname + ".").replace("userScript.", "self."))
                instNames += instname + ","

        mse.append("self.preProcessingList = [%s]" % instNames)
        mse.append("return self.preProcessingList")


        mse.append("def initAnalysis(self):", 1)

        # Get init from analysis
        mse.append('self.attack = %s(self.parent, showScriptParameter=self.showScriptParameter)' % type(self.attack).__name__)
        for s in self.attack.getStatements('init'):
            mse.append(s.replace("self.", "self.attack.").replace("userScript.", "self."))

        mse.append('return self.attack')

        # Get init from reporting

        # Get go command from analysis
        mse.append("def initReporting(self, results):", 1)
        # mse.append("results.clear()")
        mse.append("results.setAttack(self.attack)")
        mse.append("results.setTraceManager(self.traceManager())")
        mse.append("self.results = results")

        mse.append("def doAnalysis(self):", 1)
        mse.append("self.attack.doAttack()")

        # Get other commands from attack module
        for k in self.attack._smartstatements:
            if k == 'init' or k == 'go' or k == 'done':
                pass
            else:
                mse.append("def %s(self):" % k, 1)
                for s in self.attack.getStatements(k):
                    mse.append(s.replace("self.", "self.attack.").replace("userScript.", "self."))


        # Get other commands from other utilities
        for index, util in enumerate(self.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for k in util._smartstatements:
                    util._smartstatements[k].addSelfReplacement("utilList[%d]." % index)
                    util._smartstatements[k].addSelfReplacement("parent.")
                    statements = util.getStatements(k)

                    if len(statements) > 0:
                        mse.append("def %s_%s(self):" % (util.__class__.__name__, k), 1)
                        for s in statements:
                            mse.append(s.replace("userScript.", "self."))

        mse.restoreSliderPosition()