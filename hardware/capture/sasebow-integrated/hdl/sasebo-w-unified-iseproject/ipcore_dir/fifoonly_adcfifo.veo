/*******************************************************************************
*     This file is owned and controlled by Xilinx and must be used solely      *
*     for design, simulation, implementation and creation of design files      *
*     limited to Xilinx devices or technologies. Use with non-Xilinx           *
*     devices or technologies is expressly prohibited and immediately          *
*     terminates your license.                                                 *
*                                                                              *
*     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" SOLELY     *
*     FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR XILINX DEVICES.  BY     *
*     PROVIDING THIS DESIGN, CODE, OR INFORMATION AS ONE POSSIBLE              *
*     IMPLEMENTATION OF THIS FEATURE, APPLICATION OR STANDARD, XILINX IS       *
*     MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION IS FREE FROM ANY       *
*     CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE FOR OBTAINING ANY        *
*     RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY        *
*     DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE    *
*     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR           *
*     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF          *
*     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A    *
*     PARTICULAR PURPOSE.                                                      *
*                                                                              *
*     Xilinx products are not intended for use in life support appliances,     *
*     devices, or systems.  Use in such applications are expressly             *
*     prohibited.                                                              *
*                                                                              *
*     (c) Copyright 1995-2013 Xilinx, Inc.                                     *
*     All rights reserved.                                                     *
*******************************************************************************/

/*******************************************************************************
*     Generated from core with identifier: xilinx.com:ip:fifo_generator:8.2    *
*                                                                              *
*     The FIFO Generator is a parameterizable first-in/first-out memory        *
*     queue generator. Use it to generate resource and performance             *
*     optimized FIFOs with common or independent read/write clock domains,     *
*     and optional fixed or programmable full and empty flags and              *
*     handshaking signals.  Choose from a selection of memory resource         *
*     types for implementation.  Optional Hamming code based error             *
*     detection and correction as well as error injection capability for       *
*     system test help to insure data integrity.  FIFO width and depth are     *
*     parameterizable, and for native interface FIFOs, asymmetric read and     *
*     write port widths are also supported.                                    *
*******************************************************************************/

// Interfaces:
//    AXI4Stream_MASTER_M_AXIS
//    AXI4Stream_SLAVE_S_AXIS
//    AXI4_MASTER_M_AXI
//    AXI4_SLAVE_S_AXI
//    AXI4Lite_MASTER_M_AXI
//    AXI4Lite_SLAVE_S_AXI

// The following must be inserted into your Verilog file for this
// core to be instantiated. Change the instance name and port connections
// (in parentheses) to your own signal names.

//----------- Begin Cut here for INSTANTIATION Template ---// INST_TAG
fifoonly_adcfifo your_instance_name (
  .rst(rst), // input rst
  .wr_clk(wr_clk), // input wr_clk
  .rd_clk(rd_clk), // input rd_clk
  .din(din), // input [31 : 0] din
  .wr_en(wr_en), // input wr_en
  .rd_en(rd_en), // input rd_en
  .prog_full_thresh(prog_full_thresh), // input [13 : 0] prog_full_thresh
  .dout(dout), // output [127 : 0] dout
  .full(full), // output full
  .empty(empty), // output empty
  .rd_data_count(rd_data_count), // output [12 : 0] rd_data_count
  .prog_full(prog_full) // output prog_full
);
// INST_TAG_END ------ End INSTANTIATION Template ---------

// You must compile the wrapper file fifoonly_adcfifo.v when simulating
// the core, fifoonly_adcfifo. When compiling the wrapper file, be sure to
// reference the XilinxCoreLib Verilog simulation library. For detailed
// instructions, please refer to the "CORE Generator Help".

