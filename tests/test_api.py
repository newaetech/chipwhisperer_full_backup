import unittest
import numpy as np
import os, sys
import shutil
import random
from zipfile import ZipFile
import pathlib

script_dir = os.path.dirname(os.path.realpath(__file__))
cw_dir = os.path.realpath('%s/../software' % script_dir)
sys.path.insert(1, cw_dir)

from chipwhisperer.common.api.ProjectFormat import ensure_cwp_extension
import chipwhisperer as cw
import chipwhisperer.common.utils.util as util
import chipwhisperer.analyzer as cwa


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
        self.project = cw.create_project(self.project_name)
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
        self.project_name = 'projects/test_project'

    def tearDown(self):
        self.project.remove(i_am_sure=True)


    def test_short_segments(self):
        self.project = cw.create_project(self.project_name)
        self.project.seg_len = 2
        self.project.seg_ind_max = self.project.traces.seg_len - 1
        traces = create_random_traces(100, 1000)
        self.project.traces.extend(traces)

        index = random.randrange(0, len(traces))
        index_wave = random.randrange(0, len(traces[0]))
        self.assertEqual(traces[index].wave[index_wave], self.project.traces[index].wave[index_wave])


        # Note: I think we can just call assertEqual on the arrays instead of doing it per each value
        for i in range(16):
            # check the plaintext matches
            self.assertEqual(traces[index].textin[i], self.project.traces[index].textin[i])

            # check the textout matches
            self.assertEqual(traces[index].textout[i], self.project.traces[index].textout[i])

            # check the key matches
            self.assertEqual(traces[index].key[i], self.project.traces[index].key[i])

    def test_create_and_save_project(self):
        self.project = cw.create_project(self.project_name)
        self.assertTrue(os.path.isdir(self.project_name + '_data'))

        trace = cw.Trace(np.array([i for i in range(100)]), 'text in', 'text out', 'key')
        for i in range(500):
            self.project.traces.append(trace)

        self.project.save()
        self.assertTrue(os.path.exists(ensure_cwp_extension(self.project_name)))

        # calling it again should not cause issues.
        self.project.save()

    def test_remove_project(self):
        self.project = cw.create_project(self.project_name)

        # must supply i_am_sure argument.
        self.assertRaises(RuntimeWarning, self.project.remove)
        self.assertTrue(os.path.exists(self.project.datadirectory))

        self.project.remove(i_am_sure=True)
        self.assertFalse(os.path.exists(self.project.datadirectory))

    def test_traces_are_retrievable(self):
        self.project = cw.create_project(self.project_name)

        # make sure textin is still textin and not key, etc.
        traces = create_random_traces(100, 1000)
        self.project.traces.extend(traces)

        # retrieve a random trace
        index = random.randrange(0, len(traces))
        # retrieve a random part of the power trace
        index_wave = random.randrange(0, len(traces[0].wave))

        # check that the power trace matches
        self.assertEqual(traces[index].wave[index_wave], self.project.traces[index].wave[index_wave])


        # Note: I think we can just call assertEqual on the arrays instead of doing it per each value
        for i in range(16):
            # check the plaintext matches
            self.assertEqual(traces[index].textin[i], self.project.traces[index].textin[i])

            # check the textout matches
            self.assertEqual(traces[index].textout[i], self.project.traces[index].textout[i])

            # check the key matches
            self.assertEqual(traces[index].key[i], self.project.traces[index].key[i])

    def test_individual_iterables(self):
        self.project = cw.create_project(self.project_name)

        # make sure textin is still textin and not key, etc.
        traces = create_random_traces(50, 1)
        self.project.traces.extend(traces)

        index = 0
        for wave, textin, textout, key in zip(self.project.waves, self.project.textins, self.project.textouts, self.project.keys):
            self.assertEqual(traces[index].textin, textin)
            self.assertEqual(traces[index].textout, textout)
            self.assertEqual(traces[index].key, key)
            self.assertEqual(traces[index].wave, wave)
            index += 1

        self.assertEqual(index, len(self.project.textins))
    
    def test_numpy_conversion(self):
        self.project = cw.create_project(self.project_name)

        traces = create_random_traces(100, 1000)
        self.project.traces.extend(traces)

        np_waves = np.array(self.project.waves)
        self.assertEqual(np.shape(np_waves), (len(traces), len(traces[0].wave)))
        self.assertEqual(np_waves.dtype, 'float64')
        for i in range(len(np_waves)):
            self.assertEqual((np_waves[i,:] == self.project.waves[i]).all(), True)


    def test_project_openable(self):
        self.project = cw.create_project(self.project_name)
        traces = create_random_traces(100, 5000)
        self.project.traces.extend(traces)
        self.project.save()

        # make sure you can open the project with open_project
        self.project = cw.open_project(self.project_name)


