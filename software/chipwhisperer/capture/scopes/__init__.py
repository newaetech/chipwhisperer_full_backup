"""
Package containing all of the scope types that the ChipWhisperer API can connect to:

Scopes:

   * OpenADC- CWLite, CWPro, and CWHusky

   * CWNano - CWNano

   * PicoScope - PicoScope (old, untested)

   * VisaScope - VisaScope (requires Visa module, old, untested)

"""
from .OpenADC import OpenADC
from .cwnano import CWNano
from typing import Union
# try:
#     from .sakura_g import SakuraG
# except:
#     pass

ScopeTypes = Union[OpenADC, CWNano]
