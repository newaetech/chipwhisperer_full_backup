from chipwhisperer.analyzer.attacks.snr import calculate_snr
from chipwhisperer.analyzer.attacks import cpa_algorithms
from chipwhisperer.analyzer import preprocessing
from chipwhisperer.common.utils.util import camel_case_deprecated
from chipwhisperer.common.api.ProjectFormat import Project
from chipwhisperer.analyzer.utils import aes_funcs as aes_funcs
from chipwhisperer.analyzer.attacks.models import EightBitAES128LeakageModels
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AESLeakageHelper


leakage_models = EightBitAES128LeakageModels()


def cpa(proj, leak_model, algorithm=cpa_algorithms.Progressive):
    """Create a CPA object to attack traces in project using leak_model and algorithm.

    Args:
       proj (Project): TraceManager or preprocessing object that
           holds traces, plaintext, etc.
       leak_model (AESLeakageHelper): Model describing information
           leaked from target.
       algorithm (AlgorithmsBase, optional): Algorithm to perform CPA loop.
           Default is almost always the right choice. Defaults to
           cpa_algorithms.Progressive.

    Returns:
       A CPA object. For more information, see documentation for CPA.

    .. versionchanged:: 5.1
        Now uses new CPA attack object
    """
    from chipwhisperer.analyzer.attacks.cpa_new import CPA
    attack = CPA(proj, leak_model, algorithm)

    return attack


def profiling(trace_source):
    """Not yet implemented, create/import manually"""
    raise NotImplementedError


def analyzer_plots(attack_results=None):
    """Create an object to get plot data for analyzer results

    Args:
        attack_results(Results, optional): Results from attack to use
            to generate plots.

    Returns:
        Object to generate plots.
    """
    from chipwhisperer.common.results.noguiplots import NoGUIPlots
    return NoGUIPlots(attack_results)

analyzerPlots = analyzer_plots

# JUPYTER ONLY
# current_trace_iteration = 0
current_trace_iteration = 0
def _default_jupyter_callback(attack, head = 6, fmt="{:02X}<br>{:.3f}"):
    import pandas as pd # type: ignore
    from IPython.display import clear_output # type: ignore
    global current_trace_iteration
    attack_results = attack.results
    key = attack.known_key()

    def format_stat(stat):
        if (type(stat) is int) or (type(stat) is float):
            return str(stat)
        return str(fmt.format(stat[0], stat[2]))

    def color_corr_key(row):
        ret = [""] * 16
        key = attack.known_key()
        for i,bnum in enumerate(row):
            try:
                if (type(bnum) is int) or (type(bnum) is float):
                    continue
                if bnum[0] == key[i]:
                    ret[i] = "color: red"
                else:
                    ret[i] = ""
            except Exception as e:
                print("bnum: {}, key: {}".format(bnum, key))
        return ret
    attack_results.set_known_key(key)
    stat_data = attack_results.find_maximums()
    df = pd.DataFrame(stat_data).transpose()

    #Add PGE row
    df_pge = pd.DataFrame(attack_results.pge).transpose().rename(index={0:"PGE="}, columns=int)
    df = pd.concat([df_pge, df], ignore_index=False)

    clear_output(wait=True)
    reporting_interval = attack.reporting_interval
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




