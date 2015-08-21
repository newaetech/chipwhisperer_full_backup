.. |ChipWhisperer| unicode:: ChipWhisperer U+00AE

.. _installing:

Installing Python, |ChipWhisperer|, Drivers & Building Target Firmware
======================================================================

Using VMWare Virtual Machine
----------------------------


Windows XP/7
------------

Assuming you want to install the full ChipWhisperer software, you can follow the following steps to do so. Note it is recommended to use the VMWare image
if possible, since it includes all required tools out of the box!

Python Setup
^^^^^^^^^^^^

You may already have Python installed, in which case you can skip some of these steps. If you already have Python installed make sure it is
the correct version (2.7.x). Python 3.x will not work with this codebase.


.. _installingwinpython:

Installing Python via WinPython
"""""""""""""""""""""""""""""""

The recommend method of installing Python is to use a distribution called `WinPython <http://winpython.sourceforge.net/>`__. This setup avoids
installing Python globally, and includes most of the software you will need. In addition it makes it possible to install 32-bit and 64-bit
Python on the same system with minimal problems. This can be very useful as the 64-bit version is handy for doing analysis on large data sets.

1. Install WinPython 2.7.x. Download the latest release in the 2.7.x branch from the `WinPython <http://winpython.sourceforge.net/>`__ site. It's recommended
   to use the 32-bit version, but you can also use the 64-bit version. Note that certain drivers (such as the SmartCard driver) *do not* work on the 64-bit version.
   Choose a reasonable location to install this to - note the default is simply in the download directory. Instead it's recommended to find a directory such as
   ``c:\WinPython32bit-2.7.6.4``, or into your local directory such as ``c:\Users\yourname\WinPython-32bit-2.7.6.4``.

2. Go to your installation directory for WinPython, and run the shortcut called **WinPython Command Prompt.exe**. This will give you a command prompt which
   is setup to run Python along with associated scripts.

3. Run the following commands to get needed packages::

    pip install pyqtgraph
    pip install configobj

   Assuming you are planning on using the ChipWhisperer Capture Rev2 hardware, you also require `PyUSB <http://sourceforge.net/projects/pyusb/>`_.
   The easiest method is to again use pip::

    pip install pyusb

   If that command fails, you may need to specify a version. Note the latest version printed by the command, and try::

    pip install pyusb==1.0.0b1

4. Optional: You can add the python.exe you just installed to your path. To do so navigate to your installation folder, and run the **WinPython Control Panel.exe**
   program. Then select *Advanced -> Register distribution...*. If you do not do this, you will have to run all commands in this document via the **WinPython Command Prompt.exe**.

   If you plan on running both 32-bit and 64-bit Python, you should not register them. Instead explicitly call the correct Python by always running the **WinPython Command Prompt.exe**,
   and then calling specific programs (such as CW Capture or Analyzer) from that command prompt.
   
5. Jump to :ref:`installingcw` to install ChipWhisperer software.

.. _optional-ftdi:

