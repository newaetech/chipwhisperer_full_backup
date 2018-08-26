"""Glitch Explorer example to modify clock offset & width.

To use this be sure to set 'Output Format' as $GLITCH$ so data is passed through.
"""
import numpy as np

class GlitchParam(object):
    def __init__(self,desc,o,attr_name,arange):
        self._desc = desc
        self._o = o
        self._attr_name = attr_name
        self.range = arange
        self.index = 0

    def size(self):
        """ return the amount of items """
        return len(self.range)

    def __next__(self):
        """ return the next item in the list """
        self.index = self.index+1
        if self.index > self.size():
            raise StopIteration
        return self.range[self.index-1]

    def set_next(self):
        """ Get the next value of the glitch param and set it in hardware """
        a = next(self)
        print(("Setting %s to value %d" % (self._attr_name,a)))
        setattr(self._o,self._attr_name,a)

    def get_desc(self):
        """ Return a human readable description """
        return self._desc

    def get_real_value(self):
        """ Return the value as set in hardware e.g. setting freq to 47.5 might result in a frequency of 47.6565 """
        return getattr(self._o,self._attr_name)

    def has_next(self):
        """ Allows to know if there is a next element to iterate over """
        return self.index < self.size()

    def set_initial(self):
        """ Reset the iterator and set the initial value in hardware """
        self.reset()
        self.set_next()
        self.reset()

    def reset(self):
        """ Reset/rewind the parameter iterator """
        self.index =0

class IterateGlitch(object):
    def __init__(self, ge_window):
        self.params = []
        self.ge_window = ge_window

    def reset_glitch_to_default(self, scope, target, project):
        """ Set glitch settings to defaults. """
        for x in self.params:
            x.set_initial()


    def add_glitch_param(self,desc,obj,attr_name,arange):
        """ Adds a parameters for the glitcher """
        p = GlitchParam(desc,obj,attr_name,arange)
        self.params.append(p)

    def get_count(self):
        """ return the total amount of experiments to be done """
        count =1
        for x in self.params:
            count = count * x.size()
        return count

    def change_glitch_parameters(self, scope, target, project):
        """ Example of a simple glitch parameter modification function. """
        # Change at least one param. if the param is "empty" reset it
        # and go to the next param
        for x in self.params:
            if x.has_next():
                x.set_next()
                break
            else:
                x.reset()
                x.set_next()

        #You MUST tell the glitch explorer about the updated settings
        if self.ge_window:
            for x in self.params:
                self.ge_window.add_data(x.get_desc(), x.get_real_value())

glitch_iterator = IterateGlitch(self.glitch_explorer)
glitch_iterator.add_glitch_param("Glitch Width",scope.glitch,"width",np.arange(5,40,0.390625))
glitch_iterator.add_glitch_param("Glitch Offset",scope.glitch,"offset",np.arange(-40,40,0.390625))

self.aux_list.register(glitch_iterator.change_glitch_parameters, "before_trace")
self.aux_list.register(glitch_iterator.reset_glitch_to_default, "before_capture")

#
# Set the number of experiments to run
#
# self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', glitch_iterator.get_count()])
