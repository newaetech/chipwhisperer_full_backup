`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the ChipWhisperer Clock Glitcher registers

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
module reg_clockglitch(
	input 			reset_i,
	input 			clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	output [7:0]   reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output [15:0]  reg_hyplen,

	input wire 		sourceclk0,
	input wire     sourceclk1,
	
	output wire    glitchclk,
	input wire     exttrigger
   );
	 
	 wire	  reset;
	 assign reset = reset_i;
	 assign reg_stream = 1'b0;

`ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
`endif
        	  
	 			  
	/* Controls width of pulse */
	 wire [8:0] phase1_requested;
	 wire [8:0] phase1_actual;
	 wire		   phase1_load;
	 wire		   phase1_done;

	 /* Controls delay between falling edge of glitch & risinge edge of clock */
	 wire [8:0] phase2_requested;
	 wire [8:0] phase2_actual;
	 wire		   phase2_load;
	 wire		   phase2_done;			
			  
	 `define CLOCKGLITCH_SETTINGS	51
	 `define CLOCKGLITCH_LEN      8
	 
	 reg [63:0]  clockglitch_settings_reg;
	 wire [63:0] clockglitch_settings_read; 	 	 
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `CLOCKGLITCH_SETTINGS: reg_hyplen_reg <= `CLOCKGLITCH_LEN;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;
	 	 
	 wire [8:0] glitchwidth_phase;
	 wire 		glitchwidth_load;
	 
	 wire [8:0] glitchdelay_phase;
	 wire       glitchdelay_load;
	 
	 reg [8:0] phase2_in;
	 reg [8:0] phase1_in;
	 
	 wire dcm1_locked;
	 wire dcm2_locked;
	 
	 reg phase2_done_reg;
	 reg phase1_done_reg;
	 
	 assign phase1_requested = clockglitch_settings_reg[8:0];
	 assign phase2_requested = clockglitch_settings_reg[17:9];
	 
	 wire dcm_rst;

/*
	 Clock-glitch settings main registers (address 51)
	 [8..0]  = Glitch Offset Fine Phase 
	 [17..9] = Glitch Width Fine Phase
	 [18] = Load Phases
	 [..19] =  Glitch Offset current setting
	 [36..] =  Glitch Width current setting
	 [37] = Offset Fine loaded
	 [38] = Width Fine loaded
	 [39] (Byte 4, Bit 7)  = Offset DCM Locked
	 [40] (Byte 5, Bit 0)  = Width DCM Locked	 
	 [41] (Byte 5, Bit 1)  = DCM Reset
	 [43..42] (Byte 5, Bit [3..2]) = Glitch trigger source
	      00 = Manual
			01 = Capture Trigger (with offset)
			10 = Continous
	 
	 [46..44] (Byte 5, Bit [6..4]) = Glitch Type
			000 = Glitch is XORd with Clock (Positive or Negative going glitch)
		   001 = Glitch is ORd with Clock (Positive going glitch only)
			010 = Glitch Only
			011 = Clock Only	 

	 [47] (Byte 5, Bit 7) = Manual Glitch. Set to 1 then 0, glitch on rising edge
	 
	 [55..48] (Byte 6, Bits [7..0])
	      Cycles to glitch-1 (e.g. 0 means 1 glitch)
			
	 [57..56] (Byte 7, But [1..0]) = Glitch Clock Source
	       00 = Source 0
			 01 = Source 1	      
*/	
	 
	 wire [2:0] glitch_type;
	 assign glitch_type = clockglitch_settings_reg[46:44];
	 wire [1:0] glitch_trigger_src;
	 assign glitch_trigger_src = clockglitch_settings_reg[43:42];
	 
	 wire [7:0] max_glitches;
	 assign max_glitches = clockglitch_settings_reg[55:48];
	 
	 wire       sourceclk;
	 assign sourceclk = (clockglitch_settings_reg[57:56] == 1'b01) ? sourceclk1 :
								sourceclk0;
							  //(clockglitch_settings_reg[57:56] == 1'b01) ? sourceclk1
	 
	 wire manual;
	 assign manual = clockglitch_settings_reg[47];
	 reg manual_dly;
	 always @(posedge sourceclk)
		manual_dly <= manual;
		
	 reg exttrigger_resync;
	 reg exttrigger_resync_dly;
	 always @(posedge sourceclk)
		exttrigger_resync <= exttrigger;
	 
	 always @(posedge sourceclk)
		exttrigger_resync_dly <= exttrigger_resync;
	 
	 reg glitch_trigger;
	 always @(posedge sourceclk) begin
		if (glitch_trigger_src == 2'b10)
			glitch_trigger <= 1'b1;
		else if (glitch_trigger_src == 2'b00)
			glitch_trigger <= manual & ~manual_dly;
		else if (glitch_trigger_src == 2'b01)
			glitch_trigger <= exttrigger_resync & ~exttrigger_resync_dly;
		else
			glitch_trigger <= 1'b0;
	 end	 
	
	 reg [8:0] glitch_cnt;
	 reg glitch_go;
	 always @(posedge sourceclk) begin
		if (glitch_trigger)
			glitch_go <= 'b1;
	 	else if (glitch_cnt >= max_glitches)
			glitch_go <= 'b0;
	 end
	
	 always @(posedge sourceclk) begin
		if (glitch_go)
			glitch_cnt <= glitch_cnt + 8'd1;
		else
			glitch_cnt <= 0;
	 end
	 
	 assign clockglitch_settings_read[18:0] = clockglitch_settings_reg[18:0];
	 assign clockglitch_settings_read[36:19] = {phase2_actual, phase1_actual};
	 assign clockglitch_settings_read[37] = phase1_done_reg;
	 assign clockglitch_settings_read[38] = phase2_done_reg;
	 assign clockglitch_settings_read[39] = dcm1_locked;
	 assign clockglitch_settings_read[40] = dcm2_locked;
	 assign clockglitch_settings_read[63:41] = clockglitch_settings_reg[63:41];
	 assign dcm_rst = clockglitch_settings_reg[41];	 	 
	
	 always @(posedge clk) begin
		if (phase1_load)
			phase1_done_reg <= 'b0;
		else if (phase1_done)
			phase1_done_reg <= 'b1;
	 end
			
	 always @(posedge clk) begin
		if (phase2_load)
			phase2_done_reg <= 'b0;
		else if (phase2_done)
			phase2_done_reg <= 'b1;
	 end	 
			 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`CLOCKGLITCH_SETTINGS: begin reg_datao_reg <= clockglitch_settings_read[reg_bytecnt*8 +: 8]; end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 assign phase2_load  = clockglitch_settings_reg[18];
	 assign phase1_load  = clockglitch_settings_reg[18];
	 
	 always @(posedge clk) begin
		if (reset) begin
			clockglitch_settings_reg <= 0;
			
		end else if (clockglitch_settings_reg[18]) begin
			clockglitch_settings_reg[18] <= 0;
			
		end else if (reg_write) begin
			case (reg_address)
				`CLOCKGLITCH_SETTINGS: clockglitch_settings_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
				default: ;
			endcase
		end
	 end	 	 
			 
	 /* Glitch Delay */

	 /* Glitch Hardware */
	 clockglitch_s6 gc(
		.source_clk(sourceclk),
		.glitched_clk(glitchclk),
		.glitch_next(glitch_go),
		.glitch_type(glitch_type),
		.phase_clk(clk),
		.dcm_rst(dcm_rst),
		.phase1_requested(phase1_requested),
		.phase1_actual(phase1_actual),
		.phase1_load(phase1_load),
		.phase1_done(phase1_done),
		.dcm1_locked(dcm1_locked),
		.phase2_requested(phase2_requested),
		.phase2_actual(phase2_actual),
		.phase2_load(phase2_load),
		.phase2_done(phase2_done),
		.dcm2_locked(dcm2_locked)
	);
	
endmodule

`undef CHIPSCOPE
