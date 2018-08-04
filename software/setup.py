#!/usr/bin/env python

from setuptools import setup

setup(
    name = 'chipwhisperer',
    version = '4.0.2',
    description = "ChipWhisperer Side-Channel Analysis Tool",
    author = "Colin O'Flynn",
    author_email = 'coflynn@newae.com',
    license = 'GPLv3',
    url = 'http://www.ChipWhisperer.com',
    download_url='https://github.com/newaetech/chipwhisperer/archive/4.0.2.tar.gz',
    packages = ['chipwhisperer',
                'chipwhisperer.capture',
                'chipwhisperer.capture.acq_patterns',
                'chipwhisperer.capture.api',
                'chipwhisperer.capture.auxiliary',
                'chipwhisperer.capture.scopes',
                'chipwhisperer.capture.scopes.cwhardware',
                'chipwhisperer.capture.scopes.openadc_interface',
                'chipwhisperer.capture.scopes.visascope_interface',
                'chipwhisperer.capture.targets',
                'chipwhisperer.capture.scripts',
                'chipwhisperer.capture.targets.simpleserial_readers',
                'chipwhisperer.capture.targets.smartcard_readers',
                'chipwhisperer.capture.targets.smartcard_protocols',
                'chipwhisperer.capture.ui',
                'chipwhisperer.capture.utils',
                'chipwhisperer.analyzer',
                'chipwhisperer.analyzer.attacks',
                'chipwhisperer.analyzer.attacks.cpa_algorithms',
                'chipwhisperer.analyzer.attacks.profiling_algorithms',
                'chipwhisperer.analyzer.attacks.models',
                'chipwhisperer.analyzer.attacks.models.aes',
                'chipwhisperer.analyzer.preprocessing',
                'chipwhisperer.analyzer.scripts',
                'chipwhisperer.analyzer.ui',
                'chipwhisperer.analyzer.utils',
                'chipwhisperer.analyzer.utils.TraceExplorerScripts',
                'chipwhisperer.common',
                'chipwhisperer.common.api',
                'chipwhisperer.common.results',
                'chipwhisperer.common.scripts',
                'chipwhisperer.common.traces',
                'chipwhisperer.common.ui',
                'chipwhisperer.common.utils',
                'chipwhisperer.hardware',
                'chipwhisperer.hardware.firmware',
                'chipwhisperer.hardware.naeusb',
                'chipwhisperer.tests',
                ],
    install_requires = [
        'configobj',
        'pyserial',

        #docutils is kinda optional, but makes the help prettier so we add it here
        'docutils',

        #Numpy is required, but might be installed in a way pip doesn't know about it. Need to test this more still.
        #'numpy',

        # pyusb could be removed from required list, but for most people it's needed
        # and it's not "too" bad (i.e., won't hurt on a cloud platform I think)
        'pyusb',

        #This is normally required, but not listed to avoid pulling in on cloud platforms, which will also
        #trigger a bunch of stuff like pyside, numpy, etc.
        #pyqtgraph
    ],
    scripts=[
        'scripts/chipwhisperer-ana',
        'scripts/chipwhisperer-cap',
    ],
)
