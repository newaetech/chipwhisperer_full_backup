#import matplotlib.pyplot as plt # type: ignore

# GlitchController will be part of ChipWhisperer core - just run this block
# for now.

try:
    import ipywidgets as widgets # type: ignore
except ModuleNotFoundError:
    widgets = None

class GlitchController:
    
    def __init__(self, groups, parameters):
        self.groups = groups
        self.parameters = parameters
        
        self.results = GlitchResults(groups=groups, parameters=parameters)
        
        self.parameter_min = [0.0] * len(parameters)
        self.parameter_max = [10.0] * len(parameters)
        self.steps = [[1]] * len(parameters) # add a separate step setting for each parameter
        
        self.widget_list_parameter = None
        self.widget_list_groups = None

        self._dmaps = None
        self._buffers = None
        self._glitch_plotdots = None
        
        self.clear()
        
    def clear(self):
        self.results.clear()        
        self.group_counts = [0] * len(self.groups)
        
        if self.widget_list_groups:
            for w in self.widget_list_groups:
                w.value = 0
        
    def set_range(self, parameter, low, high):
        
        if high < low:
            t = low
            low = high
            high = t
        
        i = self.parameters.index(parameter)
        self.parameter_min[i] = low
        self.parameter_max[i] = high
        if not (widgets is None): 
            if self.widget_list_parameter:
                # When changing them, need to ensure we don't have min > max ever or will throw
                # an error, so we set max to super-high first.
                self.widget_list_parameter[i].max = 1E9
                self.widget_list_parameter[i].min = low
                self.widget_list_parameter[i].max = high
    
    def set_step(self, parameter, step):
        '''Set a step for a single parameter

        Can be a single value, or a list of step-sizes.

        Single values will be extended to match the length of the rest of 
        parameters' step-sizes.

        Lists must match the length of other step sizes.

        Example::

            gc.set_global_step([1, 2, 3])
            gc.set_step("width", 10) # eqv to [10, 10, 10]
            gc.set_step("offset", [5, 10, 15])
            gc.set_step("ext_offset", [1, 2]) # error, list too short
            gc.set_step(2, [1, 2, 5, 10]) # error, list too long
        '''
        if type(parameter) is str:
            parameter = self.parameters.index(parameter)
        if hasattr(step, "__iter__"):
            if len(step) != self._num_steps:
                raise ValueError("Invalid number of steps {}")
            self.steps[parameter] = step
        else:
            self.steps[parameter] = [step] * self._num_steps



        
    def set_global_step(self, steps):
        '''Set step for all parameters. 
        
        Can be a single value, or a list of step-sizes to iterate through.

        Single values will be converted to a list of length 1.
        
        Overwrites individually set steps.
        '''
        for i in range(len(self.steps)):
            if hasattr(steps, "__iter__"):
                self.steps[i] = steps
            else:
                self.steps[i] = [steps]
        self._num_steps = len(self.steps[0])

    
    def add(self, group, parameters=None, strdesc=None, metadata=None, plot=True):
        if parameters is None:
            parameters = self.parameter_values
        self.results.add(group, parameters, strdesc, metadata)    
        
        i = self.groups.index(group)        
        #Basic count
        self.group_counts[i] += 1
        self.widget_list_groups[i].value =  self.group_counts[i]

        if plot and self._buffers:
            self.update_plot(parameters[self._x_index], parameters[self._y_index], group)

    def glitch_plot(self, plotdots, x_index=0, y_index=1, x_bound=None, y_bound=None, bufferlen=10000):
        import holoviews as hv
        from holoviews.streams import Buffer
        from pandas import DataFrame
        hv.extension('bokeh', logo=False) #don't display logo, otherwise it pops up everytime this func is called.
        if type(x_index) is str:
            x_index = self.parameters.index(x_index)
        if type(y_index) is str:
            y_index = self.parameters.index(y_index)

        self._glitch_plotdots = plotdots
        self._buffers = {}
        self._dmaps = {}
        self._x_index = x_index
        self._y_index = y_index

        x_label = self.parameters[x_index]
        y_label = self.parameters[y_index]

        for k in plotdots.keys():
            if plotdots[k] is None:
                continue
            self._buffers[k] = Buffer(DataFrame({'x': [], 'y': []}, columns=['x', 'y']), length=bufferlen, index=False)
            self._dmaps[k] = hv.DynamicMap(hv.Points, streams=[self._buffers[k]]).opts(height=600, width=800, 
                framewise=True, size=10, marker=plotdots[k][0], color=plotdots[k][1], tools=['hover'])


        plot_iter = iter(self._dmaps)
        plot = self._dmaps[next(plot_iter)]

        for tmp in plot_iter:
            plot *= self._dmaps[tmp]

        if not x_bound:
            x_bound = {}
        else:
            x_bound = {"range": x_bound}

        if not y_bound:
            y_bound = {}
        else:
            y_bound = {"range": y_bound}
        return plot.redim(x=hv.Dimension(x_label, **x_bound), y=hv.Dimension(y_label, **y_bound))
        
    def update_plot(self, x, y, label):
        from pandas import DataFrame
        if label not in self._buffers:
            #raise ValueError("Invalid label {}. Valid labels are {}".format(label, self._buffers.keys()))
            return #probably a label not used
        self._buffers[label].send(DataFrame([(x, y)], columns=['x', 'y']))
    
    def display_stats(self):
        if widgets is None:
            raise ModuleNotFoundError("Could not load ipywidgets, display not available")
        self.widget_list_groups = [widgets.IntText(value=0, description=group + " count:", disabled=True)
                                   for group in self.groups]
        
        self.widget_list_parameter = [widgets.FloatSlider(
                                            value=self.parameter_min[i],
                                            min=self.parameter_min[i],
                                            max=self.parameter_max[i],
                                            step=0.01,
                                            description=p + " setting:",
                                            disabled=True,
                                            continuous_update=False,
                                            orientation='horizontal',
                                            readout=True,
                                            readout_format='.01f')
                                          for i,p in enumerate(self.parameters)]
            
        display(*(self.widget_list_groups + self.widget_list_parameter))

    def plot_2d(self, plotdots=None, x_index=0, y_index=1, x_units=None, y_units=None, mask=True):
        if type(x_index) is str:
            x_index = self.parameters.index(x_index)
        if type(y_index) is str:
            y_index = self.parameters.index(y_index)
        if plotdots is None:
            plotdots = self._glitch_plotdots

        return self.results.plot_2d(plotdots, x_index, y_index, x_units, y_units, mask)

       
        
    def glitch_values(self, clear=True):
        """Generator returning the given parameter values in order, using the step size (or step list)"""
        
        self.parameter_values = self.parameter_min[:]
        
        if clear:
            self.clear()
        
        #transpose steps so that all parameters' steps get passed to loop_rec instead of just one
        steps = list(map(list, zip(*self.steps)))

        for stepsize in steps:
            for val in self._loop_rec(0, len(self.parameter_values)-1, stepsize):
                if self.widget_list_parameter:
                    for i,v in enumerate(val):
                        self.widget_list_parameter[i].value = v
                yield val
        
    def _loop_rec(self, parameter_index, final_index, step):
        self.parameter_values[parameter_index] = self.parameter_min[parameter_index]
        if parameter_index == final_index:            
            while self.parameter_values[parameter_index] <= self.parameter_max[parameter_index]:                                
                yield self.parameter_values
                self.parameter_values[parameter_index] += step[parameter_index]
        else:
            while self.parameter_values[parameter_index] <= self.parameter_max[parameter_index]: 
                yield from self._loop_rec(parameter_index+1, final_index, step)
                self.parameter_values[parameter_index] += step[parameter_index]
                

