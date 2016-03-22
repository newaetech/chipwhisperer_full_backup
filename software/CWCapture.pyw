# Calls ChipWhisperer w/o the annoying window
import ctypes
import os

from chipwhisperer.capture.ui.ChipWhispererCapture import main

if __name__ == '__main__':

    # Windows work-around
    if os.name == "nt":
        myappid = u'newaetech.chipwhisperer.capture.git'  # arbitrary string
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    cwdir = os.path.join('chipwhisperer', 'capture')
    main(cwdir)
