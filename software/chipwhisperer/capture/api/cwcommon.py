from ..scopes.cwhardware.ChipWhispererSAM3Update import SAMFWLoader
from ...hardware.naeusb.naeusb import NAEUSB
from typing import Callable, Union, Dict, Tuple, cast, List, Optional

import io

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

    def upgrade_firmware(self):
        """Attempt a firmware upgrade. See https://chipwhisperer.readthedocs.io/en/latest/firmware.html for more information.

        .. versionadded:: 5.6.1
            Improved programming interface
        """
        prog = SAMFWLoader(self)
        prog.auto_program()

    def reset_sam3u(self):
        """Reset the ChipWhisperer's microcontroller

        .. versionadded:: 5.6.1
            Add common cw interface
        """
        self._getNAEUSB().reset()
