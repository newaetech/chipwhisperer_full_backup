#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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

try:
    from pyqtgraph.parametertree import Parameter
    from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
    
    #Do not remove - this import is unused, but it affects other imports
    import chipwhisperer.common.ui.ParameterTypesCustom
    
    
    #Default parameter - uses pyqtgraph to store
    class ConfigParameter(Parameter):
        @staticmethod
        def create_extended(parent, *args, **kwargs):
            params = Parameter.create(*args, **kwargs)
            if parent:
                ExtendedParameter.setupExtended(params, parent)
            return params
        
except ImportError:
    
    #Fallback - uses dictionary
    class ConfigParameter(Parameter):
        @staticmethod
        def create_extended(parent, *args, **kwargs):
            return kwargs['children']