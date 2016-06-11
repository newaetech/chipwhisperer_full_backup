#!/usr/bin/env python

from setuptools import setup

setup(
    name = 'chipwhisperer',
    version = '3.1rc1',
    description = "ChipWhisperer Side-Channel Analysis Tool",
    author = "Colin O'Flynn",
    author_email = 'coflynn@newae.com',
    license = 'GPLv3',
    url = 'www.ChipWhisperer.com',
    download_url='https://www.newae.com/files/chipwhisperer-3.1rc1.tar.gz',
    packages = ['chipwhisperer',
                'chipwhisperer.capture.api',
                'chipwhisperer.capture.scopes',
                'chipwhisperer.capture.scopes.cwhardware',
                'chipwhisperer.capture.scopes.openadc_interface',
                'chipwhisperer.capture.scopes.visascope_interface',
                'chipwhisperer.capture.targets',
                'chipwhisperer.capture.targets.simpleserial_readers',
                'chipwhisperer.capture.targets.smartcard_readers',
                'chipwhisperer.capture.targets.smartcard_protocols',
                'chipwhisperer.capture.ui',
                'chipwhisperer.capture.utils',
                'chipwhisperer.analyzer',
                'chipwhisperer.analyzer.attacks',
                'chipwhisperer.analyzer.attacks.cpa_algorithms',
                'chipwhisperer.analyzer.attacks.cpa_algorithms.c_accel',
                'chipwhisperer.analyzer.attacks.models',
                'chipwhisperer.analyzer.preprocessing',
                'chipwhisperer.analyzer.ui',
                'chipwhisperer.analyzer.utils',
                'chipwhisperer.analyzer.utils.TraceExplorerScripts',
                'chipwhisperer.common',
                'chipwhisperer.common.api',
                'chipwhisperer.common.results',
                'chipwhisperer.common.traces',
                'chipwhisperer.common.ui',
                'chipwhisperer.common.utils',
                'chipwhisperer.hardware',
                'chipwhisperer.hardware.firmware',
                'chipwhisperer.hardware.naeusb',
                ],
    install_requires = [
        'pyqtgraph',
        'configobj',
        #pyusb could be removed from required list, but for most people it's needed
        'pyusb',
        'docutils',
        'numpy'
    ]
)
