#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
    name='chipwhisperer',
    version='5.5.0',
    description="ChipWhisperer Side-Channel Analysis Tool",
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    author="Colin O'Flynn",
    author_email='coflynn@newae.com',
    license='GPLv3',
    url='https://www.chipwhisperer.com',
    packages=find_packages('software'),
    data_files=[('chipwhisperer/hardware/naeusb', ['software/chipwhisperer/hardware/naeusb/libusb-1.0.dll', 'software/chipwhisperer/hardware/naeusb/libusb-master.zip'])],
    package_dir={'': 'software'},
    install_requires=[
        'configobj',
        'pyserial',
        'numpy',
        'pyusb',
        'ECpy',
        'scipy',
        'fastdtw',
        'Cython',
        'tqdm'
    ],
    project_urls={
        'Documentation': 'https://chipwhisperer.readthedocs.io',
        'Source': 'https://github.com/newaetech/chipwhisperer',
        'Issue Tracker': 'https://github.com/newaetech/chipwhisperer/issues',
    },
    python_requires='~=3.6',
)
