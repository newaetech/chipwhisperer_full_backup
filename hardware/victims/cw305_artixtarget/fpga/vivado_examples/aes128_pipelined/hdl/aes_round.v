`default_nettype none
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

   NewAE modifications: adapted from aes_core.v, removing control and key expansion logic,
   keeping per-round logic to aes_round.v; add LUT sbox option.
*/
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:37:57 05/13/2014 
// Design Name: 
// Module Name:    aes_core 
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
module aes_round (
	input wire clk,
        input wire [3:0] round,
        input wire [3:0] round_max,
	input wire load_i,
	input wire [127:0] key_i,
	input wire [127:0] data_i,
	output reg [127:0] data_o
);

wire dec_r = 1'b0;

wire [127:0] state;
reg [127:0] state_new;
reg [127:0] sbb_i;
wire [127:0] sbb_o;

reg [127:0] ks_mem[14:0];
reg [127:0] ks;
wire [127:0] ks_val;

always @(posedge clk) if (load_i) data_o <= state_new;
assign state = data_i;

// NEWAE mod: GF or LUT sboxes
`ifdef SBOX_GF
    aes_sbox sbox_inst00(.U(sbb_i[  7:  0]), .dec(dec_r), .S(sbb_o[  7:  0]));
    aes_sbox sbox_inst01(.U(sbb_i[ 15:  8]), .dec(dec_r), .S(sbb_o[ 15:  8]));
    aes_sbox sbox_inst02(.U(sbb_i[ 23: 16]), .dec(dec_r), .S(sbb_o[ 23: 16]));
    aes_sbox sbox_inst03(.U(sbb_i[ 31: 24]), .dec(dec_r), .S(sbb_o[ 31: 24]));
    aes_sbox sbox_inst04(.U(sbb_i[ 39: 32]), .dec(dec_r), .S(sbb_o[ 39: 32]));
    aes_sbox sbox_inst05(.U(sbb_i[ 47: 40]), .dec(dec_r), .S(sbb_o[ 47: 40]));
    aes_sbox sbox_inst06(.U(sbb_i[ 55: 48]), .dec(dec_r), .S(sbb_o[ 55: 48]));
    aes_sbox sbox_inst07(.U(sbb_i[ 63: 56]), .dec(dec_r), .S(sbb_o[ 63: 56]));
    aes_sbox sbox_inst08(.U(sbb_i[ 71: 64]), .dec(dec_r), .S(sbb_o[ 71: 64]));
    aes_sbox sbox_inst09(.U(sbb_i[ 79: 72]), .dec(dec_r), .S(sbb_o[ 79: 72]));
    aes_sbox sbox_inst10(.U(sbb_i[ 87: 80]), .dec(dec_r), .S(sbb_o[ 87: 80]));
    aes_sbox sbox_inst11(.U(sbb_i[ 95: 88]), .dec(dec_r), .S(sbb_o[ 95: 88]));
    aes_sbox sbox_inst12(.U(sbb_i[103: 96]), .dec(dec_r), .S(sbb_o[103: 96]));
    aes_sbox sbox_inst13(.U(sbb_i[111:104]), .dec(dec_r), .S(sbb_o[111:104]));
    aes_sbox sbox_inst14(.U(sbb_i[119:112]), .dec(dec_r), .S(sbb_o[119:112]));
    aes_sbox sbox_inst15(.U(sbb_i[127:120]), .dec(dec_r), .S(sbb_o[127:120]));
`else
    aes_sbox_lut sbox_inst00(.byte_in(sbb_i[  7:  0]), .dec(dec_r), .byte_out(sbb_o[  7:  0]));
    aes_sbox_lut sbox_inst01(.byte_in(sbb_i[ 15:  8]), .dec(dec_r), .byte_out(sbb_o[ 15:  8]));
    aes_sbox_lut sbox_inst02(.byte_in(sbb_i[ 23: 16]), .dec(dec_r), .byte_out(sbb_o[ 23: 16]));
    aes_sbox_lut sbox_inst03(.byte_in(sbb_i[ 31: 24]), .dec(dec_r), .byte_out(sbb_o[ 31: 24]));
    aes_sbox_lut sbox_inst04(.byte_in(sbb_i[ 39: 32]), .dec(dec_r), .byte_out(sbb_o[ 39: 32]));
    aes_sbox_lut sbox_inst05(.byte_in(sbb_i[ 47: 40]), .dec(dec_r), .byte_out(sbb_o[ 47: 40]));
    aes_sbox_lut sbox_inst06(.byte_in(sbb_i[ 55: 48]), .dec(dec_r), .byte_out(sbb_o[ 55: 48]));
    aes_sbox_lut sbox_inst07(.byte_in(sbb_i[ 63: 56]), .dec(dec_r), .byte_out(sbb_o[ 63: 56]));
    aes_sbox_lut sbox_inst08(.byte_in(sbb_i[ 71: 64]), .dec(dec_r), .byte_out(sbb_o[ 71: 64]));
    aes_sbox_lut sbox_inst09(.byte_in(sbb_i[ 79: 72]), .dec(dec_r), .byte_out(sbb_o[ 79: 72]));
    aes_sbox_lut sbox_inst10(.byte_in(sbb_i[ 87: 80]), .dec(dec_r), .byte_out(sbb_o[ 87: 80]));
    aes_sbox_lut sbox_inst11(.byte_in(sbb_i[ 95: 88]), .dec(dec_r), .byte_out(sbb_o[ 95: 88]));
    aes_sbox_lut sbox_inst12(.byte_in(sbb_i[103: 96]), .dec(dec_r), .byte_out(sbb_o[103: 96]));
    aes_sbox_lut sbox_inst13(.byte_in(sbb_i[111:104]), .dec(dec_r), .byte_out(sbb_o[111:104]));
    aes_sbox_lut sbox_inst14(.byte_in(sbb_i[119:112]), .dec(dec_r), .byte_out(sbb_o[119:112]));
    aes_sbox_lut sbox_inst15(.byte_in(sbb_i[127:120]), .dec(dec_r), .byte_out(sbb_o[127:120]));
`endif

