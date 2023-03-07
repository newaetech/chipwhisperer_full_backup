# Run with pytest -v -rs

import chipwhisperer as cw
import time
import random
import os
from tqdm import tqdm

scope = cw.scope()
scope.default_setup()

bs = '../vivado/ss2.runs/impl_1/ss2_wrapper.bit'
target = cw.target(scope, cw.targets.CW305, bsfile=bs, platform='ss2')
target.baud = 230400

target.REG_BUILDTIME = 0xaabbcc
print('\n*** FPGA buildtime: %s ***\n' % target.get_fpga_buildtime())

for i in tqdm(range(25), desc='Testing flashing LED3 for 5 seconds'):
    target.fpga_write(0, [1])
    assert target.fpga_read(0,1)[0] == 1
    time.sleep(0.1)

    target.fpga_write(0, [0])
    assert target.fpga_read(0,1)[0] == 0
    time.sleep(0.1)


for i in tqdm(range(200), desc='testing IO3/IO4 toggling'):
    io34 = random.randint(0,3)
    target.fpga_write(0x0034, [io34])
    rdata = target.fpga_read(0x0034, 1)[0]
    assert rdata == io34, "Wrote %d, read %d" % (io34, rdata)


for i in tqdm(range(200), desc='testing echo packets'):
    target._ss2_test_echo()


registers = [{'name': 'reg1', 'len': 1,  'address': 0x01},
             {'name': 'reg2', 'len': 4,  'address': 0x10},
             {'name': 'reg3', 'len': 16, 'address': 0x112233},
             {'name': 'reg4', 'len': 16, 'address': 0x45678a},
            ]

for i in tqdm(range(1000), desc='testing register reads and writes'):
    reg = random.choice(registers)
    address = reg['address']
    reglen = reg['len']
    name = reg['name']
    wdata = []
    for b in range(reglen):
        wdata.append(random.randint(0,255))
    target.fpga_write(address, wdata)
    rdata = target.fpga_read(address, reglen)
    assert rdata == wdata, "Wrote %s, read %s" % (wdata, rdata)



