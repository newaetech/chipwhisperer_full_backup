`include "includes.v"
//`define CHIPSCOPE

`define BYTE_SELECTION(__bytenum) (7+(8*__bytenum)):(0 + 8*__bytenum))
`define MSB(__bytenum) (7+(8*__bytenum))
`define LSB(__bytenum) (8*bytenum)

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the computer interface. It provides a simple interface to the
rest of the board.

Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.

Notes on interface:

[ 1 RW A5 A4 A3 A2 A1 A0] - Header
[ Write/Read Size LSB   ] - Size, '0' is special case for read which means 'natural size'
[ Write/Read Size MSB   ] 
[ Data Byte 0           ]
[ Data Byte 1           ]
 .......................
[ Data Byte N           ] - Payload size (Variable Length). On WR sent in, on RD sent out
[ Checksum Byte         ] - Checksum, on WR sent in, on RD sent out.


*************************************************************************/
module usb_interface(
	input 			reset_i,
	output			reset_o,
	input 			clk,
	
	/* Interface to computer (e.g.: serial, FTDI chip, etc) */
	input 			cmdfifo_rxf,
	input				cmdfifo_txe,
	output			cmdfifo_rd,
	output			cmdfifo_wr,	
	input [7:0]	   cmdfifo_din,
	output [7:0]	cmdfifo_dout,
	// Following is provided for units with half-duplex interface such as FTDI
	output			cmdfifo_isout,
		
	/* Interface to gain module */
	output [7:0]	gain,
	output			hilow,
	
	/* General status stuff input */
	input [7:0]		status,
								
	/* ADC Fifo Interface */
	input				fifo_empty,
	input [7:0]		fifo_data,
	output			fifo_rd_en,
	output			fifo_rd_clk,
							
	/* Interface to trigger unit */
	output 			cmd_arm,
	output 			trigger_mode,
	output 			trigger_wait,
	output [9:0] 	trigger_level,
	output 			trigger_source,
	output 			trigger_now,
	output [31:0]  trigger_offset,
	
	/* Measurement of external clock frequency */
	input [31:0]	extclk_frequency,
	
	/* Interface to phase shift module */
	output			phase_clk_o,
	output [8:0]	phase_o,
	output			phase_ld_o,
	input  [8:0]	phase_i,
	input				phase_done_i,
	
	/* Additional ADC control lines */
	output			adc_clk_src_o,
	output [31:0]	maxsamples_o,
	input  [31:0]  maxsamples_i										              

	/* If using DDR interface additional lines required */
`ifdef USE_DDR
	 ,output [31:0] 				ddr_address,
	 output							ddr_rd_req,
	 input							ddr_rd_done
`endif

`ifdef USE_ETH
	 ,input			eth_clk,
	 input			eth_clken,
	 output			eth_start,
	 input			eth_done,
	 output [15:0] eth_datalen,
	 output [7:0]  eth_data
`endif

`ifdef USE_SCARD
	 ,output [7:0] scard_cla,
	 output [7:0]  scard_ins,
	 output [7:0]  scard_p1,
	 output [7:0]  scard_p2,
	 output [5:0]  scard_len_command,
	 output [127:0]scard_command,
	 output [5:0]  scard_len_response,
	 input  [127:0]scard_response,
	 input         scard_status,
	 input  [15:0] scard_resp_code,
	 input  [7:0]  scard_async_data,
	 input         scard_async_datardy,
	 input			scard_present,	 
	 output		   scard_reset,
	 output			scard_docmd,
	 input			scard_busy 
`endif
    );

		 
`ifdef USE_DDR
	 reg								ddr_rd_reg; 
	 assign ddr_rd_req = ddr_rd_reg;
