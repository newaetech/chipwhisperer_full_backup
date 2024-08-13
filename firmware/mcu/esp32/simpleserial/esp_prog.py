from esptool import main
import chipwhisperer as cw
import sys


scope = cw.scope()

scope.default_setup()
scope.io.tio1 = None
scope.io.tio2 = None
scope.io.tio3 = 0
scope.io.pdic=0
scope.clock.clkgen_freq = 26E6
import time
scope.io.nrst = 0
time.sleep(0.2)
scope.io.nrst = None
time.sleep(0.2)

port = sys.argv[1]
name = sys.argv[0]
sys.argv = [name, '--chip', 'esp32', '--port', port, '--baud', '38400', '--before', 'default_reset', '--after', 'hard_reset', 'write_flash', '-z', 
'--flash_mode', 'dio', '--flash_freq', '40m', '--flash_size', 'detect', '0x1000', 'build/bootloader/bootloader.bin', '0x10000', 
'build/simpleserial.bin', '0x8000', 'build/partitions_singleapp.bin']

print(sys.argv)
main()