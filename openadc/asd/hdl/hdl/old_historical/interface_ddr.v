`include "includes.v"
//`define CHIPSCOPE 1
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the main interface.

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

module interface(
    input         reset_i,
    
`ifdef AVNET
    input         clk_40mhz,
    input         clk_100mhz,
	 inout			sda,
	 inout			scl,
`endif

`ifdef DLP_HS_FPGA
	 input			clk_66mhz,
`endif

`ifdef NEXYS2
	 input			clk_50mhz,
`endif

`ifdef SASEBOW
	 input         clk_100mhz,
	 input			clk_24mhz,
	 input			ADC_clk_feedback,
	 
`ifdef FAST_FTDI
	 input			ftdi_clk,	 
	 inout [7:0]  	ftdi_data,
	 input			ftdi_rxfn,
	 input			ftdi_txen,
	 output		 	ftdi_rdn,
	 output		 	ftdi_wrn,
	 output		 	ftdi_oen,	
	 output			ftdi_siwua,
`endif
	 
`endif
	 
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

 /* To avoid modifying UCF file we keep these even without Ethernet */
`ifdef OPT_ETH
	 ,input 		   eth_col,
    input 		   eth_crs,
	 output 		   eth_mdc,
	 inout  		   eth_mdio,
	 
	 output        eth_reset_n,
	 
	 input 		   eth_rx_clk,
	 input [3:0]   eth_rx_data,
	 input         eth_rx_dv,
	 input         eth_rx_er,
	 
	 input         eth_tx_clk,
	 output[3:0]   eth_tx_data,
	 output        eth_tx_en
`endif

`ifdef USE_SCARD
	,input			scard_present,
	output			scard_rst,
	inout 			scard_io
`endif

    );

/*
  wire [35:0]                          chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(ftdi_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );
	
	assign cs_data[7:0] = ftdi_data;
	assign cs_data[8] = ftdi_rxfn;
	assign cs_data[9] = ftdi_txen;
	assign cs_data[10] = ftdi_rdn;
	assign cs_data[11] = ftdi_wrn;
	assign cs_data[12] = ftdi_oen;
*/
	wire        slowclock;
	wire 			clk_100mhz_buf; 	
	wire			dcm_locked;
	wire 			reset_intermediate;
	
`ifdef AVNET
	assign slowclock = clk_40mhz;
	//These need pull-ups enabled to avoid screwing up parts on board
	assign scl = 1'bz;
	assign sda = 1'bz;
`endif

`ifdef DLP_HS_FPGA
	wire clk_100mhz;
	assign clk_100mhz = clk_66mhz;
	assign slowclock = clk_100mhz_buf;
`endif

`ifdef SASEBOW

`ifdef FAST_FTDI
	assign slowclock = ftdi_clk;
`else
	assign slowclock = clk_24mhz;
`endif
	assign clk_100mhz_buf = clk_100mhz;
`endif
	
`ifdef NEXYS2
	wire clk_100mhz;
	assign clk_100mhz = clk_50mhz;
	assign slowclock = clk_100mhz_buf;
`endif
	
	wire       phase_clk;
	wire [8:0] phase_requested;
	wire [8:0] phase_actual;
	wire 		phase_load;
	wire 		phase_done;
	
	wire 			adc_capture_go;
	wire			adc_capture_done;
	wire			armed;	  
   wire        reset;
                      
   assign GPIO_LED1 = ~reset_i;   
  
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
   assign GPIO_LED4 = adc_capture_go;
 
	//Frequency Measurement
	wire freq_measure;
	//BUFG buf_freqmeasure (.I(timer_heartbeat[25]), .O(freq_measure));
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
		//ADC_Data_tofifo <= ADC_Data;
		
		//Input Validation Test #1: Uncomment the following
		//ADC_Data_tofifo <= 10'hAA;
		
		//Input Validation Test #2: uncomment following, which should
		//put a perfect ramp. Tests FIFO & USB interface for proper
		//syncronization
		ADC_Data_tofifo <= ADC_Data_tofifo + 10'd1;
		
		//Input Validation Test #3: 
	end
   	
	wire [7:0] 	reg_status;
	
`ifdef CHIPSCOPE
  wire [35:0]                          chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 
  