`endif
	
    wire          				ftdi_rxf_n;
    wire          				ftdi_txe_n;	 
    reg           				ftdi_rd_n;
    reg           				ftdi_wr_n;
	 reg								fifo_rd_en_reg;
	 assign fifo_rd_en = fifo_rd_en_reg;
	 
	 wire	  reset;
	 reg 	  reset_latched;
	 assign reset = reset_i | reset_latched;
	 assign reset_o = reset;
    	 
    wire [7:0] ftdi_din;
    reg [7:0]  ftdi_dout;
    reg        ftdi_isOutput;
    wire       ftdi_clk;
    
    assign ftdi_clk = clk;
    assign ftdi_rxf_n = ~cmdfifo_rxf;
	 assign ftdi_txe_n = ~cmdfifo_txe;
	 assign ftdi_din = cmdfifo_din;
	 assign cmdfifo_dout = ftdi_dout;
	 assign cmdfifo_rd = ~ftdi_rd_n;
	 assign cmdfifo_wr = ~ftdi_wr_n;
	 assign cmdfifo_isout = ftdi_isOutput;
	 assign fifo_rd_clk = clk;
	 
	 assign phase_clk_o = clk;
    
    //For FTDI interface you would do this in the main module that instiated this:
    //assign ftdi_d = ftdi_isOutput ? ftdi_dout : 8'bZ;
    //assign ftdi_din = ftdi_d;
	 
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
        	  
    //Register definitions
    reg [7:0]  registers_gain;
    reg [7:0]  registers_settings;
	 reg [7:0]  registers_echo;
	 reg [31:0] registers_extclk_frequency;
	 reg [31:0] registers_ddr_address;
	 reg [31:0] registers_samples;
	 reg [31:0] registers_offset;
	 reg [8:0]	phase_out;
	 reg [8:0]  phase_in;
	 reg        phase_loadout;
	 reg			phase_done;		
	
`ifdef USE_DDR
	 assign ddr_address = registers_ddr_address;
`endif

`ifdef USE_SCARD

	reg [7:0] scard_cla_reg;
	reg [7:0]  scard_ins_reg;
	reg [7:0]  scard_p1_reg;
	reg [7:0]  scard_p2_reg;
	reg [5:0]  scard_len_command_reg;
	reg [127:0] scard_command_reg;
	reg [5:0]  scard_len_response_reg;

	reg [7:0]	registers_scardctrl;
	wire [7:0]  registers_scardctrl_read;
	reg			scard_docmd_reg;
	wire        scard_async_en;
	
	assign scard_docmd = scard_docmd_reg;		
	assign scard_reset = registers_scardctrl[0];
	assign scard_async_en = registers_scardctrl[2];
	assign registers_scardctrl_read[0] = registers_scardctrl[0];
	assign registers_scardctrl_read[1] = scard_present;
	assign registers_scardctrl_read[3] = scard_busy;
	assign registers_scardctrl_read[4] = scard_status;

	assign scard_cla = scard_cla_reg;
	assign scard_ins = scard_ins_reg;
	assign scard_p1 = scard_p1_reg;
	assign scard_p2 = scard_p2_reg;
	assign scard_len_command = scard_len_command_reg;
	assign scard_command = scard_command_reg;
	assign scard_len_response = scard_len_response_reg;
	
