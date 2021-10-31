`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the OpenADC IO Trigger registers.

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
module reg_usi(
	input 			reset_i,
	input 			clk,
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
	output [15:0]  reg_hyplen,
	
	output			usi_out,
	input				usi_in									              
   );
	 
	 wire	  reset;
	 assign reset = reset_i;
	 assign reg_stream = 1'b0;

`ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
`endif
        	  
	 `define USICLKDDIV_ADDR	44
	 `define USIPROG_ADDR		45 
	 `define USISTATUS_ADDR		46
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `USICLKDDIV_ADDR: reg_hyplen_reg <= 3;
				`USIPROG_ADDR: reg_hyplen_reg <= 4;
				`USISTATUS_ADDR: reg_hyplen_reg <= 4;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;
	 
	 reg [31:0] clkdiv_reg;
	 reg [31:0] prog_reg;
	 reg [31:0] status_reg;
	 	 
	 wire [31:0] status_reg_read;
	 wire [7:0] prog_data;
	 wire [15:0]	prog_addr;
	 reg			prog_en;
	 reg			prog_wr;
	 
	 wire [17:0]	clkdiv;
	 wire			rst_core;
	 
	 assign prog_data = prog_reg[7:0];
	 assign prog_addr = prog_reg[23:8];
	 
	 assign clkdiv = clkdiv_reg[17:0];
	 assign rst_core = clkdiv_reg[31];
	 
	 /*
	 reg reg_datao_valid_reg;
	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`USICLKDDIV_ADDR: begin reg_datao_valid_reg <= 1; end
				`USIPROG_ADDR: begin reg_datao_valid_reg <= 1; end
				`USISTATUS_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 */
	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`USICLKDDIV_ADDR: begin reg_datao_reg <= clkdiv_reg[reg_bytecnt*8 +: 8]; end
				`USIPROG_ADDR: begin reg_datao_reg <= prog_reg_read[reg_bytecnt*8 +: 8];	end
				`USISTATUS_ADDR: begin reg_datao_reg <= status_reg_read[reg_bytecnt*8 +: 8]; end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 always @(posedge clk) begin
		if (reset) begin
			clkdiv_reg <= 0;
			prog_reg <= 0;
			status_reg <= 1;
		end else if (reg_write) begin
			case (reg_address)
				`USICLKDDIV_ADDR: clkdiv_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
				`USIPROG_ADDR: prog_reg[reg_bytecnt*8 +: 8] <= reg_datai;
				`USISTATUS_ADDR: status_reg[reg_bytecnt*8 +: 8] <= reg_datai;
				default: ;
			endcase
		end
	 end	 


	 reg prog_done;
	 always @(posedge clk)
		prog_done <= prog_wr;
	 
						  
	 always @(posedge clk) begin
	  if (prog_done)
			prog_en <= 0;
	  else if ((reg_write) & (reg_address == `USIPROG_ADDR))
			prog_en <= 1;	  
	 end
	 
	 reg prog_dowrnext;
	 always @(posedge clk)
		prog_wr <= prog_dowrnext;
	 
	 always @(posedge clk)
		if ((reg_write) & (reg_address == `USIPROG_ADDR) & (reg_bytecnt == 3))
			prog_dowrnext <= 1;
		else
			prog_dowrnext <= 0;
			
	assign status_reg_read[31:4] = status_reg[31:4];
	assign status_reg_read[2] = status_reg[2];
	assign status_reg_read[0] = status_reg[0];
	
	/* 
	 0x?? - Status Register (Four Bytes)
	 
	   [   TX States LSB         ]
		[   RX States LSB         ]
		[   TX States MSB (4 bits) | RX States MSB (4 bits)  ]	 
	   [ X  X  X  RS RD TS TD I ]
	     
		  I = (bit 0) Idle State of TX/RX (default = 1)
		  TD = TX Done
		  TS = TX Start. Set high & clear once TD goes high
		  RD = RX Done
		  RS = RX Start. Set high & clear once RD goes high
	*/
	
	wire [31:0] prog_reg_read;
	assign prog_reg_read[31:8] = prog_reg[31:8];
	 
	usitx usi_systemtx (
		.clk(clk), 
		.rst(rst_core | reset), 
		.idle(status_reg[0]),
		.dataout(usi_out),
		.run(status_reg[2]),
		.done(status_reg_read[1]),
		.num_states({status_reg[15:12], status_reg[31:24]}),
		 
		.clkdivider(clkdiv), 
		.state_prog_en(prog_en & prog_reg[31]), 
		.state_prog_addr(prog_addr), 
		.state_prog_wr(prog_wr), 
		.state_prog_data(prog_data)
	);
	
	usirx usi_systemrx (
		.clk(clk), 
		.rst(rst_core | reset), 
		.idle(status_reg[0]),
		.datain(usi_in),
		.run(status_reg[4]),
		.done(status_reg_read[3]),
		.num_states({status_reg[11:8], status_reg[23:16]}),
		 
		.clkdivider(clkdiv), 
		.state_read_en(prog_en), 
		.state_read_addr(prog_addr), 
		.state_read_data(prog_reg_read[7:0])
	);
	 
	 
 `ifdef CHIPSCOPE
	 assign cs_data[17:0] = prog_data;
	 assign cs_data[23:18] = prog_addr;
	 assign cs_data[24] = prog_wr;
	 assign cs_data[25] = prog_en;
	 assign cs_data[26] = clkdiv;
	 assign cs_data[27] = io_line;
	 assign cs_data[28] = trig_out;
	 assign cs_data[46:29] = clkdiv;
	 assign cs_data[47] = rst_core;
	 
 `endif
 
endmodule

`undef CHIPSCOPE
