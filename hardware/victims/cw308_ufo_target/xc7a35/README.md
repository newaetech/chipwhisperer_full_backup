# CW312T-A35
This repository contains example FPGA design projects for the CW312T-A35
target. We show two types of projects: simple (for UART-based targets) and
advanced (for targets which use the CW305 SAM3U USB parallel interface).


## 1. Simple UART Targets: SimpleSerial AES
[ss\_aes.xpr](vivado/ss_aes.xpr) is a port of the AES project originally
written for our [Spartan6 UFO target](../spartan6lx9/). It understands basic
[SimpleSerial
v1.0](https://chipwhisperer.readthedocs.io/en/latest/simpleserial.html#simpleserial-v1-0)
commands and can be interacted with like our microprocessor-based targets.


## 2. Advanced CW305-style Targets: SimpleSerial v2.1 Wrapper
The 
[SS2 wrapper](https://github.com/newaetech/fpga-common/tree/main#notes-regarding-ss2-wrapper)
provides a more powerful mechanism to communicate with the target over UART,
and it allows for existing CW305 design to be easily ported to the CW312T-A35.
We've done just that with our [CW305 AES](vivado/ss2_cw305_aes.xpr), [CW305 AES
pipeline](vivado/ss2_cw305_aes_pipelined.xpr) and [CW305
ECC](vivado/ss2_cw305_ecc.xpr) projects.

Refer
[here](https://github.com/newaetech/fpga-common/tree/main#notes-regarding-ss2-wrapper)
for more information.