`endif

    assign trigger_offset = registers_offset;
	 
	 assign phase_o = phase_out;
	 assign phase_ld_o = phase_loadout;
	 
	 always @(posedge ftdi_clk) begin
		if (reset | phase_loadout) begin
			phase_in <= 0;
			phase_done <= 0;
		end else if (phase_done_i) begin
			phase_in <= phase_i;
			phase_done <= 1;
		end
	end
	 
	 /* Registers:
	 
	 0x00 - Gain Settings (One Byte)
	 
	   [ G7 G6 G5 G4 G3 G2 G1 G0 ]
		
		  G = 8-bit PWM setting for gain voltage.
		      Voltage = G / 256 * VCCIO
	 
	 0x01 - Settings (One Byte)
	 
	   [  I  C  W  P  A  T  H  R ]
	     
		  R = (bit 0) System Reset, active high
		  H = (bit 1) Hilo output to amplifier
		  T = (bit 2) Trigger Polarity:
		      1 = Trigger when 'trig in' = 1
				0 = Trigger when 'trig in' = 0
		  A = (bit 3) Arm Trigger
		      1 = Arm trigger
				0 = No effect, but you must clear bit to 0
				    before next trigger cycle can be started
		  P = (bit 4) DUT Clkin PLL Reset
		      1 = Reset to PLL active (must do this when ext clock changes)
				0 = Reset to PLL inactive
		  W = (bit 5) Before arming wait for trigger to go inactive (e.g: edge sensitive)
		      1 = Wait for trigger to go inactive before arming
				0 = Arm immediatly, which if trigger line is currently in active state
				    will also immediatly trigger
		  C = (bit 6) Select clock source for ADC
		      1 = External x4
				0 = Internal 100 MHz				
		  I = (bit 7) Select trigger source: int/ext
		      1 = Internal (e.g.: based on ADC reading)
				0 = External (e.g.: based on trigger-in line)
		  
	 0x02 - Status (One Byte)
	 
	    [  X  M  DC DE P  E  F  T ] 
		 T = (bit 0) Triggered status
		      1 = System armed
				0 = System disarmed		
		 F = (bit 1) Capture Status
		      1 = FIFO Full / Capture Done
				0 = FIFO Not Full / Capture Not Done
		 E = (bit 2) External trigger status
		      1 = Trigger line high
				0 = Trigger line low
		 P = (bit 3) DUT Clkin PLL Status
		      1 = Locked / OK
				0 = Unlocked				
		 DE = (bit 4) DDR Error
		      1 = DDR error (FIFO underflow/overflow or DDR Error)
				0 = No error		 
		 DC = (bit 5) DDR Calibration Done
		      1 = Cal done OK
				0 = Cal in progress/failed	
		 M =  (bit 6) Memory Mode
		      1 = DDR
				0 = FIFO
		
	 0x03 - ADC Readings
	 
       Data is read from this register by issuing a READ command.
		 The entire contents of the FIFO will be dumped following
		 that read command (e.g.: number of samples requested), or
		 in DDR mode a different formatting is used (described elsewhere)
	 
	    [  1  X  X  P OR D9 D8 D7 ]
		 
		 [  0 D6 D5 D4 D3 D2 D1 D0 ]
	 
	 0x04 - Echo Register (1 byte)
	 
		 [ E7 E6 E5 E4 E3 E2 E1 E0 ]
		 
		 E = Write data to this register then read back to
		     confirm device connection is OK	

	 0x05 - External Frequency Counter (4 bytes)
	 
	 0x09 - Phase Adjust (2 Bytes)
	 
	    [ P7 P6 P5 P4 P3 P2 P1 P0 ] (Byte 0)	    
		 [                    S P8 ] (Byte 1)
		 
		 S = Start (write), Status (read)
		 
	 0x10 - Number of samples to capture on trigger (4 Bytes)
	    On reset set to maximum number of samples possible.
	    [ LSB ] (Byte 0)
		 [     ] (Byte 1)
		 [     ] (Byte 2)
		 [ MSB ] (Byte 3)
	 
	 0x14 - DDR address to read from (4 Bytes)
	 
	    This must be 32-bit aligned, e.g. lower 2 bits are zero.
		 This register is automatically incremented following a
		 READ command. So to dump entire memory set DDR address to
		 'zero' then issue read commands.
		 
		 [ LSB ] (Byte 0)
		 [     ] (Byte 1)
		 [     ] (Byte 2)
		 [ MSB ] (Byte 3)
		 
	 0x18 - ADC Trigger Level (2 Bytes)
	 
	    [ LSB ] (Byte 0)
		 [ MSB ] (Byte 1)
	 
	 0x1A - Offset of trigger to start of capture, clock cycles (8 Bytes)
	   
		 [ LSB ] (Byte 0)
		 [     ] (Byte 1)
		 [     ] (Byte 2)
		 [     ] (Byte 3)
		 [     ] (Byte 4)
		 [     ] (Byte 5)
		 [     ] (Byte 6)
		 [ MSB ] (Byte 7)


	 0x1E - Smartcard Control/Status Register
	    [ X X X S D PT C R ]	 	 
		
		 S  = (bit 4) ACK Status (R)
		      1 = Last transaction successful
				0 = Last transaction had wrong ack
		
		 B  = (bit 3) Smartcard Core Busy (R)
		      1 = Busy
				0 = Not Busy (done)
		
		 PT = (bit 2) Pass Through( R/W)
		      1 = Pass any received data to bus - done in ASYNC fasion, be careful
				0 = Pass thru disabled
		
		 C = (bit 1) Card Present (R)
		      1 = Card Inserted
				0 = No Card Present
				
		 R = (bit 0) Card Reset (R/W)
				1 = Reset Asserted (low)
				0 = Reset Deasserted (high)
	 	 
	 0x1F - Smartcard Header Register (W)
	     Always write 6 bytes here.
		    
	 
	 0x20 - Smartcard Payload Register (R/W)
	     Always write 16 bytes here. If actual
		  payload is less than 16 bytes, this will
		  be determined by numbers written into
		  the smartcard header register
		 
	*/
	 
    `define GAIN_ADDR    	0
    `define SETTINGS_ADDR  1
	 `define STATUS_ADDR    2
    `define ADCDATA_ADDR	3
    `define ECHO_ADDR      4
	 `define EXTFREQ_ADDR   5
	 `define EXTFREQ_LEN    4
	 `define PHASE_ADDR     9 
	 `define PHASE_LEN      2
	 `define SAMPLES_ADDR   16
	 `define SAMPLES_LEN    4
	 `define DDRADDR_ADDR   20	 
	 `define OFFSET_ADDR    26
	 	 
