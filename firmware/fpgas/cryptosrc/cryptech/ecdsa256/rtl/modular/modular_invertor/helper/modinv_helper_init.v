`timescale 1ns / 1ps

module modinv_helper_init
	(
		clk, rst_n,
		ena, rdy,
		a_addr, a_din,
		q_addr, q_din,
		r_addr, r_wren, r_dout,
		s_addr, s_wren, s_dout,
		u_addr, u_wren, u_dout,
		v_addr, v_wren, v_dout
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
	localparam PROC_NUM_CYCLES	= OPERAND_NUM_WORDS + 3;
	localparam PROC_CNT_BITS	= clog2(PROC_NUM_CYCLES);
	
	
		//
		// Ports
		//
	input		wire									clk;
	input		wire									rst_n;
	input		wire									ena;
	output	wire									rdy;

	output	wire	[OPERAND_ADDR_BITS-1:0]	a_addr;
	output	wire	[OPERAND_ADDR_BITS-1:0]	q_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	r_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	s_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	u_addr;
	output	wire	[ BUFFER_ADDR_BITS-1:0]	v_addr;
	
	output	wire									r_wren;
	output	wire									s_wren;
	output	wire									u_wren;
	output	wire									v_wren;
	
	input		wire	[                 31:0]	a_din;
	input		wire	[                 31:0]	q_din;
	output	wire	[                 31:0]	r_dout;
	output	wire	[                 31:0]	s_dout;
	output	wire	[                 31:0]	u_dout;
	output	wire	[                 31:0]	v_dout;


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
	reg	[OPERAND_ADDR_BITS-1:0]	addr_aq;

	wire	[OPERAND_ADDR_BITS-1:0]	addr_aq_max		= OPERAND_NUM_WORDS - 1;
	wire	[OPERAND_ADDR_BITS-1:0]	addr_aq_zero	= {OPERAND_ADDR_BITS{1'b0}};
	wire	[OPERAND_ADDR_BITS-1:0]	addr_aq_next	= (addr_aq < addr_aq_max) ?
																		addr_aq + 1'b1 : addr_aq_zero;
																		
	reg	[BUFFER_ADDR_BITS-1:0]	addr_rsuv;
	
	wire	[BUFFER_ADDR_BITS-1:0]	addr_rsuv_max	= BUFFER_NUM_WORDS - 1;
	wire	[BUFFER_ADDR_BITS-1:0]	addr_rsuv_zero	= {BUFFER_ADDR_BITS{1'b0}};
	wire	[BUFFER_ADDR_BITS-1:0]	addr_rsuv_next	= (addr_rsuv < addr_rsuv_max) ?
																		addr_rsuv + 1'b1 : addr_rsuv_zero;
																		
	assign a_addr = addr_aq;
	assign q_addr = addr_aq;
	
	assign r_addr = addr_rsuv;
	assign s_addr = addr_rsuv;
	assign u_addr = addr_rsuv;
	assign v_addr = addr_rsuv;
	
		
		//
		// Ready Flag
		//
	assign rdy = (proc_cnt == proc_cnt_zero);
	
	
		//
		// Address Increment Logic
		//
	wire	inc_addr_aq;
	wire	inc_addr_rsuv;

	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_aq_start	= 1;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_aq_stop		= OPERAND_NUM_WORDS;
	
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_rsuv_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_inc_addr_rsuv_stop	= BUFFER_NUM_WORDS + 1;

	assign inc_addr_aq   = (proc_cnt >= cnt_inc_addr_aq_start)   && (proc_cnt <= cnt_inc_addr_aq_stop);
	assign inc_addr_rsuv = (proc_cnt >= cnt_inc_addr_rsuv_start) && (proc_cnt <= cnt_inc_addr_rsuv_stop);
	
	always @(posedge clk) begin
		//
		if (inc_addr_aq)	addr_aq <= addr_aq_next;
		else					addr_aq <= addr_aq_zero;
		//
		if (inc_addr_rsuv)	addr_rsuv <= addr_rsuv_next;
		else						addr_rsuv <= addr_rsuv_zero;
		//
	end
	
	
		//
		// Write Enable Logic
		//
	wire	wren_rsuv;

	wire	[PROC_CNT_BITS-1:0]	cnt_wren_rsuv_start	= 2;
	wire	[PROC_CNT_BITS-1:0]	cnt_wren_rsuv_stop	= BUFFER_NUM_WORDS + 1;

	assign wren_rsuv = (proc_cnt >= cnt_wren_rsuv_start) && (proc_cnt <= cnt_wren_rsuv_stop);

	assign r_wren = wren_rsuv;
	assign s_wren = wren_rsuv;
	assign u_wren = wren_rsuv;
	assign v_wren = wren_rsuv;
	
	
		//
		// Data Logic
		//
	assign r_dout = 32'd0;
	assign s_dout = (proc_cnt == cnt_wren_rsuv_start) ? 32'd1 : 32'd0;
	assign u_dout = (proc_cnt != cnt_wren_rsuv_stop)  ? q_din : 32'd0;
	assign v_dout = (proc_cnt != cnt_wren_rsuv_stop)  ? a_din : 32'd0;
	
	
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
