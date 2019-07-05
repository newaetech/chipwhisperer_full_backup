from chipwhisperer.analyzer.attacks.snr import calculate_snr
from chipwhisperer.analyzer.attacks import cpa_algorithms
from chipwhisperer.analyzer import preprocessing
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit as AES128
from chipwhisperer.analyzer.attacks.models import AES128_8bit as aes128leakage
from chipwhisperer.common.utils.util import camel_case_deprecated
from chipwhisperer.common.api.ProjectFormat import Project

def cpa(trace_source, leak_model = None, algorithm=cpa_algorithms.Progressive):
    """Create a CPA attack object with sane defaults

    Create a CPA attack object with:

    * trace source = trace_source
    * traces start at beginning
    * uses all traces for attack
    * 1 iteration
    * Reporting Interval = 10
    * Attack all keys
    * Full point range

    leak_model must be set either by this function or by attack.setLeakModel()
    before running attack.

    Args:
       trace_source (TraceManager): TraceManager or preprocessing object that
           holds traces, plaintext, etc.
       leak_model (AESLeakageHelper, optional): Model describing information
           leaked from target. Must be set before running attack. Defaults to
           None.
       algorithm (AlgorithmsBase, optional): Algorithm to perform CPA loop.
           Default is almost always the right choice. Defaults to
           cpa_algorithms.Progressive.

    Returns:
       A CPA object. For more information, see documentation for CPA.
    """
    from chipwhisperer.analyzer.attacks.cpa import CPA
    attack = CPA()
    attack.set_analysis_algorithm(algorithm, leak_model)

    #until new analyzer is implemented
    if isinstance(trace_source, Project):
        trace_source = trace_source.trace_manager()

    attack.setTraceSource(trace_source)
    attack.set_trace_start(0)
    attack.set_traces_per_attack(trace_source.num_traces())
    attack.set_iterations(1)
    attack.set_reporting_interval(10)
    attack.set_target_subkeys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
    attack.set_point_range((0, -1))
    return attack

def profiling(trace_source):
    """Not yet implemented, create/import manually"""
    raise NotImplementedError

def analyzer_plots(attack_results=None):
    """Create an object to get plot data for analyzer results
    """
    from chipwhisperer.common.results.noguiplots import NoGUIPlots
    return NoGUIPlots(attack_results)

analyzerPlots = analyzer_plots

# JUPYTER ONLY
# current_trace_iteration = 0
current_trace_iteration = 0
def _default_jupyter_callback(attack, head = 6, fmt="{:02X}<br>{:.3f}"):
    import pandas as pd
    from IPython.display import clear_output
    global current_trace_iteration
    attack_results = attack.get_statistics()
    key = attack.known_key()

    def format_stat(stat):
        if type(stat) is int:
            return str(stat)
        return str(fmt.format(stat[0], stat[2]))

    def color_corr_key(row):
        ret = [""] * 16
        key = attack.known_key()
        for i,bnum in enumerate(row):
            if type(bnum) is int:
                continue
            if bnum[0] == key[i]:
                ret[i] = "color: red"
            else:
                ret[i] = ""
        return ret
    attack_results.setKnownkey(key)
    stat_data = attack_results.find_maximums()
    df = pd.DataFrame(stat_data).transpose()

    #Add PGE row
    df_pge = pd.DataFrame(attack_results.pge).transpose().rename(index={0:"PGE="}, columns=int)
    df = pd.concat([df_pge, df], ignore_index=False)

    clear_output(wait=True)
    reporting_interval = attack.get_reporting_interval()
    tstart = current_trace_iteration * reporting_interval
    tend = tstart + reporting_interval
    current_trace_iteration += 1
    display(df.head(head).style.format(format_stat).apply(color_corr_key, axis=1).set_caption("Finished traces {} to {}".format(tstart, tend)))

def get_jupyter_callback(attack, head = 6, fmt="{:02X}<br>{:.3f}"):
    """Get callback for use in Jupyter"""
    global current_trace_iteration
    current_trace_iteration = 0
    return lambda : _default_jupyter_callback(attack, head, fmt)
getJupyterCallback = camel_case_deprecated(get_jupyter_callback)

def reset_iteration():
    global current_trace_iteration
    current_trace_iteration = 0
