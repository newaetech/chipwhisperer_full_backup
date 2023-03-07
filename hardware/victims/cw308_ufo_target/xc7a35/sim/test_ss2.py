import cocotb
from cocotbext_uart import UARTParity
from cocotbext_uart import UARTConfig
from cocotbext_uart import UARTModule
from cocotbext_uart import UARTSignals
from cocotb.triggers import RisingEdge, ClockCycles, Join, Lock, Event
from cocotb.clock import Clock
from cocotb.queue import Queue
from cocotb.handle import Force, Release
from cocotb.log import SimLogFormatter
#from cocotb.regression import TestFactory
import chipwhisperer.capture.targets.SimpleSerial2 as ss2
import random
import math
import logging
import os

# Note: this could also be place in individual test functions by replacing root_logger by dut._log.
root_logger = logging.getLogger()
logfile = os.getenv('LOGFILE', 'make.log')
print('Logfile = %s' % logfile)
fh = logging.FileHandler(logfile, 'w')
fh.setFormatter(SimLogFormatter())
root_logger.addHandler(fh)

class Harness(object):
    def __init__(self, dut, reps, period, bit_rate):
        self.dut = dut
        self.reps = reps
        self.tests = []
        self.errors = 0
        self.period = period
        # Actual seed is obtained only if RANDOM_SEED is defined on vvp command line (otherwise you get 0)
        # regress.py always specifies the seed so this is fine.
        self.dut._log.info("seed: %d" % int(os.getenv('RANDOM_SEED', '0')))
        self.clock = Clock(dut.clk, self.period, units="ns")
        clk_thread = cocotb.start_soon(self.clock.start())
        self.bit_clocks = bit_rate
        self.SS2 = ss2()
        # these have to line up with what's in the DUT:
        self.registers = [{'name': 'reg1', 'len': 1,  'address': 0x0100,     'value': [0xa0]},
                          {'name': 'reg2', 'len': 4,  'address': 0x1000,     'value': [0x12, 0x34, 0x56, 0x78][::-1]},
                          {'name': 'reg3', 'len': 16, 'address': 0x11223300, 'value': [0x98, 0x76, 0x54, 0x32, 0x12, 0x34, 0x56, 0x78, 0x09, 0x87, 0x65, 0x43, 0x21, 0x23, 0x45, 0x67][::-1]},
                          {'name': 'reg4', 'len': 16, 'address': 0x45678a00, 'value': [0xab, 0xcd, 0xef, 0xed, 0xcb, 0xa0, 0x98, 0x76, 0x54, 0x32, 0x10, 0x12, 0x34, 0x56, 0x78, 0x9a][::-1]},
                         ]
        #  initialize all DUT input values:
        self.dut.resetn.value = 1
        self.dut.rxd.value = 1


    async def reset(self):
        await ClockCycles(self.dut.clk, 10)
        self.dut.resetn.value = 0
        await ClockCycles(self.dut.clk, 4)
        self.dut.resetn.value = 1

    def register_test(self, test):
        """ Add to list of running tests, so that we can later wait for all of
        them to complete via all_tests_done().
        """
        self.tests.append(test)

    async def all_tests_done(self):
        """ Wait for all tests which were registered via register_test() to finish.
        """
        for test in self.tests:
            await test.done()

    def start_tests(self):
        """ Wait for all tests which were registered via register_test() to finish.
        """
        for test in self.tests:
            test.start()

    def inc_error(self):
        self.errors += 1
        self.dut.errors.value = self.errors

    @staticmethod
    def hexstring(string, max_chars=24):
        """ Convenience function to put a string onto the simulation waveform."""
        data = 0
        for i,j in enumerate(string[:max_chars]):
            data += (ord(j) << ((max_chars-1-i)*8))
        return data

    @staticmethod
    def hexlist(declist):
        """ Convenience function, takes a list of integers and returns a string of the list elements in hex format"""
        hlist = ''
        for b in declist:
            hlist += '%02x ' % b
        return hlist


