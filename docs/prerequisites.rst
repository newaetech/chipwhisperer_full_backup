.. _prerequisites:

#############
Prerequisites
#############

.. note:: We recommend using Python 3.7 as it seems to have the best general compatability; however,
        any version >= 3.6 should work. Python 2.x does **NOT** work with this codebase.

If you're using our virtual machine or Windows installer, 
you can proceed directly to :ref:`install`. Otherwise,
select our operating system below for instructions on
installing required software for ChipWhisperer:

 * :ref:`GNU/Linux <prerequisites-linux>`
 * :ref:`Windows <prerequisites-windows>`
 * :ref:`Mac <prerequisites-mac>`

.. _prerequisites-linux:

*********
GNU/Linux
*********

There is only manual install available on GNU/Linux, besides the VM; however, it is much
easier to grab all the required prerequisites than on Windows.

Quick Install
=============

The following commands are provided if you want to get right into using ChipWhisperer.

.. code:: bash

    sudo apt install python3 python3-pip libusb-dev make git avr-libc gcc-avr gcc-arm-none-eabi libusb-1.0.0-dev
    sudo printf "SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"2b3e\", ATTRS{idProduct}==\"ace[0-9]|c[3-6][0-9][0-9]\", TAG+=\"uaccess\"" >> /etc/udev/rules.d/50-newae.rules
    sudo udevadm control --reload-rules
    sudo usermod -a -G dialout $USER

You are now ready to move on to :ref:`install-repo`.

We go through each of these commands below:

Python
======

On Linux installing Python and the the associated package is straightforward.
Typically, you can install them from the package manager (ie. apt-get,
yum, ...).

On Ubuntu or similar:

.. code:: bash

    sudo apt install python3 python3-pip

Packages
========

There are some packages required for **chipwhisperer** and its dependencies such
as **libusb1** to work. Install using:

.. code:: bash

    sudo apt install libusb-dev make libusb-1.0.0-dev

You'll probably want to pick up git as well:

.. code:: bash

    sudo apt install git

Compilers
=========

The AVR Toolchain can be installed using:

.. code:: bash

    sudo apt install avr-libc gcc-avr

The Arm Toolchain can be installed using:

.. code:: bash

    sudo apt install gcc-arm-none-eabi

Hardware Drivers
================

The driver for Linux is built in; however, you need to allow your user account to access the peripheral. To do so, you'll have to make a 
file called :code:`/etc/udev/rules.d/50-newae.rules`. The contents of this file should be:

.. code::

    # Match all CW devices
    SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace[0-9]|c[3-6][0-9][0-9]", TAG+="uaccess"

The following command will put this into the proper file:

.. code:: bash

    sudo printf "SUBSYSTEM==\"usb\", ATTRS{idVendor}==\"2b3e\", ATTRS{idProduct}==\"ace[0-9]|c[3-6][0-9][0-9]\", TAG+=\"uaccess\"" >> /etc/udev/rules.d/50-newae.rules

Alternatively, you can just copy :code:`chipwhisperer/hardware/50-newae.rules`
to :code:`/etc/udev/rules.d/`.

Next, reset the udev system:

.. code:: bash

    sudo udevadm control --reload-rules

Finally log out & in again for the group change to take effect.

You can always find the latest version of this file on
`Github <https://github.com/newaetech/chipwhisperer/blob/develop/hardware/50-newae.rules>`_.

You should also add your username to the dialout group, which will allow you to reprogram
the USB firmware on your ChipWhisperer and use the ChipWhisperer's serial port:

.. code:: bash

    sudo usermod -a -G dialout YOUR-USERNAME

ChipWhisperer
=============

You are now ready to move on to :ref:`install-repo`.

.. _prerequisites-windows:

**************
Windows Manual
**************

Starting with ChipWhisperer 5.5, all of the Windows prerequisites
will can be installed by using our :ref:`install-windows-exe`. If
you don't want to use our installer, you should grab
the following programs:

Python
======

For any of the other installation methods, you'll need to have Python
3 installed on your computer. If you already a recent version of
Python installed (3.6.x+), you can skip this step. 

