######## HARDWARE ON BOARD

# LEDs
set_property -dict { DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN R1 } [get_ports LED[1]]
set_property -dict { DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN V2 } [get_ports LED[2]]
set_property -dict { DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN V5 } [get_ports LED[3]]

# clocks
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN A13 } [get_ports CLKOUT]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN D15 } [get_ports CLKIN]
create_clock -period 7.000 -name clk -waveform {0.000 3.500} [get_nets CLKIN]
#set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets clk]

# IO1-4
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V10 } [get_ports IO1]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V11 } [get_ports IO2]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V12 } [get_ports IO3]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V14 } [get_ports IO4]

# HDR
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN L1 } [get_ports HDR[1]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN M1 } [get_ports HDR[2]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN N1 } [get_ports HDR[3]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN T1 } [get_ports HDR[4]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN U1 } [get_ports HDR[5]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V1 } [get_ports HDR[6]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V4 } [get_ports HDR[7]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V6 } [get_ports HDR[8]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V7 } [get_ports HDR[9]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V9 } [get_ports HDR[10]]

# misc pins
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN A16 } [get_ports nRST]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V15 } [get_ports SCK]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN L18 } [get_ports MISO]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN K18 } [get_ports MOSI]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V16 } [get_ports nRST_OUT]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN V17 } [get_ports TRACECLK]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN U18 } [get_ports TRACEDATA[0]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN T18 } [get_ports TRACEDATA[1]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN R18 } [get_ports TRACEDATA[2]]
set_property -dict { IOSTANDARD LVCMOS33 PACKAGE_PIN P18 } [get_ports TRACEDATA[3]]

# pull downs
set_property PULLTYPE PULLDOWN [get_ports IO3]
set_property PULLTYPE PULLDOWN [get_ports HDR*]

# output delays
set_input_delay -clock clk 0.000 [get_ports HDR*]
set_input_delay -clock clk 0.000 [get_ports IO2]
set_input_delay -clock clk 0.000 [get_ports IO3]
set_false_path -from [get_ports HDR*]
set_false_path -from [get_ports IO2]
set_false_path -from [get_ports IO3]

# output delays
set_output_delay -clock clk 0.000 [get_ports LED*]
set_output_delay -clock clk 0.000 [get_ports IO1]
set_output_delay -clock clk 0.000 [get_ports IO4]
set_false_path -to [get_ports LED*]
set_false_path -to [get_ports IO1]
set_false_path -to [get_ports IO4]

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

