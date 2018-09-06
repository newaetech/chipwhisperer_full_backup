"""Contains the tools needed for running tests easily
like adding a fake ui that can be used to emulate what the user would do when using the GUI
"""

import sys
import os

import chipwhisperer as cw

# setup directory path needed for most tests
CHIPWHISPERER_DIR = os.path.abspath(os.path.join(os.path.dirname(cw.__file__), os.pardir, os.pardir))
CAPTURE_SCRIPTS_DIR = os.path.join(CHIPWHISPERER_DIR, "software", "chipwhisperer", "capture", "scripts")
ANALYZER_SCRIPTS_DIR = os.path.join(CHIPWHISPERER_DIR, "software", "chipwhisperer", "analyzer", "scripts")
PROJECTS_DIR = os.path.join(os.path.expanduser('~'), 'chipwhisperer', 'projects')
TESTS_DIR = os.path.join(PROJECTS_DIR, 'Test Logs')
FIRMWARE_DIR = os.path.join(CHIPWHISPERER_DIR, "hardware", "victims", "firmware")
SCRIPTING_EXAMPLES_DIR = os.path.join(CHIPWHISPERER_DIR, "software", "scripting-examples")


class ExecWithParentAsSelf(object):
    def __init__(self, parent=None, ):
        self.new_self = parent

    def __call__(self, script_path=None, string=None):
        temp_locals = locals()
        temp_locals['self'] = self.new_self
        if string is not None:
            exec(string, temp_locals)

        if script_path is not None:
            execfile(script_path, temp_locals)