class TestProjectExportImport(unittest.TestCase):

    def setUp(self):
        self.project_name = 'projects/test_project'
        self.project = cw.create_project(self.project_name)
        self.traces = create_random_traces(100, 5000)
        self.project.traces.extend(self.traces)
        self.zipfile_path = 'exported_test_project.zip'

        file_paths = list()
        file_paths.append(os.path.join(ensure_cwp_extension(os.path.split(self.project_name)[1])))
        dir_containing_project = os.path.abspath(os.path.join(self.project.datadirectory, '..'))
        for root, dirs, files in os.walk(self.project.datadirectory):
            for file in files:
                file_paths.append(os.path.relpath(os.path.join(root, file), dir_containing_project))

        self.file_paths = [pathlib.Path(x).as_posix() for x in file_paths]

    def tearDown(self):
        self.project.remove(i_am_sure=True)
        os.remove(self.zipfile_path)

    def test_project_exportable(self):
        self.project.export(self.zipfile_path, 'zip')

        # check the zipfile was created
        self.assertTrue(os.path.isfile(self.zipfile_path))

        # check that all paths remain the same
        with ZipFile(self.zipfile_path, 'r') as zippy:
            archive_files = zippy.namelist()
            for file_path in self.file_paths:
                self.assertIn(file_path, archive_files)

    def test_project_importable(self):
        self.project.export(self.zipfile_path, 'zip')
        self.project.remove(i_am_sure=True)
        self.project = cw.import_project(self.zipfile_path)

        # verify that the data is the same
        for path in self.file_paths:
            self.assertTrue(os.path.exists(path))

    def test_import_export_data_integrity(self):
        self.project.export(self.zipfile_path, 'zip')
        self.project.remove(i_am_sure=True)
        self.project = cw.import_project(self.zipfile_path)

        # retrieve a random trace
        index = random.randrange(0, len(self.traces))

        # retrieve a random part of the power trace
        index_wave = random.randrange(0, len(self.traces[0].wave))

        # check that the power trace matches
        self.assertEqual(self.traces[index].wave[index_wave], self.project.traces[index].wave[index_wave])

        for i in range(16):
            # check the plaintext matches
            self.assertEqual(self.traces[index].textin[i], self.project.traces[index].textin[i])

            # check the textout matches
            self.assertEqual(self.traces[index].textout[i], self.project.traces[index].textout[i])

            # check the key matches
            self.assertEqual(self.traces[index].key[i], self.project.traces[index].key[i])


class TestSNR(unittest.TestCase):

    def setUp(self):
        self.project = cw.create_project('test_project')
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


class TestPreprocessing(unittest.TestCase):

    def setUp(self):
        self.project = cw.create_project('test_project')
        traces = create_random_traces(100, 3000)
        self.project.traces.extend(traces)

    def tearDown(self):
        self.project.remove(i_am_sure=True)

    def test_resync(self):
        resync_traces = cwa.preprocessing.ResyncSAD(self.project)
        resync_traces.ref_trace = 0
        resync_traces.target_window = (1000, 1400)
        resync_traces.max_shift = 1000
        new_project = resync_traces.preprocess()


