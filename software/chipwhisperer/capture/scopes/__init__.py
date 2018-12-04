#just imports
from .OpenADC import OpenADC as CWLite
CWPro = CWLite #same interface as lite
from .cwnano import CWNano
from .PicoScope import PicoScopeInterface as PicoScope

try:
    from .VisaScope import VisaScopeInterface as VisaScope #not working (missing visa module?)
except ImportError:
    pass
