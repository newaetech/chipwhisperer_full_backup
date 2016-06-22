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
module spi_decoder(	
	input wire				sck,
	input wire				din,
	input wire				cs,
	
	output wire [7:0] 	dataout,
	output reg           data_rdy
   );
	
	wire sck_int;
	
	assign sck_int = sck;
	
	reg [7:0] data_posclk;	
	always @(posedge sck_int) begin
		data_posclk[7:0] = {data_posclk[6:0], din};
	end
	
	reg [2:0] bit_cnt;
	always @(posedge sck_int or posedge cs) begin
		if (cs == 1'b1)
			bit_cnt <= 3'd0;
		else
			bit_cnt <= bit_cnt + 1'd1;
	end
	
	always @(posedge sck_int) begin
		data_rdy <= (bit_cnt == 3'b111) ? 1'b1 : 1'b0;
	end
	
	assign dataout = data_posclk;
	
	  
endmodule

`undef CHIPSCOPE
