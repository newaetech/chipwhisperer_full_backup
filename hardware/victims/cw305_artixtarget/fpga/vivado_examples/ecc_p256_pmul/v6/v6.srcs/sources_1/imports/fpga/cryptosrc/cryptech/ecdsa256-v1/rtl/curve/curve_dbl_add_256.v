//------------------------------------------------------------------------------
//
// curve_dbl_add_256.v
// -----------------------------------------------------------------------------
// Elliptic curve point adder and doubler.
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

module curve_dbl_add_256
	(
		clk, rst_n,
		ena, rdy,
		uop_addr, uop,
		px_addr, py_addr, pz_addr, rx_addr, ry_addr, rz_addr, q_addr, v_addr,
		rx_wren, ry_wren, rz_wren,
		px_din, py_din, pz_din,
		rx_din, ry_din, rz_din,
		rx_dout, ry_dout, rz_dout, q_din, v_din,
                // XXX NEW 
                brom_g_x_addr, brom_g_x_dout,
                brom_g_y_addr, brom_g_y_dout
	);


		//
		// Microcode
		//
`include "uop_ecdsa.v"


		//
		// Constants
		//
	localparam	WORD_COUNTER_WIDTH	= 3;	// 0 .. 7
	localparam	OPERAND_NUM_WORDS		= 8;	// 8 * 32 = 256

	
		//
		// Ports
		//
	input		wire	clk;		// system clock
	input		wire	rst_n;	// active-low async reset
	
	input		wire	ena;		// enable input
	output	wire	rdy;		// ready output
		
	output	reg	[ 6-1: 0]	uop_addr;
	input		wire	[20-1: 0]	uop;
		
	output	reg	[WORD_COUNTER_WIDTH-1:0]	px_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	py_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	pz_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	rx_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	ry_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	rz_addr;
	output	reg	[WORD_COUNTER_WIDTH-1:0]	v_addr;
	output	wire	[WORD_COUNTER_WIDTH-1:0]	q_addr;
	
	output	wire										rx_wren;
	output	wire										ry_wren;
	output	wire										rz_wren;

	input		wire	[                32-1:0]	px_din;
	input		wire	[                32-1:0]	py_din;
	input		wire	[                32-1:0]	pz_din;
	input		wire	[                32-1:0]	rx_din;
	input		wire	[                32-1:0]	ry_din;
	input		wire	[                32-1:0]	rz_din;
	output	wire	[                32-1:0]	rx_dout;
	output	wire	[                32-1:0]	ry_dout;
	output	wire	[                32-1:0]	rz_dout;
	input		wire	[                32-1:0]	q_din;
	input		wire	[                32-1:0]	v_din;

        // XXX NEW 
	output reg	[WORD_COUNTER_WIDTH-1:0]	brom_g_x_addr;
	output reg	[WORD_COUNTER_WIDTH-1:0]	brom_g_y_addr;
	input  wire	[                32-1:0]	brom_g_x_dout;
	input  wire	[                32-1:0]	brom_g_y_dout;

	
		//
		// Microcode
		//   
	wire	[ 4: 0]	uop_opcode	= uop[19:15];
	wire	[ 4: 0]	uop_src_a	= uop[14:10];
	wire	[ 4: 0]	uop_src_b	= uop[ 9: 5];
	wire	[ 2: 0]	uop_dst		= uop[ 4: 2];
	wire	[ 1: 0]	uop_exec		= uop[ 1: 0];
	

		//
		// Multi-Word Comparator
		//
	wire	mw_cmp_ena;
	wire	mw_cmp_rdy;
		
	wire	mw_cmp_out_l;
	wire	mw_cmp_out_e;
	wire	mw_cmp_out_g;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	mw_cmp_addr_xy;
	
	wire	[                32-1:0]	mw_cmp_din_x;
	wire	[                32-1:0]	mw_cmp_din_y;

		// flags
	reg	flag_pz_is_zero;
	reg	flag_t1_is_zero;
	reg	flag_t2_is_zero;
	
	mw_comparator #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)
	)
	mw_comparator_inst
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(mw_cmp_ena),
		.rdy			(mw_cmp_rdy),
		
		.xy_addr		(mw_cmp_addr_xy),
		.x_din		(mw_cmp_din_x),
		.y_din		(mw_cmp_din_y),
		
		.cmp_l		(mw_cmp_out_l),
		.cmp_e		(mw_cmp_out_e),
		.cmp_g		(mw_cmp_out_g)
	);
	
	
		//
		// Modular Adder
		//
	wire	mod_add_ena;
	wire	mod_add_rdy;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_add_addr_ab;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_add_addr_n;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_add_addr_s;
	wire										mod_add_wren_s;
	
	wire	[                32-1:0]	mod_add_din_a;
	wire	[                32-1:0]	mod_add_din_b;
	wire	[                32-1:0]	mod_add_din_n;
	wire	[                32-1:0]	mod_add_dout_s;
	
	assign mod_add_din_n = q_din;
	
	modular_adder #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)
	)
	modular_adder_inst
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(mod_add_ena),
		.rdy			(mod_add_rdy),
		
		.ab_addr		(mod_add_addr_ab),
		.n_addr		(mod_add_addr_n),
		.s_addr		(mod_add_addr_s),
		.s_wren		(mod_add_wren_s),
		
		.a_din		(mod_add_din_a),
		.b_din		(mod_add_din_b),
		.n_din		(mod_add_din_n),
		.s_dout		(mod_add_dout_s)
	);
	
	
		//
		// Modular Subtractor
		//
	wire	mod_sub_ena;
	wire	mod_sub_rdy;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_sub_addr_ab;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_sub_addr_n;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_sub_addr_d;
	wire										mod_sub_wren_d;
	
	wire	[                32-1:0]	mod_sub_din_a;
	wire	[                32-1:0]	mod_sub_din_b;
	wire	[                32-1:0]	mod_sub_din_n;
	wire	[                32-1:0]	mod_sub_dout_d;
	
	assign mod_sub_din_n = q_din;
	
	modular_subtractor #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)
	)
	modular_subtractor_inst
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(mod_sub_ena),
		.rdy			(mod_sub_rdy),
		
		.ab_addr		(mod_sub_addr_ab),
		.n_addr		(mod_sub_addr_n),
		.d_addr		(mod_sub_addr_d),
		.d_wren		(mod_sub_wren_d),
		
		.a_din		(mod_sub_din_a),
		.b_din		(mod_sub_din_b),
		.n_din		(mod_sub_din_n),
		.d_dout		(mod_sub_dout_d)
	);
	
	
		//
		// Modular Multiplier
		//
	wire	mod_mul_ena;
	wire	mod_mul_rdy;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_mul_addr_a;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_mul_addr_b;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_mul_addr_n;
	wire	[WORD_COUNTER_WIDTH-1:0]	mod_mul_addr_p;
	wire										mod_mul_wren_p;
	
	wire	[                32-1:0]	mod_mul_din_a;
	wire	[                32-1:0]	mod_mul_din_b;
	wire	[                32-1:0]	mod_mul_din_n;
	wire	[                32-1:0]	mod_mul_dout_p;
	
	assign mod_mul_din_n = q_din;
	
	modular_multiplier_256 modular_multiplier_inst
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(mod_mul_ena),
		.rdy		(mod_mul_rdy),
		
		.a_addr	(mod_mul_addr_a),
		.b_addr	(mod_mul_addr_b),
		.n_addr	(mod_mul_addr_n),
		.p_addr	(mod_mul_addr_p),
		.p_wren	(mod_mul_wren_p),
		
		.a_din	(mod_mul_din_a),
		.b_din	(mod_mul_din_b),
		.n_din	(mod_mul_din_n),
		.p_dout	(mod_mul_dout_p)
	);
	
	
		//
		// Multi-Word Data Mover
		//
	wire	mw_mov_ena;
	wire	mw_mov_rdy;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	mw_mov_addr_x;
	wire	[WORD_COUNTER_WIDTH-1:0]	mw_mov_addr_y;
	wire										mw_mov_wren_y;
	
	wire	[                32-1:0]	mw_mov_din_x;
	wire	[                32-1:0]	mw_mov_dout_y;
	
	mw_mover #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)

	)
	mw_mover_inst
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(mw_mov_ena),
		.rdy		(mw_mov_rdy),
		
		.x_addr	(mw_mov_addr_x),
		.y_addr	(mw_mov_addr_y),
		.y_wren	(mw_mov_wren_y),
		
		.x_din	(mw_mov_din_x),
		.y_dout	(mw_mov_dout_y)
	);
	
	
		//
		// ROMs
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	brom_one_addr;
	//reg	[WORD_COUNTER_WIDTH-1:0]	brom_zero_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	brom_delta_addr;
	//reg	[WORD_COUNTER_WIDTH-1:0]	brom_g_x_addr;
	//reg	[WORD_COUNTER_WIDTH-1:0]	brom_g_y_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	brom_h_x_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	brom_h_y_addr;

	wire	[                32-1:0]	brom_one_dout;
	wire	[                32-1:0]	brom_zero_dout;
	wire	[                32-1:0]	brom_delta_dout;
	//wire	[                32-1:0]	brom_g_x_dout;
	//wire	[                32-1:0]	brom_g_y_dout;
	wire	[                32-1:0]	brom_h_x_dout;
	wire	[                32-1:0]	brom_h_y_dout;
	
	(* ROM_STYLE="BLOCK" *) brom_p256_one brom_one_inst
		(.clk(clk), .b_addr(brom_one_addr), .b_out(brom_one_dout));
		
	brom_p256_zero brom_zero_inst
		(.b_out(brom_zero_dout));
		
	(* ROM_STYLE="BLOCK" *) brom_p256_delta brom_delta_inst
		(.clk(clk), .b_addr(brom_delta_addr), .b_out(brom_delta_dout));
	
        /* XXX NEW
	(* ROM_STYLE="BLOCK" *) brom_p256_g_x brom_g_x_inst
		(.clk(clk), .b_addr(brom_g_x_addr), .b_out(brom_g_x_dout));

	(* ROM_STYLE="BLOCK" *) brom_p256_g_y brom_g_y_inst
		(.clk(clk), .b_addr(brom_g_y_addr), .b_out(brom_g_y_dout));
        */
		
	(* ROM_STYLE="BLOCK" *) brom_p256_h_x brom_h_x_inst
		(.clk(clk), .b_addr(brom_h_x_addr), .b_out(brom_h_x_dout));

	(* ROM_STYLE="BLOCK" *) brom_p256_h_y brom_h_y_inst
		(.clk(clk), .b_addr(brom_h_y_addr), .b_out(brom_h_y_dout));

	
		//
		// Temporary Variables
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t1_wr_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t2_wr_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t3_wr_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t4_wr_addr;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t1_rd_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t2_rd_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t3_rd_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	bram_t4_rd_addr;
	
	wire										bram_t1_wr_en;
	wire										bram_t2_wr_en;
	wire										bram_t3_wr_en;
	wire										bram_t4_wr_en;
	
	wire	[                32-1:0]	bram_t1_wr_data;
	wire	[                32-1:0]	bram_t2_wr_data;
	wire	[                32-1:0]	bram_t3_wr_data;
	wire	[                32-1:0]	bram_t4_wr_data;
	
	wire	[                32-1:0]	bram_t1_rd_data;
	wire	[                32-1:0]	bram_t2_rd_data;
	wire	[                32-1:0]	bram_t3_rd_data;
	wire	[                32-1:0]	bram_t4_rd_data;
		
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH)
	)
	bram_t1
	(	.clk		(clk),
		.a_addr(bram_t1_wr_addr), .a_wr(bram_t1_wr_en), .a_in(bram_t1_wr_data), .a_out(),
		.b_addr(bram_t1_rd_addr),                                               .b_out(bram_t1_rd_data)
	);
	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH)
	)
	bram_t2
	(	.clk		(clk),
		.a_addr(bram_t2_wr_addr), .a_wr(bram_t2_wr_en), .a_in(bram_t2_wr_data), .a_out(),
		.b_addr(bram_t2_rd_addr),                                               .b_out(bram_t2_rd_data)
	);
	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH)
	)
	bram_t3
	(	.clk		(clk),
		.a_addr(bram_t3_wr_addr), .a_wr(bram_t3_wr_en), .a_in(bram_t3_wr_data), .a_out(),
		.b_addr(bram_t3_rd_addr),                                               .b_out(bram_t3_rd_data)
	);
	
	bram_1rw_1ro_readfirst #
	(	.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH)
	)
	bram_t4
	(	.clk		(clk),
		.a_addr(bram_t4_wr_addr), .a_wr(bram_t4_wr_en), .a_in(bram_t4_wr_data), .a_out(),
		.b_addr(bram_t4_rd_addr),                                               .b_out(bram_t4_rd_data)
	);
	

		//
		// uOP Trigger Logic
		//
	reg	uop_trig;
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)	uop_trig <= 1'b0;
		else						uop_trig <= (fsm_state == FSM_STATE_FETCH) ? 1'b1 : 1'b0;
		

		//
		// FSM
		//
	localparam	[ 1: 0]	FSM_STATE_STALL	= 2'b00;
	localparam	[ 1: 0]	FSM_STATE_FETCH	= 2'b01;
	localparam	[ 1: 0]	FSM_STATE_EXECUTE	= 2'b10;
	
	reg	[ 1: 0]	fsm_state		= FSM_STATE_STALL;
	wire	[ 1: 0]	fsm_state_next	= (uop_opcode == OPCODE_RDY) ? FSM_STATE_STALL : FSM_STATE_FETCH;
	
	
		//
		// FSM Transition Logic
		//
	reg	uop_done;
	
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0)		fsm_state <= FSM_STATE_STALL;
		else case (fsm_state)
			FSM_STATE_STALL:		fsm_state <= ena ? FSM_STATE_FETCH : FSM_STATE_STALL;
			FSM_STATE_FETCH:		fsm_state <= FSM_STATE_EXECUTE;
			FSM_STATE_EXECUTE:	fsm_state <= (!uop_trig && uop_done) ? fsm_state_next : FSM_STATE_EXECUTE;
			default:					fsm_state <= FSM_STATE_STALL;
		endcase
	
	
		//
		// uOP Address Increment Logic
		//
	always @(posedge clk)
		//
		if (fsm_state == FSM_STATE_STALL)
			uop_addr <= 5'd0;
		else if (fsm_state == FSM_STATE_EXECUTE)
			if (!uop_trig && uop_done)
				uop_addr <= (uop_opcode == OPCODE_RDY) ? 5'd0 : uop_addr + 1'b1;
		
	
		//
		// uOP Completion Logic
		//
	always @(*)
		//
		case (uop_opcode)
			OPCODE_CMP:	uop_done = mw_cmp_rdy;
			OPCODE_MOV:	uop_done = mw_mov_rdy;
			OPCODE_ADD:	uop_done = mod_add_rdy;
			OPCODE_SUB:	uop_done = mod_sub_rdy;
			OPCODE_MUL:	uop_done = mod_mul_rdy;
			OPCODE_RDY:	uop_done = 1'b1;
			default:		uop_done = 1'b0;
		endcase
	

		//
		// Helper Modules Enable Logic
		//
	assign mw_cmp_ena		= uop_opcode[0] & uop_trig;
	assign mw_mov_ena		= uop_opcode[1] & uop_trig;
	assign mod_add_ena	= uop_opcode[2] & uop_trig;
	assign mod_sub_ena	= uop_opcode[3] & uop_trig;
	assign mod_mul_ena	= uop_opcode[4] & uop_trig;
	
	
		//
		// uOP Source Value Decoding Logic
		//
	reg	[31: 0]	uop_src_a_value;
	
	always @(*)
		//
		case (uop_src_a)
			UOP_SRC_PX:		uop_src_a_value = px_din;
			UOP_SRC_PY:		uop_src_a_value = py_din;
			UOP_SRC_PZ:		uop_src_a_value = pz_din;
			
			UOP_SRC_RX:		uop_src_a_value = rx_din;
			UOP_SRC_RY:		uop_src_a_value = ry_din;
			UOP_SRC_RZ:		uop_src_a_value = rz_din;
			
			UOP_SRC_T1:		uop_src_a_value = bram_t1_rd_data;
			UOP_SRC_T2:		uop_src_a_value = bram_t2_rd_data;
			UOP_SRC_T3:		uop_src_a_value = bram_t3_rd_data;
			UOP_SRC_T4:		uop_src_a_value = bram_t4_rd_data;
			
			UOP_SRC_ONE:	uop_src_a_value = brom_one_dout;
			UOP_SRC_ZERO:	uop_src_a_value = brom_zero_dout;
			UOP_SRC_DELTA:	uop_src_a_value = brom_delta_dout;
			
			UOP_SRC_G_X:	uop_src_a_value = brom_g_x_dout;
			UOP_SRC_G_Y:	uop_src_a_value = brom_g_y_dout;

			UOP_SRC_H_X:	uop_src_a_value = brom_h_x_dout;
			UOP_SRC_H_Y:	uop_src_a_value = brom_h_y_dout;
			
			UOP_SRC_V:		uop_src_a_value = v_din;
			
			default:			uop_src_a_value = {32{1'bX}};
		endcase

		
	assign mw_cmp_din_x  = uop_src_a_value;
	assign mw_mov_din_x  = uop_src_a_value;
	assign mod_add_din_a = uop_src_a_value;
	assign mod_sub_din_a = uop_src_a_value;
	assign mod_mul_din_a = uop_src_a_value;
	
	reg	[31: 0]	uop_src_b_value;
		
	always @(*)
		//
		case (uop_src_b)
			UOP_SRC_PX:		uop_src_b_value = px_din;
			UOP_SRC_PY:		uop_src_b_value = py_din;
			UOP_SRC_PZ:		uop_src_b_value = pz_din;
			
			UOP_SRC_RX:		uop_src_b_value = rx_din;
			UOP_SRC_RY:		uop_src_b_value = ry_din;
			UOP_SRC_RZ:		uop_src_b_value = rz_din;
			
			UOP_SRC_T1:		uop_src_b_value = bram_t1_rd_data;
			UOP_SRC_T2:		uop_src_b_value = bram_t2_rd_data;
			UOP_SRC_T3:		uop_src_b_value = bram_t3_rd_data;
			UOP_SRC_T4:		uop_src_b_value = bram_t4_rd_data;
			
			UOP_SRC_ONE:	uop_src_b_value = brom_one_dout;
			UOP_SRC_ZERO:	uop_src_b_value = brom_zero_dout;
			UOP_SRC_DELTA:	uop_src_b_value = brom_delta_dout;

			UOP_SRC_G_X:	uop_src_b_value = brom_g_x_dout;
			UOP_SRC_G_Y:	uop_src_b_value = brom_g_y_dout;

			UOP_SRC_H_X:	uop_src_b_value = brom_h_x_dout;
			UOP_SRC_H_Y:	uop_src_b_value = brom_h_y_dout;
			
			UOP_SRC_V:		uop_src_b_value = v_din;
			
			default:			uop_src_b_value = {32{1'bX}};
		endcase
	
	assign mw_cmp_din_y  = uop_src_b_value;
	assign mod_add_din_b = uop_src_b_value;
	assign mod_sub_din_b = uop_src_b_value;
	assign mod_mul_din_b = uop_src_b_value;
	
	
		//
		// uOP Source & Destination Address Decoding Logic
		//
	reg	[WORD_COUNTER_WIDTH-1:0]	uop_src_a_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	uop_src_b_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	uop_dst_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	uop_q_addr;
	
	assign q_addr = uop_q_addr;
	
	always @(*)
		//
		case (uop_opcode)
			//
			OPCODE_CMP:	begin
				uop_src_a_addr = mw_cmp_addr_xy;
				uop_src_b_addr = mw_cmp_addr_xy;
				uop_dst_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				uop_q_addr		= {WORD_COUNTER_WIDTH{1'bX}};
			end
			//
			OPCODE_MOV:	begin
				uop_src_a_addr = mw_mov_addr_x;
				uop_src_b_addr = {WORD_COUNTER_WIDTH{1'bX}};
				uop_dst_addr	= mw_mov_addr_y;
				uop_q_addr		= {WORD_COUNTER_WIDTH{1'bX}};
			end
			//
			OPCODE_ADD:	begin
				uop_src_a_addr = mod_add_addr_ab;
				uop_src_b_addr = mod_add_addr_ab;
				uop_dst_addr	= mod_add_addr_s;
				uop_q_addr		= mod_add_addr_n;
			end
			//
			OPCODE_SUB:	begin
				uop_src_a_addr = mod_sub_addr_ab;
				uop_src_b_addr = mod_sub_addr_ab;
				uop_dst_addr	= mod_sub_addr_d;
				uop_q_addr		= mod_sub_addr_n;
			end
			//
			OPCODE_MUL:	begin
				uop_src_a_addr = mod_mul_addr_a;
				uop_src_b_addr = mod_mul_addr_b;
				uop_dst_addr	= mod_mul_addr_p;
				uop_q_addr		= mod_mul_addr_n;
			end
			//
			default: begin
				uop_src_a_addr = {WORD_COUNTER_WIDTH{1'bX}};
				uop_src_b_addr = {WORD_COUNTER_WIDTH{1'bX}};
				uop_dst_addr	= {WORD_COUNTER_WIDTH{1'bX}};
				uop_q_addr		= {WORD_COUNTER_WIDTH{1'bX}};
			end
			//
		endcase
	
	
		//
		// uOP Conditional Execution Logic
		//
	reg	uop_exec_effective;

	always @(*)
		//
		case (uop_exec)
			UOP_EXEC_ALWAYS:		uop_exec_effective = 1'b1;
			UOP_EXEC_PZT1T2_0XX:	uop_exec_effective =  flag_pz_is_zero;
			UOP_EXEC_PZT1T2_100:	uop_exec_effective = !flag_pz_is_zero && flag_t1_is_zero &&  flag_t2_is_zero;
			UOP_EXEC_PZT1T2_101:	uop_exec_effective = !flag_pz_is_zero && flag_t1_is_zero && !flag_t2_is_zero;
		endcase


		//
		// uOP Destination Store Logic
		//
	reg	uop_dst_wren;
	
	always @(*)
		//
		case (uop_opcode)
			//
			OPCODE_MOV:	uop_dst_wren = mw_mov_wren_y & uop_exec_effective;
			OPCODE_ADD:	uop_dst_wren = mod_add_wren_s;
			OPCODE_SUB:	uop_dst_wren = mod_sub_wren_d;
			OPCODE_MUL:	uop_dst_wren = mod_mul_wren_p;
			default:		uop_dst_wren = 1'b0;
			//
		endcase
		
	
	always @(*) begin
		//
		//
		//
		if      (uop_src_a == UOP_SRC_PX) px_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_PX) px_addr = uop_src_b_addr;
		else                              px_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_PY) py_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_PY) py_addr = uop_src_b_addr;
		else                              py_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_PZ) pz_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_PZ) pz_addr = uop_src_b_addr;
		else                              pz_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if      (uop_src_a == UOP_SRC_ONE)   brom_one_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_ONE)   brom_one_addr = uop_src_b_addr;
		else                                 brom_one_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//if      (uop_src_a == UOP_SRC_ZERO)  brom_zero_addr = uop_src_a_addr;
		//else if (uop_src_b == UOP_SRC_ZERO)  brom_zero_addr = uop_src_b_addr;
		//else                                 brom_zero_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_DELTA) brom_delta_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_DELTA) brom_delta_addr = uop_src_b_addr;
		else                                 brom_delta_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if      (uop_src_a == UOP_SRC_G_X) brom_g_x_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_G_X) brom_g_x_addr = uop_src_b_addr;
		else                               brom_g_x_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_G_Y) brom_g_y_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_G_Y) brom_g_y_addr = uop_src_b_addr;
		else                               brom_g_y_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if      (uop_src_a == UOP_SRC_H_X) brom_h_x_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_H_X) brom_h_x_addr = uop_src_b_addr;
		else                               brom_h_x_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_H_Y) brom_h_y_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_H_Y) brom_h_y_addr = uop_src_b_addr;
		else                               brom_h_y_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if      (uop_src_a == UOP_SRC_V) v_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_V) v_addr = uop_src_b_addr;
		else                             v_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if      (uop_src_a == UOP_SRC_T1) bram_t1_rd_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_T1) bram_t1_rd_addr = uop_src_b_addr;
		else                              bram_t1_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_T2) bram_t2_rd_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_T2) bram_t2_rd_addr = uop_src_b_addr;
		else                              bram_t2_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_T3) bram_t3_rd_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_T3) bram_t3_rd_addr = uop_src_b_addr;
		else                              bram_t3_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if      (uop_src_a == UOP_SRC_T4) bram_t4_rd_addr = uop_src_a_addr;
		else if (uop_src_b == UOP_SRC_T4) bram_t4_rd_addr = uop_src_b_addr;
		else                              bram_t4_rd_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if (uop_dst == UOP_DST_T1) bram_t1_wr_addr = uop_dst_addr;
		else                       bram_t1_wr_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if (uop_dst == UOP_DST_T2) bram_t2_wr_addr = uop_dst_addr;
		else                       bram_t2_wr_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if (uop_dst == UOP_DST_T3) bram_t3_wr_addr = uop_dst_addr;
		else                       bram_t3_wr_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		if (uop_dst == UOP_DST_T4) bram_t4_wr_addr = uop_dst_addr;
		else                       bram_t4_wr_addr = {WORD_COUNTER_WIDTH{1'bX}};
		//
		//
		//
		if ((uop_dst == UOP_DST_RX) && (uop_dst_wren))	rx_addr = uop_dst_addr;
		else begin
			if      (uop_src_a == UOP_SRC_RX) 				rx_addr = uop_src_a_addr;
			else if (uop_src_b == UOP_SRC_RX) 				rx_addr = uop_src_b_addr;
			else                              				rx_addr = {WORD_COUNTER_WIDTH{1'bX}};
		end
		//
		if ((uop_dst == UOP_DST_RY) && (uop_dst_wren))	ry_addr = uop_dst_addr;
		else begin
			if      (uop_src_a == UOP_SRC_RY) 				ry_addr = uop_src_a_addr;
			else if (uop_src_b == UOP_SRC_RY) 				ry_addr = uop_src_b_addr;
			else                              				ry_addr = {WORD_COUNTER_WIDTH{1'bX}};
		end
		//
		if ((uop_dst == UOP_DST_RZ) && (uop_dst_wren))	rz_addr = uop_dst_addr;
		else begin
			if      (uop_src_a == UOP_SRC_RZ) 				rz_addr = uop_src_a_addr;
			else if (uop_src_b == UOP_SRC_RZ) 				rz_addr = uop_src_b_addr;
			else                              				rz_addr = {WORD_COUNTER_WIDTH{1'bX}};
		end
		//
	end
	
	
	assign rx_wren = uop_dst_wren && (uop_dst == UOP_DST_RX);
	assign ry_wren = uop_dst_wren && (uop_dst == UOP_DST_RY);
	assign rz_wren = uop_dst_wren && (uop_dst == UOP_DST_RZ);
	
	assign bram_t1_wr_en = uop_dst_wren && (uop_dst == UOP_DST_T1);
	assign bram_t2_wr_en = uop_dst_wren && (uop_dst == UOP_DST_T2);
	assign bram_t3_wr_en = uop_dst_wren && (uop_dst == UOP_DST_T3);
	assign bram_t4_wr_en = uop_dst_wren && (uop_dst == UOP_DST_T4);
	
	
	
		//
		// Destination Value Selector
		//
	reg	[31: 0]	uop_dst_value;
	
	always @(*)
		//
		case (uop_opcode)
		
			OPCODE_MOV:	uop_dst_value = mw_mov_dout_y;
			OPCODE_ADD:	uop_dst_value = mod_add_dout_s;
			OPCODE_SUB:	uop_dst_value = mod_sub_dout_d;
			OPCODE_MUL:	uop_dst_value = mod_mul_dout_p;
			
			default:		uop_dst_value = {32{1'bX}};
			
		endcase
	
	assign rx_dout = uop_dst_value;
	assign ry_dout = uop_dst_value;
	assign rz_dout = uop_dst_value;
	
	assign bram_t1_wr_data = uop_dst_value;
	assign bram_t2_wr_data = uop_dst_value;
	assign bram_t3_wr_data = uop_dst_value;
	assign bram_t4_wr_data = uop_dst_value;


		//
		// Latch Comparison Flags
		//
	always @(posedge clk)
		//
		if (	(fsm_state  == FSM_STATE_EXECUTE) &&
				(uop_opcode == OPCODE_CMP)        &&
				(uop_done && !uop_trig) ) begin
			
			if ( (uop_src_a == UOP_SRC_PZ) && (uop_src_b == UOP_SRC_ZERO) )
				flag_pz_is_zero <= !mw_cmp_out_l && mw_cmp_out_e && !mw_cmp_out_g;
				
			if ( (uop_src_a == UOP_SRC_T1) && (uop_src_b == UOP_SRC_ZERO) )
				flag_t1_is_zero <= !mw_cmp_out_l && mw_cmp_out_e && !mw_cmp_out_g;
				
			if ( (uop_src_a == UOP_SRC_T2) && (uop_src_b == UOP_SRC_ZERO) )
				flag_t2_is_zero <= !mw_cmp_out_l && mw_cmp_out_e && !mw_cmp_out_g;
				
		end

	
		//
		// Ready Flag Logic
		//
	reg rdy_reg = 1'b1;
	assign rdy = rdy_reg;
		
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0) rdy_reg <= 1'b1;
		else begin
			
				/* clear flag */
			if (fsm_state == FSM_STATE_STALL)
				if (ena) rdy_reg <= 1'b0;
			
				/* set flag */
			if ((fsm_state == FSM_STATE_EXECUTE) && !uop_trig && uop_done)
				if (uop_opcode == OPCODE_RDY) rdy_reg <= 1'b1;
				
		end
			
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