class TestUtils(unittest.TestCase):
    _OBJ_HW_DICT = {
        False: 2,
        True: 4,
        2: 6,
    }

    _OBJ_HW_MAP = {
        2: False,
        4: True,
        6: 2,
    }

    _ENUM_HW_LIST = (
        2,
        4,
        6,
    )

    _ENUM_HW_MAP = {
        2: 0,
        4: 1,
        6: 2,
    }

    _OBJ_STR_DICT = {
        False: 'false',
        True: 'true',
        2: 'int',
    }

    _ENUM_STR_LIST = (
        'zero',
        'one',
        'two',
    )

    _OBJ_EXTRA_DICT = {
        None: False,
    }

    _ENUM_EXTRA_DICT = {
        None: 0,
    }

    _OBJ_TRANSLATE_DIRECT = util.ObjTranslationDirect.alloc_instance(
        _OBJ_STR_DICT,
        _OBJ_EXTRA_DICT
    )

    _ENUM_TRANSLATE_DIRECT = util.EnumTranslationDirect.alloc_instance(
        _ENUM_STR_LIST,
        _ENUM_EXTRA_DICT
    )

    _OBJ_TRANSLATE_HW = util.ObjTranslationToHW.alloc_instance(
        _OBJ_HW_DICT,
        _OBJ_STR_DICT,
        _OBJ_EXTRA_DICT
    )

    _ENUM_TRANSLATE_HW = util.EnumTranslationToHW.alloc_instance(
        _ENUM_HW_LIST,
        _ENUM_STR_LIST,
        _ENUM_EXTRA_DICT
    )

    _OBJ_TRANSLATE_API = util.ObjTranslationAPI.alloc_instance(
        _OBJ_HW_DICT,
        _OBJ_STR_DICT,
        _OBJ_EXTRA_DICT
    )

    _ENUM_TRANSLATE_API = util.EnumTranslationAPI.alloc_instance(
        _ENUM_HW_LIST,
        _ENUM_STR_LIST,
        _ENUM_EXTRA_DICT
    )

    _TEST_BFIELD = util.BitField(3, 2)

    def test_bytearray(self):
        arr = cw.bytearray([1, 2, 3])
        self.assertEqual(str(arr), "CWbytearray(b'01 02 03')")

        arr = cw.bytearray([14, 10, 2])
        self.assertEqual(str(arr), "CWbytearray(b'0e 0a 02')")

    @staticmethod
    def _iter_to_tuple(itr):
        tup = ()
        for val in itr:
            tup += ( val, )
        return tup

    def _test_obj_direct(self, conv):
        self.assertTrue(conv.is_valid_api(False))
        self.assertTrue(conv.is_valid_api(True))
        self.assertTrue(conv.is_valid_api(2))
        self.assertFalse(conv.is_valid_api(3))
        self.assertFalse(conv.is_valid_api(None))
        self.assertEqual(conv.try_var_to_api('false', -1), False)
        self.assertEqual(conv.try_var_to_api('true', -1), True)
        self.assertEqual(conv.try_var_to_api('int', -1), 2)
        self.assertEqual(conv.try_var_to_api(None, -1), False)
        self.assertEqual(conv.try_var_to_api(3, -1), -1)
        self.assertEqual(conv.api_to_str(False), 'false')
        self.assertEqual(conv.api_to_str(True), 'true')
        self.assertEqual(conv.api_to_str(2), 'int')

    def _test_obj_hw(self, conv):
        test_coll = {
            False: conv.api_to_hw(False),
            True: conv.api_to_hw(True),
            2: conv.api_to_hw(2),
        }
        self.assertEqual(test_coll, self._OBJ_HW_DICT)

        actual = self._iter_to_tuple(self._OBJ_HW_DICT.values())
        test_coll = self._iter_to_tuple(conv.hw_values())
        self.assertEqual(len(test_coll), len(actual))
        for key in actual:
            self.assertTrue(key in test_coll)

    def test_obj_direct(self):
        self._test_obj_direct(self._OBJ_TRANSLATE_DIRECT)

    def test_obj_hw(self):
        self._test_obj_direct(self._OBJ_TRANSLATE_HW)
        self._test_obj_hw(self._OBJ_TRANSLATE_HW)

    def test_obj_api(self):
        self._test_obj_direct(self._OBJ_TRANSLATE_API)
        self._test_obj_hw(self._OBJ_TRANSLATE_API)
        test_dict = {}
        for hw in self._OBJ_TRANSLATE_API.hw_values():
            test_dict[hw] = self._OBJ_TRANSLATE_API.try_hw_to_api(hw, -1)
        self.assertEqual(len(test_dict), len(self._OBJ_HW_MAP))
        self.assertEqual(test_dict, self._OBJ_HW_MAP)

    def _test_enum_direct(self, conv):
        self.assertTrue(conv.is_valid_api(0))
        self.assertTrue(conv.is_valid_api(1))
        self.assertTrue(conv.is_valid_api(2))
        self.assertFalse(conv.is_valid_api(-1))
        self.assertFalse(conv.is_valid_api(3))
        self.assertEqual(conv.try_var_to_api('zero'), 0)
        self.assertEqual(conv.try_var_to_api('one'), 1)
        self.assertEqual(conv.try_var_to_api('two'), 2)
        self.assertEqual(conv.try_var_to_api(None), 0)
        self.assertEqual(conv.try_var_to_api(-1), -1)
        self.assertEqual(conv.try_var_to_api(3), -1)
        self.assertEqual(conv.api_to_str(0), 'zero')
        self.assertEqual(conv.api_to_str(1), 'one')
        self.assertEqual(conv.api_to_str(2), 'two')

    def _test_enum_hw(self, conv):
        test_coll = (
            conv.api_to_hw(0),
            conv.api_to_hw(1),
            conv.api_to_hw(2),
        )
        self.assertEqual(test_coll, self._ENUM_HW_LIST)

        test_coll = self._iter_to_tuple(conv.hw_values())
        self.assertEqual(test_coll, self._ENUM_HW_LIST)

    def test_enum_direct(self):
        self._test_enum_direct(self._ENUM_TRANSLATE_DIRECT)

    def test_enum_hw(self):
        self._test_enum_direct(self._ENUM_TRANSLATE_HW)
        self._test_enum_hw(self._ENUM_TRANSLATE_HW)

    def test_enum_api(self):
        self._test_enum_direct(self._ENUM_TRANSLATE_API)
        self._test_enum_hw(self._ENUM_TRANSLATE_API)
        test_dict = {}
        for hw in self._ENUM_TRANSLATE_API.hw_values():
            test_dict[hw] = self._ENUM_TRANSLATE_API.try_hw_to_api(hw)
        self.assertEqual(len(test_dict), len(self._ENUM_HW_MAP))
        self.assertEqual(test_dict, self._ENUM_HW_MAP)

    def test_bfield(self):
        self.assertEqual(self._TEST_BFIELD.width, 3)
        self.assertEqual(self._TEST_BFIELD.pos, 2)
        self.assertEqual(self._TEST_BFIELD.value_mask, 0x7)
        self.assertEqual(self._TEST_BFIELD.extr_mask, 0x1C)
        self.assertEqual(self._TEST_BFIELD.clr_mask, ~0x1C)
        self.assertEqual(self._TEST_BFIELD.clr_field(0x3F), 0x23)
        self.assertEqual(self._TEST_BFIELD.extr_field(0x3F), 0x1C)
        self.assertEqual(self._TEST_BFIELD.make_field(0xF), 0x1C)
        self.assertEqual(self._TEST_BFIELD.extr_value(0x3F), 0x7)
        self.assertEqual(self._TEST_BFIELD.ins_field(0x37, 0x28), 0x2B)
        self.assertEqual(self._TEST_BFIELD.ins_value(0x37, 0xA), 0x2B)

