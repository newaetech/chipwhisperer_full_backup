`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
or the codebase at https://github.com/newaetech/chipwhisperer .

This file is the ChipWhisperer Clock Glitcher registers

Copyright (c) 2013-2020, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This file is released under the 2-Clause BSD License:

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
	input wire     exttrigger,
	
	output wire		dcm_unlocked,
	output wire    led_glitch
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
	 `define CLOCKGLITCH_OFFSET    25
	 `define CLOCKGLITCH_OFFSET_LEN 8
	 
`ifdef SUPPORT_GLITCH_READBACK
	 `define GLITCHCYCLES_CNT 19
	 `define GLITCHCYCLES_CNT_LEN 4
	 `define GLITCH_RECONFIG_RB_ADDR 56
	 `define GLITCH_RECONFIG_RB_LEN 16
	  reg [127:0] clockglitch_readback_reg;
`endif
	 
	 // @ Address 56, 8 bytes (+8 extra reserved for future...)
	 //
	 // [1 .. 0 ] = Offset (DCM Load Value, MSB = 1, LSB=0)
	 // [3 .. 2 ] = Width (DCM Load Value, MSB=3, LSB=2)
	 // [4      ] = Offset (%, integer)
	 // [5      ] = Offset (%, fraction)
	 // [6      ] = Width (%, integer)
	 // [7      ] = Width (%, fraction)
	 //
	 // i.e., 5.6% = 5 (integer) + 60 (fraction)
	 //
	 //
	 
	 
	 
	 reg [63:0]  clockglitch_settings_reg;
	 wire [63:0] clockglitch_settings_read; 	

	 reg [31:0] clockglitch_offset_reg;
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `CLOCKGLITCH_SETTINGS: reg_hyplen_reg <= `CLOCKGLITCH_LEN;
				`CLOCKGLITCH_OFFSET: reg_hyplen_reg <= `CLOCKGLITCH_OFFSET_LEN;
`ifdef SUPPORT_GLITCH_READBACK
				`GLITCHCYCLES_CNT: reg_hyplen_reg <= `GLITCHCYCLES_CNT_LEN;
				`GLITCH_RECONFIG_RB_ADDR: reg_hyplen_reg <= `GLITCH_RECONFIG_RB_LEN;
`endif
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
	 
	 assign dcm_unlocked = ~(dcm1_locked & dcm2_locked);	 

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
			01 = Capture Trigger (with offset, continous)
			10 = Continous
			11 = Capture Trigger (with offset, single-shot when manual glitch is '1')
	 
	 [46..44] (Byte 5, Bit [6..4]) = Glitch Type
			000 = Glitch is XORd with Clock (Positive or Negative going glitch)
		   001 = Glitch is ORd with Clock (Positive going glitch only)
			010 = Glitch Only
			011 = Clock Only	 
			100 = Glitch only based on enable, does not use phase-based difference.
			      Can generate very long glitch pulses.

	 [47] (Byte 5, Bit 7) = Manual Glitch. Set to 1 then 0, glitch on rising edge
	 
	 [55..48] (Byte 6, Bits [7..0])
	      Cycles to glitch-1 (e.g. 0 means 1 glitch) (lower 8 bits)
			
	 [57..56] (Byte 7, Bits [1..0]) = Glitch Clock Source
	       00 = Source 0
			 01 = Source 1
			 
             **THE FOLLOWING TOP 5-BIT GLITCH WAS ADDED in 2020 - Only on release 5.1.4 or later**
	 [62..58] (Byte 7, Bits [6..2]) = Cycles to glitch (top 5 bits)
     [63] (Byte 7, Bit 7) = Unused (reads as 0, used to later expand if needed)
	 
	 
*/	
	 
	 wire [2:0] glitch_type;
	 assign glitch_type = clockglitch_settings_reg[46:44];
	 wire [1:0] glitch_trigger_src;
	 assign glitch_trigger_src = clockglitch_settings_reg[43:42];
	 
	 wire [12:0] max_glitches;
	 assign max_glitches = {clockglitch_settings_reg[62:58], clockglitch_settings_reg[55:48]};
	 
	 wire       sourceclk;
	 assign sourceclk = (clockglitch_settings_reg[57:56] == 1'b01) ? sourceclk1 :
								sourceclk0;
							  //(clockglitch_settings_reg[57:56] == 1'b01) ? sourceclk1	 
	 reg manual;
	 always @(posedge clk)
		manual <= clockglitch_settings_reg[47];
	 
	 reg manual_rs1, manual_rs2;	
	 reg manual_dly;
	 always @(posedge sourceclk) begin
		//Resync with double-FF
		manual_rs1 <= manual;
		manual_rs2 <= manual_rs1;
		
		//Use delay to convert to single pulse in sourceclk domain
		manual_dly <= ~manual_rs2;
	 end
		 
	 reg glitch_trigger;
	 wire exttrigger_resync;
	 //reg exttrigger_resync_dly;
	 
	 reg oneshot;
	 
	 always @(posedge sourceclk) begin
		if (glitch_trigger_src == 2'b10)
			glitch_trigger <= 1'b1;
		else if (glitch_trigger_src == 2'b00)
			glitch_trigger <= manual_rs2 & manual_dly;
		else if (glitch_trigger_src == 2'b01)
			glitch_trigger <= exttrigger_resync; //exttrigger_resync & ~exttrigger_resync_dly;
		else if (glitch_trigger_src == 2'b11)
			glitch_trigger <= exttrigger_resync & oneshot;
	 end	 
	 
	 always @(posedge sourceclk) begin
		if (manual_rs2 & manual_dly)
			oneshot <= 1'b1;
		else if (exttrigger_resync)
			oneshot <= 1'b0;		
	 end
	
	 reg [12:0] glitch_cnt;
	 reg glitch_go;
	 always @(posedge sourceclk) begin
		if (glitch_trigger)
			glitch_go <= 'b1;
	 	else if (glitch_cnt >= max_glitches)
			glitch_go <= 'b0;
	 end
	
	 always @(posedge sourceclk) begin
		if (glitch_go)
			glitch_cnt <= glitch_cnt + 13'd1;
		else
			glitch_cnt <= 0;
	 end
	 	 
	 reg [31:0] clockglitch_cnt;
	 reg clockglitch_cnt_rst;
	 always @(posedge sourceclk) begin
		/*if ((clockglitch_cnt_rst == 1'b1) || (reset == 1'b1))
				clockglitch_cnt <= 32'd0;
		else*/ if (glitch_go)
				clockglitch_cnt <= clockglitch_cnt + 32'd1;
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
			 
	 wire [63:0] clockglitch_offset_read_reg;
	 assign clockglitch_offset_read_reg[31:0] = clockglitch_offset_reg;
			 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`CLOCKGLITCH_SETTINGS: begin reg_datao_reg <= clockglitch_settings_read[reg_bytecnt*8 +: 8]; end
				`CLOCKGLITCH_OFFSET: begin reg_datao_reg <= clockglitch_offset_read_reg[reg_bytecnt*8 +: 8]; end
`ifdef SUPPORT_GLITCH_READBACK
				`GLITCH_RECONFIG_RB_ADDR: begin reg_datao_reg <= clockglitch_readback_reg[reg_bytecnt*8 +: 8]; end
				`GLITCHCYCLES_CNT: begin reg_datao_reg <= clockglitch_cnt[reg_bytecnt*8 +: 8]; end
