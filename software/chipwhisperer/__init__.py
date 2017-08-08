def capture_gui():
    """Open the CWCapture GUI, blocking until it's closed.

    Note that opening the GUI does not use any existing scope/target/project objects that were made using the API.

    Known issue: after finishing this function, many API calls don't work from the command line, as ChipWhisperer
    then relies on PyQT for timers and other utilities.
    """
    from chipwhisperer.capture.ui.CWCaptureGUI import main
    main()

def analyzer_gui():
    """Open the Analyzer GUI, blocking until it's closed.
    """
    from chipwhisperer.analyzer.ui.CWAnalyzerGUI import main
    main()

import os, os.path
from chipwhisperer.common.traces import TraceContainerNative as trace_container_native
from chipwhisperer.common.api import ProjectFormat as project

def open_project(filename):
    """Load an existing project from disk.

    Raise an IOError if no such project exists.
    """
    if not os.path.isfile(filename):
        raise IOError("File " + filename + " does not exist or is not a file")
    # TODO: open project here, knowing it exists
    # TODO: catch that annoying "ConfigError" if the file isn't a .cwp

def create_project(filename, overwrite=False):
    """Create a new project with the path <filename>.

    If <overwrite> is True, raise an IOError if this path already exists.
    """
    if os.path.isfile(filename) and (overwrite == False):
        raise IOError("File " + filename + " already exists")

    proj = project.ProjectFormat()
    proj.setFilename(filename)

    return proj

from chipwhisperer.capture.scopes.OpenADC import OpenADC as cwhardware

def scope(type = cwhardware):
    scope = type()
    scope.con()
    return scope

from chipwhisperer.common.utils.parameter import Parameter
import chipwhisperer.capture.ui.CWCaptureGUI as cwc
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.scopes.base import ScopeTemplate
from chipwhisperer.capture.targets._base import TargetTemplate

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
