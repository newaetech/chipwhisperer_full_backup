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

module usitx_tb;

	// Inputs
	reg clk;
	reg rst;
	reg mon_line;
	reg [17:0] clkdivider;
	reg state_prog_en;
	reg [15:0] state_prog_addr;
	reg state_prog_wr;
	reg [7:0] state_prog_data;
	reg run;
	reg [16:0] num_states;

	// Outputs
	wire done;
	wire dataout;
	
	always begin
		clk = !clk;
		#5;
	end

	// Instantiate the Unit Under Test (UUT)
	usitx uut (
		.clk(clk), 
		.rst(rst),
		.dataout(dataout),		
		.run(run),
		.done(done),
		.num_states(num_states),
		.clkdivider(clkdivider), 
		.state_prog_en(state_prog_en), 
		.state_prog_addr(state_prog_addr), 
		.state_prog_wr(state_prog_wr), 
		.state_prog_data(state_prog_data)
	);
	
	

	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;
		run = 0;
		clkdivider = 2;
		state_prog_en = 0;
		state_prog_addr = 0;
		state_prog_wr = 0;
		state_prog_data = 0;

		// Wait 100 ns for global reset to finish
		#100;
		
		rst = 0;
		
		#50;
		
		state_prog_en = 1;
		write_one(8'hAA, 0);
		write_one(8'hFF, 1);
		write_one(8'hF0, 2);
		state_prog_en = 0;
		num_states = 3;
		
		run = 1;
		
		#500;
        
		// Add stimulus here
		
		//simulate_ioline();
		
		@(posedge done);
		
		#100;
		
		$finish();
		
	end


task write_one;
	input [7:0] data;
	input [15:0] addr;
	begin
	
	@(posedge clk);
	state_prog_data = data;
	state_prog_addr = addr;
	state_prog_wr = 1;
	
	@(posedge clk);
	state_prog_wr = 0;
		
	end
endtask    
	
		
endmodule




