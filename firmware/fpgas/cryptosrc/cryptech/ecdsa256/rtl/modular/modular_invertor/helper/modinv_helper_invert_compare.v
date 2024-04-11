`timescale 1ns / 1ps

module modinv_helper_invert_compare
	(
		clk, rst_n,
		ena, rdy,
		
		u_addr, u_din,
		v_addr, v_din,
		
		u_gt_v, v_eq_1,
		u_is_even, v_is_even
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
	localparam PROC_NUM_CYCLES	= 1 * BUFFER_NUM_WORDS + 10;
	localparam PROC_CNT_BITS	= clog2(PROC_NUM_CYCLES);
	
	
		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	input		wire									ena;
	output	wire									rdy;

	output	wire	[BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_addr;
		
	input		wire	[              32-1:0]	u_din;
	input		wire	[              32-1:0]	v_din;
		
	output	wire									u_gt_v;
	output	wire									v_eq_1;
	output	wire									u_is_even;
	output	wire									v_is_even;


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
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_prev	= (addr_in > addr_in_zero) ?
																		addr_in - 1'b1 : addr_in_last;
																			
	assign u_addr					= addr_in;
	assign v_addr					= addr_in;	
	
	
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
	
	
		//
		// Address Decrement Logic
		//
	wire	dec_addr_in;

	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_in_start	= 0 * BUFFER_NUM_WORDS + 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_dec_addr_in_stop		= 1 * BUFFER_NUM_WORDS + 0;
	
	assign dec_addr_in   = (proc_cnt >= cnt_dec_addr_in_start)   && (proc_cnt <= cnt_dec_addr_in_stop);
	
	always @(posedge clk)
		//
		if (rdy)						addr_in <= addr_in_last;
		else if (dec_addr_in)	addr_in <= addr_in_prev;
	
	
		//
		// Comparison Stage Flags
		//
	wire	calc_leg;
	wire	calc_leg_final;
	wire	calc_parity;

	wire	[PROC_CNT_BITS-1:0]	cnt_calc_leg_start	= 0 * BUFFER_NUM_WORDS + 3;
	wire	[PROC_CNT_BITS-1:0]	cnt_calc_leg_stop		= 1 * BUFFER_NUM_WORDS + 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_calc_parity		= 1 * BUFFER_NUM_WORDS + 1;
	
	assign calc_leg = (proc_cnt >= cnt_calc_leg_start) && (proc_cnt <= cnt_calc_leg_stop);
	assign calc_leg_final = (proc_cnt == cnt_calc_leg_stop);
	assign calc_parity = (proc_cnt == cnt_calc_parity);

	
		//
		// Dummy Input
		//
	reg	sub32_din_1_lsb;
	wire	[31: 0]	sub32_din_1 = {{31{1'b0}}, sub32_din_1_lsb};
	
	always @(posedge clk)
		//
		sub32_din_1_lsb <= (addr_in == addr_in_zero) ? 1'b1 : 1'b0;
	
	
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
		// Subtractor (v - 1)
		//
	wire	[31: 0]	sub32_v_minus_1_difference_out;
	wire				sub32_v_minus_1_borrow_in;
	wire				sub32_v_minus_1_borrow_out;
	
	subtractor32_wrapper sub32_v_minus_1
	(
		.clk		(clk),
		.a			(v_din),
		.b			(sub32_din_1),
		.d			(sub32_v_minus_1_difference_out),
		.b_in		(sub32_v_minus_1_borrow_in),
		.b_out	(sub32_v_minus_1_borrow_out)
	);
	
	
	
		//
		// Borrow Masking Logic
		//
	reg	mask_borrow;
	
	always @(posedge clk)
		//
		mask_borrow <= ((proc_cnt > cnt_dec_addr_in_start) && (proc_cnt <= cnt_dec_addr_in_stop)) ?
			1'b0 : 1'b1;
		
	assign sub32_u_minus_v_borrow_in = sub32_u_minus_v_borrow_out & ~mask_borrow;
	assign sub32_v_minus_1_borrow_in = sub32_v_minus_1_borrow_out & ~mask_borrow;
	
		
		//
		// Comparison Logic
		//
	reg	cmp_u_v_l;
	reg	cmp_u_v_e;
	reg	cmp_u_v_g;

	reg	cmp_v_1_l;
	reg	cmp_v_1_e;
	reg	cmp_v_1_g;

	wire	cmp_unresolved_u_v = !(cmp_u_v_l || cmp_u_v_g);
	wire	cmp_unresolved_v_1 = !(cmp_v_1_l || cmp_v_1_g);

	wire	cmp_u_v_borrow_is_set			= (sub32_u_minus_v_borrow_out     ==  1'b1) ? 1'b1 : 1'b0;
	wire	cmp_u_v_difference_is_nonzero	= (sub32_u_minus_v_difference_out != 32'd0) ? 1'b1 : 1'b0;

	wire	cmp_v_1_borrow_is_set			= (sub32_v_minus_1_borrow_out     ==  1'b1) ? 1'b1 : 1'b0;
	wire	cmp_v_1_difference_is_nonzero	= (sub32_v_minus_1_difference_out != 32'd0) ? 1'b1 : 1'b0;

	reg	u_is_even_reg;
	reg	v_is_even_reg;

	always @(posedge clk)
		//
		if (rdy) begin
			//
			if (ena) begin
				//
				cmp_u_v_l		<= 1'b0;
				cmp_u_v_e		<= 1'b0;
				cmp_u_v_g		<= 1'b0;
				//
				cmp_v_1_l		<= 1'b0;
				cmp_v_1_e		<= 1'b0;
				cmp_v_1_g		<= 1'b0;
				//
				u_is_even_reg	<= 1'bX;
				v_is_even_reg	<= 1'bX;
				//
			end
			//
		end else begin
			//
			// parity
			//
			if (calc_parity) begin
				u_is_even_reg <= ~u_din[0];
				v_is_even_reg <= ~v_din[0];
			end
			//
			// u <> v
			//
			if (cmp_unresolved_u_v && calc_leg) begin
				//
				if (cmp_u_v_borrow_is_set)
					cmp_u_v_l <= 1'b1;
				//
				if (!cmp_u_v_borrow_is_set && cmp_u_v_difference_is_nonzero)
					cmp_u_v_g <= 1'b1;
				//
				if (!cmp_u_v_borrow_is_set && !cmp_u_v_difference_is_nonzero && calc_leg_final)
					cmp_u_v_e <= 1'b1;
				//
			end
			//
			// v <> 1
			//
			if (cmp_unresolved_v_1 && calc_leg) begin
				//
				if (cmp_v_1_borrow_is_set)
					cmp_v_1_l <= 1'b1;
				//
				if (!cmp_v_1_borrow_is_set && cmp_v_1_difference_is_nonzero)
					cmp_v_1_g <= 1'b1;
				//
				if (!cmp_v_1_borrow_is_set && !cmp_v_1_difference_is_nonzero && calc_leg_final)
					cmp_v_1_e <= 1'b1;
				//
			end			
			//
		end


		//
		// Output Flags
		//
	assign u_gt_v = !cmp_u_v_l && !cmp_u_v_e &&  cmp_u_v_g;
	assign v_eq_1 = !cmp_v_1_l &&  cmp_v_1_e && !cmp_v_1_g;
	
	assign u_is_even = u_is_even_reg;
	assign v_is_even = v_is_even_reg;


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
