
######## HARDWARE ON BOARD

#LEDs
set_property DRIVE 8 [get_ports led1]
set_property PACKAGE_PIN T2 [get_ports led1]

set_property DRIVE 8 [get_ports led2]
set_property PACKAGE_PIN T3 [get_ports led2]

set_property DRIVE 8 [get_ports led3]
set_property PACKAGE_PIN T4 [get_ports led3]

#Switch's
set_property PACKAGE_PIN J16 [get_ports sw1]

set_property PACKAGE_PIN K16 [get_ports sw2]

set_property PACKAGE_PIN K15 [get_ports sw3]

set_property PACKAGE_PIN L14 [get_ports sw4]

set_property PACKAGE_PIN R1 [get_ports pushbutton]

#PLL Connections
set_property PACKAGE_PIN N13 [get_ports pll_clk1]
#set_property PACKAGE_PIN E12 [get_ports pll_clk2]

######## 20-Pin Connector

set_property PACKAGE_PIN T14 [get_ports tio_trigger]

set_property PACKAGE_PIN M16 [get_ports tio_clkout]

set_property PACKAGE_PIN N14 [get_ports tio_clkin]

####### USB Connector

set_property PACKAGE_PIN F5 [get_ports usb_clk]

set_property IOSTANDARD LVCMOS33 [get_ports *]
set_property PACKAGE_PIN A7 [get_ports {usb_data[0]}]
set_property PACKAGE_PIN B6 [get_ports {usb_data[1]}]
set_property PACKAGE_PIN D3 [get_ports {usb_data[2]}]
set_property PACKAGE_PIN E3 [get_ports {usb_data[3]}]
set_property PACKAGE_PIN F3 [get_ports {usb_data[4]}]
set_property PACKAGE_PIN B5 [get_ports {usb_data[5]}]
set_property PACKAGE_PIN K1 [get_ports {usb_data[6]}]
set_property PACKAGE_PIN K2 [get_ports {usb_data[7]}]

set_property PACKAGE_PIN F4 [get_ports {usb_addr[0]}]
set_property PACKAGE_PIN G5 [get_ports {usb_addr[1]}]
set_property PACKAGE_PIN J1 [get_ports {usb_addr[2]}]
set_property PACKAGE_PIN H1 [get_ports {usb_addr[3]}]
set_property PACKAGE_PIN H2 [get_ports {usb_addr[4]}]
set_property PACKAGE_PIN G1 [get_ports {usb_addr[5]}]
set_property PACKAGE_PIN G2 [get_ports {usb_addr[6]}]
set_property PACKAGE_PIN F2 [get_ports {usb_addr[7]}]
set_property PACKAGE_PIN E1 [get_ports {usb_addr[8]}]
set_property PACKAGE_PIN E2 [get_ports {usb_addr[9]}]
set_property PACKAGE_PIN D1 [get_ports {usb_addr[10]}]
set_property PACKAGE_PIN C1 [get_ports {usb_addr[11]}]
set_property PACKAGE_PIN K3 [get_ports {usb_addr[12]}]
set_property PACKAGE_PIN L2 [get_ports {usb_addr[13]}]
set_property PACKAGE_PIN J3 [get_ports {usb_addr[14]}]
set_property PACKAGE_PIN B2 [get_ports {usb_addr[15]}]
set_property PACKAGE_PIN C7 [get_ports {usb_addr[16]}]
set_property PACKAGE_PIN C6 [get_ports {usb_addr[17]}]
set_property PACKAGE_PIN D6 [get_ports {usb_addr[18]}]
set_property PACKAGE_PIN C4 [get_ports {usb_addr[19]}]
set_property PACKAGE_PIN D5 [get_ports {usb_addr[20]}]

set_property PACKAGE_PIN A4 [get_ports usb_rdn]
set_property PACKAGE_PIN C2 [get_ports usb_wrn]
set_property PACKAGE_PIN A3 [get_ports usb_cen]

set_property PACKAGE_PIN A5 [get_ports usb_trigger]

#set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets usb_clk]
#set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets crypt_clk]


create_clock -period 10.000 -name usb_clk -waveform {0.000 5.000} [get_nets usb_clk]
#create_clock -period 10.000 -name usb_clk -waveform {0.000 5.000} [get_nets tio_clkin]

set_input_delay -clock [get_clocks -filter { NAME =~  "*usb_clk*" }] 3.000 [get_ports -filter { NAME =~  "*usb_data*" && DIRECTION == "INOUT" }]
