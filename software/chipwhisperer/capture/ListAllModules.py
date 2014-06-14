#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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

import sys

try:
    import chipwhisperer.capture.scopes.OpenADC
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["scopes.OpenADC", ok, err])

try:
    import chipwhisperer.capture.scopes.VisaScope
    ok = True
    err = ""
except:
    ok = False
    err = str(sys.exc_info())
finally:
    modList.append(["scopes.VisaScope", ok, err])

try:
    import chipwhisperer.capture.scopes.PicoScope
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["scopes.PicoScope", ok, err])

try:
    import chipwhisperer.capture.scopes.PartialReconfiguration
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["scopes.PartialReconfiguration", ok, err])

try:
    import chipwhisperer.capture.scopes.ChipWhispererExtra
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["scopes.ChipWhispererExtra", ok, err])

try:
    import chipwhisperer.capture.scopes.ChipWhispererGlitch
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["scopes.ChipWhispererGlitch", ok, err])


try:
    import chipwhisperer.capture.targets.ChipWhispererTargets
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["targets.ChipWhispererTargets", ok, err])

try:
    import chipwhisperer.capture.targets.SAKURAG
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["targets.SAKURAG", ok, err])

try:
    import chipwhisperer.capture.targets.SASEBOGII
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["targets.SASEBOGII", ok, err])

try:
    import chipwhisperer.capture.targets.SimpleSerial
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["targets.SmartCard", ok, err])

try:
    import chipwhisperer.capture.targets.ChipWhispererSPI
    ok = True
    err = ""
except ImportError, e:
    ok = False
    err = str(e)
finally:
    modList.append(["targets.ChipWhispererSPI", ok, err])

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

