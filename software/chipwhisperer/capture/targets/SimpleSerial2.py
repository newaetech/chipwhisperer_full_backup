import logging

from chipwhisperer.common.utils import util
from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite

class SimpleSerial2(TargetTemplate):
    "AHHHHHHHHH"
    _frame_byte = 0x00
    def __init__(self):
        TargetTemplate.__init__(self)
        self.ser = SimpleSerial_ChipWhispererLite()
        self._protver = 'auto'
        self.protformat = 'hex'
        self.last_key = bytearray(16)
        self._output_len = 16

    def _calc_crc(self, buf):
        crc = 0x00
        for b in buf:
            crc ^= b
            for _ in range(8):
                if crc & 0x80:
                    crc = (crc << 1) ^ 0xA6
                    crc &= 0xFF
                else:
                    crc <<= 1
                    crc &= 0xFF
        return crc

    def _stuff_data(self, buf):
        l = len(buf)
        ptr = 0
        last = 0
        for i in range(1, l):
            if (buf[i] == self._frame_byte):
                if (i != (l - 1)):
                    buf[last] = ptr - last
                last = i
        return buf

    def _unstuff_data(self, buf):
        n = buf[0]
        l = len(buf) - 1
        while n < l:
            tmp = buf[n]
            buf[n] = self._frame_byte
            n += tmp
        if n > l:
            return next
        return 0x00



    def con(self, scope=None):
        self.ser.con(scope)
        #self.write("xxxxxxxxxxxxxxxxxx")
        self.baud = 230400
        self.flush()

    def simpleserial_write(self, cmd, data, end='\n'):
        if cmd == 'p':
            self.send_cmd(0x01, 0x01, data)
        elif cmd == 'k':
            self.send_cmd(0x01, 0x02, data)

    def simpleserial_read(self, cmd=None, pay_len=None, end='\n', timeout=250, ack=True):
        recv_len = 3 + pay_len #cmd, len, data, crc
        response = self.read(recv_len, timeout=timeout)
        if cmd and response[0] != cmd:
            logging.warning(f"Unexpected start to command {cmd}")

        l = response[1]

        if not pay_len:
            # user didn't specify
            response.extend(self.read(l+1, timeout=timeout))
            pay_len = len(response) - 3
        if pay_len and l != pay_len:
            logging.warning(f"Unexpected length {l}")

        crc = self._calc_crc(response[:-1]) #calc crc for all bytes except last (crc)
        if crc != response[-1]:
            logging.warning(f"Invalid CRC. Expected {crc} got {response[-1]}")

        return bytearray(response[2:-1])

    def read(self, num_char = 0, timeout = 250):
        """ Reads data from the target over serial.

        Args:
            num_char (int, optional): Number of byte to read. If 0, read all
                data available. Defaults to 0.
            timeout (int, optional): How long in ms to wait before returning.
                If 0, block until data received. Defaults to 250.

        Returns:
            String of received data.

        .. versionadded:: 5.1
            Added target.read()
        """
        if num_char == 0:
            num_char = self.ser.inWaiting()
        return self.ser.read(num_char, timeout)

    def send_cmd(self, cmd, scmd, data):
        buf = [cmd, scmd]
        buf.extend(data)
        crc = self._calc_crc(buf)
        buf.append(crc)
        self.write(data)

    def write(self, data):
        self.ser.write(data)

    @property
    def baud(self):
        """The current baud rate of the serial connection.

        :Getter: Return the current baud rate.

        :Setter: Set a new baud rate. Valid baud rates are any integer in the
            range [500, 2000000].

        Raises:
            AttributeError: Target doesn't allow baud to be changed.
        """
        if hasattr(self.ser, 'baud') and callable(self.ser.baud):
            return self.ser.baud()
        else:
            raise AttributeError("Can't access baud rate")

    @baud.setter
    def baud(self, new_baud):
        if hasattr(self.ser, 'baud') and callable(self.ser.baud):
            self.ser.setBaud(new_baud)
        else:
            raise AttributeError("Can't access baud rate")

    def simpleserial_wait_ack(self, timeout=500):
        data = self.read(1, timeout=timeout)
        if len(data) < 1:
            logging.warning("Read timed out")
        if data[0] != 0x00:
            logging.warning(f"Simpleserial error {data[0]}")
        return data[0]

    def set_key(self, key, ack=True, timeout=250):
        """Checks if key is different than the last one sent. If so, send it.

        Uses simpleserial_write('k')

        Args:
            key (bytearray): key to send
            ack (bool, optional): Wait for ack after sending key. Defaults to
                True.
            timeout (int, optional): How long in ms to wait for the ack.
                Defaults to 250.

        Raises:
            Warning: Device did not ack or error during read.

        .. versionadded:: 5.1
            Added target.set_key()
        """
        if self.last_key != key:
            self.last_key = key
            self.simpleserial_write('k', key)
            if ack:
                if self.simpleserial_wait_ack(timeout) is None:
                    raise Warning("Device failed to ack")

    def in_waiting(self):
        """Returns the number of characters available from the serial buffer.

        Returns:
            The number of characters available via a target.read() call.

        .. versionadded:: 5.1
            Added target.in_waiting()
        """
        return self.ser.inWaiting()


    def flush(self):
        """Removes all data from the serial buffer.

        .. versionadded:: 5.1
            Added target.flush()
        """
        self.ser.flush()

    def in_waiting_tx(self):
        """Returns the number of characters waiting to be sent by the ChipWhisperer.

        Requires firmware version >= 0.2 for the CWLite/Nano and firmware version and
        firmware version >= 1.2 for the CWPro.

        Used internally to avoid overflowing the TX buffer, since CW version 5.3

        Returns:
            The number of characters waiting to be sent to the target

        .. versionadded:: 5.3.1
            Added public method for in_waiting_tx().
        """
        return self.ser.inWaitingTX()