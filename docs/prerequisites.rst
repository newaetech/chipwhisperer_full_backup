.. _prerequisites:

#############
Prerequisites
#############

A few methods for installing ChipWhisperer do not require prior installation of
the following prerequisites (:ref:`install-windows-auto`,
:ref:`install-virtual-machine`). However, if you want more control over where, and how
you install the prerequisites, the prerequisite installation process for your
operating system is detailed below.

 * :ref:`GNU/Linux <prerequisites-linux>`
 * :ref:`Windows <prerequisites-windows>`
 * :ref:`Mac <prerequisites-mac>`

.. _prerequisites-linux:

*********
GNU/Linux
*********

There is only manual install available on GNU/Linux.

Python
======

On Linux installing Python and the the associated package is straightforward.
Typically, you can install them from the package manager (ie. apt-get,
yum, ...).

On Ubuntu or similar:

.. code:: bash

    sudo apt install python3 python3-pip


Compilers
=========

The AVR Toolchain can be installed using:

.. code:: bash

    sudo apt install avr-libc gcc-avr

The Arm Toolchain can be installed using:

.. code:: bash

    sudo apt install gcc-arm-none-eabi

.. note::

    Some versions of Ubuntu (Ubuntu Bionic Beaver and likely before) provide
    a GNU Arm toolchain that links to incorrect files during the build process.
    To install a working version of the toolchain.

    .. code:: bash

        wget https://mirrors.kernel.org/ubuntu/pool/universe/n/newlib/libnewlib-dev_3.0.0.20180802-2_all.deb
        wget https://mirrors.kernel.org/ubuntu/pool/universe/n/newlib/libnewlib-arm-none-eabi_3.0.0.20180802-2_all.deb
        sudo dpkg -i libnewlib-arm-none-eabi_3.0.0.20180802-2_all.deb libnewlib-dev_3.0.0.20180802-2_all.deb


Hardware Drivers
================

The driver for Linux is built in, however you need to allow your user account to access the peripheral. To do so, you'll have to make a file called /etc/udev/rules.d/99-newae.rules . The contents of this file should be:

.. code::

    # CW-Lite
    SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace2", MODE="0664", GROUP="plugdev"

    # CW-1200
    SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace3", MODE="0664", GROUP="plugdev"

    # CW-Nano
    SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace0", MODE="0664", GROUP="plugdev"

    # CW-305 (Artix Target)
    SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="c305", MODE="0664", GROUP="plugdev"

    # CW-CR2
    SUBSYSTEM=="usb", ATTRS{idVendor}=="04b4", ATTRS{idProduct}=="8613", MODE="0664", GROUP="plugdev"
    SUBSYSTEM=="usb", ATTRS{idVendor}=="221a", ATTRS{idProduct}=="0100", MODE="0664", GROUP="plugdev"

Then add your username to the plugdev group:

.. code:: bash

    sudo usermod -a -G plugdev YOUR-USERNAME

And reset the udev system:

.. code:: bash

    sudo udevadm control --reload-rules

Finally log out & in again for the group change to take effect.

You can always find the latest version of this file on
`Github <https://github.com/newaetech/chipwhisperer/blob/master/hardware/99-newae.rules>`_.

ChipWhisperer
=============

You are now ready to move on to :ref:`install-repo`.


.. _prerequisites-windows:

**************
Windows Manual
**************

Python
======

For any of the other installation methods, you'll need to have Python
3 installed on your computer. If you already a recent version of
Python installed (3.5.x+), you can skip this step. Note that Python
2.x will **not** work with this codebase. There's also a bit of setup
that's needed to get other tools and prepare other drivers.

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


Python Packages
===============

There are a number of packages that the ChipWhisperer project uses.
You'll need to install these so that the software can run. With your
WinPython command prompt, navigate to where you downloaded
ChipWhisperer to, then move to the *chipwhisperer/software/* directory.

Run the following commands to get the needed packages:

.. code:: bash

    pip install -r requirements.txt


Installing Hardware Drivers
===========================

Details of driver installation are on specific pages for supported
hardware (such as cwcapturerev2 and naecw1173_cwlite). Drivers are
available from ChipWhisperer `releases`_ section.

.. _releases: https://github.com/newaetech/chipwhisperer/releases


Compilers
=========

It assumed any time the :code:`make` command is run, that the
appropriate compiler is on the path, whether that is the `ARM toolchain`_ or the
`AVR-GCC standalone`_.

.. _ARM toolchain: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
.. _AVR-GCC standalone: https://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en607654


ChipWhisperer
=============

You are ready to move on to :ref:`install-repo`.


.. _prerequisites-mac:

********
Mac OS X
********

Not yet available. Once users have *brew* installed, instructions should be similar to
:ref:`prerequisites-linux`.