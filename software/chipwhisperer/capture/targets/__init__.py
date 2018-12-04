"""
Package containing all of the target types that the ChipWhisperer API can connect to:

Scopes:
* SimpleSerial - Communication via CW Uart Pins
* CW305 - USB communication to CW305
* SAKURAG - Requires FTDI module, old, untested
* SASEBOGII - Requires FTDI module, old, untested
* SmartCard - Currently unavailable
"""
from .SimpleSerial import SimpleSerial
from .CW305 import CW305 #check works

try:
    from .SAKURAG import SakuraG #needs ftdi module
    from .SASEBOGII import SaseboGII
except ImportError:
    pass

# from .SmartCard import SmartCard #Tied to GUI
