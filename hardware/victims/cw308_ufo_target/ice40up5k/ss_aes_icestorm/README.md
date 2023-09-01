Rebuilding the FPGA bitfile requires
[nextpnr](https://github.com/YosysHQ/nextpnr) and its prerequisites, with iCE40
support.

Then, `make bitfile` to create the bitfile (or use the one provided here).

To program the target via ChipWhisperer:
```python
from chipwhisperer.hardware.naeusb.programmer_targetfpga import LatticeICE40
fpga = LatticeICE40(scope)
fpga.erase_and_init()
fpga.program('iCE40UP5K_SS_example.bin', sck_speed=20e6, start=True, use_fast_usb=False)
```

The target uses SimpleSerial v1 to communicate, 38400 baud at 7.37 MHz like our
microprocessor targets. The target must be clocked via HS2. The target
implements only the "k", "p" and "r" commands.

```python
target_type = cw.targets.SimpleSerial
target = cw.target(scope, target_type)
```

CW308 LED2 is a "clock alive" indicator (it should blink slowly).

Example encryption:
```python
ktp = cw.ktp.Basic()
key, text = ktp.next()
target.simpleserial_write('k', key)
target.simpleserial_write('p', text)
time.sleep(0.1)
r = target.simpleserial_read('r', target.output_len, ack=False)

# verify the result:
from Crypto.Cipher import AES
cipher = AES.new(key, AES.MODE_ECB)
assert bytearray(cipher.encrypt(text)) == r
```

or simply:
```python
ret = cw.capture_trace(scope, target, text, key, ack=False)
```

