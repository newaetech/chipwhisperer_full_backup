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
                'chipwhisperer.api',
                'chipwhisperer.api.ui',
                'chipwhisperer.api.api',
                'chipwhisperer.api.scopes',
                'chipwhisperer.api.scopes.cwhardware',
                'chipwhisperer.api.scripts',
                'chipwhisperer.api.targets',
                'chipwhisperer.api.utils',
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
