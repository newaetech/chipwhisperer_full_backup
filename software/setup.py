#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
    name = 'chipwhisperer',
    version = '5.0.1',
    description = "ChipWhisperer Side-Channel Analysis Tool",
    author = "Colin O'Flynn",
    author_email = 'coflynn@newae.com',
    license = 'GPLv3',
    url = 'http://www.ChipWhisperer.com',
    download_url='https://github.com/newaetech/chipwhisperer/archive/4.0.2.tar.gz',
    packages = find_packages('software'),
    package_dir = {'': 'software'},
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
    ],
)
