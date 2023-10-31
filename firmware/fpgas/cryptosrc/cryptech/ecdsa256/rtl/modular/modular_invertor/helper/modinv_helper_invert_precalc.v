`timescale 1ns / 1ps

module modinv_helper_invert_precalc
	(
		clk, rst_n,
		ena, rdy,
		
		r_addr, r_din,
		s_addr, s_din,
		u_addr, u_din,
		v_addr, v_din,
		
		r_dbl_addr,          r_dbl_wren,          r_dbl_dout,
		s_dbl_addr,          s_dbl_wren,          s_dbl_dout,
		r_plus_s_addr,       r_plus_s_wren,       r_plus_s_dout,
		u_half_addr,         u_half_wren,         u_half_dout,
		v_half_addr,         v_half_wren,         v_half_dout,
		u_minus_v_addr,      u_minus_v_wren,      u_minus_v_dout,      u_minus_v_din,
		v_minus_u_addr,      v_minus_u_wren,      v_minus_u_dout,      v_minus_u_din,
		u_minus_v_half_addr, u_minus_v_half_wren, u_minus_v_half_dout,
		v_minus_u_half_addr, v_minus_u_half_wren, v_minus_u_half_dout
	);
	

		//
		// Parameters
		//
	parameter BUFFER_NUM_WORDS		= 9;
	parameter BUFFER_ADDR_BITS		= 4;
	
	
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

	output	wire	[BUFFER_ADDR_BITS-1:0]	r_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_addr;
		
	input		wire	[              32-1:0]	r_din;
	input		wire	[              32-1:0]	s_din;
	input		wire	[              32-1:0]	u_din;
	input		wire	[              32-1:0]	v_din;
		
	output	wire	[BUFFER_ADDR_BITS-1:0]	r_dbl_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	s_dbl_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	r_plus_s_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_minus_v_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_minus_u_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_minus_v_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_minus_u_half_addr;
		
	output	wire	[              32-1:0]	r_dbl_dout;
	output	wire	[              32-1:0]	s_dbl_dout;
	output	wire	[              32-1:0]	r_plus_s_dout;
	output	wire	[              32-1:0]	u_half_dout;
	output	wire	[              32-1:0]	v_half_dout;
	output	wire	[              32-1:0]	u_minus_v_dout;
	output	wire	[              32-1:0]	v_minus_u_dout;
	output	wire	[              32-1:0]	u_minus_v_half_dout;
	output	wire	[              32-1:0]	v_minus_u_half_dout;
		
	output	wire									r_dbl_wren;
	output	wire									s_dbl_wren;
	output	wire									r_plus_s_wren;
	output	wire									u_half_wren;
	output	wire									v_half_wren;
	output	wire									u_minus_v_wren;
	output	wire									v_minus_u_wren;
	output	wire									u_minus_v_half_wren;
	output	wire									v_minus_u_half_wren;
	
	input		wire	[              32-1:0]	u_minus_v_din;
	input		wire	[              32-1:0]	v_minus_u_din;
	


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
	reg	[BUFFER_ADDR_BITS-1:0]	addr_in;

	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_next	= (addr_in < addr_in_last) ?
																		addr_in + 1'b1 : addr_in_zero;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_prev	= (addr_in > addr_in_zero) ?
																		addr_in - 1'b1 : addr_in_zero;
																		
	reg	[BUFFER_ADDR_BITS-1:0]	addr_out1;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out1_next	= (addr_out1 < addr_out1_last) ?
																		addr_out1 + 1'b1 : addr_out1_zero;
																		
	reg	[BUFFER_ADDR_BITS-1:0]	addr_out2;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_next	= (addr_out2 < addr_out2_last) ?
																		addr_out2 + 1'b1 : addr_out2_zero;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out2_prev	= (addr_out2 > addr_out2_zero) ?
																		addr_out2 - 1'b1 : addr_out2_zero;
																		
	reg	[BUFFER_ADDR_BITS-1:0]	addr_out3;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out3_prev	= (addr_out3 > addr_out3_zero) ?
																		addr_out3 - 1'b1 : addr_out3_last;

	reg	[BUFFER_ADDR_BITS-1:0]	addr_out4;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out4_last	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out4_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out4_prev	= (addr_out4 > addr_out4_zero) ?
																		addr_out4 - 1'b1 : addr_out4_last;

	
	assign r_addr					= addr_in;
	assign s_addr					= addr_in;
	assign u_addr					= addr_in;
	assign v_addr					= addr_in;
		
	assign r_dbl_addr				= addr_out1;
	assign s_dbl_addr				= addr_out1;
	assign r_plus_s_addr			= addr_out2;
	assign u_half_addr			= addr_out3;
	assign v_half_addr			= addr_out3;
	assign u_minus_v_addr		= addr_out2;
	assign v_minus_u_addr		= addr_out2;
	assign u_minus_v_half_addr	= addr_out4;
	assign v_minus_u_half_addr	= addr_out4;
	
		
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
	
	
		//
		// Address Increment/Decrement Logic
		//
	wire	inc_addr_in;
	wire	dec_addr_in;
	wire	inc_addr_out1;
	wire	inc_addr_out2;
	wire	dec_addr_out2;
	wire	dec_addr_out3;
	wire	dec_addr_out4;

	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_in_start	= 0 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_in_stop		= 1 * BUFFER_NUM_WORDS - 1;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out1_start	= 0 * BUFFER_NUM_WORDS + 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out1_stop	= 1 * BUFFER_NUM_WORDS + 1;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out2_start	= 0 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out2_stop	= 1 * BUFFER_NUM_WORDS + 1;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out2_start	= 1 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out2_stop	= 2 * BUFFER_NUM_WORDS + 1;	

	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_in_start	= 1 * BUFFER_NUM_WORDS + 0;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_in_stop		= 2 * BUFFER_NUM_WORDS - 2;	
	
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out3_start	= 1 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out3_stop	= 2 * BUFFER_NUM_WORDS + 0;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out4_start	= 1 * BUFFER_NUM_WORDS + 4;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_out4_stop	= 2 * BUFFER_NUM_WORDS + 3;	

	assign inc_addr_in   = (proc_cnt >= cnt_inc_addr_in_start)   && (proc_cnt <= cnt_inc_addr_in_stop);
	assign dec_addr_in   = (proc_cnt >= cnt_dec_addr_in_start)   && (proc_cnt <= cnt_dec_addr_in_stop);
	assign inc_addr_out1 = (proc_cnt >= cnt_inc_addr_out1_start) && (proc_cnt <= cnt_inc_addr_out1_stop);
	assign inc_addr_out2 = (proc_cnt >= cnt_inc_addr_out2_start) && (proc_cnt <= cnt_inc_addr_out2_stop);
	assign dec_addr_out2 = (proc_cnt >= cnt_dec_addr_out2_start) && (proc_cnt <= cnt_dec_addr_out2_stop);
	assign dec_addr_out3 = (proc_cnt >= cnt_dec_addr_out3_start) && (proc_cnt <= cnt_dec_addr_out3_stop);
	assign dec_addr_out4 = (proc_cnt >= cnt_dec_addr_out4_start) && (proc_cnt <= cnt_dec_addr_out4_stop);
	
	
	always @(posedge clk) begin
		//
		if (rdy) begin
			//
			addr_in 		<= addr_in_zero;
			addr_out1	<= addr_out1_zero;
			addr_out2	<= addr_out2_zero;
			addr_out3	<= addr_out3_last;
			addr_out4	<= addr_out4_last;
			//
		end else begin
			//
			if (inc_addr_in)				addr_in <= addr_in_next;
			else if (dec_addr_in)		addr_in <= addr_in_prev;
			//
			if (inc_addr_out1)			addr_out1 <= addr_out1_next;
			else								addr_out1 <= addr_out1_zero;
			//
			if (inc_addr_out2)			addr_out2 <= addr_out2_next;
			else if (dec_addr_out2)		addr_out2 <= addr_out2_prev;
			//
			if (dec_addr_out3)			addr_out3 <= addr_out3_prev;
			else								addr_out3 <= addr_out3_last;
			//
			if (dec_addr_out4)			addr_out4 <= addr_out4_prev;
			else								addr_out4 <= addr_out4_last;
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
	wire	wren_out4;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out1_start	= 0 * BUFFER_NUM_WORDS + 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out1_stop	= 1 * BUFFER_NUM_WORDS + 1;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out2_start	= 0 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out2_stop	= 1 * BUFFER_NUM_WORDS + 2;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out3_start	= 1 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out3_stop	= 2 * BUFFER_NUM_WORDS + 0;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out4_start	= 1 * BUFFER_NUM_WORDS + 4;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out4_stop	= 2 * BUFFER_NUM_WORDS + 3;

	assign wren_out1 = (proc_cnt >= cnt_wren_out1_start) && (proc_cnt <= cnt_wren_out1_stop);
	assign wren_out2 = (proc_cnt >= cnt_wren_out2_start) && (proc_cnt <= cnt_wren_out2_stop);
	assign wren_out3 = (proc_cnt >= cnt_wren_out3_start) && (proc_cnt <= cnt_wren_out3_stop);
	assign wren_out4 = (proc_cnt >= cnt_wren_out4_start) && (proc_cnt <= cnt_wren_out4_stop);

	assign r_dbl_wren				= wren_out1;
	assign s_dbl_wren				= wren_out1;
	assign r_plus_s_wren			= wren_out2;
	assign u_half_wren			= wren_out3;
	assign v_half_wren			= wren_out3;
	assign u_minus_v_wren		= wren_out2;
	assign v_minus_u_wren		= wren_out2;
	assign u_minus_v_half_wren	= wren_out4;
	assign v_minus_u_half_wren	= wren_out4;


		//
		// Adder (r + s)
		//
	wire	[31: 0]	add32_r_plus_s_sum_out;
	wire				add32_r_plus_s_carry_in;
	wire				add32_r_plus_s_carry_out;
	
	adder32_wrapper add32_r_plus_s
	(
		.clk		(clk),
		.a			(r_din),
		.b			(s_din),
		.s			(add32_r_plus_s_sum_out),
		.c_in		(add32_r_plus_s_carry_in),
		.c_out	(add32_r_plus_s_carry_out)
	);
	
		//
		// Subtractor (u - v)
		//
	wire	[31: 0]	sub32_u_minus_v_difference_out;
	wire				sub32_u_minus_v_borrow_in;
	wire				sub32_u_minus_v_borrow_out;
	
	subtractor32_wrapper sub32_u_minus_v
	(
		.clk		(clk),
		.a			(u_din),
		.b			(v_din),
		.d			(sub32_u_minus_v_difference_out),
		.b_in		(sub32_u_minus_v_borrow_in),
		.b_out	(sub32_u_minus_v_borrow_out)
	);
	
		//
		// Subtractor (v - u)
		//
	wire	[31: 0]	sub32_v_minus_u_difference_out;
	wire				sub32_v_minus_u_borrow_in;
	wire				sub32_v_minus_u_borrow_out;
	
	subtractor32_wrapper sub32_v_minus_u
	(
		.clk		(clk),
		.a			(v_din),
		.b			(u_din),
		.d			(sub32_v_minus_u_difference_out),
		.b_in		(sub32_v_minus_u_borrow_in),
		.b_out	(sub32_v_minus_u_borrow_out)
	);
	
	
		//
		// Carry & Borrow Masking Logic
		//
	reg	mask_carry_borrow;
	
	always @(posedge clk)
		//
		mask_carry_borrow <= ((proc_cnt >= cnt_wren_out1_start) && (proc_cnt < cnt_wren_out1_stop)) ?
			1'b0 : 1'b1;
		
	assign add32_r_plus_s_carry_in   = add32_r_plus_s_carry_out   & ~mask_carry_borrow;
	assign sub32_u_minus_v_borrow_in = sub32_u_minus_v_borrow_out & ~mask_carry_borrow;
	assign sub32_v_minus_u_borrow_in = sub32_v_minus_u_borrow_out & ~mask_carry_borrow;
	
	
		//
		// Carry Bits
		//
	reg	r_dbl_carry;
	reg	s_dbl_carry;
	reg	u_half_carry;
	reg	v_half_carry;
	reg	u_minus_v_half_carry;
	reg	v_minus_u_half_carry;
	
	always @(posedge clk) begin
		
		r_dbl_carry					<= ((proc_cnt >= cnt_wren_out1_start) && (proc_cnt < cnt_wren_out1_stop)) ?
											r_din[31] : 1'b0;
								
		s_dbl_carry					<= ((proc_cnt >= cnt_wren_out1_start) && (proc_cnt < cnt_wren_out1_stop)) ?
											s_din[31] : 1'b0;
								
		u_half_carry				<= ((proc_cnt >= cnt_wren_out3_start) && (proc_cnt < cnt_wren_out3_stop)) ?
											u_din[0] : 1'b0;
		
		v_half_carry				<= ((proc_cnt >= cnt_wren_out3_start) && (proc_cnt < cnt_wren_out3_stop)) ?
											v_din[0] : 1'b0;
									
		u_minus_v_half_carry		<= ((proc_cnt >= cnt_wren_out4_start) && (proc_cnt < cnt_wren_out4_stop)) ?
											u_minus_v_din[0] : 1'b0;
		
		v_minus_u_half_carry		<= ((proc_cnt >= cnt_wren_out4_start) && (proc_cnt < cnt_wren_out4_stop)) ?
											v_minus_u_din[0] : 1'b0;

	end
	
	
		//
		// Data Mapper
		//
	assign r_dbl_dout				= {r_din[30:0], r_dbl_carry};
	assign s_dbl_dout				= {s_din[30:0], s_dbl_carry};
	assign r_plus_s_dout			= add32_r_plus_s_sum_out;
	assign u_half_dout			= {u_half_carry, u_din[31:1]};
	assign v_half_dout			= {v_half_carry, v_din[31:1]};
	assign u_minus_v_dout		= sub32_u_minus_v_difference_out;
	assign v_minus_u_dout		= sub32_v_minus_u_difference_out;
	assign u_minus_v_half_dout	= {u_minus_v_half_carry, u_minus_v_din[31:1]};
	assign v_minus_u_half_dout	= {v_minus_u_half_carry, v_minus_u_din[31:1]};
	
	
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


endmodule
