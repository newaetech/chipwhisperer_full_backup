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

class FakeQTimer(object):
    """ Replicates basic QTimer() API but does nothing """
    def __init__(self):
        self.timeout = self

    def connect(self, callback):
        pass

    def setInterval(self, ms_timeout):
        pass

    def start(self):
        pass

    def stop(self):
        pass

    def setSingleShot(self, _):
        pass

try:
    from PySide.QtCore import QTimer
    Timer = QTimer

except ImportError:
    Timer = FakeQTimer

def runTask(task, timeout_in_s, single_shot = False, start_timer = False):
    timer = Timer()
    timer.timeout.connect(task)
    timer.setInterval(int(timeout_in_s * 1000))
    timer.setSingleShot(single_shot)
    if start_timer:
        timer.start()
    return timer
