`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the computer interface. It provides a simple interface to the
rest of the board.

Copyright (c) 2014-2016, NewAE Technology Inc. All rights reserved.

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
	
	/* Interface to ChipWhisperer-CW1200 USB Chip */
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
	 
	 wire 		rdflag = ~cwusb_rdn;// & ~cwusb_cen;

	 reg rdflag_rs, rdflag_rs_dly;
	 always @(posedge usb_clk) rdflag_rs <= rdflag;
	 always @(posedge usb_clk) rdflag_rs_dly <= rdflag_rs;
	 
	 //TODO: this should be synchronous to device clock, but is phase OK? Might need to
	 //use resyncronized version...
	 assign		reg_read = rdflag & ~rdflag_rs;
	 
	 //rdflag_rs & ~rdflag_rs_dly;
	 
	 assign 		cwusb_dout = reg_datai; 
	 
	 reg isoutreg, isoutregdly;
	 
	 always @(posedge usb_clk) begin
		isoutreg <= rdflag;
		isoutregdly <= isoutreg;
	 end
	 
	 //Don't immediatly turn off output drivers
	 assign cwusb_isout = isoutreg | isoutregdly | rdflag;
		 
	 //Address valid on rising edge of ALEn
	 assign reg_hypaddress = cwusb_addr[5:0];
	 reg [5:0] address;
	 assign reg_address = address;
	 always @(posedge cwusb_alen)
	     address <= cwusb_addr[5:0];
	 
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
	 
	 reg cwusb_wrn_rs, cwusb_wrn_rs_dly, cwusb_wrn_rs_dly2;
	 always @(posedge usb_clk) cwusb_wrn_rs <= cwusb_wrn;
	 always @(posedge usb_clk) cwusb_wrn_rs_dly <= cwusb_wrn_rs;
	 always @(posedge usb_clk) cwusb_wrn_rs_dly2 <= cwusb_wrn_rs_dly;
	  
	 reg [7:0] cwusb_databuf;
	 
	 //Sync to rising edge of WRn
	 always @(posedge cwusb_wrn) begin
			cwusb_databuf <= cwusb_din;
	 end
	 
	 //Data isn't resync'd, but write is delayed slightly
	 assign reg_datao = cwusb_databuf;	 
	 reg regout_write;
	 always @(posedge usb_clk) begin
		 regout_write <= cwusb_wrn_rs_dly2 & ~cwusb_wrn_rs_dly;
	 end
	 
	 assign reg_write = regout_write;
	 
	 /* Byte count block. We need to increment after a read or after a write */
	 reg regout_write_dly;
	 always @(posedge usb_clk) regout_write_dly <= regout_write;
	 
	 reg regout_read;
	 reg regout_read_dly;
	 always @(posedge usb_clk) regout_read <= cwusb_rdn_rs_dly & ~cwusb_rdn_rs;	 
	 always @(posedge usb_clk) regout_read_dly <= regout_read;

	 always @(posedge usb_clk) begin
		if (cwusb_alen_rs == 1'b0) begin
			regout_bytecnt <= 0;
		end else if ((regout_read_dly) || (regout_write_dly) ) begin
			//Stop roll-over
			if (regout_bytecnt != 16'hFFFF) begin
				regout_bytecnt <= regout_bytecnt + 1;			 
			end
		end
	 end
	 
`ifdef CHIPSCOPE
	wire [35:0] CONTROL0;
	wire [63:0] cs_data;
	
	assign cs_data[7:0] = cwusb_databuf;
	assign cs_data[15:8] = reg_datai;
	assign cs_data[21:16] = address;
	assign cs_data[22] = regout_addrvalid;
	assign cs_data[23] = regout_write;
	assign cs_data[24] = rdflag_rs;
	assign cs_data[25] = cwusb_rdn;
	assign cs_data[26] = cwusb_cen;
	
	coregen_icon icon (
    .CONTROL0(CONTROL0) // INOUT BUS [35:0]
	);
	
	coregen_ila ila (
    .CONTROL(CONTROL0), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [63:0]
	);
`endif
	 
 
endmodule

`undef CHIPSCOPE
