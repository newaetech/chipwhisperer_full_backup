# AES-128 Pipeline

This Vivado project implements a few different pipelined AES cores. What
these cores are is covered in the companion 
[Jupyter notebook](https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/CW305_AES_pipelined.ipynb).

## Simulation
The `sim/` directory contains a very basic Verilog testbench. It is not
meant for exhaustive verification; it is meant to help show how the core
functions.

## Implementation Notes

The Vivado project has four design runs, one for each setting of the
`HALF_PIPE` parameter (0,1,2,3), but the `HALF_PIPE` setting is not a property
of the design run: when choosing a design run, you must also manually set the
`HALF_PIPE` value (in "settings", under the Project Manager tab).

Additionally, `A35` must be defined for the A35 targets. This is used to
halve the FIFO depths, to account for the reduced BRAM resources.

## Netlist 

`aes128_pipelined_half2_netlist.v` is the synthesized netlist of the
`HALF_PIPE=2` implementation, provided here for convenience as it is
discussed in the companion Jupyter notebook.

