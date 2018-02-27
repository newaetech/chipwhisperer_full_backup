from __future__ import print_function, division

import unittest
import time
import logging
import os
import subprocess

import numpy as np
from tqdm import tqdm
import matplotlib.pyplot as plt

import chipwhisperer as cw
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic
from chipwhisperer.tests.tools_for_tests import FIRMWARE_DIR, CAPTURE_SCRIPTS_DIR
from chipwhisperer.capture.api.programmers import XMEGAProgrammer
from chipwhisperer.capture.auxiliary.ResetCW1173Read import ResetCW1173



class TestTutorialB6BreakingAESManual(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.INFO)
        self.scope = cw.scope()
        self.target = cw.target(self.scope)

        self.HW = [bin(n).count("1") for n in range(0, 256)]

        self.sbox = (
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
            0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
            0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
            0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
            0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
            0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
            0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
            0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
            0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
            0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
            0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
            0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
            0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16)

    def tearDown(self):
        self.scope.dis()
        self.target.dis()

    def intermediate(self, pt, keyguess):
        return self.sbox[pt ^ keyguess]

    def test_tutorial_B6(self):
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
        textin = []
        N = 100 # Number of traces
        print('Capturing traces...')
        for i in tqdm(range(N)):
            key, text = ktp.newPair() # manual creation of a key, text pair can be substituted here
            textin.append(text)

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
                    logging.warning('Timeout happened during acquisition')
            except IOError as e:
                logging.error('IOError: %s' % str(e))

            # run aux stuff that should happen after trace here

            traces.append(scope.getLastTrace())

        knownkey = key
        pt = np.asarray(textin)
        traces = np.asarray(traces)
        numtraces = np.shape(traces)[0]
        numpoint = np.shape(traces)[1]

        bestguess = [0] * 16
        pge = [256] * 16
        print("Attacking subkeys...")
        for bnum in tqdm(range(0, 16)):
            cpaoutput = [0] * 256
            maxcpa = [0] * 256
            for kguess in range(0, 256):
                #print("Subkey %2d, hyp = %02x: " % (bnum, kguess), end='')

                # Initialize arrays &amp; variables to zero
                sumnum = np.zeros(numpoint)
                sumden1 = np.zeros(numpoint)
                sumden2 = np.zeros(numpoint)

                hyp = np.zeros(numtraces)
                for tnum in range(0, numtraces):
                    hyp[tnum] = self.HW[self.intermediate(pt[tnum][bnum], kguess)]

                # Mean of hypothesis
                meanh = np.mean(hyp, dtype=np.float64)

                # Mean of all points in trace
                meant = np.mean(traces, axis=0, dtype=np.float64)

                # For each trace, do the following
                for tnum in range(0, numtraces):
                    hdiff = (hyp[tnum] - meanh)
                    tdiff = traces[tnum, :] - meant

                    sumnum = sumnum + (hdiff * tdiff)
                    sumden1 = sumden1 + hdiff * hdiff
                    sumden2 = sumden2 + tdiff * tdiff

                cpaoutput[kguess] = sumnum / np.sqrt(sumden1 * sumden2)
                maxcpa[kguess] = max(abs(cpaoutput[kguess]))

                #print(maxcpa[kguess])

            bestguess[bnum] = np.argmax(maxcpa)

            cparefs = np.argsort(maxcpa)[::-1]

            # Find PGE
            pge[bnum] = list(cparefs).index(knownkey[bnum])

        print("Best Key Guess: ", end="")
        for b in bestguess: print("%02x " % b, end="")

        print("")
        print("PGE: ", end="")
        for b in pge: print("%02d " % b, end="")
        self.assertLess(sum(pge), 5)


class TestTutorialA2GlitchAttacks(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(level=logging.INFO)
        self.scope = cw.scope()
        self.target = cw.target(self.scope)

    def tearDown(self):
        self.scope.dis()
        self.target.dis()

    def test_tutorial_A2(self):
        # build the firmware
        self.glitch_simple_firmware_dir = os.path.join(FIRMWARE_DIR, "glitch-simple")

        # Build firmware
        logging.info('Building firmware from make file')
        call = ["make", "--directory", self.glitch_simple_firmware_dir, "PLATFORM=CW303"]
        logging.info(call)
        exit_code = subprocess.check_call(call)
        if exit_code == 0:
            logging.info('Finished building firmware')
        else:
            logging.error('Build Failed with exit code {}'.format(exit_code))

        # connect the scope and target
        scope = self.scope
        target = self.target

        # program the XMEGA with the built hex file
        xmega = XMEGAProgrammer()
        xmega.setUSBInterface(scope.scopetype.dev.xmega)
        xmega._logging = None
        xmega.find()
        xmega.erase()
        glitch_simple_hex = os.path.join(self.glitch_simple_firmware_dir, r"glitchsimple-CW303.hex")
        xmega.program(glitch_simple_hex, memtype="flash", verify=True)
        xmega.close()

        # setup parameters needed for glitch the XMEGA
        scope.glitch.clk_src = 'clkgen'

        scope.gain.gain = 45
        scope.adc.samples = 3000
        scope.adc.offset = 0
        scope.adc.basic_mode = "rising_edge"
        scope.clock.clkgen_freq = 7370000
        scope.clock.adc_src = "clkgen_x4"
        scope.trigger.triggers = "tio4"
        scope.io.tio1 = "serial_rx"
        scope.io.tio2 = "serial_tx"
        scope.io.hs2 = "glitch"

        target.go_cmd = ""
        target.key_cmd = ""
        target.output_cmd = "$GLITCH$"

        # setup aux modules for automatically resetting target
        # Delay between arming and resetting, in ms
        delay_ms = 1000

        # glitch cycle
        n = 200
        for i in range(n):
            target.reinit()

            # run aux stuff that should run before the scope arms here

            scope.arm()

            # run aux stuff that should run after the scope arms here

            target.go()
            timeout = 50
            # wait for target to finish
            while target.isDone() is False and timeout:
                timeout -= 1
                time.sleep(0.01)

            try:
                ret = scope.capture()
                if ret:
                    logging.warning('Timeout happened during acquisition')
            except IOError as e:
                logging.error('IOError: %s' % str(e))

            # run aux stuff that should happen after trace here
            self.fail('Finish this test')