always @*
begin : subbytes_pre
	if(dec_r)
	begin		
		//InvShiftRows(state);
		sbb_i = {
			state[127:120], state[ 23: 16], state[ 47: 40], state[ 71: 64],
			state[ 95: 88], state[119:112], state[ 15:  8], state[ 39: 32],
			state[ 63: 56], state[ 87: 80], state[111:104], state[  7:  0],
			state[ 31: 24], state[ 55: 48], state[ 79: 72], state[103: 96]
		};
	end
	else
	begin
		sbb_i = state;
	end
end

function [7:0] xtime;
	input [7:0] b; xtime={b[6:0],1'b0} ^ (8'h1b & {8{b[7]}});
endfunction

function [7:0] x02;
	input [7:0] b; x02={b[6:0],1'b0} ^ (8'h1b & {8{b[7]}});
endfunction

function [7:0] x03;
	input [7:0] b; x03=x02(b)^b;
endfunction

function [7:0] x04;
	input [7:0] b; x04=x02(x02(b));
endfunction

function [7:0] x08;
	input [7:0] b; x08=x02(x04(b));
endfunction

function [7:0] x09;
	input [7:0] b; x09=x08(b)^b;
endfunction

function [7:0] x11;
	input [7:0] b; x11=x08(b)^x02(b)^b;
endfunction

function [7:0] x13;
	input [7:0] b; x13=x08(b)^x04(b)^b;
endfunction

function [7:0] x14;
	input [7:0] b; x14=x08(b)^x04(b)^x02(b);
endfunction
	
