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

""" TODO-note, to use SS2:
SS2 = ss2()
stuffed = SS2._stuff_data([1,2,3,0,1])
ret = SS2._unstuff_data(stuffed)
"""

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
        #  initialize all DUT input values:
        self.dut.reset.value = 0
        self.dut.rxd.value = 1


    async def reset(self):
        await ClockCycles(self.dut.clk, 10)
        self.dut.reset.value = 1
        await ClockCycles(self.dut.clk, 4)
        self.dut.reset.value = 0

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
        await ClockCycles(self.dut.clk, 10) # to give time for fifo_watch errors to be seen

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


class SS2Test(object):
    def __init__(self, dut, harness, reps, baud, max_len):
        self.dut = dut
        self.harness = harness
        self.name = 'SS2 test'
        self.baud = baud
        self.reps = reps
        self.max_len = max_len
        self.min_len = 4    # TODO: this should depend on the command being sent
        self.data_bits = 8
        self.uart_rx_queue = Queue()
        self.uncobbed_rx_queue = Queue()
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
        # ensure last word is received:
        await ClockCycles(self.dut.clk, self.harness.bit_clocks*5)
        if self.dut.U_dut.error.value:
            self.harness.inc_error()
            self.dut._log.error("DUT reported internal error")
        if not self.uart_rx_queue.empty():
            self.harness.inc_error()
            self.dut._log.error("UART Rx queue still contains %d words!" % self.uart_rx_queue.qsize())
        if not self.uncobbed_rx_queue.empty():
            self.harness.inc_error()
            self.dut._log.error("Uncobbed Rx queue still contains %d words!" % self.uncobbed_rx_queue.qsize())
        if self.errors:
            self.dut._log.error("%6s test done, failed with %d errors" % (self.name, self.errors))
        else:
            self.dut._log.info("%6s test done: passed!" % self.name)

    async def _run(self) -> None:
        self.dut._log.debug('_run starting')
        await ClockCycles(self.dut.clk, 30)
        for i in range(self.reps):
            msg = []
            # command: (TODO: illegal commands)
            msg.append(35)
            # subcommand:
            msg.append(0x57) # write (TODO: randomize subcommand, including illegal subcommands)
            # dlen:
            dlen = random.randint(self.min_len, self.max_len)
            msg.append(dlen)
            # address:
            assert dlen >= 4
            # TODO: if there are only a few addresses, need to ensure that non-zero bytes here are properly digested by ss2.v
            #address = random.randint(2**8, 2**32-1)
            #address = 0x11223300
            address = 0x00001000
            msg.extend(list(int.to_bytes(address, length=4, byteorder='little')))
            self.dut._log.info("address: %08x" % address)

            # data:
            for j in range(dlen-4):
                #  higher incidence of zeros
                if random.randint(0,2) == 0:
                    msg.append(0)
                else:
                    msg.append(random.randint(0,(2**self.data_bits)-1))
            #msg[4] = 0; # TODO: temp to test zeros!
            #msg[5] = 0; # TODO: temp to test zeros!
            #msg[6] = 0; # TODO: temp to test zeros!
            # CRC:
            crc = self.harness.SS2._calc_crc(msg)
            self.dut._log.info("CRC byte: %02x (msg = %s)" % (crc, msg))
            msg.append(crc)
            # send it! testbench queue gets the unstuffed message (for verification), UART sends the stuffed version:
            raw_string = ''
            for m in msg:
                raw_string += '%02x ' % m
                self.uncobbed_rx_queue.put_nowait(m)
            self.dut._log.info("Raw message:     %s" % raw_string)
            # _stuff_data requires an extra 0 at the start and an extra 0 at the end:
            msg.insert(0, 0)
            msg.append(0)
            stuffed_msg = self.harness.SS2._stuff_data(msg)
            stuffed_string = ''
            for m in stuffed_msg:
                stuffed_string += '%02x ' % m
                self.uart_rx_queue.put_nowait(m)
                await self._uart_drv.send(m)
                #await ClockCycles(self.dut.clk, self.harness.bit_clocks*10)
            self.dut._log.info("Stuffed message: %s" % stuffed_string)
            #self.dut._log.info("Sending stuffed message: %s" % stuffed_msg)

    async def _check_uart_rx_thread(self) -> None:
        self.dut._log.debug('%6s _check_uart_rx_thread starting' % self.name)
        i = 0
        while self.uart_rx_queue.empty():
            await ClockCycles(self.dut.clk, 1)
        while True:
            await self.wait_signal(self.dut.U_dut.rx_data_ready, 1, self.dut.clk)
            rdata = self.dut.U_dut.rx_data_byte.value
            self.dut._log.debug("UART Rx rep %d:  got raw %02x" % (i, rdata))
            edata = self.uart_rx_queue.get_nowait()
            if rdata != edata:
                self.harness.inc_error()
                self.dut._log.error("UART Rx rep %d: Expected %4x, got %4x" % (i, edata, rdata))
            else:
                self.dut._log.debug("UART Rx rep %d: Expected %4x, got %4x" % (i, edata, rdata))
            await self.wait_signal(self.dut.U_dut.rx_data_ready, 0, self.dut.clk)
            i += 1


    async def _check_rx_uncobbed(self) -> None:
        self.dut._log.debug('%6s _check_rx_uncobbed starting' % self.name)
        i = 0
        while self.uncobbed_rx_queue.empty():
            await ClockCycles(self.dut.clk, 1)
        while True:
            await self.wait_signal(self.dut.U_dut.rx_valid_byte, 1, self.dut.clk)
            rdata = self.dut.U_dut.rx_data.value
            self.dut._log.info("Uncobbed Rx rep %d:  got raw %02x" % (i, rdata))

            edata = self.uncobbed_rx_queue.get_nowait()
            if rdata != edata:
                self.harness.inc_error()
                self.dut._log.error("Uncobbed Rx rep %d: Expected %4x, got %4x" % (i, edata, rdata))
            else:
                self.dut._log.debug("Uncobbed Rx rep %d: Expected %4x, got %4x" % (i, edata, rdata))
            await self.wait_signal(self.dut.U_dut.rx_data_ready, 0, self.dut.clk)
            i += 1

            await self.wait_signal(self.dut.U_dut.rx_valid_byte, 0, self.dut.clk)

    #@staticmethod - hmm, looks like staticmethod + async don't play well together?
    async def wait_signal(self, signal, value, clock):
        while signal != value:
            await ClockCycles(clock, 1)


@cocotb.test(timeout_time=100000, timeout_unit="us")
#@cocotb.test(skip=True)
async def ss2_test(dut):
    reps  = int(os.getenv('REPS', '2'))
    max_len = int(os.getenv('MAX_LEN', '100'))
    period = int(os.getenv('PERIOD', '100'))

    bit_rate = dut.U_dut.pBIT_RATE.value + 1 # pBIT_RATE is fixed in this design
    baud = int(1/(period * 1e-9 * bit_rate))
    harness = Harness(dut, reps, period, bit_rate)
    await harness.reset()

    ss2test = SS2Test(dut, harness, reps, baud, max_len)

    harness.register_test(ss2test)
    harness.start_tests()
    await harness.all_tests_done()
    assert harness.errors == 0

