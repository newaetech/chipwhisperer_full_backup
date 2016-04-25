# Date Auto-Generated: 2016.04.25-16.18.28
from chipwhisperer.common.scripts._base import UserScriptBase
# Imports from Preprocessing
import chipwhisperer.analyzer.preprocessing as preprocessing
# Imports from Capture
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.cpa_algorithms.CPAProgressive import CPAProgressive
import chipwhisperer.analyzer.attacks.models.AES128_8bit
from chipwhisperer.analyzer.attacks.cpa_algorithms.CPASimpleLoop import CPASimpleLoop
# Imports from utilList

class UserScript(UserScriptBase):
    name = "CPA with noise"
    description = "Simple example of attack script using CPA Progressive and random noise"
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
        self.api.resultWidgets["Trace Output Plot"].setObservedTraceSource(self.traces)
        self.api.resultWidgets["Trace Output Plot"].setObservedAttack(self.attack)
        self.api.resultWidgets["Correlation vs Traces in Attack"].setObservedTraceSource(self.traces)
        self.api.resultWidgets["Correlation vs Traces in Attack"].setObservedAttack(self.attack)
        self.api.resultWidgets["Output vs Point Plot"].setObservedTraceSource(self.traces)
        self.api.resultWidgets["Output vs Point Plot"].setObservedAttack(self.attack)
        self.api.resultWidgets["PGE vs Trace Plot"].setObservedTraceSource(self.traces)
        self.api.resultWidgets["PGE vs Trace Plot"].setObservedAttack(self.attack)
        self.api.resultWidgets["Results Table"].setObservedTraceSource(self.traces)
        self.api.resultWidgets["Results Table"].setObservedAttack(self.attack)

    def run(self, progressBar):
        self.attack.doAttack(progressBar)