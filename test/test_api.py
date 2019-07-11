import unittest
import chipwhisperer as cw
import numpy as np
import os
from chipwhisperer.common.api.ProjectFormat import ensure_cwp_extension
import shutil


class TestTraces(unittest.TestCase):

    def setUp(self):
        self.project_name = 'testing'
        self.project = cw.create_project(self.project_name, overwrite=True)
        self.fake_trace = (np.array([i for i in range(35)]), 'asdf', 'sdaf', 'sdf')
        self.project.traces.seg_ind_max = 4
        self.trace_num = 13
        for i in range(self.trace_num-1):
            self.project.traces.append(self.fake_trace)
        self.fake_trace_2 = (np.array([i for i in range(35)]), 'asdf', 'sdaf', 'hello')
        self.project.traces.append(self.fake_trace_2)

    def tearDown(self):
        try:
            os.remove(os.path.join(cw.PROJECT_DIR, ensure_cwp_extension(self.project_name)))
        except FileNotFoundError:
            pass
        try:
            shutil.rmtree(os.path.join(cw.PROJECT_DIR, self.project_name + '_data'))
        except FileNotFoundError:
            pass

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


class TestProjectFormat(unittest.TestCase):

    def setUp(self):
        self.project_name = 'test_project'

    def tearDown(self):
        try:
            os.remove(os.path.join(cw.PROJECT_DIR, ensure_cwp_extension(self.project_name)))
        except FileNotFoundError:
            pass
        try:
            shutil.rmtree(os.path.join(cw.PROJECT_DIR, self.project_name + '_data'))
        except FileNotFoundError:
            pass

    def test_create_and_save_project(self):
        project = cw.create_project(self.project_name, overwrite=True)
        self.assertTrue(os.path.isdir(os.path.join(cw.PROJECT_DIR, self.project_name + '_data')))

        trace = (np.array([i for i in range(100)]), 'text in', 'text out', 'key')
        for i in range(500):
            project.traces.append(trace)

        project.save()
        self.assertTrue(os.path.exists(os.path.join(cw.PROJECT_DIR, ensure_cwp_extension(self.project_name))))

        print(project.traces[0])
        project.save()

if __name__ == '__main__':
    unittest.main()
