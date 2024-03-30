# N76E003 target firmware for chipwhisperer
This contains the platform firmware to be loaded onto the CW308_N76E003 target.

## Requirements
- SDCC
- makebin
- packihx

## Building
In the target directory (e.g. simpleserial-glitch, simpleserial-aes, etc), run:
    `make SS_VER=SS_VER_2_1`

### Caveats
Only SS_VER_2_1 is supported when using a crypto target. This is due to the N76E003 having a very small amount of memory, and the SS_VER_1_1 firmware won't fit the memory model for most targets.