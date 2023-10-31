# ECC
This Vivado project implements the Cryptech ECC P256 point multiplier, and a few variants on that core.
What these cores are is covered in the [sca204 Jupyter
notebooks](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/courses/sca204/).

## Supported Platforms
Links are to the Vivado projects. You do not *need* to re-build these projects
as pre-built bitfiles are provided (but you can if you want). Note that
rebuilding (with a different Vivado version and/or implementation strategy)
may cause the leakage to shift around a bit in a way that makes the sca204
notebooks not work out of the box (the leakage should still be there but may
manifest itself differently).

Note that each target variant has its own Vivado project.  The Vivado
projects for the basic (unmodified) target are:
* [`CW305_100t`](vivado/cw305_ecc.xpr)
* [`CW305_35t`](vivado/cw305_ecc.xpr) (not all variants due to FPGA resource limitations)
* [`CW312_A35`](vivado/ss2_xc7a35_ecc.xpr) (not all variants due to FPGA resource limitations)

## Interface
The CW305 targets use the CW305 interface, while the CW312 target uses the
CW305/SS2 wrapper (see [here](../aes/README.md) for more information on these).
Both cases are handled transparently by
[CW305\_ECC\.py](https://github.com/newaetech/chipwhisperer/blob/develop/software/chipwhisperer/capture/targets/CW305_ECC.py).

## Simulation
The `sim/` directory contains a very basic Verilog testbench. It is not
meant for exhaustive verification; it is meant to help show how the core
functions.

