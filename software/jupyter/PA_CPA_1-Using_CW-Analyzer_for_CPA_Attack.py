#!/usr/bin/env python
# coding: utf-8

# # Using CW-Analyzer for CPA Attack

# This tutorial will take you through a complete attack on a software AES implementation. The specific implementation being attacked is a well-known AES implementation written in C, which is likely to be similar to other implementations used by proprietary systems.

# In[42]:


#Parameters
SCOPETYPE = 'OPENADC'
PLATFORM = 'CWLITEARM'
CRYPTO_TARGET= 'TINYAES128C'
N=500 #number of traces to capture
CHECK_CORR=False


# In[2]:


get_ipython().run_cell_magic('bash', '-s "$PLATFORM" "$CRYPTO_TARGET"', 'cd ../../hardware/victims/firmware/simpleserial-aes\nmake PLATFORM=$1 CRYPTO_TARGET=$2')


# ## Capturing Power Traces

# ### Setup

# We'll use some helper scripts to make setup and programming easier. If you're using an XMEGA or STM (CWLITEARM) target, binaries with the correct should be setup for you:

# In[3]:


get_ipython().run_line_magic('run', '"Helper_Scripts/Setup.ipynb"')


# In[ ]:


fw_path = '../../hardware/victims/firmware/simpleserial-aes/simpleserial-aes-{}.hex'.format(PLATFORM)


# In[ ]:


cw.programTarget(scope, prog, fw_path)


# In addition, before we capture our traces, we'll need to create a ChipWhipserer project, since that's what Analyzer expects for an input:

# In[60]:


project = cw.createProject("projects/Tutorial_B5.cwp", overwrite = True)


# And we can get the class used to hold our traces by:

# In[61]:


tc = project.newSegment()


# ### Capturing Traces

# Below you can see the capture loop. The main body of the loop loads some new plaintext, arms the scope, sends the key and plaintext, then finally records and our new trace into our trace class.

# In[62]:


#Capture Traces
from tqdm import tnrange
import numpy as np
import time

ktp = cw.ktp.Basic(target=target)
target.init()
for i in tnrange(2500, desc='Capturing traces'):
    # run aux stuff that should come before trace here

    key, text = ktp.newPair()  # manual creation of a key, text pair can be substituted here

    #target.reinit()

    target.setModeEncrypt()  # only does something for targets that support it
    target.loadEncryptionKey(key)
    target.loadInput(text)

    # run aux stuff that should run before the scope arms here

    scope.arm()

    # run aux stuff that should run after the scope arms here

    target.go()
    timeout = 50
    # wait for target to finish
    while target.isDone() is False and timeout:
        timeout -= 1
        time.sleep(0.01)

    ret = scope.capture()
    if ret:
        print('Timeout happened during acquisition')

    # run aux stuff that should happen after trace here
    _ = target.readOutput()  # clears the response from the serial port
    #traces.append(scope.getLastTrace())
    tc.addTrace(scope.getLastTrace(), text, "", key)


# In[64]:


from bokeh.plotting import figure, show
from bokeh.io import output_notebook

output_notebook()
data = scope.getLastTrace()
p = figure()
p.line(range(len(data)), data, line_color='red')
show(p)


# Now that we have our traces, we need to tell the project that the traces are loaded and add them to the project's trace manager.

# In[65]:


project.appendSegment(tc)


# If you'd like, you can also save the project for later analysis (this closes the project, so if you run this block you'll need to reopen it in the next section):

# In[66]:


project.save()


# We're now done with the ChipWhisperer hardware, so we should disconnect from the scope and target:

# In[ ]:


# cleanup the connection to the target and scope
scope.dis()
target.dis()


# ## Analysis

# If you saved in the last part (or if you're continuing from where you left off), you'll need to reload the project:

# In[67]:


import chipwhisperer as cw
project = cw.openProject("projects/Tutorial_B5.cwp")
tm = project.traceManager()


# Now that we have our traces, we can begin our attack! We'll start off by setting up our attack by creating an attack object:

# In[68]:


attack = cw.cpa(tm)


# And then setting up the leakage model (more on this in the following CPA tutorials):

# In[69]:


leak_model = cw.AES128(cw.aes128leakage.SBox_output)

attack.setLeakModel(leak_model)


# With that done, we can run our attack:

# In[70]:


cb = cw.getJupyterCallback(attack)
attack_results = attack.processTraces(cb)


