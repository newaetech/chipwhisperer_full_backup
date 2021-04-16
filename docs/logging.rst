.. _cwlogging:

#####################
ChipWhisperer Logging
#####################

As of ChipWhisperer 5.5.1, logging has been reworked in ChipWhisperer
to take advantage of having multiple specialized loggers. Instead 
of using the default logger for everything, we now use 6 primary
loggers for different parts of ChipWhisperer software (from
:code:`chipwhisperer.logging`)::

    other_logger = logging.getLogger("ChipWhisperer Other")
    target_logger = logging.getLogger("ChipWhisperer Target")
    scope_logger = logging.getLogger("ChipWhisperer Scope")
    naeusb_logger = logging.getLogger("ChipWhisperer NAEUSB")
    tracewhisperer_logger = logging.getLogger("ChipWhisperer TraceWhisperer")
    glitch_logger = logging.getLogger("ChipWhisperer Glitch")

These loggers are all in the top level ChipWhisperer :code:`__init__.py`,
so you can do::

    import chipwhisperer as cw
    cw.scope_logger.warning("Test warning")

This allows you to turn different parts of the software to different
logging levels. For example, if you're having issues communicating
with the target, you might set the target_logger to debug::

    import chipwhisperer as cw
    import logging
    cw.target_logger.setLevel(logging.DEBUG)

Or if you're doing glitching and find the warnings about double glitches 
and width/offset of 0 annoying::

    cw.glitch_logger.setLevel(logging.ERROR)

There's also a convenience function for setting the logging level
of all the ChipWhisperer levels::

    cw.set_all_log_levels(logging.WARNING)