`include "includes.v"
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:23:33 05/24/2010 
// Design Name: 
// Module Name:    interface 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////

module interface(
    input         reset_i,
    
    input         clk_40mhz,
    input         clk_100mhz,
	 inout			sda,
	 inout			scl,
	 
    input         rxd,
    output        txd,
       
    output        GPIO_LED1,
    output        GPIO_LED2,
    output        GPIO_LED3,
    output        GPIO_LED4,
	 
	 input [9:0]   ADC_Data,
	 input         ADC_OR,
	 output        ADC_clk,
	 input         DUT_CLK_i,
	 input         DUT_trigger_i,
	 output        amp_gain,
	 output        amp_hilo
	 
`ifdef USE_DDR
	 ,output [12:0] LPDDR_A,
	 output [1:0]  LPDDR_BA,
	 inout  [15:0] LPDDR_DQ,
	 output        LPDDR_LDM,
	 output        LPDDR_UDM,
	 input			LPDDR_LDQS,
	 input			LPDDR_UDQS,
	 output			LPDDR_CK_N,
	 output			LPDDR_CK_P,
	 output			LPDDR_CKE,
	 output			LPDDR_CAS_n,
	 output			LPDDR_RAS_n,
	 output			LPDDR_WE_n,
	 output			LPDDR_RZQ