`ifdef USE_SCARD
	 `define SCARDCTRL_ADDR	30
	 `define SCARDHDR_ADDR	31
	 `define SCARDPLD_ADDR	32
`endif

	 `define MULTIECHO_ADDR	34
	 
	 `undef  IDLE
    `define IDLE            'b0000
    `define ADDR            'b0001
	 `define BYTECNTLSB      'b0010
	 `define BYTECNTMSB      'b0011
    `define DATAWR1         'b0100
    `define DATAWR2         'b0101
    `define DATAWRDONE      'b0110
    `define DATARDSTART     'b1000
    `define DATARD1         'b1001
    `define DATARD2         'b1010
	 `define DATARD_DDRSTART 'b1011
	 `define CHECKSUM			 'b1110

	 reg [15:0]					total_bytes;    //Byte count for this transaction
	 reg [7:0]					totalbytes_lsb; //LSB from input 
	 reg [8:0]					bytecnt;
    reg [3:0]              state = `IDLE;
	 reg [3:0]              state_new;
    reg [5:0]              address;
	 reg							extclk_locked;
	 reg 							ddr_rd_done_reg;

	 wire 						multiecho_empty;
	 wire [7:0]					multiecho_data;
	 reg [7:0] registers_multiecho;
    
	 assign reset_fromreg = registers_settings[0];
	 assign hilow = registers_settings[1];
	 assign trigger_mode = registers_settings[2];
	 assign cmd_arm = registers_settings[3];
	 assign trigger_wait = registers_settings[5];
	 assign adc_clk_src_o = registers_settings[6];
	 
	 assign gain = registers_gain;
	 assign maxsamples_o = registers_samples;
	  
	 always @(posedge ftdi_clk) begin
		reset_latched <= reset_fromreg;
	 end
	  
	 always @(posedge ftdi_clk)
	 begin
		if (extclk_locked == 0) begin
			registers_extclk_frequency <= extclk_frequency;
		end
	 end
	 
