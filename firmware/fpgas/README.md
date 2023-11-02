# ChipWhisperer FPGA Targets
Here you'll find all of the ChipWhisperer target FPGA material.
(This material used to be in various places under `hardware/victims/`; we've
cleaned up and consolidated everything here.)

Here are the targets you'll find (follow the link for more
information on each target):
* [AES](aes/README.md) (the original target, now with several variants)
* [Pipelined AES](aes_pipelined/README.md)
* [ECC](ecc/README.md)

<!---
* [CW305 SPI flash feedthrough](spiflash_feedthrough/README.md)
-->

In addition to the source files, there are pre-built bitfiles (as well as
everything you'll need to re-build the bitfiles yourself).


## Soft Cores
We've also brought up several soft-cores on our target boards; source code and
instuctions for building these are found elsewhere, but we've collected
pointers to that here:
* [neorv32 on CW312T\_iCE40](https://rtfm.newae.com/Targets/UFO%20Targets/CW312T-ICE40UP/#neorv32-soft-core-processor)
* [lowRISC Ibex on CW305 and CW312T\_A35](https://rtfm.newae.com/Targets/UFO%20Targets/CW312T-XC7A35T/#1-ibex)
* [Arm DesignStart on CW305 and
  CW312T\_A35](https://github.com/newaetech/CW305-Arm-DesignStart), with or
  without built-in 
  [TraceWhisperer capability](https://github.com/newaetech/DesignStartTrace)

Note that pre-built bitfiles for these *are* included in the chipwhisperer
repository (except for DesignStart, which you must build yourself using our
[detailed instructions]((https://github.com/newaetech/CW305-Arm-DesignStart)).


## Target Boards
In general we aim to provide builds of each project for each of our target
boards (if the project fits!):
* [CW305](https://rtfm.newae.com/Targets/CW305%20Artix%20FPGA/) (35t and
  100t variants)
* [CW312T\_A35](https://rtfm.newae.com/Targets/UFO%20Targets/CW312T-XC7A35T/)
* [CW312T\_iCE40](https://rtfm.newae.com/Targets/UFO%20Targets/CW312T-ICE40UP/)
* [CW308T-S6LX9](https://rtfm.newae.com/Targets/UFO%20Targets/CW308T-S6LX9/)


|project          | CW305-100t | CW305-35t | CW312T\_A35 | CW312T\_iCE40 | CW308T\_S6LX9 |
|-----------------|------------|-----------|-------------|---------------|---------------|
|AES              | ✅         | ✅        | ✅          | ✅            | ✅            |
|pipelined AES    | ✅         | ✅        | ✅          | ❌            | ❌            |
|ECC              | ✅         | partly    | partly      | ❌            | ❌            |
|neorv32          | ❌         | ❌        | ❌          | ✅            | ❌            |
|Ibex             | ✅         | ✅        | ✅          | ❌            | ❌            |
|DesignStart      | ✅         | ✅        | ✅          | ❌            | ❌            |
|DesignStartTrace | ✅         | ✅        | ✅          | ❌            | ❌            |


## The Big Targets
Projects for the [CW310](https://github.com/newaetech/cw310-bergen-board)
and [CW340](https://github.com/newaetech/cw340-luna-board) target boards are
found in their own respective repositories.


## OpenTitan
The [OpenTitan](https://opentitan.org/) project uses NewAE FPGA target boards;
refer to 
[their documentation](https://opentitan.org/book/doc/getting_started/setup_fpga.html)
to learn more.


