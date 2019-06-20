# ChipWhisperer

[Wiki](https://wiki.newae.com/Main_Page) | [Knowledge Base](https://wiki.newae.com/Category:Knowledge_Base) | [Forum](http://www.newae.com/forum) | [Store](https://store.newae.com) | [NewAE](http://newae.com)

ChipWhisperer is an open source toolchain dedicated to hardware security research. This toolchain consists of several layers of open source components:
* __Hardware__: The ChipWhisperer uses a _capture_ board and a _target_ board. Schematics and PCB layouts for the ChipWhisperer-Lite capture board and a number of target boards are freely available.
* __Firmware__: Three separate pieces of firmware are used on the ChipWhisperer hardware. The capture board has a USB controller (in C) and an FPGA for high-speed captures (in Verilog) with open-source firmware. Also, the target device has its own firmware; this repository includes many firmware examples for different targets.
* __Software__: The ChipWhisperer software is made up of a _capture_ program to control the hardware and an _analyzer_ program to process captured data.

---

## ChipWhisperer 5.0 Is Coming

The biggest (and most breaking) change is coming. ChipWhisperer 5.0 uses Python 3 & Juypter Notebooks. Check out the [GITHub issue](https://github.com/newaetech/chipwhisperer/issues/177) to track this, which is in a new branch on the git repo. No major changes to the current branch will be done as all new features will be going into 5.0.

## Getting Started
First time using ChipWhisperer? This is the v5 development branch, which hasn't been officially released yet. If you're sure you want to try this branch out, check out these links:
* [Getting Started](https://wiki.newae.com/Getting_Started) if you have no idea where to start
* [CW5 Information](https://wiki.newae.com/CW5) for information about ChipWhisperer 5 and Jupyter.
* [Installing ChipWhisperer](https://wiki.newae.com/v5:Installing_ChipWhisperer) if you're trying to set up this software package

## GIT Source
Note all development occurs on the [develop](https://github.com/newaetech/chipwhisperer/tree/develop) branch. If you are looking for bleeding edge it's NOT on master - we push each release (and possibly any critical changes) to master. This means that "master" always gives you the latest known-working branch, but there may be new features on the "develop" branch.

## Help!
Stuck? If you need a hand, there are a few places you can ask for help:
* The [NewAE Forum](https://www.newae.com/forum/) is full of helpful people that can point you in the right direction
* If you find a bug, let us know through the [issue tracker](https://github.com/newaetech/chipwhisperer/issues)

---

ChipWhisperer is a trademark of NewAE Technology Inc., registered in the US, Europe, and China.