`ifdef USE_SCARD
	 /*
	 always @(posedge ftdi_clk) begin
	  if ((state == `DATARDSTART) & (address == `SCARDRD_ADDR)) begin
			scard_dataird_reg <= 1'b1;
	  end else begin
			scard_dataird_reg <= 1'b0;
	  end
	 end
	 */
	 
	 always @(posedge ftdi_clk) begin
	 //FIXME : write_bytes no longer counts down to zero, no longer exists
	  if ((state == `DATAWR2) & (address == `SCARDPLD_ADDR) & (total_bytes == 0)) begin
			scard_docmd_reg <= 1'b1;
	  end else begin
			scard_docmd_reg <= 1'b0;
	  end
	 end	
	 
	 
`endif
	 /*
	 always @(posedge cmdfifo_ready)
	 begin
		if (state == `DATARDSTART) & (addr != `ADCDATA_ADDR) begin
			cmdfifo_ready_int <= 1'b1;
		end else begin
			cmdfifo_ready_int <= 1'b0;
		end
	 end
	 */
	 assign cmdfifo_ready = 1'b0;
	 
    always @(posedge ftdi_clk)
    begin
      if (reset == 1) begin
         state <= `IDLE; 
         ftdi_rd_n <= 1;
         ftdi_wr_n <= 1;
         ftdi_isOutput <= 0;
			extclk_locked <= 0;					
			
			/* Load Default Values */
			registers_samples <= maxsamples_i;
			registers_settings <= 0;
			registers_offset <= 0;
			
      end else begin
         case (state)
            `IDLE: begin
					fifo_rd_en_reg <= 0;
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  ftdi_wr_n <= 1;
                  ftdi_isOutput <= 0;
                  state <= `ADDR;
               end else begin
					`ifdef USE_SCARD
						if(scard_async_datardy & scard_async_en) begin
							ftdi_wr_n <= 0;
							ftdi_rd_n <= 1;
							ftdi_isOutput <= 1;
							ftdi_dout <= scard_async_data;
							state <= `IDLE;
						end else begin
					 `endif
                  ftdi_rd_n <= 1;
                  ftdi_wr_n <= 1;
                  ftdi_isOutput <= 0;
                  state <= `IDLE;
					 `ifdef USE_SCARD
					  end
					  `endif
               end
            end

            `ADDR: begin	
					bytecnt <= 0;										
					case(ftdi_din[5:0])
						`SCARDHDR_ADDR: total_bytes <= 6; 
						`SCARDPLD_ADDR: total_bytes <= 16; 
						`MULTIECHO_ADDR: total_bytes <= 500;
						`EXTFREQ_ADDR: total_bytes <= `EXTFREQ_LEN;
						`PHASE_ADDR: total_bytes <= `PHASE_LEN;
						`SAMPLES_ADDR: total_bytes <= `SAMPLES_LEN;
						default: total_bytes <= 1;
					endcase				
										
               address <= ftdi_din[5:0];
               ftdi_rd_n <= 0;
               ftdi_wr_n <= 1;
					fifo_rd_en_reg <= 0;
               if (ftdi_din[7] == 1) begin
                  if (ftdi_din[6] == 1) begin
                     //MSB means WRITE
                     ftdi_isOutput <= 0;
                     state_new <= `DATAWR1;               
							state <= `BYTECNTLSB;
                  end else begin
                     //MSB means READ
                     ftdi_isOutput <= 0;
                     state_new <= `DATARDSTART;               
							state <= `BYTECNTLSB;
                  end
               end else begin
                  ftdi_isOutput <= 0;
                  state <= `IDLE;                  
               end
             end
				 
				`BYTECNTLSB: begin
					ftdi_isOutput <= 0;
					ftdi_wr_n <= 1;
					ftdi_rd_n <= 0;
					fifo_rd_en_reg <= 0;
					totalbytes_lsb <= ftdi_din;
					state <= `BYTECNTMSB;					
				end
				
				`BYTECNTMSB: begin
					ftdi_wr_n <= 1;		
					ftdi_rd_n <= 1;					
					fifo_rd_en_reg <= 0;
					if((ftdi_din > 0) | (totalbytes_lsb > 0)) begin
						total_bytes[7:0] <= totalbytes_lsb;
						total_bytes[15:8] <= ftdi_din;
               end
					state <= state_new;
					if(state_new == `DATARDSTART) begin
						ftdi_isOutput <= 1;
					end else begin
						ftdi_isOutput <= 0;
					end
				 end
					
            `DATAWR1: begin
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
					fifo_rd_en_reg <= 0;
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  state <= `DATAWR2;
               end else begin
                  ftdi_rd_n <= 1;
                  state <= `DATAWR1;
               end
             end
               
            `DATAWR2: begin
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
               ftdi_rd_n <= 1;
					fifo_rd_en_reg <= 0;
 
					if (address == `GAIN_ADDR) begin
                  registers_gain <= ftdi_din;
               end else if (address == `SETTINGS_ADDR) begin
                  registers_settings <= ftdi_din;
               end else if (address == `ECHO_ADDR) begin
                  registers_echo <= ftdi_din;
					end else if (address == `SAMPLES_ADDR) begin
						registers_samples[bytecnt*8 +: 8] <= ftdi_din;
					end else if (address == `DDRADDR_ADDR) begin
						registers_ddr_address[bytecnt*8 +: 8] <= ftdi_din;
					end else if (address == `OFFSET_ADDR) begin
						registers_offset[bytecnt*8 +: 8] <= ftdi_din;
