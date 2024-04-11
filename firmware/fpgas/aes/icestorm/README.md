Rebuilding the FPGA bitfile requires
[nextpnr](https://github.com/YosysHQ/nextpnr) and its prerequisites, with iCE40
support.

Then simply run `make` to create the bitfile (or use the one provided here).
Set `TOP=ss_ice40_aes_top` to build the SimpleSerial interface variant, or
`TOP=ss2_ice40_aes_top` to build the CW305/SS2 interface variant.

In both cases, the target must be clocked at 7.37 MHz via the HS2 pin.

To program the target via ChipWhisperer:
```python
from chipwhisperer.hardware.naeusb.programmer_targetfpga import LatticeICE40
fpga = LatticeICE40(scope)
fpga.erase_and_init()
fpga.program('ss2_ice40_aes_top.bin', sck_speed=20e6, start=True, use_fast_usb=False)
```

```python
target_type = cw.targets.SimpleSerial
target = cw.target(scope, target_type)
```

CW308 LED2 is a "clock alive" indicator (it should blink slowly).

Example encryption for the SimpleSerial variant:
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

For the CW305/SS2 variant, use the normal CW305 AES notebook, with
`TARGET_PLATFORM = 'CW312T_ICE40'`.

