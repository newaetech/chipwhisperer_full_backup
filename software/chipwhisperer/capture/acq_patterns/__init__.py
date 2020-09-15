"""
Package containing all of the key text generation types

KTP:
* Basic - fixed/random key and text (default fixed key, random text)
* DPA - Applies mask to random plaintext, then randomly undoes it
* TVLATTest
"""
from .basic import AcqKeyTextPattern_Basic as Basic
from .dpahelper import AcqKeyTextPattern_DPA as DPA
from .tvlattest import AcqKeyTextPattern_TVLATTest as TVLATTest
from .var_vec import AcqKeyTextPattern_VarVec as VarVec
