`timescale 1ns / 1ps

module modinv_helper_invert_update
	(
		clk, rst_n,
		ena, rdy,
		
		u_gt_v, v_eq_1,
		u_is_even, v_is_even,
		
		r_addr, r_wren, r_dout,
		s_addr, s_wren, s_dout,
		u_addr, u_wren, u_dout,
		v_addr, v_wren, v_dout,
		
		r_dbl_addr,          r_dbl_din,
		s_dbl_addr,          s_dbl_din,
		r_plus_s_addr,       r_plus_s_din,
		u_half_addr,         u_half_din,
		v_half_addr,         v_half_din,
		u_minus_v_half_addr, u_minus_v_half_din,
		v_minus_u_half_addr, v_minus_u_half_din
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
	localparam PROC_NUM_CYCLES	= BUFFER_NUM_WORDS + 3;
	localparam PROC_CNT_BITS	= clog2(PROC_NUM_CYCLES);
	
	
		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	input		wire									ena;
	output	wire									rdy;

	input		wire									u_gt_v;
	input		wire									v_eq_1;
	input		wire									u_is_even;
	input		wire									v_is_even;
		
	output	wire	[BUFFER_ADDR_BITS-1:0]	r_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_addr;
		
	output	wire									r_wren;
	output	wire									s_wren;
	output	wire									u_wren;
	output	wire									v_wren;
		
	output	wire	[              32-1:0]	r_dout;
	output	wire	[              32-1:0]	s_dout;
	output	wire	[              32-1:0]	u_dout;
	output	wire	[              32-1:0]	v_dout;
		
	output	wire	[BUFFER_ADDR_BITS-1:0]	r_dbl_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	s_dbl_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	r_plus_s_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_minus_v_half_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_minus_u_half_addr;
		
	input		wire	[              32-1:0]	r_dbl_din;
	input		wire	[              32-1:0]	s_dbl_din;
	input		wire	[              32-1:0]	r_plus_s_din;
	input		wire	[              32-1:0]	u_half_din;
	input		wire	[              32-1:0]	v_half_din;
	input		wire	[              32-1:0]	u_minus_v_half_din;
	input		wire	[              32-1:0]	v_minus_u_half_din;
		
	
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

	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_max		= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_in_next	= (addr_in < addr_in_max) ?
																		addr_in + 1'b1 : addr_in_zero;
																		
	reg	[BUFFER_ADDR_BITS-1:0]	addr_out;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out_max	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_out_next	= (addr_out < addr_out_max) ?
																		addr_out + 1'b1 : addr_out_zero;
																		
	assign r_addr					= addr_out;
	assign s_addr					= addr_out;
	assign u_addr					= addr_out;
	assign v_addr					= addr_out;
	
	assign r_dbl_addr				= addr_in;
	assign s_dbl_addr				= addr_in;
	assign r_plus_s_addr			= addr_in;
	assign u_half_addr			= addr_in;
	assign v_half_addr			= addr_in;
	assign u_minus_v_half_addr	= addr_in;
	assign v_minus_u_half_addr	= addr_in;
	
	
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
	
	
		//
		// Address Increment Logic
		//
	wire	inc_addr_in;
	wire	inc_addr_out;

	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_in_start	= 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_in_stop		= BUFFER_NUM_WORDS;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_out_stop	= BUFFER_NUM_WORDS + 1;

	assign inc_addr_in  = (proc_cnt >= cnt_inc_addr_in_start)  && (proc_cnt <= cnt_inc_addr_in_stop);
	assign inc_addr_out = (proc_cnt >= cnt_inc_addr_out_start) && (proc_cnt <= cnt_inc_addr_out_stop);
	
	always @(posedge clk) begin
		//
		if (inc_addr_in)	addr_in <= addr_in_next;
		else					addr_in <= addr_in_zero;
		//
		if (inc_addr_out)	addr_out <= addr_out_next;
		else					addr_out <= addr_out_zero;
		//
	end
	
		//
		// Write Enable Logic
		//
	wire	wren_out;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_out_stop		= BUFFER_NUM_WORDS + 1;

	assign wren_out = (proc_cnt >= cnt_wren_out_start) && (proc_cnt <= cnt_wren_out_stop);

	reg	r_wren_allow;
	reg	s_wren_allow;
	reg	u_wren_allow;
	reg	v_wren_allow;

	assign r_wren = wren_out && r_wren_allow && !v_eq_1 && !rdy;
	assign s_wren = wren_out && s_wren_allow && !v_eq_1 && !rdy;
	assign u_wren = wren_out && u_wren_allow && !v_eq_1 && !rdy;
	assign v_wren = wren_out && v_wren_allow && !v_eq_1 && !rdy;
	
	
		//
		// Data Logic
		//
	reg	[31: 0]	r_dout_mux;
	reg	[31: 0]	s_dout_mux;
	reg	[31: 0]	u_dout_mux;
	reg	[31: 0]	v_dout_mux;
	
	assign r_dout = r_dout_mux;
	assign s_dout = s_dout_mux;
	assign u_dout = u_dout_mux;
	assign v_dout = v_dout_mux;
	
	always @(*) begin
		//
		// r, s, u, v
		//
		if (u_is_even) begin
			//
			u_dout_mux		= u_half_din;
			v_dout_mux		= {32{1'bX}};
			r_dout_mux		= {32{1'bX}};
			s_dout_mux		= s_dbl_din;
			//
			u_wren_allow	= 1'b1;
			v_wren_allow	= 1'b0;
			r_wren_allow	= 1'b0;
			s_wren_allow	= 1'b1;
			//
		end else begin
			//
			if (v_is_even) begin
				//
				u_dout_mux		= {32{1'bX}};
				v_dout_mux		= v_half_din;
				r_dout_mux		= r_dbl_din;
				s_dout_mux		= {32{1'bX}};
				//
				u_wren_allow	= 1'b0;
				v_wren_allow	= 1'b1;
				r_wren_allow	= 1'b1;
				s_wren_allow	= 1'b0;
				//
			end else begin
				//
				u_dout_mux		=  u_gt_v ? u_minus_v_half_din : {32{1'bX}};
				v_dout_mux		=  u_gt_v ? {32{1'bX}}         : v_minus_u_half_din;
				r_dout_mux		=  u_gt_v ? r_plus_s_din       : r_dbl_din;
				s_dout_mux		=  u_gt_v ? s_dbl_din          : r_plus_s_din;
				//
				u_wren_allow	=  u_gt_v;
				v_wren_allow	= !u_gt_v;
				r_wren_allow	=  1'b1;
				s_wren_allow	=  1'b1;
				//
			end
			//
		end
		//
	end
		
		
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