`ifdef USE_SCARD 
					end else if (address == `SCARDCTRL_ADDR) begin
						registers_scardctrl <= ftdi_din;
					end else if (address == `SCARDHDR_ADDR) begin					
						case(bytecnt)
							0: scard_cla_reg <= ftdi_din;
							1: scard_ins_reg <= ftdi_din;
							2: scard_p1_reg <= ftdi_din;
							3: scard_p2_reg <= ftdi_din;							
							4: scard_len_command_reg <= ftdi_din;
							5: scard_len_response_reg <= ftdi_din;
							default: ;				
						endcase							
						 
					end else if (address == `SCARDPLD_ADDR) begin				
						case(bytecnt)
							0: scard_command_reg[127:120] <= ftdi_din;
							1: scard_command_reg[119:112] <= ftdi_din;
							2: scard_command_reg[111:104] <= ftdi_din;
							3: scard_command_reg[103:96] <= ftdi_din;
							4: scard_command_reg[95:88] <= ftdi_din;
							5: scard_command_reg[87:80] <= ftdi_din;
							6: scard_command_reg[79:72] <= ftdi_din;
							7: scard_command_reg[71:64] <= ftdi_din;
							8: scard_command_reg[63:56] <= ftdi_din;
							9: scard_command_reg[55:48] <= ftdi_din;
							10: scard_command_reg[47:40] <= ftdi_din;
							11: scard_command_reg[39:32] <= ftdi_din;
							12: scard_command_reg[31:24] <= ftdi_din;
							13: scard_command_reg[23:16] <= ftdi_din;
							14: scard_command_reg[15:8] <= ftdi_din;
							15: scard_command_reg[7:0] <= ftdi_din;
							default: ;
						endcase

`endif
					end else if (address == `MULTIECHO_ADDR) begin
						registers_multiecho <= ftdi_din;
					end
					
					if (bytecnt == (total_bytes-1)) begin
						state <= `IDLE;         
					end else begin
						state <= `DATAWR1;
					end
					
					bytecnt <= bytecnt + 1;
					
             end

            
            `DATARDSTART: begin
               ftdi_isOutput <= 1;               
               ftdi_rd_n <= 1;
					fifo_rd_en_reg <= 0;
					
					bytecnt <= bytecnt + 1;
					           					
					if (address == `GAIN_ADDR) begin
                  ftdi_dout <= registers_gain;
						ftdi_wr_n <= 0;
						extclk_locked <= 0;
						state <= `IDLE;
               end else if (address == `SETTINGS_ADDR) begin
                  ftdi_dout <= registers_settings;
						ftdi_wr_n <= 0;
						extclk_locked <= 0;
						state <= `IDLE;
               end else if (address == `ECHO_ADDR) begin
                  ftdi_dout <= registers_echo;
						ftdi_wr_n <= 0;
						extclk_locked <= 0;
						state <= `IDLE;
					end else if (address == `ADCDATA_ADDR) begin						
						ftdi_dout <= 8'hAC; //Sync Byte
						ftdi_wr_n <= 1;					
`ifdef USE_DDR
						state <= `DATARD_DDRSTART;
`else						
						state <= `DATARD1;
`endif
						extclk_locked <= 0;
					end else if (address == `STATUS_ADDR) begin
						ftdi_dout <= status;
						ftdi_wr_n <= 0;
						extclk_locked <= 0;
						state <= `IDLE;
					end else if (address == `EXTFREQ_ADDR) begin
						ftdi_dout <= registers_extclk_frequency[bytecnt*8 +: 8];
						extclk_locked <= 1;
						ftdi_wr_n <= 0;
						if (bytecnt == (total_bytes-1)) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end
					end else if (address == `PHASE_ADDR) begin
						ftdi_dout <= phase_in[bytecnt*8 +: 8];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						if (bytecnt == (total_bytes-1)) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end
					end else if (address == `SAMPLES_ADDR) begin
						ftdi_dout <= registers_samples[bytecnt*8 +: 8];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						if (bytecnt == (total_bytes-1)) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end						
					end else if (address == `DDRADDR_ADDR) begin
						ftdi_dout <= registers_ddr_address[bytecnt*8 +: 8];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						if (bytecnt == (total_bytes-1)) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end
					end else if (address == `OFFSET_ADDR) begin
						ftdi_dout <= registers_offset[bytecnt*8 +: 8];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;	
						if (bytecnt == (total_bytes-1)) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end
