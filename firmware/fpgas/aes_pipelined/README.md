# AES-128 Pipeline
This Vivado project implements a few different pipelined AES cores. What
these cores are is covered in the companion 
[Jupyter notebook](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/CW305_AES_pipelined.ipynb).

## Supported Platforms
Links are to the Vivado projects. You do not *need* to re-build these projects
as pre-built bitfiles are provided (but you can if you want).

* [`CW305_100t`](vivado/cw305_aes_pipelined.xpr)
* [`CW305_35t`](vivado/cw305_aes_pipelined.xpr)
* [`CW312_A35`](vivado/s2_xc7a35_aes_pipelined.xpr)

## Interface
The CW305 targets use the CW305 interface, while the CW312 target uses the
CW305/SS2 wrapper (see [here](../aes/README.md) for more information on these).
Both cases are handled transparently by
[CW305\.py](https://github.com/newaetech/chipwhisperer/blob/develop/software/chipwhisperer/capture/targets/CW305.py).

## Simulation
The `sim/` directory contains a very basic Verilog testbench. It is not
meant for exhaustive verification; it is meant to help show how the core
functions.

## Implementation Notes
The Vivado project has four design runs, one for each setting of the
`HALF_PIPE` parameter (0,1,2,3), but the `HALF_PIPE` setting is not a property
of the design run: when choosing a design run, you must also manually set the
`HALF_PIPE` value (in "settings", under the Project Manager tab).

Additionally, `A35` must be defined for the cw305\_35t target (but not the
CW312T\_A35). This is used to halve the FIFO depths, to account for the reduced
BRAM resources.

## Netlist 
`cw305_aes_pipelined_half2_netlist.v` is the synthesized netlist of the
`HALF_PIPE=2` implementation, provided here for convenience as it is
discussed in the companion Jupyter notebook.

