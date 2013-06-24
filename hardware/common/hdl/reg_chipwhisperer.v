`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/chipwhisperer .

This file is the ChipWhisperer main registers. Does not include the actual data
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
module reg_chipwhisperer(
	input 			reset_i,
	input 			clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	inout [7:0]    reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output  [15:0] reg_hyplen,
			
	/* Extern Trigger Connections */
	input				trigger_fpa_i,
	input				trigger_fpb_i,
	input				trigger_io1_i,
	input				trigger_io2_i,
	input				trigger_io3_i,
	input				trigger_io4_i,
	
	/* Advanced IO Trigger Connections */
	output			trigger_ext_o,	
	input				trigger_advio_i, 
	
	/* Main trigger connections */
	output			trigger_o /* Trigger signal to capture system */
    ); 
	 wire	  reset;
	 assign reset = reset_i;
    
	 `define CW_EXTCLK_ADDR			38
	 `define CW_TRIGSRC_ADDR		39
	 `define CW_TRIGMOD_ADDR		40
 
 /*  0xXX - External Clock Source (One Byte)
	 
	   [  X  X  X  X  X  S  S  S ]
	     
		  S S S = 000 Front Panel Channel A
					 001 Front Panel Channel B
					 010 Front Panel PLL Input
		          011 Rear TargetIO - High Speed Input
					 100 Rear TargetIO - High Speed Output				
   
     0xXX - External Trigger Connections (One Byte)
	 
	   [  M  M  R4  R3  R2  R1  FB FA ]
	     All external triggers are combined into a single
		  trigger signal, which can then be passed into one
		  of the enabled 'trigger modules'
		  
		  FA = Front Panel Channel A
		  FB = Front Panel Channel B
		  R1 = Rear TargetIO - Line 1
		  R2 = Rear TargetIO - Line 2
		  R3 = Rear TargetIO - Line 3
		  R4 = Rear TargetIO - Line 4
		  MM = Mode to combine multiple channels
		    00 = OR
			 01 = AND
						
	  0xXX - Trigger Module Enabled
	  
	   [ X  X  X  X  X  M  M  M ]
		  M M M = 000 Normal Edge-Mode Trigger
		          001 Advanced IO Pattern Trigger
					 010 Advanced Correlator Trigger						
 */
    
	 reg [7:0] registers_cwextclk;
	 reg [7:0] registers_cwtrigsrc;
	 reg [7:0] registers_cwtrigmod;
  	 
	 wire trigger_and;
	 wire trigger_or;
	 wire trigger_ext;
	 
	 assign trigger_and = (registers_cwtrigsrc[0] & trigger_fpa_i) &
								 (registers_cwtrigsrc[1] & trigger_fpb_i) &
								 (registers_cwtrigsrc[2] & trigger_io1_i) &
								 (registers_cwtrigsrc[3] & trigger_io2_i) &
								 (registers_cwtrigsrc[4] & trigger_io3_i) &
								 (registers_cwtrigsrc[5] & trigger_io4_i);
								 
	 assign trigger_or  = (registers_cwtrigsrc[0] & trigger_fpa_i) |
								 (registers_cwtrigsrc[1] & trigger_fpb_i) |
								 (registers_cwtrigsrc[2] & trigger_io1_i) |
								 (registers_cwtrigsrc[3] & trigger_io2_i) |
								 (registers_cwtrigsrc[4] & trigger_io3_i) |
								 (registers_cwtrigsrc[5] & trigger_io4_i);	
								 
	 assign trigger_ext =  (registers_cwtrigsrc[7:6] == 2'b00) ? trigger_or :
								  (registers_cwtrigsrc[7:6] == 2'b01) ? trigger_and : 1'b0;
								  
	 wire trigger;	 
	 assign trigger = (registers_cwtrigmod[2:0] == 3'b000) ? trigger_ext :
						   (registers_cwtrigmod[2:0] == 3'b001) ? trigger_advio_i : 1'b0;
							
	 assign trigger_ext_o = trigger_ext;
	 
	 assign trigger_o = trigger;
	 
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `CW_EXTCLK_ADDR: reg_hyplen_reg <= 1;
				`CW_TRIGSRC_ADDR: reg_hyplen_reg <= 1;
				`CW_TRIGMOD_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end
	 	     
	 assign input_clk_src_o = registers_cwextclk[1:0];

	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 assign reg_datao = (reg_datao_valid_reg/*& reg_read*/) ? reg_datao_reg : 8'd0;

	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_TRIGSRC_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_TRIGMOD_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: reg_datao_reg <= registers_cwextclk; 
				`CW_TRIGSRC_ADDR: reg_datao_reg <= registers_cwtrigsrc; 
				`CW_TRIGMOD_ADDR: reg_datao_reg <= registers_cwtrigmod; 
				default: reg_datao_reg <= 0;	
			endcase
		end
	 end

	 always @(posedge clk) begin
		if (reset) begin
			registers_cwextclk <= 0;
			registers_cwtrigsrc <= 1;
			registers_cwtrigmod <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: registers_cwextclk <= reg_datai;
				`CW_TRIGSRC_ADDR: registers_cwtrigsrc <= reg_datai;
				`CW_TRIGMOD_ADDR: registers_cwtrigmod <= reg_datai;
				default: ;
			endcase
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
 `endif
 
endmodule

`undef CHIPSCOPE