always @*
begin : subbytes_pst
	if(dec_r)
	begin : subbytes_pst_decrypt
		reg [127:0] ark_i, ark_o, mxc_o;
		
		// AddRoundKey(state, &ctx->ks[round * Nb]);
		if(round == round_max)
			ark_i = state;
		else
			ark_i = sbb_o;
		ark_o = ark_i ^ ks;
		
		// InvMixColumns(state);
		mxc_o = {
			x14(ark_o[127:120]) ^ x11(ark_o[119:112]) ^ x13(ark_o[111:104]) ^ x09(ark_o[103: 96]),
			x09(ark_o[127:120]) ^ x14(ark_o[119:112]) ^ x11(ark_o[111:104]) ^ x13(ark_o[103: 96]),
			x13(ark_o[127:120]) ^ x09(ark_o[119:112]) ^ x14(ark_o[111:104]) ^ x11(ark_o[103: 96]),
			x11(ark_o[127:120]) ^ x13(ark_o[119:112]) ^ x09(ark_o[111:104]) ^ x14(ark_o[103: 96]),
			
			x14(ark_o[ 95: 88]) ^ x11(ark_o[ 87: 80]) ^ x13(ark_o[ 79: 72]) ^ x09(ark_o[ 71: 64]),
			x09(ark_o[ 95: 88]) ^ x14(ark_o[ 87: 80]) ^ x11(ark_o[ 79: 72]) ^ x13(ark_o[ 71: 64]),
			x13(ark_o[ 95: 88]) ^ x09(ark_o[ 87: 80]) ^ x14(ark_o[ 79: 72]) ^ x11(ark_o[ 71: 64]),
			x11(ark_o[ 95: 88]) ^ x13(ark_o[ 87: 80]) ^ x09(ark_o[ 79: 72]) ^ x14(ark_o[ 71: 64]),
			
			x14(ark_o[ 63: 56]) ^ x11(ark_o[ 55: 48]) ^ x13(ark_o[ 47: 40]) ^ x09(ark_o[ 39: 32]),
			x09(ark_o[ 63: 56]) ^ x14(ark_o[ 55: 48]) ^ x11(ark_o[ 47: 40]) ^ x13(ark_o[ 39: 32]),
			x13(ark_o[ 63: 56]) ^ x09(ark_o[ 55: 48]) ^ x14(ark_o[ 47: 40]) ^ x11(ark_o[ 39: 32]),
			x11(ark_o[ 63: 56]) ^ x13(ark_o[ 55: 48]) ^ x09(ark_o[ 47: 40]) ^ x14(ark_o[ 39: 32]),
			
			x14(ark_o[ 31: 24]) ^ x11(ark_o[ 23: 16]) ^ x13(ark_o[ 15:  8]) ^ x09(ark_o[  7:  0]),
			x09(ark_o[ 31: 24]) ^ x14(ark_o[ 23: 16]) ^ x11(ark_o[ 15:  8]) ^ x13(ark_o[  7:  0]),
			x13(ark_o[ 31: 24]) ^ x09(ark_o[ 23: 16]) ^ x14(ark_o[ 15:  8]) ^ x11(ark_o[  7:  0]),
			x11(ark_o[ 31: 24]) ^ x13(ark_o[ 23: 16]) ^ x09(ark_o[ 15:  8]) ^ x14(ark_o[  7:  0])
		};
		
		if((round == round_max) || (round == 0))
			state_new = ark_o;
		else
			state_new = mxc_o;
	end
	else
	begin : subbytes_pst_encrypt
		reg [127:0] shr_o, mxc_o, ark_i;
		reg [31:0] mxc_tmp;
		
		// ShiftRows(state);
		shr_o = {
			sbb_o[127:120], sbb_o[ 87: 80], sbb_o[ 47: 40], sbb_o[  7:  0],
			sbb_o[ 95: 88], sbb_o[ 55: 48], sbb_o[ 15:  8], sbb_o[103: 96],
			sbb_o[ 63: 56], sbb_o[ 23: 16], sbb_o[111:104], sbb_o[ 71: 64],
			sbb_o[ 31: 24], sbb_o[119:112], sbb_o[ 79: 72], sbb_o[ 39: 32]
		};
		
		// MixColumns(state);
		mxc_tmp = {
			shr_o[127:120] ^ shr_o[119:112] ^ shr_o[111:104] ^ shr_o[103: 96],
			shr_o[ 95: 88] ^ shr_o[ 87: 80] ^ shr_o[ 79: 72] ^ shr_o[ 71: 64],
			shr_o[ 63: 56] ^ shr_o[ 55: 48] ^ shr_o[ 47: 40] ^ shr_o[ 39: 32],
			shr_o[ 31: 24] ^ shr_o[ 23: 16] ^ shr_o[ 15:  8] ^ shr_o[  7:  0]
		};
		mxc_o = {
			shr_o[127:120] ^ xtime(shr_o[127:120] ^ shr_o[119:112]) ^ mxc_tmp[31:24],
			shr_o[119:112] ^ xtime(shr_o[119:112] ^ shr_o[111:104]) ^ mxc_tmp[31:24],
			shr_o[111:104] ^ xtime(shr_o[111:104] ^ shr_o[103: 96]) ^ mxc_tmp[31:24],
			shr_o[103: 96] ^ xtime(shr_o[103: 96] ^ shr_o[127:120]) ^ mxc_tmp[31:24],
			
			shr_o[ 95: 88] ^ xtime(shr_o[ 95: 88] ^ shr_o[ 87: 80]) ^ mxc_tmp[23:16],
			shr_o[ 87: 80] ^ xtime(shr_o[ 87: 80] ^ shr_o[ 79: 72]) ^ mxc_tmp[23:16],
			shr_o[ 79: 72] ^ xtime(shr_o[ 79: 72] ^ shr_o[ 71: 64]) ^ mxc_tmp[23:16],
			shr_o[ 71: 64] ^ xtime(shr_o[ 71: 64] ^ shr_o[ 95: 88]) ^ mxc_tmp[23:16],
			
			shr_o[ 63: 56] ^ xtime(shr_o[ 63: 56] ^ shr_o[ 55: 48]) ^ mxc_tmp[15: 8],
			shr_o[ 55: 48] ^ xtime(shr_o[ 55: 48] ^ shr_o[ 47: 40]) ^ mxc_tmp[15: 8],
			shr_o[ 47: 40] ^ xtime(shr_o[ 47: 40] ^ shr_o[ 39: 32]) ^ mxc_tmp[15: 8],
			shr_o[ 39: 32] ^ xtime(shr_o[ 39: 32] ^ shr_o[ 63: 56]) ^ mxc_tmp[15: 8],
			
			shr_o[ 31: 24] ^ xtime(shr_o[ 31: 24] ^ shr_o[ 23: 16]) ^ mxc_tmp[ 7: 0],
			shr_o[ 23: 16] ^ xtime(shr_o[ 23: 16] ^ shr_o[ 15:  8]) ^ mxc_tmp[ 7: 0],
			shr_o[ 15:  8] ^ xtime(shr_o[ 15:  8] ^ shr_o[  7:  0]) ^ mxc_tmp[ 7: 0],
			shr_o[  7:  0] ^ xtime(shr_o[  7:  0] ^ shr_o[ 31: 24]) ^ mxc_tmp[ 7: 0]
		};
		
		// AddRoundKey(state, &ctx->ks[round * Nb]);
		if(round == 0)
			ark_i = state;
		else if(round == round_max)
			ark_i = shr_o;
		else
			ark_i = mxc_o;
			
		state_new = ark_i ^ ks_val;
	end
end

assign ks_val = key_i;


endmodule
`default_nettype wire
