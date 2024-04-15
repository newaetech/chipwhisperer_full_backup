`include "includes.v"
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the FIFO interface. It provides a simple interface to the FIFO
memory in the FPGA.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
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
//`define CHIPSCOPE
module fifo_top(
    input         reset_i,
	 output			reset_o,
    input			clk_100mhz_in,
	 output			clk_100mhz_out,
	 
	 //ADC Sample Input
	 input [9:0]   adc_datain,
	 input 			adc_sampleclk,
	 input			adc_or,
	 input			adc_trig_status,
	 input			adc_capture_go, //Set to '1' to start capture, keep at 1 until adc_capture_stop goes high
	 output			adc_capture_stop,
	 
	 //FIFO to USB Read Interface
	 input			fifo_read_fifoclk,
	 input			fifo_read_fifoen,
	 output			fifo_read_fifoempty,
	 output [7:0]	fifo_read_data,
	 input         arm_i,
	 
	 input  [31:0] presample_i,
	 input  [31:0]	max_samples_i,
	 output [31:0]	max_samples_o,
	 output [31:0] samples_o
	 
`ifdef CHIPSCOPE
	 ,inout [35:0]  chipscope_control
`endif
	 );
	

	wire [31:0] 	adcfifo_in;	
	reg [1:0]  		adcfifo_merge_cnt;
	reg        		adcfifo_or;
	reg				adcfifo_trigstat;
	reg [9:0]		adcfifo_adcsample0;
	reg [9:0]		adcfifo_adcsample1;
	reg [9:0]		adcfifo_adcsample2;
	
	wire 				adcfifo_full;
	wire 				adcfifo_empty;
	reg 				adcfifo_wr_en;
	wire 	[31:0] 	adcfifo_dout;
	
	reg [31:0]  	sample_counter; //How many samples gone through fifo
	reg 				presample;
	
	reg 				adc_capture_stop_reg;
	assign			adc_capture_stop = adc_capture_stop_reg;
	
	
	`ifdef NOBUFG_ADCCLK
	assign clk_100mhz_out = clk_100mhz_in;
	`else	
	IBUFG IBUFG_inst (
	.O(clk_100mhz_out),
	.I(clk_100mhz_in) );
	`endif
		
	assign reset_o = reset_i;
	
	//3 samples per 4 bytes
	assign max_samples_o = 32'd`MAX_SAMPLES ;
	
	always@(posedge adc_sampleclk) begin
		if ((sample_counter < max_samples_i) && (adcfifo_full == 0)) begin
				adc_capture_stop_reg <= 0;
		end else begin
				adc_capture_stop_reg <= 1;
		end
	end
		
	always@(posedge adc_sampleclk) begin
		if ((adc_capture_go | presample) == 0) begin
			adcfifo_merge_cnt <= 'b00;
		end else begin		
			if (adcfifo_merge_cnt == 'b00)
				adcfifo_adcsample0 <= adc_datain;
			else if (adcfifo_merge_cnt == 'b01)
				adcfifo_adcsample1 <= adc_datain;
			else if (adcfifo_merge_cnt == 'b10)
				adcfifo_adcsample2 <= adc_datain;
		
			adcfifo_merge_cnt <= (adcfifo_merge_cnt == 2'b10) ? 2'b00 : (adcfifo_merge_cnt + 1'b1);			
		end
	end
		
	always@(posedge adc_sampleclk) begin
		if (~adc_capture_go) begin
			sample_counter <= 0;
		end else begin
			sample_counter <= sample_counter + 1;
		end
	end
	
	always@(posedge adc_sampleclk) begin
		if ((adc_capture_go | presample) == 0) begin
			adcfifo_wr_en <= 0;
		end else begin						
			if (adcfifo_merge_cnt == 'b10) begin
				adcfifo_wr_en <= 1;
			end else begin
				adcfifo_wr_en <= 0;
			end
		end
	end
	
	reg [1:0] mergeloc;
	always @(posedge adc_sampleclk) begin
		if (~adc_capture_go)
			mergeloc <= 2'b11;
		else if (adc_capture_go && (mergeloc == 2'b11))
			mergeloc <= adcfifo_merge_cnt; //(adcfifo_merge_cnt == 'b10) ? 'b00 : adcfifo_merge_cnt + 1;	;
		end
	
	assign adcfifo_in[31:30] = mergeloc;
	assign adcfifo_in[29:20] = adcfifo_adcsample2;
	assign adcfifo_in[19:10] = adcfifo_adcsample1;
	assign adcfifo_in[9:0] = adcfifo_adcsample0;
		
	/* Stretch fifo_empty */
	wire fifo_empty;
	reg fifo_empty_longer1;
	reg fifo_empty_longer2;
	reg fifo_empty_longer3;
	assign fifo_read_fifoempty = fifo_empty | fifo_empty_longer1 | fifo_empty_longer2 | fifo_empty_longer3;
	always @(posedge fifo_read_fifoclk) begin
		fifo_empty_longer1 <= fifo_empty;
		fifo_empty_longer2 <= fifo_empty_longer1;
		fifo_empty_longer3 <= fifo_empty_longer2;
	end
	
	
		
	wire [31:0] prog_full_thresh;
	assign prog_full_thresh = presample_i;
	
	wire prog_full;
	reg drain_fifo;
	
	/* WARNING: signals here CROSS CLOCK DOMAINS WITHOUT PROPER RESYNC. Typically this is OK, 
	    as the result is simply that you may have slightly less/more samples than requested.
		 The user reads the actual amount anyway, so ultimately we decide to not care. */
		 
	always @(posedge fifo_read_fifoclk)
		if (prog_full & presample)
			drain_fifo <= 1;
		else
			drain_fifo <= 0;
	
	reg presample_possible;
	
	always @(posedge adc_sampleclk)
		if (reset_i | fifo_empty_longer3)
			presample_possible <= 1;
		else if (adc_capture_go)
			presample_possible <= 0;
			
	always @(posedge adc_sampleclk)
		presample <= (prog_full_thresh == 0) ? 1'b0 : (~adc_capture_go & presample_possible);
	
	/* Convert 128-bit to 8-bit */
	reg [15:0] byte_select;
	always @(posedge fifo_read_fifoclk)
		if (reset_i | fifo_empty_longer3)
			byte_select <= 16'b0000000000000001;
		else if (fifo_read_fifoen)
			byte_select <= (byte_select == 16'b0000000000000001) ?  16'b0000000000000010 :
			               (byte_select == 16'b0000000000000010) ?  16'b0000000000000100 :
								(byte_select == 16'b0000000000000100) ?  16'b0000000000001000 :
								(byte_select == 16'b0000000000001000) ?  16'b0000000000010000 :
								(byte_select == 16'b0000000000010000) ?  16'b0000000000100000 :
								(byte_select == 16'b0000000000100000) ?  16'b0000000001000000 :
								(byte_select == 16'b0000000001000000) ?  16'b0000000010000000 :
								(byte_select == 16'b0000000010000000) ?  16'b0000000100000000 :
								(byte_select == 16'b0000000100000000) ?  16'b0000001000000000 :
								(byte_select == 16'b0000001000000000) ?  16'b0000010000000000 :
								(byte_select == 16'b0000010000000000) ?  16'b0000100000000000 :
								(byte_select == 16'b0000100000000000) ?  16'b0001000000000000 :
								(byte_select == 16'b0001000000000000) ?  16'b0010000000000000 :
								(byte_select == 16'b0010000000000000) ?  16'b0100000000000000 :
								(byte_select == 16'b0100000000000000) ?  16'b1000000000000000 :								
								16'b0000000000000001;
	
	reg read_en;
	always @(posedge fifo_read_fifoclk)
		if (reset_i | presample | adc_capture_go | fifo_empty)
			read_en <= 0;
		else
			read_en <= (byte_select == 16'b1000000000000000) ? 1'b1 : 1'b0;
	
	wire [127:0] fifo_data;
	reg [7:0] fifo_read_data_reg;
	assign fifo_read_data = fifo_read_data_reg;
	always @(posedge fifo_read_fifoclk)
		fifo_read_data_reg <= (byte_select[15]) ?  fifo_data[7:0] :
			                   (byte_select[14]) ?  fifo_data[15:8] :
									 (byte_select[13]) ?  fifo_data[23:16] :
									 (byte_select[12]) ?  fifo_data[31:24] :
									 (byte_select[11]) ?  fifo_data[39:32] :
									 (byte_select[10]) ?  fifo_data[47:40] :
									 (byte_select[9]) ?  fifo_data[55:48] :
									 (byte_select[8]) ?  fifo_data[63:56] :
									 (byte_select[7]) ?  fifo_data[71:64] :
									 (byte_select[6]) ?  fifo_data[79:72] :
									 (byte_select[5]) ?  fifo_data[87:80] :
									 (byte_select[4]) ?  fifo_data[95:88] :
									 (byte_select[3]) ?  fifo_data[103:96] :
									 (byte_select[2]) ?  fifo_data[111:104] :
									 (byte_select[1]) ?  fifo_data[119:112] :									 
								    fifo_data[127:120];
	
		
	wire rd_clk;
		
	/* 
	We use a 32-bit in to 128-bit out FIFO. The 32-bit input allows us
	to pack 3 10-bit samples into each word, and the remaining 2 bits
	tell us which of the samples caused the trigger to happen.
			
	FIFO needs to be configured with:
	*Interface Type: Native
	*FIFO Implementation: Independant Clocks, Block RAM
	*Read Mode: First-Word Fall-Through
	*Write Width: 32-bit
	*Write Depth: Set as you desire, needs to match setup.v
	*Read Width: 128-bit
	*Optional Flags: None
	*Handshaking Options: None
	*Initalization:
	       +Reset Pin
			 +Enable Reset Syncronization
			 +Full Flags Reset Value = 0
			 +Use Dout reset value = 0
	*Programmable Flags
			 +Programmable Full Type: Signle Programmable Full Threshold Input Port
	*Data Count Options
			 +Use extra logic for more accurate Data Counts
			 +Read Data Count
	
	We use 128-bit output for the pretrigger ability. Pretrigger is done by emptying
	the fifo while we fill it to keep a certain number of words in the fifo. The problem
	is the ADC clock might be much faster than the read clock - so we empty 4x more samples
	than we load on every clock. This allows up to 4x faster ADC than the fifo read clock.
	
	Fifo read clock depends on HW, typically 30-100 MHz. The ADC clock will be < 120 MHz.
	
	Downside of this is that when the FIFO EMPTY goes high, there may be a 'partially filled' 128-bit
	bucket left. This bucket will screw with syncronization next time around, so we reset the FIFO
	after the empty flag goes high to totally clear it out.
	
	
	*/
		
        `ifndef __ICARUS__
	fifoonly_adcfifo fifoonly_adcfifo_inst (
  .rst(reset_i | (fifo_empty_longer2 & ~fifo_empty_longer3)), // input rst
  .wr_clk(adc_sampleclk), // input wr_clk
  .rd_clk(fifo_read_fifoclk), // input rd_clk
  .din(adcfifo_in), // input [31 : 0] din
  .wr_en(adcfifo_wr_en), // input wr_en
  .rd_en((fifo_read_fifoen & read_en) | drain_fifo), // input rd_en
  .dout(fifo_data), // output [127 : 0] dout
  .full(adcfifo_full), // output full
  .empty(fifo_empty), // output empty
  .prog_full_thresh(prog_full_thresh), // input [12 : 0] prog_full_thresh
  .prog_full(prog_full), // output prog_full
  .rd_data_count(samples_o[31:4])
);
        `endif

	assign samples_o[3:0] = 4'b0000;

`ifdef CHIPSCOPE
	wire [127:0] cs_data;
	
	assign cs_data[31:0] = max_samples_o;
	assign cs_data[63:32] = max_samples_i;
	assign cs_data[95:64] = sample_counter;
	assign cs_data[96] = adcfifo_full;
	assign cs_data[97] = adc_capture_go;
	assign cs_data[98] = fifo_read_fifoempty;
	assign cs_data[99] = fifo_read_fifoen;
	    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(adc_sampleclk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );
	
`endif

	
endmodule
