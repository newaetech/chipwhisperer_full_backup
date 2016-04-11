#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

from chipwhisperer.common.api.config_parameter import ConfigParameter


class AcqKeyTextPattern_Base(object):
    def __init__(self, target=None):
        self.params = ConfigParameter.create_extended(self, name='Key/Text Pattern', type='group', children=self.setupParams())
        self._target = target
        self._initPattern()

    def setTarget(self, target):
        self._target = target
        self._initPattern()

    def keyLen(self):
        if self._target:
            return self._target.keyLen()
        else:
            return 16

    def validateKey(self):
        if self._target:
            if len(self._key) != self._target.keyLen():
                raise IOError("Key Length Wrong for Given Target, %d != %d" % (self._target.keyLen(), len(self.key)))

            self._key = self._target.checkEncryptionKey(self._key)

    def setupParams(self):
        return [{'name':'Do Something', 'type':'bool'},]

    def paramList(self):
        return [self.params]

    def _initPattern(self):
        """Perform any extra init stuff required. Called at the end of main init() & when target changed."""
        pass

    def setInitialKey(self, initialKey, binaryKey=False):
        pass

    def setInitialText(self, initialText, binaryText=False):
        pass

    def initPair(self):
        """Called before a capture run, does not return anything"""
        raise AttributeError("This needs to be reimplemented")

    def newPair(self):
        """Called when a new encryption pair is requested"""
        raise AttributeError("This needs to be reimplemented")
