`timescale 1ns / 1ps
/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2013-2014, NewAE Technology Inc. All rights reserved.
Author: Colin O'Flynn <coflynn@newae.com>

  chipwhisperer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  chipwhisperer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/

module trigger_system#(
	parameter stateaddr_width=6,
	parameter stateaddr_states=64
	)(
	/* System */
	input clk,
	input rst,
	
	/* IO line to monitor */
   input mon_line,
	
	/* High for one cycle when trigger sequence met */
   output trig_out,
	
	/* freq = clk / (2*(clkdivider-1)) . Therefor:
	   clkdivider = (clk / (2 * desired_freq)) + 1
	*/
	input [17:0] clkdivider,
	
	/* Programming of trigger data*/
   input state_prog_en,
   input [stateaddr_width-1:0] state_prog_addr,
   input state_prog_wr,
   input [17:0] state_prog_data	
   );
	
	//Clock Enables for 2x and divided clock
	wire clkdiv;
	reg  clkdiv_rst;
	
	wire [stateaddr_width-1:0] stateaddr;
	reg  [stateaddr_width-1:0] stateaddr_reg;

	wire [17:0] statedata;
	
	wire stateaddr_we;

	assign stateaddr = (state_prog_en) ? state_prog_addr : stateaddr_reg;
	assign stateaddr_we = state_prog_en & state_prog_wr;
	
	/* Double-FF used for metastable buffering when crossing clock domain */
	reg int_io_temp, io_line;
	always @(posedge clk) begin
		if (clkdiv) begin
			int_io_temp <= mon_line;
			io_line <= int_io_temp;
		end
	end
	
	/* Detect state change */
	reg laststate;
	wire io_changed;
	always @(posedge clk) begin
		laststate <= io_line;
	end
	assign io_changed = (laststate == io_line) ? 1'b0 : 1'b1;
	
	reg clkdiv_delay;
	always @(posedge clk) begin
		clkdiv_delay <= clkdiv;
	end
	
	/* Count interations in current state */
	reg [8:0] currentstate_cnt;
	always @(posedge clk)
			if (rst) begin
				currentstate_cnt <= 9'd1;
			end else if (io_changed) begin
				currentstate_cnt <= 9'd1;
			end else if (clkdiv_delay) begin
				if (currentstate_cnt < 9'd510)
					currentstate_cnt <= currentstate_cnt + 9'd1;
			end
		
	wire [7:0] expected_low_limit;
	wire [8:0] expected_high_limit;
	wire       expected_state;
	
	assign expected_low_limit = statedata[7:0];
	assign expected_high_limit = statedata[16:8];
	assign expected_state = statedata[17];
	
	// Is state OK and count OK
	reg state_cnt_ok;	
	always @(posedge clk)
		//if (clkdiv)
			state_cnt_ok <= ((expected_state == io_line) && (((currentstate_cnt >= expected_low_limit) && (currentstate_cnt <= expected_high_limit)))) ? 1'b1 : 1'b0;
	
	/* State Counter */
	always @(posedge clk)
		if (rst)
			stateaddr_reg <= 0;
		else begin
			//If IO change & state NOT in correct state, reset
			if ((io_changed & !state_cnt_ok) || (trig_out_reg))
				stateaddr_reg <= 0;
				
			//If IO change & state IN correct state, goto next one
			else if ((io_changed & state_cnt_ok & clkdiv_delay) || (state_cnt_ok & clkdiv_delay & (expected_high_limit == 9'd511)) || (io_changed & state_cnt_ok & clkdiv_delay & (expected_high_limit == 9'd510)))
				stateaddr_reg <= stateaddr_reg + 1'b1;
		end
	
	/* Start the clock divider on change of IO pin to syncronize to device clock */
	always @(posedge clk) begin
		if (rst) begin
			clkdiv_rst <= 1'b1;
		end else if ((stateaddr_reg == 0) && (currentstate_cnt == 9'd510)) begin
			clkdiv_rst <= 1'b1;
		end else begin
			clkdiv_rst <= 1'b0;
		end
	end
	
	/* Watch for end of state storage */
	reg trig_out_reg;
	assign trig_out = trig_out_reg;
	always @(posedge clk)
		if (statedata == 18'b111111111111111111)
			trig_out_reg <= 1;
		else
			trig_out_reg <= 0;

	/* State RAM */
	state_ram #(stateaddr_width, stateaddr_states) sram (
		.clk(clk),
		.din(state_prog_data),
		.addr(stateaddr),
		.we(stateaddr_we),
		.dout(statedata)
	);
	
	/* Clock Divider */
	clk_div cdiv(
		.clk(clk),
		.rst(clkdiv_rst),
		.clk2xdiv_setting(clkdivider),
		//.clk2xdiv(clk2xdiv),
		.clk2xdiv(),
		.clkdiv(clkdiv));
	/*
		wire [35:0] control;
		wire [127:0] cs;
			
		assign cs[5:0] = stateaddr_reg;
		assign cs[6] = io_line;
		assign cs[7] = trig_out;
		assign cs[25:8] = statedata;
		assign cs[34:26] = currentstate_cnt;
		assign cs[35] = clkdiv_rst;
		assign cs[36] = clkdiv;
		assign cs[37] = io_changed;
		assign cs[38] = state_cnt_ok;
		assign cs[39] = mon_line;
	
coregen_ila ila (
    .CONTROL(control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs) // IN BUS [31:0]
);

coregen_icon icon (
    .CONTROL0(control) // INOUT BUS [35:0]
);
*/
endmodule


module clk_div(
	input clk,
	input rst,
	input [17:0] clk2xdiv_setting,
	output clk2xdiv,
	output clkdiv
	);
		
	reg clk2xdiv_reg, clkdiv_reg, clkdiv_reg_last;
	reg [17:0] clk2xcnt;
	
	assign clk2xdiv = clk2xdiv_reg;
	assign clkdiv = clkdiv_reg;
		
	always @(posedge clk) begin
		if (rst)
			clkdiv_reg_last <= 1'b0;
		else if (clk2xcnt == clk2xdiv_setting)
			clkdiv_reg_last <= ~clkdiv_reg_last;
	end
		
	always @(posedge clk) begin	
		if (rst) begin
			clk2xdiv_reg <= 1'b1;
			clkdiv_reg <= 1'b1;
			clk2xcnt <= 0;
		end else if (clk2xcnt == clk2xdiv_setting) begin
			clk2xdiv_reg <= 1'b1;
			clkdiv_reg <= clkdiv_reg_last;
			clk2xcnt <= 0;
		end else begin
			clk2xdiv_reg <= 1'b0;
			clkdiv_reg <= 1'b0;
			clk2xcnt <= clk2xcnt + 18'd1;
		end
	end
	
endmodule


module state_ram#(
	parameter address_width=6,
	parameter mem_elements=64,
	parameter data_width=18 //Check datasheet for acceptable values
	)(
	input clk,
	input [data_width-1:0] din,
	input [address_width-1:0] addr,
	input we,
	output [data_width-1:0] dout
	);

reg [data_width-1:0] mem[mem_elements-1:0];

/* Use the directive above ONLY if you want to disable Block RAM extraction.
Set the block_ram attribute to FALSE on the signal memory.
The block_ram attribute must be set on the memory signal. */

reg [address_width - 1:0] addr_reg;

always @(posedge clk)
begin
addr_reg <= addr;
if (we)
mem[addr] <= din;
end

assign dout = mem[addr_reg];

endmodule 