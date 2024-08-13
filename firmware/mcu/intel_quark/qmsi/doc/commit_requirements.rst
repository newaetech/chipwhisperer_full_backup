COMMIT REQUIREMENTS
###################

Overview
********

This page provides details on what requirements there are when making a
submission to QMSI.

.. contents::

File permissions
****************
Any file that isn’t .sh, .py, .rb must not be executable.

Clang-format
************
Clang-format is run against any files in the submitted patch. If any differences
are detected in the submitted files and the output of clang-format, the patch
will be rejected. Currently the version of clang-format is 3.6, and if you are
using a different version this can cause issues.

Commit message
**************
Your commit message must adhere to the guidelines in doc/commit_style.rst.
(Note: for people with really long names, the signed off line can be > 72 chars)
