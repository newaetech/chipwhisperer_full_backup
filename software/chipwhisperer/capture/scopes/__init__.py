#just imports
from .OpenADC import OpenADC as CWLite
CWPro = CWLite #same interface as lite
from .cwnano import CWNano
from .PicoScope import PicoScopeInterface as PicoScope
from .VisaScope import VisaScopeInterface as VisaScope
