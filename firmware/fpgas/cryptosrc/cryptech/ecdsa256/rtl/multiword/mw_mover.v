//------------------------------------------------------------------------------
//
// mw_mover.v
// -----------------------------------------------------------------------------
// Multi-word data mover.
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

module mw_mover
	(
		clk, rst_n,
		ena, rdy,
		x_addr, y_addr, y_wren,
		x_din, y_dout
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
	function	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_NEXT_OR_ZERO;
		input	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_CURRENT;
		begin
			WORD_INDEX_NEXT_OR_ZERO = (WORD_INDEX_CURRENT < WORD_INDEX_LAST) ?
				WORD_INDEX_CURRENT + 1'b1 : WORD_INDEX_ZERO;
		end
	endfunction
	

		//
		// Ports
		//
	input		wire											clk;			// system clock
	input		wire											rst_n;		// active-low async reset
	
	input		wire											ena;			// enable input
	output	wire											rdy;			// ready output
		
	output	wire	[WORD_COUNTER_WIDTH-1:0]		x_addr;		// address of current X word
	output	wire	[WORD_COUNTER_WIDTH-1:0]		y_addr;		// address of current Y word
	output	wire											y_wren;		// store current Y word
	
	input		wire	[                32-1:0]		x_din;		// current X word
	output	wire	[                32-1:0]		y_dout;		// current Y word
	
	
		//
		// Word Indices
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	index_x;
	reg	[WORD_COUNTER_WIDTH-1:0]	index_y;
	
	
		//
		// Output Mapping
		//
	assign x_addr	= index_x;
	assign y_addr	= index_y;


		//
		// FSM
		//
	localparam FSM_SHREG_WIDTH = 1 * OPERAND_NUM_WORDS + 2;
	
	reg	[FSM_SHREG_WIDTH-1:0]	fsm_shreg;
	
	assign rdy = fsm_shreg[0];
	
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_inc_index_x	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 1) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 0)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_inc_index_y	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 2) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 1)];
	
	wire inc_index_x	= |fsm_shreg_inc_index_x;
	wire inc_index_y	= |fsm_shreg_inc_index_y;
	wire store_word_y	= |fsm_shreg_inc_index_x;

	
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
		if (rdy) begin
			index_x <= WORD_INDEX_ZERO;
			index_y <= WORD_INDEX_ZERO;
		end else begin
			if (inc_index_x)	index_x <= WORD_INDEX_NEXT_OR_ZERO(index_x);
			if (inc_index_y)	index_y <= WORD_INDEX_NEXT_OR_ZERO(index_y);
		end
		
		
		//
		// Write Enable Logic
		//
	reg	y_wren_reg;
	
	assign y_wren = y_wren_reg;
	
	always @(posedge clk)
		//
		if (rdy)		y_wren_reg	<= 1'b0;
		else			y_wren_reg	<= store_word_y;
		
		
		//
		// Output Logic
		//
	assign y_dout = x_din;
		
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