/*
	wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(ADC_clk_sample), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );
*/
`endif 
	
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
	 
	wire cmdfifo_rxf;
	wire cmdfifo_txe;
	wire cmdfifo_rd;
	wire cmdfifo_wr;
	wire cmdfifo_isout;
	wire [7:0] cmdfifo_din;
	wire [7:0] cmdfifo_dout;
	
	wire [31:0] maxsamples_limit;
	wire [31:0] maxsamples;
	
`ifdef FAST_FTDI	 	
    assign ftdi_data = cmdfifo_isout ? cmdfifo_dout : 8'bZ;
	 assign cmdfifo_din = ftdi_data;
	 assign cmdfifo_rxf = ~ftdi_rxfn;
	 assign cmdfifo_txe = ~ftdi_txen;
	 assign ftdi_rdn = ~cmdfifo_rd;
	 assign ftdi_wrn = ~cmdfifo_wr;
	 assign ftdi_oen = cmdfifo_isout;
	 assign ftdi_siwua = 1'b1;
`else	
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
`endif
											  
`ifdef USE_SCARD

	 
	 wire [7:0] scard_cla, scard_ins, scard_p1, scard_p2, scard_async_data;
	 wire [4:0] scard_len_command, scard_len_response;
	 wire [127:0] scard_command, scard_response;
    wire scard_docmd, scard_busy, scard_async_datardy, scard_status;
	 wire [15:0] scard_resp_code;

	 serial_scard_hls_iface scard_inst(.reset_i(reset),
													.clk_i(slowclock),
													.scard_io(scard_io),
													.scard_cla(scard_cla),
													.scard_ins(scard_ins),
													.scard_p1(scard_p1),
													.scard_p2(scard_p2),
													.scard_len_command(scard_len_command),
													.scard_command(scard_command),
													.scard_len_response(scard_len_response),
													.scard_response(scard_response),
													.scard_status(scard_status),
													.scard_resp_code(scard_resp_code),	
													.async_data(scard_async_data),
													.async_datardy(scard_async_datardy),
													.do_cmd(scard_docmd),
													.busy(scard_busy));	
`endif


`ifdef USE_ETH
	wire [7:0]  ethusr_data;
	wire        ethusr_clken;
	wire        ethusr_start;
	wire        ethusr_clk;
	wire [15:0] ethusr_datalen;
	wire        ethusr_done;

	eth_phydirect phy(
	  .reset_i(reset),

     .eth_col(eth_col),
     .eth_crs(eth_crs),
	  .eth_mdc(eth_mdc),
	  .eth_mdio(eth_mdio),
	 
	  .eth_reset_n(eth_reset_n),
	  .eth_rx_clk(eth_rx_clk),
	  .eth_rx_data(eth_rx_data),
	  .eth_rx_dv(eth_rx_dv),
	  .eth_rx_er(eth_rx_er),
	 
	  .eth_tx_clk(eth_tx_clk),
	  .eth_tx_data(eth_tx_data),
	  .eth_tx_en(eth_tx_en),
	  
	  .usr_clk_o(ethusr_clk),
	  .usr_clken_o(ethusr_clken),
	  .usr_start_i(ethusr_start),
	  .usr_done_o(ethusr_done),
	  .usr_ethsrc_i(48'h000102030405),
	  .usr_ethdst_i(48'hD067E5455171),
	  .usr_ipsrc_i(32'hC0A8020A),
	  .usr_ipdst_i(32'hC0A80201),
	  .usr_data_len_i(ethusr_datalen),
	  .usr_udpport_i(16'd17209),
	  .usr_data_i(ethusr_data)
    );
`else
	 assign eth_mdc = 1'b0;
	 assign eth_mdio = 1'bZ;	 
	 assign eth_reset_n = 1'b0;
	 assign eth_tx_data = 4'd0;
	 assign eth_tx_en = 1'b0;
