QMSI example application
########################

.. contents::

Overview
********
This page outlines the recommended structure for QMSI example applications.

The intention with example applications is to demonstrate the usage of the QMSI
API.

Where possible, the example applications should be written so that they are as
simple and easy to understand as possible. Don't assume that the end user will
have a high level of proficiency with C.

The applications should also be kept consistent with those that already exist
in the repo. The layout, variable name and style should match existing apps.

Structure
*********
main.c file should conform to the following structure:

* Copyright notice.
* Application description comment.

  - Single line title (eg. QMSI GPIO app example).
  - Brief description of what the app is demonstrating.
  - Optionally, setup instructions if required for specific hardware.

* #includes system headers in alphabetical order.
* #includes QMSI headers in alphabetical order.
* #includes all other includes in alphabetical order.
* #defines.
* Enums.
* Structs.
* Global variables.
* Local functions.
* Main function.

A Makefile is required to build the application and must contain:

* APP_NAME
* SOC (default: quark_d2000)
* TARGET (default: x86)

If an application cannot be compiled for a target / SoC combination the Makefile
must include a guard to prevent compilation and generate an error.

The error message generated should conform to one of the following or similar:


        Supported SOC value is <SOC_VALUE>

        Supported TARGET value is <TARGET_VALUE>

Where multiple values are supported, the error message should conform
to the following:


        Supported SOC values are <SOC_VALUE_1> , <SOC_VALUE_2>

        Supported TARGET values are <TARGET_VALUE_1> , <TARGET_VALUE_2>

Output
******
The application should always start by printing the following:
::

        Starting: <APP_NAME>

The application should print the following when it completes:
::

        Finished: <APP_NAME>

If an error has been encountered the message must follow this format:
::

        Error: <ERROR_STRING>

No special code is required for informational messages.

Development platforms
*********************
Any references to development platforms (boards) in the comments should use full
names, for example:
* "Intel(R) Quark(TM) D2000 development platform".
* "Intel(R) Quark(TM) SE development platform".

The use of processor or platform code names should be avoided.

Recommendations
***************
* Functions longer than ~70 lines should be split up.
* Function prototypes are not recommended.
* Functions and global variables used within the translation unit should be marked as static.
* Comments should be C90-style (no //) with capital letters and full stops.
* Print statements should not be used in callback functions.
* Where possible, variable names should be consistent with those used in other
  examples.
