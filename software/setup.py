#!/usr/bin/env python

from setuptools import setup

setup(
    name = 'chipwhisperer',
    version = '3.1rc3',
    description = "ChipWhisperer Side-Channel Analysis Tool",
    author = "Colin O'Flynn",
    author_email = 'coflynn@newae.com',
    license = 'GPLv3',
    url = 'http://www.ChipWhisperer.com',
    download_url='https://www.newae.com/files/chipwhisperer-3.1rc1.tar.gz',
    packages = ['chipwhisperer',
                'chipwhisperer.capture.acq_pattern',
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
                'chipwhisperer.analyzer.analysis',
                'chipwhisperer.analyzer.analysis.diffmode',
                'chipwhisperer.analyzer.analysis.partition',
                'chipwhisperer.analyzer.attacks',
                'chipwhisperer.analyzer.attacks.cpa_algorithms',
                'chipwhisperer.analyzer.attacks.profiling_algorithms',
                'chipwhisperer.analyzer.attacks.models',
                'chipwhisperer.analyzer.models',
                'chipwhisperer.analyzer.models.aes',
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
                ],
    install_requires = [
        'configobj',
        'docutils',
        'numpy',

        # pyusb could be removed from required list, but for most people it's needed
        # and it's not "too" bad (i.e., won't hurt on a cloud platform I think)
        'pyusb',

        #This is normally required, but not listed to avoid pulling in on cloud platforms, which will also
        #trigger a bunch of stuff like pyside
        #pyqtgraph
    ],
    scripts=[
        'scripts/chipwhisperer-ana',
        'scripts/chipwhisperer-cap',
    ],
)
