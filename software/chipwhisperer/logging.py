import logging

other_logger = logging.getLogger("ChipWhisperer Other")
target_logger = logging.getLogger("ChipWhisperer Target")
scope_logger = logging.getLogger("ChipWhisperer Scope")
naeusb_logger = logging.getLogger("ChipWhisperer NAEUSB")
tracewhisperer_logger = logging.getLogger("ChipWhisperer TraceWhisperer")
glitch_logger = logging.getLogger("ChipWhisperer Glitch")

chipwhisperer_loggers = [
    logging.getLogger("ChipWhisperer NAEUSB"),
    logging.getLogger("ChipWhisperer Other"),
    logging.getLogger("ChipWhisperer Target"),
    logging.getLogger("ChipWhisperer Scope"),
    logging.getLogger("ChipWhisperer TraceWhisperer"),
    logging.getLogger("ChipWhisperer Glitch")

]

def set_all_log_levels(level):
    for logger in chipwhisperer_loggers:
        logger.setLevel(level)

# logging.debug("Need to print this to have logging work right")
# set_all_log_levels(logging.WARNING)
logging.debug("WTF logging")

#initialize levels for loggers
for logger in chipwhisperer_loggers:
    if logger.level == logging.NOTSET:
        logger.setLevel(logging.WARNING)
