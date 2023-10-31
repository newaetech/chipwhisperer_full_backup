//------------------------------------------------------------------------------
//
// modular_invertor.v
// -----------------------------------------------------------------------------
// Modular invertor.
//
// Authors: Pavel Shatov
//
// Copyright (c) 2016, NORDUnet A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//------------------------------------------------------------------------------

module modular_invertor
	(
		clk, rst_n,
		ena, rdy,
		a_addr, q_addr, a1_addr, a1_wren,
		a_din, q_din, a1_dout
	);


		//
		// Parameters
		//
	parameter MAX_OPERAND_WIDTH = 256;
	
	
		//
		// clog2
		//
`include "modinv_clog2.v"


		//
		// More Parameters
		//
	localparam OPERAND_NUM_WORDS	= MAX_OPERAND_WIDTH / 32;
	localparam OPERAND_ADDR_BITS	= clog2(OPERAND_NUM_WORDS);
	
	localparam BUFFER_NUM_WORDS	= OPERAND_NUM_WORDS + 1;
	localparam BUFFER_ADDR_BITS	= clog2(BUFFER_NUM_WORDS);
	
	localparam LOOP_NUM_ROUNDS		= 2 * MAX_OPERAND_WIDTH;
	localparam ROUND_COUNTER_BITS	= clog2(LOOP_NUM_ROUNDS);
	
	localparam K_NUM_BITS			= clog2(LOOP_NUM_ROUNDS + 1);
	

		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	
	input		wire									ena;
	output	wire									rdy;
	
	output	wire	[OPERAND_ADDR_BITS-1:0]	a_addr;
	output	reg	[OPERAND_ADDR_BITS-1:0]	q_addr;
	output	wire	[OPERAND_ADDR_BITS-1:0]	a1_addr;
	output	wire									a1_wren;
	
	input		wire	[32-1:0]						a_din;
	input		wire	[32-1:0]						q_din;
	output	wire	[32-1:0]						a1_dout;


		//
		// "Redundant" Power of 2 (K)
		//
	reg	[K_NUM_BITS-1:0]	k;

	
		//
		// Buffers
		//
	reg	[BUFFER_ADDR_BITS-1:0]	buf_r_wr_addr;
	reg	[BUFFER_ADDR_BITS-1:0]	buf_r_rd_addr;
	reg									buf_r_wr_en;
	reg	[              32-1:0]	buf_r_wr_din;
	wire	[              32-1:0]	buf_r_wr_dout;
	wire	[              32-1:0]	buf_r_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_r
	(	.clk(clk),	
		.a_addr(buf_r_wr_addr), .a_out(buf_r_wr_dout), .a_wr(buf_r_wr_en), .a_in(buf_r_wr_din),
		.b_addr(buf_r_rd_addr), .b_out(buf_r_rd_dout)
	);
	
	reg	[BUFFER_ADDR_BITS-1:0]	buf_s_wr_addr;
	reg	[BUFFER_ADDR_BITS-1:0]	buf_s_rd_addr;
	reg									buf_s_wr_en;
	reg	[              32-1:0]	buf_s_wr_din;
	wire	[              32-1:0]	buf_s_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_s
	(	.clk(clk),	
		.a_addr(buf_s_wr_addr), .a_out(),              .a_wr(buf_s_wr_en), .a_in(buf_s_wr_din),
		.b_addr(buf_s_rd_addr), .b_out(buf_s_rd_dout)
	);
	
	reg	[BUFFER_ADDR_BITS-1:0]	buf_u_wr_addr;
	reg	[BUFFER_ADDR_BITS-1:0]	buf_u_rd_addr;
	reg									buf_u_wr_en;
	reg	[              32-1:0]	buf_u_wr_din;
	wire	[              32-1:0]	buf_u_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_u
	(	.clk(clk),	
		.a_addr(buf_u_wr_addr), .a_out(),              .a_wr(buf_u_wr_en), .a_in(buf_u_wr_din),
		.b_addr(buf_u_rd_addr), .b_out(buf_u_rd_dout)
	);
	
	reg	[BUFFER_ADDR_BITS-1:0]	buf_v_wr_addr;
	reg	[BUFFER_ADDR_BITS-1:0]	buf_v_rd_addr;
	reg									buf_v_wr_en;
	reg	[              32-1:0]	buf_v_wr_din;
	wire	[              32-1:0]	buf_v_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_v
	(	.clk(clk),	
		.a_addr(buf_v_wr_addr), .a_out(),              .a_wr(buf_v_wr_en), .a_in(buf_v_wr_din),
		.b_addr(buf_v_rd_addr), .b_out(buf_v_rd_dout)
	);	

	wire	[BUFFER_ADDR_BITS-1:0]	buf_r_dbl_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_r_dbl_rd_addr;
	wire									buf_r_dbl_wr_en;
	wire	[              32-1:0]	buf_r_dbl_wr_din;
	wire	[              32-1:0]	buf_r_dbl_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_r_dbl
	(	.clk(clk),	
		.a_addr(buf_r_dbl_wr_addr), .a_out(),                  .a_wr(buf_r_dbl_wr_en), .a_in(buf_r_dbl_wr_din),
		.b_addr(buf_r_dbl_rd_addr), .b_out(buf_r_dbl_rd_dout)
	);
	
	wire	[BUFFER_ADDR_BITS-1:0]	buf_s_dbl_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_s_dbl_rd_addr;
	wire									buf_s_dbl_wr_en;
	wire	[              32-1:0]	buf_s_dbl_wr_din;
	wire	[              32-1:0]	buf_s_dbl_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_s_dbl
	(	.clk(clk),	
		.a_addr(buf_s_dbl_wr_addr), .a_out(),                  .a_wr(buf_s_dbl_wr_en), .a_in(buf_s_dbl_wr_din),
		.b_addr(buf_s_dbl_rd_addr), .b_out(buf_s_dbl_rd_dout)
	);
	
	wire	[BUFFER_ADDR_BITS-1:0]	buf_r_plus_s_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_r_plus_s_rd_addr;
	wire									buf_r_plus_s_wr_en;
	wire	[              32-1:0]	buf_r_plus_s_wr_din;
	wire	[              32-1:0]	buf_r_plus_s_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_r_plus_s
	(	.clk(clk),	
		.a_addr(buf_r_plus_s_wr_addr), .a_out(),                     .a_wr(buf_r_plus_s_wr_en), .a_in(buf_r_plus_s_wr_din),
		.b_addr(buf_r_plus_s_rd_addr), .b_out(buf_r_plus_s_rd_dout)
	);
	
	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_minus_v_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_minus_v_rd_addr;
	wire									buf_u_minus_v_wr_en;
	wire	[              32-1:0]	buf_u_minus_v_wr_din;
	wire	[              32-1:0]	buf_u_minus_v_wr_dout;

	assign buf_u_minus_v_rd_addr = ~buf_u_minus_v_wr_addr;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_u_minus_v
	(	.clk(clk),	
		.a_addr(buf_u_minus_v_wr_addr), .a_out(buf_u_minus_v_wr_dout), .a_wr(buf_u_minus_v_wr_en), .a_in(buf_u_minus_v_wr_din),
		.b_addr(buf_u_minus_v_rd_addr), .b_out()
	);

	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_minus_u_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_minus_u_rd_addr;
	wire									buf_v_minus_u_wr_en;
	wire	[              32-1:0]	buf_v_minus_u_wr_din;
	wire	[              32-1:0]	buf_v_minus_u_wr_dout;
	
	assign buf_v_minus_u_rd_addr = ~buf_v_minus_u_wr_addr;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_v_minus_u
	(	.clk(clk),	
		.a_addr(buf_v_minus_u_wr_addr), .a_out(buf_v_minus_u_wr_dout), .a_wr(buf_v_minus_u_wr_en), .a_in(buf_v_minus_u_wr_din),
		.b_addr(buf_v_minus_u_rd_addr), .b_out()
	);

	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_half_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_half_rd_addr;
	wire									buf_u_half_wr_en;
	wire	[              32-1:0]	buf_u_half_wr_din;
	wire	[              32-1:0]	buf_u_half_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_u_half
	(	.clk(clk),	
		.a_addr(buf_u_half_wr_addr), .a_out(),                   .a_wr(buf_u_half_wr_en), .a_in(buf_u_half_wr_din),
		.b_addr(buf_u_half_rd_addr), .b_out(buf_u_half_rd_dout)
	);
	
	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_half_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_half_rd_addr;
	wire									buf_v_half_wr_en;
	wire	[              32-1:0]	buf_v_half_wr_din;
	wire	[              32-1:0]	buf_v_half_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_v_half
	(	.clk(clk),	
		.a_addr(buf_v_half_wr_addr), .a_out(),                   .a_wr(buf_v_half_wr_en), .a_in(buf_v_half_wr_din),
		.b_addr(buf_v_half_rd_addr), .b_out(buf_v_half_rd_dout)
	);
	
	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_minus_v_half_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_u_minus_v_half_rd_addr;
	wire									buf_u_minus_v_half_wr_en;
	wire	[              32-1:0]	buf_u_minus_v_half_wr_din;
	wire	[              32-1:0]	buf_u_minus_v_half_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_u_minus_v_half
	(	.clk(clk),	
		.a_addr(buf_u_minus_v_half_wr_addr), .a_out(),                           .a_wr(buf_u_minus_v_half_wr_en), .a_in(buf_u_minus_v_half_wr_din),
		.b_addr(buf_u_minus_v_half_rd_addr), .b_out(buf_u_minus_v_half_rd_dout)
	);

	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_minus_u_half_wr_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	buf_v_minus_u_half_rd_addr;
	wire									buf_v_minus_u_half_wr_en;
	wire	[              32-1:0]	buf_v_minus_u_half_wr_din;
	wire	[              32-1:0]	buf_v_minus_u_half_rd_dout;

	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(BUFFER_ADDR_BITS)
	)
	buf_v_minus_u_half
	(	.clk(clk),	
		.a_addr(buf_v_minus_u_half_wr_addr), .a_out(),                           .a_wr(buf_v_minus_u_half_wr_en), .a_in(buf_v_minus_u_half_wr_din),
		.b_addr(buf_v_minus_u_half_rd_addr), .b_out(buf_v_minus_u_half_rd_dout)
	);


		//
		// Helper Modules
		//
	wire helper_init_ena;
	wire helper_invert_precalc_ena;
	wire helper_invert_compare_ena;
	wire helper_invert_update_ena;
	wire helper_reduce_precalc_ena;
	wire helper_reduce_update_ena;
	wire helper_copy_ena;
	
	wire helper_init_rdy;
	wire helper_invert_precalc_rdy;
	wire helper_invert_compare_rdy;
	wire helper_invert_update_rdy;
	wire helper_reduce_precalc_rdy;
	wire helper_reduce_update_rdy;
	wire helper_copy_rdy;
	
	wire helper_init_done				= helper_init_rdy           && !helper_init_ena;
	wire helper_invert_precalc_done	= helper_invert_precalc_rdy && !helper_invert_precalc_ena;
	wire helper_invert_compare_done	= helper_invert_compare_rdy && !helper_invert_compare_ena;
	wire helper_invert_update_done	= helper_invert_update_rdy  && !helper_invert_update_ena;
	wire helper_reduce_precalc_done	= helper_reduce_precalc_rdy && !helper_reduce_precalc_ena;
	wire helper_reduce_update_done	= helper_reduce_update_rdy  && !helper_reduce_update_ena;
	wire helper_copy_done				= helper_copy_rdy           && !helper_copy_ena;
	
	
		//
		// Helper Module - Initialization
		//
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_init_r_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_init_s_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_init_u_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_init_v_addr;
	wire	[OPERAND_ADDR_BITS-1:0]	helper_init_q_addr;
	
	wire									helper_init_r_wren;
	wire									helper_init_s_wren;
	wire									helper_init_u_wren;
	wire									helper_init_v_wren;
	
	wire	[              32-1:0]	helper_init_r_data;
	wire	[              32-1:0]	helper_init_s_data;
	wire	[              32-1:0]	helper_init_u_data;
	wire	[              32-1:0]	helper_init_v_data;
	
	modinv_helper_init #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.OPERAND_ADDR_BITS	(OPERAND_ADDR_BITS),
	
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_init
	(
		.clk 		(clk),
		.rst_n	(rst_n),
		
		.ena 		(helper_init_ena),
		.rdy 		(helper_init_rdy),
		
		.a_addr	(a_addr),
		.q_addr	(helper_init_q_addr),
		
		.r_addr	(helper_init_r_addr),
		.s_addr	(helper_init_s_addr),
		.u_addr	(helper_init_u_addr),
		.v_addr	(helper_init_v_addr),
		
		.q_din	(q_din),
		.a_din	(a_din),
		
		.r_dout	(helper_init_r_data),
		.s_dout	(helper_init_s_data),
		.u_dout	(helper_init_u_data),
		.v_dout	(helper_init_v_data),
		
		.r_wren	(helper_init_r_wren),
		.s_wren	(helper_init_s_wren),
		.u_wren	(helper_init_u_wren),
		.v_wren	(helper_init_v_wren)
	);
	
	
		//
		// Helper Module - Inversion Pre-Calculation
		//
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_precalc_r_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_precalc_s_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_precalc_u_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_precalc_v_addr;
	
	modinv_helper_invert_precalc #
	(
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_invert_precalc
	(
		.clk 							(clk),
		.rst_n						(rst_n),
		
		.ena 							(helper_invert_precalc_ena),
		.rdy 							(helper_invert_precalc_rdy),
		
		.r_addr						(helper_invert_precalc_r_addr),
		.s_addr						(helper_invert_precalc_s_addr),
		.u_addr						(helper_invert_precalc_u_addr),
		.v_addr						(helper_invert_precalc_v_addr),
		
		.r_din						(buf_r_rd_dout),
		.s_din						(buf_s_rd_dout),
		.u_din						(buf_u_rd_dout),
		.v_din						(buf_v_rd_dout),
		
		.r_dbl_addr					(buf_r_dbl_wr_addr),
		.s_dbl_addr					(buf_s_dbl_wr_addr),
		.r_plus_s_addr				(buf_r_plus_s_wr_addr),
		
		.u_half_addr				(buf_u_half_wr_addr),
		.v_half_addr				(buf_v_half_wr_addr),
		.u_minus_v_addr			(buf_u_minus_v_wr_addr),
		.v_minus_u_addr			(buf_v_minus_u_wr_addr),
		.u_minus_v_half_addr		(buf_u_minus_v_half_wr_addr),
		.v_minus_u_half_addr		(buf_v_minus_u_half_wr_addr),
		
		.r_dbl_dout					(buf_r_dbl_wr_din),
		.s_dbl_dout					(buf_s_dbl_wr_din),
		.r_plus_s_dout				(buf_r_plus_s_wr_din),
		
		.u_half_dout				(buf_u_half_wr_din),
		.v_half_dout				(buf_v_half_wr_din),
		.u_minus_v_dout			(buf_u_minus_v_wr_din),
		.v_minus_u_dout			(buf_v_minus_u_wr_din),
		.u_minus_v_half_dout		(buf_u_minus_v_half_wr_din),
		.v_minus_u_half_dout		(buf_v_minus_u_half_wr_din),
		
		.r_dbl_wren					(buf_r_dbl_wr_en),
		.s_dbl_wren					(buf_s_dbl_wr_en),
		.r_plus_s_wren				(buf_r_plus_s_wr_en),
		
		.u_half_wren				(buf_u_half_wr_en),
		.v_half_wren				(buf_v_half_wr_en),
		.u_minus_v_wren			(buf_u_minus_v_wr_en),
		.v_minus_u_wren			(buf_v_minus_u_wr_en),
		.u_minus_v_half_wren		(buf_u_minus_v_half_wr_en),
		.v_minus_u_half_wren		(buf_v_minus_u_half_wr_en),
		
		.u_minus_v_din				(buf_u_minus_v_wr_dout),
		.v_minus_u_din				(buf_v_minus_u_wr_dout)
	);
	
	
		//
		// Helper Module - Inversion Comparison
		//
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_compare_u_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_compare_v_addr;

	wire	flag_invert_u_gt_v;
	wire	flag_invert_v_eq_1;
	wire	flag_invert_u_is_even;
	wire	flag_invert_v_is_even;

	modinv_helper_invert_compare #
	(
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_invert_compare
	(
		.clk 			(clk),
		.rst_n		(rst_n),
		
		.ena 			(helper_invert_compare_ena),
		.rdy 			(helper_invert_compare_rdy),
				
		.u_addr		(helper_invert_compare_u_addr),
		.v_addr		(helper_invert_compare_v_addr),
		
		.u_din		(buf_u_rd_dout),
		.v_din		(buf_v_rd_dout),
		
		.u_gt_v		(flag_invert_u_gt_v),
		.v_eq_1		(flag_invert_v_eq_1),
		.u_is_even	(flag_invert_u_is_even),
		.v_is_even	(flag_invert_v_is_even)
	);
	
		
		//
		// Helper Module - Inversion Update
		//
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_update_r_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_update_s_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_update_u_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_invert_update_v_addr;
	
	wire									helper_invert_update_r_wren;
	wire									helper_invert_update_s_wren;
	wire									helper_invert_update_u_wren;
	wire									helper_invert_update_v_wren;
	
	wire	[              32-1:0]	helper_invert_update_r_data;
	wire	[              32-1:0]	helper_invert_update_s_data;
	wire	[              32-1:0]	helper_invert_update_u_data;
	wire	[              32-1:0]	helper_invert_update_v_data;
	
	modinv_helper_invert_update #
	(
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_invert_update
	(
		.clk 							(clk),
		.rst_n						(rst_n),
		
		.ena 							(helper_invert_update_ena),
		.rdy 							(helper_invert_update_rdy),
		
		.u_gt_v						(flag_invert_u_gt_v),
		.v_eq_1						(flag_invert_v_eq_1),
		.u_is_even					(flag_invert_u_is_even),
		.v_is_even					(flag_invert_v_is_even),
		
		.r_addr						(helper_invert_update_r_addr),
		.s_addr						(helper_invert_update_s_addr),
		.u_addr						(helper_invert_update_u_addr),
		.v_addr						(helper_invert_update_v_addr),
		
		.r_wren						(helper_invert_update_r_wren),
		.s_wren						(helper_invert_update_s_wren),
		.u_wren						(helper_invert_update_u_wren),
		.v_wren						(helper_invert_update_v_wren),
		
		.r_dout						(helper_invert_update_r_data),
		.s_dout						(helper_invert_update_s_data),
		.u_dout						(helper_invert_update_u_data),
		.v_dout						(helper_invert_update_v_data),
		
		.r_dbl_addr					(buf_r_dbl_rd_addr),
		.s_dbl_addr					(buf_s_dbl_rd_addr),
		.r_plus_s_addr				(buf_r_plus_s_rd_addr),
		.u_half_addr				(buf_u_half_rd_addr),
		.v_half_addr				(buf_v_half_rd_addr),
		.u_minus_v_half_addr		(buf_u_minus_v_half_rd_addr),
		.v_minus_u_half_addr		(buf_v_minus_u_half_rd_addr),
		
		.r_dbl_din					(buf_r_dbl_rd_dout),
		.s_dbl_din					(buf_s_dbl_rd_dout),
		.r_plus_s_din				(buf_r_plus_s_rd_dout),
		.u_half_din					(buf_u_half_rd_dout),
		.v_half_din					(buf_v_half_rd_dout),
		.u_minus_v_half_din		(buf_u_minus_v_half_rd_dout),
		.v_minus_u_half_din		(buf_v_minus_u_half_rd_dout)
	);
	
	
		//
		// Helper Module - Reduction Pre-Calculation
		//
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_reduce_precalc_r_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_reduce_precalc_s_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_reduce_precalc_u_addr;
	wire	[ BUFFER_ADDR_BITS-1:0]	helper_reduce_precalc_v_addr;
	wire	[OPERAND_ADDR_BITS-1:0]	helper_reduce_precalc_q_addr;
	
	wire									helper_reduce_precalc_r_wren;
	wire									helper_reduce_precalc_u_wren;
	wire									helper_reduce_precalc_v_wren;
	
	wire	[              32-1:0]	helper_reduce_precalc_r_data;
	wire	[              32-1:0]	helper_reduce_precalc_u_data;
	wire	[              32-1:0]	helper_reduce_precalc_v_data;

	wire	flag_reduce_s_is_odd;
	wire	flag_invert_k_is_nul;
	
	modinv_helper_reduce_precalc #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.OPERAND_ADDR_BITS	(OPERAND_ADDR_BITS),
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS),
		.K_NUM_BITS				(K_NUM_BITS)
	)
	helper_reduce_precalc
	(
		.clk 			(clk),
		.rst_n		(rst_n),
		
		.ena 			(helper_reduce_precalc_ena),
		.rdy 			(helper_reduce_precalc_rdy),
		
		.r_addr		(helper_reduce_precalc_r_addr),
		.s_addr		(helper_reduce_precalc_s_addr),
		.u_addr		(helper_reduce_precalc_u_addr),
		.v_addr		(helper_reduce_precalc_v_addr),
		.q_addr		(helper_reduce_precalc_q_addr),
		
		.k				(k),
		
		.s_is_odd	(flag_reduce_s_is_odd),
		.k_is_nul	(flag_reduce_k_is_nul),
		
		.r_din		(buf_r_wr_dout),
		.s_din		(buf_s_rd_dout),
		.q_din		(q_din),
		
		.r_wren		(helper_reduce_precalc_r_wren),
		.u_wren		(helper_reduce_precalc_u_wren),
		.v_wren		(helper_reduce_precalc_v_wren),
		
		.r_dout		(helper_reduce_precalc_r_data),
		.u_dout		(helper_reduce_precalc_u_data),
		.v_dout		(helper_reduce_precalc_v_data)
	);
	
		//
		// Helper Module - Reduction Update
		//
	wire	[BUFFER_ADDR_BITS-1:0]	helper_reduce_update_s_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_reduce_update_u_addr;
	wire	[BUFFER_ADDR_BITS-1:0]	helper_reduce_update_v_addr;
	
	wire									helper_reduce_update_s_wren;
	
	wire	[              32-1:0]	helper_reduce_update_s_data;
	
	modinv_helper_reduce_update #
	(
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_reduce_update
	(
		.clk 							(clk),
		.rst_n						(rst_n),
		
		.ena 							(helper_reduce_update_ena),
		.rdy 							(helper_reduce_update_rdy),
		
		.s_is_odd					(flag_reduce_s_is_odd),
		.k_is_nul					(flag_reduce_k_is_nul),
		
		.s_addr						(helper_reduce_update_s_addr),
		.u_addr						(helper_reduce_update_u_addr),
		.v_addr						(helper_reduce_update_v_addr),
		
		.s_wren						(helper_reduce_update_s_wren),
		
		.s_dout						(helper_reduce_update_s_data),
				
		.u_din						(buf_u_rd_dout),
		.v_din						(buf_v_rd_dout)
	);
	
	
		//
		// Helper Module - Copying
		//
	wire	[BUFFER_ADDR_BITS-1:0]	helper_copy_s_addr;
		
	modinv_helper_copy #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.OPERAND_ADDR_BITS	(OPERAND_ADDR_BITS),
	
		.BUFFER_NUM_WORDS		(BUFFER_NUM_WORDS),
		.BUFFER_ADDR_BITS		(BUFFER_ADDR_BITS)
	)
	helper_copy
	(
		.clk 		(clk),
		.rst_n	(rst_n),
		
		.ena 		(helper_copy_ena),
		.rdy 		(helper_copy_rdy),
		
		.s_addr	(helper_copy_s_addr),
		.a1_addr	(a1_addr),
		
		.s_din	(buf_s_rd_dout),
		
		.a1_dout	(a1_dout),
		
		.a1_wren	(a1_wren)
	);
	
	
		//
		// Round Counter
		//
	reg	[ROUND_COUNTER_BITS-1:0]	round_counter;
	wire	[ROUND_COUNTER_BITS-1:0]	round_counter_max = LOOP_NUM_ROUNDS - 1;
	wire	[ROUND_COUNTER_BITS-1:0]	round_counter_zero = {ROUND_COUNTER_BITS{1'b0}};
	wire	[ROUND_COUNTER_BITS-1:0]	round_counter_next =
		(round_counter < round_counter_max) ? round_counter + 1'b1 : round_counter_zero;

	
		//
		// FSM
		//
	localparam FSM_STATE_IDLE				= 4'd0;
	
	localparam FSM_STATE_INIT				= 4'd1;
	
	localparam FSM_STATE_INVERT_PRECALC	= 4'd11;
	localparam FSM_STATE_INVERT_COMPARE	= 4'd12;
	localparam FSM_STATE_INVERT_UPDATE	= 4'd13;
	
	localparam FSM_STATE_REDUCE_PRECALC	= 4'd14;
	localparam FSM_STATE_REDUCE_UPDATE	= 4'd15;
	
	localparam FSM_STATE_COPY				= 4'd2;
	
	localparam FSM_STATE_DONE				= 4'd3;
	
	reg [3:0] fsm_state = FSM_STATE_IDLE;
	reg [3:0] fsm_state_dly = FSM_STATE_IDLE;
	
	wire fsm_state_new = (fsm_state != fsm_state_dly);

	wire [3:0] fsm_state_invert_next = (round_counter < round_counter_max) ?
		FSM_STATE_INVERT_PRECALC : FSM_STATE_REDUCE_PRECALC;
		
	wire [3:0] fsm_state_reduce_next = (round_counter < round_counter_max) ?
		FSM_STATE_REDUCE_PRECALC : FSM_STATE_COPY;
	
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0) fsm_state <= FSM_STATE_IDLE;
		else case (fsm_state)
			FSM_STATE_IDLE:				fsm_state <= ena                        ? FSM_STATE_INIT           : FSM_STATE_IDLE;
			FSM_STATE_INIT:				fsm_state <= helper_init_done           ? FSM_STATE_INVERT_PRECALC : FSM_STATE_INIT;
			FSM_STATE_INVERT_PRECALC:	fsm_state <= helper_invert_precalc_done ? FSM_STATE_INVERT_COMPARE : FSM_STATE_INVERT_PRECALC;
			FSM_STATE_INVERT_COMPARE:	fsm_state <= helper_invert_compare_done ? FSM_STATE_INVERT_UPDATE  : FSM_STATE_INVERT_COMPARE;
			FSM_STATE_INVERT_UPDATE:	fsm_state <= helper_invert_update_done  ? fsm_state_invert_next    : FSM_STATE_INVERT_UPDATE;
			FSM_STATE_REDUCE_PRECALC:	fsm_state <= helper_reduce_precalc_done ? FSM_STATE_REDUCE_UPDATE  : FSM_STATE_REDUCE_PRECALC;
			FSM_STATE_REDUCE_UPDATE:	fsm_state <= helper_reduce_update_done  ? fsm_state_reduce_next    : FSM_STATE_REDUCE_UPDATE;
			FSM_STATE_COPY:				fsm_state <= helper_copy_done           ? FSM_STATE_DONE           : FSM_STATE_COPY;
			FSM_STATE_DONE:				fsm_state <= FSM_STATE_IDLE;
			default:							fsm_state <= FSM_STATE_IDLE;
		endcase
		
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)	fsm_state_dly <= FSM_STATE_IDLE;
		else						fsm_state_dly <= fsm_state;


	assign helper_init_ena				= (fsm_state == FSM_STATE_INIT)           && fsm_state_new;
	assign helper_invert_precalc_ena	= (fsm_state == FSM_STATE_INVERT_PRECALC) && fsm_state_new;
	assign helper_invert_compare_ena	= (fsm_state == FSM_STATE_INVERT_COMPARE) && fsm_state_new;
	assign helper_invert_update_ena	= (fsm_state == FSM_STATE_INVERT_UPDATE)  && fsm_state_new;
	assign helper_reduce_precalc_ena	= (fsm_state == FSM_STATE_REDUCE_PRECALC) && fsm_state_new;
	assign helper_reduce_update_ena	= (fsm_state == FSM_STATE_REDUCE_UPDATE)  && fsm_state_new;
	assign helper_copy_ena				= (fsm_state == FSM_STATE_COPY)           && fsm_state_new;
	
	
		//
		// Counter Increment
		//
	always @(posedge clk) begin
		//
		if ((fsm_state == FSM_STATE_INIT) && helper_init_done)
			round_counter <= round_counter_zero;
		//	
		if ((fsm_state == FSM_STATE_INVERT_UPDATE) && helper_invert_update_done)
			round_counter <= round_counter_next;
		//
		if ((fsm_state == FSM_STATE_REDUCE_UPDATE) && helper_reduce_update_done)
			round_counter <= round_counter_next;
		//
	end
		
		
		//
		// Q Address Selector
		//
	always @(*) begin
		//
		case (fsm_state)
			FSM_STATE_INIT:				q_addr = helper_init_q_addr;
			FSM_STATE_REDUCE_PRECALC:	q_addr = helper_reduce_precalc_q_addr;
			default:							q_addr = {OPERAND_ADDR_BITS{1'bX}};
		endcase
		//
	end
	
	
		//
		// Buffer Address Selector
		//
	always @(*) begin
		//
		// Write Ports
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_r_wr_addr = helper_init_r_addr;
			FSM_STATE_INVERT_UPDATE:	buf_r_wr_addr = helper_invert_update_r_addr;
			FSM_STATE_REDUCE_PRECALC:	buf_r_wr_addr = helper_reduce_precalc_r_addr;
			default:							buf_r_wr_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_s_wr_addr = helper_init_s_addr;
			FSM_STATE_INVERT_UPDATE:	buf_s_wr_addr = helper_invert_update_s_addr;
			FSM_STATE_REDUCE_UPDATE:	buf_s_wr_addr = helper_reduce_update_s_addr;
			default:							buf_s_wr_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_u_wr_addr = helper_init_u_addr;
			FSM_STATE_INVERT_UPDATE:	buf_u_wr_addr = helper_invert_update_u_addr;
			FSM_STATE_REDUCE_PRECALC:	buf_u_wr_addr = helper_reduce_precalc_u_addr;
			default:							buf_u_wr_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_v_wr_addr = helper_init_v_addr;
			FSM_STATE_INVERT_UPDATE:	buf_v_wr_addr = helper_invert_update_v_addr;
			FSM_STATE_REDUCE_PRECALC:	buf_v_wr_addr = helper_reduce_precalc_v_addr;
			default:							buf_v_wr_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		// Read Ports
		//
		case (fsm_state)
			FSM_STATE_INVERT_PRECALC:	buf_r_rd_addr = helper_invert_precalc_r_addr;
			default:							buf_r_rd_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INVERT_PRECALC:	buf_s_rd_addr = helper_invert_precalc_s_addr;
			FSM_STATE_REDUCE_PRECALC:	buf_s_rd_addr = helper_reduce_precalc_s_addr;
			FSM_STATE_COPY:				buf_s_rd_addr = helper_copy_s_addr;
			default:							buf_s_rd_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INVERT_PRECALC:	buf_u_rd_addr = helper_invert_precalc_u_addr;
			FSM_STATE_INVERT_COMPARE:	buf_u_rd_addr = helper_invert_compare_u_addr;
			FSM_STATE_REDUCE_UPDATE:	buf_u_rd_addr = helper_reduce_update_u_addr;
			default:							buf_u_rd_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INVERT_PRECALC:	buf_v_rd_addr = helper_invert_precalc_v_addr;
			FSM_STATE_INVERT_COMPARE:	buf_v_rd_addr = helper_invert_compare_v_addr;
			FSM_STATE_REDUCE_UPDATE:	buf_v_rd_addr = helper_reduce_update_v_addr;
			default:							buf_v_rd_addr = {BUFFER_ADDR_BITS{1'bX}};
		endcase
		//
	end
	
	
		//
		// Buffer Write Enable Logic
		//
	always @(*) begin
		//
		// Write Ports
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_r_wr_en  = helper_init_r_wren;
			FSM_STATE_INVERT_UPDATE:	buf_r_wr_en = helper_invert_update_r_wren;
			FSM_STATE_REDUCE_PRECALC:	buf_r_wr_en = helper_reduce_precalc_r_wren;
			default:							buf_r_wr_en = 1'b0;
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_s_wr_en = helper_init_s_wren;
			FSM_STATE_INVERT_UPDATE:	buf_s_wr_en = helper_invert_update_s_wren;
			FSM_STATE_REDUCE_UPDATE:	buf_s_wr_en = helper_reduce_update_s_wren;
			default:							buf_s_wr_en = 1'b0;
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_u_wr_en = helper_init_u_wren;
			FSM_STATE_INVERT_UPDATE:	buf_u_wr_en = helper_invert_update_u_wren;
			FSM_STATE_REDUCE_PRECALC:	buf_u_wr_en = helper_reduce_precalc_u_wren;
			default:							buf_u_wr_en = 1'b0;
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_v_wr_en = helper_init_v_wren;
			FSM_STATE_INVERT_UPDATE:	buf_v_wr_en = helper_invert_update_v_wren;
			FSM_STATE_REDUCE_PRECALC:	buf_v_wr_en = helper_reduce_precalc_v_wren;
			default:							buf_v_wr_en = 1'b0;
		endcase
		//
	end
	
	
		//
		// Buffer Write Data Selector
		//
	always @(*) begin
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_r_wr_din = helper_init_r_data;
			FSM_STATE_INVERT_UPDATE:	buf_r_wr_din = helper_invert_update_r_data;
			FSM_STATE_REDUCE_PRECALC:	buf_r_wr_din = helper_reduce_precalc_r_data;
			default:							buf_r_wr_din = {32{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_s_wr_din = helper_init_s_data;
			FSM_STATE_INVERT_UPDATE:	buf_s_wr_din = helper_invert_update_s_data;
			FSM_STATE_REDUCE_UPDATE:	buf_s_wr_din = helper_reduce_update_s_data;
			default:							buf_s_wr_din = {32{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_u_wr_din = helper_init_u_data;
			FSM_STATE_INVERT_UPDATE:	buf_u_wr_din = helper_invert_update_u_data;
			FSM_STATE_REDUCE_PRECALC:	buf_u_wr_din = helper_reduce_precalc_u_data;
			default:							buf_u_wr_din = {32{1'bX}};
		endcase
		//
		case (fsm_state)
			FSM_STATE_INIT:				buf_v_wr_din = helper_init_v_data;
			FSM_STATE_INVERT_UPDATE:	buf_v_wr_din = helper_invert_update_v_data;
			FSM_STATE_REDUCE_PRECALC:	buf_v_wr_din = helper_reduce_precalc_v_data;
			default:							buf_v_wr_din = {32{1'bX}};
		endcase
		//
	end
	
	
		//
		// Ready Logic
		//
	reg rdy_reg = 1'b1;

	assign rdy = rdy_reg;
	
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0) rdy_reg <= 1'b1;
		else begin
		
				/* clear */
			if (rdy && ena) rdy_reg <= 1'b0;
			
				/* set */
			if (!rdy && (fsm_state == FSM_STATE_DONE)) rdy_reg <= 1'b1;
			
		end
	
		
		//
		// Store Redundant Power of 2 (K)
		//
	always @(posedge clk)
		//
		if (helper_init_ena)
			k <= {K_NUM_BITS{1'b0}};
		else begin
		
			if (helper_invert_update_ena && !flag_invert_v_eq_1)
				k <= k + 1'b1;
				
			if (helper_reduce_update_ena && (k != {K_NUM_BITS{1'b0}}))
				k <= k - 1'b1;
				
		end
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
