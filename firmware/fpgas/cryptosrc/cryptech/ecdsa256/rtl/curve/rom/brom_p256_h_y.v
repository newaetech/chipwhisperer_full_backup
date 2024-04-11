//======================================================================
//
// Copyright (c) 2016, NORDUnet A/S All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//======================================================================

`timescale 1ns / 1ps

module brom_p256_h_y
   (
		input		wire				clk,
		input		wire	[ 3-1:0]	b_addr,
		output	wire	[32-1:0]	b_out
    );


   //
   // Output Registers
   //
   reg [31:0] bram_reg_b;

   assign b_out = bram_reg_b;


   //
   // Read-Only Port B
	//
	always @(posedge clk)
		//
		case (b_addr)
			3'b000:	bram_reg_b <= 32'hc840ae07;
			3'b001:	bram_reg_b <= 32'h3449bf97;
			3'b010:	bram_reg_b <= 32'h94cea131;
			3'b011:	bram_reg_b <= 32'hd431cca9;
			3'b100:	bram_reg_b <= 32'h83f061e9;
			3'b101:	bram_reg_b <= 32'h711814b5;
			3'b110:	bram_reg_b <= 32'h01e58065;
			3'b111:	bram_reg_b <= 32'hb01cbd1c;
		endcase


endmodule
