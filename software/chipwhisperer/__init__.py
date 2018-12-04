from chipwhisperer.capture.api.acquisition_controller import AcquisitionController
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic
from chipwhisperer.common.utils.util import updateUI
from chipwhisperer.common.results.noguiplots import NoGUIPlots

import os, os.path
from chipwhisperer.common.traces import TraceContainerNative as trace_container_native
from chipwhisperer.common.api import ProjectFormat as project

from chipwhisperer.capture.scopes.OpenADC import OpenADC as cwhardware
from chipwhisperer.capture.targets.SimpleSerial import SimpleSerial as cwtarget
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic as BasicKtp
from chipwhisperer.common.utils.util import cw_bytearray

from chipwhisperer.common.utils.parameter import Parameter
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.scopes.base import ScopeTemplate
from chipwhisperer.capture.targets._base import TargetTemplate

#analyzer imports
from chipwhisperer.analyzer.attacks.cpa import CPA as cpa
from chipwhisperer.analyzer.attacks.cpa_algorithms.progressive import CPAProgressive as cpaProgressive
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit as aes128
from chipwhisperer.analyzer.attacks.models import AES128_8bit as aes128Leakage

from functools import wraps

def gui_only(func):
    """Decorator for the functions that can only be used in the gui
    If it is called outside the gui, it will raise UserWarning
    """
    @wraps(func)
    def wrapper(*args, **kwargs):
        gui_warning = "This api function is for use inside GUI Python Console"
        try:
            api = CWCoreAPI.getInstance()
            if api is None:
                raise Exception
        except:
            raise UserWarning(gui_warning)
        return func(api, *args, **kwargs)
    return wrapper

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


def scope(type = cwhardware):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the scope
    is a ChipWhisperer OpenADC object, but this can be set to any valid scope
    class.
    """
    scope = type()
    scope.con()
    return scope

def target(scope, type = cwtarget, **kwargs):
    """Create a target object and connect to it.
    """
    target = type()
    target.con(scope, **kwargs)
    return target


def analyzerPlots(attack_results=None):
    """Create an object to get plot data for analyzer results
    """
    return NoGUIPlots(attack_results)

# JUPYTER ONLY
try:
    current_trace_iteration = 0
    import pandas as pd
    from IPython.display import clear_output
    def defaultJupyterCallback(attack, head = 6):
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
        current_trace_iteration = 0
        return lambda : defaultJupyterCallback(attack, head)
except ImportError:
    def getJupyterCallback(attack, head = 6):
        raise UserWarning("This function is only available in Jupyter with pandas installed")
        return None

@gui_only
def auxList(api):
    # TODO: this should create a fresh aux list
    # We can already access API one via self.aux_list
    return api.getAuxList()

@gui_only
def captureN(api, scope=None, target=None, project=None, aux_list=None, ktp=None, N=1, seg_size=None):
    """Capture a number of traces, saving power traces and input/output text
    and keys to disk along the way.

    Args:
        scope: A connected scope object. If None, no power trace will be
            recorded - possibly helpful for testing target setups
        target: A connected target object. If None, no target commmands will be
            sent - assumed that aux commands or external boards are controlling
            target
        project: A ChipWhisperer project object. If None, no data will be
            saved - helpful when testing scope settings without saving
        aux_list: An AuxList object with auxiliary functions registered. If
            None, no auxiliary functions are run
        ktp: A key/text input object. Produces pairs of encryption key/input
            text for each capture. Can't be None as these values are required
        N: The number of traces to capture.
        seg_size: The number of traces to record in each segment. The data is
            saved to disk in a number of segments to avoid making one enormous
            data file. If None, a sane default is used.

    To emulate GUI capture:
    >>> cw.captureN(self.scope, self.target, self.project, self.aux_list, self.ktp, 50)
    """
    api.captureM(scope=scope, target=target, project=project, aux_list=aux_list, ktp=ktp, N=N, seg_size=seg_size)

@gui_only
def getLastKey(core_api):
    """Return the last key used in captureN
    """
    key = core_api.getLastKey()
    if key is None:
        return key
    else:
        return cw_bytearray(key)

@gui_only
def getLastTextin(core_api):
    """Return the last input text used in captureN
    """
    lti = core_api.getLastTextin()
    if lti is None:
        return lti
    else:
        return cw_bytearray(lti)

@gui_only
def getLastTextout(core_api):
    """Return the last input text used in captureN
    """
    lto = core_api.getLastTextout()
    if lto is None:
        return lto
    else:
        return cw_bytearray(lto)

@gui_only
def getLastExpected(core_api):
    """Return the last input text used in captureN
    """
    le = core_api.getLastExpected()

    if le is None:
        return le
    else:
        return cw_bytearray(le)


# TODO: decide whether to remove all of this
class gui(object):
    def __init__(self):
        Parameter.usePyQtGraph = True
        self.api = CWCoreAPI()

    def register_scope(self, obj):
        self.api.setScope(obj, addToList=True, blockSignal=True)


    def register_target(self, obj):
        self.api.setTarget(obj, addToList=True, blockSignal=True)

    def register(self, obj):
        #Figure out what we're registering here
        if isinstance(obj, ScopeTemplate):
            self.register_scope(obj)
        elif isinstance(obj, TargetTemplate):
            self.register_target(obj)
        else:
            raise ValueError("Unknown object type %s"%str(obj))

    def capture(self):
        self.app = cwc.makeApplication("Capture")
        self.window = cwc.CWCaptureGUI(self.api)
        self.window.show()
        # Run the main Qt loop
        self.app.exec_()


def acquisition_controller(scope, target=None, writer=None, aux=None, key_text_pattern=None):
    """Not required when running GUI"""
    ac = AcquisitionController(scope=scope, target=target, keyTextPattern=key_text_pattern)
    return ac
