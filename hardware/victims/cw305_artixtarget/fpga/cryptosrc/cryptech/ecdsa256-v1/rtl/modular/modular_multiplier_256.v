//------------------------------------------------------------------------------
//
// modular_multiplier_256.v
// -----------------------------------------------------------------------------
// Modular multiplier.
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

module modular_multiplier_256
	(
		clk, rst_n,
		ena, rdy,
		a_addr, b_addr, n_addr, p_addr, p_wren,
		a_din, b_din, n_din, p_dout
	);
	
	
		//
		// Constants
		//
	localparam	OPERAND_NUM_WORDS					= 8;
	localparam	WORD_COUNTER_WIDTH				= 3;
	
	
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
	
	function	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_PREVIOUS_OR_LAST;
		input	[WORD_COUNTER_WIDTH-1:0]	WORD_INDEX_CURRENT;
		begin
			WORD_INDEX_PREVIOUS_OR_LAST = (WORD_INDEX_CURRENT > WORD_INDEX_ZERO) ?
				WORD_INDEX_CURRENT - 1'b1 : WORD_INDEX_LAST;
		end
	endfunction
	
	
		//
		// Ports
		//
	input		wire										clk;		// system clock
	input		wire										rst_n;	// active-low async reset
	
	input		wire										ena;		// enable input
	output	wire										rdy;		// ready output
	
	output	wire	[WORD_COUNTER_WIDTH-1:0]	a_addr;	// index of current A word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	b_addr;	// index of current B word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	n_addr;	// index of current N word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	p_addr;	// index of current P word
	output	wire										p_wren;	// store current P word now	
	
	input		wire	[                  31:0]	a_din;	// A
	input		wire	[                  31:0]	b_din;	// B
	input		wire	[                  31:0]	n_din;	// N (must be P-256!)
	output	wire	[                  31:0]	p_dout;	// P = A * B mod N
	
	
		//
		// Word Indices
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	index_a;
	reg	[WORD_COUNTER_WIDTH-1:0]	index_b;
		
		/* map registers to output ports */
	assign a_addr	= index_a;
	assign b_addr	= index_b;
	
		//
		// FSM
		//
	localparam	FSM_SHREG_WIDTH	= (1 * OPERAND_NUM_WORDS + 1) + (2 * OPERAND_NUM_WORDS + 1) + (2 * OPERAND_NUM_WORDS + 2) + (0 * OPERAND_NUM_WORDS + 2) + 1;
	
	reg	[FSM_SHREG_WIDTH-1:0]	fsm_shreg;
	
	assign rdy = fsm_shreg[0];
	
	wire [1 * OPERAND_NUM_WORDS-1:0]	fsm_shreg_inc_index_a	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 1) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 0)];
	wire [1 * OPERAND_NUM_WORDS-1:0]	fsm_shreg_store_word_a	= fsm_shreg[FSM_SHREG_WIDTH - (0 * OPERAND_NUM_WORDS + 2) : FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 1)];
	wire [2 * OPERAND_NUM_WORDS-1:0]	fsm_shreg_inc_index_b	= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 1) : FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 1)];
	wire [2 * OPERAND_NUM_WORDS-2:0]	fsm_shreg_store_si_msb	= fsm_shreg[FSM_SHREG_WIDTH - (1 * OPERAND_NUM_WORDS + 2) : FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 1)];
	wire [0 * OPERAND_NUM_WORDS-0:0] fsm_shreg_store_si_lsb	= fsm_shreg[FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 2) : FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 2)];
	wire [2 * OPERAND_NUM_WORDS-2:0]	fsm_shreg_shift_si		= fsm_shreg[FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 3) : FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 1)];
	wire [0 * OPERAND_NUM_WORDS-0:0]	fsm_shreg_mask_cw1_sum	= fsm_shreg[FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 4) : FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 4)];
	wire [2 * OPERAND_NUM_WORDS-1:0]	fsm_shreg_store_c_word	= fsm_shreg[FSM_SHREG_WIDTH - (3 * OPERAND_NUM_WORDS + 5) : FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 4)];
	wire [0 * OPERAND_NUM_WORDS-0:0]	fsm_shreg_reduce_start	= fsm_shreg[FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 5) : FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 5)];
	wire [0 * OPERAND_NUM_WORDS-0:0]	fsm_shreg_reduce_stop	= fsm_shreg[FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 6) : FSM_SHREG_WIDTH - (5 * OPERAND_NUM_WORDS + 6)];
	
	wire inc_index_a		= |fsm_shreg_inc_index_a;
	wire store_word_a		= |fsm_shreg_store_word_a;
	wire inc_index_b		= |fsm_shreg_inc_index_b;
	wire clear_mac_ab		= |fsm_shreg_inc_index_b;
	wire shift_wide_a		= |fsm_shreg_inc_index_b;
	wire enable_mac_ab	= |fsm_shreg_inc_index_b;
	wire store_si_msb		= |fsm_shreg_store_si_msb;
	wire store_si_lsb		=  fsm_shreg_store_si_lsb;
	wire shift_si			= |fsm_shreg_shift_si;
	wire mask_cw1_sum		=  fsm_shreg_mask_cw1_sum;
	wire store_c_word		= |fsm_shreg_store_c_word;
	wire reduce_start		=  fsm_shreg_reduce_start;
	wire reduce_stop		=  fsm_shreg_reduce_stop;
	
	
		//
		// FSM Logic
		//
	wire	reduce_done;
		
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)
			//
			fsm_shreg <= {{FSM_SHREG_WIDTH-1{1'b0}}, 1'b1};
			//
		else begin
			//
			if (rdy)
				fsm_shreg <= {ena, {FSM_SHREG_WIDTH-2{1'b0}}, ~ena};
			//
			else if (!reduce_stop || reduce_done)
				fsm_shreg <= {1'b0, fsm_shreg[FSM_SHREG_WIDTH-1:1]};
			//
		end
	
		
		//
		// Word Index Increment Logic
		//
	reg	index_b_ff;
	
	always @(posedge clk)
		//
		if (inc_index_b) index_b_ff <= ~index_b_ff;
		else index_b_ff <= 1'b0;
	
	always @(posedge clk)
		//
		if (rdy) begin
			//
			index_a		<= WORD_INDEX_ZERO;
			index_b		<= WORD_INDEX_LAST;
			//
		end else begin
			//
			if (inc_index_a)						index_a	<= WORD_INDEX_NEXT_OR_ZERO(index_a);
			if (inc_index_b && !index_b_ff)	index_b	<= WORD_INDEX_PREVIOUS_OR_LAST(index_b);
			//
		end
		
		
		//
		// Wide Operand Buffer
		//
	reg	[255:0]	buf_a_wide;
	
	always @(posedge clk)
		//
		if (store_word_a)
			buf_a_wide <= {buf_a_wide[16 +: 256 - 3 * 16], {a_din[15:0], a_din[31:16]}, buf_a_wide[256 - 2 * 16 +: 16]};
		else if (shift_wide_a)
			buf_a_wide <= {buf_a_wide[256-(16+1):0], buf_a_wide[256-16+:16]};
		
		
		//
		// Multiplier Array
		//
	wire	mac_inhibit;			// control signal to pause all accumulators
	
	wire	[46: 0]	mac[0:15];	// outputs of all accumulators
	reg	[15: 0]	mac_clear;	// individual per-accumulator clear flag
	
	assign mac_inhibit = ~enable_mac_ab;
	
	always @(posedge clk)
		//
		if (!clear_mac_ab)
			mac_clear <= {16{1'b1}};
		else begin
		
			if (mac_clear == {16{1'b1}})
				mac_clear <= {{14{1'b0}}, 1'b1, {1{1'b0}}};
			else
				mac_clear <= (mac_clear[15] == 1'b0) ? {mac_clear[14:0], 1'b0} : {16{1'b1}};
				
		
		end
	
		//
		// Array of parallel multipliers
		//
	genvar i;
	generate for (i=0; i<16; i=i+1)
		begin : gen_mac_array
			//
			mac16_wrapper mac16_inst
			(
				.clk		(clk),
				.ce		(~mac_inhibit),
				
				.clr		(mac_clear[i]),
				
				.a			(buf_a_wide[16*i+:16]),
				.b			(index_b_ff ? b_din[15:0] : b_din[31:16]),
				.s			(mac[i])
			);
			//
		end
	endgenerate
	
		//
		// Intermediate Words
		//
	reg	[47*(2*OPERAND_NUM_WORDS-1)-1:0]	si_msb;
	reg	[47*(2*OPERAND_NUM_WORDS-0)-1:0]	si_lsb;
	
	
	wire	[47*(2*OPERAND_NUM_WORDS-1)-1:0]	si_msb_new;
	wire	[47*(2*OPERAND_NUM_WORDS-0)-1:0]	si_lsb_new;

	generate for (i=0; i<16; i=i+1)
		begin : gen_si_lsb_new
			assign si_lsb_new[47*i+:47] = mac[15-i];
		end
	endgenerate
	
	generate for (i=1; i<16; i=i+1)
		begin : gen_si_msb_new
			assign si_msb_new[47*(15-i)+:47] = mac_clear[i] ? mac[i] : si_msb[47*(15-i)+:47];
		end
	endgenerate
	
	always @(posedge clk) begin
		//
		if (shift_si) begin
			si_msb <= {{2*47{1'b0}}, si_msb[15*47-1:2*47]};
			si_lsb <= {si_msb[2*47-1:0], si_lsb[16*47-1:2*47]};
		end else begin
		
			if (store_si_msb)
				si_msb <= si_msb_new;
			
			if (store_si_lsb)
				si_lsb <= si_lsb_new;
		end
			
	end
	
				
		//
		// Accumulators
		//
	wire	[46: 0]	add47_cw0_s;
	wire	[46: 0]	add47_cw1_s;
	
	
		//
		// cw0, b, cw1, b
		//
	reg	[30: 0]	si_prev_dly;
	reg	[15: 0]	si_next_dly;
	
	always @(posedge clk)
		//
		if (shift_si)
			si_prev_dly <= si_lsb[93:63];
		else
			si_prev_dly <= {31{1'b0}};
			
	always @(posedge clk)
		//
		si_next_dly <= si_lsb[62:47];
	
	wire	[46: 0]	add47_cw0_a = si_lsb[46:0];
	wire	[46: 0]	add47_cw0_b = {{16{1'b0}}, si_prev_dly};
	
	wire	[46: 0]	add47_cw1_a = add47_cw0_s;
	wire	[46: 0]	add47_cw1_b = {{15{1'b0}}, si_next_dly, mask_cw1_sum ? {16{1'b0}} : {1'b0, add47_cw1_s[46:32]}};	
	
	adder47_wrapper add47_cw0_inst
	(
		.clk	(clk),
		.a		(add47_cw0_a),
		.b		(add47_cw0_b),
		.s		(add47_cw0_s)
	);
	
	adder47_wrapper add47_cw1_inst
	(
		.clk	(clk),
		.a		(add47_cw1_a),
		.b		(add47_cw1_b),
		.s		(add47_cw1_s)
	);
	
	
	
		//
		// Full-Size Product
		//
	reg	[WORD_COUNTER_WIDTH:0]	bram_c_addr;
	
	wire	[WORD_COUNTER_WIDTH:0]	reduce_c_addr;
	wire	[                31:0]	reduce_c_word;
	
	always @(posedge clk)
		//
		if (store_c_word)
			bram_c_addr <= bram_c_addr + 1'b1;
		else
			bram_c_addr <= {2*WORD_COUNTER_WIDTH{1'b0}};
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH + 1)
	)
	bram_c_inst
	(
		.clk		(clk),

		.a_addr	(bram_c_addr),
		.a_wr		(store_c_word),
		.a_in		(add47_cw1_s[31:0]),
		.a_out	(),

		.b_addr	(reduce_c_addr),
		.b_out	(reduce_c_word)
	);
	
	
		//
		// Reduction Stage
		//
	modular_reductor_256 reduce_256_inst
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(reduce_start),
		.rdy		(reduce_done),
		
		.x_addr	(reduce_c_addr),
		.n_addr	(n_addr),
		.p_addr	(p_addr),
		.p_wren	(p_wren),
		
		.x_din	(reduce_c_word),
		.n_din	(n_din),
		.p_dout	(p_dout)
	);
	
		
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
