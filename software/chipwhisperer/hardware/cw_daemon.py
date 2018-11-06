#
# Copyright (c) 2014-2018, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com
#
#    This file is part of ChipWhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#
# ChipWhisperer is a trademark of NewAE Technology Inc., registered in the
# United States of America, the European Union, and other jurisdictions.
#
#==========================================================================


import asyncio
import usb.backend.libusb0 as libusb0
import usb.core
import usb.util
import queue

allow_kill = True


class CWCommands(object):
    CONNECT = 1
    DISCONNECT = 2
    GET_FW_VERSION = 3
    SET_CTRL = 4
    GET_CTRL = 5
    CMD_READMEM = 6
    CMD_WRITEMEM = 7


    def pack_command(self):
        pass


class CWSerialServerThread(object):
    """"This class handles the serial terminal data (or log data etc)"""

class CWCommandServerThread(object):
    """This class handles the command server"""

class CWUSBThread(object):
    """This class handles the USB communication with ChipWhisperer Hardware"""

    def __init__(self):
        self.queue = queue.Queue()

    def connect(self, serial_number=None, vid=None):
        pass



class CWServerThreadManager(object):

    def __init__(self):

        self.item_template = { 'pid':0,
          'vid':0,
          'type':None,
          'sn':None,
          'status':'CLOSED',
          'cmdport':0,
          'serport':0,
          'thread':None
        }

        self.devices = []


    def get_devices(self, scan_for_new=True):
        """Get list of known and new devices"""

        if scan_for_new:
            newdevs = self.get_usb_devices()
            olddevs = self.devices.copy()

            #Delete matching devices ot end up with only MISSING devices
            for i,d in enumerate(olddevs):
                for n in newdevs:
                    if d['sn'] == n.serial_number:
                        del olddevs[i]

            #Delete matching devices to end up with only UNKNOWN devices
            for d in self.devices:
                for i,n in enumerate(newdevs):
                    if d['sn'] == n.serial_number:
                        del newdevs[i]

            print(newdevs)
            print(olddevs)

            #Found any devices? if so insert in table
            for d in newdevs:
                self.devices.append(self.item_template.copy())
                self.devices[-1]['vid'] = d.idVendor
                self.devices[-1]['pid'] = d.idProduct
                self.devices[-1]['sn'] = d.serial_number

            #Do we have missing devices? If so should shut down the server
            for d in olddevs:
                print("Killing thread for sn: {}".format(d['sn']))

        return self.devices

    def get_usb_devices(self):
        """Scan for USB devices"""

        devlist = []

        try:
            # Connect to device (attempt #1)
            dev = list(usb.core.find(find_all=True, idVendor=0x2B3E, backend=libusb0.get_backend()))
        except usb.core.NoBackendError:
            try:
                # An error in the previous one is often caused by Windows 64-bit not detecting the correct library, attempt to force this with paths
                # that often work so user isn't aware
                dev = list(usb.core.find(find_all=True, idVendor=0x2B3E, backend=libusb0.get_backend(
                    find_library=lambda x: r"c:\Windows\System32\libusb0.dll")))
            except usb.core.NoBackendError:
                raise IOError(
                    "Failed to find USB backend. Check libusb drivers installed, check for path issues on library, and check for 32 vs 64-bit issues.")
        # Found something
        if len(dev) > 0:
            devlist.extend(dev)

        return devlist

    def open(self, sn=None):
        pass


    def close(self, sn=None):
        pass


class CWServerTelnetClient(asyncio.Protocol):
    def print_welcome(self):
        """Print Welcome Message on Connection"""
        self.transport.write("Welcome to ChipWhisperer Hardware Server\r\n".encode())
        self.print_prompt()

    def print_prompt(self):
        """Print the user prompt"""
        self.transport.write("\r\n>".encode())

    def print_help(self):
        """Prints help messages"""
        help_msg = """
  Short  Long   Description\r
  ?      help   This screen.\r
  q      quit   Closes connection.\r
  k      kill   Closes ALl connections, kills server.\r
  l      list   Lists connected USB devices.\r
  o      open   Opens a device. If more than one device available must specify serial number.\r
  c      close  Closes a device. If more than one device open must specify serial number.\r
"""
        self.transport.write(help_msg.encode())

    def process_control_chars(self, input_string):
        """"Process control characters (deal with backspace only for now)"""

        # Initial state
        # String is stored as a list because
        # python forbids the modification of
        # a string
        displayed_string = []
        cursor_position = 0

        # Loop on our input (transitions sequence)
        for character in input_string:

            # Alphanumeric transition
            if str.isprintable(character):
                # Add the character to the string
                displayed_string[cursor_position:cursor_position + 1] = character
                # Move the cursor forward
                cursor_position += 1

            # Backward transition
            elif character == "\x08":
                # Move the cursor backward
                cursor_position -= 1
            elif character == "\r":
                pass
            elif character == "\n":
                pass
            else:
                print("{} is not handled by this function".format(repr(character)))

        # We transform our "list" string back to a real string
        return "".join(displayed_string)

    def process_cmd(self, cmd):
        """Process commands"""

        if cmd == "?" or cmd == "help":
            self.print_help()

        elif cmd == "q" or cmd == "quit":
            self.close()

        elif cmd == "k" or cmd == "kill":
            asyncio.get_event_loop().stop()

        elif cmd == "l" or cmd == "list":
            self.list_usb()

        elif cmd == "o" or cmd == "open":
            pass

        elif cmd == "c" or cmd == "close":
            pass

        else:
            self.transport.write("Unknown command: {}".format(cmd).encode())

    def list_usb(self):
        table_string = "PID:VID\t\tTYPE\t\tSERIALNUM\t\t\t\tSTATUS\tCMDPORT\tSERPORT\r\n"

        devlist = cwmanager.get_devices()

        for d in devlist:
            table_string += hex(d['vid']) + ":"
            table_string += hex(d['pid']) + "\t"
            table_string += "?\t" + "\t"
            table_string += d['sn'] + "\t"

            table_string += "\r\n"

        self.transport.write(table_string.encode())

    def close(self):
        """Close connection, print to screen"""
        print('Closed connection to {}'.format(self.transport.get_extra_info('peername')))
        self.transport.close()

    def connection_made(self, transport):
        """"Called on connection setup."""
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))
        self.transport = transport
        self.message_buffer = ""

        self.print_welcome()

    def data_received(self, data):
        """Called when data is received"""
        message = data.decode()

        self.message_buffer += message

        if message[0] == '\x03':
            self.close()

        if "\r" in message or "\n" in message:
            cmd = self.process_control_chars(self.message_buffer)
            cmd = cmd.split()[0]
            self.message_buffer = ""

            self.process_cmd(cmd)
            self.print_prompt()


cwmanager = CWServerThreadManager()

loop = asyncio.get_event_loop()
# Each client connection will create a new protocol instance
coro = loop.create_server(CWServerTelnetClient, '127.0.0.1', 1173)
server = loop.run_until_complete(coro)

# Serve requests until Ctrl+C is pressed
print('Serving on {}'.format(server.sockets[0].getsockname()))
loop.run_forever()

# Close the server
server.close()
loop.run_until_complete(server.wait_closed())
loop.close()