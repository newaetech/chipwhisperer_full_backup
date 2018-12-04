from .SimpleSerial import SimpleSerial
from .CW305 import CW305 #check works

try:
    from .SAKURAG import SakuraG #needs ftdi module
    from .SASEBOGII import SaseboGII
except ImportError:
    pass

# from .SmartCard import SmartCard #Tied to GUI
