//------------------------------------------------------------------------------
//
// modular_subtractor.v
// -----------------------------------------------------------------------------
// Modular subtractor.
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

module modular_subtractor
	(
		clk, rst_n,
		ena, rdy,
		ab_addr, n_addr, d_addr, d_wren,
		a_din, b_din, n_din, d_dout
	);


		//
		// Parameters
		//
	parameter	OPERAND_NUM_WORDS		= 8;
	parameter	WORD_COUNTER_WIDTH	= 3;
	
	
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
	input		wire										clk;			// system clock
	input		wire										rst_n;		// active-low async reset
	
	input		wire										ena;			// enable input
	output	wire										rdy;			// ready output
	
	output	wire	[WORD_COUNTER_WIDTH-1:0]	ab_addr;		// index of current A and B words
	output	wire	[WORD_COUNTER_WIDTH-1:0]	n_addr;		// index of current N word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	d_addr;		// index of current D word
	output	wire										d_wren;		// store current D word now
	
	input		wire	[                  31:0]	a_din;		// A
	input		wire	[                  31:0]	b_din;		// B
	input		wire	[                  31:0]	n_din;		// N
	output	wire	[                  31:0]	d_dout;		// D = (A - B) mod N
	
	
		//
		// Word Indices
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	index_ab;
	reg	[WORD_COUNTER_WIDTH-1:0]	index_n;
	reg	[WORD_COUNTER_WIDTH-1:0]	index_d;
		
		/* map registers to output ports */
	assign ab_addr	= index_ab;
	assign n_addr	= index_n;
	assign d_addr	= index_d;


		//
		// Subtractor
		//
	wire	[31: 0]	sub32_d;
	wire				sub32_b_in;
	wire				sub32_b_out;
	
	subtractor32_wrapper subtractor32
	(
		.clk		(clk),
		.a			(a_din),
		.b			(b_din),
		.d			(sub32_d),
		.b_in		(sub32_b_in),
		.b_out	(sub32_b_out)
	);
	
	
		//
		// Adder
		//
	wire	[31: 0]	add32_s;
	wire				add32_c_in;
	wire				add32_c_out;
	
	adder32_wrapper adder32
	(
		.clk		(clk),
		.a			(sub32_d),
		.b			(n_din),
		.s			(add32_s),
		.c_in		(add32_c_in),
		.c_out	(add32_c_out)
	);
	
	
		//
		// FSM
		//
		
	localparam FSM_SHREG_WIDTH = 2*OPERAND_NUM_WORDS + 5;
	
	reg	[FSM_SHREG_WIDTH-1:0]	fsm_shreg;
	
	assign rdy = fsm_shreg[0];
	
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_inc_index_ab	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 1) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 0)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_inc_index_n		= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 2) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 1)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_store_dif_ab	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 3) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 2)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_store_dif_ab_n	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 4) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 3)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_store_data_d	= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 4) : FSM_SHREG_WIDTH - (2 * OPERAND_NUM_WORDS + 3)];
	wire [OPERAND_NUM_WORDS-1:0] fsm_shreg_inc_index_d		= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 5) : FSM_SHREG_WIDTH - (2 * OPERAND_NUM_WORDS + 4)];
	
	wire fsm_latch_msb_borrow	= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 2)];
	
	wire inc_index_ab		= |fsm_shreg_inc_index_ab;
	wire inc_index_n		= |fsm_shreg_inc_index_n;
	wire store_dif_ab		= |fsm_shreg_store_dif_ab;
	wire store_dif_ab_n	= |fsm_shreg_store_dif_ab_n;
	wire store_data_d		= |fsm_shreg_store_data_d;
	wire inc_index_d		= |fsm_shreg_inc_index_d;
	
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
		// Borrow & Carry Masking Logic
		//
	reg	sub32_b_mask;
	reg	add32_c_mask;
	
	
	always @(posedge clk) begin
		//
		sub32_b_mask <= (index_ab == WORD_INDEX_ZERO) ? 1'b1 : 1'b0;
		add32_c_mask <= (index_n  == WORD_INDEX_ZERO) ? 1'b1 : 1'b0;
		//
	end
	
	assign sub32_b_in = sub32_b_out & ~sub32_b_mask;	
	assign add32_c_in = add32_c_out & ~add32_c_mask;
	
	
	
		//
		// Borrow & Carry Latch Logic
		//
	reg sub32_borrow_latch;
	
	always @(posedge clk) begin
		//
		if (fsm_latch_msb_borrow) sub32_borrow_latch <= sub32_b_out;
		//
	end

		
		//
		// Intermediate Results
		//
	reg	[32*OPERAND_NUM_WORDS-1:0]		d_ab;
	reg	[32*OPERAND_NUM_WORDS-1:0]		d_ab_n;
	
	always @(posedge clk)
		//
		if (store_data_d) begin
			//
			d_ab		<= {{32{1'bX}}, d_ab[32*OPERAND_NUM_WORDS-1:32]};
			d_ab_n	<= {{32{1'bX}}, d_ab_n[32*OPERAND_NUM_WORDS-1:32]};		
			//
		end else begin
			//
			if (store_dif_ab) d_ab <= {sub32_d, d_ab[32*OPERAND_NUM_WORDS-1:32]};
			if (store_dif_ab_n) d_ab_n <= {add32_s, d_ab_n[32*OPERAND_NUM_WORDS-1:32]};
			//
		end
	
	
		//
		// Word Index Increment Logic
		//
	always @(posedge clk)
		//
		if (rdy) begin
			//
			index_ab		<= WORD_INDEX_ZERO;
			index_n		<= WORD_INDEX_ZERO;
			index_d		<= WORD_INDEX_ZERO;
			//
		end else begin
			//
			if (inc_index_ab) index_ab <= WORD_INDEX_NEXT_OR_ZERO(index_ab);
			if (inc_index_n)	index_n	<= WORD_INDEX_NEXT_OR_ZERO(index_n);
			if (inc_index_d)	index_d	<= WORD_INDEX_NEXT_OR_ZERO(index_d);
			//
		end
	
	
			//
			// Output Sum Selector
			//
	wire	mux_select_ab_n = sub32_borrow_latch;
			
	
			//
			// Output Data and Write Enable Logic
			//
	reg				d_wren_reg;
	reg	[31: 0]	d_dout_reg;
	wire	[31: 0]	d_dout_mux = mux_select_ab_n ? d_ab_n[31:0] : d_ab[31:0];
	
	assign d_wren = d_wren_reg;
	assign d_dout = d_dout_reg;
	
	always @(posedge clk)
		//
		if (rdy) begin
			//
			d_wren_reg	<= 1'b0;
			d_dout_reg	<= {32{1'bX}};
			//
		end else begin
			//
			d_wren_reg <= store_data_d;
			d_dout_reg <= store_data_d ? d_dout_mux : {32{1'bX}};
			//
		end			

	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
