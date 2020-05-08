================ Silicon Labs Peripheral Library ============================

This directory, "EMLIB", contains the Silicon Labs Peripheral Support
library for the EFM32, EZR32 and EFR32 microcontrollers and System-On-Chip
devices.

Some design guidelines for this library:

* Follow the guidelines established by ARM's and Silicon Labs's adaptation
  of the CMSIS (see below) standard.

* Be usable as a starting point for developing richer, more target specific
  functionality (i.e. copy and modify further).

* Ability to be used as a standalone software component, to be used by other
  drivers, that should cover "the most common cases".

* Readability of the code and usability preferred before optimization for speed
  and size or covering a particular "narrow" purpose.

* As little "cross-dependency" between modules as possible, to enable users to
  pick and choose what they want.

================ About CMSIS ================================================

These APIs are based on CMSIS Device header file structure.

As a result of this, the library requires basic C99-support. You might have
to enable C99 support in your compiler. Comments are in doxygen compatible
format.

The CMSIS Device library contains all peripheral module registers and bit field
descriptors.

For more information about CMSIS see
    http://www.onarm.com
    http://www.arm.com/products/CPUs/CMSIS.html

The requirements for using CMSIS also apply to this package.

================ File structure ==============================================

inc/ - header files
src/ - source files

================ Software documentation and updates ==========================

Silicon Labs continually works to provide updated and improved example code,
header files, other software and documentation. Please check

http://www.silabs.com/support/pages/document-library.aspx?p=MCUs--32-bit

or Simplicity Studio for the latest content.

If you download and install the Simplicity Studio package, you can easily
keep up-to-date with the latest Silicon Labs software releases and 
documentation.

To download Simplicity Studio, please visit
https://www.silabs.com/products/development-tools/software/simplicity-studio

==============================================================================

