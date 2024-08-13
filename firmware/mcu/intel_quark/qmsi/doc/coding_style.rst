Good QMSI coding practice
#########################

Overview
********

This page provides details on good coding rules and practices for the QMSI
project.

.. contents::


Documentation
*************

Source files use C style comments. Extensive use of comments and a minimum of
5:1 lines of code to comment ratio is encouraged.

::

        /* This is a comment. */

Quark documentation is generated from the source files using Doxygen. Doxygen
compliant comments must be present in all public exposed functions and placed
in header files.

More info about documentation can be found in the
`Doxygen documentation <doxygen_style.rst>`_

Coding Conventions
******************

This is a list of the different conventions and recommendations for all
Quark Microcontroller code.

* ISO 9899:1990 standard (a.k.a. C90) compliant code.
* Use C standard data types, as defined in <stdint.h>.
* #define expressions are enclosed in parenthesis.
* K&R indent style.
* Use hard tabulation for indentation (8 characters-wide tabs).
* Enclose all statement blocks in curly braces (including one liners).
* Lines must not be longer than 80 characters.
* Use #if (CONDITION) expression rather than #ifdef (SYMBOL)

Preprocessor Macros
*******************

Public preprocessor macro identifiers follow these rules:

* All capital letters.
* Identifiers are prefixed with ‘QM\_’

Model specific code in the Quark Microcontroller family is enabled using
conditional preprocessor macros using this pattern: QM_<MODEL> (i.e. QM_Z1000).
The value shall be set to TRUE for a build on a specific target. The macro will
not be defined if built for an alternative target.

Example:

::

  #define QM_Z1000 (0)       /* This is disabled */
  /* #define QM_Z1000 (0)*/  /* This is disabled as well */
  #define QM_Z1000 (1)       /* This is enabled */

  #if (QM_Z1000)
  /* Quark Z1000 only code. */
  #endif

Where a number of SKUs are defined for a product, the Family SKU define shall
use XXX as wildcard in the name. For example:

::

  #if (QM_Z1XXX)
  /* Quark Z1XXX – All devices in the Z1*** family of devices. */
  #endif

While these definitions are provided, frugal use is suggested, and separation
of code into separate C files is preferred.

#defines which represent register map addresses shall have the suffix _BASE

Identifiers
***********

These are the rules for naming identifiers:

* All capital names identify constants and enum labels.
* Function names are lowercase with underscores separating the words.
* Public function names follow this pattern:
        * qm_<noun>_<verb>
        * where <noun> is the component name (i.e. uart).
        * where <verb> is the action name (i.e. write).
        * i.e. qm_uart_write
* Private function names (i.e. not in QMSI header files) are not prepended with
  “qm\_” but are recommended to follow the coding conventions.
* User defined types shall follow the same naming rules as functions.
* Source file names are lowercase with underscores separating the words.
* Noun list include:
        * ac, adc, pt, gpio, i2c, i2s, irq, master, rtc, slave, soc, spi, timer,
          pwm, uart, wdt.

* Verb list include:
        * enable, disable, set, get, read, write, config, init, reset.

Reserved Identifiers
********************

In addition to ANSI C reserved identifiers, names starting with the following
prefixes are reserved:

* _ (leading underscore)
* task\_
* fiber\_
* isr\_
* nano\_

Return Codes
************

Public QMSI functions should return non-void error codes based on the
`errno codes <https://en.wikipedia.org/wiki/Errno.h>`_ and should follow these
rules:

* Return code type shall be int.
* Return zero if successful.
* Return negative errno value otherwise.
        * Common error codes shall be consistently named for all devices.

* The recommended coding practice to check return codes is the following:

::

  if (0 != function()) {
        /* Error handling. */
  }

The return type can be void under the following circumstances:

* Functions that cannot possibly return, like qm_soc_reset.
* Functions that don’t receive any parameters and cannot fail.

User Input
**********

To optimize the code for size, functions do not validate user input by default
when building in release mode. It is expected that when using functions defined
in the QMSI, users will provide valid input that does not break functionality.
When building in debug mode, the default behaviour is to return -EINVAL
when the user supplies invalid parameters to a public function.

When to use parenthesis
***********************

==========================      ===========     ===============
Operation                       Parenthesis     Sample
==========================      ===========     ===============
Assignment                      no              A = B
Unary operation                 no              A++
single binary   operation       no              A = B ◊ C
multiple binary operations      yes             A ◊= (B ◊ C)
ternary operation               yes             (A ◊ B) ? C : D
==========================      ===========     ===============

Formatting your code using Clang-format
***************************************

Before you submit your code, you should run the following command to ensure that
it is formatted correctly.
clang-format-3.6 is the preferred version.

**clang-format-3.6 -style=file -i main.c**

"-style=file" This instructs clang-format to search up through the directory
tree for a ".clang-format" rules file. This can be found in the root directory
of the the Quark Microcontroller BSP directory. clang-format will search back up
through the dir tree until it finds a config file to use.

"-i" This instructs clang-format to format the file "inline".

"main.c" This is the name of the file that you want to format.
