`include "includes.v"
//`define CHIPSCOPE
`define USE_I2C

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the I2C Interface.

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
module reg_i2c(
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
	
	inout				scl,
	inout				sda
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
        	  
	 `define I2CSTATUS_ADDR 	47
	 `define I2CDATA_ADDR		48 
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `I2CSTATUS_ADDR: reg_hyplen_reg <= 3;
				`I2CDATA_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;	 
	 reg [7:0]  datawr_reg;
	 reg [23:0] status_reg;	 	 	 
	 wire [7:0] i2c_regdataout;
		 
	 wire [23:0] status_reg_read;	 
		 
		/*
	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`I2CSTATUS_ADDR: begin reg_datao_valid_reg <= 1; end
				`I2CDATA_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 reg reg_datao_valid_reg;
	 */
	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`I2CSTATUS_ADDR: begin reg_datao_reg <= status_reg_read[reg_bytecnt*8 +: 8]; end
				`I2CDATA_ADDR: begin reg_datao_reg <= i2c_regdataout;	end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 always @(posedge clk) begin
		if (reset) begin
			datawr_reg <= 0;
			status_reg <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`I2CSTATUS_ADDR: status_reg[reg_bytecnt*8 +: 8] <= reg_datai;
				`I2CDATA_ADDR: datawr_reg <= reg_datai;
				default: ;
			endcase
		end
	 end	 

	assign status_reg_read[23:2] = status_reg[23:2];		
	
	/*
	  Byte 2:
	     7..0 = Register Address
	
	  Byte 1:
	     R/W = Read (1) or Write(0) Will be Requested
		  6..0 = Slave Address
	
	  Byte 0:
	     B = (bit 0) Core Busy
		  A = (bit 1) Slave Error (no ACK)
		  G = (bit 2) Go
	*/
		   	 
	wire i2c_rd_nwr;
	
	 
`ifdef USE_I2C
	/* The following module not yet in GIT due to licensing issues. Was purchased from
	    http://skytek.x10.mx.

	If you have the core, enable it with 'USE_I2C'.
		 */
	iic iic_sys(
		.sysclk(clk),
		.i2c_rd_nwr(status_reg[15]),
		.i2c_rdy_cs_n(~status_reg[2]),
		.i2c_Ain(status_reg[23:16]),
		.i2c_Din(datawr_reg),
		.i2c_partCSAdd(status_reg[14:8]),
		.i2c_status(status_reg_read[1:0]),
		.i2c_Dout(i2c_regdataout),
		.i2c_sda(sda),
		.i2c_sclk(scl)
		);
`else
	assign sda = 1'bZ;
	assign scl = 1'bZ;
`endif

endmodule
