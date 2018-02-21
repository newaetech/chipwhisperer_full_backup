import unittest
import time
import logging
import sys

import numpy as np
import chipwhisperer as cw
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic
import matplotlib.pyplot as plt


class TestTutorialB1SimpleSerialProject(unittest.TestCase):

    def setUp(self):
        self.scope = cw.scope()
        self.target = cw.target(self.scope)
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)
        handler = logging.StreamHandler(sys.stdout)
        handler.setLevel(logging.DEBUG)
        formatter = logging.Formatter('%(name)s: %(levelname)s: %(message)s')
        handler.setFormatter(formatter)
        logger.addHandler(handler)

    def tearDown(self):
        self.scope.dis()
        self.target.dis()

    def test_Tutorial_B1_headless(self):
        scope = self.scope
        target = self.target

        # setup scope parameters
        scope.gain.gain = 45
        scope.adc.samples = 3000
        scope.adc.offset = 1250
        scope.adc.basic_mode = "rising_edge"
        scope.clock.clkgen_freq = 7370000
        scope.clock.adc_src = "clkgen_x4"
        scope.trigger.triggers = "tio4"
        scope.io.tio1 = "serial_rx"
        scope.io.tio2 = "serial_tx"
        scope.io.hs2 = "clkgen"

        ktp = AcqKeyTextPattern_Basic(target=target)

        traces = []
        N = 50 # Number of traces
        for i in range(N):
            key, text = ktp.newPair() # manual creation of a key, text pair can be substituted here

            target.reinit()

            target.setModeEncrypt() # only does something for targets that support it
            target.loadEncryptionKey(key)
            target.loadInput(text)

            # run aux stuff that should run before the scope arms here

            scope.arm()

            # run aux stuff that should run after the scope arms here

            target.go()
            timeout=50
            # wait for target to finish
            while target.isDone() is False and timeout:
                timeout -= 1
                time.sleep(0.01)

            try:
                ret = scope.capture()
                if ret:
                    logging.debug('Timeout happened during acquisition')
            except IOError as e:
                logging.error('IOError: %s' % str(e))

            # run aux stuff that should happen after trace here

            traces.append(scope.getLastTrace())

        trace_array = np.asarray(traces) # if you prefer to work with numpy array for number crunching

        # show an example trace
        plt.plot(traces[0])
        plt.show()

        # TODO: add analyzer part
