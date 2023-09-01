.. _install-mac:

*********************
Mac OS X Installation
*********************

Brew
====

Open your bash terminal and paste this into the prompt:

.. code:: bash

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

This snippet is taken directly from `brew's website`_. The script will guide
you through the installation.

.. _brew's website: https://brew.sh/

Packages
========

**libusb** is required by the **chipwhisperer** software to communicate with
the board. You can install it using **brew**:

.. code:: bash

    brew install libusb


Python
======

The best way to install Python on Mac is in a Python virtualenv. The steps
for this are found at https://opensource.com/article/19/5/python-3-default-mac.
A quick summary is:

.. code:: bash

    brew install pyenv
    pyenv install 3.9.5
    pyenv global 3.9.5
    echo -e 'if command -v pyenv 1>/dev/null 2>&1; then\n  eval "$(pyenv init -)"\nfi' >> ~/.zshrc

Then launch a new terminal and run

.. code:: bash

    python --version

and make sure it reports that you're using Python 3.9.5.

Compilers
=========

The compilers are also available on Mac OSx. You can install by running:

.. code:: bash

    # install arm-gcc
    brew install --cask gcc-arm-embedded

    # install avr-gcc
    brew tap osx-cross/avr
    brew install avr-gcc


This brew install does not work for some older versions of Mac OS X. You will
have to find a different way to install the compilers.

.. _Python Software Foundation's website: https://www.python.org/downloads/mac-osx/

Mac M1 Notes
============

ChipWhisperer is compatable with Mac M1; however, due to the M1's different architecture,
as well as its x86 emulation, you need to manually install libusb1 and ensure that the
architecture for libusb1 matches the architecture for Python.

The best way to do this seems to be to install Python via conda miniforge and to use the newest
homebrew to install libusb.

See https://github.com/newaetech/chipwhisperer/issues/381 for more information.

Installing ChipWhisperer
========================

See the :ref:`Linux install instructions. <linux-install-chipwhisperer>`