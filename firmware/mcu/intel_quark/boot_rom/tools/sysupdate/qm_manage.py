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

"""qm-update: Script for Quark Microcontroller management.

Note: Need more verbose description.

::

   usage: qm_manage.py CMD [options]"""

from __future__ import print_function, division, absolute_import
import os
import sys
import argparse
import subprocess
import tempfile

import qmfmlib

__version__ = "1.1"

# If you plan to use Unicode characters (e.g., ® and ™) in the following string
# please ensure that your solution works fine on a Windows console.
DESC = "Intel(R) Quark(TM) Microcontroller Firmware Management tool."


class QMManage(object):
    """Class containing all manage functionality."""

    def __init__(self, parser):
        self.parser = parser
        self.args = None

    def _add_parser_con_arguments(self):
        """Adds parser defaults to argparse instance."""
        self.parser.add_argument(
            "-p", metavar="SERIAL_PORT", type=str, dest="port", required=False,
            help="specify the serial port to use")

        self.parser.add_argument(
            "-d", metavar="USB_DEVICE", type=str, dest="device", required=False,
            help="specify the USB device (vendor:product) to use")

        self.parser.add_argument(
            "-S", metavar="USB_SERIAL", type=str, dest="serial", required=False,
            help="specify the serial string of the USB device to use")

    def _command(self):
        """Evaluate command line arguments and decide what transport to use."""
        cmd = []
        # -p SERIAL_PORT
        if self.args.port:
            if self.args.device or self.args.serial:
                self.parser.error("cannot combine -p with -d or -S option")
            cmd.append("dfu-util-qda")
            cmd.append("-p")
            cmd.append(self.args.port)

        else:
            cmd.append("dfu-util")

            # -d VENDOR:PRODUCT
            if self.args.device:
                cmd.append("-d")
                cmd.append(self.args.device)

            if self.args.serial:
                cmd.append("-S")
                cmd.append(self.args.serial)

        if len(cmd) < 2:
            self.parser.error("no device specified. Use -p, -d or -S")

        return cmd

    def _create_temp(self, data):
        """Create a temporary data file containing data."""
        file_name = None
        try:
            file_handler = tempfile.NamedTemporaryFile("wb", delete=False)
            file_name = file_handler.name
            file_handler.write(data)
            file_handler.close()
        except IOError as error:
            self.parser.error(error)
        return file_name

    def call_tools(self, cmd, message):
        """Call command and return stdout or False."""
        print(message, end="")
        sys.stdout.flush()
        p = subprocess.Popen(cmd,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        out, err = p.communicate()

        if err:
            print("[FAIL]")
            print(err.strip())
            return False

        print("[DONE]")
        return out

    def list_devices(self):
        """Perform 'list' tasks."""
        self.parser.description += " Retrieve list of connected USB devices."
        self.parser.add_argument(
            "-d", metavar="USB_DEVICE", type=str, dest="device", required=False,
            help="specify the USB device (vendor:product) to use")
        self.args = self.parser.parse_args()

        cmd = ["dfu-util", "-l"]
        # -d VENDOR:PRODUCT
        if self.args.device:
            cmd.append("-d")
            cmd.append(self.args.device)

        out = self.call_tools(cmd, "Reading USB device list...\t\t")
        if not out:
            exit(1)
        print(out)

    def info(self):
        """Perform 'info' tasks."""

        self.parser.description += " Retrieve device information."
        self._add_parser_con_arguments()
        self.parser.add_argument("--format", choices=['text', 'json'],
                                 help="presentation format [default: text]")
        self.args = self.parser.parse_args()
        cmd = self._command()

        # Prepare sys info request.
        request = qmfmlib.QFMRequest(qmfmlib.QFMRequest.REQ_SYS_INFO).content
        image = qmfmlib.DFUImage()
        data = image.add_suffix(request)

        # Write temp output file. This file will be passed on to dfu-utils-qda.
        file_name = self._create_temp(data)
        # Download request to device. Using alternate setting 0 (QFM).
        if not self.call_tools(cmd + ["-D", file_name, "-a", "0"],
                               "Requesting system information...\t"):
            os.remove(file_name)
            exit(1)
        os.remove(file_name)

        # Create and delete a temporary file. This is done to check the file
        # permissions of the output file we give dfu-utils-qda to store the
        # result of our requested response.
        file_name = self._create_temp("")
        os.remove(file_name)

        # Upload response from device. Using alternate setting 0 (QFM).
        if not self.call_tools(cmd + ["-U", file_name, "-a", "0"],
                               "Reading system information...\t\t"):
            exit(1)

        in_file = open(file_name, "rb")
        response = qmfmlib.QFMResponse(in_file.read())
        in_file.close()
        os.remove(file_name)

        if not response.cmd == qmfmlib.QFMResponse.RESP_SYS_INFO:
            print("Error: Invalid response.")
            exit(1)

        # Parse and Present Sys-Info data.
        info = qmfmlib.QFMSysInfo(response.content)

        if self.args.format == "json":
            print(info.info_json())
        else:
            print(info.info_string())

    def erase(self):
        """Perform 'erase' tasks."""
        self.parser.description += "Erase all applications on the device."
        self._add_parser_con_arguments()
        self.args = self.parser.parse_args()
        cmd = self._command()

        request = qmfmlib.QFMRequest(qmfmlib.QFMRequest.REQ_APP_ERASE).content
        image = qmfmlib.DFUImage()
        data = image.add_suffix(request)

        # Write output file.
        file_name = self._create_temp(data)
        if not self.call_tools(cmd + ["-D", file_name, "-a", "0"],
                               "Erasing all application data...\t\t"):
            os.remove(file_name)
            exit(1)
        os.remove(file_name)


def main():
    """The main function."""
    version = "qm_manage {version}".format(version=__version__)

    if "--version" in sys.argv:
        print (version)
        exit(0)

    choices_desc = "possible commands:\n" + \
                   "  erase         erase all applications\n" + \
                   "  info          retrieve device information\n" + \
                   "  list          retrieve list of connected devices"
    _parser = argparse.ArgumentParser(
        description=DESC,
        epilog=choices_desc,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    # Note: Version is not parsed by argparse if --version is not added.
    _parser.add_argument('--version', action='version', version=version)
    _parser.add_argument("cmd", help="run specific command",
                         choices=['info', 'erase', 'list'])
    group = _parser.add_mutually_exclusive_group()
    group.add_argument("-q", "--quiet", action="store_true",
                       help="suppress non-error messages")
    group.add_argument("-v", "--verbose", action="count",
                       help="increase verbosity")

    # parse for command only
    args = _parser.parse_args(sys.argv[1:2])

    manager = QMManage(_parser)

    if args.cmd == "info":
        manager.info()
        exit(0)

    if args.cmd == "list":
        manager.list_devices()
        exit(0)

    if args.cmd == "erase":
        manager.erase()
        exit(0)
    exit(1)

if __name__ == "__main__":
    main()
