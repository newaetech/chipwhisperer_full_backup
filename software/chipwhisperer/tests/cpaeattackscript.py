# Date Auto-Generated: 2016.06.09-16.47.02
import os
import shutil

from chipwhisperer.common.scripts.base import UserScriptBase
# Imports from Preprocessing
import chipwhisperer.analyzer.preprocessing as preprocessing
# Imports from Attack
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive
import chipwhisperer.analyzer.attacks.models.AES128_8bit
# Imports from utilList
from chipwhisperer.capture.api.programmers import XMEGAProgrammer


class Capture(UserScriptBase):
    _name = "CPA Attack Capture Script"

    def run(self):
        # Deletes previous saved data
        if os.path.isfile("projects/tut_cpa.cwp"): os.remove("projects/tut_cpa.cwp")
        shutil.rmtree("projects/tut_cpa_data", ignore_errors=True)

        # Setup the capture hardware
        self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        self.api.setParameter(['Generic Settings', 'Target Module', 'Simple Serial'])
        self.api.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])
        self.api.setParameter(['Simple Serial', 'Connection', 'NewAE USB (CWLite/CW1200)'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'NewAE USB (CWLite/CW1200)'])

        # Conect both: scope and target
        self.api.connect()

        # Flash the firmware
        xmega = XMEGAProgrammer()
        xmega.setUSBInterface(self.api.getScope().scopetype.dev.xmega)
        xmega._logging = None
        xmega.find()
        xmega.erase()
        xmega.program(r"simeplserial-aes-xmega.hex", memtype="flash", verify=True)
        xmega.close()

        # Setup the capture parameters
        lstexample = [['CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                      ['CW Extra Settings', 'Target IOn Pins', 'Target IO1', 'Serial RXD'],
                      ['CW Extra Settings', 'Target IOn Pins', 'Target IO2', 'Serial TXD'],
                      ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Desired Frequency', 7370000.0],
                      ['CW Extra Settings', 'Target HS IO-Out', 'CLKGEN'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'CLKGEN x4 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 3000],
                      ['OpenADC', 'Trigger Setup', 'Offset', 1250],
                      ['OpenADC', 'Gain Setting', 'Setting', 45],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]
        for cmd in lstexample: self.api.setParameter(cmd)

        # Capture a set of traces with fixed key and save the project
        self.api.newProject()
        self.api.saveProject("projects/tut_cpa.cwp")
        self.api.setParameter(['Generic Settings', 'Basic', 'Key', 'Fixed'])
        self.api.setParameter(['Generic Settings', 'Basic', 'Fixed Encryption Key', u'2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C'])
        self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 50])
        self.api.captureM()
        self.api.saveProject()


class Attack(UserScriptBase):
    _name = "CPA Attack Analyzer Script"

    def initPreprocessing(self):
        # Add amplitude noise and jitter to the original traces
        ppMod0 = preprocessing.AddNoiseJitter.AddNoiseJitter(self.api.project().traceManager())
        ppMod0.setEnabled(True)
        ppMod0.setMaxJitter(2)
        ppMod0.init()
        ppMod1 = preprocessing.AddNoiseRandom.AddNoiseRandom(ppMod0)
        ppMod1.setEnabled(False)
        ppMod1.setMaxNoise(0.005000)
        ppMod1.init()
        self.traces = ppMod1

    def initPreprocessing2(self):
        # Resync using SAD (to fix the jitter) and applies decimation (because we want to test this feature :)
        ppMod0 = preprocessing.ResyncSAD.ResyncSAD(self.api.project().traceManager())
        ppMod0.setEnabled(True)
        ppMod0.setReference(rtraceno=0, refpoints=(90,100), inputwindow=(70,120))
        ppMod0.init()
        ppMod1 = preprocessing.DecimationFixed.DecimationFixed(ppMod0)
        ppMod1.setEnabled(True)
        ppMod1.setDecimationFactor(2)
        ppMod1.init()
        self.traces = ppMod1

    def initAnalysis(self):
        # Setup the CPA algorith
        self.attack = CPA()
        self.attack.setAnalysisAlgorithm(CPAProgressive,chipwhisperer.analyzer.attacks.models.AES128_8bit,chipwhisperer.analyzer.attacks.models.AES128_8bit.LEAK_HW_SBOXOUT_FIRSTROUND)
        self.attack.setTraceStart(0)
        self.attack.setTracesPerAttack(50)
        self.attack.setIterations(1)
        self.attack.setReportingInterval(10)
        self.attack.setTargetBytes([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
        self.attack.setTraceSource(self.traces)
        self.attack.setPointRange((0, 2999))

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
        # This is what the API will execute
        self.api.openProject("projects/tut_cpa.cwp")
        self.initPreprocessing()

        # In order to increase test coverage, we will save the trace with noise to the trace manager
        self.api.getResults("Trace Recorder").setTraceSource(self.traces)
        self.api.setParameter(['Results', 'Trace Recorder', 'Trace Format', 'ChipWhisperer/Native'])
        self.api.setParameter(['Results', 'Trace Recorder', 'Trace Range', (0, 49)])
        self.api.setParameter(['Results', 'Trace Recorder', 'Point Range', (0, 2999)])
        self.api.setParameter(['Results', 'Trace Recorder', 'Save', None])

        # Deselect the original traces and select this new one
        self.api.project().traceManager().setTraceSegmentStatus(0, False)
        self.api.project().traceManager().setTraceSegmentStatus(1, True)
        self.api.saveProject()

        # Fix the traces
        self.initPreprocessing2()

        # Setup the analysis, widgets, and do the attack
        self.initAnalysis()
        self.initReporting()
        self.attack.processTraces()

        # Delete all pending scripts executions (that are observing the api be available again),
        # otherwise the current setup would be overridden
        self.api.executingScripts.disconnectAll()

if __name__ == '__main__':
    from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
    import chipwhisperer.capture.ui.CWCaptureGUI as cwc
    import chipwhisperer.analyzer.ui.CWAnalyzerGUI as cwa
    from chipwhisperer.common.utils.parameter import Parameter
    app = cwc.makeApplication()
    Parameter.usePyQtGraph = True
    api = CWCoreAPI()               # Instantiate the API

    gui = cwc.CWCaptureGUI(api)     # Instantiate the Capture GUI
    api.runScriptClass(Capture)
    gui.deleteLater()
    gui.reset()

    gui = cwa.CWAnalyzerGUI(api)    # Instantiate the Analyzer GUI
    api.runScriptClass(Attack)      # Run the script (default is the "run" method)
    app.exec_()