# -*- coding: utf-8 -*-
#
# Copyright (c) 2025, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#==========================================================================

import pytest

def pytest_addoption(parser):
    parser.addoption("--reps", action="store", default=1, help="Number of times to run some of the tests.")
    parser.addoption("--fulltest", action="store_true", default=False, help="Run all tests (slow)")
    parser.addoption("--stress", action="store_true", default=False, help="Run more iterations of temp/voltage stress.")
    parser.addoption("--swo_trace", action="store_true", default=False, help="Enable trace tests.")
    parser.addoption("--xadc", action="store_true", default=False, help="Report XADC temp and status after each test")
    parser.addoption("--log", action="store_true", default=False, help="Log XADC stats to file")
    parser.addoption("--loose", action="store_true", default=False, help="Loosen test margins for test_s6_glitch.py")
    parser.addoption("--target", action="store", default="Lite", help="Target name for test_s6_glitch.py (Lite/Pro)")

@pytest.fixture
def fulltest(request):
    return request.config.getoption("--fulltest")

@pytest.fixture
def stress(request):
    return request.config.getoption("--stress")

@pytest.fixture
def swo_trace(request):
    return request.config.getoption("--swo_trace")

@pytest.fixture
def reps(request):
    return int(request.config.getoption("--reps"))

@pytest.fixture
def xadc(request):
    return int(request.config.getoption("--xadc"))

@pytest.fixture
def log(request):
    return int(request.config.getoption("--log"))

@pytest.fixture
def loose(request):
    return int(request.config.getoption("--loose"))

@pytest.fixture
def target(request):
    return request.config.getoption("--target")

