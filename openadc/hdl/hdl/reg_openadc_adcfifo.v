`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the OpenADC main registers. Does not include the actual data
transfer register which is in a seperate file.

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
module reg_openadc_adcfifo(
	input 			reset_i,
	output			reset_o,
	input 			clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	inout  [7:0]   reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output  [15:0] reg_hyplen,
				
	/* ADC Fifo Interface */
	input				fifo_empty,
	input [7:0]		fifo_data,
	output			fifo_rd_en,
	output			fifo_rd_clk				              
    );
	 
	 wire	  reset;
	 assign reset = reset_i;	 
	 assign fifo_rd_clk = clk;
	 
	 reg fifo_rd_en_reg;
	 assign fifo_rd_en = fifo_rd_en_reg;
	 
	 reg reg_stream_reg;
	 assign reg_stream = reg_stream_reg;
    
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
        	  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `ADCREAD_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end
	
	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 assign reg_datao = (reg_datao_valid_reg /*& reg_read*/) ? reg_datao_reg : 8'd0;


	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`ADCREAD_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	  
	 always @(reg_address, reg_addrvalid, reg_bytecnt, fifo_data) begin
//	 always @(posedge clk) begin
		if (reg_read) begin
//		if (reg_addrvalid) begin
			case (reg_address)
				`ADCREAD_ADDR: reg_datao_reg <= (reg_bytecnt == 0) ? 8'hAC : fifo_data; 
				default: reg_datao_reg <= 0;	
			endcase
		end
	//	end
	 end

/*
	 always @(negedge clk, negedge reg_read) begin
		if (reg_read == 0) begin
			fifo_rd_en_reg <= 0;
		end else if (reg_address == `ADCREAD_ADDR) begin
			if ((reg_bytecnt == 0) || (reg_bytecnt == 1)) begin
				fifo_rd_en_reg <= 0;
			end else begin
				fifo_rd_en_reg <= ~fifo_empty;
			end
		end else begin
			fifo_rd_en_reg <= 0;
		end
	 end	 
 */
 
	 //always @(reg_read, reg_address, reg_bytecnt) begin
	 always @(posedge clk) begin
		 if ((reg_read == 1) && (reg_address == `ADCREAD_ADDR) && (reg_bytecnt > 16'd0)) begin
			fifo_rd_en_reg <= 1;
		 end else begin
			fifo_rd_en_reg <= 0;
		 end
	 end
  	 
	 //always begin
	 wire stream_rst;
	 assign stream_rst = fifo_empty | reset | ~reg_addrvalid;
	 always  @(posedge clk or posedge stream_rst) begin
		//if ((fifo_empty == 1) || (reset == 1) || (reg_addrvalid == 0) || (reg_address != `ADCREAD_ADDR)) begin
		if (stream_rst == 1) begin
			reg_stream_reg <= 0;
		end else begin
			if ((reg_addrvalid == 1) && (reg_address == `ADCREAD_ADDR)) begin
				reg_stream_reg <= 1;
			end
		end
	 end
			
 `ifdef CHIPSCOPE
	 assign cs_data[5:0] = reg_address;
	 assign cs_data[21:6] = reg_bytecnt;
	 assign cs_data[29:22] = reg_datai;
	 assign cs_data[37:30] = reg_datao;
	 assign cs_data[38] = reg_read;
	 assign cs_data[39] = reg_write;
	 assign cs_data[40] = reg_addrvalid;
	 assign cs_data[46:41] = reg_hypaddress;
	 assign cs_data[62:47] = reg_hyplen;
	 assign cs_data[63] = reg_stream;

	 assign cs_data[64] = fifo_empty;
	 assign cs_data[65] = fifo_rd_en;
	 assign cs_data[73:66] = fifo_data;
 `endif
 
endmodule

`undef CHIPSCOPE
