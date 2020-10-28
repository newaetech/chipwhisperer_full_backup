import logging
import time
import chipwhisperer as cw

from chipwhisperer.common.utils import util
from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite

class SimpleSerial2(TargetTemplate):
    "AHHHHHHHHH"
    _frame_byte = 0x00
    def __init__(self):
        TargetTemplate.__init__(self)
        self.ser = SimpleSerial_ChipWhispererLite()
        self.ser._baud = 230400
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
                buf[last] = i - last
                last = i
        return buf

    def _unstuff_data(self, buf):
        if len(buf) == 0:
            return 0x00
        n = buf[0]
        buf[0] = 0
        l = len(buf) - 1
        while n < l:
            tmp = buf[n]
            buf[n] = self._frame_byte
            n += tmp
            if (n == 0) and (tmp == 0):
                logging.error("Infinite loop in unstuff data")
                return
        if n > l:
            return n
        return 0x00

    def con(self, scope=None, flush_on_err=True):
        self.ser.con(scope)
        self._flush_on_err = flush_on_err
        self.reset_comms()
        #self.baud = 230400
        self.flush()

    def simpleserial_write(self, cmd, data, end='\n'):
        if data == []:
            data = [0x00] #cannot send 0 length stuff
        if cmd == 'p':
            self.send_cmd(0x01, 0x01, data)
        elif cmd == 'k':
            self.send_cmd(0x01, 0x02, data)
        else:
            self.send_cmd(cmd, 0x00, data)

    def simpleserial_read(self, cmd=None, pay_len=None, end='\n', timeout=250, ack=True):
        rtn = self.read_cmd(cmd, pay_len, timeout)
        if not rtn:
            return None
        else:
            rtn = rtn[3:-2]
        try:
            if ack:
                self.simpleserial_wait_ack(timeout)
        except:
            pass
        return bytearray(rtn)
    
    def is_done(self):
        return True

    def flush_on_error(self):
        if self._flush_on_err:
            self.reset_comms()
            time.sleep(0.05)
            self.flush()

    def simpleserial_wait_ack(self, timeout=500):
        rtn = self.read_cmd('e')
        if not rtn:
            logging.warning(f"Device did not ack")
            return
        if rtn[3] != 0x00:
            logging.warning(f"Device reported error {hex(rtn[3])}")
            self.flush_on_error()
            print(bytearray(rtn))
        return rtn


    # very ugly since we're decoding stuff as we read it back
    # need to decode bytearray to give raw serial back
    # TODO: Improve this
    def simpleserial_read_witherrors(self, cmd=None, pay_len=None, end='\n',\
                                    timeout=250, glitch_timeout=1000, ack=True):

        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        if pay_len is None:
            recv_len = 3
        else:
            recv_len = 5 + pay_len #cmd, len, data, crc
        response = self.read(recv_len, timeout=timeout)


        if response is None or len(response) < recv_len:
            # got nothing or less than requested back
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        response = bytearray(response.encode('latin-1'))
        if self._frame_byte in response and len(response) == 3:
            # invalid response
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}


        next_frame = self._unstuff_data(response)
        if cmd and response[1] != cmd:
            logging.warning(f"Unexpected start to command {response[1]}")

        l = response[2]

        if not pay_len:
            # user didn't specify, do second read based on sent length
            x = self.read(l+2, timeout=timeout)
            if x is None:
                print("Read timed out")
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
            if len(x) != (l + 2):
                print(f"Didn't get all data {len(x)}, {l+2}")
                print(bytearray(x.encode('latin-1')))
                print(response)
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

            response.extend(bytearray(x.encode('latin-1')))
            pay_len = len(response) - 5

            # need to do second unstuff since we read stuff after last one
            if self._frame_byte in response[3:-1]:
                #logging.warning(f"Unexpected frame byte in {response}")
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
            resp_cpy = response[next_frame:]
            self._unstuff_data(resp_cpy)
            response[next_frame:] = resp_cpy[:]
        if pay_len and l != pay_len:
            logging.warning(f"Unexpected length {l}, {pay_len}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        crc = self._calc_crc(response[1:-2]) #calc crc for all bytes except last (crc)
        if crc != response[-2]:
            logging.warning(f"Invalid CRC. Expected {crc} got {response[-2]}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        if response[-1] != self._frame_byte:
            logging.warning(f"Did not receive end of frame, got {response[-1]}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
        
        try:
            rv = self.simpleserial_wait_ack()
            if rv is None:
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
        except:
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        return {'valid': True, 'payload': bytearray(response[3:-2]), 'full_response': response, 'rv': rv}

    def read_cmd(self, cmd=None, pay_len=None, timeout=250, flush_on_err=None):
        """Read a simpleserial-v2 command
        """
        tmp = None
        if not flush_on_err is None:
            tmp = self._flush_on_err
            self._flush_on_err = flush_on_err
        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        if pay_len is None:
            recv_len = 3
        else:
            recv_len = 5 + pay_len #cmd, len, data, crc
        response = self.read(recv_len, timeout=timeout)
        if response is None or len(response) < recv_len:
            self.flush_on_error()
            print("Read timed out" + response)
            return

        response = bytearray(response.encode('latin-1'))
        if self._frame_byte in response and len(response) == 3:
            logging.warning(f"Unexpected frame byte in {response}")
            self.flush_on_error()
            return
        next_frame = self._unstuff_data(response)
        if cmd and response[1] != cmd:
            logging.warning(f"Unexpected start to command {response[1]}")

        l = response[2]

        if not pay_len:
            # user didn't specify, do second read based on sent length
            x = self.read(l+2, timeout=timeout)
            if x is None:
                print("Read timed out")
                self.flush_on_error()
                return
            if len(x) != (l + 2):
                print(f"Didn't get all data {len(x)}, {l+2}")
                print(bytearray(x.encode('latin-1')))
                print(response)
            response.extend(bytearray(x.encode('latin-1')))
            pay_len = len(response) - 5

            # need to do second unstuff since we read stuff after last one
            if self._frame_byte in response[3:-1]:
                logging.warning(f"Unexpected frame byte in {response}")
                self.flush_on_error()
            resp_cpy = response[next_frame:]
            self._unstuff_data(resp_cpy)
            response[next_frame:] = resp_cpy[:]
        if pay_len and l != pay_len:
            logging.warning(f"Unexpected length {l}, {pay_len}")
            self.flush_on_error()
            return

        crc = self._calc_crc(response[1:-2]) #calc crc for all bytes except last (crc)
        if crc != response[-2]:
            logging.warning(f"Invalid CRC. Expected {crc} got {response[-2]}")

        if response[-1] != self._frame_byte:
            logging.warning(f"Did not receive end of frame, got {response[-1]}")

        if not flush_on_err is None:
            self._flush_on_err = tmp

        return response

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
        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        buf = [0x00, cmd, scmd, len(data)]
        buf.extend(data)
        crc = self._calc_crc(buf[1:])
        buf.append(crc)
        buf.append(0x00)
        buf = self._stuff_data(buf)
        self.write(buf)
        #print(bytearray(buf))
        #print(bytearray(self._unstuff_data(buf)))

    def reset_comms(self):
        self.write([0x00]*10) # make sure target not processing a command

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