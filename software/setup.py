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
                'chipwhisperer.capture.api.scopes',
                'chipwhisperer.capture.scripts',
                'chipwhisperer.capture.targets',
                'chipwhisperer.capture.utils',
                'chipwhisperer.analyzer',
                'chipwhisperer.analyzer.attacks',
                'chipwhisperer.analyzer.attacks.models',
                'chipwhisperer.analyzer.preprocessing'
                ],
    install_requires = []
)