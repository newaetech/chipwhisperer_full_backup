//------------------------------------------------------------------------------
//
// modular_reductor_256.v
// -----------------------------------------------------------------------------
// Modular reductor.
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

module modular_reductor_256
	(
		clk, rst_n,
		ena, rdy,
		x_addr, n_addr, p_addr, p_wren,
		x_din, n_din, p_dout
	);
		
		//
		// Constants
		//
	localparam	OPERAND_NUM_WORDS		= 8;
	localparam	WORD_COUNTER_WIDTH	= 3;
	
	
		//
		// Handy Numbers
		//
	localparam	[WORD_COUNTER_WIDTH:0]	WORD_INDEX_ZERO	= 0;
	localparam	[WORD_COUNTER_WIDTH:0]	WORD_INDEX_LAST	= 2 * OPERAND_NUM_WORDS - 1;
	
	
		//
		// Handy Functions
		//
	function	[WORD_COUNTER_WIDTH:0]	WORD_INDEX_PREVIOUS_OR_LAST;
		input	[WORD_COUNTER_WIDTH:0]	WORD_INDEX_CURRENT;
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
	
	output	wire	[WORD_COUNTER_WIDTH-0:0]	x_addr;	// index of current X word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	n_addr;	// index of current N word
	output	wire	[WORD_COUNTER_WIDTH-1:0]	p_addr;	// index of current P word
	output	wire										p_wren;	// store current P word now	
	
	input		wire	[                  31:0]	x_din;	// X
	input		wire	[                  31:0]	n_din;	// N (must be P-256!)
	output	wire	[                  31:0]	p_dout;	// P = X mod N
	
	
		//
		// Word Indices
		//
	reg	[WORD_COUNTER_WIDTH:0]	index_x;
	
		
		/* map registers to output ports */
	assign x_addr	= index_x;
	
	
		//
		// FSM
		//
	localparam	FSM_SHREG_WIDTH	= (2 * OPERAND_NUM_WORDS + 1) + (5 * 2) + 1;
	
	reg	[FSM_SHREG_WIDTH-1:0]	fsm_shreg;
	
	assign rdy = fsm_shreg[0];
	
	wire [2 * OPERAND_NUM_WORDS - 1:0]	fsm_shreg_inc_index_x	= fsm_shreg[FSM_SHREG_WIDTH - 0*OPERAND_NUM_WORDS - 1 -: 2 * OPERAND_NUM_WORDS];
	wire [2 * OPERAND_NUM_WORDS - 1:0]	fsm_shreg_store_word_z	= fsm_shreg[FSM_SHREG_WIDTH - 0*OPERAND_NUM_WORDS - 2 -: 2 * OPERAND_NUM_WORDS];
	wire [2 *                 5 - 1:0]	fsm_shreg_reduce_stages	= fsm_shreg[                                        1 +: 2 *                 5];
	
	wire [5-1:0] fsm_shreg_reduce_stage_start;
	wire [5-1:0] fsm_shreg_reduce_stage_stop;
	
	genvar s;
	generate for (s=0; s<5; s=s+1)
		begin : gen_fsm_shreg_reduce_stages
			assign fsm_shreg_reduce_stage_start[5 - (s + 1)]	= fsm_shreg_reduce_stages[2 * (5 - s) - 1];
			assign fsm_shreg_reduce_stage_stop[5 - (s + 1)]	= fsm_shreg_reduce_stages[2 * (5 - s) - 2];
		end
	endgenerate
	
	wire inc_index_x	= |fsm_shreg_inc_index_x;
	wire store_word_z	= |fsm_shreg_store_word_z;
	wire reduce_start	= |fsm_shreg_reduce_stage_start;
	wire reduce_stop	= |fsm_shreg_reduce_stage_stop;
	wire store_p		=  fsm_shreg_reduce_stage_stop[0];
	
	
	wire	reduce_adder0_done;
	wire	reduce_adder1_done;
	wire	reduce_subtractor_done;
	
	wire	reduce_done_all = reduce_adder0_done & reduce_adder1_done & reduce_subtractor_done;
	
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)
			//
			fsm_shreg <= {{FSM_SHREG_WIDTH-1{1'b0}}, 1'b1};
			//
		else begin
			//
			if (rdy)
				//
				fsm_shreg <= {ena, {FSM_SHREG_WIDTH-2{1'b0}}, ~ena};
			//
			else if (!reduce_stop || reduce_done_all)
				//
				fsm_shreg <= {1'b0, fsm_shreg[FSM_SHREG_WIDTH-1:1]};
			//
		end
	
		
		//
		// Word Index Increment Logic
		//
	always @(posedge clk)
		//
		if (rdy)
			//
			index_x <= WORD_INDEX_LAST;
			//
		else if (inc_index_x)
			//
			index_x	<= WORD_INDEX_PREVIOUS_OR_LAST(index_x);
			
			
		//
		// Look-up Table
		//
		
		//
		// Take a look at the corresponding C model for more information
		// on how exactly the math behind reduction works. The first step
		// is to assemble nine 256-bit values ("z-words") from 32-bit parts
		// of the full 512-bit product ("c-word"). The problem with z5 is
		// that it contains c13 two times. This implementation scans from
		// c15 to c0 and writes current part of c-word into corresponding
		// parts of z-words. Since those 32-bit parts are stored in block
		// memories, one source word can only be written to one location in
		// every z-word at a time. The trick is to delay c13 and then write
		// the delayed value at the corresponding location in z5 instead of
		// the next c12. "z_save" flag is used to indicate that the current
		// word should be delayed and written once again during the next cycle.
		//
		
	reg	[9*WORD_COUNTER_WIDTH-1:0]	z_addr;	//
	reg	[9                   -1:0]	z_wren;	//
	reg	[9                   -1:0]	z_mask;	// mask input to store zero word
	reg	[9                   -1:0]	z_save;	// save previous word once again
	
	always @(posedge clk)
		//
		if (inc_index_x)
			//
			case (index_x)
				//
				//                     s9     s8     s7     s6     s5     s4     s3     s2     s1
				//                     ||     ||     ||     ||     ||     ||     ||     ||     ||
                                // XXX changed from 3'dxx to 3'bxxx because iverilog doesn't like the latter
				4'd00:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd00};
				4'd01:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd01};
				4'd02:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd02};
				4'd03:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd03};
				4'd04:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd04};
				4'd05:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd05};
				4'd06:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd06};
				4'd07:	z_addr <= {3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'bxxx, 3'd07};
				4'd08:	z_addr <= {3'd02,  3'd03,  3'd04,  3'd06,  3'd07,  3'd00,  3'd00,  3'd00,  3'bxxx};
				4'd09:	z_addr <= {3'd03,  3'd04,  3'd06,  3'd03,  3'd00,  3'd01,  3'd01,  3'd01,  3'bxxx};
				4'd10:	z_addr <= {3'd04,  3'd05,  3'd05,  3'd07,  3'd01,  3'd02,  3'd02,  3'd02,  3'bxxx};
				4'd11:	z_addr <= {3'd05,  3'd06,  3'd07,  3'd00,  3'd02,  3'd03,  3'd07,  3'd03,  3'bxxx};
				4'd12:	z_addr <= {3'd06,  3'd07,  3'd00,  3'd01,  3'd06,  3'd04,  3'd03,  3'd04,  3'bxxx};
				4'd13:	z_addr <= {3'd07,  3'd00,  3'd01,  3'd02,  3'd03,  3'd05,  3'd04,  3'd05,  3'bxxx};
				4'd14:	z_addr <= {3'd00,  3'd01,  3'd02,  3'd04,  3'd04,  3'd06,  3'd05,  3'd06,  3'bxxx};
				4'd15:	z_addr <= {3'd01,  3'd02,  3'd03,  3'd05,  3'd05,  3'd07,  3'd06,  3'd07,  3'bxxx};
				//
            default:	z_addr <= {9*WORD_COUNTER_WIDTH{1'bX}};
				//
         endcase
	
	always @(posedge clk)
		//
		case (index_x)
			//
			//                     9     8     7     6     5     4     3     2     1
			//                     |     |     |     |     |     |     |     |     |
			4'd00:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd01:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd02:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd03:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd04:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd05:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd06:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd07:	z_wren <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1};
			4'd08:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd09:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd10:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd11:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd12:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd13:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd14:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			4'd15:	z_wren <= {1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b1, 1'b0};
			//
			default:	z_wren <= {9{1'b0}};
			//
		endcase
		
	always @(posedge clk)
		//
		if (inc_index_x)
			//
			case (index_x)
				//
				//                     9     8     7     6     5     4     3     2     1
				//                     |     |     |     |     |     |     |     |     |
				4'd00:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd01:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd02:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd03:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd04:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd05:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd06:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd07:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd08:	z_mask <= {1'b1, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b1, 1'b1, 1'b0};
				4'd09:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b1, 1'b1, 1'b0};
				4'd10:	z_mask <= {1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b1, 1'b1, 1'b0};
				4'd11:	z_mask <= {1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b1, 1'b1, 1'b0, 1'b0};
				4'd12:	z_mask <= {1'b1, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0};
				4'd13:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0};
				4'd14:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd15:	z_mask <= {1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				//
            default:	z_mask <= {9{1'bX}};
				//
         endcase
			
	always @(posedge clk)
		//
		if (inc_index_x)
			//
			case (index_x)
				//
				//                     9     8     7     6     5     4     3     2     1
				//                     |     |     |     |     |     |     |     |     |
				4'd00:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd01:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd02:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd03:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd04:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd05:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd06:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd07:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd08:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd09:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd10:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd11:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd12:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd13:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd14:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				4'd15:	z_save <= {1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0};
				//
            default:	z_save <= {9{1'bX}};
				//
         endcase
		
		
		//
		// Intermediate Numbers
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	reduce_z_addr[1:9];
	wire	[                32-1:0]	reduce_z_dout[1:9];
	
	reg	[31: 0]	x_din_dly;
	always @(posedge clk)
		//
		x_din_dly <= x_din;
		
	
	genvar z;
	generate for (z=1; z<=9; z=z+1)
		//
		begin : gen_z_bram
			//
			bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
			bram_c_inst
			(
				.clk		(clk),

				.a_addr	(z_addr[(z-1) * WORD_COUNTER_WIDTH +: WORD_COUNTER_WIDTH]),
				.a_wr		(z_wren[z-1] & store_word_z),
				.a_in		(z_mask[z-1] ? {32{1'b0}} : (z_save[z-1] ? x_din_dly : x_din)),
				.a_out	(),

				.b_addr	(reduce_z_addr[z]),
				.b_out	(reduce_z_dout[z])
			);
			//
		end
		//
	endgenerate
		
		
		
	
	wire	[                32-1:0]	bram_sum0_wr_din;
	wire	[WORD_COUNTER_WIDTH-1:0]	bram_sum0_wr_addr;
	wire										bram_sum0_wr_wren;
	
	wire	[                32-1:0]	bram_sum1_wr_din;
	wire	[WORD_COUNTER_WIDTH-1:0]	bram_sum1_wr_addr;
	wire										bram_sum1_wr_wren;
	
	wire	[                32-1:0]	bram_diff_wr_din;
	wire	[WORD_COUNTER_WIDTH-1:0]	bram_diff_wr_addr;
	wire										bram_diff_wr_wren;
	
	wire	[                32-1:0]	bram_sum0_rd_dout;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_sum0_rd_addr;
	
	wire	[                32-1:0]	bram_sum1_rd_dout;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_sum1_rd_addr;

	wire	[                32-1:0]	bram_diff_rd_dout;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_diff_rd_addr;

	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_sum0_inst
	(
		.clk		(clk),

		.a_addr	(bram_sum0_wr_addr),
		.a_wr		(bram_sum0_wr_wren),
		.a_in		(bram_sum0_wr_din),
		.a_out	(),

		.b_addr	(bram_sum0_rd_addr),
		.b_out	(bram_sum0_rd_dout)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_sum1_inst
	(
		.clk		(clk),

		.a_addr	(bram_sum1_wr_addr),
		.a_wr		(bram_sum1_wr_wren),
		.a_in		(bram_sum1_wr_din),
		.a_out	(),

		.b_addr	(bram_sum1_rd_addr),
		.b_out	(bram_sum1_rd_dout)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_diff_inst
	(
		.clk		(clk),

		.a_addr	(bram_diff_wr_addr),
		.a_wr		(bram_diff_wr_wren),
		.a_in		(bram_diff_wr_din),
		.a_out	(),

		.b_addr	(bram_diff_rd_addr),
		.b_out	(bram_diff_rd_dout)
	);
	
	
	wire	[WORD_COUNTER_WIDTH-1:0]	adder0_ab_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	adder1_ab_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	subtractor_ab_addr;
	
	reg	[                32-1:0]	adder0_a_din;
	reg	[                32-1:0]	adder0_b_din;
	
	reg	[                32-1:0]	adder1_a_din;
	reg	[                32-1:0]	adder1_b_din;
	
	reg	[                32-1:0]	subtractor_a_din;
	reg	[                32-1:0]	subtractor_b_din;
	
	// n_addr - only 1 output, because all modules are in sync
	
	modular_adder #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH)
	)
	adder_inst0
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(reduce_start),
		.rdy			(reduce_adder0_done),
		
		.ab_addr		(adder0_ab_addr),
		.n_addr		(),
		.s_addr		(bram_sum0_wr_addr),
		.s_wren		(bram_sum0_wr_wren),
		
		.a_din		(adder0_a_din),
		.b_din		(adder0_b_din),
		.n_din		(n_din),
		.s_dout		(bram_sum0_wr_din)
	);
	
	modular_adder #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH)
	)
	adder_inst1
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(reduce_start),
		.rdy			(reduce_adder1_done),
		
		.ab_addr		(adder1_ab_addr),
		.n_addr		(),
		.s_addr		(bram_sum1_wr_addr),
		.s_wren		(bram_sum1_wr_wren),
		
		.a_din		(adder1_a_din),
		.b_din		(adder1_b_din),
		.n_din		(n_din),
		.s_dout		(bram_sum1_wr_din)
	);
	
	modular_subtractor #
	(
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS),
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH)
	)
	subtractor_inst
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(reduce_start),
		.rdy			(reduce_subtractor_done),
		
		.ab_addr		(subtractor_ab_addr),
		.n_addr		(n_addr),
		.d_addr		(bram_diff_wr_addr),
		.d_wren		(bram_diff_wr_wren),
		
		.a_din		(subtractor_a_din),
		.b_din		(subtractor_b_din),
		.n_din		(n_din),
		.d_dout		(bram_diff_wr_din)
	);
	
	
		//
		// Address (Operand) Selector
		//
	always @(*)
		//
		case (fsm_shreg_reduce_stage_stop)
			//
			5'b10000: begin
				reduce_z_addr[1]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[2]	= adder0_ab_addr;
				reduce_z_addr[3]	= adder1_ab_addr;
				reduce_z_addr[4]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[5]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[6]	= subtractor_ab_addr;
				reduce_z_addr[7]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[8]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[9]	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum0_rd_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum1_rd_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_diff_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
			end
			//
			5'b01000: begin
				reduce_z_addr[1]	= adder0_ab_addr;
				reduce_z_addr[2]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[3]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[4]	= adder1_ab_addr;
				reduce_z_addr[5]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[6]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[7]	= subtractor_ab_addr;
				reduce_z_addr[8]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[9]	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum0_rd_addr	= adder0_ab_addr;
				bram_sum1_rd_addr	= adder1_ab_addr;
				bram_diff_rd_addr = subtractor_ab_addr;
			end
			//
			5'b00100: begin
				reduce_z_addr[1]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[2]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[3]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[4]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[5]	= adder0_ab_addr;
				reduce_z_addr[6]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[7]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[8]	= subtractor_ab_addr;
				reduce_z_addr[9]	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum0_rd_addr	= adder0_ab_addr;
				bram_sum1_rd_addr	= adder1_ab_addr;
				bram_diff_rd_addr = subtractor_ab_addr;
			end
			//
			5'b00010: begin
				reduce_z_addr[1]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[2]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[3]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[4]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[5]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[6]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[7]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[8]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[9]	= subtractor_ab_addr;
				bram_sum0_rd_addr	= adder0_ab_addr;
				bram_sum1_rd_addr	= adder0_ab_addr;
				bram_diff_rd_addr = subtractor_ab_addr;
			end
			//
			5'b00001: begin
				reduce_z_addr[1]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[2]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[3]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[4]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[5]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[6]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[7]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[8]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[9]	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum0_rd_addr	= adder0_ab_addr;
				bram_sum1_rd_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_diff_rd_addr = adder0_ab_addr;
			end			
			//
			default: begin
				reduce_z_addr[1]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[2]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[3]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[4]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[5]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[6]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[7]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[8]	= {WORD_COUNTER_WIDTH{1'bX}};
				reduce_z_addr[9]	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum0_rd_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_sum1_rd_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				bram_diff_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
			end
			//
		endcase

	
		//
		// adder 0
		//
	always @(*) begin
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	adder0_a_din = reduce_z_dout[2];
			5'b01000:	adder0_a_din = bram_sum0_rd_dout;
			5'b00100:	adder0_a_din = bram_sum0_rd_dout;
			5'b00010:	adder0_a_din = bram_sum0_rd_dout;
			5'b00001:	adder0_a_din = bram_sum0_rd_dout;
			default:		adder0_a_din = {32{1'bX}};
		endcase
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	adder0_b_din = reduce_z_dout[2];
			5'b01000:	adder0_b_din = reduce_z_dout[1];
			5'b00100:	adder0_b_din = reduce_z_dout[5];
			5'b00010:	adder0_b_din = bram_sum1_rd_dout;
			5'b00001:	adder0_b_din = bram_diff_rd_dout;
			default:		adder0_b_din = {32{1'bX}};
		endcase
		//
	end
	
		//
		// adder 1
		//
	always @(*) begin
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	adder1_a_din = reduce_z_dout[3];
			5'b01000:	adder1_a_din = bram_sum1_rd_dout;
			5'b00100:	adder1_a_din = bram_sum1_rd_dout;
			5'b00010:	adder1_a_din = {32{1'bX}};
			5'b00001:	adder1_a_din = {32{1'bX}};
			default:		adder1_a_din = {32{1'bX}};
		endcase
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	adder1_b_din = reduce_z_dout[3];
			5'b01000:	adder1_b_din = reduce_z_dout[4];
			5'b00100:	adder1_b_din = {32{1'b0}};
			5'b00010:	adder1_b_din = {32{1'bX}};
			5'b00001:	adder1_b_din = {32{1'bX}};
			default:		adder1_b_din = {32{1'bX}};
		endcase
		//
	end
	
	
		//
		// subtractor
		//
	always @(*) begin
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	subtractor_a_din = {32{1'b0}};
			5'b01000:	subtractor_a_din = bram_diff_rd_dout;
			5'b00100:	subtractor_a_din = bram_diff_rd_dout;
			5'b00010:	subtractor_a_din = bram_diff_rd_dout;
			5'b00001:	subtractor_a_din = {32{1'bX}};
			default:		subtractor_a_din = {32{1'bX}};
		endcase
		//
		case (fsm_shreg_reduce_stage_stop)
			5'b10000:	subtractor_b_din = reduce_z_dout[6];
			5'b01000:	subtractor_b_din = reduce_z_dout[7];
			5'b00100:	subtractor_b_din = reduce_z_dout[8];
			5'b00010:	subtractor_b_din = reduce_z_dout[9];
			5'b00001:	subtractor_b_din = {32{1'bX}};
			default:		subtractor_b_din = {32{1'bX}};
		endcase
		//
	end


		//
		// Address Mapping
		//
	assign p_addr	= bram_sum0_wr_addr;
	assign p_wren	= bram_sum0_wr_wren & store_p;
	assign p_dout	= bram_sum0_wr_din;
		
		
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
