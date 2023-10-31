######## HARDWARE ON BOARD

# LEDs
set_property -dict {DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN R1} [get_ports led1]
set_property -dict {DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN V2} [get_ports led2]
set_property -dict {DRIVE 8 IOSTANDARD LVCMOS33 PACKAGE_PIN V5} [get_ports led3]

# clocks
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN D15} [get_ports clk]
create_clock -period 20.000 -name clk -waveform {0.000 10.000} [get_nets clk]

set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN A13} [get_ports clkout]

# IO1-4
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN V10} [get_ports txd]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN V11} [get_ports rxd]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN V12} [get_ports io3]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN V14} [get_ports io4]

# misc pins
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN A16} [get_ports resetn]

# output delays
set_input_delay -clock clk 0.000 [get_ports resetn]
set_input_delay -clock clk 0.000 [get_ports rxd]
set_false_path -from [get_ports resetn]
set_false_path -from [get_ports rxd]

# output delays
set_output_delay -clock clk 0.000 [get_ports led*]
set_output_delay -clock clk 0.000 [get_ports txd]
set_output_delay -clock clk 0.000 [get_ports io3]
set_output_delay -clock clk 0.000 [get_ports io4]
set_false_path -to [get_ports led*]
set_false_path -to [get_ports txd]
set_false_path -to [get_ports io3]
set_false_path -to [get_ports io4]

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]

set_property BITSTREAM.CONFIG.USR_ACCESS TIMESTAMP [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
