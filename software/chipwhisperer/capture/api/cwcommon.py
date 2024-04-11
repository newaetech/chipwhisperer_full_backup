# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2022, NewAE Technology Inc
# All rights reserved.
#
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com 
#
#=================================================

from ...hardware.naeusb.naeusb import NAEUSB
from ...common.utils import util
from ...hardware.naeusb.fpga import FPGA
from typing import Callable, Union, Dict, Tuple, cast, List, Optional

import io

class ChipWhispererSAMErrors(util.DisableNewAttr):
    LED_settings_str = ("Default", "Debug", "On Error")
    def __init__(self, naeusb):
        super().__init__()
        self.naeusb = naeusb
        self.disable_newattr()
        pass

    def _error_to_str(self, err):
        rtn = ""
        while err:
            if rtn != "":
                rtn += ", "
            if err & 0x01:
                rtn += "serial rx overflow"
                err &= 0xFE
            elif err & 0x02:
                rtn += "serial tx overflow"
                err &= 0xFD

        if rtn == "":
            rtn = False
        return rtn

    @property
    def sam_led_setting(self):
        if self.naeusb.check_feature("SAM_ERR_LED"):
            setting = self.naeusb.get_led_settings()
            if setting >= len(self.LED_settings_str):
                return "Unknown"
            return self.LED_settings_str[setting]
        return None

    @sam_led_setting.setter
    def sam_led_setting(self, setting):
        if self.naeusb.check_feature("SAM_ERR_LED"):
            if setting not in self.LED_settings_str:
                raise ValueError("Invalid LED setting {}, valid are {}".format(setting, self.LED_settings_str))
            self.naeusb.set_led_settings(self.LED_settings_str.index(setting))


    @property
    def sam_errors(self):
        if self.naeusb.check_feature("SAM_ERR_LED"):
            err = self.naeusb.check_sam_errors()
            return self._error_to_str(err)
        return None # not supported
            
    def clear(self):
        if self.naeusb.check_feature("SAM_ERR_LED"):
            self.naeusb.clear_sam_errors()

    def _dict_repr(self):
        rtn = {'sam_errors': None, 'sam_led_setting': None}
        if self.naeusb.check_feature("SAM_ERR_LED"):
            rtn['sam_errors'] = self.sam_errors
            rtn['sam_led_setting'] = self.sam_led_setting

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()


class ChipWhispererCommonInterface:
    def __init__(self):
        pass

    def enable_MPSSE(self, enable=True):
        """Enable or disable MPSSE mode
        """
        if enable:
            self._getNAEUSB().enable_MPSSE()
        else:
            self.reset_sam3u()
        pass

    def check_feature(self, name, raise_exception=False):
        """Check if a feature is available on this ChipWhisperer
        """
        return self._getNAEUSB().check_feature(name, raise_exception=raise_exception)

    def feature_list(self) -> List[str]:
        """Returns a list of supported features that depend on device/firmware version
        """
        return self._getNAEUSB().feature_list()

    def _getNAEUSB(self) -> NAEUSB:
        raise NotImplementedError("_getNAEUSB() method required")

    def _getFWPy(self) -> List[int]:
        raise NotImplementedError("_getFWPy method required")

    def _get_fpga_programmer(self) -> FPGA:
        raise NotImplementedError("_get_fpga_programmer method required")

    def was_fpga_prog_on_con(self):
        """ Whether or not the ChipWhisperer's FPGA was programmed last time it was connected to

        Will raise NotImplementedError if the device does not have an FPGA.

        Returns:
            True if the FPGA was programmed, False otherwise.
        """
        return self._get_fpga_programmer()._programmed

    @property
    def latest_fw(self) -> Dict[str, int]:
        "Get the newest firmware as a dict with elements major, minor and debug"
        fwver = self._getFWPy()
        return {"major": fwver[0], "minor": fwver[1]}

    @property
    def latest_fw_str(self) -> str:
        fwver = self._getFWPy()
        return "{}.{}.0".format(fwver[0], fwver[1])

    @property
    def fw_version(self) -> Dict[str, int]:
        "Get current firmware version as a dict with elements major, minor and debug"
        a = self._getNAEUSB().readFwVersion()
        return {"major": a[0], "minor": a[1], "debug": a[2]}

    @property
    def fw_version_str(self) -> str:
        """A string of the firmware version:: 
        
            'x.y.z'
        """
        a = self._getNAEUSB().readFwVersion()
        return "{}.{}.{}".format(a[0], a[1], a[2])

    @property
    def sam_build_date(self) -> str:
        """The date the SAM3U firmware was built on

        .. versionadded:: 5.6.1
            Added sam build date to chipwhisperer
        """
        return self._getNAEUSB().get_fw_build_date()

    @property
    def sn(self) -> str:
        """The USB serial number of this scope. 
        
        Can be passed to cw.scope() to specify which ChipWhisperer to connect to
        """
        return self._getNAEUSB().snum

    def get_serial_ports(self) -> Optional[List[Dict[str, int]]]:
        """Get serial ports associated with a ChipWhisperer

        .. versionadded:: 5.5
            Add get_serial_ports()
        """
        return self._getNAEUSB().get_serial_ports()

    def upgrade_firmware(self, fw_path=None):
        """Attempt a firmware upgrade. See https://chipwhisperer.readthedocs.io/en/latest/firmware.html for more information.

        .. versionadded:: 5.6.1
            Improved programming interface
        """
        from ..scopes.cwhardware.ChipWhispererSAM3Update import SAMFWLoader
        prog = SAMFWLoader(self)
        prog.auto_program(fw_path)

    def reset_sam3u(self):
        """Reset the ChipWhisperer's microcontroller

        .. versionadded:: 5.6.1
            Add common cw interface
        """
        self._getNAEUSB().reset()

    def dis(self):
        pass

    def __enter__(self):
        return self

    def __exit__(self, type, value, trackback):
        return self.dis()