Optional Packages
"""""""""""""""""

Note there are some optional packages you may want. Generally you can avoid them unless you have specific need of
the features they enable:

**FTD2XX**: `ftd2xx <https://github.com/snmishra/ftd2xx>`_ is required for SASEBO-W, SAKURA-G, and SASEBO-GII Support. To install
this package, `download a copy of the ftd2xx repository <https://github.com/snmishra/ftd2xx/archive/master.zip>`_ and
unzip it somewhere. Then run the following where you unzipped it::

    python setup.py install

This package will also require you to install the `FTDI D2XX Drivers <http://www.ftdichip.com/Drivers/D2XX.htm>`_. In the preceeding
link simply find the correct driver for your OS Version & install that.

**MYSQL**: If you want to use the MySQL trace format (not used by default), you'll need to install `umysql <https://pypi.python.org/pypi/umysql>`_::

	pip install umysql

**PYSCARD**: If planning on using a PS/SC smartcard reader (i.e. standard USB-connected reader), you will need to install `pyscard <https://sourceforge.net/projects/pyscard/files/pyscard/ >`__.

.. _installingcw:

Getting & Installing ChipWhisperer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Assuming you've got a working Python installation, the next step is to install & run ChipWhisperer itself. This requires you to get a copy
of ChipWhisperer, you should download the `Latest Release <https://www.assembla.com/wiki/show/chipwhisperer/ChipWhisperer_Software_Firmware_Releases>`_.
Download & unzip the file somewhere. Note if you are adventurous you can clone the GIT Repository ``git://git.assembla.com/chipwhisperer.git`` instead,
but this **may be broken** so you should start with the latest release instead.

Generally 'somewhere' will become your ChipWhisperer working directory. These examples assume you've chosen ``C:\chipwhisperer`` as your
working directory.

Once you've got the file, we run the Python install procedure again (setup.py). We slightly modify it to use the *develop* command to reflect
that the files will probably be changing frequently. Again open a terminal and run the following, adjusting paths as needed::

    cd c:\chipwhisperer\openadc\controlsw\python
    python setup.py develop
    cd c:\chipwhisperer\software
    python setup.py develop

If you wish to confirm the installation worked, you can run ChipWhispererAnalyzer in the same terminal::

    cd c:\chipwhisperer\software\chipwhisperer\analyzer
    python ChipWhispererAnalyzer.py

Alternatively you can just double-click on ``ChipWhispererAnalyzer.py`` from Windows explorer, which should run the file using
Python, *provided you've registered Python to execute the .py extension*.

You can see a `Video <http://www.youtube.com/watch?v=qd86cUD8iBs&hd=1>`__ of the Installation Procedure:

|YouTubeWin7Install|_

.. |YouTubeWin7Install| image:: /images/youtube-win7-install.png
.. _YouTubeWin7Install: http://www.youtube.com/watch?v=qd86cUD8iBs&hd=1

Installing Hardware Drivers
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Details of driver installation are on specific pages for supported hardware (such as :ref:`hwcapturerev2` and :ref:`naecw1173_cwlite`). Drivers are available from `ChipWhisperer <http://chipwhisperer.com>`__ release section.

.. _buildprogrammingavr:

Getting AVR Compiler Toolchain
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following section is **NOT REQUIRED** for your first attack - you can jump right to the tutorial if you wish. However you'll ultimately wish to
modify the code of the device under test, and these instructions tell you how. You should first follow the tutorial to confirm your system is working
before modifying the code however!

To build the code, you'll need to install WinAVR on Windows (if using the Virtual Machine, the following is *not required*, as the VM comes setup with the
AVR compiler already).

.. include:: installing_winavr.rsti

Note that the default codebase *does not* include the cryptographic code we attack. Instead you are required to add this code into the system -- if the
external code changes, this may eventually be instead rolled into the main codebase. To setup the code, follow the following steps:

1.  ``cd`` to the directory with the avr-serial example, and run ``make``::

        cd c:\chipwhisperer\hardware\victims\firmware\simpleserial-aes
        make

    If this is successful, you'll see an output like the following:

    .. image:: /images/avr-build-ok.png

    If instead you get an error something like ``make: *** No rule to make target `simpleserial.elf', needed by `elf'.  Stop.``, this means a required file was missing.

2. Programming the target AVR is accomplished in one of two methods depending on your hardware. The ChipWhisperer Capture Rev 2 uses the external "AVR Studio"
   program, whereas the CW1173 and CW1180 (ChipWhisperer-Lite based platforms) use a programmer menu from the ChipWhisperer-Capture software. To see details
   refer to either :ref:`cwcr2programmer` or :ref:`cwliteprogramming`.

Linux
-----

Python Setup
^^^^^^^^^^^^

On Linux, installing Python & all the associated packages is much easier than on Windows. Typically you can install them from a package manager, if you are
using Fedora Core or similar, just type::

    $ sudo yum install python27 python27-devel python27-libs python-pyside numpy scipy python-configobj pyusb

On Ubuntu or similar:

    $ sudo apt-get install python2.7 python2.7-dev python2.7-libs python-numpy python-scipy python-pyside python-configobj python-setuptools python-pip
    $ sudo pip install pyusb-1.0.0b1


You also need to install `PyQtGraph <http://www.pyqtgraph.org/>`_ which is not normally in those repositories. See the PyQtGraph website
for either a Debian/Ubuntu package, or information about installing from source.

Getting ChipWhisperer
^^^^^^^^^^^^^^^^^^^^^

As in the Windows release, you can download a complete ChipWhisperer software release. Alternatively you can clone the
ChipWhisperer repository with git, which is very simple on Linux::

    $ git clone git://git.assembla.com/chipwhisperer.git
    $ cd chipwhisperer
    $ git clone git://git.assembla.com/openadc.git

Be aware that the git code may be broken, although we try not to commit completely untested code to the master branch. Either way
once you have the `chipwhisperer` directory somewhere, do the following from within that directory::

    $ cd software
    $ sudo python setup.py develop
    $ cd ../openadc/controlsw/python
    $ sudo python setup.py develop

**NB**: This assumes the `python` command links to Python-2.7, and not Python-3.x. You may need to specify either `python2.7` or `python27`
as the command instead of python to force this.

ChipWhisperer Rev2 Capture Hardware Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The *driver* for Linux is built in, however you need to allow your user account to access the peripheral. To do so,
you'll have to make a file called ``/etc/udev/rules.d/99-ztex.rules`` . The contents of this file should be::

    # allow users to claim the device
    SUBSYSTEM=="usb", ATTRS{idVendor}=="04b4", ATTRS{idProduct}=="8613", MODE="0664", GROUP="plugdev"
    SUBSYSTEM=="usb", ATTRS{idVendor}=="221a", ATTRS{idProduct}=="0100", MODE="0664", GROUP="plugdev"

Then add your username to the plugdev group::

    $ sudo usermod -a -G plugdev YOUR-USERNAME

And reset the udev system::

    $ sudo udevadm control --reload-rules

Finally log out & in again for the group change to take effect.

AVR-USB Programmer
^^^^^^^^^^^^^^^^^^

To use the AVR Programmer from Linux, you again need to pass control of the device to a special group. Create
a file called ``/etc/udev/rules.d/99-avrisp.rules`` . The contents of this file should be::

    # allow users to claim the device
    SUBSYSTEM=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2104", MODE="0664", GROUP="plugdev"

You'll have to add yourself to the plugdev group if you haven't already::

    $ sudo usermod -a -G plugdev YOUR-USERNAME

And reset the udev system::

    $ sudo udevadm control --reload-rules

Finally log out & in again for the group change to take effect.


FTDI Hardware Driver (SASEBO-W, SAKURA-G, SASEBO-GII)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
**This is only required for supporting FTDI-connected hardware** such as the SASEBO-W, SAKURA-G, SASEBO-GII. This is NOT
required for the ChipWhisperer Capture Rev2.

First, you need to install the D2XX drivers & python module. See the section :ref:`optional-ftdi`.

Currently, there is a bit of a hack needed. You have to create (or modify if it exists) the file ``/etc/udev/rules.d/99-libftdi.rules`` .
The following modifications will cause **any FTDI-serial device to stop working**, so backup the existing file! The contents of this file should be::

    SUBSYSTEM=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6010", MODE="0664", GROUP="plugdev"
    ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6010", RUN+="/bin/sh -c 'echo $kernel > /sys/bus/usb/drivers/ftdi_sio/unbind'"

Then add your username to the plugdev group (if not already done)::

    $ sudo usermod -a -G plugdev YOUR-USERNAME

And reset the udev system::

    $ sudo udevadm control --reload-rules

Finally log out & in again for the group change to take effect.

.. _macosxinstall:

MAC OS X
--------

The following have been tested on Mac OS X Yosemite (10.10) - earlier versions may not have a recent enough Python installation (recommended
2.7.6 or later). It's possible to install other Python versions on your Mac OS X via the 'homebrew' system, we will use this for installing a
few additional required tools.

Python Setup
^^^^^^^^^^^^

1. Ensure your user account has a password. In order for the 'sudo' command to work it requires you to type your password, so if you don't have
   one enabled be sure to set a temporary password now.
   
2. Install the 'homebrew' system, see `brew.sh <http://brew.sh>`_ for details. Briefly, you can install it by pasting the following in a terminal::

    $ ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

3. Install PySide using homebrew::

    $ brew install pyside
   
   The install will probably print a message like this:
 
    .. image:: /images/install_macosx_brewpath.png   
   
   Note you must run that command in order to successfully import the modules, in this example it would be::
   
    $ mkdir -p /Users/macmini/Library/Python/2.7/lib/python/site-packages
    $ echo 'import site; site.addsitedir("/usr/local/lib/python2.7/site-packages")' >> /Users/macmini/Library/Python/2.7/lib/python/site-packages/homebrew.pth
    
  Be sure to adjust paths for your system as described in the message from homebrew.
    
4. Install libusb backend using homebrew::

    $ brew install libusb
    
5. Install Python dependancies using easy_install::

    $ sudo easy_install pyusb
    $ sudo easy_install pyqtgraph
    $ sudo easy_install configobj
    $ sudo easy_install pyserial
    
    
Getting ChipWhisperer
^^^^^^^^^^^^^^^^^^^^^

As in the Windows release, you can download a complete ChipWhisperer software release. Alternatively you can clone the
ChipWhisperer repository with git, which is very simple on Linux::

    $ git clone git://git.assembla.com/chipwhisperer.git
    $ cd chipwhisperer
    $ git clone git://git.assembla.com/openadc.git

Be aware that the git code may be broken, although we try not to commit completely untested code to the master branch. Either way
once you have the `chipwhisperer` directory somewhere, do the following from within that directory::

    $ cd software
    $ sudo python setup.py develop
    $ cd ../openadc/controlsw/python
    $ sudo python setup.py develop
    
Running ChipWhisperer
^^^^^^^^^^^^^^^^^^^^^

1. Running ChipWhisperer can be done from the command prompt::

    $ cd ~/chipwhisperer/software
    $ python CWCapture.pyw
    $ python CWAnalyzer.pyw
   
2. Be aware there is a problem with displaying the "CWFirmware Config" dialog on current releases. To fix this, type the following
   into the "Python Console" of the ChipWhisperer-Capture window::
   
     self.scope.scopetype.CWFirmwareConfig.show()

   You can then point the ChipWhisperer-Capture software to the firmware zip file for the required hardware. Once
   this is done you will not have to repeat the process, as the software remembers the location of the firmware file.

3. If toolbars are missing, they may have been hidden by default (another unknown issue on MAC OS X). To fix this simply right-click
   and confirm all toolbars are being shown:
   
    .. image:: /images/install_macosx_toolbars.png
    
    