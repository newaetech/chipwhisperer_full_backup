############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2012 Xilinx Inc. All rights reserved.
############################################################
open_project test_smartcard
set_top smartcard
add_files smartcard.c
add_files -tb smartcard_tb.c
open_solution "smartcard_test"
set_part  {xc7k70tfbg484-1}
create_clock -period 16

source "./test_smartcard/smartcard_test/directives.tcl"
csynth_design
