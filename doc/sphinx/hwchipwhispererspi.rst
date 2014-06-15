.. _hwchipwhispererspi:

ChipWhisperer SPI Driver
===========================

Installing HIDAPI
---------------------

Communication with the HID USB interface requires a Python package, hidapi. Follow these steps to install:


1. If you are using WinPython or have pip installed, try the following::

        pip install hidapi
    
   If you do not have pip, you can also try easy_install::

        easy_install hidapi
    
    If you do not have a suitable C compiler installed, you will see the following error from those:
    
        error: Unable to find vcvarsall.bat
        
2. If you received the ``vcvarsall.bat`` error, continue with this guide. Download and install `MinGW <http://sourceforge.net/projects/mingw/files/latest/download?source=files>`

3. Once MinGW is installed, right-click on the *mingw32-gcc-g++* and select *Mark for Installation*

4. Under the *Installation* menu, select *Apply Changes*. Once this completes close the dialog & mingw window.

5. Create a file with the name ``C:\PYTHONG_PATH\Lib\distutils\distutils.cfg``, where *PYTHON_PATH* is your Python path. If you have used WinPython the path might be
   something like ``C:\WinPython-32bit-2.7.6.4\python-2.7.6\Lib\distutils\distutils.cfg``. The contents of this file must be::
   
    [build]
    compiler=mingw32

6. Re-run the ``pip install hidapi`` or ``easy_install hidapi``, looking for the final output **Successfully installed hidapi**.


NOTE: It appears sometimes the resulting hid.pyd file causes a crash on Windows. If this is the case a pre-compiled .pyd file is available for use.

Re-Programming the AVR
-----------------------

You need to use the **ChipWhispererSPI.hex** file in the AVR-USB. See the ChipWhisperer Capture Hardware guide.



