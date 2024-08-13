Dependencies for Intel® Quark™ Microcontroller Software Interface
#################################################################

Overview
********

Intel® Quark™ Microcontroller Software Interface (QMSI) requires
external packages to be installed. This document describes the required packages
and other optional dependencies.

Toolchain
*********

  ISSM (Intel System Studio Microcontrollers) Toolchain is used for building
  QMSI. These two compilers are essential:

  * i586-intel-elfiamcu 5.2.1
  * arc-elf32 4.8.5

Optional build dependencies
***************************

* Tinycrypt-0.2.0
* doxygen
* texlive-latex-base
* texlive-latex-extra
* g++-multilib
* gcc-multilib
* IPP (Intel Performance Primitives) library

Development and Debugging packages
**********************************

* python2.7
* Terminal client
* clang-format-3.6
* ruby
* GNU make
* Git for version control
* Telnet client / GDB for debugging
