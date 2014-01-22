#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================


modList = []

try:
    import chipwhisperer.analyzer.attacks.CPA
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["attacks.CPA", ok, err])
    
try:
    import chipwhisperer.analyzer.attacks.CPAProgressive
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["attacks.CPAProgressive", ok, err])
    
try:
    import chipwhisperer.analyzer.attacks.CPASimpleLoop
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["attacks.CPASimpleLoop", ok, err])
    
try:
    import chipwhisperer.analyzer.preprocessing.Filter
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["preprocessing.Filter", ok, err])
    
try:
    import chipwhisperer.analyzer.preprocessing.Preprocessing
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["preprocessing.Preprocessing", ok, err])
    
try:
    import chipwhisperer.analyzer.preprocessing.ResyncCrossCorrelation
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["preprocessing.ResyncCrossCorrelation", ok, err])
    
try:
    import chipwhisperer.analyzer.preprocessing.ResyncPeakDetect
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["preprocessing.ResyncPeakDetect", ok, err])
    
try:
    import chipwhisperer.analyzer.preprocessing.ResyncSAD
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["preprocessing.ResyncSAD", ok, err])
 
try:
    import chipwhisperer.common.traces.TraceContainerNative
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["traces.TraceContinerNative", ok, err])

try:
    import chipwhisperer.common.traces.TraceContainerDPAv3
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["traces.TraceContainerDPAv3", ok, err])

try:
    import chipwhisperer.common.traces.TraceContainerMySQL
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["traces.TraceContainerMySQL", ok, err])


def ListAllModules():   
    return modList
    