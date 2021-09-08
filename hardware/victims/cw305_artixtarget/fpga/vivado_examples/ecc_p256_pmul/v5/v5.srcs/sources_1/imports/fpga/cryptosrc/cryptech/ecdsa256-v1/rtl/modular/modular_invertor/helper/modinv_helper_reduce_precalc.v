`timescale 1ns / 1ps

module modinv_helper_reduce_precalc
	(
		clk, rst_n,
		ena, rdy,
		
		k,
		
		s_is_odd, k_is_nul,
		
		r_addr, r_din, r_wren, r_dout,
		s_addr, s_din,
		u_addr,        u_wren, u_dout,
		v_addr,        v_wren, v_dout,
		q_addr, q_din
	);
	

		//
		// Parameters
		//
	parameter OPERAND_NUM_WORDS	= 8;
	parameter OPERAND_ADDR_BITS	= 3;
	parameter BUFFER_NUM_WORDS		= 9;
	parameter BUFFER_ADDR_BITS		= 4;
	parameter K_NUM_BITS				= 10;
	
	
		//
		// clog2
		//
`include "modinv_clog2.v"
	
	
		//
		// Constants
		//
	localparam PROC_NUM_CYCLES	= 2 * BUFFER_NUM_WORDS + 4;
	localparam PROC_CNT_BITS	= clog2(PROC_NUM_CYCLES);
	
	
		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	input		wire									ena;
	output	wire									rdy;

	input		wire	[       K_NUM_BITS-1:0]	k;
		
	output	wire									s_is_odd;
	output	wire									k_is_nul;

	output	wire	[ BUFFER_ADDR_BITS-1:0]	r_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	v_addr;
	output	wire	[OPERAND_ADDR_BITS-1:0]	q_addr;

	input		wire	[              32-1:0]	r_din;
	input		wire	[              32-1:0]	s_din;
	input		wire	[              32-1:0]	q_din;
	
	output	wire									r_wren;
	output	wire									u_wren;
	output	wire									v_wren;
	
	output	wire	[              32-1:0]	r_dout;
	output	wire	[              32-1:0]	u_dout;
	output	wire	[              32-1:0]	v_dout;
				

		//
		// Counter
		//
	reg	[PROC_CNT_BITS-1:0]	proc_cnt;

	wire	[PROC_CNT_BITS-1:0]	proc_cnt_max	= PROC_NUM_CYCLES - 1;
	wire	[PROC_CNT_BITS-1:0]	proc_cnt_zero	= {PROC_CNT_BITS{1'b0}};
	wire	[PROC_CNT_BITS-1:0]	proc_cnt_next	= (proc_cnt < proc_cnt_max) ?
																	proc_cnt + 1'b1 : proc_cnt_zero;
	
		//
		// Addresses
		//
	reg	[ BUFFER_ADDR_BITS-1:0]	addr_in_buf;
	reg	[OPERAND_ADDR_BITS-1:0]	addr_in_op;
	reg	[ BUFFER_ADDR_BITS-1:0]	addr_out1;
	reg	[ BUFFER_ADDR_BITS-1:0]	addr_out2;
	reg	[ BUFFER_ADDR_BITS-1:0]	addr_out3;

	wire	[ BUFFER_ADDR_BITS-1:0]	addr_in_buf_last	= BUFFER_NUM_WORDS - 1;
	wire	[ BUFFER_ADDR_BITS-1:0]	addr_in_buf_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[ BUFFER_ADDR_BITS-1:0]	addr_in_buf_next	= (addr_in_buf < addr_in_buf_last) ?
																		addr_in_buf + 1'b1 : addr_in_buf_zero;
	wire	[ BUFFER_ADDR_BITS-1:0]	addr_in_buf_prev	= (addr_in_buf > addr_in_buf_zero) ?
																		addr_in_buf - 1'b1 : addr_in_buf_zero;

	wire	[OPERAND_ADDR_BITS-1:0]	addr_in_op_last	= OPERAND_NUM_WORDS - 1;
	wire	[OPERAND_ADDR_BITS-1:0]	addr_in_op_zero	= {OPERAND_ADDR_BITS{1'b0}};
	wire	[OPERAND_ADDR_BITS-1:0]	addr_in_op_next	= (addr_in_op < addr_in_op_last) ?
																		addr_in_op + 1'b1 : addr_in_op_zero;
																		
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_next	= (addr_out1 < addr_out1_last) ?
																		addr_out1 + 1'b1 : addr_out1_zero;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_prev	= (addr_out1 > addr_out1_zero) ?
																		addr_out1 - 1'b1 : addr_out1_zero;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_prev	= (addr_out2 > addr_out2_zero) ?
																		addr_out2 - 1'b1 : addr_out2_last;

	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_prev	= (addr_out3 > addr_out3_zero) ?
																		addr_out3 - 1'b1 : addr_out3_last;

	
	assign s_addr = addr_in_buf;
	assign q_addr = addr_in_op;
	assign r_addr = addr_out1;
	assign u_addr = addr_out2;
	assign v_addr = addr_out3;
	
	
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
		
		
		//
		// Address Increment/Decrement Logic
		//
	wire	inc_addr_buf_in;
	wire	dec_addr_buf_in;
	wire	inc_addr_op_in;
	wire	inc_addr_out1;
	wire	dec_addr_out1;
	wire	dec_addr_out2;
	wire	dec_addr_out3;

	wire	[PROC_CNT_BITS-1:0]	cnt_calc_flags					= 0 * BUFFER_NUM_WORDS + 2;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_buf_in_start	= 0 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_buf_in_stop	= 1 * BUFFER_NUM_WORDS - 1;

	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_buf_in_start	= 1 * BUFFER_NUM_WORDS + 0;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_buf_in_stop	= 2 * BUFFER_NUM_WORDS - 2;	

	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_op_in_start	= 0 * OPERAND_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_op_in_stop		= 1 * OPERAND_NUM_WORDS + 0;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out1_start		= 0 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out1_stop		= 1 * BUFFER_NUM_WORDS + 1;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out1_start		= 1 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out1_stop		= 2 * BUFFER_NUM_WORDS + 1;	

	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out2_start		= 1 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out2_stop		= 2 * BUFFER_NUM_WORDS + 0;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out3_start		= 1 * BUFFER_NUM_WORDS + 4;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out3_stop		= 2 * BUFFER_NUM_WORDS + 3;

	assign inc_addr_buf_in = (proc_cnt >= cnt_inc_addr_buf_in_start) && (proc_cnt <= cnt_inc_addr_buf_in_stop);
	assign dec_addr_buf_in = (proc_cnt >= cnt_dec_addr_buf_in_start) && (proc_cnt <= cnt_dec_addr_buf_in_stop);
	assign inc_addr_op_in  = (proc_cnt >= cnt_inc_addr_op_in_start)  && (proc_cnt <= cnt_inc_addr_op_in_stop);
	assign inc_addr_out1   = (proc_cnt >= cnt_inc_addr_out1_start) && (proc_cnt <= cnt_inc_addr_out1_stop);
	assign dec_addr_out1   = (proc_cnt >= cnt_dec_addr_out1_start) && (proc_cnt <= cnt_dec_addr_out1_stop);
	assign dec_addr_out2   = (proc_cnt >= cnt_dec_addr_out2_start) && (proc_cnt <= cnt_dec_addr_out2_stop);
	assign dec_addr_out3   = (proc_cnt >= cnt_dec_addr_out3_start) && (proc_cnt <= cnt_dec_addr_out3_stop);

	always @(posedge clk) begin
		//
		if (rdy) begin
			//
			addr_in_buf		<= addr_in_buf_zero;
			addr_in_op		<= addr_in_op_zero;
			addr_out1		<= addr_out1_zero;
			addr_out2		<= addr_out2_last;
			addr_out3		<= addr_out3_last;
			//
		end else begin
			//
			if (inc_addr_buf_in)			addr_in_buf	<= addr_in_buf_next;
			else if (dec_addr_buf_in)	addr_in_buf	<= addr_in_buf_prev;
			//
			if (inc_addr_op_in)			addr_in_op	<= addr_in_op_next;
			else								addr_in_op	<= addr_in_op_zero;
			//
			if (inc_addr_out1)			addr_out1	<= addr_out1_next;
			else if (dec_addr_out1)		addr_out1	<= addr_out1_prev;
			//
			if (dec_addr_out2)			addr_out2	<= addr_out2_prev;
			else								addr_out2	<= addr_out2_last;
			//
			if (dec_addr_out3)			addr_out3	<= addr_out3_prev;
			else								addr_out3	<= addr_out3_last;
			//
		end
		//
	end
	
	
		//
		// Write Enable Logic
		//
	wire	wren_out1;
	wire	wren_out2;
	wire	wren_out3;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out1_start	= 0 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out1_stop	= 1 * BUFFER_NUM_WORDS + 2;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out2_start	= 1 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out2_stop	= 2 * BUFFER_NUM_WORDS + 0;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out3_start	= 1 * BUFFER_NUM_WORDS + 4;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out3_stop	= 2 * BUFFER_NUM_WORDS + 3;

	assign wren_out1 = (proc_cnt >= cnt_wren_out1_start) && (proc_cnt <= cnt_wren_out1_stop);
	assign wren_out2 = (proc_cnt >= cnt_wren_out2_start) && (proc_cnt <= cnt_wren_out2_stop);
	assign wren_out3 = (proc_cnt >= cnt_wren_out3_start) && (proc_cnt <= cnt_wren_out3_stop);

	assign r_wren = wren_out1;
	assign u_wren = wren_out2;
	assign v_wren = wren_out3;
	
		//
		// Adder (s + q)
		//
	wire	[31: 0]	q_din_masked;
	wire	[31: 0]	add32_s_plus_q_sum_out;
	wire				add32_s_plus_q_carry_in;
	wire				add32_s_plus_q_carry_out;
	
	adder32_wrapper add32_r_plus_s
	(
		.clk		(clk),
		.a			(s_din),
		.b			(q_din_masked),
		.s			(add32_s_plus_q_sum_out),
		.c_in		(add32_s_plus_q_carry_in),
		.c_out	(add32_s_plus_q_carry_out)
	);
		
		
		//
		// Carry Masking Logic
		//
	wire	mask_carry;

	assign mask_carry = ((proc_cnt >= cnt_wren_out1_start) && (proc_cnt < cnt_wren_out1_stop)) ? 1'b0 : 1'b1;


		//
		// Addend Masking Logic
		//
	reg	q_din_mask;
	
	always @(posedge clk)
		q_din_mask <= (addr_in_buf == addr_in_buf_last) ? 1'b1 : 1'b0;
	
	assign q_din_masked = q_din_mask ? {32{1'b0}} : q_din;
	
	assign add32_s_plus_q_carry_in = add32_s_plus_q_carry_out & ~mask_carry;


		//
		// Carry Bits
		//
	reg	s_half_carry;
	reg	s_plus_q_half_carry;
	
	always @(posedge clk) begin
		//					
		s_half_carry				<= ((proc_cnt >= cnt_wren_out2_start) && (proc_cnt < cnt_wren_out2_stop)) ?
											s_din[0] : 1'b0;
		//
		s_plus_q_half_carry		<= ((proc_cnt >= cnt_wren_out3_start) && (proc_cnt < cnt_wren_out3_stop)) ?
											r_din[0] : 1'b0;
		//
	end

		//
		// Data Mapper
		//
	assign r_dout = add32_s_plus_q_sum_out;
	assign u_dout = {s_half_carry,        s_din[31:1]};
	assign v_dout = {s_plus_q_half_carry, r_din[31:1]};
	
	
		//
		// Primary Counter Logic
		//
	always @(posedge clk or negedge rst_n)
		//
		if (rst_n == 1'b0) proc_cnt <= proc_cnt_zero;
		else begin
			if (!rdy)		proc_cnt <= proc_cnt_next;
			else if (ena)	proc_cnt <= proc_cnt_next;
		end
		
		
		//
		// Output Flags
		//
	reg	s_is_odd_reg;
	reg	k_is_nul_reg;
	
	assign s_is_odd = s_is_odd_reg;
	assign k_is_nul = k_is_nul_reg;

	always @(posedge clk)
		//
		if (proc_cnt == cnt_calc_flags) begin
			s_is_odd_reg <= s_din[0];
			k_is_nul_reg <= (k == {K_NUM_BITS{1'b0}}) ? 1'b1 : 1'b0;
		end


endmodule
