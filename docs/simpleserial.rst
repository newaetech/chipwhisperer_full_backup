Simpleserial Documenetation
===========================

Simpleserial v1.1
-----------------

All communication is initiated by the capture board. The capture board
begins communication by sending a packet:

``[cmd, data_0, data_1, ..., data_n, '\n']``

where ``cmd`` is an ASCII character indicating the desired command and
``data`` is an ASCII representation of the data (i.e. ``0xAA`` will be
``'A'``, ``'A'``) The length of data is fixed by the firmware.

If the target board needs to respond with data, it sends back:

``['r', data_0, data_1, ..., data_n, '\n']``

Again, ``data_n`` is an ASCII representation of the data.

Whether or not the device sends back data, the device will send back an
ack to tell the capture board that it is done its operation:

``['z', status, '\n']``

Where status is an 8-bit binary number indicating the result/status of
the operation. For example, most functions use 0x00 to indicate success.

The default baud rate is 38400bps.

SimpleSerial v1.0
~~~~~~~~~~~~~~~~~

The same as SimpleSerial v1.1, except the ack packet is not included.

Variable Length Commands
~~~~~~~~~~~~~~~~~~~~~~~~

Variable length commands have been added, primarily to support masked
AES. Variable length commands are decided by firmware. In the case of a
variable length command, the send command will instead be:

``[cmd, len, data_0, data_1, ..., data_n, '\n']``

SimpleSerial v2.0
-----------------

All communication is initiated by the capture board. The capture board
begins communication by sending a packet:

``[cmd, scmd, dlen, data_0, data_1, ..., data_n, CRC (poly=0xA6)]``

cmd indicates the desired command, scmd indicates a subcommand, dlen is
the length of the data, data is binary data, and the crc is a CRC of
polynomial 0xA6 of the entire packet (from ``cmd`` to ``data_n``). All
bytes in the packet may take any 8-bit binary value, except dlen, which
is limited to less than 250. Therefore, only 249 bytes can be sent per
packet.

If the target board needs to respond with data, it sends back:

``[cmd, dlen, data_0, data_1, ..., data_n, CRC (poly=0xA6)]``

Whether or not the device sends back data, the device will send back an
error packet to tell the capture board that it is done its operation:

``['e', 0x01, error, CRC (poly=0xA6)]``

Before being sent, all packets are `Consistant Overhad Byte Stuffed
(COBS) <https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing>`__
and null terminated:

``[overhead_byte, [packet], 0x00]``.

The following standard errors are used:

::

    0x00 - OK
    0x01 - Invalid command
    0x02 - Bad CRC
    0x03 - Timeout
    0x04 - Invalid length
    0x05 - Unexpected frame byte (0x00)
    0x06 - Reserved
    ...

    0x0F - Reserved

Commands may use additional error codes to indicate the result of an
operation.

The default baud rate is 230400bps.
