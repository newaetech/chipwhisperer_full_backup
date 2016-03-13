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
// Create Date:    13:58:16 05/13/2014
// Design Name: 
// Module Name:    crypto_aes_top 
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
module crypto_aes_top (
	input core_clk,
	
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

	output wb_ack_o,
	output wb_err_o,
	output wb_rty_o,
	output [31:0] wb_dat_o
);

wire [255:0] key_i;
wire dec_i;
wire [1:0] size_i;
wire [127:0] data_i, data_o;
wire load_clk1, busy_clk2;

reg [1:0] busy_clk1_buf;
reg [1:0] load_clk2_buf;
wire busy_clk1 = busy_clk1_buf[1];
wire load_clk2 = load_clk2_buf[1];

wb_aes_ctrl wb_ctrl (
	// Wishbone slave 
	.wb_clk_i(wb_clk_i),
	.wb_rst_i(wb_rst_i),
	.wb_adr_i(wb_adr_i),
	.wb_dat_i(wb_dat_i),
	.wb_sel_i(wb_sel_i),
	.wb_we_i(wb_we_i),
	.wb_bte_i(wb_bte_i),
	.wb_cti_i(wb_cti_i),
	.wb_cyc_i(wb_cyc_i),
	.wb_stb_i(wb_stb_i),
	.wb_ack_o(wb_ack_o),
	.wb_err_o(wb_err_o),
	.wb_rty_o(wb_rty_o),
	.wb_dat_o(wb_dat_o),
	// AES core
	.key_o(key_i),
	.dec_o(dec_i),
	.size_o(size_i),
	.data_o(data_i),
	.load_o(load_clk1),
	.data_i(data_o),
	.busy_i(busy_clk1)
);

always @(posedge wb_clk_i) busy_clk1_buf <= { busy_clk1_buf[0], busy_clk2 };
always @(posedge core_clk) load_clk2_buf <= { load_clk2_buf[0], load_clk1 };

aes_core aes (
	.clk(core_clk),
	.load_i(load_clk2),
	.dec_i(dec_i),
	.size_i(size_i),
	.data_i(data_i),
	.key_i(key_i),
	.data_o(data_o),
	.busy_o(busy_clk2)
);

endmodule
