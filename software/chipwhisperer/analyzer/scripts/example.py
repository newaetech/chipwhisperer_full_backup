# Date Auto-Generated: 2016.06.09-16.47.02
from chipwhisperer.common.scripts.base import UserScriptBase
# Imports from Preprocessing
import chipwhisperer.analyzer.preprocessing as preprocessing
# Imports from Capture
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive
import chipwhisperer.analyzer.attacks.models.AES128_8bit
# Imports from utilList

class UserScript(UserScriptBase):
    _name = "CPA with noise"
    _description = "Simple example of attack script using CPA Progressive and random noise"

    def __init__(self, api):
        UserScriptBase.__init__(self, api)
        self.initProject()
        self.initPreprocessing()
        self.initAnalysis()
        self.initReporting()

    def initProject(self):
        pass

    def initPreprocessing(self):
        ppMod0 = preprocessing.AddNoiseRandom.AddNoiseRandom(None, self.api.project().traceManager())
        ppMod0.setEnabled(True)
        ppMod0.setMaxNoise(0.005000)
        ppMod0.init()
        self.traces = ppMod0

    def initAnalysis(self):
        self.attack = CPA()
        self.attack.setAnalysisAlgorithm(CPAProgressive,chipwhisperer.analyzer.attacks.models.AES128_8bit,chipwhisperer.analyzer.attacks.models.AES128_8bit.LEAK_HW_SBOXOUT_FIRSTROUND)
        self.attack.setTraceStart(0)
        self.attack.setTracesPerAttack(50)
        self.attack.setIterations(1)
        self.attack.setReportingInterval(10)
        self.attack.setTargetBytes([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
        self.attack.setTraceSource(self.traces)
        self.attack.setPointRange((0,3000))

    def initReporting(self):
        # Configures the attack observers (usually a set of GUI widgets)
        self.api.resultWidgets["Correlation vs Traces in Attack"].setAnalysisSource(self.attack)
        self.api.resultWidgets["Knownkey Source"].setAnalysisSource(self.attack)
        self.api.resultWidgets["Output vs Point Plot"].setAnalysisSource(self.attack)
        self.api.resultWidgets["PGE vs Trace Plot"].setAnalysisSource(self.attack)
        self.api.resultWidgets["Results Table"].setAnalysisSource(self.attack)
        self.api.resultWidgets["Save to Files"].setAnalysisSource(self.attack)
        self.api.resultWidgets["Trace Output Plot"].setTraceSource(self.traces)

    def run(self):
        self.attack.processTraces()

if __name__ == '__main__':
    import sys
    from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
    import chipwhisperer.analyzer.ui.CWAnalyzerGUI as cwa
    from chipwhisperer.common.utils.parameter import Parameter
    app = cwa.makeApplication()                     # Comment this line if you don't want to use the GUI
    Parameter.usePyQtGraph = True                   # Comment this line if you don't want to use the GUI
    api = CWCoreAPI()                               # Instantiate the API
    # app.setApplicationName("Capture Scripted")    # If you DO NOT want to overwrite settings from the GUI
    gui = cwa.CWAnalyzerGUI(api)                     # Comment this line if you don't want to use the GUI
    gui.show()                                      # Comment this line if you don't want to use the GUI
    api.runScriptClass(UserScript)                  # Run the User Script

    sys.exit(app.exec_())                           # Comment this line if you don't want to use the GUI