`endif
    );

	wire        slowclock;
	wire        fastclock;
	
	assign slowclock = clk_40mhz;
	
	 wire       phase_clk;
	 wire       dcm_psen;
	 wire       dcm_psincdec;
	 wire       dcm_psdone;
	 wire [7:0] dcm_status;
	 wire [8:0] phase_requested;
	 wire [8:0] phase_actual;
	 wire 		phase_load;
	 wire 		phase_done;
	 wire       clkfx;
	 wire       locked;
	 wire       dcm_clk;
	
	//These need pull-ups enabled to avoid screwing up parts on board
	assign scl = 1'bz;
	assign sda = 1'bz;
  
   wire        reset;
                      
   assign GPIO_LED1 = ~reset_i;   
  
   //Global reset
   assign reset = reset_i;

   //Divide clock by 2^24 for heartbeat LED
	//Divide clock by 2^25 for frequency measurement
   reg [25:0] timer_heartbeat;
   always @(posedge slowclock)
      if (reset) begin
         timer_heartbeat <= 26'b0;
      end else begin
         timer_heartbeat <= timer_heartbeat +  26'd1;
      end	
      
   //Blink heartbeat LED
   assign GPIO_LED2 = timer_heartbeat[24];
	assign GPIO_LED3 = armed;
   assign GPIO_LED4 = fifo_full;
 
	//Frequency Measurement
	wire freq_measure;
	assign freq_measure = timer_heartbeat[25];
	reg [31:0] extclk_frequency_int;
	always @(posedge DUT_CLK_i or negedge freq_measure) begin
		if (freq_measure == 1'b0) begin
			extclk_frequency_int <= 32'd0;
		end else if (freq_measure == 1'b1) begin
			extclk_frequency_int <= extclk_frequency_int + 32'd1;
		end
	end
		
	reg [31:0] extclk_frequency;
	always @(negedge freq_measure) begin
		extclk_frequency <= extclk_frequency_int;
	end	
 
   wire ADC_clk_sample; 
    
	wire chipscope_clk;
	
	reg [9:0] ADC_Data_tofifo;
	
	always @(posedge ADC_clk_sample) begin
		ADC_Data_tofifo <= ADC_Data;
		
		//Input Validation Test: uncomment following, which should
		//put a perfect ramp. Tests FIFO & USB interface for proper
		//syncronization
		//ADC_Data_tofifo <= ADC_Data_tofifo + 10'd1;
	end
   	
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
		.Q(ADC_clk),   // 1-bit DDR output data
		.C0(ADC_clk_src), // 1-bit clock input
		.C1(~ADC_clk_src), // 1-bit clock input
		.CE(1'b1), // 1-bit clock enable input
		.D0(1'b1), // 1-bit data input (associated with C0)
		.D1(1'b0), // 1-bit data input (associated with C1)
		.R(1'b0),   // 1-bit reset input
		.S(1'b0)    // 1-bit set input
	);

  
	//Input to FIFO based on output register format
	wire [31:0] fifo_in;	
	reg [2:0]  	fifo_merge_cnt;
	reg        	fifo_or;
	reg			fifo_trigstat;
	reg			adc_capture_go;
	reg [9:0] 	adcsample [0:2];
	
	wire 			fifo_full;
	wire 			fifo_empty;
	reg 			fifo_wr_en;
	wire 			fifo_rd_en;
	wire 	[7:0] fifo_dout;
	wire [7:0] 	reg_status;
	wire 			fifo_has64;
	reg 			armed;
	
	reg [31:0]  sample_counter; //How many 3-sample tuples gone through fifo
	
	reg 			adc_capture_stop;
	
	always@(posedge ADC_clk_sample) begin
		if sample_counter > 1000000 begin
				adc_capture_stop = 1;
		end else begin
				adc_capture_stop = 0;
		end
	end
	
	always@(posedge ADC_clk_sample) begin
		if (~adc_capture_go) begin
			ADC_clk_sample <= 0;
			fifo_merge_cnt <= 0;
			fifo_wr_en <= 0;
		end else begin
		   fifo_adcsample[fifo_merge_cnt] = adcsample;
			fifo_or = ADC_OR;
			fifo_trigstat = DUT_trigger_i;
			if (fifo_merge_cnt == 3'b10) begin
				fifo_merge_cnt <= 0;
				fifo_wr_en <= 1;
				ADC_clk_sample <= ADC_clk_sample + 1;
			end else begin
				fifo_merge_cnt <= fifo_merge_cnt + 1;
				fifo_wr_en <= 0;
			end
		end
	end
	
	assign fifo_in[31] = fifo_or;
	assign fifo_in[30] = fifo_trigstat;
	assign fifo_in[29:20] = adcsample[2];
	assign fifo_in[19:10] = adcsample[1];
	assign fifo_in[9:0] = adcsample[0];
	
  `ifdef CHIPSCOPE
  wire [35:0]                          chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   );  
   
   wire [15:0] cs_data;
	 
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(chipscope_clk), // IN
    .TRIG0(cs_data) // IN BUS [255:0]
   );   

  `endif


	wire [31:0] adc_fifo_in


	//Generate ADC FIFO
	adc_fifo adcfifo (
	  .wr_clk(ADC_clk_sample), // input wr_clk
	  .rst(reset), // input rst
	  .rd_clk(fifo_rd_clk), // input rd_clk
	  .din(fifo_in), // input [31 : 0] din
	  .wr_en(fifo_wr_en), // input wr_en
	  .rd_en(fifo_rd_en), // input rd_en
	  .dout(fifo_dout), // output [31 : 0] dout
	  .full(fifo_full), // output full
	  .empty(fifo_empty), // output empty
	  .prog_full(fifo_has64) // has 64 words at least
	);  
	
	wire trigger;	
	assign trigger = DUT_trigger_i;
	
	//1 = trigger on high, 0 = trigger on low
	wire trigger_mode;
	
	//1 = wait for trigger to be INACTIVE before arming (e.g.: avoid triggering immediatly)
	//0 = arm as soon as cmd_arm goes high (e.g.: if trigger is already in active state, trigger)
	wire trigger_wait;
	
	//ADC Trigger Stuff	
	reg reset_arm;
	always @(posedge ADC_clk_sample or posedge reset) begin
		if (reset) begin
			adc_capture_go <= 0;
			reset_arm <= 0;
		end else begin
			if (adc_capture_stop) begin
				adc_capture_go <= 0;
				reset_arm <= 0;
			end else if ((trigger == trigger_mode) & armed) begin
				adc_capture_go <= 1;
				reset_arm <= 1;
			end
		end
	end	
	
	wire resetarm;
	wire cmd_arm;
	assign resetarm = reset | reset_arm;
	always @(posedge slowclock or posedge resetarm)
	  if (resetarm) begin
			armed <= 0;
		end else if (cmd_arm & ((trigger != trigger_mode) | (trigger_wait == 0))) begin
			armed <= 1;
		end
        
	assign reg_status[0] = armed;
   assign reg_status[1] = fifo_full;
	assign reg_status[2] = trigger;            







	wire [7:0] PWM_incr;

	wire ADC_clk_extsrc;
	wire ADC_clk_intsrc;
	wire ADC_clk_selection; //0=internal, 1=external

`undef CHIPSCOPE
   usb_interface usb(.reset(reset),
                     .clk(slowclock),
							.rx_in(rxd),
                     .tx_out(txd),
							.gain(PWM_incr),
                     .hilow(amp_hilo),
							.status(reg_status),
							.fifo_empty(ddrfifo_empty),
							.fifo_data(ddrfifo_dout),
							.fifo_rd_en(ddrfifo_rd_en),
							.fifo_rd_clk(ddrfifo_rd_clk),                     
							.cmd_arm(cmd_arm),
							.trigger_mode(trigger_mode),
							.trigger_wait(trigger_wait),                     
							.extclk_frequency(extclk_frequency),							
							.phase_o(phase_requested),
							.phase_ld_o(phase_load),
							.phase_i(phase_actual),
							.phase_done_i(phase_done),
							.phase_clk_o(phase_clk),
							.adc_clk_src_o(ADC_clk_selection)
							
`ifdef CHIPSCOPE                     
                     , .chipscope_control(chipscope_control)
`endif
                     );                      	 
	
	 dcm_phaseshift_interface dcmps(.clk_i(phase_clk),
											  .reset_i(reset),
											  .default_value_i(8'd0),
											  .value_i(phase_requested),
											  .load_i(phase_load),
											  .value_o(phase_actual),
											  .done_o(phase_done),
											  .dcm_psen_o(dcm_psen),
											  .dcm_psincdec_o(dcm_psincdec),
											  .dcm_psdone_i(dcm_psdone),
											  .dcm_status_i(dcm_status));	 
		 
		 
	 	
	assign ADC_clk_intsrc = clk_100mhz;		 
		 
	// BUFGMUX: Global Clock Mux Buffer
	// Spartan-6
	// Xilinx HDL Libraries Guide, version 13.2
	BUFGMUX #(
	.CLK_SEL_TYPE("SYNC") // Glitchles ("SYNC") or fast ("ASYNC") clock switch-over
	)
	BUFGMUX_inst (
	.O(ADC_clk_src), // 1-bit output: Clock buffer output
	.I0(ADC_clk_intsrc), // 1-bit input: Clock buffer input (S=0)
	.I1(ADC_clk_extsrc), // 1-bit input: Clock buffer input (S=1)
	.S(ADC_clk_selection) // 1-bit input: Clock buffer select
	);
	// End of BUFGMUX_inst instantiation
		 
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
	.CLKFX(ADC_clk_extsrc), // 1-bit output: Digital Frequency Synthesizer output (DFS)
	.LOCKED(locked), // 1-bit output: DCM_SP Lock Output
	.PSDONE(dcm_psdone), // 1-bit output: Phase shift done output
	.STATUS(dcm_status), // 8-bit output: DCM_SP status output
	.CLKFB(dcm_clk), // 1-bit input: Clock feedback input
	.CLKIN(DUT_CLK_i), // 1-bit input: Clock input
	.PSCLK(phase_clk), // 1-bit input: Phase shift clock input
	.PSEN(dcm_psen), // 1-bit input: Phase shift enable
	.PSINCDEC(dcm_psincdec), // 1-bit input: Phase shift increment/decrement input
	.RST(reset) // 1-bit input: Active high reset input
	);
	
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
	
	reg [8:0] PWM_accumulator;
	always @(posedge slowclock) PWM_accumulator <= PWM_accumulator[7:0] + PWM_incr;
	
	//assign amp_hilo = 1'b0;
	assign amp_gain = PWM_accumulator[8];	
	

endmodule
