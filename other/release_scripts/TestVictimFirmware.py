#
#ChipWhisperer Release Test Script
#

import subprocess, os

def testRV(rv):
    if rv[0]:
        print "[OK]"
    else:
        print "[FAILED]"
        print "stdout: %s"%rv[1][0]
        print "stderr: %s"%rv[1][1]
        raise ValueError("Test Failed")

class TestVictimFirmware(object):

    def __init__(self):
        self.env = os.environ.copy()

        # For Windows
        self.env["PATH"] = self.env["PATH"] + r";C:\WinAVR-20100110\bin;C:\WinAVR-20100110\utils\bin"

        self.basePath = "../../hardware/victims/firmware/"

    def test(self):
        print "--Victim Firmware Build Test--"
        tests = 5

        print "  avr-serial-nocrypto            ",
        testRV(self.runMake('avr-serial-nocrypto', 'MCU=atmega328p'))

        print "  avr-serial with crypto         ",
        testRV(self.runMake('avr-serial', 'MCU=atmega328p'))

        print "  xmega-serial with crypto       ",
        #todo: fix this script so don't need this bogus argument
        testRV(self.runMake('xmega-serial', 'IGNORETHIS=22'))

    def runMake(self, path, args):
        self.runMakeLL(path, [args, "clean"])
        rv = self.runMakeLL(path, args)
        self.runMakeLL(path, [args, "clean"])
        return rv

    def runMakeLL(self, path, args):
        path = os.path.join(self.basePath, r'%s' % path)
        # print path
        vl = subprocess.Popen(["make", args], env=self.env, cwd=path, shell=True)
        data = vl.communicate()

        if (vl.returncode == 0):
            return (True, data)

        return (False, data)


if __name__ == '__main__':
    tvf = TestVictimFirmware()
    tvf.test()
