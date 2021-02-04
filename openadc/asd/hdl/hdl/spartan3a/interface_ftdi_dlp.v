`include "includes.v"
//`define CHIPSCOPE 1
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the main interface.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/

module interface(
    input         reset_i,
    
	 input			clk_66mhz,
	 
    //input         rxd,
    //output        txd,
 
	 input 			cmdfifo_rxf_n,
	 input			cmdfifo_txe_n,
	 output			cmdfifo_rd_n,
	 output			cmdfifo_wr_n,	
	 inout [7:0]	cmdfifo_data,
		 
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
	 
`ifdef OPT_DDR
 /* To avoid modifying UCF file we keep these even in FIFO mode */
	 ,output [12:0] LPDDR_A,
	 output [1:0]  LPDDR_BA,
	 inout  [15:0] LPDDR_DQ,
	 output        LPDDR_LDM,
	 output        LPDDR_UDM,
	 inout			LPDDR_LDQS,
	 inout			LPDDR_UDQS,
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
	wire 			clk_100mhz_buf; 	
	wire			dcm_locked;
	wire 			reset_intermediate;

   reg [25:0] timer_heartbeat;
	wire clk_100mhz;
	assign clk_100mhz = clk_66mhz;
	assign slowclock = timer_heartbeat[8];
	
	wire       phase_clk;
	wire [8:0] phase_requested;
	wire [8:0] phase_actual;
	wire 		phase_load;
	wire 		phase_done;
	
	wire 			adc_capture_go;
	wire			adc_capture_done;
	wire			armed;	  
   wire        reset;
                      
   assign GPIO_LED1 = reset_i;   
  
   //Divide clock by 2^24 for heartbeat LED
	//Divide clock by 2^25 for frequency measurement
   always @(posedge clk_100mhz_buf)
      begin
         timer_heartbeat <= timer_heartbeat +  26'd1;
      end	
      
   //Blink heartbeat LED
   assign GPIO_LED2 = timer_heartbeat[24];
	assign GPIO_LED3 = armed;
   assign GPIO_LED4 = adc_capture_go;
 
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
	wire [9:0] trigger_level;
	
	always @(posedge ADC_clk_sample) begin
		ADC_Data_tofifo <= ADC_Data;
		
		//Input Validation Test #1: Uncomment the following
		//ADC_Data_tofifo <= 10'hAA;
		
		//Input Validation Test #2: uncomment following, which should
		//put a perfect ramp. Tests FIFO & USB interface for proper
		//syncronization
		//ADC_Data_tofifo <= ADC_Data_tofifo + 10'd1;
		
		//Input Validation Test #3: 
	end
   	
	wire [7:0] 	reg_status;
	
	
	//1 = trigger on high, 0 = trigger on low
	wire trigger_mode;
	
	//1 = wait for trigger to be INACTIVE before arming (e.g.: avoid triggering immediatly)
	//0 = arm as soon as cmd_arm goes high (e.g.: if trigger is already in active state, trigger)
	wire trigger_wait;
	wire cmd_arm;
	       
	trigger_unit tu_inst(
	 .reset(reset),
	 .clk(slowclock),				

    .adc_clk(ADC_clk_sample),		
	 .adc_data(ADC_Data_tofifo),

    .ext_trigger_i(DUT_trigger_i),
	 .trigger_level_i(trigger_mode),	 
	 .trigger_wait_i(trigger_wait),	 
	 .trigger_adclevel_i(trigger_level),
	 .trigger_source_i(trigger_source),
	 .trigger_now_i(trigger_now),
	 .arm_i(cmd_arm),
	 .arm_o(armed),
	 .capture_go_o(adc_capture_go),
	 .capture_done_i(adc_capture_done));		 
			 
	assign reg_status[0] = armed;
   assign reg_status[1] = ~adc_capture_go;
	assign reg_status[2] = DUT_trigger_i;
	assign reg_status[3] = dcm_locked;
	//reg_status[4]
	//reg_status[5]
	`ifdef USE_DDR
	assign reg_status[6] = 1'b1;
	`else
	assign reg_status[6] = 1'b0;
	`endif

	wire [7:0] PWM_incr;

	wire ADC_clk_selection; //0=internal, 1=external
	
	wire				ddr_read_req;
	wire				ddr_read_done;
	wire [31:0]		ddr_read_address;
	wire [7:0] 		ddrfifo_dout;
	wire				ddrfifo_empty;
	wire				ddrfifo_rd_en;
	wire				ddrfifo_rd_clk;
	 
	wire       cmdfifo_rxf;
	wire       cmdfifo_txe;
	wire       cmdfifo_wr;
	wire       cmdfifo_rd;
	wire [7:0] cmdfifo_din;
	wire [7:0] cmdfifo_dout;
	wire       cmdfifo_isout;
	
	reg reg_cmdfifo_wr_n;
	
	assign cmdfifo_rxf = ~cmdfifo_rxf_n;
	assign cmdfifo_txe = ~cmdfifo_txe_n;
	assign cmdfifo_wr_n = reg_cmdfifo_wr_n;
	assign cmdfifo_rd_n = ~cmdfifo_rd;
	
	always @(posedge slowclock) begin
		reg_cmdfifo_wr_n <= ~cmdfifo_wr;
	end
	
	wire [31:0] maxsamples_limit;
	wire [31:0] maxsamples;
	
	
	/*
	serial_reg_iface cmdfifo_serial(.reset_i(reset),
											  .clk_i(slowclock),
											  .rx_i(rxd),
											  .tx_o(txd),
											  .cmdfifo_rxf(cmdfifo_rxf),
											  .cmdfifo_txe(cmdfifo_txe),
											  .cmdfifo_rd(cmdfifo_rd),
											  .cmdfifo_wr(cmdfifo_wr),
											  .cmdfifo_din(cmdfifo_din),
											  .cmdfifo_dout(cmdfifo_dout));
	*/
	
	assign cmdfifo_data = cmdfifo_isout ? cmdfifo_dout : 8'bZ;
   assign cmdfifo_din = cmdfifo_data;

`undef CHIPSCOPE
   usb_interface usb(.reset_i(1'b0),
							.reset_o(reset_intermediate),
                     .clk(slowclock),
							.cmdfifo_rxf(cmdfifo_rxf),
							.cmdfifo_txe(cmdfifo_txe),
							.cmdfifo_rd(cmdfifo_rd),
							.cmdfifo_wr(cmdfifo_wr),
							.cmdfifo_din(cmdfifo_din),
							.cmdfifo_dout(cmdfifo_dout),
							.cmdfifo_isout(cmdfifo_isout),
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
							.trigger_source(trigger_source),
							.trigger_level(trigger_level),
							.trigger_now(trigger_now),
							.extclk_frequency(extclk_frequency),							
							.phase_o(phase_requested),
							.phase_ld_o(phase_load),
							.phase_i(phase_actual),
							.phase_done_i(phase_done),
							.phase_clk_o(phase_clk),
							.maxsamples_i(maxsamples_limit),
							.maxsamples_o(maxsamples),
							.adc_clk_src_o(ADC_clk_selection)
`ifdef USE_DDR
							, .ddr_address(ddr_read_address),
							.ddr_rd_req(ddr_read_req),
							.ddr_rd_done(ddr_read_done)
`endif

						
`ifdef CHIPSCOPE                     
                     , .chipscope_control(chipscope_control)
`endif

                     );                      	 
	
	clock_managment genclocks(
	 .reset(reset),
    .clk_100mhz(clk_100mhz_buf),
    .ext_clk(DUT_CLK_i),   
	 .adc_clk(ADC_clk),
	 .use_ext_clk(ADC_clk_selection),
	 
	 .systemsample_clk(ADC_clk_sample),

	 .phase_clk(phase_clk),
	 .phase_requested(phase_requested),
	 .phase_actual(phase_actual),
	 .phase_load(phase_load),
	 .phase_done(phase_done),
	 
	 .dcm_locked(dcm_locked)
    );
			 
	reg [8:0] PWM_accumulator;
	always @(posedge slowclock) PWM_accumulator <= PWM_accumulator[7:0] + PWM_incr;
	
	//assign amp_hilo = 1'b0;
	assign amp_gain = PWM_accumulator[8];
	
`ifdef USE_DDR
//`define CHIPSCOPE 1
	ddr_top ddr(
    .reset_i(reset_intermediate),
	 .reset_o(reset),
    .clk_100mhz_in(clk_100mhz),
	 .clk_100mhz_out(clk_100mhz_buf),
	 
	 //ADC Sample Input
	 .adc_datain(ADC_Data_tofifo),
	 .adc_sampleclk(ADC_clk_sample),
	 .adc_or(ADC_OR),
	 .adc_trig_status(DUT_trigger_i),
	 .adc_capture_go(adc_capture_go), //Set to '1' to start capture, keep at 1 until adc_capture_stop goes high
	 .adc_capture_stop(adc_capture_done),
	 
	 .max_samples_i(maxsamples),
	 .max_samples_o(maxsamples_limit),
	 
	 //DDR to USB Read Interface
	 .ddr_read_req(ddr_read_req),
	 .ddr_read_done(ddr_read_done),
	 .ddr_read_address(ddr_read_address[29:0]),
	 .ddr_read_fifoclk(ddrfifo_rd_clk),
	 .ddr_read_fifoen(ddrfifo_rd_en),
	 .ddr_read_fifoempty(ddrfifo_empty),
	 .ddr_read_data(ddrfifo_dout),
	 .ddr_cal_done(reg_status[5]),
	 .ddr_error(reg_status[4]),
	 
	 //DDR HW Interface
	 .LPDDR_A(LPDDR_A),
	 .LPDDR_BA(LPDDR_BA),
	 .LPDDR_DQ(LPDDR_DQ),
	 .LPDDR_LDM(LPDDR_LDM),
	 .LPDDR_UDM(LPDDR_UDM),
	 .LPDDR_LDQS(LPDDR_LDQS),
	 .LPDDR_UDQS(LPDDR_UDQS),
	 .LPDDR_CK_N(LPDDR_CK_N),
	 .LPDDR_CK_P(LPDDR_CK_P),
	 .LPDDR_CKE(LPDDR_CKE),
	 .LPDDR_CAS_n(LPDDR_CAS_n),
	 .LPDDR_RAS_n(LPDDR_RAS_n),
	 .LPDDR_WE_n(LPDDR_WE_n),
	 .LPDDR_RZQ(LPDDR_RZQ)
`ifdef CHIPSCOPE
	 ,.chipscope_control(chipscope_control)
`endif	 
	 );
`else		

//`define CHIPSCOPE 1
	fifo_top fifo_top_inst(
    .reset_i(reset_intermediate),
	 .reset_o(reset),
    .clk_100mhz_in(clk_100mhz),
	 .clk_100mhz_out(clk_100mhz_buf),
	 
	 //ADC Sample Input
	 .adc_datain(ADC_Data_tofifo),
	 .adc_sampleclk(ADC_clk_sample),
	 .adc_or(ADC_OR),
	 .adc_trig_status(DUT_trigger_i),
	 .adc_capture_go(adc_capture_go), //Set to '1' to start capture, keep at 1 until adc_capture_stop goes high
	 .adc_capture_stop(adc_capture_done),
	  
	 //DDR to USB Read Interface
	 .fifo_read_fifoclk(ddrfifo_rd_clk),
	 .fifo_read_fifoen(ddrfifo_rd_en),
	 .fifo_read_fifoempty(ddrfifo_empty),
	 .fifo_read_data(ddrfifo_dout),

	 .max_samples_i(maxsamples),
	 .max_samples_o(maxsamples_limit)

`ifdef CHIPSCOPE
	 ,.chipscope_control(chipscope_control)
`endif	 
	 );


`ifdef OPT_DDR
	 /* To avoid modifying UCF file we keep these even in FIFO mode */
	 assign LPDDR_A = 0;
	 assign LPDDR_BA = 0;
	 assign LPDDR_DQ = 'bz;
	 assign LPDDR_LDM = 0;
	 assign LPDDR_UDM = 0;
	 assign LPDDR_LDQS = 'bz;
	 assign LPDDR_UDQS = 'bz;
	 assign LPDDR_CKE = 0;
	 assign LPDDR_CAS_n = 1;
	 assign LPDDR_RAS_n = 1;
	 assign LPDDR_WE_n = 1;
	 assign LPDDR_RZQ = 1;
	 
	 OBUFDS #(
		.IOSTANDARD("DEFAULT") // Specify the output I/O standard
		) OBUFDS_inst (
			.O(LPDDR_CK_P), // Diff_p output (connect directly to top-level port)
			.OB(LPDDR_CK_N), // Diff_n output (connect directly to top-level port)
			.I(1'b1) // Buffer input
		);
`endif
	 
`endif
			 		
endmodule
