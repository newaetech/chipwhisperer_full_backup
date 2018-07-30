"""Setup script for CWLite/1200 with ESP32 Target (CW308T-ESP32)

Configures scope settings to prepare for capturing SimpleSerial power traces. Also allows programming via
external USB serial cable connected.
"""

import time

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 55
scope.gain.mode = 'high'
scope.adc.samples = 5000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7372800
scope.clock.adc_src = "clkgen_x4"
scope.clock.adc_phase = 128
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.tio3 = True #Run condition
scope.io.pdic = False #SPI forwarding off
scope.io.hs2 = "clkgen"

target.baud = 38400

print("\n***ESP 32 Target Usage Notes***")
print("   Call set_bootloader to enter BOOTLOADER so external USB-Serial cable will work.")
print("   Call set_run to enter RUN mode.")
print("   Jumper TDI to GND to disable boot-up messages (reduce noise if looking at boot decryption).")


def set_bootloader():
    scope.io.tio3 = False
    scope.io.tio1 = None
    scope.io.tio2 = None
 
    #Toggle reset
    scope.io.nrst = False
    time.sleep(0.05)
    scope.io.nrst = None #Pulls high by default
   
def set_run():
    scope.io.tio3 = True
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    
    #Toggle reset
    scope.io.nrst = False
    time.sleep(0.05)
    scope.io.nrst = None #Pulls high by default
    