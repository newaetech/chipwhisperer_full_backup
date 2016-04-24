from datetime import *
from PySide.QtCore import *
from chipwhisperer.common.utils import pluginmanager
from chipwhisperer.analyzer.utils.scripteditor import MainScriptEditor
from functools import partial
from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter


class AttackScriptGen(pluginmanager.Parameterized):
    name = "Attack Script Generator"

    def __init__(self, parentParam, cwGUI):
        self.cwGUI = cwGUI
        super(AttackScriptGen, self).__init__(parentParam)
        self.scriptParamTree = pluginmanager.CWParameterTree("Script", [self])
        self.preprocessingParamTree = pluginmanager.CWParameterTree("Preprocessing")
        self.attackParamTree = pluginmanager.CWParameterTree("Attack")
        self.postprocessingParamTree = pluginmanager.CWParameterTree("Postprocessing")

    def setupParameters(self):
        self.scriptList = []
        self.scriptList.append({'widget':MainScriptEditor(self.cwGUI)})
        self.scriptList[0]['filename'] = self.scriptList[0]['widget'].filename
        self.scriptList[0]['dockname'] = 'Auto-Generated'
        self.defaultEditor = self.scriptList[0]
        autogen = (self.defaultEditor['dockname'], self.defaultEditor['filename'])
        self.preprocessingListGUI = [None, None, None, None]

        return [
                    {'name':'Attack Script', 'type':'group', 'children':[
                        {'name':'Filename', 'key':'attackfilelist', 'type':'filelist', 'values':{autogen:0}, 'value':0, 'editor':self.editorControl,},# , 'values':self.attackscripts, 'value':None
                        ]},

                    {'name':'Pre-Processing', 'type':'group', 'children':
                        [{'name':'Module #%d' % step, 'type':'list', 'values':self.preprocessingListGUI, 'value':self.preprocessingListGUI[step], 'set':partial(self.setPreprocessing, step)} for step in range(0, len(self.preprocessingListGUI))]},

                    {'name':'Attack', 'type':'group', 'children':[
                        {'name':'Module', 'type':'list', 'values':self.cwGUI.api.valid_attacks, 'value':self.cwGUI.api.valid_attacks["CPA"]},
                        ]},

                    {'name':'Post-Processing', 'type':'group'}
                ]

    def editorControl(self, filename, filedesc, default=False, bringToFront=True):
        """This is the call-back from the script editor file list, which opens editors"""

        # Find filename
        thisEditor = None
        for e in self.scriptList:
            if e['filename'] == filename:
                thisEditor = e
                break

        if thisEditor is None:
            thisEditor = {'widget':MainScriptEditor(parent=self, filename=filename)}
            thisEditor['filename'] = filename
            thisEditor['dockname'] = filedesc
            self.scriptList.append(thisEditor)

        # Update all docks if required
        thisEditor['dockname'] = filedesc
        self.editorDocks()

        if bringToFront:
            thisEditor['dock'].show()
            thisEditor['dock'].raise_()

        if default:
            # Set as default for attacks etc
            self.defaultEditor = thisEditor

    def editorDocks(self):
        """Ensure we have a script editor window for each referenced analyzer script file"""

        for script in self.scriptList:
            dockname = "Analysis Script: %s" % script['dockname']

            # No previous dock, do setup
            if 'dock' not in script.keys():
                script['widget'].editWindow.runFunction.connect(partial(self.runScriptFunction, filename=script['filename']))
                script['dock'] = self.cwGUI.addDock(script['widget'], name=dockname, area=Qt.BottomDockWidgetArea)

            # Dock present, check if name changed
            if script['dock'].windowTitle() != dockname:
                script['dock'].setWindowTitle(dockname)

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

    def reloadParamListPreprocessing(self, list=None):
        """Reload the parameter lists, ensuring GUI is showing correct options to user"""

        plist = []
        for p in self.preprocessingListGUI:
            if p:
                for item in p.paramList():
                    plist.append(item)
        ExtendedParameter.reloadParams(plist, self.preprocessingParamTree, help_window=self.cwGUI.api.helpWidget)

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

    def setupScriptModule(self, filename=None):
        """Loads a given script as a module for dynamic run-time insertion.

        Args:
            filename (str): The full filename to open. If None it opens the
                            auto-generated script instead.

        """

        if filename and filename != self.defaultEditor['filename']:
            raise Warning("Script Error: Cannot run script from non-default function")

        mod = self.defaultEditor['widget'].loadModule()

        # Check if we aborted due to conflitcing edit
        if mod is None:
            return None

        script = mod.UserScript(self.cwGUI.api.project())
        script.initPreprocessing()
        return script

    def reloadScripts(self):
        """Rewrite the auto-generated analyzer script, using settings from the GUI"""

        # Auto-Generated is always first
        mse = self.scriptList[0]['widget']

        mse.saveSliderPosition()
        mse.editWindow.clear()

        mse.append("# Date Auto-Generated: %s" % datetime.now().strftime('%Y.%m.%d-%H.%M.%S'), 0)

        mse.append("from chipwhisperer.common.api.autoscript import AutoScriptBase", 0)

        # Get imports from preprocessing
        mse.append("# Imports from Preprocessing", 0)
        mse.append("import chipwhisperer.analyzer.preprocessing as preprocessing", 0)
        for p in self.preprocessingListGUI:
            if p:
                imports = p.getImportStatements()
                for i in imports: mse.append(i, 0)

        # Get imports from api
        mse.append("# Imports from Capture", 0)
        if self.cwGUI.api.getAttack():
            for i in self.cwGUI.api.getAttack().getImportStatements():
                mse.append(i, 0)

        # Some other imports
        mse.append("# Imports from utilList", 0)
        for index, util in enumerate(self.cwGUI.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for i in util.getImportStatements(): mse.append(i, 0)

        mse.append("", 0)

        # Add main class
        mse.append("class UserScript(AutoScriptBase):", 0)

        mse.append("def initProject(self):", 1)
        mse.append("pass")

        mse.append("def initPreprocessing(self):", 1)

        # Get init from preprocessing
        lastOutput = "self.project().traceManager()"
        for i, p in enumerate(self.preprocessingListGUI):
            if p and p.getName() != "None":
                classname = type(p).__name__
                instname = "ppMod%d" % i
                mse.append("%s = preprocessing.%s.%s(%s)" % (instname, classname, classname, lastOutput))
                for s in p.getStatements('init'):
                    mse.append(s.replace("self.", instname + ".").replace("UserScript.", "self."))
                mse.append("%s.init()" % (instname))
                lastOutput = instname
        mse.append("self.ppOutput = %s" % lastOutput)

        # Get init from analysis
        mse.append("def initAnalysis(self):", 1)
        if self.cwGUI.api.getAttack():
            mse.append('self.attack = %s()' % type(self.cwGUI.api.getAttack()).__name__)
            for s in self.cwGUI.api.getAttack().getStatements('init'):
                mse.append(s.replace("self.", "self.attack.").replace("UserScript.", "self."))
        else:
            mse.append('pass')

        # Get init from reporting
        mse.append("def initReporting(self, results):", 1)
        mse.append("# Configures the attack observers (usually a set of GUI widgets)")
        # [mse.append("results[\"%s\"].setObservedAttack(self.attack)" % k) for k, _ in self.cwGUI.api.resultWidgets.iteritems()]
        mse.append("pass")

        # Do the attack
        mse.append("def doAnalysis(self, progressBar):", 1)
        mse.append("self.attack.doAttack(progressBar)")

        # Get other commands from attack module
        if self.cwGUI.api.getAttack():
            for k in self.cwGUI.api.getAttack()._smartstatements:
                if k == 'init' or k == 'go' or k == 'done':
                    pass
                else:
                    mse.append("def %s(self):" % k, 1)
                    for s in self.cwGUI.api.getAttack().getStatements(k):
                        mse.append(s.replace("self.", "self.api.getAttack().").replace("UserScript.", "self."))

        # Get other commands from other utilities
        for index, util in enumerate(self.cwGUI.utilList):
            if hasattr(util, '_smartstatements') and util.isVisible():
                for k in util._smartstatements:
                    util._smartstatements[k].addSelfReplacement("utilList[%d]." % index)
                    util._smartstatements[k].addSelfReplacement("parent.")
                    statements = util.getStatements(k)

                    if len(statements) > 0:
                        mse.append("def %s_%s(self):" % (util.__class__.__name__, k), 1)
                        for s in statements:
                            mse.append(s.replace("UserScript.", "self."))

        mse.restoreSliderPosition()
