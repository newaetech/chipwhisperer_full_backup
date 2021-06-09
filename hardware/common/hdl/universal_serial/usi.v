`timescale 1ns / 1ps
/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project (and file) is released under the 2-Clause BSD License:

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.


*************************************************************************/
module usitx#(
	parameter stateaddr_width=10,
	parameter data_num=1024
	)(
	/* System */
	input clk,
	input rst,
	input idle,
	output reg dataout,
	input run,
	output reg done,
	input [stateaddr_width-1:0] num_states,

	
	/* freq = clk / (2*(clkdivider-1)) . Therefor:
	   clkdivider = (clk / (2 * desired_freq)) + 1
	*/
	input [17:0] clkdivider,
	
	/* Programming of trigger data*/
   input state_prog_en,
   input [stateaddr_width-1:0] state_prog_addr,
   input state_prog_wr,
   input [7:0] state_prog_data	
   );
	
	//Clock Enables for 2x and divided clock
	wire clk2xdiv, clkdiv;
	
	wire [stateaddr_width-1:0] stateaddr;
	reg  [stateaddr_width:0] stateaddr_reg;

	wire [7:0] statedata;
	
	wire stateaddr_we;

	assign stateaddr = (state_prog_en) ? state_prog_addr : stateaddr_reg;
	assign stateaddr_we = state_prog_en & state_prog_wr;

	 reg oneshot_run;
	 reg oneshot_run_arm;
	 always @(posedge clk)
		if (run == 0)
			oneshot_run_arm <= 1;
		else if (oneshot_run)
			oneshot_run_arm <= 0;
			
	 always @(posedge clk)
		if (clkdiv)
			if (run & oneshot_run_arm)
				oneshot_run <= 1;
			else
				oneshot_run <= 0;
		

	reg go;
	
	wire addrmatch;
	
	assign addrmatch = (stateaddr_reg == num_states) ? 1'b1 : 1'b0;
	
	always @(posedge clk)
		if (oneshot_run & clkdiv)
			go <= 1;
		else if (addrmatch | rst)
			go <= 0;

	always @(posedge clk)
		if (go == 1'b0)
			stateaddr_reg <= 0;
		else if (clkdiv)
			if (bitcnt == 3'b111)
				stateaddr_reg <= stateaddr_reg + 1'b1;
		
	reg [2:0] bitcnt;
	wire advgo;
	assign advgo = (oneshot_run & clkdiv) | go;
	always @(posedge clk)
		if (advgo == 1'b0)
			bitcnt <= 0;
		else if (clkdiv)
			bitcnt <= bitcnt + 1'b1;
	
	reg datareg;
	always @(posedge clk)
		if (clkdiv)
			datareg <= statedata[bitcnt];
			
	always @(posedge clk)
		if (clkdiv)
			dataout <= (go) ? datareg : idle;
			
	always @(posedge clk)
		if	(run == 1'b0)
			done <= 0;
		else if (addrmatch == 1'b1)
			done <= 1;
	
	/* State RAM */
	state_ram #(stateaddr_width, data_num, 8) sram (
		.clk(clk),
		.din(state_prog_data),
		.addr(stateaddr),
		.we(stateaddr_we),
		.dout(statedata)
	);
	
	/* Clock Divider */
	clk_div cdiv(
		.clk(clk),
		.rst(rst),
		.clk2xdiv_setting(clkdivider),
		.clk2xdiv(clk2xdiv),
		.clkdiv(clkdiv));
endmodule

module usirx#(
	parameter stateaddr_width=10,
	parameter data_num=1024
	)(
	/* System */
	input clk,
	input rst,
	input idle,
	input datain,
	input run,
	output reg done,
	input [stateaddr_width-1:0] num_states,

	
	/* freq = clk / (2*(clkdivider-1)) . Therefor:
	   clkdivider = (clk / (2 * desired_freq)) + 1
	*/
	input [17:0] clkdivider,
	
	/* Read of Data */
   input state_read_en,
   input [stateaddr_width-1:0] state_read_addr,
   output reg [7:0] state_read_data	
   );
	
	//Clock Enables for 2x and divided clock
	wire clk2xdiv, clkdiv;
	
	wire [stateaddr_width-1:0] stateaddr;
	reg  [stateaddr_width:0] stateaddr_reg;

	wire [7:0] statedata;
	
	wire stateaddr_we;

	//When reading out, get address from commanding module
	assign stateaddr = (state_read_en) ? state_read_addr : stateaddr_reg;

	//Fix metastability due to crossing clock domains
	reg tempinput;
	reg data;
	always @(posedge clk) begin
		tempinput <= datain;
		data <= tempinput;
	end
	
	//Indicates wire is NOT in idle state, used for marking start
	reg not_idle4;
	
	reg [2:0] idlecnt;
	always @(posedge clk)
		if (data != idle)
			idlecnt <= idlecnt + 1'b1;
		else
			idlecnt <= 0;
			
	always @(posedge clk)
		if (idlecnt == 3'b111)
			not_idle4 <= 1;
		else
			not_idle4 <= 0;
	
	//assign not_idle = ~data;//(data == idle) ? 1'b0 : 1'b1;
	
	//We don't sample the wire, rather we consider if it's been more '1' than '0' in the previous
	//sample-clock time.
	reg reset_cnt;
	reg [15:0] zeros_cnt;
	reg [15:0] ones_cnt;
	
	always @(posedge clk)
		if (reset_cnt | rst)
			zeros_cnt <= 0;
		else if (data == 0)
			zeros_cnt <= zeros_cnt + 1'b1;
			
	always @(posedge clk)
		if (reset_cnt | rst)
			ones_cnt <= 0;
		else if (data == 1)
			ones_cnt <= ones_cnt + 1'b1;
			
	reg bit_state;
	
	always @(posedge clk)
		if (ones_cnt > zeros_cnt)
			bit_state <= 1;
		else
			bit_state <= 0;
		
   reg prevclkdiv;		
	always @(posedge clk)
		prevclkdiv <= clkdiv;
		
	always @(posedge clk)
		if (((prevclkdiv == 0) & (clkdiv == 1)) | (oneshot_run))
			reset_cnt <= 'b1;
		else
			reset_cnt <= 'b0;
			
	//Once user requests we start running, doesn't take effect until receive line becomes non-idle
	reg oneshot_run;
	reg oneshot_run_arm;
	always @(posedge clk)
		if (run == 0)
			oneshot_run_arm <= 'b1;
		else if (oneshot_run)
			oneshot_run_arm <= 'b0;
			
	//If we are 'armed', user requests run, and line is not idle we start
	always @(posedge clk)
			if (run & oneshot_run_arm & not_idle4)
				oneshot_run <= 'b1;
			else
				oneshot_run <= 'b0;
		
	reg go;
	wire addrmatch;
	assign addrmatch = (stateaddr_reg == num_states) ? 1'b1 : 1'b0;
		
	//Latch start condition until we write specified number of bytes
	always @(posedge clk)
		if (oneshot_run)
			go <= 'b1;
		else if (addrmatch | rst)
			go <= 'b0;

	//Increment the address every time clockdiv starts
	always @(posedge clk)
		if (go == 1'b0)
			stateaddr_reg <= 0;
		else if (ram_wr)
			if (bitcnt == 3'b001)
				stateaddr_reg <= stateaddr_reg + 1'b1;
		
	reg [2:0] bitcnt;
	wire advgo;
	//assign advgo = (oneshot_run & clkdiv) | go;
	always @(posedge clk)
	   if (go == 1'b0)
			bitcnt <= 'b0;
		else if (clkdiv)
			bitcnt <= bitcnt + 1'b1;
	
	always @(posedge clk)
		if	(run == 1'b0)
			done <= 'b0;
		else if (addrmatch == 1'b1)
			done <= 'b1;
	
	reg [7:0] data_to_ram;
			
	//Every rising edge of clkdiv, store a new bit
	always @(posedge clk)
		if (clkdiv)
			data_to_ram[bitcnt] <= bit_state;
			
	reg ram_wr;
	always @(posedge clk)
		if (prevclkdiv)
			if (bitcnt == 3'b000)
				ram_wr <= go;
			else
				ram_wr <= 1'b0;
	
	wire[7:0] read_data;
	
	always @(posedge clk)
		if (state_read_en)
			state_read_data <= read_data;
	
	/* State RAM */
	state_ram #(stateaddr_width, data_num, 8) sram (
		.clk(clk),
		.din(data_to_ram),
		.addr(stateaddr),
		.we(ram_wr),
		.dout(read_data)
	);
	
			
	/* Clock Divider */
	clk_div cdiv(
		.clk(clk),
		.rst(rst | ~go),
		.clk2xdiv_setting(clkdivider),
		.clk2xdiv(clk2xdiv),
		.clkdiv(clkdiv));			
			
	
endmodule