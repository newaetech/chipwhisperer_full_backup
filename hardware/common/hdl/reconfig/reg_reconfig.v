`include "includes.v"
`timescale 1ns / 1ps
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the Partial Reconfiguration (using ICAP) interface.

Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
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

module reg_reconfig(
	input 			reset_i,
	input 			clk,
	input          icap_clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	output [7:0]   reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output [15:0]  reg_hyplen
    );

	wire icap_ce;
	wire icap_wr;
	wire [15:0] icap_in;
	
	reg fifo_wr_en;
	reg fifo_rd_en;
	wire [17:0] fifo_din;
	wire [17:0] fifo_dout;
	
	//Data ordering required by ICAP (same as SelectMAP) differs from .bit file
	//assign icap_in[15:0] = {fifo_dout[8:15], fifo_dout[0:7]};
	assign icap_in[15] = fifo_dout[8];
	assign icap_in[14] = fifo_dout[9];
	assign icap_in[13] = fifo_dout[10];
	assign icap_in[12] = fifo_dout[11];
	assign icap_in[11] = fifo_dout[12];
	assign icap_in[10] = fifo_dout[13];
	assign icap_in[9] = fifo_dout[14];
	assign icap_in[8] = fifo_dout[15];
	assign icap_in[7] = fifo_dout[0];
	assign icap_in[6] = fifo_dout[1];
	assign icap_in[5] = fifo_dout[2];
	assign icap_in[4] = fifo_dout[3];
	assign icap_in[3] = fifo_dout[4];
	assign icap_in[2] = fifo_dout[5];
	assign icap_in[1] = fifo_dout[6];
	assign icap_in[0] = fifo_dout[7];
	
	`define START_PATTERN 5'h1A
	`define RECONFIG_ADDR 52
	
	reg [7:0]  icap_statusreg;
	
	/* 
	 [ 7 6 5 4 3 2 1 0 ]
	   x x R G G G G G               
		
		      G G G G G = 1 1 0 1 0 (0x1A) start pattern
			 R = Reset bit
			 
	 The first byte is written to the status register, remaining bytes written to FIFO.
	 
	 To use interface:
	 * Split bitstream into byte chunks
	 * Write (1<<5) to reconfig addr
	 * Write 0x00 to reconfig addr
	 * Prepend a 0 to bitstream chunks (required as first byte will go into status/control register)
	 * Write chunks with prepended 0 into reconfig addr
	 * Write 0x1A to reconfig addr (start pattern)
	 * Write 0x00 to reconfig addr		
	*/
	reg [15:0] reg_hyplen_reg;
	assign reg_hyplen = reg_hyplen_reg;
	
	always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `RECONFIG_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    	
	
	 wire fifo_rst;
         wire fifo_empty;
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;
	
	assign fifo_rst = icap_statusreg[5];
	
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`RECONFIG_ADDR: begin reg_datao_reg <= icap_statusreg; end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end	

	 reg [15:0] data_in;
	 reg dowr;
	 
	 always @(posedge clk)
		fifo_wr_en <= dowr; 

	 always @(posedge clk) begin
		if (reset_i) begin
			icap_statusreg <= 0;
		end else if ((icap_statusreg[4:0] == `START_PATTERN) & (fifo_empty)) begin
			icap_statusreg <= 0;
		end else if (reg_write) begin
			if (reg_bytecnt == 0) begin
				case (reg_address)
					`RECONFIG_ADDR: icap_statusreg <= reg_datai;	
					default: ;
				endcase
			end else begin
				if (reg_bytecnt[0] == 1'b1) begin
					data_in[15:8] <= reg_datai;
				end else begin
					data_in[7:0] <= reg_datai;				
				end
			end
		end
	 end	

	 always @(posedge clk) begin
		if (reset_i)
			dowr <= 0;
		else if ((reg_write) & (reg_bytecnt != 0) & (reg_bytecnt[0] == 1'b0) & (reg_address == `RECONFIG_ADDR))
			dowr <= 1;
		else
			dowr <= 0;
	 end	

	assign fifo_din[15:0] = data_in;
	assign fifo_din[17:16] = 2'b00;

	reg do_config;
	always @(posedge clk) begin
		 do_config <= (icap_statusreg[4:0] == `START_PATTERN) ? 1'b1 : 1'b0;
	end
	
	//Cross clock domain
	reg icap_cfg;
	reg icap_cfg2;
	always @(posedge icap_clk) begin
		icap_cfg2 <= do_config;
		icap_cfg <= icap_cfg2;
	end
	
        `ifndef __ICARUS__
	// ICAP_SPARTAN6: Internal Configuration Access Port
	// Spartan-6
	ICAP_SPARTAN6 #(
	
	//'h04004093
	.DEVICE_ID(`FPGA_IDCODE), // Specifies the pre-programmed Device ID value
	.SIM_CFG_FILE_NAME("NONE") // Specifies the Raw Bitstream (RBT) file to be parsed by the simulation
	// model
	)
	
	ICAP_SPARTAN6_inst (
		.BUSY(), // 1-bit Busy/Ready output
		.O(), // 16-bit Configuartion data output bus
		.CE(~icap_cfg), // 1-bit Active-Low ICAP Enable input
		.CLK(icap_clk), // 1-bit Clock input
		.I(icap_in), // 16-bit Configuration data input bus
		.WRITE(1'b0) // 1-bit Read/Write control input
	);
	// End of ICAP_SPARTAN6_inst instantiation

	icap_fifo icap_fifo (
		.rst(reset_i | fifo_rst),
		.wr_clk(clk),
		.rd_clk(icap_clk),
		.din(fifo_din),
		.wr_en(fifo_wr_en),
		.rd_en(icap_cfg & ~fifo_empty),
		.dout(fifo_dout),
		.full(fifo_full),
		.empty(fifo_empty),
		.almost_empty()
		);
        `endif

`ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(icap_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
	
	assign cs_data[15:0] = icap_in;
	assign cs_data[16] = fifo_wr_en;
	assign cs_data[17] = icap_cfg;
	assign cs_data[18] = fifo_empty;	
	
`endif

endmodule
`undef CHIPSCOPE