class SS2Test(object):
    def __init__(self, dut, harness, reps, baud):
        self.dut = dut
        self.harness = harness
        self.name = 'SS2 test'
        self.baud = baud
        self.reps = reps
        self.data_bits = 8
        self.uart_rx_queue = Queue()
        self.uncobbed_rx_queue = Queue()
        self.command_queue = Queue()
        self._uart_config = UARTConfig(baud=baud, 
                                       parity=UARTParity.NONE,
                                       bits=8,
                                       stopbits=1)
        self._uartsig = UARTSignals(tx = dut.rxd,
                                    rx = dut.txd,
                                    ctsn = None,
                                    rtsn = None)
        self._uart_drv = UARTModule(config=self._uart_config,
                        signals=self._uartsig,
                        clk=self.harness.clock)
        self._coro = None
        self.errors = 0

    def start(self) -> None:
        """Start test thread"""
        if self._coro is not None:
            raise RuntimeError("Capture already started")
        self._coro = cocotb.start_soon(self._run())
        self._ccoro = cocotb.start_soon(self._check_uart_rx_thread())
        self._ccoro = cocotb.start_soon(self._check_rx_uncobbed())
        self._ccoro = cocotb.start_soon(self._check_response())

    def stop(self) -> None:
        """Stop test thread"""
        if self._coro is None:
            raise RuntimeError("Capture never started")
        self._coro.kill()
        self._coro = None

    def running(self) -> bool:
        if self._coro is None or self._coro.done():
            return False
        else:
            return True

    async def done(self) -> None:
        """ wait for _run() to complete """
        await Join(self._coro)
        # wait for all responses to be processed:
        while not self.command_queue.empty():
            await ClockCycles(self.dut.clk, 10)
        if self.dut.U_dut.ss2_error.value:
            self.harness.inc_error()
            self.dut._log.error("DUT reported internal error")
        # check all queues are empty:
        if not self.uart_rx_queue.empty():
            self.harness.inc_error()
            self.dut._log.error("UART Rx queue still contains %d words!" % self.uart_rx_queue.qsize())
        if not self.uncobbed_rx_queue.empty():
            self.harness.inc_error()
            self.dut._log.error("Uncobbed Rx queue still contains %d words!" % self.uncobbed_rx_queue.qsize())
        if not self.command_queue.empty():
            self.harness.inc_error()
            self.dut._log.error("Command queue still contains %d words!" % self.command_queue.qsize())
        if self.errors:
            self.dut._log.error("%6s test done, failed with %d errors" % (self.name, self.errors))
        else:
            self.dut._log.info("%6s test done: passed!" % self.name)

    async def _run(self) -> None:
        self.dut._log.debug('_run starting')
        await ClockCycles(self.dut.clk, 30)
        for i in range(self.reps):
            # wait for previous command to get processed and responded:
            while not self.command_queue.empty():
                await ClockCycles(self.dut.clk, 10)
            # sometimes add extra frame delimiters:
            if not random.randint(0,4):
                await ClockCycles(self.dut.clk, random.randint(0,100))
                self.uart_rx_queue.put_nowait(0)
                await self._uart_drv.send(0)
                self.uart_rx_queue.put_nowait(0)
                await self._uart_drv.send(0)
                await ClockCycles(self.dut.clk, random.randint(0,100))


            # command: (TODO: illegal commands)
            command = random.choices(['read', 'write', 'echo'], weights=[2,2,1])[0]
            msg = []
            msg.append(35)
            # subcommand:
            if command == 'echo':
                msg.append(0x45)
                dlen = 4
                msg.append(dlen)
                payload = []
                for b in range(4):
                    #  higher incidence of zeros:
                    if random.randint(0,3) == 0:
                        payload.append(0)
                    else:
                        payload.append(random.randint(0,255))
                msg.extend(payload)
                self.dut._log.info("Msg %d: echo command, payload: %s" % (i, self.harness.hexlist(payload)))
                # define expected responses:
                expected_data = [0x00, 0x23, 0x04]
                expected_data.extend(payload)
                expected_crc = self.harness.SS2._calc_crc(expected_data[1:]) # omit leading zero, CRC, trailing zero
                expected_data.extend([expected_crc, 0x00])
                self.command_queue.put_nowait(expected_data)
                expected_data = [0x00, 0x65, 0x01, 0x00]
                expected_crc = self.harness.SS2._calc_crc(expected_data[1:]) # omit leading zero, CRC, trailing zero
                expected_data.extend([expected_crc, 0x00])
                self.command_queue.put_nowait(expected_data)

            elif command in ['read', 'write']:
                reg = random.choice(self.harness.registers)
                address = reg['address']
                reglen = reg['len']
                name = reg['name']
                if command == 'read':
                    msg.append(0x52)
                    dlen = 5
                    msg.append(dlen)
                    msg.extend(list(int.to_bytes(address, length=4, byteorder='little')))
                    msg.append(reglen)
                    self.dut._log.info("Msg %d: read command to register %s: %0d bytes; address: %06x" % (i, name, reglen, address))
                    # define expected responses:
                    expected_data = [0x00, 0x23, reglen]
                    expected_data.extend(reg['value'])
                    expected_crc = self.harness.SS2._calc_crc(expected_data[1:]) # omit leading zero, CRC, trailing zero
                    expected_data.extend([expected_crc, 0x00])
                    self.command_queue.put_nowait(expected_data)
                    expected_data = [0x00, 0x65, 0x01, 0x00]
                    expected_crc = self.harness.SS2._calc_crc(expected_data[1:]) # omit leading zero, CRC, trailing zero
                    expected_data.extend([expected_crc, 0x00])
                    self.command_queue.put_nowait(expected_data)

                elif command == 'write':
                    msg.append(0x57)
                    dlen = 4 + reglen
                    msg.append(dlen)
                    msg.extend(list(int.to_bytes(address, length=4, byteorder='little')))
                    writeval = []
                    for b in range(reglen):
                        #  higher incidence of zeros:
                        if random.randint(0,3) == 0:
                            writeval.append(0)
                        else:
                            writeval.append(random.randint(0,255))
                    msg.extend(writeval)
                    self.dut._log.info("Msg %d: write command to register %s: %0d bytes; address: %06x; write data: %s" % (i, name, reglen, address, self.harness.hexlist(writeval)))
                    reg['value'] = writeval
                    # define expected response:
                    expected_data = [0x00, 0x65, 0x01, 0x00]
                    expected_crc = self.harness.SS2._calc_crc(expected_data[1:]) # omit leading zero, CRC, trailing zero
                    expected_data.extend([expected_crc, 0x00])
                    self.command_queue.put_nowait(expected_data)

            else:
                raise ValueError

            crc = self.harness.SS2._calc_crc(msg)
            self.dut._log.info("Msg %d CRC byte: %02x (msg = %s)" % (i, crc, self.harness.hexlist(msg)))
            msg.append(crc)
            # send it! testbench queue gets the unstuffed message (for verification), UART sends the stuffed version:
            for m in msg:
                self.uncobbed_rx_queue.put_nowait(m)
            self.dut._log.info("Raw message %d:     %s" % (i, self.harness.hexlist(msg)))
            # _stuff_data requires an extra 0 at the start and an extra 0 at the end:
            msg.insert(0, 0)
            msg.append(0)
            stuffed_msg = self.harness.SS2._stuff_data(msg)
            for m in stuffed_msg:
                self.uart_rx_queue.put_nowait(m)
                await self._uart_drv.send(m)
                #await ClockCycles(self.dut.clk, self.harness.bit_clocks*10)
            self.dut._log.info("Stuffed message %d: %s" % (i, self.harness.hexlist(stuffed_msg)))
            #self.dut._log.info("Sending stuffed message: %s" % stuffed_msg)


    async def _check_response(self) -> None:
        """ This checks the response(s) sent back by SS2 on its UART Tx."""
        i = 0
        while True:
            ret = 1
            resp = []
            while ret:
                ret = await self._uart_drv.wait_for_recv()
                self.dut._log.debug("Tx got: %02x" % ret)
                resp.append(ret)
            self.harness.SS2._unstuff_data(resp)
            self.dut._log.info("Received response %d: %s" % (i, self.harness.hexlist(resp)))
            # check CRC:
            expected_crc = self.harness.SS2._calc_crc(resp[1:-2]) # omit leading zero, CRC, trailing zero
            if expected_crc != resp[-2]:
                self.harness.inc_error()
                self.dut._log.error('Msg %d response bad CRC: got %02x, calculated %02x' % (i, resp[-2], expected_crc))
            else:
                self.dut._log.info('Msg %d response good CRC (%02x)' % (i, expected_crc))
            if resp[-1]:
                self.harness.inc_error()
                self.dut._log.error('Msg %d bad/missing frame delimiter (%02x)' % (i, resp[-1]))
            else:
                self.dut._log.info('Msg %d: good frame delimiter' % i)
            # check full message:
            expected_data = self.command_queue.get_nowait()
            self.dut._log.info("EXPECTED READ RESPONSE PACKET: %s" % self.harness.hexlist(expected_data))
            self.dut._log.info("ACTUAL   READ RESPONSE PACKET: %s" % self.harness.hexlist(resp))

            for j,e,a in zip(range(len(expected_data)), expected_data, resp):
                if e != a:
                    self.harness.inc_error()
                    self.dut._log.error('Msg %d payload byte %d error: expected %02x, got %02x' % (i, j, e, a))
            i += 1

    async def _check_uart_rx_thread(self) -> None:
        """ This peaks internally to the SS2's Rx UART to see if it's receiving the bytes we sent."""
        self.dut._log.debug('%6s _check_uart_rx_thread starting' % self.name)
        i = 0
        while self.uart_rx_queue.empty():
            await ClockCycles(self.dut.clk, 1)
        while True:
            await self.wait_signal(self.dut.U_dut.U_ss2.rx_data_ready, 1, self.dut.clk)
            rdata = self.dut.U_dut.U_ss2.rx_data_byte.value
            self.dut._log.debug("UART Rx msg %d:  got raw %02x" % (i, rdata))
            edata = self.uart_rx_queue.get_nowait()
            if rdata != edata:
                self.harness.inc_error()
                self.dut._log.error("UART Rx msg %d: Expected %4x, got %4x" % (i, edata, rdata))
            else:
                self.dut._log.debug("UART Rx msg %d: Expected %4x, got %4x" % (i, edata, rdata))
            await self.wait_signal(self.dut.U_dut.U_ss2.rx_data_ready, 0, self.dut.clk)
            i += 1


    async def _check_rx_uncobbed(self) -> None:
        """ This peaks inside SS2 to see if the message sent is correctly assembled after
        removing the COBS encoding."""
        self.dut._log.debug('%6s _check_rx_uncobbed starting' % self.name)
        i = 0
        while self.uncobbed_rx_queue.empty():
            await ClockCycles(self.dut.clk, 1)
        while True:
            await self.wait_signal(self.dut.U_dut.U_ss2.rx_valid_byte, 1, self.dut.clk)
            rdata = self.dut.U_dut.U_ss2.rx_data.value
            self.dut._log.debug("Uncobbed Rx msg %d:  got raw %02x" % (i, rdata))

            edata = self.uncobbed_rx_queue.get_nowait()
            if rdata != edata:
                self.harness.inc_error()
                self.dut._log.error("Uncobbed Rx msg %d: Expected %4x, got %4x" % (i, edata, rdata))
            else:
                self.dut._log.debug("Uncobbed Rx msg %d: Expected %4x, got %4x" % (i, edata, rdata))
            await self.wait_signal(self.dut.U_dut.U_ss2.rx_data_ready, 0, self.dut.clk)
            i += 1

            await self.wait_signal(self.dut.U_dut.U_ss2.rx_valid_byte, 0, self.dut.clk)

    #@staticmethod - hmm, looks like staticmethod + async don't play well together?
    async def wait_signal(self, signal, value, clock):
        while signal != value:
            await ClockCycles(clock, 1)


@cocotb.test(timeout_time=100000, timeout_unit="us")
#@cocotb.test(skip=True)
async def ss2_test(dut):
    reps  = int(os.getenv('REPS', '2'))
    period = int(os.getenv('PERIOD', '100'))

    bit_rate = dut.U_dut.U_ss2.pBIT_RATE.value + 1 # pBIT_RATE is fixed in this design
    baud = int(1/(period * 1e-9 * bit_rate))
    harness = Harness(dut, reps, period, bit_rate)
    await harness.reset()

    ss2test = SS2Test(dut, harness, reps, baud)

    harness.register_test(ss2test)
    harness.start_tests()
    await harness.all_tests_done()
    assert harness.errors == 0

