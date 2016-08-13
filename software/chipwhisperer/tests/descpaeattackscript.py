# Date Auto-Generated: 2016.06.09-16.47.02
import os
import shutil

import time

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
        if os.path.isfile("projects/tut_descpa.cwp"): os.remove("projects/tut_descpa.cwp")
        shutil.rmtree("projects/tut_descpa_data", ignore_errors=True)

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
        xmega.program(r"simpleserial-des.hex", memtype="flash", verify=True)
        xmega.close()

        # Setup the capture parameters
        lstexample = [
            ['CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
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
        self.api.saveProject("projects/tut_descpa.cwp")
        self.api.setParameter(['Simple Serial', 'Key Length (Bytes)', 8])
        self.api.setParameter(['Simple Serial', 'Input Length (Bytes)', 8])
        self.api.setParameter(['Simple Serial', 'Output Length (Bytes)', 8])
        self.api.setParameter(['Generic Settings', 'Basic', 'Key', 'Fixed'])
        self.api.setParameter(['Generic Settings', 'Basic', 'Fixed Encryption Key', u'2B 7E 15 16 28 AE D2 A6'])
        self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 100])
        time.sleep(1)
        self.api.captureM()
        self.api.saveProject()


class Attack(UserScriptBase):
    _name = "DES-CPA Attack Analyzer Script"

    def initPreprocessing(self):
        self.traces = self.api.project().traceManager()

    def initAnalysis(self):
        # Setup the CPA algorith
        self.attack = CPA()
        self.attack.setAnalysisAlgorithm(chipwhisperer.analyzer.attacks.cpa_algorithms.progressive.CPAProgressive,chipwhisperer.analyzer.attacks.models.DES.DES,chipwhisperer.analyzer.attacks.models.DES.DES.LEAK_HW_SBOXOUT_FIRSTROUND)
        self.attack.setTraceStart(0)
        self.attack.setTracesPerAttack(100)
        self.attack.setIterations(1)
        self.attack.setReportingInterval(10)
        self.attack.setTargetSubkeys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
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
        self.api.openProject("projects/tut_descpa.cwp")
        self.initPreprocessing()

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
    app = cwc.makeApplication(doDeleteLater=False)
    Parameter.usePyQtGraph = True
    api = CWCoreAPI()               # Instantiate the API

    gui = cwc.CWCaptureGUI(api)     # Instantiate the Capture GUI
    api.runScriptClass(Capture)
    gui.deleteLater()
    gui.reset()

    gui = cwa.CWAnalyzerGUI(api)    # Instantiate the Analyzer GUI
    api.runScriptClass(Attack)      # Run the script (default is the "run" method)
    app.exec_()