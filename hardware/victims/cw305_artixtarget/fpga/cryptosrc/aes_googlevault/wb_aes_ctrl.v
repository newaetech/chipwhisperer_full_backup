`timescale 1ns / 1ps
/*
   Copyright 2015, Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:37:26 05/13/2014 
// Design Name: 
// Module Name:    wb_aes_ctrl 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module wb_aes_ctrl (
	input wb_clk_i,
	input wb_rst_i,
	input [31:0] wb_adr_i,
	input [31:0] wb_dat_i,
	input [3:0] wb_sel_i,
	input wb_we_i,
	input [1:0] wb_bte_i,
	input [2:0] wb_cti_i,
	input wb_cyc_i,
	input wb_stb_i,
	output reg wb_ack_o,
	output wb_err_o,
	output wb_rty_o,
	output reg [31:0] wb_dat_o,
	
	output reg [255:0] key_o,
	output reg dec_o,
	output reg [1:0] size_o,
	output reg [127:0] data_o,
	output reg load_o,
	input [127:0] data_i,
	input busy_i
);

`include "wb_common.v"

assign wb_err_o = 0;
assign wb_rty_o = 0;

wire valid = (wb_cyc_i & wb_stb_i);
reg valid_r;
wire new_cycle = valid & ~valid_r;

reg [6:0] adr_r;
wire [6:0] next_adr = wb_next_adr(adr_r, wb_cti_i, wb_bte_i, 32);
wire [6:0] wb_adr = new_cycle ? wb_adr_i : next_adr;

reg [127:0] data_ir;

always @(posedge wb_clk_i)
begin
	adr_r <= wb_adr;
	valid_r <= valid;
	wb_ack_o <= valid & (!((wb_cti_i == 3'b000) | (wb_cti_i == 3'b111)) | !wb_ack_o);
	if(wb_rst_i)
	begin
		adr_r <= 0;
		valid_r <= 0;
		wb_ack_o <= 0;
	end
end

reg busy_r, rst_r, go_r;
wire rst = (rst_r | wb_rst_i);

`define KEY_WRITE(x) \
	key_o[256 - (x * 32) - 23:256 - (x * 32) - 32] <= (wb_sel_i[0]) \
		? wb_dat_i[ 7: 0] : key_o[256 - (x * 32) - 23:256 - (x * 32) - 32]; \
	key_o[256 - (x * 32) - 17:256 - (x * 32) - 24] <= (wb_sel_i[1]) \
		? wb_dat_i[15: 8] : key_o[256 - (x * 32) - 17:256 - (x * 32) - 24]; \
	key_o[256 - (x * 32) -  9:256 - (x * 32) - 16] <= (wb_sel_i[2]) \
		? wb_dat_i[23:16] : key_o[256 - (x * 32) -  9:256 - (x * 32) - 16]; \
	key_o[256 - (x * 32) -  1:256 - (x * 32) -  8] <= (wb_sel_i[3]) \
		? wb_dat_i[31:24] : key_o[256 - (x * 32) -  1:256 - (x * 32) -  8];

`define DATA_READ(x) \
	wb_dat_o <= (load_o | busy_i) \
		? 32'h0 : data_ir[128 - (x * 32) - 1:128 - (x * 32) - 32];

`define DATA_WRITE(x) \
	data_o[128 - (x * 32) - 23:128 - (x * 32) - 32] <= (wb_sel_i[0]) \
		? wb_dat_i[ 7: 0] : data_o[128 - (x * 32) - 23:128 - (x * 32) - 32]; \
	data_o[128 - (x * 32) - 17:128 - (x * 32) - 24] <= (wb_sel_i[1]) \
		? wb_dat_i[15: 8] : data_o[128 - (x * 32) - 17:128 - (x * 32) - 24]; \
	data_o[128 - (x * 32) -  9:128 - (x * 32) - 16] <= (wb_sel_i[2]) \
		? wb_dat_i[23:16] : data_o[128 - (x * 32) -  9:128 - (x * 32) - 16]; \
	data_o[128 - (x * 32) -  1:128 - (x * 32) -  8] <= (wb_sel_i[3]) \
		? wb_dat_i[31:24] : data_o[128 - (x * 32) -  1:128 - (x * 32) -  8];


always @(posedge wb_clk_i)
begin : ctrl_block
	busy_r <= busy_i;
	rst_r <= 0;
	
	if(rst)
	begin	
		data_ir <= 0;
		load_o <= 0;
		dec_o <= 0;
		size_o <= 0;
		data_o <= 0;
		key_o <= 0;
		load_o <= 0;
	end
	else
	begin
		if(busy_r & ~busy_i)
		begin
			data_ir <= data_i;
		end
		if(busy_i)
			load_o <= 0;
			
		if(valid & wb_we_i & ~busy_i & ~load_o)
		begin
			case(wb_adr[6:4])
			3'b000:
			begin
				case(wb_adr[3:2])
				0: begin `KEY_WRITE(0); end
				1: begin `KEY_WRITE(1); end
				2: begin `KEY_WRITE(2); end
				3: begin `KEY_WRITE(3); end
				endcase
			end
			3'b001:
			begin
				case(wb_adr[3:2])
				0: begin `KEY_WRITE(4); end
				1: begin `KEY_WRITE(5); end
				2: begin `KEY_WRITE(6); end
				3: begin `KEY_WRITE(7); end
				endcase
			end
			3'b010:
			begin
				case(wb_adr[3:2])
				0: begin `DATA_WRITE(0); end
				1: begin `DATA_WRITE(1); end
				2: begin `DATA_WRITE(2); end
				3: begin `DATA_WRITE(3); end
				endcase
			end
			3'b100:
			begin
				if(wb_adr[3:2] == 0)
				begin
					load_o <= wb_sel_i[0] & wb_dat_i[0];
					dec_o <= wb_sel_i[0] & wb_dat_i[1];			
					size_o <= {2{wb_sel_i[0]}} & wb_dat_i[3:2];
					rst_r <= wb_sel_i[1] & wb_dat_i[8];
				end
			end
			endcase
		end // write handler
		
		if(valid & ~wb_we_i)
		begin
			case(wb_adr[6:4])
			3'b011:
			begin
				case(wb_adr[3:2])
				0: begin `DATA_READ(0); end
				1: begin `DATA_READ(1); end
				2: begin `DATA_READ(2); end
				3: begin `DATA_READ(3); end
				endcase
			end
			3'b100:
			begin		
				if(wb_adr[3:2] == 0)
					wb_dat_o <= { 15'h0, load_o | busy_i, 16'h0 };
				else
					wb_dat_o <= 32'h0;
			end
			default: wb_dat_o <= 32'h0;
			endcase
		end // read handler
	end
end

endmodule
