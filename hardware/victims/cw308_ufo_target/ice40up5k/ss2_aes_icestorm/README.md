Implements the same AES engine as [ss\_aes\_icestorm](../ss\_aes\_icestorm),
but uses the SS2 wrapper described [here](../../xc7a35/README.md) in order
to provide CW305-style register reads and writes.

This makes the iCE40 target look and feel like a CW305 target! (minus all the
extras found on the CW305 board, like PLLs, etc...). So you can do things like
`target.fpga_write(address, data)` and `target.fpga_read(address, bytes)` for
arbitrary addresses and number of bytes, and it all gets automagically
carried over UART and translated into reads and writes, thanks to the [SS2
wrapper](https://github.com/newaetech/fpga-common/tree/main#notes-regarding-ss2-wrapper).

For example usage, including programming the target and communicating with
it, refer to the [CW305 AES
notebook](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/PA_HW_CW305_1-Attacking_AES_on_an_FPGA.ipynb).
The target is intended to be clocked at 7.37 MHz (via HS2); all of the setup
is provided in the notebook.

Note that the overhead of the SS2 wrapper is not insignificant on this very
small FPGA: the example AES target provided here uses 96% of the iCE40's
logic cells (compared to 76% for the [more basic AES
target](../ss_aes_icestorm)).  It's all about tradeoffs... with the SS2
wrapper it's really easy to add registers or port existing CW305 designs;
but if you need that extra 20% logic cells, then do something along [these
lines](../ss\_aes\_icestorm) (or roll your own).

To rebuild the bitfile, install
[nextpnr](https://github.com/YosysHQ/nextpnr) and its prerequisites, with
iCE40 support; then `make bitfile`.

LED1 is a "clock alive" indicator (LED3 on CW308 version).

LED2 has dual roles: it is connected to both GPIO4 and an internal SS2 error
detector. If it remains on, outside of captures, it indicates that an error
occured in the SS2 communications protocol; recovery from this usually
requires a reset, which on this target means re-programming the bitfile. If
it flashed quickly during captures, then it's just the trigger line and
there is no cause for concern.

