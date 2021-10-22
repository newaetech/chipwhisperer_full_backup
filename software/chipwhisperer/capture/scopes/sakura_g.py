import logging
import sys
import chipwhisperer.capture.scopes._qt as openadc_qt # type: ignore
from .OpenADC import OpenADC
from collections import OrderedDict
#from chipwhisperer.common.utils.pluginmanager import Plugin
#from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.utils.util import dict_to_str, DelayedKeyboardInterrupt
try:
    import ftd2xx as ft # type: ignore
except:
    ft = None

from chipwhisperer.logging import *

class SakuraGScopeType:
    def __init__(self, oadcInstance):
        super().__init__()
        self._sn = ''
        self._sns = ['']
        self.ser = None
        self.scopetype = self

        if (openadc_qt is None) or (ft is None):
            raise ImportError("Needed imports for FTDI missing")
        else:
            self._scope = oadcInstance

    def con(self, sn=None):
        if self.ser is None:
            if sn is None:
                pdevs = ft.listDevices()
                if len(pdevs) > 1:
                    new_pdevs = []
                    for d in pdevs:
                        if d[-1] == ord('A'):
                            new_pdevs.append(d)
                    pdevs = new_pdevs
                if len(pdevs) > 1:
                    raise OSError("Multiple SAKURA-G devices found, please specify serial number")
                if len(pdevs) == 0:
                    raise OSError("Unable to find Sakura-G")
                self._sn = pdevs[0]
            try:
                if isinstance(self._sn, str):
                    self._sn = self._sn.encode('utf-8')
                self.dev = ft.openEx(self._sn, ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
                self.dev.setBitMode(0x00, 0x40)
                self.dev.setTimeouts(500, 500)
                self.dev.setLatencyTimer(2)
                self.ser = self
            except ft.ftd2xx.DeviceError as e:
                self.ser = None
                raise IOError("Unable to connect to Sakura-G {}: {}".format(self._sn, e))
        try:
            self._scope.con(self.ser)
        except IOError as e:
            raise IOError("Openadc error")

    def dis(self):
        self.ser = None
        if hasattr(self, 'dev'):
            self.dev.close()
            del self.dev

    def write(self, data, debug=False):
        if isinstance(data, str):
            data = data.encode('latin-1')
        return self.dev.write(bytes(data))
    def read(self, N=0, debug=False):
        return bytearray(self.dev.read(int(N)))

    @property
    def snum(self):
        return self.scopetype._sn

    def readFwVersion(self):
        return '0.0.0'

class DummyTrigger:
    def __init__(self):
        pass
    def _dict_repr(self):
        return {'': ''}

class SakuraG(OpenADC):

    def default_setup(self):
        self.clock.clkgen_div = 2
        self.gain.gain = 40
        self.adc.samples = 408
        self.adc.offset = 0
        self.adc.basic_mode = "falling_edge"
        self.clock.adc_src = "clkgen_x1"

    def _con(self, sn=None):
        super()._con(sn)
        self.adc._is_sakura_g = True
        self.connectStatus = True

    def __init__(self):
        super().__init__()
        self._sn = ''
        self.scopetype = SakuraGScopeType(self.qtadc)


        if (openadc_qt is None) or (ft is None):
            raise ImportError("Needed imports for FTDI missing")
        else:
            self._scope = self.qtadc

    def write(self, data, debug=False):
        if isinstance(data, str):
            data = data.encode('latin-1')
        return self.scopetype.dev.write(bytes(data))
    def read(self, N=0, debug=False):
        return bytearray(self.scopetype.dev.read(N))

    def _dict_repr(self):
        d = OrderedDict()
        d['adc'] = self.adc._dict_repr()
        d['gain'] = self.gain._dict_repr()
        d['clock'] = self.clock._dict_repr()
        return d

    def __repr__(self):
        if self._is_connected:
            ret = "Sakura-G Device\n"
            return ret + dict_to_str(self._dict_repr())