# Once you see the above block complete, all the heavylifting is done! All that's left is to actually look at the data. Everything important is contained in the `attack_results` class that `attack.processTraces()` returned.
# 
# We can find the max correlation for every one of the subkey by calling `stats.findMaximums()`, which returns a list of the subkeys, the point location of the max if calculated, and the correlation (which is a value between 0 and 1 that effectively tells us how well our guess fit the data). 
# 
# 
# Note the "point location of the max" is normally not calculated/tracked, and thus returns as a 0. Using the pandas library lets us print them nicely in a DataFrame. We have to transpose the frame to get our expected orientation:

# In[ ]:


import pandas as pd
stat_data = attack_results.findMaximums()
df = pd.DataFrame(stat_data).transpose()
print(df.head())


# Even better, we can use the `.style` method to customize this further. This also lets us chain formatting functions. For example, we can remove the extra 0 and clean up the data. Since we know the correct key, we can even do things like printing the key in a different colour! 
# 
# You can do lots of formatting thanks to the pandas library! Check out https://pandas.pydata.org/pandas-docs/stable/style.html for more details.

# In[ ]:


key = project.traceManager().getKnownKey(0)
def format_stat(stat):
    return str("{:02X}<br>{:.3f}".format(stat[0], stat[2]))

def color_corr_key(row):
    global key
    ret = [""] * 16
    for i,bnum in enumerate(row):
        if bnum[0] == key[i]:
            ret[i] = "color: red"
        else:
            ret[i] = ""
    return ret

df.head().style.format(format_stat).apply(color_corr_key, axis=1)


# You should see red numbers printed at the top of a table. Congratulations, you've now completed a successful CPA attack against AES!
# 
# Next, we'll look at how we can use some of Analyzer's other features to improve the attack process, as well as better interpret the data we have.

# ### Reporting Intervals

# When we ran `attack.processTraces()`, we processed all of the traces before getting any information back. While this works okay for shorter attacks like this, for longer ones it can helpful to get feedback during the attack. This can be done by creating a callback function and passing it to `attack.processTraces()`. This function is called each time we pass our `attack.setReportingInterval()` (default 25 traces) and has access to everything a normal python function does.
# 
# Let's use this to update our table every 10 traces. Most of this is just putting our existing code into the callback function. We also need use the `clear_output` function to clear the table, as well as `display()` to actually get it to show up:

# In[ ]:


from IPython.display import clear_output
import numpy as np
        
def stats_callback():
    attack_results = attack.getStatistics()
    attack_results.setKnownkey(key)
    stat_data = attack_results.findMaximums()
    df = pd.DataFrame(stat_data).transpose()
    clear_output(wait=True)
    display(df.head().style.format(format_stat).apply(color_corr_key,axis=1))
    
attack.setReportingInterval(10)
attack_results = attack.processTraces(stats_callback)


# A default jupyter callback is also available:

# In[ ]:


import chipwhisperer as cw
cb = cw.getJupyterCallback(attack)
attack_results = attack.processTraces(cb)


# Here we used a reporting interval of 10 traces. Depending on the attack and what you want to learn from it, you may want to use higher or lower values: in general reporting less often is faster, but more frequent reporting can allow you to end a long attack early. More frequent reporting also increases the resolution of some plot data (which we will look at next).

# ### Plot Data

# Analyzer also includes a module to create plots to help you interpret the data. These act on one subkey at a time and return some data that we can plot using bokeh (or your graphing module of choice). Let's start by grabbing the class that does all the calculations:

# In[ ]:


plot_data = cw.analyzerPlots(attack_results)


# #### Output Vs. Time

# We'll start by looking at the Output Vs. Time module, which will allow us to plot correlation of our guesses in time. This is useful for finding exactly where the operations we're attacking are. Like in previous tutorials, we'll use bokeh to plot the data we get back.
# 
# The method we're interested in is `getPlotData(bnum)`, which returns in a list: `[xrange, correct_key, incorrect_key_data, incorrect_key_data]` for the position `bnum` passed to it. The method returns two sets of incorrect key data because one is for the key guesses below the correct one, and the other is for guesses above the correct one.
# 
# We'll start by just looking at the 0th subkey. Once we get this data back we'll plot the correct key in red, and the rest in green.

# In[ ]:


from bokeh.plotting import figure, show
from bokeh.io import output_notebook

ret = plot_data.outputVsTime(0)

output_notebook()
p = figure()
p.line(ret[0], ret[2], line_color='green')
p.line(ret[0], ret[3], line_color='green')

p.line(ret[0], ret[1], line_color='red')
show(p)


# You should see some distinctive red spikes in your plot. The largest of these is where the sbox lookup is actually happening (the smaller ones are typically other AES operations that move the sbox data around).
# 
# Let's repeat this for all the subkeys. This is quite a bit more data to plot, so give it a few seconds:

# In[ ]:


