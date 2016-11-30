sysupdate
#########

Scripts
*******

Scripts are provided for creating DFU (Device Firmware Update) images
`qm_make_dfu.py` and managing a device `qm_manage.py`. These scripts
can be used as they are or as a template to create costumed scripts.

qm_make_dfu_
============

To upgrade a device with new firmware, the compiled binary needs to be converted
to a QM (Quark Microcontroller) compatible DFU file before downloading to the
device. The script `qm_make_dfu.py` provides this functionality. The script
takes as input a binary file and a target partition. You can also optionally
provide a configuration file specifying additional metadata (e.g., Vendor Id and
Product Id). The resulting file is named after the input file with a `.dfu`
extension or specified by argument. The generated dfu image file can be
downloaded to a device using `dfu-util-qda` or the make build system
(*make flash*).

qm_manage_
==========

qm_manage can be used to retrieve device information and erase application data.
The argument `erase` or `info` and the serial port `-p` need to be provided.
This script is using `dfu-util-qda` binary to communicate with the device.

qmfmlib
*******

The `qmfmlib` library supports the host side features of Quark Microcontroller
sysupdate. It is used by `qm_make_dfu.py` and `qm_manage.py`.

Installation
************

setup.py can be used to install the python qmfmlib to your local python
packages. If you do not have root access add the *--user* option to install the
library in your user specific installation path.

Installation is not needed if the scripts are not moved to another path as
qmfmlib is located in the same folder.

.. code::

    python setup.py install
