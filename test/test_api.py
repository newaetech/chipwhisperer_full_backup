import unittest
import chipwhisperer as cw
import chipwhisperer.analyzer as cwa
import numpy as np
import os
from chipwhisperer.common.api.ProjectFormat import ensure_cwp_extension
import shutil
import random


def create_random_traces(num, wave_length):
    traces = []
    for i in range(num):
        wave = np.random.rand(wave_length)
        textin = [random.randrange(256) for i in range(16)]
        textout = [random.randrange(256) for i in range(16)]
        key = [random.randrange(256) for i in range(16)]
        traces.append(cw.Trace(wave, textin, textout, key))
    return traces


class TestTraces(unittest.TestCase):

    def setUp(self):
        self.project_name = 'testing'
        self.project = cw.create_project(self.project_name, overwrite=True)
        self.fake_trace = cw.Trace(np.array([i for i in range(35)]), 'asdf', 'sdaf', 'sdf')
        self.project.traces.seg_ind_max = 4
        self.trace_num = 13
        for i in range(self.trace_num-1):
            self.project.traces.append(self.fake_trace)
        self.fake_trace_2 = cw.Trace(np.array([i for i in range(35)]), 'asdf', 'sdaf', 'hello')
        self.project.traces.append(self.fake_trace_2)

    def tearDown(self):
        self.project.remove(i_am_sure=True)

    def test_traces_len(self):
        traces = self.project.traces
        self.assertEqual(self.trace_num, len(traces))

    def test_number_of_segments(self):
        self.assertEqual(3, len(self.project.segments))

    def test_get_trace(self):
        traces = self.project.traces

        # Access outside of traces should raise IndexError
        self.assertRaises(IndexError, traces.__getitem__, 13)

        # getting item with a non slice or int object is not allowed
        self.assertRaises(TypeError, traces.__getitem__, object)

        # negative indexing is supported
        self.assertEqual('hello', traces[-1][3])

        # do not allow step in slices
        self.assertRaises(TypeError, traces[1:5:2])

        # do allow slice without step
        self.assertEqual('hello', traces[-2:][-1][3])

    def test_textin_individually(self):
        textins = self.project.textins

        # Access outside of iterable should raise IndexError
        self.assertRaises(IndexError, textins.__getitem__, 13)

        # getting item with a non slice or int object is not allowed
        self.assertRaises(TypeError, textins.__getitem__, object)

        # negative indexing is supported
        self.assertEqual(self.fake_trace_2[1], textins[-1])

        # allow slicing
        self.assertEqual(self.fake_trace_2[1], textins[-2:][-1])


class TestProject(unittest.TestCase):

    def setUp(self):
        self.project_name = 'test_project'

    def tearDown(self):
        self.project.remove(i_am_sure=True)

    def test_create_and_save_project(self):
        self.project = cw.create_project(self.project_name, overwrite=True)
        self.assertTrue(os.path.isdir(os.path.join(cw.PROJECT_DIR, self.project_name + '_data')))

        trace = cw.Trace(np.array([i for i in range(100)]), 'text in', 'text out', 'key')
        for i in range(500):
            self.project.traces.append(trace)

        self.project.save()
        self.assertTrue(os.path.exists(os.path.join(cw.PROJECT_DIR, ensure_cwp_extension(self.project_name))))

        # calling it again should not cause issues.
        self.project.save()

    def test_remove_project(self):
        self.project = cw.create_project(self.project_name, overwrite=True)

        # must supply i_am_sure argument.
        self.assertRaises(RuntimeWarning, self.project.remove)
        self.assertTrue(os.path.exists(self.project.datadirectory))

        self.project.remove(i_am_sure=True)
        self.assertFalse(os.path.exists(self.project.datadirectory))

    def test_traces_are_retrievable(self):
        self.project = cw.create_project(self.project_name)

        # make sure textin is still textin and not key, etc.
        traces = create_random_traces(100, 1000)
        for trace in traces:
            self.project.traces.append(trace)

        # retrieve a random trace
        index = random.randrange(0, len(traces))
        # retrieve a random part of the power trace
        index_wave = random.randrange(0, len(traces[0].wave))

        # check that the power trace matches
        self.assertEqual(traces[index].wave[index_wave], self.project.traces[index].wave[index_wave])

        # check the plaintext matches
        self.assertEqual(traces[index].textin, self.project.traces[index].textin)

        # check the textout matches
        self.assertEqual(traces[index].textout, self.project.traces[index].textout)

        # check the key matches
        self.assertEqual(traces[index].key, self.project.traces[index].key)



class TestSNR(unittest.TestCase):

    def setUp(self):
        self.project = cw.create_project('test_project', overwrite=True)
        self.traces = create_random_traces(1000, 5000)
        for trace in self.traces:
            self.project.traces.append(trace)

    def tearDown(self):
        self.project.remove(i_am_sure=True)

    def test_calculate_snr_with_project(self):
        self.assertRaises(TypeError, cwa.calculate_snr, self.project, cwa.leakage_models.sbox_output)

    def test_calculate_snr_with_traces(self):
        snr = cwa.calculate_snr(self.traces, cwa.leakage_models.sbox_output)
        self.assertEqual(len(snr), 5000)


if __name__ == '__main__':
    unittest.main()
