`include "includes.v"
//`default_nettype none

module fifo_top(
    input wire    reset_i,
	 output wire	reset_o,
    input wire 	clk_100mhz_in,
	 output wire	clk_100mhz_out,
	 
	 //ADC Sample Input
	 input wire [9:0]   adc_datain,
	 input wire			adc_sampleclk,
	 input wire			adc_or,
	 input wire			adc_trig_status,
	 input wire			adc_capture_go, //Set to '1' to start capture, keep at 1 until adc_capture_stop goes high
	 output wire   	adc_capture_stop,
	 input wire       adc_capture_armed,
	 input wire			arm_i,
	 
	 //FIFO to USB Read Interface
	 input wire			fifo_read_fifoclk,
	 input wire			fifo_read_fifoen,
	 output wire			fifo_read_fifoempty,
	 output wire  [7:0]	fifo_read_data,
	 
	 input wire  [31:0] presample_i,
	 input wire  [31:0]	max_samples_i,
	 output wire [31:0]	max_samples_o,
	 output wire [31:0] samples_o,
	 
	 output wire      fifo_overflow, //If overflow happens (bad during stream mode)
	 input wire       stream_mode //1=Enable stream mode, 0=Normal
	 );
	 
	 parameter FIFO_FULL_SIZE = `MAX_SAMPLES - 128;
	 parameter FIFO_FULL_SIZE_LARGEWORDS = ((`MAX_SAMPLES - 32) / 3) / 4;

	wire				adc_capture_stop_int;
	reg 				adc_capture_stop_reg;
   wire           fifo_overflow_int;
	reg            fifo_overflow_reg;
	assign fifo_overflow = fifo_overflow_reg;
	assign adc_capture_stop_int = (stream_mode) ? fifo_overflow_int : adc_capture_stop_reg;
	assign adc_capture_stop = adc_capture_stop_int;
	  
	//Reset FIFO, clearning contents
	reg 				fifo_rst;
	
	reg  				adcfifo_wr_en;
	reg 				fifo_capture_en;
	
	//Number of triggered samples in FIFO
	reg [31:0]  	presample_counter;
	
	/** Combine 10-bit input samples into one 32-bit sample **/
	wire [31:0] 	adcfifo_in;	
	reg [9:0]		adcfifo_adcsample0;
	reg [9:0]		adcfifo_adcsample1;
	reg [9:0]		adcfifo_adcsample2;
	
	always @(posedge fifo_read_fifoclk) begin
		if (fifo_rst == 1'b1) begin
			fifo_overflow_reg <= 1'b0;
		end else if (fifo_overflow_int) begin
			fifo_overflow_reg <= 1'b1;
		end		
		
	end
	
	
	`ifdef NOBUFG_ADCCLK
	assign clk_100mhz_out = clk_100mhz_in;
	`else	
	IBUFG IBUFG_inst (
	.O(clk_100mhz_out),
	.I(clk_100mhz_in) );
	`endif
		
	assign reset_o = reset_i;
	
	//3 samples per 4 bytes
	assign max_samples_o = FIFO_FULL_SIZE ;
	 
	 
	reg [1:0]  		adcfifo_merge_cnt;
	always@(posedge adc_sampleclk) begin
		if (fifo_rst == 1) begin
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
	
	/** Keep fifi write enabled until we reach number of samples **/
	always@(posedge adc_sampleclk) begin
		if (~adc_capture_go)
			presample_counter <= FIFO_FULL_SIZE-6; //max_samples_i
		else 
			presample_counter <= presample_counter - 32'd1;		
	end
	
	always@(posedge adc_sampleclk) begin
		if((presample_counter == presample_i) && (stream_mode == 1'b0))
			adc_capture_stop_reg <= 1'b1;
		else
			adc_capture_stop_reg <= 1'b0;
	end
	
	always@(posedge adc_sampleclk) begin
		if (fifo_rst)
			fifo_capture_en <= 1'b1;
		else if (adc_capture_stop_int)
			fifo_capture_en <= 1'b0;
	end
	
	always@(posedge adc_sampleclk) begin
		if (fifo_capture_en == 0) begin
			adcfifo_wr_en <= 0;
		end else begin						
			if (adcfifo_merge_cnt == 'b10) begin
				adcfifo_wr_en <= 1;
			end else begin
				adcfifo_wr_en <= 0;
			end
		end
	end
	
	/** Reset FIFO on arm **/
	reg prev_arm, prev_arm_dly;
	always@(posedge adc_sampleclk) prev_arm <= arm_i;
	always@(posedge adc_sampleclk) prev_arm_dly <= prev_arm;
	always@(posedge adc_sampleclk) fifo_rst <= ~prev_arm_dly & prev_arm;

		
	reg [1:0] mergeloc;
	always @(posedge adc_sampleclk) begin
		if (~adc_capture_go)
			mergeloc <= 2'b11;
		else if (adc_capture_go && (mergeloc == 2'b11))
			mergeloc <= adcfifo_merge_cnt;
	end
	
	assign adcfifo_in[31:30] = mergeloc;
	assign adcfifo_in[29:20] = adcfifo_adcsample2;
	assign adcfifo_in[19:10] = adcfifo_adcsample1;
	assign adcfifo_in[9:0]	 = adcfifo_adcsample0;
	 
	 	
	/* Convert 128-bit to 8-bit */
	reg [15:0] byte_select;
	always @(posedge fifo_read_fifoclk)
		if (reset_i | fifo_rst)
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
		if (fifo_rst | reset_i | (adc_capture_go & ~stream_mode))
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
	
	wire adcfifo_full, fifo_empty;
	
	wire stream_write;
	assign stream_write = (stream_mode) ? adc_capture_go : 1'b1; //In stream mode we don't write until trigger	
	reg fifo_too_full;
	wire drain;
	assign drain = (stream_mode) ? 1'b0 : fifo_too_full; //In steam mode we don't drain FIFO, and rely on USB reading to do this
	
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
	*Data Count Options
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
	
	fifoonly_adcfifo fifoonly_adcfifo_inst (
	  .rst(fifo_rst | reset_i), // input rst
	  .wr_clk(adc_sampleclk), // input wr_clk
	  .rd_clk(fifo_read_fifoclk), // input rd_clk
	  .din(adcfifo_in), // input [31 : 0] din
	  .wr_en(adcfifo_wr_en & stream_write), // input wr_en
	  .rd_en((fifo_read_fifoen & read_en) | drain), // input rd_en
	  .dout(fifo_data), // output [127 : 0] dout
	  .full(adcfifo_full), // output full
	  .empty(fifo_empty), // output empty
	  .overflow(fifo_overflow_int), //
	  .rd_data_count(samples_o[31:4])
	);
	
	always @(posedge fifo_read_fifoclk) begin
		if (samples_o[31:4] > FIFO_FULL_SIZE_LARGEWORDS)
			fifo_too_full <= 1'b1;
		else
			fifo_too_full <= 1'b0;
	end

	assign samples_o[3:0] = 4'b0000;
/*
	wire [35:0] CONTROL0;
	wire [63:0] cs_data;
	
	coregen_icon icon (
    .CONTROL0(CONTROL0) // INOUT BUS [35:0]
	);
	
	coregen_ila ila (
    .CONTROL(CONTROL0), // INOUT BUS [35:0]
    .CLK(fifo_read_fifoclk), // IN
    .TRIG0(cs_data) // IN BUS [63:0]
	);

	assign cs_data[1:0] = mergeloc;
	assign cs_data[3:2] = adcfifo_merge_cnt;
	assign cs_data[4] = adc_capture_go;
	assign cs_data[5] = adcfifo_wr_en;
	assign cs_data[6] = adc_capture_stop_reg;
	assign cs_data[7] = read_en;
	assign cs_data[8] = fifo_read_fifoen;
	assign cs_data[9] = fifo_rst;
	assign cs_data[10] = fifo_too_full;
	assign cs_data[11] = drain;
	assign cs_data[31:16] = samples_o[19:4];
	assign cs_data[63:32] = presample_counter;
*/

endmodule
