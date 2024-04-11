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

module ecdsa256
  (
   input  wire        clk,
	input  wire        rst_n,

   input  wire        next,
   output wire        valid,

   input  wire        bus_cs,
   input  wire        bus_we,
   input  wire [ 4:0] bus_addr,
   input  wire [31:0] bus_data_wr,
   output wire [31:0] bus_data_rd
   );


	//
	// Memory Banks
	//
	localparam [1:0] BUS_ADDR_BANK_K = 2'b00;
	localparam [1:0] BUS_ADDR_BANK_X = 2'b01;
	localparam [1:0] BUS_ADDR_BANK_Y = 2'b10;

	wire [1:0] bus_addr_upper = bus_addr[4:3];
	wire [2:0] bus_addr_lower = bus_addr[2:0];
	
	
   //
   // Memories
   //
	
   wire [31:0] user_rw_k_bram_out;
   wire [31:0] user_ro_x_bram_out;
   wire [31:0] user_ro_y_bram_out;
	
	wire [ 2:0] core_ro_k_bram_addr;
	wire [ 2:0] core_rw_x_bram_addr;
	wire [ 2:0] core_rw_y_bram_addr;
	
	wire        core_rw_x_bram_wren;
	wire        core_rw_y_bram_wren;
	
	wire [31:0] core_ro_k_bram_dout;
	wire [31:0] core_rw_x_bram_din;
	wire [31:0] core_rw_y_bram_din;

	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(3)
	)
	bram_k
	(	.clk(clk),
		.a_addr(bus_addr_lower), .a_out(user_rw_k_bram_out), .a_wr(bus_cs && bus_we && (bus_addr_upper == BUS_ADDR_BANK_K)), .a_in(bus_data_wr), 
		.b_addr(core_ro_k_bram_addr), .b_out(core_ro_k_bram_dout)
	);
	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(3)
	)
	bram_x
	(	.clk(clk),
		.a_addr(core_rw_x_bram_addr), .a_out(), .a_wr(core_rw_x_bram_wren), .a_in(core_rw_x_bram_din), 
		.b_addr(bus_addr_lower),      .b_out(user_ro_x_bram_out)
	);
	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(3)
	)
	bram_y
	(	.clk(clk),
		.a_addr(core_rw_y_bram_addr), .a_out(), .a_wr(core_rw_y_bram_wren), .a_in(core_rw_y_bram_din), 
		.b_addr(bus_addr_lower),      .b_out(user_ro_y_bram_out)
	);


   //
   // Curve Base Point Multiplier
   //
	reg  next_dly;
	
	always @(posedge clk) next_dly <= next;
	
	wire next_trig = next && !next_dly;
	
   curve_mul_256 base_point_multiplier_p256
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(next_trig),
		.rdy		(valid),
		
		.k_addr	(core_ro_k_bram_addr),
		.rx_addr	(core_rw_x_bram_addr),
		.ry_addr	(core_rw_y_bram_addr),
		
		.rx_wren	(core_rw_x_bram_wren),
		.ry_wren	(core_rw_y_bram_wren),
		
		.k_din	(core_ro_k_bram_dout),
		.rx_dout	(core_rw_x_bram_din),
		.ry_dout	(core_rw_y_bram_din)
	);

	//
   // Output Selector
   //
   reg [1:0] bus_addr_upper_prev;
   always @(posedge clk) bus_addr_upper_prev = bus_addr_upper;

   reg [31: 0] bus_data_rd_mux;
   assign bus_data_rd = bus_data_rd_mux;

   always @(*)
     //
     case (bus_addr_upper_prev)
       //
       BUS_ADDR_BANK_K: bus_data_rd_mux = user_rw_k_bram_out;
       BUS_ADDR_BANK_X: bus_data_rd_mux = user_ro_x_bram_out;
       BUS_ADDR_BANK_Y: bus_data_rd_mux = user_ro_y_bram_out;
       //
       default:         bus_data_rd_mux = {32{1'b0}};
       //
     endcase

endmodule
