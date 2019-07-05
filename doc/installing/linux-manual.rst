.. _sec-install-linux:

*********
GNU/Linux
*********

There is only manual install available on GNU/Linux. 

Manual Install
--------------

Python
^^^^^^

On Linux installing Python and the the associated package is straightforward.
Typically, you can install them from the package manager (ie. apt-get, 
yum, ...).

On Ubuntu or similar:

.. code:: bash

    sudo apt install python3 python3-pip


Compilers
^^^^^^^^^

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
^^^^^^^^^^^^^^^^

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
-------------

You are now ready to move on to :ref:`sec-install-repo`.