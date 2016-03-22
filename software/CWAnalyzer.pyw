# Calls ChipWhisperer w/o the annoying window
import ctypes
import os

from chipwhisperer.analyzer.ui.ChipWhispererAnalyzer import main

if __name__ == '__main__':

    # Windows work-around
    if os.name == "nt":
        myappid = u'newaetech.chipwhisperer.analyzer.git'  # arbitrary string
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    cwdir = os.path.join('chipwhisperer', 'analyzer')

    main(cwdir)
