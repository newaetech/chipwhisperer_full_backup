`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
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
module reg_smartcards(
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
	output  [15:0] reg_hyplen,
	
			
	output [7:0] scard_cla,
	output [7:0]  scard_ins,
	output [7:0]  scard_p1,
	output [7:0]  scard_p2,
	output [5:0]  scard_len_command,
	output [127:0]scard_command,
	output [5:0]  scard_len_response,
	input  [127:0]scard_response,
	input         scard_status,
	input  [15:0] scard_resp_code,
	input  [7:0]  scard_async_data,
	input         scard_async_datardy,
	input			scard_present,	 
	output		   scard_reset,
	output			scard_docmd,
	input			scard_busy 										              
   );
	 
	 wire	  reset;
	 assign reset = reset_i;

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
        	  
/*
	 0x1E - Smartcard Control/Status Register
	    [ X X X S D PT C R ]	 	 
		
		 S  = (bit 4) ACK Status (R)
		      1 = Last transaction successful
				0 = Last transaction had wrong ack
		
		 B  = (bit 3) Smartcard Core Busy (R)
		      1 = Busy
				0 = Not Busy (done)
		
		 PT = (bit 2) Pass Through( R/W)
		      1 = Pass any received data to bus - done in ASYNC fasion, be careful
				0 = Pass thru disabled
		
		 C = (bit 1) Card Present (R)
		      1 = Card Inserted
				0 = No Card Present
				
		 R = (bit 0) Card Reset (R/W)
				1 = Reset Asserted (low)
				0 = Reset Deasserted (high)
	 	 
	 0x1F - Smartcard Header Register (W)
	     Always write 6 bytes here.
		    
	 
	 0x20 - Smartcard Payload Register (R/W)
	     Always write 16 bytes here. If actual
		  payload is less than 16 bytes, this will
		  be determined by numbers written into
		  the smartcard header register
*/
	 `define SCARDCTRL_ADDR	30
	 `define SCARDHDR_ADDR	31
	 `define SCARDPLD_ADDR	32
	 
	 reg [7:0] scard_cla_reg;
	 reg [7:0]  scard_ins_reg;
	 reg [7:0]  scard_p1_reg;
	 reg [7:0]  scard_p2_reg;
	 reg [5:0]  scard_len_command_reg;
	 reg [127:0] scard_command_reg;
	 reg [5:0]  scard_len_response_reg;

	 reg [7:0]	registers_scardctrl;
	 wire [7:0]  registers_scardctrl_read;
	 reg			scard_docmd_reg;
	 wire        scard_async_en;
	
	 assign scard_docmd = scard_docmd_reg;		
	 assign scard_reset = registers_scardctrl[0];
	 assign scard_async_en = registers_scardctrl[2];
	 assign registers_scardctrl_read[0] = registers_scardctrl[0];
	 assign registers_scardctrl_read[1] = scard_present;
	 assign registers_scardctrl_read[3] = scard_busy;
	 assign registers_scardctrl_read[4] = scard_status;

	 assign scard_cla = scard_cla_reg;
	 assign scard_ins = scard_ins_reg;
	 assign scard_p1 = scard_p1_reg;
	 assign scard_p2 = scard_p2_reg;
	 assign scard_len_command = scard_len_command_reg;
	 assign scard_command = scard_command_reg;
	 assign scard_len_response = scard_len_response_reg;
	 
	 //Stream hacks output for asyncronous mode
	 assign reg_stream = scard_async_datardy & scard_async_en & ~(reg_read | reg_write | reg_addrvalid);
	 
	 reg reg_stream_dly;
	 reg reg_stream_dly2;
	 always @(posedge clk) begin
		reg_stream_dly <= reg_stream;
		reg_stream_dly2 <= reg_stream_dly;
	 end
	  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `SCARDCTRL_ADDR: reg_hyplen_reg <= 1;
				`SCARDHDR_ADDR: reg_hyplen_reg <= 6;
				`SCARDPLD_ADDR: reg_hyplen_reg <= 18;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 
	 reg [7:0] reg_datao;
	 
	 always @(*) begin
		if ((reg_stream) | (reg_stream_dly) | (reg_stream_dly2)) begin
			reg_datao <= scard_async_data;
		end else if (reg_datao_valid_reg == 1'b1) begin
			reg_datao <= reg_datao_reg;
		end else begin
			reg_datao <= 8'd0;
		end
	 end

	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`SCARDCTRL_ADDR: begin reg_datao_valid_reg <= 1; end
				`SCARDHDR_ADDR: begin reg_datao_valid_reg <= 1; end
				`SCARDPLD_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`SCARDPLD_ADDR: begin
						case(reg_bytecnt)
							0: reg_datao_reg <= scard_response[127:120];
							1: reg_datao_reg <= scard_response[119:112];
							2: reg_datao_reg <= scard_response[111:104];
							3: reg_datao_reg <= scard_response[103:96];
							4: reg_datao_reg <= scard_response[95:88];
							5: reg_datao_reg <= scard_response[87:80];
							6: reg_datao_reg <= scard_response[79:72];
							7: reg_datao_reg <= scard_response[71:64];
							8: reg_datao_reg <= scard_response[63:56];
							9: reg_datao_reg <= scard_response[55:48];
							10: reg_datao_reg <= scard_response[47:40];
							11: reg_datao_reg <= scard_response[39:32];
							12: reg_datao_reg <= scard_response[31:24];
							13: reg_datao_reg <= scard_response[23:16];
							14: reg_datao_reg <= scard_response[15:8];
							15: reg_datao_reg <= scard_response[7:0];
							16: reg_datao_reg <= scard_resp_code[15:8];
							17: reg_datao_reg <= scard_resp_code[7:0];
							default: ;
						endcase
				end			
				`SCARDHDR_ADDR: reg_datao_reg <= 8'd0; 
				`SCARDCTRL_ADDR: reg_datao_reg <= registers_scardctrl_read;
				default: reg_datao_reg <= 0;	
			endcase
		end
	 end
						
	 always @(posedge clk) begin
		if (reset) begin
			scard_command_reg <= 0;
			scard_cla_reg <= 0;
			scard_ins_reg <= 0;
			scard_p1_reg <= 0;
			scard_p2_reg <= 0;
			scard_len_command_reg <= 0;
			scard_len_response_reg <= 0;
			registers_scardctrl <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`SCARDCTRL_ADDR: registers_scardctrl <= reg_datai;
				`SCARDPLD_ADDR: begin
					case(reg_bytecnt)
						0: scard_command_reg[127:120] <= reg_datai;
						1: scard_command_reg[119:112] <= reg_datai;
						2: scard_command_reg[111:104] <= reg_datai;
						3: scard_command_reg[103:96] <= reg_datai;
						4: scard_command_reg[95:88] <= reg_datai;
						5: scard_command_reg[87:80] <= reg_datai;
						6: scard_command_reg[79:72] <= reg_datai;
						7: scard_command_reg[71:64] <= reg_datai;
						8: scard_command_reg[63:56] <= reg_datai;
						9: scard_command_reg[55:48] <= reg_datai;
						10: scard_command_reg[47:40] <= reg_datai;
						11: scard_command_reg[39:32] <= reg_datai;
						12: scard_command_reg[31:24] <= reg_datai;
						13: scard_command_reg[23:16] <= reg_datai;
						14: scard_command_reg[15:8] <= reg_datai;
						15: scard_command_reg[7:0] <= reg_datai;
						default: ;
					endcase
				end
				`SCARDHDR_ADDR: begin
						case(reg_bytecnt)
							0: scard_cla_reg <= reg_datai;
							1: scard_ins_reg <= reg_datai;
							2: scard_p1_reg <= reg_datai;
							3: scard_p2_reg <= reg_datai;							
							4: scard_len_command_reg <= reg_datai[5:0];
							5: scard_len_response_reg <= reg_datai[5:0];
							default: ;				
						endcase	
				end
				default: ;
			endcase
		end
	 end	 
	 
	 always @(posedge clk) begin
	  if ((reg_write) & (reg_address == `SCARDPLD_ADDR) & (reg_bytecnt == 15)) begin
			scard_docmd_reg <= 1'b1;
	  end else begin
			scard_docmd_reg <= 1'b0;
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
	 assign cs_data[68:63] = scard_len_response;
	 assign cs_data[84:69] = scard_response[127:112];
	 assign cs_data[85] = scard_busy;
	 assign cs_data[86] = scard_status;
	 assign cs_data[94:87] = scard_async_data;
	 assign cs_data[95] = scard_async_datardy;
	 assign cs_data[96] = scard_reset;
	 assign cs_data[97] = reg_stream;
	 assign cs_data[98] = scard_async_en;
 `endif
 
endmodule

`undef CHIPSCOPE
