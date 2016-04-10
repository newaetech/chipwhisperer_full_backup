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
// Create Date:    21:51:42 02/11/2015 
// Design Name: 
// Module Name:    aes_ks 
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
module aes_ks (
	input clk,
	input load_i,
	input en_i,
	input [1:0] size_i,
	input [255:0] key_i,
	output reg [127:0] ks_o
);

localparam AES_128 = 0;
localparam AES_192 = 1;
localparam AES_256 = 2;
localparam KEYSCHED_TYPE_1 = 0;
localparam KEYSCHED_TYPE_2 = 1;
localparam KEYSCHED_TYPE_3 = 2;
localparam KEYSCHED_TYPE_A = 3;
localparam KEYSCHED_TYPE_B = 4;
localparam KEYSCHED_TYPE_C = 5;

reg [2:0] fsm;
reg [255:0] state;
reg [7:0] Rcon;

wire [31:0] w0, w1, w2, w3, w4, w5;
wire [31:0] w0_new, w1_new, w2_new, w3_new, w4_new, w5_new;
wire [31:0] w0_temp, w0_sub_o;
reg [31:0] w0_sub_i;
wire [7:0] Rcon_new = {Rcon[6:0], 1'h0} ^ ((Rcon[7]) ? 8'h1b : 8'h0);

assign {w0,w1,w2,w3} = (fsm == KEYSCHED_TYPE_2)
	? state[127:  0] : state[255:128];
assign {w4,w5} = state[127: 64];

reg [1:0] size_r;

always @*
begin : ks_h1
	reg [31:0] wN;
	
	case(size_r)
	AES_128: wN = state[159:128];
	AES_192: wN = state[ 95: 64];
	default: wN = (fsm == KEYSCHED_TYPE_1)
		? state[ 31:  0] : state[159:128];
	endcase
	
	w0_sub_i = (fsm == KEYSCHED_TYPE_2)
		? wN : {wN[23:0], wN[31:24]};
end

aes_sbox ks_inst0(.U(w0_sub_i[7:0]), .dec(1'b0), .S(w0_sub_o[7:0]));
aes_sbox ks_inst1(.U(w0_sub_i[15:8]), .dec(1'b0), .S(w0_sub_o[15:8]));
aes_sbox ks_inst2(.U(w0_sub_i[23:16]), .dec(1'b0), .S(w0_sub_o[23:16]));
aes_sbox ks_inst3(.U(w0_sub_i[31:24]), .dec(1'b0), .S(w0_sub_o[31:24]));

assign w0_temp = (fsm == KEYSCHED_TYPE_2)
	? w0_sub_o : {w0_sub_o[31:24] ^ Rcon,w0_sub_o[23:0]};

assign w0_new = w0 ^ w0_temp;
assign w1_new = w1 ^ w0_new;
assign w2_new = w2 ^ w1_new;
assign w3_new = w3 ^ w2_new;
assign w4_new = w4 ^ w3_new;
assign w5_new = w5 ^ w4_new;

always @(posedge clk)
begin
	if(load_i)
	begin
		state <= key_i;
		size_r <= size_i;
		case(size_i)
		AES_128: fsm <= KEYSCHED_TYPE_1;
		AES_192: fsm <= KEYSCHED_TYPE_B;
		AES_256: fsm <= KEYSCHED_TYPE_3;
		endcase
		Rcon <= 1;
		ks_o <= key_i[255:128];
	end
	else if(en_i)
	begin
		case(fsm)
		KEYSCHED_TYPE_1:
		begin
			Rcon <= Rcon_new;
			ks_o <= {w0_new,w1_new,w2_new,w3_new};
			state[255:128] <= {w0_new,w1_new,w2_new,w3_new};
			fsm <= (size_r == AES_128) ? KEYSCHED_TYPE_1 : KEYSCHED_TYPE_2;
		end
		KEYSCHED_TYPE_2:
		begin
			ks_o <= {w0_new,w1_new,w2_new,w3_new};
			state[127:  0] <= {w0_new,w1_new,w2_new,w3_new};
			fsm <= KEYSCHED_TYPE_1;
		end
		KEYSCHED_TYPE_3:
		begin
			ks_o <= state[127:  0];
			fsm <= KEYSCHED_TYPE_1;
		end
		KEYSCHED_TYPE_A:
		begin
			ks_o <= state[255:128];
			fsm <= KEYSCHED_TYPE_B;
		end
		KEYSCHED_TYPE_B:
		begin
			ks_o <= {state[127:64],w0_new,w1_new};
			state[255:64] <= {w0_new,w1_new,w2_new,w3_new,w4_new,w5_new};
			Rcon <= Rcon_new;
			fsm <= KEYSCHED_TYPE_C;
		end
		KEYSCHED_TYPE_C:
		begin
			ks_o <= state[191:64];
			state[255:64] <= {w0_new,w1_new,w2_new,w3_new,w4_new,w5_new};
			Rcon <= Rcon_new;
			fsm <= KEYSCHED_TYPE_A;
		end
		endcase
	end
end

endmodule
