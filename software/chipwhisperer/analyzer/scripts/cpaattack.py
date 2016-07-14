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
    _name = "CPA attack with noise"
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
        ppMod0 = preprocessing.add_noise_random.AddNoiseRandom(self.api.project().traceManager())
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
        self.api.getResults("Attack Settings").setAnalysisSource(self.attack)
        self.api.getResults("Correlation vs Traces in Attack").setAnalysisSource(self.attack)
        self.api.getResults("Output vs Point Plot").setAnalysisSource(self.attack)
        self.api.getResults("PGE vs Trace Plot").setAnalysisSource(self.attack)
        self.api.getResults("Results Table").setAnalysisSource(self.attack)
        self.api.getResults("Save to Files").setAnalysisSource(self.attack)
        self.api.getResults("Trace Output Plot").setTraceSource(self.traces)
        self.api.getResults("Trace Recorder").setTraceSource(self.traces)

    def run(self):
        self.attack.processTraces()

if __name__ == '__main__':
    import sys
    from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
    import chipwhisperer.analyzer.ui.CWAnalyzerGUI as cwa
    from chipwhisperer.common.utils.parameter import Parameter
    app = cwa.makeApplication()     # Comment if you don't need the GUI
    Parameter.usePyQtGraph = True   # Comment if you don't need the GUI
    api = CWCoreAPI()               # Instantiate the API
    gui = cwa.CWAnalyzerGUI(api)    # Comment if you don't need the GUI
    api.runScriptClass(UserScript)  # Run UserScript through the API

    sys.exit(app.exec_())           # Comment if you don't need the GUI