`endif

//`undef CHIPSCOPE
/*
   usb_interface usb(.reset_i(reset_i),
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

`ifdef USE_ETH
							, .eth_clk(ethusr_clk),
							.eth_clken(ethusr_clken),
							.eth_start(ethusr_start),
							.eth_done(ethusr_done),
							.eth_datalen(ethusr_datalen),
							.eth_data(ethusr_data)
`endif
							
`ifdef USE_SCARD
							,.scard_cla(scard_cla),
							.scard_ins(scard_ins),
							.scard_p1(scard_p1),
							.scard_p2(scard_p2),
							.scard_len_command(scard_len_command),
							.scard_command(scard_command),
							.scard_len_response(scard_len_response),
							.scard_response(scard_response),
							.scard_status(scard_status),
							.scard_resp_code(scard_resp_code),
							.scard_async_data(scard_async_data),
							.scard_async_datardy(scard_async_datardy),							
							.scard_present(scard_present),
							.scard_reset(scard_rst),
							.scard_docmd(scard_docmd),
							.scard_busy(scard_busy)
`endif

`ifdef CHIPSCOPE                     
                     , .chipscope_control(chipscope_control)
`endif

                     );  
*/


	wire reg_clk;
	wire [5:0] reg_address;
	wire [15:0] reg_bytecnt;
	wire [7:0] reg_datao;
	wire [15:0] reg_size;
	wire reg_read;
	wire reg_write;
	wire reg_addrvalid;
	wire [5:0] reg_hypaddress;
	wire reg_stream;
	wire [7:0] reg_datai;
	wire [15:0] reg_hyplen;
	
	reg_main registers_mainctl (
		.reset_i(reset_i), 
		.clk(slowclock), 
		.cmdfifo_rxf(cmdfifo_rxf), 
		.cmdfifo_txe(cmdfifo_txe), 
		.cmdfifo_rd(cmdfifo_rd), 
		.cmdfifo_wr(cmdfifo_wr), 
		.cmdfifo_din(cmdfifo_din), 
		.cmdfifo_dout(cmdfifo_dout), 
		.cmdfifo_isout(cmdfifo_isout), 
		.reg_clk(reg_clk), 
		.reg_address(reg_address), 
		.reg_bytecnt(reg_bytecnt), 
		.reg_datao(reg_datao), 
		.reg_datai(reg_datai), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_stream(reg_stream),
		.reg_hypaddress(reg_hypaddress), 
		.reg_hyplen(reg_hyplen)
	);	
	
	wire reg_stream_openadc;
	wire [15:0] reg_hyplen_openadc;
	
	reg_openadc registers_openadc (
		.reset_i(reset_i),
		.reset_o(reset_intermediate),
		.clk(reg_clk),
		.reg_address(reg_address), 
		.reg_bytecnt(reg_bytecnt), 
		.reg_datao(reg_datai), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_stream(reg_stream_openadc),
		.reg_hypaddress(reg_hypaddress), 
		.reg_hyplen(reg_hyplen_openadc),
		
		.gain(PWM_incr),
      .hilow(amp_hilo),
		.status(reg_status),         
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
		);
		
	wire reg_stream_fifo;
	wire [15:0] reg_hyplen_fifo;
	reg_openadc_adcfifo registers_fiforead(
		.reset_i(reset_i),
		.clk(reg_clk),
		.reg_address(reg_address), 
		.reg_bytecnt(reg_bytecnt), 
		.reg_datao(reg_datai), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_stream(reg_stream_fifo),
		.reg_hypaddress(reg_hypaddress), 
		.reg_hyplen(reg_hyplen_fifo),
		.fifo_empty(ddrfifo_empty),
		.fifo_data(ddrfifo_dout),
		.fifo_rd_en(ddrfifo_rd_en),
		.fifo_rd_clk(ddrfifo_rd_clk)
	);

	assign reg_stream = reg_stream_fifo | reg_stream_openadc;
	assign reg_hyplen = reg_hyplen_fifo | reg_hyplen_openadc;
	
`undef CHIPSCOPE
	clock_managment genclocks(
	 .reset(reset),
    .clk_100mhz(clk_100mhz_buf),
    .ext_clk(DUT_CLK_i),   
	 .adc_clk(ADC_clk),
`ifdef SASEBOW
	 .adc_clk_feedback(ADC_clk_feedback),
`endif
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

`ifndef SASEBOW
    .clk_100mhz_in(clk_100mhz),
	 .clk_100mhz_out(clk_100mhz_buf),
`endif
	 
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

//`undef CHIPSCOPE
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
			 		
endmodule
