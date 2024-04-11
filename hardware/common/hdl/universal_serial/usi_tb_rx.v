`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   20:02:26 06/12/2013
// Design Name:   trigger_system
// Module Name:   C:/E/Documents/fpga/trigger_digital_signal/trigger_system/trigger_system_tb.v
// Project Name:  trigger_system
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: trigger_system
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module usirx_tb;

	// Inputs
	reg clk;
	reg rst;
	reg idle;
	reg mon_line;
	reg [17:0] clkdivider;
	reg state_prog_en;
	reg [15:0] state_prog_addr;
	reg state_prog_wr;
	wire [7:0] state_prog_data;
	reg run;
	reg [16:0] num_states;
	wire done;
	reg datain;
	
	always begin
		clk = !clk;
		#5;
	end

	// Instantiate the Unit Under Test (UUT)
	usirx uut (
		.clk(clk), 
		.rst(rst),
		.idle(idle),
		.datain(datain),		
		.run(run),
		.done(done),
		.num_states(num_states),
		.clkdivider(clkdivider), 
		.state_read_en(state_prog_en), 
		.state_read_addr(state_prog_addr), 
		.state_read_data(state_prog_data)
	);
	
	

	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;
		run = 0;
		clkdivider = 8;
		state_prog_en = 0;
		state_prog_addr = 0;
		datain = 1;
		idle = 1;

		// Wait 100 ns for global reset to finish
		#100;
		
		rst = 0;
		
		#50;
		num_states = 20;
		
		run = 1;		
		datain = 1;
		#1000;
		datain = 0;
		#250;
		datain = 1;
		#500;
		datain = 0;
		#1000;
		datain = 1;
		
		@(posedge done);
		
		#100;
		
		state_prog_en = 1;
		read_one(16'h0);
		read_one(16'h1);
		state_prog_en = 0;
		
		
		$finish();
		
	end


task read_one;
	input [15:0] addr;
	begin
	
	state_prog_addr = addr;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	$display("%x", state_prog_data);
		
	end
endtask    
	
		
endmodule




