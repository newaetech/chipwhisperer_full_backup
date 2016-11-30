`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the computer interface. It provides a simple interface to the
rest of the board.

Copyright (c) 2014, NewAE Technology Inc. All rights reserved.

Author: Colin O'Flynn, <coflynn@newae.com>

This project (and file) is released under the 2-Clause BSD License:

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*************************************************************************/
module reg_main_cwlite(
	input 			reset_i,
	input 			clk,
	
	/* Interface to ChipWhisperer-Lite USB Chip */
	input	[7:0]		cwusb_din,
	output [7:0]	cwusb_dout,
	output 			cwusb_isout,
	input [7:0]		cwusb_addr,
	input				cwusb_rdn,
	input				cwusb_wrn,
	input				cwusb_alen,
	input				cwusb_cen,
		
	/* Interface to registers */
	output         reg_clk,
	output [5:0]   reg_address,  // Address of register
	output [15:0]  reg_bytecnt,  // Current byte count
	output [7:0]   reg_datao,    // Data to write
	input  [7:0]   reg_datai,    // Data to read
	output [15:0]  reg_size,     // Total size being read/write
	output         reg_read,     // Read flag. One clock cycle AFTER this flag is high
	                             // valid data must be present on the reg_datai bus
	output			reg_write,    // Write flag. When high on rising edge valid data is
	                             // present on reg_datao
	output         reg_addrvalid,// Address valid flag
	input				reg_stream,	
	
	output [5:0]   reg_hypaddress,
	input  [15:0]  reg_hyplen	
    );
	 
`ifdef CHIPSCOPE
	 wire [31:0] cs_data;
	 
	 assign cs_data[7:0] = cwusb_din;
	 assign cs_data[15:8] = reg_datai;
	 assign cs_data[16] = cwusb_rdn;
	 assign cs_data[17] = cwusb_wrn;
	 assign cs_data[18] = rdflag;
	 assign cs_data[19] = regout_write;	 
	 assign cs_data[31:20] = reg_bytecnt[11:0];
`endif
		 
	 wire usb_clk;
	 assign usb_clk = clk;
	 assign reg_clk = clk;
		 
	 reg cwusb_alen_rs, cwusb_alen_rs_dly;
	 always @(posedge usb_clk) cwusb_alen_rs <= cwusb_alen;
	 always @(posedge usb_clk) cwusb_alen_rs_dly <= cwusb_alen_rs;
	 
	 reg [15:0]	regout_bytecnt;
	 reg 			regout_addrvalid;
	 assign 		reg_addrvalid = regout_addrvalid;
	 assign 		reg_bytecnt = regout_bytecnt;
	 assign	   reg_size = 0;
	 
	 wire 		rdflag = ~cwusb_rdn & ~cwusb_cen;
	 reg [7:0]  reg_datai_buf;
	 reg rdflag_rs, rdflag_rs_dly;
	 always @(posedge usb_clk) rdflag_rs <= rdflag;
	 always @(posedge usb_clk) rdflag_rs_dly <= rdflag_rs;
	 
	 always @(posedge usb_clk) begin
		if (rdflag_rs_dly)
			reg_datai_buf <= reg_datai;
	 end
	 
	 //TODO: this should be synchronous to device clock, but is phase OK? Might need to
	 //use resyncronized version...
	 assign		reg_read = rdflag_rs;
	 assign 		cwusb_dout = reg_datai;	 //reg_datai_buf 
	 
	 reg isoutreg, isoutregdly;
	 
	 always @(posedge usb_clk) begin
		isoutreg <= ~cwusb_rdn;
		isoutregdly <= isoutreg;
	 end
	 
	 //Don't immediatly turn off output drivers
	 assign cwusb_isout = isoutreg | isoutregdly;
		 
	 //Address valid on rising edge of ALEn, simplify and just latch when ALEn low
	 assign reg_hypaddress = cwusb_addr[5:0];
	 reg [5:0] address;
	 assign reg_address = address;
	 always @(posedge usb_clk) begin
		if (cwusb_alen_rs_dly == 1'b0) begin
			address <= cwusb_addr[5:0];
		end
	 end
	 
	 //Address valid from ALEn until transaction done marked OR next falling edge of ALEn
	 reg addrvalid_outreg;
	 always @(posedge usb_clk) begin
		if (cwusb_alen_rs == 1'b0) begin //TODO: Add 'transaction done' flag
			regout_addrvalid <= 1'b0;
		end else if ((cwusb_alen_rs == 1'b1) &&(cwusb_alen_rs_dly == 1'b0)) begin
			regout_addrvalid <= 1'b1;
		end
	 end
	 
	 reg cwusb_rdn_rs, cwusb_rdn_rs_dly;
	 always @(posedge usb_clk) cwusb_rdn_rs <= cwusb_rdn;
	 always @(posedge usb_clk) cwusb_rdn_rs_dly <= cwusb_rdn_rs;
	 
	 reg cwusb_wrn_rs, cwusb_wrn_rs_dly;
	 always @(posedge usb_clk) cwusb_wrn_rs <= cwusb_wrn;
	 always @(posedge usb_clk) cwusb_wrn_rs_dly <= cwusb_wrn_rs;
	  
	 reg [7:0] cwusb_databuf;
	 
	 //Sync to rising edge of WRn
	 always @(posedge cwusb_wrn) begin
			cwusb_databuf <= cwusb_din;
	 end
	 
	 //Resync to send on to system
	 assign reg_datao = cwusb_databuf;
	 
	 reg regout_write;
	 always @(posedge usb_clk) begin
		 regout_write <= cwusb_wrn_rs & ~cwusb_wrn_rs_dly;
	 end
	 
	 assign reg_write = regout_write;
	 
	 /* Byte count block. We need to increment after a read or after a write */
	 reg regout_write_dly;
	 always @(posedge usb_clk) regout_write_dly <= regout_write;


	 always @(posedge usb_clk) begin
		if (cwusb_alen_rs == 1'b0) begin
			regout_bytecnt <= 0;
		end else if ((rdflag_rs_dly) || (regout_write_dly) ) begin
			//Stop roll-over
			if (regout_bytecnt != 16'hFFFF) begin
				regout_bytecnt <= regout_bytecnt + 1;			 
			end
		end
	 end
/*
 `define CHIPSCOPE
`ifdef CHIPSCOPE	
	 wire [35:0] cs_ctrl;
	
	wire [63:0] ila_trigbus;
	wire [35:0] cs_control0;
	assign ila_trigbus[7:0] = cwusb_dout;
	assign ila_trigbus[15:8] = cwusb_addr;
	assign ila_trigbus[16] = cwusb_rdn;
	assign ila_trigbus[17] = cwusb_wrn;
	assign ila_trigbus[18] = cwusb_isout;
	assign ila_trigbus[19] = cwusb_alen;
	
	assign ila_trigbus[31:24] = reg_datai;

	coregen_icon csicon (
    .CONTROL0(cs_control0) // INOUT BUS [35:0]
	);
	
	coregen_ila csila (
    .CONTROL(cs_control0), // INOUT BUS [35:0]
    .CLK(usb_clk), // IN
    .TRIG0(ila_trigbus) // IN BUS [63:0]
	 );
`endif
*/
 
endmodule

`undef CHIPSCOPE
