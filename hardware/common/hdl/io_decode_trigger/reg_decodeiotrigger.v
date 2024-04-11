`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2016, NewAE Technology Inc. All rights reserved.
Author: Colin O'Flynn <coflynn@newae.com>

  chipwhisperer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  chipwhisperer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/
module reg_decodeiotrigger(
	input wire 			  reset_i,
	input wire 			  clk,
	input wire [5:0]    reg_address,  // Address of register
	input wire [15:0]   reg_bytecnt,  // Current byte count
	input wire [7:0]    reg_datai,    // Data to write
	output wire [7:0]   reg_datao,    // Data to read
	input wire [15:0]   reg_size,     // Total size being read/write
	input wire          reg_read,     // Read flag
	input wire  		  reg_write,    // Write flag
	input wire          reg_addrvalid,// Address valid flag
	output wire			  reg_stream,	
	
	input wire [5:0]    reg_hypaddress,
	output wire [15:0]  reg_hyplen,
	
	input wire           ext_iomux,
	input wire				sck,
	input wire				mosi,
	input wire				miso,
	input wire				pdid_cs,
	input wire				pdic,
	
	output reg				trig_out									              
   );
	 wire	  reset;
	 assign reset = reset_i;
	 assign reg_stream = 1'b0;
        	  
	  /*
		0x39 - Configuration (8 Bytes)
		 [ S  S  S  C  MM MM MM MM ] (Byte 0)
		 [ Module-Specific Config    ] (Byte 1)
		 [ Byte Bitmap (LSB = 1st) ] (Byte 2)
		 [    Baud Div (LSB)       ] (Byte 3)
		 [    Baid Div (MSB)       ] (Byte 4)
		 [                         ] (Byte 5)
		 [                         ] (Byte 6)
		 [                         ] (Byte 7)
		 
		 S = Data In Source
		   0 0 0 = Ext Trigger Mux (GPIO1-4, ext-trigger pin)
		   0 0 1 = MOSI
			0 1 0 = MISO
			0 1 1 = PDID
		 
		 C = Baud Clock Source
		   0 = sys
			1 = reserved
		 
		 MM = Match Module
		   0 0 0 0 = ?
			0 0 0 1 = USART
			0 0 1 0 = SPI
			
		0x3A = Data (8 Bytes)
	  */
			  
	 `define IODECODETRIG_CFG_ADDR	57
	 `define IODECODETRIG_DATA_ADDR	58 
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `IODECODETRIG_CFG_ADDR: reg_hyplen_reg <= 8;
				`IODECODETRIG_DATA_ADDR: reg_hyplen_reg <= 8;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
		 	
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;
	
    wire [2:0] datain_src;
	 wire [3:0] match_module;
	 reg [64:0] reg_trig_cfg;
	 reg [64:0] reg_trig_data; 
	 reg [64:0] data_buffer;
	 wire [7:0] module_specific;
	 
	 assign match_module = reg_trig_cfg[3:0];
	 assign datain_src = reg_trig_cfg[7:5];
	 assign module_specific = reg_trig_cfg[15:8];
	 	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`IODECODETRIG_CFG_ADDR: begin reg_datao_reg <= reg_trig_cfg[reg_bytecnt*8 +: 8]; end
				`IODECODETRIG_DATA_ADDR: begin reg_datao_reg <= reg_trig_data[reg_bytecnt*8 +: 8];	end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 always @(posedge clk) begin
		if (reset) begin
			reg_trig_cfg <= 64'b1;
			reg_trig_data <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`IODECODETRIG_CFG_ADDR: reg_trig_cfg[reg_bytecnt*8 +: 8] <= reg_datai;	
				`IODECODETRIG_DATA_ADDR: reg_trig_data[reg_bytecnt*8 +: 8] <= reg_datai;
				default: ;
			endcase
		end
	 end	 	 
						  		
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
					
	wire [7:0] byte_mask = reg_trig_cfg[23:16];	
	wire [7:0] byte_match_async;
	reg [7:0] byte_match;
	
	//Can't use this directly, as output needs to be registered
	genvar i;
	generate	
	for (i = 0; i < 8; i = i + 1) begin: async_match
		assign byte_match_async[i] = (reg_trig_data[i*8 +: 8] == data_buffer[i*8 +: 8]) ? 1'b1 : 1'b0;
	end
	endgenerate
	
	generate	
		for (i = 0; i < 8; i = i + 1) begin : sync_match
			always @(posedge clk) begin
				//If mask == 0, we don't use this one
				byte_match[i] <= byte_match_async[i] | (~byte_mask[i]);
			end
	end
	endgenerate
	
	
	reg [1:0] trig_out_cnt;	
	always @(posedge clk) begin
		trig_out_cnt[0] <=  &byte_match; 
		trig_out_cnt[1] <= trig_out_cnt[0];
	end
	
	//If trig_out_int is high for two cycles, indicates was not a glitch, so output actual trigger	
	assign trig_out_int = &trig_out_cnt;

 
	wire [7:0] spi_data;
	wire spi_datardy;
 
	spi_decoder spi(
	  .sck(sck),
	  .din(miso),
	  .cs(pdid_cs),
	  .dataout(spi_data),
	  .data_rdy(spi_datardy)
	);
	
	wire [15:0] RxD_Baud8GeneratorInc;
	wire even_parity, two_stopbits, rx_data_rdy;
	wire [7:0] rx_data;
	
	assign even_parity = module_specific[0]; //1'b0;
	assign two_stopbits = module_specific[1]; //1'b0;
	assign RxD_Baud8GeneratorInc = reg_trig_cfg[39:24];
		 
	targ_async_receiver uart(
	  .clk(clk),
	  .RxD(ext_iomux),
	  .parity_even(even_parity),
	  .two_stopbits(two_stopbits),
	  .RxD_data_ready(rx_data_rdy),
	  .RxD_data_error(),
	  .RxD_data(rx_data),
	  .RxD_endofpacket(),
	  .RxD_idle(),
	  .RxD_Baud8GeneratorInc(RxD_Baud8GeneratorInc)
	);

   /*** TRIGGER MODULE CONNECTIONS ****/
	wire [7:0] data_buf_int;
	wire datardy_int;
	
	assign datardy_int = (match_module == 4'b0001) ? rx_data_rdy :
	                     (match_module == 4'b0010) ? spi_datardy :
								4'b0000;
	assign data_buf_int = (match_module == 4'b0001) ? rx_data :
	                     (match_module == 4'b0010) ? spi_data :
								4'b0000;
	
	always @(posedge datardy_int) begin
		data_buffer[7:0] <= data_buf_int;
		data_buffer[15:8] <= data_buffer[7:0];
		data_buffer[23:16] <= data_buffer[15:8];
		data_buffer[31:24] <= data_buffer[23:16];
		data_buffer[39:32] <= data_buffer[31:24];
		data_buffer[47:40] <= data_buffer[39:32];
		data_buffer[55:48] <= data_buffer[47:40];
		data_buffer[64:56] <= data_buffer[55:48];
	end
	 
endmodule

`undef CHIPSCOPE
