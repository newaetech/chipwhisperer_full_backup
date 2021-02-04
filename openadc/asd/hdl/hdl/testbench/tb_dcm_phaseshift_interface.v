`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   18:23:40 06/06/2012
// Design Name:   dcm_phaseshift_interface
// Module Name:   C:/E/Documents/academic/sidechannel/aesexplorer/fpga_hdl/ise_projects/adcboard/tb_dcm_phaseshift_interface.v
// Project Name:  adcboard
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: dcm_phaseshift_interface
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module tb_dcm_phaseshift_interface;

	// Inputs
	reg reset;
	reg [8:0] value_i;
	reg load_i;
	wire dcm_psen;
	wire dcm_psincdec;
	wire dcm_psdone;
	wire [7:0] dcm_status;

	// Outputs
	wire [8:0] value_o;
	wire done_o;
	
	reg clk = 0;
	reg pclk = 0;

	// Instantiate the Unit Under Test (UUT)
	dcm_phaseshift_interface uut (
		.clk_i(pclk), 
		.reset_i(reset), 
		.default_value_i(0), 
		.value_i(value_i), 
		.load_i(load_i), 
		.value_o(value_o), 
		.done_o(done_o), 
		.dcm_psen_o(dcm_psen), 
		.dcm_psincdec_o(dcm_psincdec), 
		.dcm_status_i(dcm_status),
		.dcm_psdone_i(dcm_psdone)
	);
	
	wire dcm_clk;
	wire psdone;
	wire locked;
	wire clkfx;
	
	
	// DCM_SP: Digital Clock Manager
// Spartan-6
// Xilinx HDL Libraries Guide, version 13.2
DCM_SP #(
.CLKFX_DIVIDE(1), // Divide value on CLKFX outputs - D - (1-32)
.CLKFX_MULTIPLY(2), // Multiply value on CLKFX outputs - M - (2-32)
.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
.CLKIN_PERIOD(10.0), // Input clock period specified in nS
.CLKOUT_PHASE_SHIFT("VARIABLE"), // Output phase shift (NONE, FIXED, VARIABLE)
.CLK_FEEDBACK("1X"), // Feedback source (NONE, 1X, 2X)
.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
.PHASE_SHIFT(0), // Amount of fixed phase shift (-255 to 255)
.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
)
DCM_SP_inst (
.CLK0(dcm_clk), // 1-bit output: 0 degree clock output
.CLKFX(clkfx), // 1-bit output: Digital Frequency Synthesizer output (DFS)
.LOCKED(locked), // 1-bit output: DCM_SP Lock Output
.PSDONE(dcm_psdone), // 1-bit output: Phase shift done output
.STATUS(dcm_status), // 8-bit output: DCM_SP status output
.CLKFB(dcm_clk), // 1-bit input: Clock feedback input
.CLKIN(clk), // 1-bit input: Clock input
.PSCLK(pclk), // 1-bit input: Phase shift clock input
.PSEN(dcm_psen), // 1-bit input: Phase shift enable
.PSINCDEC(dcm_psincdec), // 1-bit input: Phase shift increment/decrement input
.RST(reset) // 1-bit input: Active high reset input
);
// End of DCM_SP_inst instantiation

	always begin
		#5 clk = ~clk;
	end
	
	always begin
		#12 pclk = ~pclk;
	end
	
	initial begin
		// Initialize Inputs
		reset = 1;
		value_i = 0;
		load_i = 0;

		// Wait 100 ns for global reset to finish
		#100;		
		reset = 0;
		
		#100;
        
		// Add stimulus here
		value_i = 8'd200;
		#5;
		load_i = 1;
		@(posedge pclk);
		#5;
		load_i = 0;				
		@(posedge done_o);
		
		#200;
		
		value_i = 8'd100;
		#5;
		load_i = 1;
		@(posedge pclk);
		#5;
		load_i = 0;				
		@(posedge done_o);
		
		#200;
		
		value_i = 8'd0;
		#5;
		load_i = 1;
		@(posedge pclk);
		#5;
		load_i = 0;				
		@(posedge done_o);
		
		#100;
		
		$finish;
		
	end
      
endmodule

