from .cpa import CPA as CPA_Old
from chipwhisperer.common.api.ProjectFormat import Project
from collections import OrderedDict
from chipwhisperer.common.utils.util import dict_to_str

class CPA(CPA_Old):
    """Correlation Power Analysis (CPA).

    Provides all the needed functionality for taking a project and performing
    a CPA attack with a specific type of leakage model.

    Args:
        proj (Project): An open project instance.
        algorithm: The algorithm used for the analysis. Default is Progressive
            which allows a callback to be given. The callback is called at
            increments of a specific interval. This is useful for auto-updating
            tables and equivalent.
        leak_model: The leakage model used when analysing the captured traces.

    The easiest way to use this class is through the
    :func:`cpa <chipwhisperer.analyzer.cpa>` API function provided by the
    :mod:`chipwhisperer.analyzer` module.

    Example::

        import chipwhisperer.analyzer as cwa
        import chipwhisperer as cw
        proj = cw.open_project('/path/to/project')
        attack = cwa.cpa(proj, cwa.leakage_models.sbox_output)
        results = attack.run()
        print(results)

    Attributes:
        project: Project to pull waves, textin, etc. from.
        algorithm: Analysis algorithm to use for attack. Should be Progressive
        leak_model: Leakage model to use during analysis. Should be of
            type AESLeakageHelper.
        trace_range: Start and end trace number. Should be a list of length 2
            (i.e. [start_num, end_num]).
        point_range: Range of points to use from waves in project. Should be
            a list of length 2 ([start_point, end_point]).
        subkey_list: List of subkeys to attack (subkey_list = [0, 1, 3] will
            attack subkeys 0, 1, and 3).

    .. versionadded:: 5.1
        Added CPA in cpa_new.py to wrap old CPA object
    """
    trace_range = None
    _project = None
    reporting_interval = 10

    def __init__(self, proj, leak_model, algorithm):
        """

        Args:
            proj (Project): c
            leak_model (AESLeakageHelper): Leakage model to use for getting
                hamming weight
            algorithm (AlgorithmsBase: Algorithm to use for attack
        """
        super().__init__()
        self.set_analysis_algorithm(algorithm, leak_model)
        self.set_trace_source(proj.trace_manager())
        self.change_project(proj)
        self.algorithm = algorithm
        self.leak_model = leak_model
        self.set_iterations(1)
        pass

    def _dict_repr(self):
        dict = OrderedDict()
        dict['project'] = self.project
        dict['leak_model'] = self.leak_model
        dict['algorithm'] = self.algorithm
        dict['trace_range'] = self.trace_range
        dict['point_range'] = self.point_range
        dict['subkey_list'] = self.subkey_list
        return dict

    def __str__(self):
        return self.__repr__() + '\n' + dict_to_str(self._dict_repr())

    @property
    def algorithm(self):
        return self.attack

    @algorithm.setter
    def algorithm(self, alg):
        self.set_analysis_algorithm(alg)

    @property
    def project(self):
        return self._project

    @project.setter
    def project(self, proj):
        self._project = proj
        self.set_trace_source(proj.trace_manager())

    @property
    def leak_model(self):
        return self.get_leak_model()

    @leak_model.setter
    def leak_model(self, model):
        self.set_leak_model(model)

    @property
    def point_range(self):
        return self.get_point_range()

    @point_range.setter
    def point_range(self, rng):
        self.set_point_range(rng)

    @property
    def subkey_list(self):
        return self.get_target_subkeys()

    @subkey_list.setter
    def subkey_list(self, subkeys):
        self.set_target_subkeys(subkeys)

    def change_project(self, proj):
        """ Change the project property and update ranges

        If you don't want to update your ranges, change the project attribute
        instead.

        Args:
            project (Project): Project to switch to

        Returns:
            None
        """
        self.project = proj
        self.trace_range = [0, len(proj.traces)]
        self.point_range = [0, len(proj.waves[0])]
        self.subkey_list = range(0, len(proj.keys[0]))

    @property
    def results(self):
        return self.get_statistics()

    def run(self, callback=None, update_interval=25):
        """ Runs the attack

        Args:
            callback (function(), optional): Callback to call every update
                interval. No arguments are passed to callback. Defaults to None.
            update_interval (int, optional):  Number of traces to process
                before updating the results of the attack.

        Returns:
            Results, the results of the attack. See documentation
            for Results for more details.
        """
        if update_interval:
            self.reporting_interval = update_interval
        self.algorithm.setModel(self.leak_model)
        self.algorithm.get_statistics().clear()
        self.algorithm.set_reporting_interval(self.reporting_interval)
        self.algorithm.set_target_subkeys(self.get_target_subkeys())
        self.algorithm.setStatsReadyCallback(callback)
        self.algorithm.addTraces(self.get_trace_source(), self.trace_range,
                                 None, pointRange=self.point_range)
        return self.results

