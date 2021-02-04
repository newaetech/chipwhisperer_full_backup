`include "includes.v"
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the example using the LX9 Board in USB (Serial) Mode.

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
    );

	wire ADC_clk_int;
	assign ADC_clk = ADC_clk_int;
	assign GPIO_LED1 = ~reset_i;

	openadc_interface oadc(
		.reset_i(reset_i),
		.clk_adcint(clk_100mhz),
		.clk_iface(clk_40mhz),
		.rxd(rxd),
		.txd(txd),		
		.LED_hbeat(GPIO_LED2),
		.LED_armed(GPIO_LED3),
		.LED_capture(GPIO_LED4),
		.ADC_Data(ADC_Data),
		.ADC_OR(ADC_OR),
		.ADC_clk(ADC_clk_int),
		.ADC_clk_feedback(ADC_clk_int),
		.DUT_CLK_i(DUT_CLK_i),
		.DUT_trigger_i(DUT_trigger_i),
		.amp_gain(amp_gain),
		.amp_hilo(amp_hilo),

		.reg_stream_i(1'b0),
		.reg_datai_i(8'd0),
		.reg_hyplen_i(16'd0),	
	
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
	);		 		
endmodule