class TestSegment(unittest.TestCase):
    def setUp(self):
        self.project = cw.create_project('test_seg', overwrite=True)
        for i in range(0, 10000):
            arr = bytearray(b'CWUNIQUESTRING1')
            tr = cw.Trace(np.array([0]), arr, arr, arr)
            self.project.traces.append(tr)

        arr = bytearray(b'CWUNIQUESTRING2')
        tr = cw.Trace(np.array([0]), arr, arr, arr)
        self.project.traces.append(tr)
        self.project.save()
        self.project.close()

    def tearDown(self):
        self.project.close(save=False)
        self.project.remove(i_am_sure=True)

    def test_trace_beyond_segment(self):
        self.project = cw.open_project('test_seg')
        arr = bytearray(b'CWUNIQUESTRING2')
        for i in range(0, len(arr)):
            self.assertEqual(self.project.textins[10000][i], arr[i])

class TestCPA(unittest.TestCase):
    def test_CPA(self):
        project = cw.open_project('projects/Tutorial_B5')
        leak_model = cwa.leakage_models.sbox_output
        attack = cwa.cpa(project, leak_model)
        results = attack.run()
        keys = results.find_key()
        for i in range(len(project.keys[0])):
            self.assertEqual(project.keys[0][i], keys[i])

        project.close(save=False)

    def test_jitter(self):
        project = cw.open_project('projects/jittertime')
        resync_traces = cwa.preprocessing.ResyncSAD(project)
        resync_traces.ref_trace = 0
        resync_traces.target_window = (700, 1500)
        resync_traces.max_shift = 700
        new_proj = resync_traces.preprocess()
        leak_model = cwa.leakage_models.sbox_output
        attack = cwa.cpa(new_proj, leak_model)
        results = attack.run()
        keys = results.find_key()
        for i in range(len(project.keys[0])):
            self.assertEqual(project.keys[0][i], keys[i])
        project.close(save=False)



if __name__ == '__main__':
    unittest.main()
