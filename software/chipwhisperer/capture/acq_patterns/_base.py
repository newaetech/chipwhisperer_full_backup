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

from chipwhisperer.common.utils.util import camel_case_deprecated

class AcqKeyTextPattern_Base:
    _name = "Key/Text Pattern"

    def __init__(self):
        self._key_len = 16
        self._text_len = 16
        self._key = None
        self._textin = None

    def setTarget(self, target):
        pass

    @property
    def key_len(self):
        return self._key_len

    @key_len.setter
    def key_len(self, n):
        self._key_len = n

    @property
    def text_len(self):
        return self._text_len

    @text_len.setter
    def text_len(self, n):
        self._text_len = n

    @property
    def fixed_key(self):
        """Generate fixed key (True) or not (False).

        :Getter: Return True if using fixed key or False if not.

        :Setter: Set whether to use fixed key (True) or not (False).
        """
        raise NotImplementedError("Target does not allow key to be changed to fixed/unfixed")

    @fixed_key.setter
    def fixed_key(self, enabled):
        raise NotImplementedError("Target does not allow key to be changed to fixed/unfixed")

    @property
    def fixed_text(self):
        """Generate fixed plaintext (True) or not (False).

        :Getter: Return True if using fixed plaintext or False if not.

        :Setter: Set whether to use fixed plaintext (True) or not (False).
        """
        raise NotImplementedError("Target does not allow plaintext to be changed to fixed/unfixed")

    @fixed_text.setter
    def fixed_text(self, enabled):
        raise NotImplementedError("Target does not allow plaintext to be changed to fixed/unfixed")

    def keyLen(self):
        return self._key_len

    def textLen(self):
        return self._text_len

    def validateKey(self):
        pass

    def validateText(self):
        pass

    def _initPattern(self):
        pass

    def setInitialKey(self, initialKey, binaryKey=False):
        pass

    def setInitialText(self, initialText, binaryText=False):
        pass

    def init(self, maxtraces):
        """Initialize ktp for trace runs

        Not required for all targets (i.e. a basic key text pair)
        """
        raise NotImplementedError("This target does not implement init")

    init_pair = init

    initPair = camel_case_deprecated(init_pair)

    def new_pair(self):
        """Called when a new encryption pair is requested"""
        raise NotImplementedError("This needs to be reimplemented")
    newPair = camel_case_deprecated(new_pair)

    def __str__(self):
        return "key: {}\ntext: {}".format(list(hex(b) for b in self._key), list(hex(b) for b in self._textin))

    def next(self):
        self.new_pair()
