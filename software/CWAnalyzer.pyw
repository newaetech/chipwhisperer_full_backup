# Calls ChipWhisperer w/o the annoying window
import ctypes
import os
from chipwhisperer.analyzer.ui.CWAnalyzerGUI import main
import cProfile
import sys

class NullWriter(object):
    def write(self, value): pass

if __name__ == '__main__':

    # Windows work-around
    if os.name == "nt":
        myappid = u'newaetech.chipwhisperer.analyzer.git'  # arbitrary string
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    # Fix for running CWCapture without a terminal
    # See http://stackoverflow.com/a/4230866
    if sys.executable.endswith("pythonw.exe"):
        sys.stdout = sys.stderr = NullWriter()

    # cProfile.run('main()')
    main()
