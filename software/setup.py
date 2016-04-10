#!/usr/bin/env python

from setuptools import setup

setup(
    name = 'chipwhisperer',
    version = '0.1',
    description = "ChipWhisperer Side-Channel Analysis Tool",
    author = "Colin O'Flynn",
    author_email = 'coflynn@newae.com',
    license = 'GPLv3',
    url = 'www.ChipWhisperer.com',
    packages = ['chipwhisperer',
                'chipwhisperer.capture',
                'chipwhisperer.capture.ui',
                'chipwhisperer.capture.api',
                'chipwhisperer.capture.scopes',
                'chipwhisperer.capture.scopes.cwhardware',
                'chipwhisperer.capture.scripts',
                'chipwhisperer.capture.targets',
                'chipwhisperer.capture.utils',
                'chipwhisperer.analyzer',
                'chipwhisperer.analyzer.attacks',
                'chipwhisperer.analyzer.attacks.models',
                'chipwhisperer.analyzer.preprocessing',
                'chipwhisperer.common',
                'chipwhisperer.common.api',
                'chipwhisperer.common.traces',
                'chipwhisperer.common.ui',
                'chipwhisperer.common.utils',
                ],
    install_requires = []
)
