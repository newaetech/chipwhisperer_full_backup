//------------------------------------------------------------------------------
//
// mw_comparator.v
// -----------------------------------------------------------------------------
// Multi-word comparator.
//
// Authors: Pavel Shatov
//
// Copyright (c) 2015-2016, NORDUnet A/S
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

module mw_comparator
	(
		clk, rst_n,
		ena, rdy,
		xy_addr, x_din, y_din,
		cmp_l, cmp_e, cmp_g
	);
	
		
		//
		// Parameters
		//
	parameter	WORD_COUNTER_WIDTH	= 3;
	parameter	OPERAND_NUM_WORDS		= 8;
	
	
		//
		// Handy Numbers
		//
	localparam	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_ZERO	= 0;
	localparam	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_LAST	= OPERAND_NUM_WORDS - 1;
	
	
		//
		// Handy Functions
		//
	function	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_PREV_OR_LAST;
		input	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_CURRENT;
		begin
			WORD_INDEX_PREV_OR_LAST = (WORD_INDEX_CURRENT > WORD_INDEX_ZERO) ?
				WORD_INDEX_CURRENT - 1'b1 : WORD_INDEX_LAST;
		end
	endfunction
	

		//
		// Ports
		//
	input		wire											clk;			// system clock
	input		wire											rst_n;		// active-low async reset
	
	input		wire											ena;			// enable input
	output	wire											rdy;			// ready output
		
	output	wire	[WORD_COUNTER_WIDTH-1:0]		xy_addr;		// address of current X and Y words
	input		wire	[                32-1:0]		x_din;		// current X word
	input		wire	[                32-1:0]		y_din;		// current Y word
	
	output	wire											cmp_l;		// X < Y ?
	output	wire											cmp_e;		// X = Y ?
	output	wire											cmp_g;		// X > Y ?
	
	
		//
		// Word Indices
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	index_xy;
	
	reg										reg_cmp_l;
	reg										reg_cmp_e;
	reg										reg_cmp_g;
	
	
		//
		// Output Mapping
		//
	assign xy_addr	= index_xy;
	
	assign cmp_l = reg_cmp_l;
	assign cmp_e = reg_cmp_e;
	assign cmp_g = reg_cmp_g;


		//
		// FSM
		//
	localparam FSM_SHREG_WIDTH = 1 * OPERAND_NUM_WORDS + 3;
	
	reg	[FSM_SHREG_WIDTH-1:0]	fsm_shreg;
	
	assign rdy = fsm_shreg[0];
	
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_dec_index_xy	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 1) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 0)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_calc_leg			= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 3) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 2)];
	wire                         fsm_shreg_calc_leg_last	= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 2)];
	
	wire dec_index_xy		= |fsm_shreg_dec_index_xy;
	wire calc_leg			= |fsm_shreg_calc_leg;
	wire calc_leg_last	=  fsm_shreg_calc_leg_last;

	
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)
			//
			fsm_shreg <= {{FSM_SHREG_WIDTH-1{1'b0}}, 1'b1};
			//
		else begin
			//
			if (rdy)	fsm_shreg <= {ena, {FSM_SHREG_WIDTH-2{1'b0}}, ~ena};
			//
			else		fsm_shreg <= {1'b0, fsm_shreg[FSM_SHREG_WIDTH-1:1]};
			//
		end


		//
		// Word Index Increment Logic
		//
	always @(posedge clk)
		//
		if (rdy)						index_xy <= WORD_INDEX_LAST;
		else if (dec_index_xy)	index_xy <= WORD_INDEX_PREV_OR_LAST(index_xy);
		
		
		//
		// 32-bit Subtractor
		//	
	wire	[31: 0]	sub32_d_out;
	wire				sub32_b_in;
	wire				sub32_b_out;
	
	subtractor32_wrapper subtractor32_inst
	(
		.clk		(clk),
		
		.a			(x_din),
		.b			(y_din),
		
		.d			(sub32_d_out),
		
		.b_in		(sub32_b_in),
		.b_out	(sub32_b_out)
	);
	
	
		//
		// Borrow Masking Logic
		//
	reg	sub32_b_mask;
	
	always @(posedge clk)
		//
		sub32_b_mask <= (index_xy  == WORD_INDEX_LAST) ? 1'b1 : 1'b0;
		
	assign sub32_b_in = sub32_b_out & ~sub32_b_mask;
		
		//
		// Output Logic
		//
	wire	cmp_unresolved = !(cmp_l || cmp_g);
	
	wire	cmp_borrow_is_set				= (sub32_b_out ==  1'b1) ? 1'b1 : 1'b0;
	wire	cmp_difference_is_nonzero	= (sub32_d_out != 32'd0) ? 1'b1 : 1'b0;
	
	always @(posedge clk)
		//
		if (rdy) begin
			//
			if (ena) begin
				//
				reg_cmp_l	<= 1'b0;
				reg_cmp_e	<= 1'b0;
				reg_cmp_g	<= 1'b0;
				//
			end
			//
		end else if (cmp_unresolved && calc_leg) begin
			//
			if ( cmp_borrow_is_set)																	reg_cmp_l <= 1'b1;
			if (!cmp_borrow_is_set &&  cmp_difference_is_nonzero)							reg_cmp_g <= 1'b1;
			if (!cmp_borrow_is_set && !cmp_difference_is_nonzero && calc_leg_last)	reg_cmp_e <= 1'b1;
			//
		end
		
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
