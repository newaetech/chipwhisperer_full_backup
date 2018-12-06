""" CPA Algorithms

Algorithms:
* Progressive - Online calculation allowing feedback during attack. You probably want this.
* SimpleLoop - Simple attack loop. No feedback before end of attack
* ProgressiveCAccel - Progressive with ctypes to increase speed. Experimental/untested.
"""

from .progressive import CPAProgressive as Progressive
from .simpleloop import CPASimpleLoop as SimpleLoop
from .progressive_caccel import CPAProgressive_CAccel as ProgressiveCAccel
