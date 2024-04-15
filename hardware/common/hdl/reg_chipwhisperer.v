`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2014-2017, NewAE Technology Inc. All rights reserved.
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
module reg_chipwhisperer(
	input 			reset_i,
	input 			clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	inout [7:0]    reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output  [15:0] reg_hyplen,
	
	/* External Clock */
	inout				extclk_fpa_io,
	input				extclk_fpb_i,
	input				extclk_pll_i,
	input				extclk_rearin_i,
	output			extclk_rearout_o,
	output			extclk_o,
			
	/* External Trigger Connections */
	input 				adc_sample_clk,
	inout				trigger_fpa_i,
	input				trigger_io1_i,
	input				trigger_io2_i,
	input				trigger_io3_i,
	input				trigger_io4_i,
	input				trigger_nrst_i,
    
	/* Advanced IO Trigger Connections */
	output			trigger_ext_o,
	input			trigger_advio_i,
	input			trigger_decodedio_i,
	input			trigger_anapattern_i,
	
	/* Clock Sources */
	input			clkgen_i,
	input			glitchclk_i,
	
	/* GPIO Pins & Routing */
	inout			targetio1_io,
	inout			targetio2_io,
	inout			targetio3_io,
	inout			targetio4_io,
	
	output			hsglitcha_o,
	output			hsglitchb_o,
	
	output			enable_avrprog,
	
	output			enable_output_nrst,
	output			output_nrst,
        output                  nrst_ignore_highz,
	output			enable_output_pdid,
	output			output_pdid,
	output			enable_output_pdic,
	output			output_pdic,
	
	input			uart_tx_i,
	output			uart_rx_o,
	
	input			usi_out_i,
	output			usi_in_o,
	
	output			targetpower_off,
	
	/* Main trigger connections */
	output			trigger_o, /* Trigger signal to capture system */
	input				adccapture_i, /* ADC capture in progress */
	output			led_auxi,
	output			led_auxo
    ); 
	 wire	  reset;
	 assign reset = reset_i;
    
	 `define CW_EXTCLK_ADDR			38
	 `define CW_TRIGSRC_ADDR		39
	 `define CW_TRIGMOD_ADDR		40
	 `define CW_IOROUTE_ADDR      55
	 `define CW_IOREAD_ADDR       59
 
 /*  0xXX - External Clock Connections (One Byte)
	 
	   [  X RO RO FA FA S  S  S ]
	     
		  S S S = 000 Front Panel Channel A / Aux SMA
					 001 Front Panel Channel B
					 010 Front Panel PLL Input
		          011 Rear TargetIO - High Speed Input
					 100 Rear TargetIO - High Speed Output				
					
			FA = 00 Front Panel A: High-Z (REQUIRED if using as input)
			     01 Front Panel A: CLKGEN
				  10 Front Panel A: Glitch Module	
					
			RO = (Bit 6/Bit 5) Rear Clock Out Source
       		  00 : Disabled (constant)
				  10 : CLKGEN
				  11 : Glitch Module
				  
   
     0xXX - External Trigger Connections (One Byte)
	 
	   [  M  M  R4  R3  R2  R1  NR FA ]
	     All external triggers are combined into a single
		  trigger signal, which can then be passed into one
		  of the enabled 'trigger modules'
		  
		  FA = Front Panel Channel A / Aux SMA
		  NR = nRESET Pin
		  R1 = Rear TargetIO - Line 1
		  R2 = Rear TargetIO - Line 2
		  R3 = Rear TargetIO - Line 3
		  R4 = Rear TargetIO - Line 4
		  M  = Mode to combine multiple channels
		    0 = OR
		    1 = AND
	
	  0xXX - Trigger Module Enabled
	  
	   [ X  X  X  X FA M  M  M ]
		  M M M = 000 Normal Edge-Mode Trigger
		          001 Advanced IO Pattern Trigger
		          010 Advanced SAD Trigger
		
		  FA = Output trigger to Front Panel A / Aux SMA
		 
		
	  0xXX - GPIO Pin Routing [8 bytes]
	   
		IMPORTANT: Only a single IO can be assigned
		           to any input. e.g. you can't assign
					  both GPIO1 and GPIO3 to 'RX'. 
					  
					  The system assigns priority to lower
					  numbered GPIOs.
					  
					  Similarly if you attempt to assign multiple
					  outputs to a single TargetIO it will use the
					  lowest bit as the actual output.
		
		GPIO1:
		  [ E G   X     X  USII USIO RX TX ]
		  
		GPIO2:
		  [ E G   X     X  USII USIO RX TX ]
		  
		GPIO3:
		  [ E G  TXO USOC  USII USIO RX TX ]  --> USOC means USIO but with Open Collector drive
		  
		GPIO4:
		  [ X X   X     X    X    X  X   X ]
		 
	   GLITCH:
		  [ X X   X     X    X    X  B   A ]
		  
		  	A  = (Bit 0) Glitch Output A
			     0  : Disabled
				  1  : Glitch Module
				  
			B  = (Bit 1) Glitch Output B
			     0  : Disabled
				 1  : Glitch Module
		
		EXTRA:
		  [ X X   SC    SC    X    S  P   A ]
		  
		  A = (Bit 0) AVR Programming Enable
				  0  : Disabled (High-Z)
				  1  : Enabled (Connected to SAM3U)
				  
		  P = (Bit 1) Target Power Disable
		        0  : Power On
				  1  : Power Off
				  
		  S = (Bit 2) Target Power Switch Type
		        0  : Slow-Start (safer)
				  1  : Fast-Start
				  
		  SC = (Bit 5 - 4) Smart-Card Interface Setup (CW1200 Only)
		  
		       00  : High-Z (Not Used)
				 01  : 
				 10  :
				 11  :
		  
		EXTRA GPIO:
		  [ X X   PC  PCE   PD   PDE  N  NE ]
		  
		  NE  =  nRST Enable as GPIO
		  N   =   nRST state
		  PDE = PDID Enable as GPIO
		  PD  = PDID State
		  PCE = PDIC Enable as GPIO
		  PC  = PDIC State
		  
		  
		RESERVED:
		  [ X X   X     X    X    X  X   X ]
		  
 */
    
	 reg [7:0] registers_cwextclk;
	 reg [7:0] registers_cwtrigsrc;
	 reg [7:0] registers_cwtrigmod;
	 reg [63:0] registers_iorouting;
	 reg [3:0] registers_ioread;
  	 
	 wire targetio_highz;
	 
	 assign led_auxi = registers_cwtrigsrc[0];
	 assign led_auxo = registers_cwextclk[4] | registers_cwextclk[3] | registers_cwtrigmod[3];
	
	 //Do to no assumed phase relationship we use regular old fabric for switching
	 assign extclk_o =  /* (registers_cwextclk[2:0] == 3'b000) ? extclk_fpa_io :  */
							  (registers_cwextclk[2:0] == 3'b001) ? extclk_fpb_i : 
							  (registers_cwextclk[2:0] == 3'b010) ? extclk_pll_i : 
							  (registers_cwextclk[2:0] == 3'b011) ? extclk_rearin_i : 
							  //(registers_cwextclk[2:0] == 3'b100) ? extclk_rearout_o : 
							  1'b0;
							  
	 //TODO: Should use a mux?
	 //The glitch-clock comes from the fabric anyway, but the clkgen comes from the DCM. Either way we are jumping back
	 //and forth a lot.
	 assign extclk_fpa_io = (registers_cwextclk[4:3] == 2'b00) ? trigger : /* adccapture_i */
                           (registers_cwextclk[4:3] == 2'b01) ? clkgen_i :
	 							   (registers_cwextclk[4:3] == 2'b10) ? glitchclk_i :
	 							    1'bZ;

/*	 
`ifdef SUPPORT_AUXLINE

	 wire extclk_fpa_int_o;
	 wire fpa_trigger_int;
	 assign extclk_fpa_io = (registers_cwextclk[4:3] == 2'b00) ? fpa_trigger_int :
									 extclk_fpa_int_o;
`else
	assign extclk_fpa_io = 1'bZ;
`endif
	*/ 
	
	wire rearclk;
	
        `ifndef __ICARUS__
	   BUFGMUX #(
	   .CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	   )
	   clkgenfx_mux (
	   .O(rearclk), // 1-bit output: Clock buffer output
	   .I0(clkgen_i), // 1-bit input: Clock buffer input (S=0)
	   .I1(glitchclk_i), // 1-bit input: Clock buffer input (S=1)
	   .S(registers_cwextclk[5]) // 1-bit input: Clock buffer select
	   );
        `else
           assign rearclk = registers_cwextclk[5]? glitchclk_i : clkgen_i;
        `endif
	
	/*
`ifdef SUPPORT_AUXLINE
	//NB: Swap I0 / I1 lines here as allows above two muxes to be placed together
	BUFGMUX #(
	.CLK_SEL_TYPE("ASYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	)
	clkauxline_mux (
	.O(extclk_fpa_int_o), // 1-bit output: Clock buffer output
	.I1(clkgen_i), // 1-bit input: Clock buffer input (S=0)
	.I0(glitchclk_i), // 1-bit input: Clock buffer input (S=1)
	.S(registers_cwextclk[4]) // 1-bit input: Clock buffer select
	);

`endif
*/
	
	//NB: Normally ODDR2 used for clock output. This won't work as this clock
	//can have glitches, which screws up the ODDR2 block. Because we don't care
	//about variations in synchronization of this clock to source clock, this
	//should be OK.
	/*
	ODDR2 #(
		// The following parameters specify the behavior
		// of the component.
		.DDR_ALIGNMENT("NONE"), // Sets output alignment
										// to "NONE", "C0" or "C1"
		.INIT(1'b0),    // Sets initial state of the Q 
							 //   output to 1'b0 or 1'b1
		.SRTYPE("ASYNC") // Specifies "SYNC" or "ASYNC"
							 //   set/reset
	)
	ODDR2_rearclk (
		.Q(extclk_rearout_o),   // 1-bit DDR output data
		.C0(rearclk), // 1-bit clock input
		.C1(~rearclk), // 1-bit clock input
		.CE(registers_cwextclk[6]), // 1-bit clock enable input
		.D0(1'b1), // 1-bit data input (associated with C0)
		.D1(1'b0), // 1-bit data input (associated with C1)
		.R(~registers_cwextclk[6]),   // 1-bit reset input
		.S(1'b0)    // 1-bit set input
	);
	*/
	
	assign extclk_rearout_o = (registers_cwextclk[6] & (~targetio_highz)) ? rearclk : 1'bZ;
	
	//Output clock using DDR2 block (recommended for Spartan-6 device)
        `ifndef __ICARUS__
	ODDR2 #(
		// The following parameters specify the behavior
		// of the component.
		.DDR_ALIGNMENT("NONE"), // Sets output alignment
										// to "NONE", "C0" or "C1"
		.INIT(1'b0),    // Sets initial state of the Q 
							 //   output to 1'b0 or 1'b1
		.SRTYPE("ASYNC") // Specifies "SYNC" or "ASYNC"
							 //   set/reset
	)
	ODDR2_hsglitcha (
		.Q(hsglitcha_o),   // 1-bit DDR output data
		.C0(glitchclk_i), // 1-bit clock input
		.C1(~glitchclk_i), // 1-bit clock input
		.CE(registers_iorouting[32]), // 1-bit clock enable input
		.D0(1'b1), // 1-bit data input (associated with C0)
		.D1(1'b0), // 1-bit data input (associated with C1)
		.R(~registers_iorouting[32]),   // 1-bit reset input
		.S(1'b0)    // 1-bit set input
	);
	
	//Output clock using DDR2 block (recommended for Spartan-6 device)
	ODDR2 #(
		// The following parameters specify the behavior
		// of the component.
		.DDR_ALIGNMENT("NONE"), // Sets output alignment
										// to "NONE", "C0" or "C1"
		.INIT(1'b0),    // Sets initial state of the Q 
							 //   output to 1'b0 or 1'b1
		.SRTYPE("ASYNC") // Specifies "SYNC" or "ASYNC"
							 //   set/reset
	)
	ODDR2_hsglitchb (
		.Q(hsglitchb_o),   // 1-bit DDR output data
		.C0(glitchclk_i), // 1-bit clock input
		.C1(~glitchclk_i), // 1-bit clock input
		.CE(registers_iorouting[33]), // 1-bit clock enable input
		.D0(1'b1), // 1-bit data input (associated with C0)
		.D1(1'b0), // 1-bit data input (associated with C1)
		.R(~registers_iorouting[33]),   // 1-bit reset input
		.S(1'b0)    // 1-bit set input
	);
        `endif
	
	
	 assign enable_avrprog = registers_iorouting[40];

	 /* Target Power */
	 
	 reg reg_targetpower_off;
	 reg reg_targetpower_off_prev;	
	 always @(posedge clk) begin
	    reg_targetpower_off <= registers_iorouting[41];
		 reg_targetpower_off_prev <= reg_targetpower_off;
	 end
	 
	 /* Target power switched ON from OFF state using PWM for programmable soft-start.
       Only in CW1200 currently. */
`ifdef SUPPORT_SOFTPOWER
	 reg targetpower_soft_on;
	 always @(posedge clk) begin
		if ((reg_targetpower_off == 1'b0) && (reg_targetpower_off_prev == 1'b1)) begin
			targetpower_soft_on <= 1'b1;
		end else if (reg_targetpower_off == 1'b1) begin
			targetpower_soft_on <= 1'b0;
		end
	 end
	 
	 reg [10:0] soft_start_pwm;	 
	 always @(posedge clk) begin
		soft_start_pwm <= soft_start_pwm + 11'd1;
	 end
	 
	 reg output_src_pwm;
	 reg [13:0] soft_start_cnt;
	 always @(posedge clk) begin
		if (targetpower_soft_on == 1'b0) begin
			soft_start_cnt <= 0;
			output_src_pwm <= 1'b0;
		end else if (soft_start_cnt == 14'd16383) begin
			output_src_pwm <= 1'b0;
		end else if (soft_start_pwm == 0) begin
			soft_start_cnt <= soft_start_cnt + 14'd1;
			output_src_pwm <= 1'b1;
		end
	 end
	 
	 wire targetpower_slow = ~registers_iorouting[42];
	 assign targetpower_off_pwm = (soft_start_pwm < 1400) ? 1'b1 : 1'b0;	 
	 assign targetpower_off = (output_src_pwm & targetpower_slow) ? targetpower_off_pwm : reg_targetpower_off;
`else
	 assign targetpower_off = reg_targetpower_off;
`endif

	 assign targetio_highz = reg_targetpower_off;
	
	 //TODO: Should use a mux?
	 /*
	 assign extclk_rearout_o = (registers_cwextclk[6:5] == 2'b01) ? clkgen_i :
								  (registers_cwextclk[6:5] == 2'b10) ? glitchclk_i :
								  1'bZ;	
	 */
		
	 wire trigger_and;
	 wire trigger_or;
	 wire trigger_ext;
	 
	 wire trigger_fpa;
	 
	 assign trigger_and = ((registers_cwtrigsrc[0] & trigger_fpa) | ~registers_cwtrigsrc[0]) &
								 ((registers_cwtrigsrc[1] & trigger_nrst_i) | ~registers_cwtrigsrc[1]) &
								 ((registers_cwtrigsrc[2] & trigger_io1_i) | ~registers_cwtrigsrc[2]) &
								 ((registers_cwtrigsrc[3] & trigger_io2_i) | ~registers_cwtrigsrc[3]) &
								 ((registers_cwtrigsrc[4] & trigger_io3_i) | ~registers_cwtrigsrc[4]) &
								 ((registers_cwtrigsrc[5] & trigger_io4_i) | ~registers_cwtrigsrc[5]);
								 
	 assign trigger_or  = (registers_cwtrigsrc[0] & trigger_fpa) |
								 (registers_cwtrigsrc[1] & trigger_nrst_i) |
								 (registers_cwtrigsrc[2] & trigger_io1_i) |
								 (registers_cwtrigsrc[3] & trigger_io2_i) |
								 (registers_cwtrigsrc[4] & trigger_io3_i) |
								 (registers_cwtrigsrc[5] & trigger_io4_i);
								 
	 assign trigger_ext =  (registers_cwtrigsrc[7:6] == 2'b00) ? trigger_or :
								  (registers_cwtrigsrc[7:6] == 2'b01) ? trigger_and : 
								  (registers_cwtrigsrc[7:6] == 2'b10) ? (~trigger_and) :
								  1'b0;
	
	 wire trigger;	 		  
	 assign trigger = (registers_cwtrigmod[2:0] == 3'b000) ? trigger_ext :
						   (registers_cwtrigmod[2:0] == 3'b001) ? trigger_advio_i : 
							(registers_cwtrigmod[2:0] == 3'b010) ? trigger_anapattern_i :
							(registers_cwtrigmod[2:0] == 3'b011) ? trigger_decodedio_i
							: 1'b0;
							
	 assign trigger_ext_o = trigger_ext;
	 
	 assign trigger_o = trigger;
	 
	 wire   fpa_trigger_int = (registers_cwtrigmod[3] == 1'b1) ? trigger : 1'bZ;
	 assign trigger_fpa_i =  fpa_trigger_int; 
	 
	 
`ifdef SUPPORT_AUXLINE
   IODELAY2 #(
			.COUNTER_WRAPAROUND("WRAPAROUND"), // "STAY_AT_LIMIT" or "WRAPAROUND"
			.DATA_RATE("SDR"), // "SDR" or "DDR"
			.DELAY_SRC("IDATAIN"), // "IO", "ODATAIN" or "IDATAIN"
			.IDELAY2_VALUE(0), // Delay value when IDELAY_MODE="PCI" (0-255)
			.IDELAY_MODE("NORMAL"), // "NORMAL" or "PCI"
			.IDELAY_TYPE("DEFAULT"), // "FIXED", "DEFAULT", "VARIABLE_FROM_ZERO", "VARIABLE_FROM_HALF_MAX"
			.IDELAY_VALUE(20), // Amount of taps for fixed input delay (0-255)
			.ODELAY_VALUE(0), // Amount of taps fixed output delay (0-255)
			.SERDES_MODE("NONE"), // "NONE", "MASTER" or "SLAVE"
			.SIM_TAPDELAY_VALUE(75) // Per tap delay used for simulation in ps
			)
		IODELAY2_inst (
			.BUSY(), // 1-bit output: Busy output after CAL
			.DATAOUT(), // 1-bit output: Delayed data output to ISERDES/input register
			.DATAOUT2(trigger_fpa), // 1-bit output: Delayed data output to general FPGA fabric
			.DOUT(), // 1-bit output: Delayed data output
			.TOUT(), // 1-bit output: Delayed 3-state output
			.CAL(~reset_i), // 1-bit input: Initiate calibration input
			.CE(1'b0), // 1-bit input: Enable INC input
			.CLK(), // 1-bit input: Clock input
			.IDATAIN(extclk_fpa_io), // 1-bit input: Data input (connect to top-level port or I/O buffer)
			.INC(), // 1-bit input: Increment / decrement input
			.IOCLK0(adc_sample_clk), // 1-bit input: Input from the I/O clock network
			.IOCLK1(), // 1-bit input: Input from the I/O clock network
			.ODATAIN(), // 1-bit input: Output data input from output register or OSERDES2.
			.RST(reset_i), // 1-bit input: Reset to zero or 1/2 of total delay period
			.T() // 1-bit input: 3-state input signal
		);
`endif
	 
`ifndef DISABLE_FPA_IN
   IODELAY2 #(
			.COUNTER_WRAPAROUND("WRAPAROUND"), // "STAY_AT_LIMIT" or "WRAPAROUND"
			.DATA_RATE("SDR"), // "SDR" or "DDR"
			.DELAY_SRC("IDATAIN"), // "IO", "ODATAIN" or "IDATAIN"
			.IDELAY2_VALUE(0), // Delay value when IDELAY_MODE="PCI" (0-255)
			.IDELAY_MODE("NORMAL"), // "NORMAL" or "PCI"
			.IDELAY_TYPE("DEFAULT"), // "FIXED", "DEFAULT", "VARIABLE_FROM_ZERO", "VARIABLE_FROM_HALF_MAX"
			.IDELAY_VALUE(20), // Amount of taps for fixed input delay (0-255)
			.ODELAY_VALUE(0), // Amount of taps fixed output delay (0-255)
			.SERDES_MODE("NONE"), // "NONE", "MASTER" or "SLAVE"
			.SIM_TAPDELAY_VALUE(75) // Per tap delay used for simulation in ps
			)
		IODELAY2_inst (
			.BUSY(), // 1-bit output: Busy output after CAL
			.DATAOUT(), // 1-bit output: Delayed data output to ISERDES/input register
			.DATAOUT2(trigger_fpa), // 1-bit output: Delayed data output to general FPGA fabric
			.DOUT(), // 1-bit output: Delayed data output
			.TOUT(), // 1-bit output: Delayed 3-state output
			.CAL(~reset_i), // 1-bit input: Initiate calibration input
			.CE(1'b0), // 1-bit input: Enable INC input
			.CLK(), // 1-bit input: Clock input
			.IDATAIN(trigger_fpa_i), // 1-bit input: Data input (connect to top-level port or I/O buffer)
			.INC(), // 1-bit input: Increment / decrement input
			.IOCLK0(adc_sample_clk), // 1-bit input: Input from the I/O clock network
			.IOCLK1(), // 1-bit input: Input from the I/O clock network
			.ODATAIN(), // 1-bit input: Output data input from output register or OSERDES2.
			.RST(reset_i), // 1-bit input: Reset to zero or 1/2 of total delay period
			.T() // 1-bit input: 3-state input signal
		);
`endif
	 
	 /* IO Routing */
	 
	 assign targetio1_io = targetio_highz ? 1'bZ :
								  registers_iorouting[0 + 0] ? uart_tx_i :
								  registers_iorouting[0 + 2] ? usi_out_i :
								  registers_iorouting[0 + 7] ? registers_iorouting[0 + 6] :
								  1'bZ;
		
	 assign targetio2_io = targetio_highz ? 1'bZ :
								  registers_iorouting[8 + 0] ? uart_tx_i :
								  registers_iorouting[8 + 2] ? usi_out_i :
								  registers_iorouting[8 + 7] ? registers_iorouting[8 + 6] :
								  1'bZ;
								  
	 assign targetio3_io = targetio_highz ? 1'bZ :
								  registers_iorouting[16 + 0] ? uart_tx_i :
								  registers_iorouting[16 + 2] ? usi_out_i :
								  registers_iorouting[16 + 4] ? (usi_out_i ? 1'bZ : 1'b0) :
								  registers_iorouting[16 + 5] ? (uart_tx_i ? 1'bZ : 1'b0) :
								  registers_iorouting[16 + 7] ? registers_iorouting[16 + 6] :
								  1'bZ;
								  
	 assign targetio4_io = targetio_highz ? 1'bZ :
								  registers_iorouting[24 + 0] ? uart_tx_i :
								  registers_iorouting[24 + 7] ? registers_iorouting[24 + 6] :
								  1'bZ;
	 
	 assign uart_rx_o = registers_iorouting[0 + 1] ? targetio1_io :
							registers_iorouting[8 + 1] ? targetio2_io :
							registers_iorouting[16 + 1] ? targetio3_io :
							registers_iorouting[24 + 1] ? targetio3_io :
							1'b1;
							
	assign usi_in_o = registers_iorouting[0 + 3] ? targetio1_io :
							registers_iorouting[8 + 3] ? targetio2_io :
							registers_iorouting[16 + 3] ? targetio3_io :
							1'b1;	 
	 
	 
	 assign enable_output_nrst = registers_iorouting[48];
	 assign output_nrst = registers_iorouting[49];
	 assign nrst_ignore_highz = registers_iorouting[54];
	 assign enable_output_pdid = registers_iorouting[50];
	 assign output_pdid = registers_iorouting[51];
	 assign enable_output_pdic = registers_iorouting[52];
	 assign output_pdic = registers_iorouting[53];

	 always @(posedge clk) begin
		if (reset) begin
			registers_ioread <= 4'b0000;
		end else begin
			registers_ioread[3:0] <= {targetio4_io, targetio3_io, targetio2_io, targetio1_io};
		end
	 end

	 
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `CW_EXTCLK_ADDR: reg_hyplen_reg <= 1;
				`CW_TRIGSRC_ADDR: reg_hyplen_reg <= 1;
				`CW_TRIGMOD_ADDR: reg_hyplen_reg <= 1;
				`CW_IOROUTE_ADDR: reg_hyplen_reg <= 8;
				`CW_IOREAD_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end

	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 assign reg_datao = (reg_datao_valid_reg/*& reg_read*/) ? reg_datao_reg : 8'd0;

	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_TRIGSRC_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_TRIGMOD_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_IOROUTE_ADDR: begin reg_datao_valid_reg <= 1; end
				`CW_IOREAD_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: reg_datao_reg <= registers_cwextclk; 
				`CW_TRIGSRC_ADDR: reg_datao_reg <= registers_cwtrigsrc; 
				`CW_TRIGMOD_ADDR: reg_datao_reg <= registers_cwtrigmod; 
				`CW_IOROUTE_ADDR: reg_datao_reg <= registers_iorouting[reg_bytecnt*8 +: 8];
				`CW_IOREAD_ADDR: reg_datao_reg <= {4'b0000, registers_ioread};
				default: reg_datao_reg <= 0;	
			endcase
		end
	 end	  

	 always @(posedge clk) begin
		if (reset) begin
			registers_cwextclk <= 8'b00000011;
`ifdef DISABLE_FPA_IN
			registers_cwtrigsrc <= 8'b00100000;
`else
			registers_cwtrigsrc <= 8'b00000001;
`endif
			registers_cwtrigmod <= 0;
			registers_iorouting <= 64'b00000010_00000001;
		end else if (reg_write) begin
			case (reg_address)
				`CW_EXTCLK_ADDR: registers_cwextclk <= reg_datai;
				`CW_TRIGSRC_ADDR: registers_cwtrigsrc <= reg_datai;
				`CW_TRIGMOD_ADDR: registers_cwtrigmod <= reg_datai;
				`CW_IOROUTE_ADDR: registers_iorouting[reg_bytecnt*8 +: 8] <= reg_datai;
				default: ;
			endcase
		end
	 end	 

			
 `ifdef CHIPSCOPE
	 assign cs_data[5:0] = reg_address;
	 assign cs_data[21:6] = reg_bytecnt;
	 assign cs_data[29:22] = reg_datai;
	 assign cs_data[37:30] = reg_datao;
	 assign cs_data[38] = reg_read;
	 assign cs_data[39] = reg_write;
	 assign cs_data[40] = reg_addrvalid;
	 assign cs_data[46:41] = reg_hypaddress;
	 assign cs_data[62:47] = reg_hyplen;
 `endif
 
endmodule

`undef CHIPSCOPE
