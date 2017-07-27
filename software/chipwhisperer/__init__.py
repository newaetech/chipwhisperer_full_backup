def capture_gui():
    from chipwhisperer.capture.ui.CWCaptureGUI import main
    main()

def analyzer_gui():
    from chipwhisperer.analyzer.ui.CWAnalyzerGUI import main
    main()

import os, os.path
from chipwhisperer.common.traces import TraceContainerNative as trace_container_native
from chipwhisperer.common.api import ProjectFormat as project

def open_project(filename):
    if not os.path.isfile(filename):
        raise IOError("File " + filename + " does not exist or is not a file")

def create_project(filename, overwrite=False):
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