`ifdef USE_SCARD
					end else if (address == `SCARDCTRL_ADDR) begin
						ftdi_dout <= registers_scardctrl_read;
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						state <= `IDLE;	
						
					end else if (address == `SCARDPLD_ADDR) begin						
						case(bytecnt)
							0: ftdi_dout <= scard_response[127:120];
							1: ftdi_dout <= scard_response[119:112];
							2: ftdi_dout <= scard_response[111:104];
							3: ftdi_dout <= scard_response[103:96];
							4: ftdi_dout <= scard_response[95:88];
							5: ftdi_dout <= scard_response[87:80];
							6: ftdi_dout <= scard_response[79:72];
							7: ftdi_dout <= scard_response[71:64];
							8: ftdi_dout <= scard_response[63:56];
							9: ftdi_dout <= scard_response[55:48];
							10: ftdi_dout <= scard_response[47:40];
							11: ftdi_dout <= scard_response[39:32];
							12: ftdi_dout <= scard_response[31:24];
							13: ftdi_dout <= scard_response[23:16];
							14: ftdi_dout <= scard_response[15:8];
							15: ftdi_dout <= scard_response[7:0];
							16: ftdi_dout <= scard_resp_code[15:8];
							17: ftdi_dout <= scard_resp_code[7:0];
							default: ;
						endcase
						
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						
						if(bytecnt == 17) begin
							state <= `IDLE;	
						end else begin
							state <= `DATARDSTART;
						end
						
					end else if (address == `SCARDHDR_ADDR) begin
						ftdi_dout <= 8'd0;
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						state <= `IDLE;	
`endif
					end else if (address == `MULTIECHO_ADDR) begin
						ftdi_dout <= multiecho_data;
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						if (multiecho_empty == 1'b1) begin
							state <= `IDLE;
						end else begin
							state <= `DATARDSTART;
						end
               end else begin
						extclk_locked <= 0;
						ftdi_dout <= 8'bx;						
						ftdi_wr_n <= 1;
						state <= `IDLE;
					end
            end
            
            `DATARD1: begin
               ftdi_isOutput <= 1;
               ftdi_rd_n <= 1;    										
									
					if (ftdi_txe_n == 0) begin
						ftdi_wr_n <= 0;
						if (fifo_empty == 0) begin 
							fifo_rd_en_reg <= 1;
							state <= `DATARD2;
						end else begin
							fifo_rd_en_reg <= 0;
`ifdef USE_DDR
							registers_ddr_address <= registers_ddr_address + 32'h100;
`endif
							state <= `IDLE;
						end
					end else begin
						ftdi_wr_n <= 1;
						fifo_rd_en_reg <= 0;
						state <= `DATARD1;
					end
            end
				
				`DATARD_DDRSTART: begin
					ftdi_isOutput <= 0;
					ftdi_wr_n <= 1;
					ftdi_rd_n <= 1;
					fifo_rd_en_reg <= 0;
					
					if (ddr_rd_done_reg) begin
						state <= `DATARD1;						
					end else begin
						state <= `DATARD_DDRSTART;
					end
					
				end
            
            `DATARD2: begin
               ftdi_isOutput <= 1;
               ftdi_wr_n <= 1;
               ftdi_rd_n <= 1;               
					fifo_rd_en_reg <= 0;
					ftdi_dout <= fifo_data;
               state <= `DATARD1;
               end    
						
				default: begin				
					fifo_rd_en_reg <= 0;
					ftdi_rd_n <= 1;
               ftdi_wr_n <= 1;
               ftdi_isOutput <= 0;
               state <= `IDLE;
				end
             
         endcase
      end                  
    end 
	 
`ifdef USE_DDR
	 always @(posedge reset or posedge ftdi_clk or posedge ddr_rd_done) 
		begin
		if (reset) begin
			ddr_rd_done_reg <= 0;
			ddr_rd_reg <= 0;
		end else if (ddr_rd_done) begin
			ddr_rd_done_reg <= 1;
			ddr_rd_reg <= 0;
		end else begin
			if (state == `IDLE) begin
				ddr_rd_done_reg <= 0;
				ddr_rd_reg <= 0;
			end else if ((state == `DATARD_DDRSTART) && (ddr_rd_done_reg == 0)) begin
				ddr_rd_reg <= 1;
			end
		end
	 end    