`endif
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 /* Know when all settings have been written successfully */
	 /*
	 always @(posedge clk) begin
		if ((reg_write) && (reg_address == `CLOCKGLITCH_SETTINGS)) begin
			if (reg_bytecnt == 16'd7) begin
				regwrite_done <= 1'b1;			
			end else begin
				regwrite_done <= 1'b0;
			end
		end
	end
	*/
	 
	 assign phase2_load  = clockglitch_settings_reg[18];
	 assign phase1_load  = clockglitch_settings_reg[18];
	 
	 always @(posedge clk) begin
		if (reset) begin
			clockglitch_settings_reg <= 0;
			clockglitch_offset_reg <= 0;
			clockglitch_cnt_rst <= 0;
`ifdef SUPPORT_GLITCH_READBACK
			clockglitch_readback_reg <= {8'd0, 8'd10, 8'd0, 8'd10, 16'd0, 16'd0};
`endif
		end else if (clockglitch_settings_reg[18]) begin
			clockglitch_settings_reg[18] <= 0;
			
		end else if (reg_write) begin
			case (reg_address)
				`CLOCKGLITCH_SETTINGS: clockglitch_settings_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
				`CLOCKGLITCH_OFFSET: clockglitch_offset_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
`ifdef SUPPORT_GLITCH_READBACK
				`GLITCHCYCLES_CNT: clockglitch_cnt_rst <= reg_datai[0];
				`GLITCH_RECONFIG_RB_ADDR: clockglitch_readback_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
`endif
				default: ;
			endcase
		end
	 end	 	 

//   always @(posedge sourceclk)
//	  exttrigger_resync_dly <= exttrigger_resync;

	trigger_resync resync(
	.reset(reset),
	.clk(sourceclk),
	.exttrig(exttrigger),	
	.offset(clockglitch_offset_reg),
	.exttrigger_resync(exttrigger_resync)
	);

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
	
	/* LED lighty up thing */
	reg [18:0] led_extend;
	reg led_on;
	always @(posedge sourceclk) begin
		if (glitch_go) begin
			led_extend <= 0;			
		end else if (led_on == 1'b1) begin
			led_extend <= led_extend + 19'b1;
		end
	end
	
	always@(posedge sourceclk) begin
		if (glitch_go)
			led_on <= 1'b1;
		else if (led_extend == 19'h7FFFF)
			led_on <= 1'b0;
	end
	
	assign led_glitch = led_on;
		
		
	
endmodule

`undef CHIPSCOPE
