# AES-128
This target implements a basic 1-round/cycle AES core.
Target usage for the CW305 interface (and CW305/SS2 interface) is covered in
[this
notebook](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/PA_HW_CW305_1-Attacking_AES_on_an_FPGA.ipynb).

With the SimpleSerial interface, target usage is the same as software AES
targets (e.g. [sca101 AES
notebooks](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/courses/sca101/Lab%203_3%20-%20DPA%20on%20Firmware%20Implementation%20of%20AES%20(MAIN).ipynb)), but with `ack=False` added to `capture_trace()` calls.


## Supported Platforms
Links are to the implementation files (i.e. Vivado projects, ISE projects,
icestorm makefiles). You do not *need* to re-build these projects as pre-built
bitfiles are provided (but you can if you want).
* [`CW305_100t`](vivado/cw305_aes.xpr)
* [`CW305_35t`](vivado/cw305_aes.xpr)
* `CW312_A35` ([SimpleSerial](vivado/ss_xc7a35_aes.xpr) or [CW305/SS2](vivado/ss2_xc7a35_aes.xpr) wrapper)
* `CW312_iCE40` ([SimpleSerial or CW305/SS2 wrapper](icestorm/README.md))
* `CW308_S6LX9` ([SimpleSerial interface](ise/ss_aes_ise.xise))

## Interfaces
While all of the above platforms use [the same AES
implementation](../cryptosrc/aes_googlevault), they have different
interfaces.

### CW305
The CW305 targets use the parallel USB interface which allow direct
read/write access to the target FPGA registers, using
[CW305\.py](https://github.com/newaetech/chipwhisperer/blob/develop/software/chipwhisperer/capture/targets/CW305.py).
This is the interface with the highest throughput and the lowest FPGA
overhead; it is only available on the CW305 target board because it requires
more FPGA pins and a supporting SAM3U processor.

### SimpleSerial
The non-CW305 targets support a SimpleSerial interface which accepts the `k`
and `p` SimpleSerial commands, just like our microprocessor-based targets, and
so must be talked to as such, i.e. **not** with the [CW305 AES demo notebook](
https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/PA_HW_CW305_1-Attacking_AES_on_an_FPGA.ipynb).
This interface is included to show that it can be done; however the SS2 wrapper
is preferred (unless you can't afford the additional resources that it
requires).

This interface has a fixed baud rate that is 38400 bps when the target is
clocked at 7.37 MHz.

### CW305/SS2 Wrapper
The `CW312_A35` and `CW312_iCE40` targets also support an "SS2 wrapper"
interface which allows the target to appear like a native CW305 target, by
encapsulating arbitrary CW305-style register reads and writes inside the SS2
protocol.  This adds overhead to the target, but also flexibility: it makes it
trivially easy to add/modify target registers, and if you use
[CW305\.py](https://github.com/newaetech/chipwhisperer/blob/develop/software/chipwhisperer/capture/targets/CW305.py),
then the same Python-side target code can be used for both native CW305
targets and SS2-wrapped targets (as we do in all the CW305 notebooks in our
[jupyter
repository](https://github.com/newaetech/chipwhisperer-jupyter/tree/master)).

This interface has a fixed baud rate that is 230400 bps when the target is
clocked at 7.37 MHz. See 
[here](https://github.com/newaetech/fpga-common/tree/main#notes-regarding-ss2-wrapper)
for more information on the SS2 wrapper.


## Simulation
The `sim/` directory contains a very basic Verilog testbench (using only the
CW305 interface). It is not meant for exhaustive verification; it is meant
to help show how the core functions.