The recommend method of installing Python is to use a distribution
called `WinPython`_. This setup avoids installing Python globally, and
includes most of the software you will need. In addition it makes it
possible to install 32-bit and 64-bit Python on the same system with
minimal problems. This can be very useful as the 64-bit version is
handy for doing analysis on large data sets.

To install WinPython 3.7.x, Download a release in the 3.7.x branch
from the WinPython site. ChipWhisperer works with both 32-bit and
64-bit versions of Python.

Choose a reasonable location to install this to - note the default is
simply in the download directory. Instead it's recommended to find a
directory such as */WinPython32bit-3.7.1.0*, or into your local
directory such as *c:/Users/yourname/WinPython32bit-3.7.1.0*.

Go to your installation directory for WinPython, and run the shortcut
called *WinPython Command Prompt.exe*. This will give you a command
prompt which is setup to run Python along with associated scripts.

Optional: You can add the python.exe you just installed to your PATH.
To do so navigate to your installation folder, and run the *WinPython
Control Panel.exe* program. Then select **Advanced** -> **Register**
distribution. If you do not do this, you will have to run all commands
in this document via the *WinPython Command Prompt.exe*. If you plan
on running both 32-bit and 64-bit Python, you should not register
them. Instead explicitly call the correct Python by always running the
*WinPython Command Prompt.exe*, and then calling specific scripts or
notebooks.

.. _WinPython: http://winpython.sourceforge.net/


Installing Hardware Drivers
===========================

As of ChipWhisperer firmware \*.23, your ChipWhisperer will automatically
configure as a WinUSB device, meaning no manual driver installation is
required.

If your ChipWhisperer has older firmware, see :ref:`our driver help page.<windows-drivers>`

.. _releases: https://github.com/newaetech/chipwhisperer/releases
.. _firmware_update: https://chipwhisperer.readthedocs.io/en/latest/api.html#firmware-update


Make
====
To build firmware for target boards, you'll need to install GNU Make. There's
a couple of ways you can install make:

 * Install MinGW and add :code:`MinGW\msys\1.0\bin` to your path.
 * Install Cygwin, install make, and add :code:`cygwin\bin` to your path.
 * Install `avr-gcc`_ and add :code:`avr-gcc-xx.x.x-x64-windows\\bin\\` to your path.

.. _avr-gcc: https://blog.zakkemble.net/avr-gcc-builds/

Compilers
=========

It assumed any time the :code:`make` command is run, that the
appropriate compiler is on the path, whether that is the `ARM toolchain`_ or
`avr-gcc`_. You'll need to add the :code:`bin` folder from your `avr-gcc`_ 
installation to your path.


.. _ARM toolchain: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
.. _avr-gcc: https://blog.zakkemble.net/avr-gcc-builds/

ChipWhisperer
=============

Remember that any time you install packages for python during the installation,
use the *WinPython Command Prompt.exe*.You are ready to move on to
:ref:`install-repo`.


.. _prerequisites-mac:

********
Mac OS X
********

The Mac OS X instructions are similar to the :ref:`GNU/Linux <prerequisites-linux>`
once you have **brew** installed.

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
    pyenv install 3.7.7
    pyenv global 3.7.7
    echo -e 'if command -v pyenv 1>/dev/null 2>&1; then\n  eval "$(pyenv init -)"\nfi' >> ~/.zshrc

Then launch a new terminal and run

.. code:: bash

    python --version

and make sure it reports that you're using Python 3.7.7.

Compilers
=========

The compilers are also available on Mac OSx. The ARM compiler can be found
on the `ARM website`_. If needed, the AVR compiler can be installed as well
using:

.. code:: bash

    brew tap osx-cross/avr
    brew install avr-gcc

This brew install does not work for some older versions of Mac OS X. You will
have to find a different way to install the compilers.

ChipWhisperer
=============

You are now ready for :ref:`installing <install-repo>` ChipWhisperer.

.. _Python Software Foundation's website: https://www.python.org/downloads/mac-osx/
.. _ARM website: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads