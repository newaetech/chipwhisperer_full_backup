#! /usr/bin/env python
"""\
Scan for serial ports.

Part of pySerial (http://pyserial.sf.net)
(C) 2002-2003 <cliechti@gmx.net>

The scan function of this module tries to open each port number
from 0 to 255 and it builds a list of those ports where this was
successful.
"""

import platform
import glob
import serial
list_ports = None
try:
    from serial.tools import list_ports
except ImportError:
    pass


def scan(max_range=256):
    """scan for available ports. return a list of names"""
    if not list_ports:
        # Very old pyserial compatibility mode
        system_name = platform.system()
        if system_name == 'Windows':
            available = []
            for i in range(max_range):
                try:
                    s = serial.Serial("COM%d" % i)
                    available.append(s.portstr)
                    s.close()   # explicit close 'cause of delayed GC in java
                except serial.SerialException:
                    pass
            return available
        elif system_name == 'Darwin':
            return glob.glob('/dev/cu.*')
        return glob.glob('/dev/ttyS*') + glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')

    ports = list_ports.comports()
    # Newer pyserial returns objects with attributes, older versions
    # were returning each port as a list.
    if hasattr(ports, 'device'):
        ports = [x.device for x in ports]
    else:
        ports = [x[0] for x in ports]
    return sorted(ports)[:max_range]

if __name__=='__main__':
    print ("Found ports:")
    for n,s in enumerate(scan()):
        print ("(%d) %s" % (n,s))
