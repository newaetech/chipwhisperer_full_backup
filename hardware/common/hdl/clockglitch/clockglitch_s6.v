`timescale 1ns / 1ps
`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at https://github.com/newaetech/chipwhisperer .

Copyright (c) 2013-2020, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
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

module clockglitch_s6(
	 /* Source Clock */
	 input wire	  source_clk,
 
    /* Glitchy Clock */
	 output wire  glitched_clk,
	 
	 /* Glitch request */
	 input wire        glitch_next,
	 input wire [2:0]  glitch_type,
	  /*
			000 = Glitch is XORd with Clock (Positive or Negative going glitch)
		   001 = Glitch is ORd with Clock (Positive going glitch only)
			010 = Glitch Only
			011 = Clock Only	 
			100 = Glitch only, based on enable reg only (output active during entire clock cycle)
	 */
 
	 /* ??? */
	 input wire   phase_clk,
	 input wire   dcm_rst,
 
	 /* Controls width of pulse */
	 input wire [8:0]  phase1_requested,
	 output wire [8:0] phase1_actual,
	 input wire		  	 phase1_load,
	 output wire		 phase1_done,
	 output wire       dcm1_locked,

	 /* Controls delay between falling edge of glitch & risinge edge of clock */
	 input wire [8:0]  phase2_requested,
	 output wire [8:0] phase2_actual,
	 input wire		  	 phase2_load,
	 output wire		 phase2_done,
	 output wire       dcm2_locked
    );


	//NOTE: Due to requirement of PHASECLK location in DCM_CLKGEN
	 //      instance, ISE does some routing which can result in an
	 //      error about clock & non-clock loads. Need to look into that.
	 wire phase_clk_buf;
	 assign phase_clk_buf = phase_clk;
	 
	 wire dcm1_psen;
	 wire dcm1_psincdec;
	 wire dcm1_psdone;
	 wire [7:0] dcm1_status;
	
	 dcm_phaseshift_interface dcmps1(.clk_i(phase_clk_buf),
											  .reset_i(dcm_rst),
											  .default_value_i(9'd0),
											  .value_i(phase1_requested),
											  .load_i(phase1_load),
											  .value_o(phase1_actual),
											  .done_o(phase1_done),
											  .dcm_psen_o(dcm1_psen),
											  .dcm_psincdec_o(dcm1_psincdec),
											  .dcm_psdone_i(dcm1_psdone),
											  .dcm_status_i(dcm1_status));	

	 wire dcm2_psen;
	 wire dcm2_psincdec;
	 wire dcm2_psdone;
	 wire [7:0] dcm2_status;
	 
	 dcm_phaseshift_interface dcmps2(.clk_i(phase_clk_buf),
											  .reset_i(dcm_rst),
											  .default_value_i(9'd0),
											  .value_i(phase2_requested),
											  .load_i(phase2_load),
											  .value_o(phase2_actual),
											  .done_o(phase2_done),
											  .dcm_psen_o(dcm2_psen),
											  .dcm_psincdec_o(dcm2_psincdec),
											  .dcm_psdone_i(dcm2_psdone),
											  .dcm_status_i(dcm2_status));	

	wire dcm1_locked_int;
	wire dcm2_locked_int;
	
	//Cannot monitor status[2] bit if not using clkfx output
	assign dcm1_locked = dcm1_locked_int; // & (~dcm1_status[2]);
	assign dcm2_locked = dcm2_locked_int; //& (~dcm2_status[2]);

	wire dcm1_clk;
	wire dcm2_clk;
	wire dcm1_clk_out;
	wire dcm2_clk_out;
	
	wire glitchstream;
		
	reg glitch_next_reg;
	reg glitch_next_reg1;
	//Need to think carefully about which clock to syncronize this too, and
	//which edge. Lots of trouble as different options on outputs & adjustable
	//phase
	always @(negedge dcm1_clk_out) begin
		glitch_next_reg1 <= glitch_next;
		glitch_next_reg <= glitch_next_reg1;
	end
	
	clk2glitch clk2glitch_inst(
		.clk1(dcm1_clk_out),
		.clk2(dcm2_clk_out),
		.enable(glitch_next_reg),
		.glitchout(glitchstream)
	);
	
	assign glitched_clk = (glitch_type == 3'b000) ? source_clk ^ glitchstream :
	                      (glitch_type == 3'b001) ? source_clk | glitchstream :
								 (glitch_type == 3'b010) ? glitchstream :
								 (glitch_type == 3'b011) ? source_clk :
								 (glitch_type == 3'b100) ? glitch_next_reg :
								 1'b0;
			
	// DCM_SP: Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	DCM_SP #(
	.CLKFX_DIVIDE(2), // Divide value on CLKFX outputs - D - (1-32)
	.CLKFX_MULTIPLY(2), // Multiply value on CLKFX outputs - M - (2-32)
	.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
	.CLKIN_PERIOD(15.0), // Input clock period specified in nS
	.CLKOUT_PHASE_SHIFT("VARIABLE"), // Output phase shift (NONE, FIXED, VARIABLE)
	.CLK_FEEDBACK("2X"), // Feedback source (NONE, 1X, 2X)
	.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
	.PHASE_SHIFT(0), // Amount of fixed phase shift (-255 to 255)
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
	)
	DCM_extclock_gen (
	.CLK2X(dcm1_clk),
	.CLK0(),
	.CLK2X180(),
	.CLK90(),
	.CLK180(dcm1_clk_out),
	.CLK270(),
	.CLKFX(),
	.CLKFX180(),
	.CLKDV(),
	.LOCKED(dcm1_locked_int), // 1-bit output: DCM_SP Lock Output
	.PSDONE(dcm1_psdone), // 1-bit output: Phase shift done output
	.STATUS(dcm1_status), // 8-bit output: DCM_SP status output
	.CLKFB(dcm1_clk), // 1-bit input: Clock feedback input
	.CLKIN(source_clk), // 1-bit input: Clock input
	.PSCLK(phase_clk_buf), // 1-bit input: Phase shift clock input
	.PSEN(dcm1_psen), // 1-bit input: Phase shift enable
	.PSINCDEC(dcm1_psincdec), // 1-bit input: Phase shift increment/decrement input
	.RST(dcm_rst) // 1-bit input: Active high reset input
	);


	// DCM_SP: Digital Clock Manager
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	DCM_SP #(
	.CLKFX_DIVIDE(2), // Divide value on CLKFX outputs - D - (1-32)
	.CLKFX_MULTIPLY(2), // Multiply value on CLKFX outputs - M - (2-32)
	.CLKIN_DIVIDE_BY_2("FALSE"), // CLKIN divide by two (TRUE/FALSE)
	.CLKIN_PERIOD(15.0), // Input clock period specified in nS
	.CLKOUT_PHASE_SHIFT("VARIABLE"), // Output phase shift (NONE, FIXED, VARIABLE)
	.CLK_FEEDBACK("2X"), // Feedback source (NONE, 1X, 2X)
	.DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
	.PHASE_SHIFT(63), // Amount of fixed phase shift (-255 to 255)
	.STARTUP_WAIT("FALSE") // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
	)
	DCM_extclock_gen2 (
	.CLK2X(dcm2_clk),
	.CLK0(dcm2_clk_out),
	.CLK2X180(),
	.CLK90(),
	.CLK180(), //dcm2_clk_out
	.CLK270(),
	.CLKFX(),
	.CLKFX180(),
	.CLKDV(),
	.LOCKED(dcm2_locked_int), // 1-bit output: DCM_SP Lock Output
	.PSDONE(dcm2_psdone), // 1-bit output: Phase shift done output
	.STATUS(dcm2_status), // 8-bit output: DCM_SP status output
	.CLKFB(dcm2_clk), // 1-bit input: Clock feedback input
	.CLKIN(dcm1_clk_out), // 1-bit input: Clock input
	.PSCLK(phase_clk_buf), // 1-bit input: Phase shift clock input
	.PSEN(dcm2_psen), // 1-bit input: Phase shift enable
	.PSINCDEC(dcm2_psincdec), // 1-bit input: Phase shift increment/decrement input
	.RST(dcm_rst) // 1-bit input: Active high reset input
	);

endmodule

module clk2glitch(
	 input wire	  clk1,

	 input wire   clk2,
	 
	 input wire   enable,
	 
	 output wire  glitchout
	 );
	 
	 wire clk1buf, clk2buf;
	 
	 //If using BUFG - not used as requires too many resources, not sure if
	 //actually required anyway...
	 //BUFG bufclk1 (.I(clk1), .O(clk1buf));
	 //BUFG bufclk2 (.I(clk2), .O(clk2buf));
	 
	 //Otherwise	 
	 assign clk1buf = clk1;
	 assign clk2buf = clk2;	 
	 
	 assign glitchout = (clk1buf & clk2buf) & enable;
	 
endmodule
 