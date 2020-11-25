"""
Package containing all of the target types that the ChipWhisperer API can connect to:

Targets:
* SimpleSerial - Communication via CW Uart Pins
* CW305 - USB communication to CW305
* SAKURAG - Requires FTDI module, old, untested
* SASEBOGII - Requires FTDI module, old, untested
* SmartCard - Currently unavailable (tied to GUI)
"""
from .SimpleSerial import SimpleSerial
from .CW305 import CW305
from .SimpleSerial2 import SimpleSerial2
from .CW305_ECC import CW305_ECC

try:
    from .sakura_g import SakuraG #needs ftdi module
except ImportError:
    pass

# from .SmartCard import SmartCard #Tied to GUI
