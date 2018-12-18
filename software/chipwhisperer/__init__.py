"""
.. module:: chipwhisperer
   :platform: Unix, Windows
   :synopsis: Test

.. moduleauthor:: NewAE

"""
import os, os.path

#analyzer imports

from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit as AES128
from chipwhisperer.analyzer.attacks.models import AES128_8bit as aes128leakage

from chipwhisperer.analyzer.attacks.snr import calculate_snr as calculateSNR
from chipwhisperer.capture import scopes, targets

from chipwhisperer.analyzer.attacks import cpa_algorithms
from chipwhisperer.analyzer import preprocessing

from chipwhisperer.capture.api import programmers


from chipwhisperer.capture import acq_patterns as key_text_patterns
ktp = key_text_patterns #alias

def programTarget(scope, type, fw_path):
    """Program the target using the programmer <type>

    Programmers can be found in the programmers submodule
    """
    prog = type()
    prog.scope = scope
    prog._logging = None
    prog.open()
    prog.find()
    prog.erase()
    prog.program(fw_path, memtype="flash", verify=True)
    prog.close()

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

    leak_model must be set either by this function or by attack.setLeakModel() before running attack.

    Args:

       trace_source (TraceManager): TraceManager or preprocessing object that holds traces, plaintext, etc.

       leak_model (AESLeakageHelper): Model describing information leaked from target. Must be set before running attack.

       algorithm (AlgorithmsBase): Algorithm to perform CPA loop. Default is almost always the right choice

    Returns:

       A CPA object. For more information, see documentation for CPA.
    """
    from chipwhisperer.analyzer.attacks.cpa import CPA
    attack = CPA()
    attack.setAnalysisAlgorithm(algorithm, leak_model)
    attack.setTraceSource(trace_source)
    attack.setTraceStart(0)
    attack.setTracesPerAttack(trace_source.numTraces())
    attack.setIterations(1)
    attack.setReportingInterval(10)
    attack.setTargetSubkeys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
    attack.setPointRange((0, -1))
    return attack

def profiling(trace_source):
    """Not yet implemented, create/import manually"""
    pass

def openProject(filename):
    """Load an existing project from disk.

    Raise an IOError if no such project exists.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if not os.path.isfile(filename):
        raise IOError("File " + filename + " does not exist or is not a file")
    proj = project.ProjectFormat()
    proj.load(filename)
    return proj

def createProject(filename, overwrite=False):
    """Create a new project with the path <filename>.

    If <overwrite> is False, raise an IOError if this path already exists.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if os.path.isfile(filename) and (overwrite == False):
        raise IOError("File " + filename + " already exists")

    proj = project.ProjectFormat()
    proj.setFilename(filename)

    return proj


def scope(type = scopes.OpenADC, sn=None):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the scope
    is a ChipWhisperer OpenADC object, but this can be set to any valid scope
    class.

    If multiple chipwhisperers are connected, the serial number of the one you
    want to connect to can be specified by passing sn=<SERIAL_NUMBER>
    """
    scope = type()
    scope.con(sn)
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
    from chipwhisperer.common.results.noguiplots import NoGUIPlots
    return NoGUIPlots(attack_results)

# JUPYTER ONLY
try:
    current_trace_iteration = 0
    import pandas as pd
    from IPython.display import clear_output
    def _defaultJupyterCallback(attack, head = 6, fmt="{:02X}<br>{:.3f}"):
        global current_trace_iteration
        attack_results = attack.getStatistics()
        key = attack.knownKey()

        def format_stat(stat):
            if type(stat) is int:
                return str(stat)
            return str(fmt.format(stat[0], stat[2]))

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

    def getJupyterCallback(attack, head = 6, fmt="{:02X}<br>{:.3f}"):
        """Get callback for use in Jupyter"""
        current_trace_iteration = 0
        return lambda : _defaultJupyterCallback(attack, head, fmt)
except ImportError:
    def getJupyterCallback(attack, head = 6):
        raise UserWarning("This function is only available in Jupyter with pandas installed")
        return None

