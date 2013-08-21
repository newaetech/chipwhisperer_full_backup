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
module reg_triggerio(
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
	
	input				io_line,
	output reg		trig_out									              
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
        	  
	 `define IOTRIGCLKDDIV_ADDR	36
	 `define IOTRIGPROG_ADDR		37 
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `IOTRIGCLKDDIV_ADDR: reg_hyplen_reg <= 3;
				`IOTRIGPROG_ADDR: reg_hyplen_reg <= 4;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 assign reg_datao = reg_datao_reg;
	 
	 reg [31:0] clkdiv_reg;
	 reg [31:0] prog_reg;
	 	 
	 wire [17:0] prog_data;
	 wire [7:0]	prog_addr;
	 reg			prog_en;
	 reg			prog_wr;
	 
	 wire [17:0]	clkdiv;
	 wire			rst_core;
	 
	 assign prog_data = prog_reg[17:0];
	 assign prog_addr = prog_reg[31:24];
	 
	 assign clkdiv = clkdiv_reg[17:0];
	 assign rst_core = clkdiv_reg[31];
	 
	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`IOTRIGCLKDDIV_ADDR: begin reg_datao_valid_reg <= 1; end
				`IOTRIGPROG_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`IOTRIGCLKDDIV_ADDR: begin reg_datao_reg <= clkdiv_reg[reg_bytecnt*8 +: 8]; end
				`IOTRIGPROG_ADDR: begin reg_datao_reg <= prog_reg[reg_bytecnt*8 +: 8];	end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 always @(posedge clk) begin
		if (reset) begin
			clkdiv_reg <= 0;
			prog_reg <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`IOTRIGCLKDDIV_ADDR: clkdiv_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
				`IOTRIGPROG_ADDR: prog_reg[reg_bytecnt*8 +: 8] <= reg_datai;
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
	  else if ((reg_write) & (reg_address == `IOTRIGPROG_ADDR))
			prog_en <= 1;	  
	 end
	 
	 reg prog_dowrnext;
	 always @(posedge clk)
		prog_wr <= prog_dowrnext;
	 
	 always @(posedge clk)
		if ((reg_write) & (reg_address == `IOTRIGPROG_ADDR) & (reg_bytecnt == 3))
			prog_dowrnext <= 1;
		else
			prog_dowrnext <= 0;
			
	//Stretch trig_out for slower ADC clocks if needed (stretches for 127 cycles)
	wire trig_out_int;
	reg [6:0] trig_cnt;
	always @(posedge clk)
		if (trig_out_int) begin
			trig_cnt <= 7'd1;
			trig_out <= 1;
		end else
			if (trig_cnt != 7'd0) begin
				trig_cnt <= trig_cnt + 7'd1;			
				trig_out <= 1'b1;
			end else
				trig_out <= 1'b0;
			
			
			
	 
	trigger_system io_trigsys (
		.clk(clk), 
		.rst(rst_core | reset), 
		.mon_line(io_line), 
		.trig_out(trig_out_int), 
		.clkdivider(clkdiv), 
		.state_prog_en(prog_en), 
		.state_prog_addr(prog_addr), 
		.state_prog_wr(prog_wr), 
		.state_prog_data(prog_data)
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
