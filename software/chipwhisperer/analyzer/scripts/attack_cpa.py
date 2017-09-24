"""CPA attack script.

Assumes that a project with XMEGA software AES traces is already open.
"""

import chipwhisperer as cw
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit, SBox_output
from chipwhisperer.analyzer.preprocessing.add_noise_random import AddNoiseRandom

#self.project = cw.openProject("2017-mar23-xmega-aes.cwp")
traces = self.project.traceManager()

#Example: If you wanted to add noise, turn the .enabled to "True"
self.ppmod[0] = AddNoiseRandom()
self.ppmod[0].noise = 0.05
self.ppmod[0].enabled = False

attack = CPA()
leak_model = AES128_8bit(SBox_output)
attack.setAnalysisAlgorithm(CPAProgressive, leak_model)

attack.setTraceStart(0)
attack.setTracesPerAttack(50)
attack.setIterations(1)
attack.setReportingInterval(10)
attack.setTargetSubkeys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
attack.setTraceSource(self.ppmod[0])
attack.setPointRange((0, 3000))

self.results_table.setAnalysisSource(attack)
self.correlation_plot.setAnalysisSource(attack)
self.output_plot.setAnalysisSource(attack)
self.pge_plot.setAnalysisSource(attack)
attack.processTraces()


#        self.api.getResults("Attack Settings").setAnalysisSource(self.attack)
#        self.api.getResults("Correlation vs Traces in Attack").setAnalysisSource(self.attack)
#        self.api.getResults("Output vs Point Plot").setAnalysisSource(self.attack)
#        self.api.getResults("PGE vs Trace Plot").setAnalysisSource(self.attack)
#        self.api.getResults("Results Table").setAnalysisSource(self.attack)
#        self.api.getResults("Save to Files").setAnalysisSource(self.attack)
#        self.api.getResults("Trace Output Plot").setTraceSource(self.traces)
#        self.api.getResults("Trace Recorder").setTraceSource(self.traces)