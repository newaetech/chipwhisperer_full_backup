# ecdsa256

## Core Description

This core implements the scalar base point multiplier for ECDSA curve P-256. It can be used during generation of public keys, the core can also be used as part of the signing operation. 

## API Specification

The core interface is similar to other Cryptech cores. FMC memory map looks like the following:

`0x0000 | NAME0`
`0x0004 | NAME1`
`0x0008 | VERSION`

`0x0020 | CONTROL`
`0x0024 | STATUS`

`0x0080 | K0`
`0x0084 | K1`
`...`
`0x009C | K7`
`0x00A0 | X0`
`0x00A4 | X1`
`...`
`0x00BC | X7`
`0x00C0 | Y0`
`0x00C4 | Y1`
`...`
`0x00DC | Y7`

The core has the following registers:

 * **NAME0**, **NAME1**
Read-only core name ("ecdsa256").

 * **VERSION**
Read-only core version, currently "0.11".

 * **CONTROL**
Control register bits:
[31:2] Don't care, always read as 0
[1] "next" control bit
[0] Don't care, always read as 0
The core starts multiplication when the "next" control bit changes from 0 to 1. This way when the bit is set, the core will only perform one multiplication and then stop. To start another operation, the bit must be cleared at first and then set to 1 again.

 * **STATUS**
Read-only status register bits:
[31:2] Don't care, always read as 0
[1] "valid" control bit
[0] "ready" control bit (always read as 1)
The "valid" control bit is cleared as soon as the core starts operation, and gets set after the multiplication operations is complete. Note, that unlike some other Cryptech cores, this core doesn't need any special initialization, so the "ready" control bit is simply hardwired to always read as 1. This is to keep general core interface consistency.

 * **K0**-**K7**
Buffer for the 256-bit multiplication factor (multiplier) K. The core will compute Q = K * G (the base point G is the multiplicand). K0 is the least significant 32-bit word of K, i.e. bits [31:0], while K7 is the most significant 32-bit word of K, i.e. bits [255:224].

 * **X0**-**X7**, **Y0**-**Y7**
Buffers for the 256-bit coordinates X and Y of the product Q = K * G. Values are returned in affine coordinates. X0 and Y0 contain the least significant 32-bit words, i.e. bits [31:0], while X7 and Y7 contain the most significant 32-bit words, i.e. bits [255:224].

## Implementation Details

The top-level core module contains block memory buffers for input and output operands and the base point multiplier, that reads from the input buffer and writes to the output buffers.

The base point multiplier itself consists of the following:
 * Buffers for storage of temporary values
 * Configurable "worker" unit
 * Microprograms for the worker unit
 * Multi-word mover unit
 * Modular inversion unit

The "worker" unit can execute five basic operations:
 * comparison
 * copying
 * modular addition
 * modular subtraction
 * modular multiplications
 
There are two primary microprograms, that the worker runs: curve point doubling and addition of curve point to the base point. Those microprograms use projective Jacobian coordinates, so one more microprogram is used to convert the product into affine coordinates with the help of modular inversion unit.

Note, that the core is supplemented by a reference model written in C, that has extensive comments describing tricky corners of the underlying math.

## Vendor-specific Primitives

Cryptech Alpha platform is based on Xilinx Artix-7 200T FPGA, so this core takes advantage of Xilinx-specific DSP slices to carry out math-intensive operations. All vendor-specific math primitives are placed under /rtl/lowlevel/artix7, the core also offers generic replacements under /rtl/lowlevel/generic, they can be used for simulation with 3rd party tools, that are not aware of Xilinx-specific stuff. Selection of vendor/generic primitives is done in ecdsa_lowlevel_settings.v, when porting to other architectures, only those four low-level modules need to be ported.


# NewAE Notes and Changes

Note that there are *two* different repositories named "ecdsa256" on
https://wiki.cryptech.is/wiki/GitRepositories.

The source code used here is from:
https://wiki.cryptech.is/browser/user/shatov/ecdsa256/?rev=180949c2d91d9e82a896c8d9f46f3b7541506f52

Some minor changes were made to the Verilog source for integration into the
NewAE CW305 platform. These can easily be found by grepping for XXX in the
source code:

- syntax changes so that iverilog can compile it
  (`rtl/modular/modular_reductor_256.v`)
- the ability to use an arbitrary point for point multiplication
  (`rtl/curve/curve_dbl_add_256.v`)

To run the point multiplication testbench, run `make` in the new sim
directory (requires iverilog).

There is also a testbench for the CW305-wrapped version of the point
multiplication core over
[here](../../../vivado_examples/ecc_p256_pmul/sim/).

