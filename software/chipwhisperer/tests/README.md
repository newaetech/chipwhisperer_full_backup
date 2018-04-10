# Software Tests #
Software tests are kept in this directory and can be started using the following commands.

## GUI Tests ##
The gui tests have to be run seperately from the tests without the gui. This is due to some thread issue.
``` C:\chipwhisperer\software\chipwhisperer>python -m unittest discover -v -s ./tests/ -p test_via_gui*.py
```

## No GUI Tests ##
These tests also include testing for the scripts found in scripting-examples.
``` C:\chipwhisperer\software\chipwhisperer>python -m unittest discover -v -s ./tests/ -p test_no_gui*.py
```

## TODO ##
 1. Make it so that both the GUI tests and the non GUI tests can be run at the same time
 1. Add a gui test for the glitch explorer
 1. Add the ability to specify what hardware is being tested (right now only xmega can be tested)

