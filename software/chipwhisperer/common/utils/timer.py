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
        self._single_shot = False

    def connect(self, callback):
        self._callback = callback

    def setInterval(self, ms_timeout):
        pass

    def start(self):
        if self._single_shot:
            print("Timer: Not using Qt, calling callback immediatly")
            self._callback()
        print("Timer: Not using Qt, timer disabled")

    def stop(self):
        pass

    def isActive(self):
        pass

    def flush(self):
        self._callback()

    def setSingleShot(self, single_shot):
        self._single_shot = single_shot

try:
    from PySide.QtCore import QTimer
    class Timer(QTimer):
        def flush(self):
            if self.isActive():
                self.timeout.emit()
                self.stop()

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
