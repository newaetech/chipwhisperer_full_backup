import matplotlib.pylab as plt

class GlitchResults(object):
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
                            leg = g.title()
                            #No need to show this one anymore
                            legs.remove(g)
                        else:
                            leg = None

                        sr = float(p[g]) / float(p['_total'])
                        plt.plot(p['_parameter'][x_index], p['_parameter'][y_index], plotdots[g], alpha=sr, label=leg)

                        if mask:
                            break
                        
        xlabel = self.parameters[x_index].title()
        if x_units:
            xlabel += " (" + x_units + ")"
        plt.xlabel(xlabel)
        
        ylabel = self.parameters[y_index].title()
        if y_units:
            ylabel += " (" + y_units + ")"
        plt.ylabel(ylabel)
        
        plt.legend()