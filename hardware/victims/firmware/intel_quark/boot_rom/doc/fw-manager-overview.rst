Quark Firmware Management
#########################

.. contents::

Overview
********

The *fw-manager* module of the Quark Bootloader enables the Firmware Management
(FM) mode, an alternative mode to the normal operation mode whereby the device
is ready to serve system management requests from an external host.  For the
whole duration of FM mode, the MCU does not run its application and only serves
management requests from an external host.

FM mode provides the following services to the users:

* System information retrieval (SoC type, application version, security
  capability, etc.)
* Application firmware update
* Application firmware erase (erase of all the application code in flash)

The MCU enters FM mode only after an explicit request from the host. The
mechanism used by the host to signal an Enter-FM request to the device depends
on the specific communication link used for firmware management. In case of UART
the host (or the user) must ground a specific pin and reset the device manually.

A special case is when there is no application running on the device (i.e.,
there is no valid application firmware). In such a case, the bootloader puts
the device into sleep mode.

When the host has completed its firmware management tasks, it can make the
device exit FM mode by issuing a reset request. The device reboots and enters
application mode or sleep mode if no application is present.

Protocol stack
**************

FM functionality has been designed to support different kinds of communication
links. To this end, a single common protocol stack for performing FM operations
has been defined. The core of such a stack is the (USB) `DFU protocol`_, a
standard for performing firmware upgrades on USB devices.

In order to use DFU also on UART (and other non-USB transports), a generic
adaptation layer for non USB-transports, the *Quark DFU Adaptation (QDA)
protocol*, has been defined. Moreover, on top of DFU we defined a common device
management protocol, the *Quark Firmware Management (QFM) protocol*, and a common
firmware image format, the *Quark Firmware Update (QFU) format*. The QFM
protocol enables the use of DFU also to perform firmware management operations
different from firmware updates (i.e., system information retrieval and
application erase), while the QFU format defines a firmware image format that
can support authentication while maintaining compatibility with plain DFU
(i.e., a signed firmware can potentially be downloaded and authenticated by the
device using any standard DFU tool).

The current FM protocol stack is the following.

+-----------------+-------------------+-------------+
|   Layer         |        USB        |     UART    |
+=================+===================+=============+
| **DFU payload** |    QFM Protocol / QFU Image     |
+-----------------+-------------------+-------------+
| **DFU flavor**  |      USB/DFU      |    QDA      |
+-----------------+-------------------+-------------+
| **Transport**   |        USB        | XMODEM-CRC  |
+-----------------+-------------------+-------------+
| **Driver**      | USB Device Driver | UART driver |
+-----------------+-------------------+-------------+

As shown in the table, XMODEM-CRC_ is used on top of UART. The reason is to
provide QDA with a reliable packet-based transport layer.

USB/DFU
=======

DFU is a generic firmware upgrade protocol which essentially enables a host to
transfer/retrieve data (typically firmware images) to/from a compliant device.
Specifically, data is transferred using DFU_DNLOAD transfers and is retrieved
using DFU_UPLOAD transfers. Each transfer is block-based and involve the
exchange of multiple DFU messages (see the `DFU Specification`_ for
additional information).

DFU relies on USB features such as descriptors, interfaces and interface
alternate settings.  Specifically, the DFU specification defines a DFU
Functional descriptor and a DFU interface.

The DFU descriptor contains information about the specific DFU functionality
offered by the device, such as the maximum block size it supports. The host is
required to retrieve the DFU Functional Descriptor (and the USB Device
Descriptor) before attempting any DFU operations (the information in the Device
Descriptor, i.e., Product ID and Device ID, is used to check the device type).

The DFU interface is the USB interface handling DFU requests. While in DFU
mode, the device must provide only the DFU interface. However, such an
interface may have multiple Alternate Settings. Specifically, the DFU standard
suggests that, in case of devices with multiple flash partitions, the DFU
interface can feature multiple alternate settings each one associate with a
specific partition. In such a way, the partition to be programmed (or read) by
the subsequent DFU_DNLOAD (or DFU_UPLOAD) operation can be selected by setting
the corresponding alternate settings.

Quark Bootloader extends this approach by also providing a "special alternative
setting" associated with "extended device management" functionality. Indeed,
when the Extended-FM alternate setting (i.e., alternate setting 0) is selected,
the device expects DFU_DNLOAD and DFU_UPLOAD transfers to carry QFM packets.

QDA - Quark DFU Adaptation
==========================

The QDA protocol defines a set of request/response messages used to replicate
the DFU functionality on top of any message-based communication layers.
Specifically, the functions provided by the QDA protocol are the following:

* Allow the host to get DFU descriptors, including the list of alternative
  settings
* Allow the host to set the active alternative setting (for the DFU interface)
* Allow the host to send a “USB reset” event to the device
* Enable all the DFU requests:
	- DFU_DETACH (ask the device to switch into DFU/FM mode)
	- DFU_DNLOAD (transfer a block of data from the host to the device)
	- DFU_UPLOAD (transfer a block of data from the device to the host)
	- DFU_GETSTATUS (state transition)
	- DFU_CLRSTATUS (exit from error)
	- DFU_ABORT (abort current download/upload)
	- DFU_GETSTATE (get device state)

.. _XMODEM-CRC: https://en.wikipedia.org/wiki/XMODEM
.. _dfu-spec: http://www.usb.org/developers/docs/devclass_docs/DFU_1.1.pdf
.. _`DFU protocol`: dfu-spec_
.. _`DFU Specification`: dfu-spec_
