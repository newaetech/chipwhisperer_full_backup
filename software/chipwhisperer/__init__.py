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
from chipwhisperer.capture.scopes.cwhardware import ChipWhispererSAM3Update as CWFirmwareUpdate
ktp = key_text_patterns #alias

def programTarget(scope, prog_type, fw_path):
    """Program the target using the programmer <type>

    Programmers can be found in the programmers submodule

    Args:
       scope (ScopeTemplate): Connected scope object to use for programming
       prog_type (Programmer): Programmer to use. See chipwhisperer.programmers
           for available programmers
       fw_path (str): Path to hex file to program
    """
    if type is None: #makes automating notebooks much easier
        return
    prog = prog_type()
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
    raise NotImplementedError

def openProject(filename):
    """Load an existing project from disk.

    Raise an OSError if no such project exists.

    Args:
       filename (str): Path to project file.

    Returns:
       A chipwhisperer project object.

    Raises:
       OSError: filename does not exist.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if not os.path.isfile(filename):
        raise OSError("File " + filename + " does not exist or is not a file")
    proj = project.ProjectFormat()
    proj.load(filename)
    return proj

def createProject(filename, overwrite=False):
    """Create a new project with the path <filename>.

    If <overwrite> is False, raise an OSError if this path already exists.

    Args:
       filename (str): File path to create project file at. Must end with .cwp
       overwrite (bool, optional): Whether or not to overwrite an existing
           project with <filename>. Raises an OSError if path already exists
           and this is false. Defaults to false.

    Returns:
       A chipwhisperer project object.

    Raises:
       OSError: filename exists and overwrite is False.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if os.path.isfile(filename) and (overwrite == False):
        raise OSError("File " + filename + " already exists")

    proj = project.ProjectFormat()
    proj.setFilename(filename)

    return proj


def scope(scope_type = scopes.OpenADC, sn=None):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the scope
    is a ChipWhisperer OpenADC object, but this can be set to any valid scope
    class.

    If multiple chipwhisperers are connected, the serial number of the one you
    want to connect to can be specified by passing sn=<SERIAL_NUMBER>

    Args:
       scope_type (ScopeTemplate, optional): Scope type to connect to. Types
           can be found in chipwhisperer.scopes. Defaults to scopes.OpenADC
           (correct for CWLite and CWPro).
       sn (str, optional): Serial number of ChipWhisperer that you want to
           connect to. Required if more than one ChipWhisperer
           of the same type is connected (i.e. two CWNano's or a CWLite and
           CWPro). Defaults to None.

    Returns:
       Connected scope object specified by scope_type

    Raises:
       OSError: Can be raised for issues connecting to the chipwhisperer, such
           as not having permission to access the USB device or no ChipWhisperer
           being connected.
       Warning: Multiple ChipWhisperers connected, but serial number not
           specified
    """
    scope = scope_type()
    scope.con(sn)
    return scope

def target(scope, target_type = targets.SimpleSerial, **kwargs):
    """Create a target object and connect to it.

    Args:
       scope (ScopeTemplate): Scope object that we're connecting to the target
           through.
       target_type (TargetTemplate, optional): Target type to connect to.
           Defaults to targets.SimpleSerial. Types can be found in
           chipwhisperer.targets.
       **kwargs: Additional keyword arguments to pass to target setup. Rarely
           needed.

    Returns:
    """
    target = target_type()
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
        raise NotImplementedError("This function is only available in Jupyter with pandas installed")

