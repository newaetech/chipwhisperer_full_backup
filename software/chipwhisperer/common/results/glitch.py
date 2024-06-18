#import matplotlib.pyplot as plt # type: ignore

# GlitchController will be part of ChipWhisperer core - just run this block
# for now.

import random, math
import pickle
from copy import deepcopy

try:
    import ipywidgets as widgets # type: ignore
except ModuleNotFoundError:
    widgets = None

def apply_ticks(value, ticks, round):
    value = float(value) * ticks
    return int(round(value))

class gc_data:
    def __init__(self):     
        self.groups = None
        self.parameters = None
        self.parameter_min = None
        self.parameter_max = None
        self.steps = None
        self.results = None
        
def save_gc_results(gc, file_path):
    x = gc_data()
    x.groups = deepcopy(gc.groups)
    x.parameters = deepcopy(gc.parameters)
    x.parameter_min = deepcopy(gc.parameter_min)
    x.parameter_max = deepcopy(gc.parameter_max)
    x.steps = deepcopy(gc.steps)
    x.results = deepcopy(gc.results)
    with open(file_path, "wb") as f:
        pickle.dump(x, f)
    return x

def load_gc_results(file_path):
    with open(file_path, "rb") as f:
        y = pickle.load(f)
        new_gc = GlitchController(groups=y.groups, parameters=y.parameters)
        new_gc.parameter_min = deepcopy(y.parameter_min)
        new_gc.parameter_max = deepcopy(y.parameter_max)
        new_gc.steps = deepcopy(y.steps)
        new_gc.results = deepcopy(y.results)

    return new_gc

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

    def save(self, file_path):
        save_gc_results(self, file_path)

    @property
    def steps_count(self):
        return len(self.steps)
    
    @property
    def group_count(self):
        return len(self.groups)

    @property
    def param_count(self):
        return len(self.parameters)

    @property
    def last_param_index(self):
        return self.param_count - 1

    def get_group_index(self, group):
        return self.groups.index(group)

    def get_param_index(self, param):
        return self.parameters.index(param)

    def get_group_counter(self, group):
        i = self.get_group_index(group)
        return self.group_counts[i]

    def clear(self):
        self.results.clear()        
        self.group_counts = [0] * self.group_count
        
        if self.widget_list_groups:
            for w in self.widget_list_groups:
                w.value = 0
        
    def set_range(self, parameter, low, high):
        
        if high < low:
            t = low
            low = high
            high = t
        
        i = self.get_param_index(parameter)
        self.parameter_min[i] = low
        self.parameter_max[i] = high
        if not (widgets is None): 
            if self.widget_list_parameter:
                # When changing them, need to ensure we don't have min > max ever or will throw
                # an error, so we set max to super-high first.
                self.widget_list_parameter[i].max = 1E9
                self.widget_list_parameter[i].min = low
                self.widget_list_parameter[i].max = high
    
    def set_extoff_range(self, insn_count, **args):
        """Sets the range of a parameter that is meant to control the ext_offset setting.
            parameter_min[extoff_param_name] = floor((range_start - tolerance) * (clkgen_freq / target_freq))
            parameter_max[extoff_param_name] = ceil((range_start + (instruction_count * arch_pipeline_cycles) + tolerance) * (clkgen_freq / target_freq))
        """
        # Minimum/starting value for the range
        start = args.get('range_start', 0)
        # Instruction pipeline cycles
        cycles = args.get('arch_pipeline_cycles', 3)
        # Extra count added to min and max
        tolerance = args.get('tolerance', 0)
        # Clock ratios
        clkgen_freq = args.get('clkgen_freq', 0)
        target_freq = args.get('target_freq', 0)
        clkgen_ticks = args.get('clkgen_per_target_tick', 1)
        # Override clkgen_per_target_tick if both frequencies defined
        if (clkgen_freq > 0) and (target_freq > 0):
            clkgen_ticks = float(clkgen_freq) / float(target_freq)
        
        # Calculate window max
        insn_count = start + (insn_count * cycles) + tolerance
        
        """Calculate window min
        NOTE: We do not apply cycles to start on the thought that the CPU's pipelines are running
        optimally.  Favor a larger window (with lower minimum/starting point) to increase the odds
        to hit a glitch.
        """
        if start > tolerance:
            start -= tolerance
        else:
            start = 0
        
        # Apply clkgen ratio
        if clkgen_ticks != 1:
            clkgen_ticks = float(clkgen_ticks)
            start = apply_ticks(start, clkgen_ticks, math.floor)
            insn_count = apply_ticks(insn_count, clkgen_ticks, math.ceil)

        name = args.get('extoff_param_name', 'ext_offset')
        self.set_range(name, start, insn_count)

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
            parameter = self.get_param_index(parameter)
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
        for i in range(self.steps_count):
            if hasattr(steps, "__iter__"):
                self.steps[i] = steps
            else:
                self.steps[i] = [steps]
        self._num_steps = len(self.steps[0])

    def add(self, group, parameters=None, strdesc=None, metadata=None, plot=True):
        if parameters is None:
            parameters = self.parameter_values
        self.results.add(group, parameters, strdesc, metadata)    
        
        i = self.get_group_index(group)        
        #Basic count
        self.group_counts[i] += 1
        if not self.widget_list_groups is None:
            self.widget_list_groups[i].value =  self.group_counts[i]

        if plot and self._buffers:
            self.update_plot(parameters[self._x_index], parameters[self._y_index], group)

    def glitch_plot(self, plotdots, x_index=0, y_index=1, x_bound=None, y_bound=None, bufferlen=100000):
        """Create a plot that can be updated in real-time with gc.add()

        Args:
            plotdots (dict): Dictionary mapping groups to shapes and colours
            x_index (int or str): Which parameter to use for the x-axis.
            y_index (int or str): Which parameter to use for the y-axis.
            x_bound (list or None): Boundarys for x-axis in plot
            y_bound (list or None): Boundarys for y-axis in plot
            buffer_len (int): Size to make the buffer that holds the points.

        x_index and y_index can be either the index for the desired parameter in the original

        buffer_len controls how many points can be plotted before earlier ones start being overwritten.
        """
        import holoviews as hv # type: ignore
        from holoviews.streams import Buffer # type: ignore
        from pandas import DataFrame # type: ignore
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
        from pandas import DataFrame # type: ignore
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

    def plot_2d(self, plotdots=None, x_index=0, y_index=1, *args, **kwargs):
        if type(x_index) is str:
            x_index = self.parameters.index(x_index)
        if type(y_index) is str:
            y_index = self.parameters.index(y_index)
        if plotdots is None:
            plotdots = self._glitch_plotdots

        return self.results.plot_2d(plotdots, x_index, y_index, *args, **kwargs)
        
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

    def calc(self, ignore_params=[], sort=None):
        if (type(ignore_params) is int) or (type(ignore_params) is str):
            ignore_params = [ignore_params]

        new_param_list = []
        for param in ignore_params:
            if type(param) is str:
                new_param_list.append(self.parameters.index(param))
            else:
                new_param_list.append(param)

        rtn = self.results.calc(ignore_params=new_param_list)
        if sort:
            rtn = sorted(rtn.items(), key=lambda x: x[1][sort])
            rtn.reverse()
        else:
            rtn =list(rtn.items())
        return rtn

    def create_iteration_counts(self):
        """Creates a tuple of iteration counts of (max - min)/step for each parameter.

        Return:
            The tuple of parameter iteration counts.
        """
        set_counts = [ None ] * self.param_count
        for i in range(self.param_count):
            steps = self.steps[i]
            param_counts = [ None ] * len(steps)
            for j in range(len(steps)):
                param_counts[j] = int((self.parameter_max[i] - self.parameter_min[i]) / steps[j])
            set_counts[i] = tuple(param_counts)
        return tuple(set_counts)

    def _set_param_val(self, i, val):
        if self.widget_list_parameter:
            self.widget_list_parameter[i].value = val
        self.parameter_values[i] = val

    def _rst_param_val(self, i):
        self._set_param_val(i, self.parameter_min[i])
        
    def _glitch_val_init(self, clear):
        if clear:
            self.clear()

        self.parameter_values = self.parameter_min[:]

    def glitch_values(self, clear=True):
        """Generator returning the given parameter values in order, using the step size (or step list)"""

        self._glitch_val_init(clear)
        step_idx = [0] * self.param_count

        for i in range(self.param_count):
            self._rst_param_val(i)

        """NOTE: Initializing/resetting i to the final index to iterate parameters starting from
        the final parameter (instead of index 0) like previous logic. Could maybe change to start
        from 0 to simplify logic?
        """
        valid = True
        i = self.last_param_index
        while i >= 0:
            if valid:
                yield self.parameter_values
            
            steps = self.steps[i]
            # Check if the current parameter has a usable step
            j = step_idx[i]
            if j < len(steps):
                # Increment parameter by the current step and check if it's valid
                val = self.parameter_values[i]
                val += steps[j]
                if val <= self.parameter_max[i]:
                    # Parameter is within the valid range
                    self._set_param_val(i, val)
                    valid = True
                    i = self.last_param_index
                else:
                    # Parameter overflowed the range, move to next step mutator
                    self._rst_param_val(i)
                    valid = False
                    step_idx[i] = j + 1
            else:
                # No step mutators left, move to the next parameter
                self._rst_param_val(i)
                valid = False
                step_idx[i] = 0
                i -= 1

    def rand_glitch_values(self, clear=True, randgen=random.randrange):
        """Generator continuously generating random values of parameters divisible by their steps and within their min-max range
        """

        self._glitch_val_init(clear)

        # Get a list of counts for RNG constraints
        counts = self.create_iteration_counts()

        while True:
            for i in range(self.param_count):
                # Randomly select a step mutator
                j = 0
                param_steps = counts[i]
                if len(param_steps) > 1:
                    j = randgen(len(param_steps))

                # Generate a random value for the parameter: min + (step_mutator * RNG(mutator_iterations))
                val = self.parameter_min[i]
                param_steps = param_steps[j]
                if param_steps > 0:
                    val += self.steps[i][j] * randgen(param_steps)
                self._set_param_val(i, val)

            yield self.parameter_values        

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
        self._result_dict = {}
        
    def clear(self):
        '''
        Clears stored statistics in preperation for a new run.
        '''
        self._result_dict = {}

    def results(self, ignore_params=[]):
        """Returns results as a dictionary of 
        results = {
            (param1, param2, ...): (num_total, num_group1, num_group2, ...)
        }
            {'params': (params), 'groups': n_A1, 'groupB': n_B1, "groupC": n_C1, ...},
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
            raise ValueError("Invalid group {} (groups are {})".format(group, self.groups))
        if len(parameters) != len(self.parameters):
            raise ValueError("Invalid number of parameters passed: {:d} passed, {:d} expected".format(len(parameters), len(self.parameters)))

        parameters = tuple(parameters) # make sure parameters is a tuple so it can be hashed

        # if the parameters aren't already in the dict, add an entry for them
        if not parameters in self._result_dict: 
            self._result_dict[parameters] = {'total': 0}
            for k in self.groups:
                self._result_dict[parameters][k] = 0
                self._result_dict[parameters][k+'_rate'] = 0 # entry for success/reset/etc rate, makes plotting easier

        # add to the group totals
        self._result_dict[parameters][group] += 1
        self._result_dict[parameters]['total'] += 1

    # def res_dict_of_lists(self, results):
    #     rtn = {}

    #     # create a list of values for each parameter
    #     orig_key = next(iter(results))
    #     for i in range(len(orig_key)):
    #         rtn[self.parameters[i]] = [p[i] for p in results]

    #     # do the same for each group/group success rate

    #     orig_val = next(iter(results.values())) # grab group dict for first parameter

    #     for k in orig_val:
    #         rtn[k] = [results[p][k] for p in results] # add a list for each group success rate

    #     return rtn

    def res_dict_of_lists(self, results, params=None):
        rtn = {}

        # create a list of values for each parameter
        orig_key = next(iter(results))
        print(orig_key)
        for i in range(len(params)):
            rtn[params[i]] = [p[i] for p in results]

        # do the same for each group/group success rate

        orig_val = next(iter(results.values())) # grab group dict for first parameter

        for k in orig_val:
            rtn[k] = [results[p][k] for p in results] # add a list for each group success rate

        return rtn

    def calc(self, ignore_params=[]):
        '''
        Calculate how many glitches had various effects. Return updated stats.

        Can ignore parameters, combining their results. For example, with 3 parameters,
        ignoring parmameter 2 will combine the results where parameter 0 and 1 are the same,
        but 2 is different.
        '''

        # make sure ignore_params is a list
        if type(ignore_params) is int:
            ignore_params = [ignore_params]

        ignore_params = list(ignore_params)
        ignore_params.sort()
        ignore_params.reverse()

        rtn = {}

        # combine results, ignoring ignore_param
        for param in self._result_dict:

            # param tuple needs to be list so we can delete entries
            new_param = list(param)

            # delete params that we're ignoring
            for p in ignore_params:
                del(new_param[p])

            # now needs to be tuple so can use as index for dict
            new_param = tuple(new_param)

            # combine groups
            if new_param in rtn:
                # already have these settings, so add in new totals
                for group in rtn[new_param]:
                    # print("adding group " + str(group))
                    rtn[new_param][group] += self._result_dict[param][group]
                # rtn[new_param]['total'] += self._result_dict[param]['total']
            else:
                rtn[new_param] = dict(self._result_dict[param])
        
        # calculate rate of occurrence for each group
        for param in rtn:
            for group in self.groups:
                rtn[param][group+'_rate'] = rtn[param][group] / rtn[param]['total']
        
        return rtn

    def plot_2d(self, plotdots, x_index=0, y_index=1, x_units=None, y_units=None, alpha=True):
        '''
        Generate a 2D plot of glitch success rate using matplotlib.

        Plotting is done in the default figure - you may need to call plt.figure() before and
        plt.show() after calling this function if you want more control (or the figure does
        not show by default).
        '''
        import holoviews as hv # type: ignore
        from holoviews import opts # type: ignore
        hv.extension('bokeh', logo=False) #don't display logo, otherwise it pops up everytime this func is called.
        plot = hv.Points([])
        if type(x_index) is str:
            x_index = self.parameters.index(x_index)
        if type(y_index) is str:
            y_index = self.parameters.index(y_index)

        # remove parameters from data that we won't be plotting
        remove_params = list(range(len(self.parameters)))
        print(remove_params)
        keep_params = []
        if x_index > y_index:
            keep_params.append(self.parameters[y_index])
            keep_params.append(self.parameters[x_index])
            del(remove_params[x_index])
            del(remove_params[y_index])
        else:
            keep_params.append(self.parameters[x_index])
            keep_params.append(self.parameters[y_index])
            del(remove_params[y_index])
            del(remove_params[x_index])

        data = self.calc(remove_params)

        # get data as {'param0': [list], 'param1': [list], 'group0_rate': n, ...} for easy plotting
        print(keep_params)
        fmt_data = self.res_dict_of_lists(data, keep_params)


        #We only want legend to show for first element... bit of a hack here
        legs = self.groups[:]
        # remove datapoints with zero % group rate
        def remove_zeros(result, group):
            rtn = {}
            for key in result:
                rtn[key] = [result[key][i] for i, j in enumerate(result[key]) if result[group][i] > 0]
            return rtn
        

        # Plot once for each group
        for g in self.groups:
            if plotdots[g]:
                # plot everything, but if group rate is 0, it'll be fully transparent
                # if g in legs:
                group_data = remove_zeros(fmt_data, g)

                leg = {'label': g.title()}
                    #No need to show this one anymore

                #     legs.remove(g)
                # else:
                #     leg = {}

                if len(plotdots[g]) < 2:
                    raise ValueError("Invalid plotdot {}, must be 2 chars long".format(plotdots[g]))
                if alpha:
                    plot *= hv.Points(group_data, **leg, kdims=[self.parameters[x_index], self.parameters[y_index]],
                        vdims=[g+'_rate']).opts(\
                        color=plotdots[g][1], marker=plotdots[g][0], size=10, height=600, width=800, alpha=g+'_rate', tools=['hover'])
                else:
                    plot *= hv.Points(group_data, **leg, kdims=[self.parameters[x_index], self.parameters[y_index]], vdims=[g+'_rate']).opts(\
                            color=plotdots[g][1], marker=plotdots[g][0], size=10, height=600, width=800, tools=['hover'])

        xlabel = self.parameters[x_index].title()
        if x_units:
            xlabel += " (" + x_units + ")"
        # plt.xlabel(xlabel)

        ylabel = self.parameters[y_index].title()
        if y_units:
            ylabel += " (" + y_units + ")"
        # plt.ylabel(ylabel)

        return plot.redim(y=hv.Dimension(ylabel), x=hv.Dimension(xlabel))
