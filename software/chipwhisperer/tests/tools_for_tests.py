"""Contains the tools needed for running tests easily
like adding a fake ui that can be used to emulate what the user would do when using the GUI
"""

import sys
import os

import chipwhisperer as cw
from chipwhisperer.common.utils.parameter import Parameter
import chipwhisperer.capture.ui.CWCaptureGUI as cwc
import chipwhisperer.analyzer.ui.CWAnalyzerGUI as cwa
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils.util import updateUI

# setup directory path needed for most tests
CHIPWHISPERER_DIR = os.path.join(os.path.abspath(cw.__file__), os.pardir, os.pardir, os.pardir)
CAPTURE_SCRIPTS_DIR = os.path.join(CHIPWHISPERER_DIR, "software", "chipwhisperer", "capture", "scripts")
ANALYZER_SCRIPTS_DIR = os.path.join(CHIPWHISPERER_DIR, "software", "chipwhisperer", "analyzer", "scripts")
PROJECTS_DIR = os.path.join(os.path.expanduser('~'), 'chipwhisperer', 'projects')
TESTS_DIR = os.path.join(PROJECTS_DIR, 'Test Logs')
FIRMWARE_DIR = os.path.join(CHIPWHISPERER_DIR, "hardware", "victims", "firmware")


class FakeUI():
    """This class is used to emulate the UI the user interfaces with

    It allows testing of the GUI as the user would interact with it.
    NOTE: This is not the correct way to use chipwhisperer without a GUI
    """

    def __init__(self):
        Parameter.usePyQtGraph = True
        self.api = CWCoreAPI()

    def create_capture(self):
        """Two instances of the GUI (analyzzer and capture) cannot exist at the same time
        therefore they both cannot be created in __init__ and need to be created seperately
        hence this method
        """
        self.capture = FakeCaptureSoftware(self.api)

    def close_capture(self):
        self.capture.gui.closeEvent = self.closeEvent
        self.capture.gui.close()
        self.capture.gui.deleteLater()
        self.capture.gui.reset()

    def create_analyzer(self):
        """Two instances of the GUI (analyzzer and capture) cannot exist at the same time
        therefore they both cannot be created in __init__ and need to be created seperately
        hence this method
        """
        self.analyzer = FakeAnalyzerSoftware(self.api)

    def close_analyzer(self):
        self.analyzer.gui.closeEvent = self.closeEvent
        self.analyzer.gui.close()
        self.analyzer.gui.deleteLater()
        self.analyzer.gui.reset()

    def closeEvent(self, event):
        """Needed to overwrite the event that gets called
        Prevents the do you want to save prompt
        """
        sys.excepthook = sys.__excepthook__  # Restore exception handlers
        event.accept()

    def updateUI(self):
        updateUI()


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


class FakeCaptureSoftware(object):

    def __init__(self, api):
        try:
            app = cwc.makeApplication(doDeleteLater=False)
        except RuntimeError as e:
            if 'QApplication instance already exists' in str(e):
                pass
            else:
                raise

        # Create and show the GUI
        self.gui = cwc.CWCaptureGUI(api)
        self.gui.show()

        self.gui.execute = ExecWithParentAsSelf(parent=self.gui)


class FakeAnalyzerSoftware(object):

    def __init__(self, api):
        try:
            self.analyzer_app = cwa.makeApplication(doDeleteLater=False)
        except RuntimeError as e:
            if 'QApplication instance already exists' in str(e):
                pass
            else:
                raise

        # Create and show the GUI
        self.gui = cwa.CWAnalyzerGUI(api)
        self.gui.show()

        self.gui.execute = ExecWithParentAsSelf(parent=self.gui)