"""rets = []
for i in range(0, 16):
    rets.append(plot_data.outputVsTime(i))

p = figure()
for ret in rets:
    p.line(ret[0], ret[2], line_color='green')
    p.line(ret[0], ret[3], line_color='green')
    
for ret in rets:
    p.line(ret[0], ret[1], line_color='red')

show(p)"""


# This information can be useful in many ways. For example, you can probably see the first 16 spikes that make up the sbox lookup are a small portion of the total trace length. If we ever needed to rerun the attack, we could capture a much smaller number of samples and speed up analysis significantly!

# #### PGE vs. Traces

# The next data we'll look at is a plot of partial guessing entropy (PGE) vs. the number of traces. As mentioned before, PGE is just how many spots away from the top the actual subkey is in our table of guesses. For example, if there are 7 subkey guesses that have a higher correlation than the actual subkey, the subkey has a PGE of 7.
# 
# This plot is useful for seeing how many traces were needed to actually break the AES implementation. Keep in mind, however, that the resolution of the plot is determined by the reporting interval (also note that `attack_results.findMaximums()` must be called in the callback function). In our case, we have a reporting interval of 10, so we'll have a resolution of 10 traces.
# 
# This module's `getPlotData()` is similar to the previous plot in that it takes `bnum` as an argument and returns a list of `[xrange, PGE]`. Plotting this data is quite a bit faster than the previous example, we'll just plot all 16 of the `bnum` now.

# In[ ]:


p = figure()

for bnum in range(16):
    ret = plot_data.pgeVsTrace(bnum)
    p.line(ret[0], ret[1], line_color='red')
show(p)


# You should see a number of lines that start off with high values, then rapidly drop off. You may notice that we broke the AES implementation without needing to use all of our traces. 
# 
# Even though we may have broken the AES implementation in fewer traces, we may not want to reduce how many traces we capture. Remember that, while we know the key here, for a real attack we won't and therefore must use the correlation to determine when we've broken a key. Our next plot will help us to determine how feesible capturing fewer traces is.

# #### Correlation vs. Traces

# The last plot we'll take a look at is correlation vs the number of traces. Like with PGE vs. Traces, this plot's resolution is determined by the reporting interval (10 in our case). One again, this is a plot with a lot of data, so we'll start of by just plotting one subkey:
# 
# This module's `getPlotData()` returns a list of `[xrange, [data_for_kguess]]`, so we'll need to plot each guess for each subkey. Like before, we'll do the plot for the correct subkey in red and the rest in green.

# In[ ]:


ret = plot_data.corrVsTrace(0)
p = figure()
for i in range(255):
    if i == key[0]:
        p.line(ret[0], ret[1][i], line_color='red')
    else:
        p.line(ret[0], ret[1][i], line_color='green')
        
show(p)


# As you can see, all the subkey guesses start of with large correlations, but all of them except for the correct guess quickly drop off. If you didn't know the key, at what point would you be sure that the guess with the highest correlation was actually the correct subkey?
# 
# Let's continue and plot all of the subkeys (give this one some time):

# In[ ]:


"""p = figure()
for bnum in range(16):
    ret = plot_data.corrVsTrace(bnum)
    for i in range(255):
        if i == key[bnum]:
            p.line(ret[0], ret[1][i], line_color='red')
        else:
            p.line(ret[0], ret[1][i], line_color='green')
            
show(p)"""


# Like in the first plot, you should see the red lines remain high while the green ones drop off. At what point would you be sure that you've broken all the subkeys? Is it higher than when all of the PGE lines reached zero?

# ## Conclusion

# You should now have completed a successful CPA attack and learned about some on Analyzer's features for improving your attack! 
# 
# You can move onto more advanced tutorials, especially showing you how the actual attack works when performed manually (Tutorial B6). This tutorial also utilized tiny-AES128-C for Arm targets, which uses the same operations as the XMEGA target. A later tutorial will preform this attack on a more typical 32 bit AES implementation.

# ## Tests

# In[ ]:


key = project.traceManager().getKnownKey(0)
recv_key = [kguess[0][0] for kguess in attack_results.findMaximums()]
assert (key == recv_key).all(), "Failed to recover encryption key\nGot: {}\nExpected: {}".format(recv_key, key)


# In[ ]:


assert (attack_results.pge == [0]*16), "PGE for some bytes not zero: {}".format(attack_results.pge)


# In[ ]:


if CHECK_CORR:
    max_corrs = [kguess[0][2] for kguess in attack_results.findMaximums()]
    assert (np.all([corr > 0.75 for corr in max_corrs])), "Low correlation in attack (corr <= 0.75): {}".format(max_corrs)


# In[ ]:




