#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
    name='chipwhisperer',
    version='5.1.0dev',
    description="ChipWhisperer Side-Channel Analysis Tool",
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    author="Colin O'Flynn",
    author_email='coflynn@newae.com',
    license='GPLv3',
    url='www.chipwhisperer.com',
    packages=find_packages('software'),
    package_dir={'': 'software'},
    install_requires=[
        'configobj',
        'pyserial',
        'docutils',  # makes help more readable.
        'numpy',
        'pyusb',
        'scipy',
        'pycryptodome'
    ],
    project_urls={
        'Documentation': 'https://chipwhisperer.readthedocs.io',
        'Source': 'https://github.com/newaetech/chipwhisperer',
        'Issue Tracker': 'https://github.com/newaetech/chipwhisperer/issues',
    },
    python_requires='~=3.4',
)
