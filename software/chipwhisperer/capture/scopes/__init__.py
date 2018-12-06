"""
Package containing all of the scope types that the ChipWhisperer API can connect to:

Scopes:

   * OpenADC- CWLite and CWPro

   * CWNano - CWNano

   * PicoScope - PicoScope (old, untested)

   * VisaScope - VisaScope (requires Visa module, old, untested)

"""
from chipwhisperer.capture.scopes.OpenADC import OpenADC
from .cwnano import CWNano
from .PicoScope import PicoScopeInterface as PicoScope

try:
    from .VisaScope import VisaScopeInterface as VisaScope #not working (missing visa module?)
except ImportError:
    pass
