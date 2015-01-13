# Calls ChipWhisperer w/o the annoying window
from chipwhisperer.capture.ChipWhispererCapture import main
import ctypes
import os
if __name__ == '__main__':

    # Windows work-around
    if os.name == "nt":
        myappid = u'newaetech.chipwhisperer.capture.git'  # arbitrary string
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    main()