`endif


	reg multiecho_wr;
	reg multiecho_rd;
   always @(posedge ftdi_clk) begin
	if ((state == `DATARDSTART) & (address == `MULTIECHO_ADDR) & (multiecho_empty == 1'b0)) begin
			multiecho_rd <= 1'b1;
	  end else begin
			multiecho_rd <= 1'b0;
	  end
	 end
	 
	 always @(posedge ftdi_clk) begin
	  if ((state == `DATAWR2) & (address == `MULTIECHO_ADDR)) begin
			multiecho_wr <= 1'b1;			
	  end else begin
			multiecho_wr <= 1'b0;
	  end
	 end	

	fifo_usb_echo fifo_usb_echo_inst (
	  .clk(ftdi_clk), // input clk
	  .rst(reset), // input rst
	  .din(registers_multiecho), // input [7 : 0] din
	  .wr_en(multiecho_wr), // input wr_en
	  .rd_en(multiecho_rd), // input rd_en
	  .dout(multiecho_data), // output [7 : 0] dout
	  //.full(full), // output full
	  .empty(multiecho_empty) // output empty
	);

`ifdef USE_ETH
	
	reg [31:0] eth_samplecount;
	reg [15:0] eth_udpsize;
	assign eth_datalen = eth_udpsize;
	
	reg eth_start_reg;
	assign eth_start = eth_start_reg;
	
	`define ETH_MAXSAMPLES 32'd1400

	reg [4:0] eth_state;
	`define ETH_IDLE 	4'b0000
	`define ETH_START	4'b0001
	`define ETH_SIZE  4'b0010
	`define ETH_SEND  4'b0011

	always @(posedge eth_clk or posedge reset) begin
		if (reset) begin
			eth_state <= `ETH_IDLE;
		end else begin
			case(eth_state)
				`ETH_IDLE: begin
					eth_samplecount <= registers_samples;
					eth_start_reg <= 1'b0;
					if ((state == `DATARD1) || (state == `DATARD2)) begin
						eth_state <= `ETH_SIZE;
					end
				end
				
				`ETH_SIZE: begin
					eth_start_reg <= 1'b0;
					if (eth_samplecount > `ETH_MAXSAMPLES) begin
						eth_samplecount <= eth_samplecount - `ETH_MAXSAMPLES;
						eth_udpsize <= `ETH_MAXSAMPLES;
						eth_state <= `ETH_START;
					end else if (eth_samplecount > 32'd0) begin
						eth_udpsize <= eth_samplecount;
						eth_samplecount <= 32'd0;		
						eth_state <= `ETH_START;						
					end else begin
						eth_state <= `ETH_IDLE;
					end
				end
				
				`ETH_START: begin
					if (eth_done) begin
						eth_state <= `ETH_IDLE;
						eth_start_reg <= 1'b0;
					end else begin
						eth_start_reg <= 1'b1;
					end
				end

			endcase
		end
	end
`endif

    always @(posedge ftdi_clk or posedge reset)
    begin
      if (reset == 1) begin
         
      end else begin
         case (state)               
            `DATAWR2: begin
               if (address == `PHASE_ADDR) begin
						if (bytecnt == 0) begin
							phase_out[7:0] <= ftdi_din;
						end else if (bytecnt == 1) begin
							phase_out[8] <= ftdi_din[0];
							phase_loadout <= ftdi_din[1];
						end
					end                     
             end
						
				default: begin
					phase_loadout <= 0;
				end             
         endcase
      end                  
    end 

	  
`ifdef CHIPSCOPE
   assign cs_data[3:0] = state;
   assign cs_data[11:4] = address;
   assign cs_data[12] = ftdi_rxf_n;
	assign cs_data[13] = ftdi_txe_n;
	assign cs_data[14] = ftdi_rd_n;
	assign cs_data[15] = ftdi_wr_n;
   assign cs_data[23:16] = registers_echo[7:0]; 
	assign cs_data[32:25] = ftdi_dout;
	assign cs_data[43:36] = ftdi_din;
	assign cs_data[51:44] = total_bytes;
 `endif
 
endmodule

`undef CHIPSCOPE
