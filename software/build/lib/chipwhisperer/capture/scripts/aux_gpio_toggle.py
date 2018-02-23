"""Toggle a pin on the ChipWhisperer's target connection.
"""

from chipwhisperer.capture.auxiliary.GPIOToggle import GPIOToggle

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

toggler = GPIOToggle("tio3", False, 100, 100)
aux_list.register(toggler.togglePin, "before_arm")
