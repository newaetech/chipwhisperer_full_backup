import chipwhisperer as cw
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit, SBox_output

#self.project = cw.openProject("2017-mar23-xmega-aes.cwp")
traces = self.project.traceManager()

attack = CPA()
leak_model = AES128_8bit(SBox_output)
attack.setAnalysisAlgorithm(CPAProgressive, leak_model)

attack.setTraceStart(0)
attack.setTracesPerAttack(50)
attack.setIterations(1)
attack.setReportingInterval(10)
attack.setTargetSubkeys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
attack.setTraceSource(traces)
attack.setPointRange((0, 3000))

self.results_table.setAnalysisSource(attack)
attack.processTraces()