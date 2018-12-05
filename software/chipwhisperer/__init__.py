from chipwhisperer.common.results.noguiplots import NoGUIPlots

import os, os.path
from chipwhisperer.common.api import ProjectFormat as project

#analyzer imports
from chipwhisperer.analyzer.attacks.cpa import CPA
from chipwhisperer.analyzer.attacks.profiling import Profiling
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive

from chipwhisperer.analyzer.attacks import cpa_algorithms
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit as AES128
from chipwhisperer.analyzer.attacks.models import AES128_8bit as AES128Leakage

from chipwhisperer.analyzer.attacks.snr import calculate_snr
import chipwhisperer.capture.scopes as scopes
import chipwhisperer.capture.targets as targets


import chipwhisperer.capture.acq_patterns as key_text_patterns
ktp = key_text_patterns #alias


# def cpa(trace_manager, leak_model, algorithm=cpa_algorithms.Progressive):
#     from chipwhisperer.analyzer.attacks.cpa import CPA
#     attack = CPA()
#     attack.setAnalysisAlgorithm(algorithm, leak_model)

def openProject(filename):
    """Load an existing project from disk.

    Raise an IOError if no such project exists.
    """
    if not os.path.isfile(filename):
        raise IOError("File " + filename + " does not exist or is not a file")
    proj = project.ProjectFormat()
    proj.load(filename)
    return proj

def createProject(filename, overwrite=False):
    """Create a new project with the path <filename>.

    If <overwrite> is False, raise an IOError if this path already exists.
    """
    if os.path.isfile(filename) and (overwrite == False):
        raise IOError("File " + filename + " already exists")

    proj = project.ProjectFormat()
    proj.setFilename(filename)

    return proj


def scope(type = scopes.OpenADC):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the scope
    is a ChipWhisperer OpenADC object, but this can be set to any valid scope
    class.
    """
    scope = type()
    scope.con()
    return scope

def target(scope, type = targets.SimpleSerial, **kwargs):
    """Create a target object and connect to it.

    This is SimpleSerial by default, but others are available in the targets subpackage
    """
    target = type()
    target.con(scope, **kwargs)
    return target

#probably not needed
def analyzerPlots(attack_results=None):
    """Create an object to get plot data for analyzer results
    """
    return NoGUIPlots(attack_results)

# JUPYTER ONLY
try:
    current_trace_iteration = 0
    import pandas as pd
    from IPython.display import clear_output
    def _defaultJupyterCallback(attack, head = 6):
        global current_trace_iteration
        attack_results = attack.getStatistics()
        key = attack.knownKey()

        def format_stat(stat):
            if type(stat) is int:
                return str(stat)
            return str("{:02X}<br>{:.3f}".format(stat[0], stat[2]))

        def color_corr_key(row):
            ret = [""] * 16
            key = attack.knownKey()
            for i,bnum in enumerate(row):
                if type(bnum) is int:
                    continue
                if bnum[0] == key[i]:
                    ret[i] = "color: red"
                else:
                    ret[i] = ""
            return ret
        attack_results.setKnownkey(key)
        stat_data = attack_results.findMaximums()
        df = pd.DataFrame(stat_data).transpose()

        #Add PGE row
        df_pge = pd.DataFrame(attack_results.pge).transpose().rename(index={0:"PGE="}, columns=int)
        df = pd.concat([df_pge, df], ignore_index=False)

        clear_output(wait=True)
        reporting_interval = attack.getReportingInterval()
        tstart = current_trace_iteration * reporting_interval
        tend = tstart + reporting_interval
        current_trace_iteration += 1
        display(df.head(head).style.format(format_stat).apply(color_corr_key, axis=1).set_caption("Finished traces {} to {}".format(tstart, tend)))

    def getJupyterCallback(attack, head = 6):
        """Get callback for use in Jupyter"""
        current_trace_iteration = 0
        return lambda : _defaultJupyterCallback(attack, head)
except ImportError:
    def getJupyterCallback(attack, head = 6):
        raise UserWarning("This function is only available in Jupyter with pandas installed")
        return None