class GlitchResults:
    """GlitchResults tracks and plots fault injection attempts.
    
    When creating a new object, you must specify the groups of potential glitch
    results (such as 'success' or 'reset'), along with what parameters will be
    varied during this experimentation. For example a typical clock glitching
    setup might look like::
    
        gr = GlitchResults(groups=["success", "reset", "normal"], 
                           parameters=["width", "offset"])
        
    The order the groups ("success", etc) takes is used as a priority when later
    plotting results. The first group (in this case "success" in that list) that
    shows *any* results will become the reported effect of that fault. This is
    done as typically you'd prefer to spot say a 5% success rate than a 95% reset
    or normal rate. Once the object is initialized as above, we could add some
    data to it::
    
        gr.add("reset", (12.3, 33.4))
        gr.add("success", (12.4, 33.8))
        gr.add("reset", (12.4, 33.8))
    
    While this simple example is manually added data, it should be normally added
    by viewing the results of a glitch. You could now visualize this with a graph::
    
        gr.plot_2d(plotdots={"success":"og", "reset":"xr", "normal":".k"})
    
    Note before importing if using Jupyter, you may want to use the notebook magic::
    
        %matplotlib notebook
    
    """
    
    def __init__(self, groups, parameters):
        self.groups = groups
        self.parameters = parameters
        
    def clear(self):
        '''
        Clears stored statistics in preperation for a new run.
        '''
        
        self.result_dict = {}
        
        for k in self.groups:
            self.result_dict[k] = []

    def results(self, ignore_params=[]):
        """Returns results as
        results = [
            {'param1': p_11, 'param2': p_21, ..., 'groupA': n_A1, 'groupB': n_B1, "groupC": n_C1, ...},
            {'param1': p_12, 'param2': p_22, ..., 'groupA': n_A2, 'groupB': n_B2, "groupC": n_C2, ...},
        ]

        Where p_1x, p_2x, ... is a unique grouping of parameters (i.e. width = 10, offset = 20, ...)
        and n_A1
        """
        return None
        
    def add(self, group, parameters, strdesc=None, metadata=None):
        '''
        Add a result to ChipWhisperer glitch map generator.
        '''
        
        if group not in self.groups:
            raise ValueError("Invalid group {:s} - know groups: ({:s})".format(group, str(self.groups)))
            
        if len(parameters) != len(self.parameters):
            raise ValueError("Invalid number of parameters passed: {:d} passed, {:d} expected".format(len(parameters), len(self.parameters)))
        
        r = {'parameters':parameters, 'strdesc':strdesc, 'metadata':metadata}
        
        self.result_dict[group].append(r)
        
    def calc(self):
        '''
        Calculate how many glitches had various effects. Return updated stats.
        '''
        
        results = set()
        
        #Figure out *all* test values
        for g in self.groups:
            for r in self.result_dict[g]:
                results.add(r['parameters'])
                
        #Convert to list
        results = list(results)
        
        #Build count dict
        r = {'_total':0}
        for k in self.groups:
            r[k] = 0
        counts = [dict(r) for _ in range(0, len(results))]
        
        for i, r in enumerate(results):
            counts[i]['_parameter'] = r
        
        #Count list elements
        for g in self.groups:
            for r in self.result_dict[g]:
                for i,t in enumerate(results):
                    if t == r['parameters']:
                        counts[i]['_total'] += 1
                        counts[i][g] += 1

        return counts


    def plot_2d(self, plotdots, x_index=0, y_index=1, x_units=None, y_units=None, mask=True):
        '''
        Generate a 2D plot of glitch success rate using matplotlib.

        Plotting is done in the default figure - you may need to call plt.figure() before and
        plt.show() after calling this function if you want more control (or the figure does
        not show by default).
        '''
        import holoviews as hv
        hv.extension('bokeh', logo=False) #don't display logo, otherwise it pops up everytime this func is called.
        plot = hv.Points([])
        data = self.calc()

        #We only want legend to show for first element... bit of a hack here
        legs = self.groups[:]

        #Generate success rates
        for p in data:
            #Plot based on non-zero priority if possible
            for g in self.groups:
                if plotdots[g]:
                    if p[g] > 0:
                        if g in legs:
                            leg = {'label': g.title()}
                            #No need to show this one anymore

                            legs.remove(g)
                        else:
                            leg = {}

                        sr = float(p[g]) / float(p['_total']) + 0.5
                        
                        if sr > 1.0:
                            sr = 1.0
                        
                        if len(plotdots[g]) < 2:
                            raise ValueError("Invalid plotdot {}, must be 2 chars long".format(plotdots[g]))
                        plot *= hv.Points((p['_parameter'][x_index], p['_parameter'][y_index]), **leg).opts(\
                            color=plotdots[g][1], marker=plotdots[g][0], size=10, height=600, width=800)
                        # plt.plot(p['_parameter'][x_index], p['_parameter'][y_index], plotdots[g], alpha=sr, label=leg)

                        if mask:
                            break

        xlabel = self.parameters[x_index].title()
        if x_units:
            xlabel += " (" + x_units + ")"
        # plt.xlabel(xlabel)

        ylabel = self.parameters[y_index].title()
        if y_units:
            ylabel += " (" + y_units + ")"
        # plt.ylabel(ylabel)

        plot.redim(y=hv.Dimension(ylabel), x=hv.Dimension(xlabel))

        return plot