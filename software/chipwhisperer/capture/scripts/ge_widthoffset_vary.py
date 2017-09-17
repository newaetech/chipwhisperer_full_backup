"""Glitch Explorer example to modify clock offset & width.

To use this be sure to set 'Output Format' as $GLITCH$ so data is passed through.
"""

class IterateGlitchWidthOffset(object):
    def __init__(self, ge_window):
        self._starting_offset = -40
        self._starting_width = 5
        self.ge_window = ge_window

    def reset_glitch_to_default(self, scope, target, project):
        """ Set glitch settings to defaults. """
        self.offset = self._starting_offset
        self.width = self._starting_width

    def change_glitch_parameters(self, scope, target, project):
        """ Example of simple glitch parameter modification function. """
        self.offset += 0.1

        if self.offset > 40:
            self.offset = self._starting_offset
            self.width += 0.2

        if self.width > 40:
            self.width = self._starting_width

        # Write data to scope
        scope.glitch.width = self.width
        scope.glitch.offset = self.offset

        #You MUST tell the glitch explorer about the updated settings
        if self.ge_window:
            self.ge_window.add_data("Glitch Width", scope.glitch.width)
            self.ge_window.add_data("Glitch Offset",scope.glitch.offset)

glitch_iterator = IterateGlitchWidthOffset(self.glitch_explorer)
self.aux_list.register(glitch_iterator.change_glitch_parameters, "before_trace")
self.aux_list.register(glitch_iterator.reset_glitch_to_default, "before_capture")
