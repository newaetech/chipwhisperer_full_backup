import numpy as np
class CorrVsTraceNoGUI(object):
    def __init__(self, stats):
        self._stats = stats
        pass
    
    def getPlotData(self, bnum):
        data = self._stats.maxes_list
        stats = self._stats
        maxdata = data[bnum]
        tlist = []
        for m in maxdata:
            tlist.append(m['trace'])
        maxlist = np.zeros((len(stats.diffs[bnum]), len(tlist)))
        for i, m in enumerate(maxdata):
            for j in range(0, len(stats.diffs[bnum])):
                maxlist[m['maxes'][j][0], i] = m['maxes'][j][2]
                
        return [tlist, maxlist]