#! /usr/bin/env python
"""\
Scan for serial ports.

Part of pySerial (http://pyserial.sf.net)
(C) 2002-2003 <cliechti@gmx.net>

The scan function of this module tries to open each port number
from 0 to 255 and it builds a list of those ports where this was
successful.
"""

import sys
import platform
import glob

try:
    import serial
except ImportError:
    print "*****************************************"
    print "*****************************************"
    print "PySerial not found. Install from https://pypi.python.org/pypi/pyserial or http://www.lfd.uci.edu/~gohlke/pythonlibs/#pyserial"
    print "*****************************************"
    print "*****************************************"
    sys.exit()

def scan():
    """scan for available ports. return a list of names"""
    system_name = platform.system()
    if system_name == 'Windows':
      available = []
      for i in range(64):
          try:
              s = serial.Serial(i)
              available.append(s.portstr)
              s.close()   # explicit close 'cause of delayed GC in java
          except serial.SerialException:
              pass
      return available
    else:
      return glob.glob('/dev/ttyS*') + glob.glob('/dev/ttyUSB*')

if __name__=='__main__':
    print ("Found ports:")
    for n,s in scan():
        print ("(%d) %s" % (n,s))
