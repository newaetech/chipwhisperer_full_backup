`timescale 1ns / 1ps
`include "includes.v"
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
module clock_managment_advanced(
	 input		  reset, //Does NOT reset CLKGEN block

	 /* Clock sources */
    input 		  clk_sys,     //System clock
    input 		  clk_ext,     //External clock
	 
	 /* Clock to ADC */
	 output 		  adc_clk,    //Output clock to ADC
`ifdef ADCCLK_FEEDBACK
	 input		  adc_clk_feedback,
`endif
	 
	 /* Clock to DUT */
	 output		  target_clk,
	 
	 /* Clock selection */
	 input [2:0]  clkadc_source,
	 input		  clkgen_source,
	 
	 /* Clock to System compensates for wire delay of ADC clock */
	 output       systemsample_clk,
	 
	 /* Mul/Div Control for Generated clock */
	 input		  clkgen_reset,
	 input [7:0]  clkgen_mul,
	 input [7:0]  clkgen_div,
	 input		  clkgen_load,
	 output		  clkgen_done,
	 
	 /* Phase shift control for external clock*/
	 input        phase_clk,
	 input [8:0]  phase_requested,
	 output [8:0] phase_actual,
	 input		  phase_load,
	 output		  phase_done,	 
	 /* Is Selected DCM Locked? */
	 output		  dcm_adc_locked,
	 output		  dcm_gen_locked 
    );
	 
	 wire ADC_clk_extsrc;
	 wire ADC_clk_src;	 
	 wire ADC_clk_sample;
	 
	 wire dcm_psen;
	 wire dcm_psincdec;
	 wire dcm_psdone;
	 wire [7:0] dcm_status;

	 /* Resync if needed */
	 //NOTE: Due to requirement of PHASECLK location in DCM_CLKGEN
	 //      instance, ISE does some routing which can result in an
	 //      error about clock & non-clock loads. Need to look into that.
	 wire phase_clk_buf = phase_clk;

	 dcm_phaseshift_interface dcmps(.clk_i(phase_clk_buf),
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

	 //NOTE: Due to requirement of PHASECLK location in DCM_CLKGEN
	 //      instance, ISE does some routing which can result in an
	 //      error about clock & non-clock loads. Need to look into that.
	 wire clkgen_clk;
	 assign clkgen_clk = phase_clk;
	 
	 wire clkgen_progdone;
	 wire clkgen_progdata;
	 wire clkgen_progen;
	 wire clkgen_progclk;
	 
	 dcm_clkgen_load clkgenload(
		.clk_i(clkgen_clk),
		.reset_i(clkgen_reset),
		.mult(clkgen_mul),
		.div(clkgen_div),
		.load(clkgen_load),
		.done(clkgen_done),
		.PROGDONE(clkgen_progdone),
		.PROGDATA(clkgen_progdata),
		.PROGEN(clkgen_progen),
		.PROGCLK(clkgen_progclk)
    );


	wire clkb;
	wire clkgenfx_in;
	 	 
	wire dcm_clk_in;
	wire clkgenfx_out;
	 
        `ifndef __ICARUS__
            BUFGMUX #(
            .CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
            )
            clkdcm_mux (
            .O(dcm_clk_in), // 1-bit output: Clock buffer output
            .I0(clkgenfx_out), // 1-bit input: Clock buffer input (S=0)
            .I1(clk_ext), // 1-bit input: Clock buffer input (S=1)
            .S(clkadc_source[2]) // 1-bit input: Clock buffer select
            );	  
             
             
            BUFGMUX #(
            .CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
            )
            clkgenfx_mux (
            .O(clkgenfx_in), // 1-bit output: Clock buffer output
            .I0(clk_sys), // 1-bit input: Clock buffer input (S=0)
            .I1(clk_ext), // 1-bit input: Clock buffer input (S=1)
            .S(clkgen_source) // 1-bit input: Clock buffer select
            ); 
        `endif

	wire dcm_locked_int;
	wire dcm_clk;
	
	//dcm_locked_int may be high if clock is removed, so also check clkfx output is toggling
	assign dcm_adc_locked = dcm_locked_int & (~dcm_status[2]);

	wire ADC_clk_times4;
	wire ADC_clk;

	// DCM_SP: Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
        `ifndef __ICARUS__
	DCM_SP #(
	.CLKFX_DIVIDE(1), // Divide value on CLKFX outputs - D - (1-32)
	.CLKFX_MULTIPLY(4), // Multiply value on CLKFX outputs - M - (2-32)
	.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
	//.CLKIN_PERIOD(10.0), // Input clock period specified in nS
	.CLKOUT_PHASE_SHIFT("VARIABLE"), // Output phase shift (NONE, FIXED, VARIABLE)
	.CLK_FEEDBACK("2X"), // Feedback source (NONE, 1X, 2X)
	.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
	.PHASE_SHIFT(0), // Amount of fixed phase shift (-255 to 255)
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
	)
	DCM_extclock_gen (
	.CLK2X(dcm_clk),
	.CLK0(ADC_clk),
	.CLK2X180(),
	.CLK90(),
	.CLK180(),
	.CLK270(),
	.CLKFX(ADC_clk_times4), // 1-bit output: Digital Frequency Synthesizer output (DFS)
	.CLKFX180(),
	.CLKDV(), //TODO: Use this output for additional options
	.LOCKED(dcm_locked_int), // 1-bit output: DCM_SP Lock Output
	.PSDONE(dcm_psdone), // 1-bit output: Phase shift done output
	.STATUS(dcm_status), // 8-bit output: DCM_SP status output
	.CLKFB(dcm_clk), // 1-bit input: Clock feedback input
	.CLKIN(dcm_clk_in), // 1-bit input: Clock input
	.PSCLK(phase_clk_buf), // 1-bit input: Phase shift clock input
	.PSEN(dcm_psen), // 1-bit input: Phase shift enable
	.PSINCDEC(dcm_psincdec), // 1-bit input: Phase shift increment/decrement input
	.RST(reset) // 1-bit input: Active high reset input
	);
        `endif

	wire clkgenfx_dev_out;
	wire dcm2_locked_int;
	wire [2:1] dcm2_status;
	assign dcm_gen_locked = dcm2_locked_int & (~dcm2_status[2]);

	// DCM_CLKGEN: Frequency Aligned Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 14.3
        `ifndef __ICARUS__
	DCM_CLKGEN #(
	.CLKFXDV_DIVIDE(4), // CLKFXDV divide value (2, 4, 8, 16, 32)
	.CLKFX_DIVIDE(2), // Divide value - D - (1-256)
	.CLKFX_MULTIPLY(2), // Multiply value - M - (2-256)
	.CLKFX_MD_MAX(1.0), // Specify maximum M/D ratio for timing anlysis
	//.CLKIN_PERIOD(10.0), // Input clock period specified in nS
	.SPREAD_SPECTRUM("NONE"), // Spread Spectrum mode "NONE"
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_CLKGEN LOCKED
	)
	DCM_CLKGEN_inst (
	.CLKFX(clkgenfx_out), // 1-bit output: Generated clock output
	.CLKFX180(), // 1-bit output: Generated clock output 180 degree out of phase from CLKFX.	
	.CLKFXDV(), // 1-bit output: Divided clock output
	.LOCKED(dcm2_locked_int), // 1-bit output: Locked output
	.PROGDONE(clkgen_progdone), // 1-bit output: Active high output to indicate the successful re-programming
	.STATUS(dcm2_status), // 2-bit output: DCM_CLKGEN status
	.CLKIN(clkgenfx_in), // 1-bit input: Input clock
	.FREEZEDCM(1'b0), // 1-bit input: Prevents frequency adjustments to input clock
	.PROGCLK(clkgen_progclk), // 1-bit input: Clock input for M/D reconfiguration
	.PROGDATA(clkgen_progdata), // 1-bit input: Serial data input for M/D reconfiguration
	.PROGEN(clkgen_progen), // 1-bit input: Active high program enable
	.RST(clkgen_reset) // 1-bit input: Reset input pin
	);
        `endif

	
	assign target_clk = clkgenfx_out;
	
	//Output buffers
	wire out_from_dcmmux;
        `ifndef __ICARUS__
	BUFGMUX #(
	.CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	)
	adcclk_mux (
	.O(ADC_clk_sample), // 1-bit output: Clock buffer output
	.I0(out_from_dcmmux), // 1-bit input: Clock buffer input (S=0)
	.I1(clk_ext), // 1-bit input: Clock buffer input (S=1)
	.S(clkadc_source[0]) // 1-bit input: Clock buffer select
	);	
		
	 BUFGMUX #(
	.CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	)
	adcclk_0_mux (
	.O(out_from_dcmmux), // 1-bit output: Clock buffer output
	.I0(ADC_clk_times4), // 1-bit input: Clock buffer input (S=0)
	.I1(ADC_clk), // 1-bit input: Clock buffer input (S=1)
	.S(clkadc_source[1]) // 1-bit input: Clock buffer select
	);	
		
	assign ADC_clk_src = ADC_clk_sample;
	
`ifdef ADCCLK_FEEDBACK
	IBUFG IBUFG_inst (
	.O(systemsample_clk),
	.I(adc_clk_feedback) );
`else
	assign systemsample_clk = ADC_clk_sample;
`endif

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
        `endif // __ICARUS__

endmodule
