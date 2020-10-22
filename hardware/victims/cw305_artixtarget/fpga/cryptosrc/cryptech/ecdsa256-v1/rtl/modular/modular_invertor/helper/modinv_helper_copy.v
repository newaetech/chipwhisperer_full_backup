`timescale 1ns / 1ps

module modinv_helper_copy
	(
		clk, rst_n,
		ena, rdy,
		s_addr,  s_din,
		a1_addr,        a1_wren, a1_dout
	);
	
	
		//
		// Parameters
		//
	parameter OPERAND_NUM_WORDS	= 8;
	parameter OPERAND_ADDR_BITS	= 3;
	
	parameter BUFFER_NUM_WORDS		= 9;
	parameter BUFFER_ADDR_BITS		= 4;
	
	
		//
		// clog2
		//
`include "modinv_clog2.v"
	
	
		//
		// Constants
		//
	localparam PROC_NUM_CYCLES	= OPERAND_NUM_WORDS + 2;
	localparam PROC_CNT_BITS	= clog2(PROC_NUM_CYCLES);
	
	
		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	
	input		wire									ena;
	output	wire									rdy;

	output	wire	[ BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[OPERAND_ADDR_BITS-1:0]	a1_addr;
	
	output	wire									a1_wren;
	
	input		wire	[                 31:0]	s_din;

	output	wire	[                 31:0]	a1_dout;


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
	reg	[OPERAND_ADDR_BITS-1:0]	addr_s;

	wire	[OPERAND_ADDR_BITS-1:0]	addr_s_max		= OPERAND_NUM_WORDS - 1;
	wire	[OPERAND_ADDR_BITS-1:0]	addr_s_zero		= {OPERAND_ADDR_BITS{1'b0}};
	wire	[OPERAND_ADDR_BITS-1:0]	addr_s_next		= (addr_s < addr_s_max) ?
																		addr_s + 1'b1 : addr_s_zero;
																		
	reg	[OPERAND_ADDR_BITS-1:0]	addr_a1;
	
	wire	[OPERAND_ADDR_BITS-1:0]	addr_a1_max		= OPERAND_NUM_WORDS - 1;
	wire	[OPERAND_ADDR_BITS-1:0]	addr_a1_zero	= {OPERAND_ADDR_BITS{1'b0}};
	wire	[OPERAND_ADDR_BITS-1:0]	addr_a1_next	= (addr_a1 < addr_a1_max) ?
																		addr_a1 + 1'b1 : addr_a1_zero;
																		
	assign s_addr  = {{(BUFFER_ADDR_BITS - OPERAND_ADDR_BITS){1'b0}}, addr_s};
	assign a1_addr = addr_a1;
	
		
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
	
	
		//
		// Address Increment Logic
		//
	wire	inc_addr_s;
	wire	inc_addr_a1;

	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_s_start		= 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_s_stop		= OPERAND_NUM_WORDS + 0;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_a1_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_a1_stop		= OPERAND_NUM_WORDS + 1;

	assign inc_addr_s		= (proc_cnt >= cnt_inc_addr_s_start)  && (proc_cnt <= cnt_inc_addr_s_stop);
	assign inc_addr_a1	= (proc_cnt >= cnt_inc_addr_a1_start) && (proc_cnt <= cnt_inc_addr_a1_stop);
	
	always @(posedge clk) begin
		//
		if (inc_addr_s)	addr_s <= addr_s_next;
		else					addr_s <= addr_s_zero;
		//
		if (inc_addr_a1)	addr_a1 <= addr_a1_next;
		else					addr_a1 <= addr_a1_zero;
		//
	end
	
	
		//
		// Write Enable Logic
		//
	wire	wren_a1;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_a1_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_a1_stop	= OPERAND_NUM_WORDS + 1;

	assign wren_a1 = (proc_cnt >= cnt_wren_a1_start) && (proc_cnt <= cnt_wren_a1_stop);

	assign a1_wren = wren_a1;
	
	
		//
		// Data Logic
		//
	assign a1_dout = s_din;
	
	
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
