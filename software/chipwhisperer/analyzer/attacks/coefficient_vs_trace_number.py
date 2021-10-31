import matplotlib.pyplot as plt # type: ignore
from tqdm import tnrange # type: ignore
import numpy as np

class CoefficientVsTracesNumber:
    """Calculate coefficient from increasing number of traces.

    We can see the changes of specific subkey coefficient while number of traces increases.
    And the time complexity is o(N).

    coefficient(X, Y) = cov(X, Y) / (std(X) * std(Y))
    cov(X, Y) = E(X * Y) - E(X) * E(Y)
    std(X) = sqrt(E(X**2) - E(X)**2)
    std(Y) = sqrt(E(Y**2) - E(Y)**2)
    coefficient(X, Y) = (E(X * Y) - E(X) * E(Y)) / ((sqrt(E(X**2) - E(X)**2)) * (sqrt(E(Y**2) - E(Y)**2)))
    Let X = [X1, X2]
    E(X) = (E(X1) * len(X1) + E(X2) * len(X2)) / (len(X1) + len(X2))
    Store E(X1 * Y1), E(X1), E(Y1), E(X1**2), E(Y1**2),
    calculate coming E(X2 * Y2), E(X2), E(Y2), E(X2**2), E(Y2**2).
    We can get the new coefficient([X1, X2], [Y1, Y2]) without overlapping coefficient(X1, Y1) that we have calculated.


    Example:
        import chipwhisperer.analyzer as cwa
        import chipwhisperer as cw

        project = cw.open_project('my_project')
        leak_model = cwa.leakage_models.sbox_output
        interval = 100
        subkey_index = 0  # First subkey.
        coefficient = CoefficientVsTracesNumber(proj.traces, subkey_index, interval, leak_model)
        results = coefficient.get_effecient()
        coefficient.plot_and_save(results, [20,8], 'your_path_saving_figure')


    Args:
        traces (Iterable of :class:`Traces <chipwhisperer.common.traces.Trace>`): An iterable of traces.
        subkey (int): The subkey you want to obverse.
        interval (int): The number of traces that we use for once calculation.
        leak_model (ModelsBase): A leakage model selected from.

    """
    def __init__(self, traces, subkey_index, interval, leak_model):
        self.traces = traces
        self.subkey_index = subkey_index
        self.interval = interval
        self.leak_model = leak_model

    def get_effecient(self):
        # Get all hamming weights or other measurements related to leakage.
        all_hws = []
        for kguess in tnrange(0, 256):
            all_hws.append(np.array([self.leak_model.leakage(trace_tmp.textin, trace_tmp.textout, kguess, self.subkey_index, {'knownkey': trace_tmp.key}) for trace_tmp in self.traces]))
        maxcpa = [0] * 256
        all_traces = np.array(list(map(lambda x:x.wave, self.traces)))

        N = len(all_traces)
        all_coef = [[0 for _ in range(int(N / self.interval))] for _ in range(256)]

        # Initialize first state by first set of traces.
        trace_array = all_traces[:self.interval]
        t_bar = self.mean(trace_array)
        t_old_square_bar = self.mean(trace_array ** 2)
        o_t = self.std_dev(trace_array, t_bar)
        all_hws_bar = []
        all_hws_old_square_bar = []
        all_hws_x_t_bar = []

        for kguess in range(256):
            hws = all_hws[kguess][: self.interval][:, np.newaxis]
            all_hws_bar.append(self.mean(hws))
            o_hws = self.std_dev(hws, all_hws_bar[kguess])
            all_hws_old_square_bar.append(self.mean(hws ** 2))
            all_hws_x_t_bar.append(self.mean(hws * trace_array))
            correlation = self.cov(trace_array, t_bar, hws, all_hws_bar[kguess])
            cpaoutput = correlation / (o_t * o_hws)
            maxcpa[kguess] = max(abs(cpaoutput))

        for j in range(256):
            all_coef[j][0] = maxcpa[j]

        # Update correaltion by coming traces.
        for i in tnrange(1, int(N/interval)):
            trace_array = all_traces[i * self.interval:(i  +1) * self.interval]
            maxcpa = [0] * 256
            t_bar = self.continue_mean(trace_array, t_bar, i)
            o_t, t_old_square_bar = self.continue_std(trace_array, t_bar, t_old_square_bar, i)

            for kguess in range(256):
                hws = all_hws[kguess][i * self.interval: (i + 1) * self.interval][:, np.newaxis]
                all_hws_bar[kguess] = self.continue_mean(hws, all_hws_bar[kguess], i)
                o_hws, tmp_square = self.continue_std(hws, all_hws_bar[kguess], all_hws_old_square_bar[kguess], i)
                all_hws_old_square_bar[kguess] = tmp_square
                all_hws_x_t_bar[kguess] = self.continue_mean(hws * trace_array, all_hws_x_t_bar[kguess], i)
                correlation = self.continue_cov(all_hws_x_t_bar[kguess], t_bar, all_hws_bar[kguess])
                cpaoutput = correlation/(o_t*o_hws)
                maxcpa[kguess] = max(abs(cpaoutput))

            for j in range(256):
                all_coef[j][i] = maxcpa[j]
        return all_coef

    def plot_and_save(self, all_coef, figsize, save_path=None):
        plt.figure(figsize=[20,8])
        for item in all_coef:
            plt.plot([i*self.interval for i in range(len(all_coef[0]))], item)
        if save_path:
            plt.savefig(save_path)
        plt.show()

    def mean(self, X):
        return np.sum(X, axis=0)/len(X)

    def std_dev(self, X, X_bar):
        return np.sqrt(np.sum((X-X_bar)**2, axis=0))

    def cov(self, X, X_bar, Y, Y_bar):
        return np.sum((X-X_bar)*(Y-Y_bar), axis=0)

    def continue_mean(self, X, X_old_bar, n):
        return (X_old_bar*n + self.mean(X))/(n + 1)

    def continue_std(self, X, X_bar, X_old_square_bar, n):
        X_square_bar = self.continue_mean(X**2, X_old_square_bar, n)
        output = np.sqrt(X_square_bar - X_bar**2)
        return output, X_square_bar

    def continue_cov(self, XY_bar, X_bar, Y_bar):
        return XY_bar - X_bar * Y_bar
