from ..scopes.cwhardware.ChipWhispererSAM3Update import SAMFWLoader

class ChipWhispererCommonInterface:
    def __init__(self):
        pass

    def _getNAEUSB(self):
        raise NotImplementedError("_getNAEUSB() method required")

    def _getFWPy(self):
        raise NotImplementedError("_getFWPy method required")

    @property
    def latest_fw(self):
        fwver = self._getFWPy()
        return {"major": fwver[0], "minor": fwver[1]}

    @property
    def latest_fw_str(self):
        fwver = self._getFWPy()
        return "{}.{}.0".format(fwver[0], fwver[1])

    @property
    def fw_version(self):
        a = self._getNAEUSB().readFwVersion()
        return {"major": a[0], "minor": a[1], "debug": a[2]}

    @property
    def fw_version_str(self):
        """A string of the firmware version:: 
        
            'x.y.z'
        """
        a = self._getNAEUSB().readFwVersion()
        return "{}.{}.{}".format(a[0], a[1], a[2])

    @property
    def sam_build_date(self):
        """The date the SAM3U firmware was built on

        .. versionadded:: 5.6.1
            Added sam build date to chipwhisperer
        """
        return self._getNAEUSB().get_fw_build_date()

    @property
    def sn(self):
        return self._getNAEUSB().snum

    @property
    def get_serial_ports(self):
        """Get serial ports associated with a ChipWhisperer

        .. versionadded:: 5.5
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
