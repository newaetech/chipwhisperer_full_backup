#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
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

""" jflash - The Quark JTAG Helper!

This is just a helper script for OpenOCD and its config files, which
are distributed with the ISSM Toolchain:

https://software.intel.com/en-us/articles/issm-toolchain-only-download
"""

from __future__ import print_function, division, absolute_import
import os
import sys
import argparse
import subprocess
import collections

BoardConfig = collections.namedtuple('BoardConfig',
                                     'soc addr_rom addr_rom_2nd addr_x86 addr_arc gdb_port')
BOARD_CFG = {'quarkse_dev':
             BoardConfig(soc='quark_se',
                         addr_rom='0xffffe000',
                         addr_rom_2nd='0x4005b000',
                         addr_x86='0x40030000',
                         addr_arc='0x40000000',
                         gdb_port='3333'),
             'd2000_dev':
             BoardConfig(soc='quark_d2000',
                         addr_rom='0x00000000',
                         addr_rom_2nd=None,
                         addr_x86='0x00180000',
                         addr_arc=None,
                         gdb_port='3333')}

def load_board_cfg(board, addr):
    """ Load board configuration according to user inputs. """
    # board is a valid dictionary index as
    # the parser has already validated it.
    cfg = BOARD_CFG[board]

    addr = getattr(cfg, addr)
    if addr is None:
        print('Incompatible input options.')
        exit(1)

    return (cfg.soc, addr, cfg.gdb_port)

def get_open_ocd_paths(tool_chain_path):
    """ Look for OpenOCD installation path. """
    if not tool_chain_path:
        if 'ISSM_TOOLCHAIN' in os.environ:
            tool_chain_path = os.environ['ISSM_TOOLCHAIN']
        elif 'IAMCU_TOOLCHAIN_DIR' in os.environ:
            tool_chain_path = os.environ['IAMCU_TOOLCHAIN_DIR']
            tool_chain_path = os.path.join(tool_chain_path,
                                           '..', '..', '..', '..', '..')
            tool_chain_path = os.path.normpath(tool_chain_path)
        else:
            print('Tool chain path not found: please provide it as input.')
            exit(1)
        # Path validation.
        tool_chain_path = is_valid_path(tool_chain_path)
    else:
        # Toolchain path given as input by
        # the user and already validated.
        pass

    ocd_path = os.path.join(tool_chain_path,
                            'tools', 'debugger', 'openocd', 'bin')
    ocd_path = is_valid_path(ocd_path)

    ocd_exe = os.path.join(ocd_path, 'openocd')

    ocd_scripts_path = os.path.join(tool_chain_path,
                                    'tools', 'debugger', 'openocd', 'scripts')
    helpers_path = os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])),
                                'helpers')
    # Path validation.
    ocd_scripts_path = is_valid_path(ocd_scripts_path)
    helpers_path = is_valid_path(helpers_path)

    return (ocd_exe, ocd_scripts_path, helpers_path)

def run_open_ocd(board_cfg, ocd_env_path, input_file, debug):
    """ Run commands to flash the target or start a
        debug session. """
    # Extract board information.
    (soc, flash_addr, gdb_port) = board_cfg
    # Extract path information.
    (bin_path, scripts_path, helpers_path) = ocd_env_path

    if debug:
        with open(os.devnull, 'w') as dev_null:
            try:
                cmd = [bin_path, '-s', scripts_path,
                       '-s', helpers_path,
                       '-f', 'debug_{}.cfg'.format(soc)]
                subprocess.Popen(cmd, stdout=dev_null, stderr=dev_null)
                cmd = ['gdb', '-ex', 'set architecture i386:intel',
                       '-ex', 'target remote :{}'.format(gdb_port),
                       '-ex', 'monitor targets lmt.cpu',
                       '-ex', 'file {}'.format(input_file)]
                subprocess.call(cmd)
            except subprocess.CalledProcessError:
                print('Command failed.')
                exit(1)
    else:
        cmd = [bin_path, '-s', scripts_path, '-s', helpers_path,
               '-f', 'flash_{}.cfg'.format(soc),
               '-c', 'load_image {} {}'.format(input_file, flash_addr),
               '-c', 'verify_image {} {}'.format(input_file, flash_addr),
               '-f', '{}-release.cfg'.format(soc)]
        try:
            subprocess.call(cmd)
        except subprocess.CalledProcessError:
            print('Command failed.')
            exit(1)

def is_valid_path(input_path):
    """ Check if input path exists. """
    if not os.path.exists(input_path):
        print('\'{}\' is not a valid path.'.format(input_path))
        exit(1)
    return input_path

def is_valid_file(input_file):
    """ Check if input file exists. """
    if not os.path.isfile(input_file):
        print('File \'{}\' not found.'.format(input_file))
        exit(1)
    return input_file

def main():
    """ Parse user input and run OpenOCD. """
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Start debugging. Image MUST be an ELF file.')
    parser.add_argument('-t', '--toolchain', type=is_valid_path,
                        help='Tool chain installation path.')
    parser.set_defaults(addr='addr_x86')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-s', '--sensor', action='store_const',
                       dest='addr', const='addr_arc',
                       help='Sensor Subsystem Application \
                       (only valid for Quark SE).')
    group.add_argument('-r', '--rom', action='store_const',
                       dest='addr', const='addr_rom',
                       help='Flash ROM (Bootloader - 1st stage).')
    group.add_argument('-u', '--rom-2nd', action='store_const',
                       dest='addr', const='addr_rom_2nd',
                       help='Flash 2nd Stage ROM (Bootloader - 2nd stage, Quark SE only).')
    parser.add_argument('board', metavar='BOARD',
                        choices=['d2000_dev', 'quarkse_dev'],
                        help='Board name (d2000_dev or quarkse_dev).')
    parser.add_argument('inputfile', type=is_valid_file, metavar='INFILE',
                        help='Image name')
    args = parser.parse_args()

    board_cfg = load_board_cfg(args.board, args.addr)
    ocd_paths = get_open_ocd_paths(args.toolchain)
    run_open_ocd(board_cfg, ocd_paths, args.inputfile, args.debug)

if __name__ == '__main__':
    main()
