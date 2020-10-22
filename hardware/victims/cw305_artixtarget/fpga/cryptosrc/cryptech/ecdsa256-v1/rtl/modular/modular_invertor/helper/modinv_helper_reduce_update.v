`timescale 1ns / 1ps

module modinv_helper_reduce_update
	(
		clk, rst_n,
		ena, rdy,
		
		s_is_odd, k_is_nul,
		
		s_addr, s_wren, s_dout,
		u_addr,                 u_din,
		v_addr,                 v_din
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

	input		wire									s_is_odd;
	input		wire									k_is_nul;
		
	output	wire	[BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[BUFFER_ADDR_BITS-1:0]	v_addr;
		
	output	wire									s_wren;
		
	output	wire	[              32-1:0]	s_dout;

	input		wire	[              32-1:0]	u_din;
	input		wire	[              32-1:0]	v_din;
		
	
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
																		
	assign s_addr					= addr_out;
	assign u_addr					= addr_in;
	assign v_addr					= addr_in;
	
	
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

	assign s_wren = wren_out && !k_is_nul; //s_wren_allow && !v_eq_1 && !rdy;
	
	
		//
		// Data Logic
		//
	assign s_dout = s_is_odd ? v_din : u_din;

		
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
