`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   19:51:02 07/04/2014
// Design Name:   trigger_resync
// Module Name:   C:/E/Documents/academic/sidechannel/chipwhisperer/hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/trigger_resync_tb.v
// Project Name:  ztex_rev2_1.11c_ise
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: trigger_resync
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module trigger_resync_tb;

	// Inputs
	reg reset;
	reg clk;
	reg exttrig;
	reg [31:0] offset;

	// Outputs
	wire exttrigger_resync;
	
   always
   begin
      clk <= 0;
      #25;
      clk <= 1;
      #25;
   end

	// Instantiate the Unit Under Test (UUT)
	trigger_resync uut (
		.reset(reset), 
		.clk(clk), 
		.exttrig(exttrig), 
		.offset(offset), 
		.exttrigger_resync(exttrigger_resync)
	);

	initial begin
		// Initialize Inputs
		reset = 1;
		clk = 0;
		exttrig = 0;
		offset = 1;

		// Wait 100 ns for global reset to finish
		#100;
		
		reset = 0;
		
		#497;
		
		exttrig = 1;
		#500;
		exttrig = 0;
		
		#500;
		
		exttrig = 1;
		#5;
		exttrig = 0;
		
		#500;
        
		// Add stimulus here

		$finish ;

	end
      
endmodule

