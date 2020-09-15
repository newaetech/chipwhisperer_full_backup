
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

import random
from chipwhisperer.common.utils import util
from ._base import AcqKeyTextPattern_Base
from chipwhisperer.common.utils.util import camel_case_deprecated
from .basic import AcqKeyTextPattern_Basic


class AcqKeyTextPattern_VarVec(AcqKeyTextPattern_Basic):
    """ Constant Text KTP with variable vector (aka single row/column)

    A constant plaintext, except for one row or column. This will result
    in a MixColumns with all bytes constant except 1 across all 4 columns.

    Useful for attacking AES after (Inv)MixColumns/ARK1. Must be analyzed
    with appropriate method (sum of absolute single bit CPA/DPA for MixColumns outputs).
    An attack class is available in chipwhisperer.analyzer.attacks.attack_mix_columns,
    but requires scared, which may be hard to install on Windows

    Args:
        var_vec (str): Which type of vector ('column' or 'row') to set as variable. Defaults
                       'column'. 'row' isn't well tested, but should work.

    Make sure to set var_vec as appropriate for your capture campaign::

        # assuming setup scope
        ktp = cw.ktp.VarVec()
        projects = []
        for cmpgn in range(4):
            project = cw.create_project(f'var_vec_proj_{cmpgn}')
            projects.append(project)
            for i in range(N):
                ktp.var_vec = cmpgn
                key, text = ktp.next()
                trace = cw.capture_trace(scope, target, text, key)
                if trace is None:
                    print('could not capture trace')
                    continue
                project.traces.append(trace)
            project.save()
    """
    VEC_TYPE_COL = 0x00
    VEC_TYPE_ROW = 0x01

    def __init__(self, vec_type="column"):
        super().__init__()
        if vec_type not in ["column", "row"]:
            raise ValueError(f"Invalid vector type {vec_type}")
        if vec_type == "column":
            self.vec_type = self.VEC_TYPE_COL
        else:
            self.vec_type = self.VEC_TYPE_ROW
        self._var_vec = 0x00

        self.COL_LUT = []
        self.ROW_LUT = []
        for i in range(4):
            self.COL_LUT.append(list(range(i*4, i*4+4)))
            self.ROW_LUT.append(list(range(i, 16, 4)))

    @property
    def var_vec(self):
        """Which vector to set as variable (aka which column/row)
        """
        return self._var_vec

    @var_vec.setter
    def var_vec(self, n):
        if n not in range(4):
            raise ValueError(f"Invalid vector number {n}")
        self._var_vec = n

    def next(self):
        tmp = bytearray(16)
        key, text = super().next()
        lut = None
        if self.vec_type == self.VEC_TYPE_COL:
            lut = self.COL_LUT
        else:
            lut = self.ROW_LUT

        for val in lut[self.var_vec]:
            tmp[val] = text[val]

        return key, tmp