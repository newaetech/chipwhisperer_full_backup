"""Package containing side channel attacks

Attack Types:
CPA - Correlation Power Analysis Attack
Profiling - Profiling Power Analysis Attack

Algorithms:
Progressive - CPA via "online" calculation
"""

from .cpa import CPA
from .profiling import Profiling

