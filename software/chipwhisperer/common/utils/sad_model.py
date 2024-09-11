# Copyright (c) 2024, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

from chipwhisperer.common.utils import util
from tqdm.notebook import tnrange
import numpy as np


class SADCounter(object):
    """Models the hardware logic for a single SAD counter.
    Used by the other SAD classes here; not meant for end-users.
    """
    _name = 'SAD counter'


    def __init__(self, idx, counter_width, ref, refen, triglen, half_threshold, threshold, interval_threshold, startup_latency, emode=False, interval_matching=False, verbose=False):
        self.verbose = verbose
        self.emode = emode # True: eSAD; False: regular SAD
        self.idx = idx
        self.counter_width = counter_width
        self.ref = ref
        self.refen = refen
        self.triglen = triglen
        self.half_threshold = half_threshold
        self.threshold = threshold
        self.interval_threshold = interval_threshold
        self.startup_latency = startup_latency
        self.interval_matching = interval_matching
        self.reflen = len(ref)
        self.started = False
        self.valid = False
        self.starting_sample = None
        self.current_idx = None
        self.ready2trigger = False
        self.armed_and_ready = False
        self.extended_mode = 0
        self.SAD = 0
        self.SADS = []

    def _dict_repr(self):
        rtn = {}
        rtn['idx'] = self.idx
        rtn['emode'] = self.emode
        rtn['reflen'] = self.reflen
        rtn['triglen'] = self.triglen
        rtn['starting_sample'] = self.starting_sample
        rtn['current_idx'] = self.current_idx
        rtn['started'] = self.started
        rtn['valid'] = self.valid
        rtn['SAD'] = self.SAD
        return rtn

    def reset(self):
        self.started = False
        self.valid = False
        self.starting_sample = None
        self.current_idx = -1
        self.ready2trigger = False
        self.SAD = 0

    def activate(self, time=-1):
        # if we were already active, don't touch anything!
        if not self.started:
            if self.verbose: print("%4d: activating counter %d" % (time, self.idx))
            self.started = True
            self.SAD = 0
            self.current_idx = -self.startup_latency
            self.ready2trigger = False

    @property
    def running(self):
        return self.current_idx >= 0


    def update(self, sample, time, armed_and_ready):
        # returns matched, covered:
        # - matched: True on a SAD counter match
        # - covered: True is sample is covered by a counter *as the first sample of a potential match pattern*
        # - logscore: log SAD score for the overall system, i.e. when it's time for this counter to trigger (or not)
        match = False
        covered = True
        logscore = None
        if not armed_and_ready:
            self.reset()
            return (match, covered, logscore)

        if not self.started:
            return (match, covered, logscore)

        if not self.running:
            self.current_idx += 1
            return (match, covered, logscore)

        incr = 0
        if self.refen[self.current_idx]:
            if self.interval_matching:
                if self.current_idx == 0 or self.SAD < 2**self.counter_width-1: # don't overflow
                    if sample > self.ref[self.current_idx]:
                        if sample > self.ref[self.current_idx] + self.interval_threshold:
                            incr = 1
                    else:
                        if sample < self.ref[self.current_idx] - self.interval_threshold:
                            incr = 1

            else:
                if self.current_idx == 0 or self.SAD < 2**(self.counter_width-1): # don't overflow
                    if sample > self.ref[self.current_idx]:
                        incr = int(sample) - int(self.ref[self.current_idx])
                    else:
                        incr = int(self.ref[self.current_idx]) - int(sample)

        if self.idx == 0 and self.current_idx < 10 and self.verbose:
            print(sample, self.ref[self.current_idx], self.interval_threshold, incr)

        if self.current_idx == 0:
            self.SAD = incr
        else:
            self.SAD += incr
        self.SADS.append(self.SAD)

        self.current_idx += 1
        # Note: DUT decides to extend to full pattern a few cycles before the halfway point:
        if (self.current_idx == self.reflen//2 - 4) and self.emode:
            if self.SAD < self.half_threshold:
                self.extended_mode = 1
                if self.verbose: print("%4d: counter %d reached halfway point and is still alive" % (time, self.idx))
            else:
                self.extended_mode = 0
                if self.verbose: print("%4d: counter %d stopping halfway" % (time, self.idx))

        elif self.current_idx == self.reflen//2 and self.emode:
            logscore = True
            if self.extended_mode:
                covered = False
            else:
                self.current_idx = 0


        elif self.emode or self.triglen is None:
            if self.current_idx == self.reflen:
                self.ready2trigger = True
                self.current_idx = 0
                logscore = True
                if self.verbose: print("%4d: counter %d done, SAD=%d" % (time, self.idx, self.SAD))
                if self.SAD <= self.threshold:
                    if self.verbose: print("%4d: counter %d MATCHED at time %6d with score: %d ===============================" % (time, self.idx, time, self.SAD))
                    match = True

        else:
            if self.current_idx == self.triglen and self.ready2trigger:
                logscore = True
                if self.verbose: print("%4d: counter %d done, SAD=%d" % (time, self.idx, self.SAD))
                if self.SAD <= self.threshold:
                    if self.verbose: print("%4d: counter %d MATCHED at time %6d with score: %d ===============================" % (time, self.idx, time, self.SAD))
                    match = True
            if self.current_idx == self.reflen:
                self.ready2trigger = True
                self.current_idx = 0

        return (match, covered, logscore)


    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()



class SADModel(object):
    """Python model of the Verilog SAD implementation, used for validation.
    Efficiency in computation is not the primary objective of this class!
    Almost 100% cycle-accurate; there can be small differences when
    multiple_triggers is false because the model will only ever generate a
    single trigger, whereas some implementations can let a extra one slip
    through if it's very close to the first one, due to pipeline delays in the
    implementation.  

    This is usually a degenerate case which shouldn't happen with a properly
    tuned SAD module IRL. The main inconvenience is that SAD simulations can
    sometimes fail, and this must be inspected manually.

    This class is not intended for end-users: see the SADModelWrapper class
    instead, which is much easier to configure and use.
    """
    _name = 'SAD model'

    def __init__(self, counter_width, ref, refen, triglen, half_threshold, threshold, interval_threshold, startup_latency, multiple_triggers, emode=False, interval_matching=False, verbose=False):
        self.emode = emode # True: eSAD; False: regular SAD
        self.ref = ref
        self.refen = refen
        self.triglen = triglen
        self.startup_latency = startup_latency
        self.multiple_triggers = multiple_triggers
        self.half_threshold = half_threshold
        self.threshold = threshold
        self.interval_matching = interval_matching
        self.interval_threshold = interval_threshold
        self.verbose = verbose
        self.reflen = len(ref)
        self.index = 0
        if emode:
            if self.reflen % 2:
                raise ValueError('Reference length must be even')
            self.num_counters = self.reflen // 2
        else:
            self.num_counters = self.reflen
        self.counters = []
        self.match_times = [] # for logging match times
        self.match_scores = []
        self.match_counters = []
        self.uncovered_samples = []
        self.covered = []
        self.SADS = []
        self.triggered = False
        for i in range(self.num_counters):
            self.counters.append(SADCounter(i, counter_width, ref, refen, self.triglen, half_threshold, threshold, interval_threshold, startup_latency, emode, interval_matching, verbose))


    def _dict_repr(self):
        rtn = {}
        rtn['emode'] = self.emode
        rtn['threshold'] = self.threshold
        if self.interval_threshold: 
            rtn['interval_threshold'] = self.interval_threshold
        rtn['num_triggers'] = self.num_triggers
        rtn['match_times'] = self.match_times
        rtn['match_time_deltas'] = self.match_time_deltas
        rtn['match_scores'] = self.match_scores
        rtn['match_counters'] = self.match_counters
        if self.emode: 
            rtn['uncovered_samples'] = self.uncovered_samples
        return rtn


    def reset(self):
        self.match_times = []
        self.match_counters = []
        self.match_scores = []
        self.index = 0
        self.triggered = False
        self.SADS = []
        self.uncovered_samples = []
        for c in self.counters:
            c.reset()

    def run(self, wave):
        # to visualize which starting samples aren't covered... easiest way
        # is to first mark all samples as covered, then later (in step())
        # demote those that aren't:
        self.covered = [1]*len(wave)
        for i in tnrange(len(wave)): # go through the full powertrace
            self.step(wave[i], True)
        # in the case of emode, we need to change the SAD score for uncovered samples to NaN, to highlight that they could not have triggered:
        if self.emode:
            for u in self.uncovered_samples:
                self.SADS[u] = np.NaN

    def activate_next_counter(self):
        for c in self.counters:
            if not c.started:
                c.activate()
                break
    @property
    def num_triggers(self):
        return len(self.match_times)

    @property
    def match_time_deltas(self):
        if self.match_times:
            deltas = []
            for i in range(1, len(self.match_times)):
                deltas.append(self.match_times[i] - self.match_times[i-1])
            return deltas
        else:
            return None

    @property
    def trigger_allowed(self):
        if self.triggered and not self.multiple_triggers:
            return False
        else:
            return True

    def step(self, sample, armed_and_ready):
        # on the first run through, counters are started one at a time:
        #if i < self.num_counters:
        #    self.counters[i].activate(i)
        if armed_and_ready and self.trigger_allowed:
            self.activate_next_counter()
        elif not armed_and_ready:
            self.triggered = False
        if not self.trigger_allowed:
            armed_and_ready = False
        # all activated counters are in free-running mode:
        matched = False
        for c in self.counters:
            match, covered, logscore = c.update(sample, 0, armed_and_ready)
            if logscore:
                self.SADS.append(c.SAD)
            if match:
                matched = True
                self.match_times.append(self.index+1)
                self.match_scores.append(c.SAD)
                self.match_counters.append(c.idx)
                self.triggered = True
                if self.verbose: print("counter %d matched at time %6d with score: %d" % (self.index, c.idx, c.SAD))
            if not covered:
                self.uncovered_samples.append(self.index+1)
                try:
                    self.covered[self.index+1] = 0 
                except:
                    pass
                if self.verbose: print("warning: sample %d is not covered" % (self.index+1))
        self.index += 1
        return matched


    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()


class SADModelWrapper(object):
    """Software model of the Husky SAD implementation(s). Wrapper which
    allows providing SAD parameters from a scope.SAD object. Use this
    to run software SAD on a trace.
    For usage and tips, see the companion Jupyter notebook.

    Example::

        sad_model = cw.SADModelWrapper(scope.SAD)
        sad_model.run(trace.wave)
        print(sad_model) # to get the results
        print(sad_model.SADS) # to get the SAD scores for the given trace.wave
    """
    _name = 'SAD model'


    def __init__(self, actual_sad, catch_emisses=False, verbose=False):

        counter_width = actual_sad._sad_counter_width
        reflen = actual_sad.sad_reference_length
        refen = actual_sad.enabled_samples
        ref = actual_sad.reference[:reflen]
        triglen = actual_sad.trigger_sample
        threshold = actual_sad.threshold
        half_threshold = threshold//2
        interval_threshold = actual_sad.interval_threshold
        startup_latency = 0 # doesn't matter here
        multiple_triggers = actual_sad.multiple_triggers
        emode = actual_sad.emode
        interval_matching = actual_sad._im

        self.sad = SADModel(counter_width, ref, refen, triglen, half_threshold, threshold, interval_threshold, startup_latency, multiple_triggers, emode, interval_matching, verbose)
        if emode and catch_emisses:
            self.fsad = SADModel(counter_width, ref, refen, triglen, half_threshold, threshold, interval_threshold, startup_latency, multiple_triggers, False, interval_matching, verbose)
        else:
            self.fsad = None

    def _dict_repr(self):
        rtn = {}
        rtn['num_triggers'] = self.num_triggers
        rtn['match_times'] = self.match_times
        rtn['match_time_deltas'] = self.match_time_deltas
        if self.sad.emode:
            rtn['uncovered_samples'] = self.uncovered_samples
            if self.fsad:
                rtn['missed_triggers'] = self.missed_triggers
        return rtn

    @property
    def SADS(self):
        """SAD scores for the wave processed by run().
        """
        return self.sad.SADS

    @property
    def num_triggers(self):
        return self.sad.num_triggers

    @property
    def match_times(self):
        """Waveform indices where a SAD match occurred. Updated after run().
        """
        return self.sad.match_times

    @property
    def match_time_deltas(self):
        """Time deltas between SAD matches. Updated after run().
        """
        return self.sad.match_time_deltas

    @property
    def match_counters(self):
        return self.sad.match_counters

    @property
    def uncovered_samples(self):
        """When using emode, starting samples for which no SAD counter was available
        (where a SAD match could potentially be missed). By definition, there is one
        uncovered sample for every successful SAD match (there may be more).
        Updated after run().
        """
        return self.sad.uncovered_samples

    @property
    def missed_triggers(self):
        """Only available in emode and when the model is created with catch_emisses=True.
        Shows which SAD matches are missed by emode. Updated after run().
        """
        if self.fsad:
            return sorted(list(set(self.fsad.match_times) - set(self.sad.match_times)))
        else:
            return None

    def reset(self):
        self.sad.reset()
        if self.fsad:
            self.fsad.reset()

    def run(self, wave):
        """Runs the SAD model.
        Args:
            wave (list): input waveform to the SAD model.
        """
        self.reset()
        self.sad.run(wave)
        if self.fsad:
            self.fsad.run(wave)

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()



