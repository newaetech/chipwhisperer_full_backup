# ChipWhisperer

[![Documentation Status](https://readthedocs.org/projects/chipwhisperer/badge/?version=latest)](https://chipwhisperer.readthedocs.io/en/latest/?badge=latest) | [![Notebook Tests](https://github.com/newaetech/ChipWhisperer-Test-Results/blob/main/.github/badges/hardware_tests.svg)](https://github.com/newaetech/ChipWhisperer-Test-Results/blob/main/tutorials/results.yaml) | [![Discord](https://img.shields.io/discord/747196318044258365?logo=discord)](https://discord.gg/chipwhisperer)

## Important Links

[Hardware Docs](https://rtfm.newae.com) | [Software Docs](https://chipwhisperer.readthedocs.io) | [Tutorials](https://github.com/newaetech/chipwhisperer-jupyter/tree/main) | [Forum](http://forum.newae.com) | [Store](https://store.newae.com) | [NewAE](http://newae.com)

## ChipWhisperer 6.0: Repo Changes and Cleanup

ChipWhisperer 6.0 is bringing many changes to the ChipWhisperer repository. The biggest one is a cleanup of the git history on both this repo and
chipwhisperer-jupyter, as well as moving some files out of these repositories. We've done our best to make everything most users will want
on this repo, as well as to backup everything else to other repos.

For more information, please check out our [writeup on this update](https://docs.google.com/document/d/1sAhfBboymKDQyOE_BurCmwsh7hb_Qiw8SW3kCu9ZGrg/edit?usp=sharing).

## What is ChipWhisperer?

ChipWhisperer is an open source toolchain dedicated to hardware security research. This toolchain consists of several layers of open source components:
* __Hardware__: The ChipWhisperer uses a _capture_ board and a _target_ board. Schematics and PCB layouts for the ChipWhisperer-Lite capture board and a number of target boards are freely available.
* __Firmware__: Three separate pieces of firmware are used on the ChipWhisperer hardware. The capture board has a USB controller (in C) and an FPGA for high-speed captures (in Verilog) with open-source firmware. Also, the target device has its own firmware; this repository includes many firmware examples for different targets.
* __Software__: The ChipWhisperer software includes a Python API for talking to ChipWhisperer hardware (ChipWhisperer Capture) and a Python API 
for processing power traces from ChipWhisperer hardware (ChipWhisperer Analyzer). You can find documentation for our Python API at
chipwhisperer.readthedocs.io

## Getting Started
First time using ChipWhisperer? Check out these links:
* [Getting started](https://chipwhisperer.readthedocs.io/en/latest/getting-started.html)
* [Installing ChipWhisperer](https://chipwhisperer.readthedocs.io/en/latest/index.html#install) if you're trying to set up this software package
* [Hardware Docs](https://rtfm.newae.com) for hardware information. Also has helpful comparison between different NewAE products.

## GIT Source
Note all development occurs on the [develop](https://github.com/newaetech/chipwhisperer/tree/develop) branch. If you are looking for bleeding edge it's NOT on master - we push each release (and possibly any critical changes) to master. This means that "master" always gives you the latest known-working branch, but there may be new features on the "develop" branch.

## Help!
Stuck? If you need a hand, there are a few places you can ask for help:
* The [NewAE Forum](https://forum.newae.com/) is full of helpful people that can point you in the right direction
* If you find a bug, let us know through the [issue tracker](https://github.com/newaetech/chipwhisperer/issues)

---

ChipWhisperer is a trademark of NewAE Technology Inc., registered in the US, Europe, and China.

