`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the clock managment, which deals with selecting the ADC clock and
required phase shifts. This file will be very implementation-specific, since
different chips & vendors offer different features.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/
module clock_managment(
	 input		  reset,

	 /* Clock sources */
    input 		  clk_100mhz, //Internal clock
    input 		  ext_clk,    //External clock 
	 
	 /* Clock to ADC */
	 output 		  adc_clk,    //Output clock to ADC
	 
	 /* Clock selection */
	 input        use_ext_clk,
	 
	 /* Clock to System compensates for wire delay of ADC clock */
	 output       systemsample_clk,
	 
	 /* Phase shift control for external clock*/
	 input        phase_clk,
	 input [8:0]  phase_requested,
	 output [8:0] phase_actual,
	 input		  phase_load,
	 output		  phase_done,
	 
	 /* Is DCM Locked? */
	 output		  dcm_locked
    );
	 
	 wire ADC_clk_extsrc;
	 wire ADC_clk_src;	 
	 wire ADC_clk_sample;
	 
	 wire dcm_psen;
	 wire dcm_psincdec;
	 wire dcm_psdone;
	 wire [7:0] dcm_status;

	 dcm_phaseshift_interface dcmps(.clk_i(phase_clk),
											  .reset_i(reset),
											  .default_value_i(9'd0),
											  .value_i(phase_requested),
											  .load_i(phase_load),
											  .value_o(phase_actual),
											  .done_o(phase_done),
											  .dcm_psen_o(dcm_psen),
											  .dcm_psincdec_o(dcm_psincdec),
											  .dcm_psdone_i(dcm_psdone),
											  .dcm_status_i(dcm_status));	

	// BUFGMUX: Global Clock Mux Buffer
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	BUFGMUX #(
	.CLK_SEL_TYPE("SYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	)
	BUFGMUX_inst (
	.O(ADC_clk_src), // 1-bit output: Clock buffer output
	.I0(clk_100mhz), // 1-bit input: Clock buffer input (S=0)
	.I1(ADC_clk_extsrc), // 1-bit input: Clock buffer input (S=1)
	.S(use_ext_clk) // 1-bit input: Clock buffer select
	);
	// End of BUFGMUX_inst instantiation

	wire dcm_locked_int;
	wire dcm_clk;
	
	//dcm_locked_int may be high if clock is removed, so also check clkfx output is toggling
	assign dcm_locked = dcm_locked_int & (~dcm_status[2]);

	// DCM_SP: Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	DCM_SP #(
	.CLKFX_DIVIDE(1), // Divide value on CLKFX outputs - D - (1-32)
	.CLKFX_MULTIPLY(4), // Multiply value on CLKFX outputs - M - (2-32)
	.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
	.CLKIN_PERIOD(10.0), // Input clock period specified in nS
	.CLKOUT_PHASE_SHIFT("VARIABLE"), // Output phase shift (NONE, FIXED, VARIABLE)
	.CLK_FEEDBACK("2X"), // Feedback source (NONE, 1X, 2X)
	.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
	.PHASE_SHIFT(0), // Amount of fixed phase shift (-255 to 255)
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
	)
	DCM_extclock_gen (
	.CLK2X(dcm_clk),
	.CLK0(),
	.CLK180(),
	.CLKFX(ADC_clk_extsrc), // 1-bit output: Digital Frequency Synthesizer output (DFS)
	.LOCKED(dcm_locked_int), // 1-bit output: DCM_SP Lock Output
	.PSDONE(dcm_psdone), // 1-bit output: Phase shift done output
	.STATUS(dcm_status), // 8-bit output: DCM_SP status output
	.CLKFB(dcm_clk), // 1-bit input: Clock feedback input
	.CLKIN(ext_clk), // 1-bit input: Clock input
	.PSCLK(phase_clk), // 1-bit input: Phase shift clock input
	.PSEN(dcm_psen), // 1-bit input: Phase shift enable
	.PSINCDEC(dcm_psincdec), // 1-bit input: Phase shift increment/decrement input
	.RST(reset) // 1-bit input: Active high reset input
	);

/*
	assign ADC_clk_extsrc = ext_clk;
*/
	
	// DCM_SP: Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	DCM_SP #(
	.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
	.CLKIN_PERIOD(10.0), // Input clock period specified in nS
	.CLKOUT_PHASE_SHIFT("FIXED"), // Output phase shift (NONE, FIXED, VARIABLE)
	.CLK_FEEDBACK("1X"), // Feedback source (NONE, 1X, 2X)
	.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
	.PHASE_SHIFT(50), // Amount of fixed phase shift (-255 to 255)
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
	)
	DCM_ADC_Sample_delay (
	.CLK0(ADC_clk_sample), // 1-bit output: 0 degree clock output
	//.LOCKED(locked), // 1-bit output: DCM_SP Lock Output
	.CLKFB(ADC_clk_sample), // 1-bit input: Clock feedback input
	.CLKIN(ADC_clk_src), // 1-bit input: Clock input
	.RST(reset) // 1-bit input: Active high reset input
	);		
	
	assign systemsample_clk = ADC_clk_sample;

	//Output clock using DDR2 block (recommended for Spartan-6 device)
	ODDR2 #(
		// The following parameters specify the behavior
		// of the component.
		.DDR_ALIGNMENT("NONE"), // Sets output alignment
										// to "NONE", "C0" or "C1"
		.INIT(1'b0),    // Sets initial state of the Q 
							 //   output to 1'b0 or 1'b1
		.SRTYPE("SYNC") // Specifies "SYNC" or "ASYNC"
							 //   set/reset
	)
	ODDR2_inst (
		.Q(adc_clk),   // 1-bit DDR output data
		.C0(ADC_clk_src), // 1-bit clock input
		.C1(~ADC_clk_src), // 1-bit clock input
		.CE(1'b1), // 1-bit clock enable input
		.D0(1'b1), // 1-bit data input (associated with C0)
		.D1(1'b0), // 1-bit data input (associated with C1)
		.R(1'b0),   // 1-bit reset input
		.S(1'b0)    // 1-bit set input
	);


endmodule
