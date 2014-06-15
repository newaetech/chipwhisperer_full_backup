import sys
from time import sleep
import usb.core
import usb.util

# Generic HID device VID, PID and report payload length (length is increased
# by one to account for the Report ID byte that must be pre-pended)
device_vid = 0x03EB
device_pid = 0xBAED

def get_and_init_hid_device():
    device = usb.core.find(idVendor=device_vid, idProduct=device_pid)

    if device is None:
        print "Failed to find USB Device"
        sys.exit("Could not find USB device.")
        

    if device.is_kernel_driver_active(0):
        try:
            device.detach_kernel_driver(0)
        except usb.core.USBError as exception:
            sys.exit("Could not detatch kernel driver: %s" % str(exception))

    try:
        device.set_configuration()
    except usb.core.USBError as exception:
        sys.exit("Could not set configuration: %s" % str(exception))

    return device

def send_led_pattern(device, led1, led2, led3, led4):
    # Report data for the demo is LED on/off data
    report_data = [led1, led2, led3, led4]

    # Send the generated report to the device
    number_of_bytes_written = device.ctrl_transfer(  # Set Report control request
        0b00100001,  # bmRequestType (constant for this control request)
        0x09,        # bmRequest (constant for this control request)
        0,           # wValue (MSB is report type, LSB is report number)
        0,           # wIndex (interface number)
        report_data  # report data to be sent
    );
    assert number_of_bytes_written == len(report_data)

    print("Sent LED Pattern: {0}".format(report_data))

def receive_led_pattern(hid_device):
    endpoint = hid_device[0][(0,0)][0]
    report_data = hid_device.read(endpoint.bEndpointAddress, endpoint.wMaxPacketSize)
    return list(report_data)

def main():    
    hid_device = get_and_init_hid_device()

    print("Connected to device 0x%04X/0x%04X - %s [%s]" %
          (hid_device.idVendor, hid_device.idProduct,
           usb.util.get_string(hid_device, 256, hid_device.iProduct),
           usb.util.get_string(hid_device, 256, hid_device.iManufacturer)))

    p = 0
    #while (True):
    #    # Convert the current pattern index to a bit-mask and send
    #    send_led_pattern(hid_device,
    #                     (p >> 3) & 1,
    #                     (p >> 2) & 1,
    #                     (p >> 1) & 1,
    #                     (p >> 0) & 1)##
    #
    #        # Receive and print the current LED pattern
    #    led_pattern = receive_led_pattern(hid_device)[0:4]
    #    print("Received LED Pattern: {0}".format(led_pattern))
    #
    #    # Compute next LED pattern in sequence
    #    p = (p + 1) % 16#
    #
    #    # Delay a bit for visual effect
    #    sleep(.2)

if __name__ == '__main__':
    main()

