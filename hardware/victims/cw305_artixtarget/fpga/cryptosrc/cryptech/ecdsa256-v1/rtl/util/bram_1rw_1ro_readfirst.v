//======================================================================
//
// Copyright (c) 2015, NORDUnet A/S All rights reserved.
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

module bram_1rw_1ro_readfirst
  #(parameter MEM_WIDTH            = 32,
    parameter MEM_ADDR_BITS        = 8)
   (
    input wire                     clk,

    input wire [MEM_ADDR_BITS-1:0] a_addr,
    input wire                     a_wr,
    input wire [MEM_WIDTH-1:0]     a_in,
    output wire [MEM_WIDTH-1:0]    a_out,

    input wire [MEM_ADDR_BITS-1:0] b_addr,
    output wire [MEM_WIDTH-1:0]    b_out
    );


   //
   // BRAM
   //
   (* RAM_STYLE="BLOCK" *)
   reg [MEM_WIDTH-1:0]             bram[0:(2**MEM_ADDR_BITS)-1];
	
	
	//
	// Initialization
	//
	/**
	integer c;
	initial begin
		for (c=0; c<(2**MEM_ADDR_BITS); c=c+1)
			bram[c] = {MEM_WIDTH{1'b0}};
	end
	**/
	


   //
   // Output Registers
   //
   reg [MEM_WIDTH-1:0]             bram_reg_a;
   reg [MEM_WIDTH-1:0]             bram_reg_b;

   assign a_out = bram_reg_a;
   assign b_out = bram_reg_b;


   //
   // Read-Write Port A
   //
   always @(posedge clk) begin
      //
      bram_reg_a <= bram[a_addr];
      //
      if (a_wr) bram[a_addr] <= a_in;
      //
   end


   //
   // Read-Only Port B
   //
   always @(posedge clk)
     //
     bram_reg_b <= bram[b_addr];


endmodule
