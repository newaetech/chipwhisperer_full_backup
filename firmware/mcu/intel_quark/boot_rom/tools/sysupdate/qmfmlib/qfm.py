#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

"""Quark Microcontroller Firmware Management Module.

This module provides classes to manage Quark Microcontrollers by using the DFU
interface."""

from __future__ import print_function, division, absolute_import
import struct
import json

# Structures
_ENDIAN = "<"   # Defines the endian for struct packing. ('<'=little, '>'=big)

# Constants
SUPPORTED_VERSIONS = [0, ]
SOC_TYPES = {0: "Quark D2000", 1: "Quark SE"}
AUTH_TYPES = {0: "NONE", }
TARGET_TYPES = {0: "x86", 1: "sensor"}
PARTITION_DATA_SIZE = 5
TARGET_DATA_SIZE = 2
META_DATA_SIZE = 4

class QFMException(Exception):
    """QFM Exception."""

    def __init__(self, message):
        super(QFMException, self).__init__(message)


class QFMResponse(object):
    """The class parsing and storing a QFM response.

    Attributes:
        cmd (int): The QFM response type.
        content (string): The QMD response content.

    Args:
        data (string): Raw response data."""

    RESP_SYS_INFO = 0x444D8000  # Sys-Info-Response identifier.

    _data = None
    cmd = 0
    content = ""

    def __init__(self, data):
        self._data = data
        if len(data) < 4:
            raise QFMException("Invalid response length.")
        (self.cmd, ) = struct.unpack("%sI" % _ENDIAN, data[:4])
        self.content = data[4:]


class QFMRequest(object):
    """The class preparing and storing a QFM request.

    Attributes:
        cmd (int): The QFM request type.
        content (string): The raw QMD request content.

    Args:
        data (int): The QFM request type."""

    REQ_SYS_INFO = 0x444D0000   # Sys-Info-Request identifier.
    REQ_APP_ERASE = 0x444D0001  # App-Erase-Request identifier.

    cmd = 0
    content = ""

    def __init__(self, cmd):
        self.cmd = cmd
        self.content = struct.pack("%sI" % _ENDIAN, cmd)


class QFMSysInfoTarget(dict):
    """The class storing a QFM system info target.

    This class is a dict subclass and can be accessed by getitem and setitem.

    Attributes:
        target_type (int): The target type.
        active_partition (int): The active partition of this target.
        target_number (int) The target number.

    Args:
        number (int): The target number.
        data (string): The raw target data."""

    def __init__(self, number, data):
        dict.__init__(self)
        (
            self.target_type,
            self.active_partition
        ) = struct.unpack("%sBB" % _ENDIAN, data)

        self.target_number = number

        if self.target_type not in TARGET_TYPES.keys():
            raise QFMException("Unknown target type. (%d)" % self.target_type)

        self["type"] = self.target_type
        self["active_partition"] = self.active_partition
        self["number"] = self.target_number
        self["name"] = TARGET_TYPES[self.target_type]


class QFMSysInfoPartition(dict):
    """The class storing a QFM system info partition.

    This class is a dict subclass and can be accessed by getitem and setitem.

    Attributes:
        app_present (int): The application-present flag.
        app_version (int): The application version.
        partition_number (int): The partition number.
        target (int) The target number.

    Args:
        number (int): The partition number.
        data (string): The raw partition data."""

    def __init__(self, number, data):
        dict.__init__(self)
        (
            self.app_present,
            self.app_version
        ) = struct.unpack("%sBI" % _ENDIAN, data)
        self["app_present"] = self.app_present
        self["app_version"] = self.app_version
        self.partition_number = number
        self["number"] = self.partition_number
        # Note: Target and partition number are the same in version 1.1.
        self.target = number
        self["target"] = self.target


class QFMSysInfo(object):
    """Manages Sys Info objects."""

    def __init__(self, data):
        self.data = data
        self.targets = []
        self.partitions = []
        self.parse()

    def parse(self):
        """Parses information received from a Quark Microcontroller
        by issuing a QFM sys info request."""

        self.targets = []
        self.partitions = []
        d = self.data

        # Check if command ID mnd Version atches sys info.
        (self.version, ) = struct.unpack("%sI" % _ENDIAN, d[:4])
        d = d[4:]

        if self.version not in SUPPORTED_VERSIONS:
            raise QFMException("System update version %d not supported." %
                               self.version)

        # Check if data length is sufficient (this time version specific).
        if len(d) < META_DATA_SIZE:
            raise QFMException("Data invalid. Meta data incomplete.")

        (
            self.soc_type,
            self.auth_type,
            self.num_targets,
            self.num_partitions
        ) = struct.unpack("%sBBBB" % _ENDIAN, d[:META_DATA_SIZE])
        d = d[META_DATA_SIZE:]

        # Check if SoC and auth types are valid.
        if self.auth_type not in AUTH_TYPES.keys():
            raise QFMException("Unknown authentication type. (%d)" %
                               self.auth_type)

        if self.soc_type not in SOC_TYPES.keys():
            raise QFMException("Unknown SoC type. (%d)" % self.soc_type)

        # Check target list. Each target has type and active partition.
        if len(d) < self.num_targets * TARGET_DATA_SIZE:
            raise QFMException("Data invalid. Target list incomplete.")
        target_data = d[:self.num_targets * TARGET_DATA_SIZE]
        d = d[self.num_targets * TARGET_DATA_SIZE:]

        for target in range(self.num_targets):
            td = target_data[(target * TARGET_DATA_SIZE):
                             (target+1) * TARGET_DATA_SIZE]
            self.targets.append(QFMSysInfoTarget(target, td))

        # Check partition list.
        if len(d) < self.num_partitions * PARTITION_DATA_SIZE:
            raise QFMException("Data invalid. Partition list incomplete.")
        partition_data = d[:self.num_partitions * PARTITION_DATA_SIZE]

        for partition in range(self.num_partitions):
            d = partition_data[(partition * PARTITION_DATA_SIZE):
                               (partition+1) * PARTITION_DATA_SIZE]
            self.partitions.append(QFMSysInfoPartition(partition, d))

    def info_json(self):
        """Returns a JSON formatted string containing all system information."""

        return json.dumps({
            "version": self.version,
            "soc_type": self.soc_type,
            "soc_name": SOC_TYPES[self.soc_type],
            "auth_type": self.auth_type,
            "auth_name": AUTH_TYPES[self.auth_type],
            "num_targets": self.num_targets,
            "num_partitions": self.num_partitions,
            "partitions": self.partitions,
            "targets": self.targets,
        })

    def info_string(self):
        """Returns a formatted string containing all system information."""

        ret = "Version   : %04x\n" % self.version
        ret += "SoC Type  : %s\n" % SOC_TYPES[self.soc_type]
        ret += "Auth.     : %s\n" % AUTH_TYPES[self.auth_type]

        for target in self.targets:
            ret += "Target %02d : %s (running application on partition %s).\n" \
                    % (target.target_number,
                       TARGET_TYPES[target.target_type],
                       target.active_partition)

        for partition in self.partitions:
            version_string = ""
            if partition.app_present == 0:
                version_string = "No application installed."
            else:
                version_string = "App Version %d." % partition.app_version

            ret += "Part.  %02d : %s\n" % (partition.partition_number,
                                           version_string)
        return ret
