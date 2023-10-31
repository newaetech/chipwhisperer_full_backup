
######## HARDWARE ON BOARD

#LEDs
set_property DRIVE 8 [get_ports led1]
set_property PACKAGE_PIN T2 [get_ports led1]

set_property DRIVE 8 [get_ports led2]
set_property PACKAGE_PIN T3 [get_ports led2]

set_property DRIVE 8 [get_ports led3]
set_property PACKAGE_PIN T4 [get_ports led3]

####### USB Connector

set_property PACKAGE_PIN F5 [get_ports usb_clk]

set_property IOSTANDARD LVCMOS33 [get_ports *]
set_property PACKAGE_PIN L12 [get_ports flash_cs]
set_property PACKAGE_PIN J13 [get_ports flash_dout]

set_property PACKAGE_PIN B4 [get_ports usb_spare1]
set_property PACKAGE_PIN B1 [get_ports usb_spare2]

create_clock -period 10.000 -name usb_clk -waveform {0.000 5.000} [get_nets usb_clk]