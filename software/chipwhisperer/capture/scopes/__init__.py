"""
Package containing all of the scope types that the ChipWhisperer API can connect to:

Scopes:

   * OpenADC- CWLite and CWPro

   * CWNano - CWNano

   * PicoScope - PicoScope (old, untested)

   * VisaScope - VisaScope (requires Visa module, old, untested)

"""
from .OpenADC import OpenADC
from .cwnano import CWNano
try:
   from .sakura_g import SakuraG
except:
   pass