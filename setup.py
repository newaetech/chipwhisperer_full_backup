#!/usr/bin/env python

from setuptools import setup, find_packages
__version__ = '5.7.0'
with open('software/chipwhisperer/version.py') as f:
    exec(f.read())

setup(
    name='chipwhisperer',
    version=__version__,
    description="ChipWhisperer Side-Channel Analysis Tool",
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    author="Colin O'Flynn",
    author_email='coflynn@newae.com',
    license='GPLv3',
    url='https://www.chipwhisperer.com',
    packages=find_packages('software'),
    package_dir={'': 'software'},
    package_data={'': ['py.typed', 'capture/trace/defines/*.v']},
    install_requires=[
        'configobj',
        'pyserial',
        'numpy',
        'libusb1',
        'ECpy',
        'fastdtw',
        'Cython',
        'tqdm',
    ],
    project_urls={
        'Documentation': 'https://chipwhisperer.readthedocs.io',
        'Source': 'https://github.com/newaetech/chipwhisperer',
        'Issue Tracker': 'https://github.com/newaetech/chipwhisperer/issues',
    },
    python_requires='~=3.7',
)
