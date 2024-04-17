/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2016-2017, NewAE Technology Inc. All rights reserved.
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

//`include "includes.v"
`default_nettype none

module cw1200_interface(  
    input wire         clk_usb,
      
    output wire        LED_TRIGGERED,
	 output wire        LED_GLITCHTRIGGERED,
    output wire        LED_ADCR,
	 output wire        LED_ADCG,
	 output wire        LED_CLKGENR,
	 output wire        LED_CLKGENG,
	 output wire        LED_GLITCHR,
	 output wire        LED_GLITCHG,
	 
	 output wire        LED_AUXI,
	 output wire        LED_AUXO,
	 inout wire 		  SMAAUX,
	 
	 /* FPGA - USB Interface */
	 inout wire [7:0]	USB_D,
	 input wire [7:0]	USB_Addr,
	 input wire			USB_RDn,
	 input wire			USB_WRn,
	 input wire			USB_CEn,
	 input wire			USB_ALEn,

	 /* ADC Interface */
	 input wire [9:0]   ADC_Data,
	 input wire        ADC_OR,
	 output wire       ADC_clk,
	 input wire			 ADC_clk_fb,
//	 input         DUT_CLK_i,
//	 input         DUT_trigger_i,
	 output wire       amp_gain,
	 output wire       amp_hilo,
	 
	 /* XMEGA Programming  */
	 output wire       target_PDIDTX,
    input wire        target_PDIDRX,
	 inout wire       target_PDIC,
	 
	 /* Spare Lines - AVR Programming */
	 inout wire 		target_nRST,
	 input wire 		target_MISO,
	 inout wire 		target_MOSI,
	 inout wire 		target_SCK,
	 	 
	 /* Target IO Interfaces */
	 inout wire			target_io4, // Normally trigger
	 inout wire			target_io3, // Normally Spare / SmartCard Comms
	 inout wire			target_io2, // Normally RXD
	 inout wire			target_io1, // Normally TXD / SmartCard Reset
	 inout wire			target_hs1, // Clock from victim device
	 inout wire			target_hs2, // Clock to victim device

	 output wire		glitchout_highpwr, // high-speed glitch output
	 output wire		glitchout_lowpwr, // high-speed glitch output 
	 
	 output wire      target_npower,
	 
	 /* Smart Card Connections */
	 inout wire       sc_rst,
	 inout wire       sc_aux1,
	 inout wire       sc_aux2,
	 inout wire       sc_clk,	 
	 inout wire       sc_io,
	 input wire       sc_present,	 
	 
	 /* Various connections to USB Chip */
	 input wire			USB_TXD3,
	 output wire		USB_RXD3,
	 
	 input wire			USB_TXD2,
	 output wire		USB_RXD2,
	 inout wire			USB_SCK2,
	 
	 input wire       USB_TXD0,
	 output wire      USB_RXD0,
	 input wire       USB_SCK0,
	 input wire       USB_RTS0,
	 input wire       USB_CTS0,
	 
	 //These control the direction of the PDID/PDIC pins
	 input wire       USB_PDID_WR, //USB_A18 (PC26)
	 input wire       USB_PDIC_EN, //USB_A17 (PC25)
	 
	 input wire       USB_TXD1,
	 output wire      USB_RXD1,
	 inout wire       USB_SCK1,
	 
	 input wire       USB_spare0,
	 input wire       USB_spare1,
	 input wire			USB_spare2
	 );


	//wire [35:0] cs_control0;
	//wire [63:0] ila_trigbus;
	
	wire reset_i;
	assign reset_i = 0;
	
	wire reset_intermediate;
	wire clk_usb_buf;
	
	wire clk_usb_buf0, clk_usb_buf1;
	assign clk_usb_buf = clk_usb_buf0;
	
	// Temp
	 wire sc_enable = 1'b1;
	 
	 assign sc_rst = (sc_enable) ? USB_spare2 : 1'bZ;
	 assign sc_clk = (sc_enable) ? USB_SCK0 : 1'bZ;
	 assign sc_aux1 = (sc_enable) ? USB_RTS0 : 1'bZ;
	 assign sc_aux2 = (sc_enable) ? USB_CTS0 : 1'bZ;
	 assign sc_io = (sc_enable) ? ((USB_TXD0) ? 1'b0 : 1'bZ) : 1'bZ;
	 assign USB_RXD0 = (sc_enable) ? sc_io : 1'bZ;

   // Clocking primitive
   //------------------------------------
   // Instantiation of the PLL primitive
   //    * Unused inputs are tied off
   //    * Unused outputs are labeled unused
   wire [15:0] pll_do_unused;
   wire        pll_drdy_unused;
   wire        pll_locked_unused;
   wire        pll_clkfbout;
   wire        pll_clkfbout_buf;
	wire        pll_clkout_12mhz;
   wire        pll_clkout2_unused;
   wire        pll_clkout3_unused;
   wire        pll_clkout4_unused;
   wire        pll_clkout5_unused;
   wire		   pll_clkin1, pll_clkout0;
	wire			clk_usb_12mhz;
  
  	IBUFG clkin1_buf
   (.O (pll_clkin1), // clk_usb_buf0
    .I (clk_usb));

	PLL_BASE
	 #(.BANDWIDTH              ("LOW"),
	 .CLK_FEEDBACK           ("CLKFBOUT"),
	 .COMPENSATION           ("SYSTEM_SYNCHRONOUS"),
	 .DIVCLK_DIVIDE          (1),
	 .CLKFBOUT_MULT          (5),
	 .CLKFBOUT_PHASE         (0.000),
	 .CLKOUT0_DIVIDE         (5),
	 .CLKOUT0_PHASE          (0.000),
	 .CLKOUT0_DUTY_CYCLE     (0.500),
	 .CLKOUT1_DIVIDE         (40),
	 .CLKOUT1_PHASE          (0.000),
	 .CLKOUT1_DUTY_CYCLE     (0.500),
	 .CLKIN_PERIOD           (10.41667),
	 .REF_JITTER             (0.050))
	pll_base_inst
	 // Output clocks
	(.CLKFBOUT              (pll_clkfbout),
	 .CLKOUT0               (pll_clkout0),
	 .CLKOUT1               (pll_clkout_12mhz),
	 .CLKOUT2               (pll_clkout2_unused),
	 .CLKOUT3               (pll_clkout3_unused),
	 .CLKOUT4               (pll_clkout4_unused),
	 .CLKOUT5               (pll_clkout5_unused),
	 .LOCKED                (pll_locked_unused),
	 .RST                   (1'b0),
	  // Input clock control
	 .CLKFBIN               (pll_clkfbout_buf),
	 .CLKIN                 (pll_clkin1));


	// Feedback buffer
	BUFG clkf_buf
	(.O (pll_clkfbout_buf),
	 .I (pll_clkfbout));
		
	//Output buffer - force to use I1 input
	//as allows placement in pair site with
	//"oadc/genclocks/clkgenfx_mux"
	 BUFGMUX
		clkout0_buf (
		.O(clk_usb_buf0), // 1-bit output: Clock buffer output
	//	.I0(1'b0), // 1-bit input: Clock buffer input (S=0)
		.I1(pll_clkout0), // 1-bit input: Clock buffer input (S=1)
		.S(1'b1) // 1-bit input: Clock buffer select
	);
	
	/*
	BUFG
	  clkout1_buf (
	   .O(clk_usb_12mhz),
		.I(pll_clkout_12mhz)
	);
	*/
	assign clk_usb_12mhz = pll_clkout_12mhz;

	//Pass raw output as this goes to a BUFGMUX
	assign clk_usb_buf1 = pll_clkout0;
	
	wire reg_rst;
	wire [5:0] reg_addr;
	wire [15:0] reg_bcnt;
	wire [7:0] reg_datao;
	wire [7:0] reg_datai_cw;
	wire [7:0] reg_datai_reconfig;
	wire [7:0] reg_datai_sad;
	wire [7:0] reg_datai_glitch;
	wire [7:0] reg_datai_decode;
	wire [15:0] reg_size;
	wire reg_read;
	wire reg_write;
	wire reg_addrvalid;
	wire [5:0] reg_hypaddr;
	wire [15:0] reg_hyplen_cw;
	wire [15:0] reg_hyplen_glitch;
	wire [15:0] reg_hyplen_reconfig;
	wire [15:0] reg_hyplen_sad;
	wire [15:0] reg_hyplen_decode;
	
	wire [9:0] ADC_Data_int;
	wire       ADC_Clk_int;
	
	wire ext_trigger;
	wire adv_trigger;
	wire extclk_mux;
	wire clkgen, glitchclk, adc_sample_clk;
	wire enable_avrprog;
	
	wire ADC_DCM_Unlock, CLKGEN_DCM_Unlock, Glitch_DCM_Unlock;
	
	assign LED_ADCR = ADC_DCM_Unlock;
	assign LED_ADCG = ~ADC_DCM_Unlock;
	assign LED_CLKGENR = CLKGEN_DCM_Unlock;
	assign LED_CLKGENG = ~CLKGEN_DCM_Unlock;
	assign LED_GLITCHR = Glitch_DCM_Unlock;
	assign LED_GLITCHG = ~Glitch_DCM_Unlock;

	openadc_interface oadc(
		.reset_i(reset_i),
		.clk_adcint(clk_usb_buf1), //Potentially could be external clock
		.clk_iface(clk_usb_buf),
		.clk_adcsample(adc_sample_clk),
		
		.USB_D(USB_D),
		.USB_Addr(USB_Addr),
		.USB_RDn(USB_RDn),
		.USB_WRn(USB_WRn),
		.USB_CEn(USB_CEn),
		.USB_ALEn(USB_ALEn),
	
		//.LED_hbeat(LED_HBEAT),
		.LED_armed(LED_TRIGGERED),
		.LED_ADCDCMUnlock(ADC_DCM_Unlock),
		.LED_CLKGENDCMUnlock(CLKGEN_DCM_Unlock),
		.ADC_Data(ADC_Data),
		.ADC_OR(ADC_OR),
		.ADC_clk(ADC_clk),
		.ADC_clk_feedback(ADC_clk_fb),
		.DUT_CLK_i(extclk_mux),
		.DUT_trigger_i(ext_trigger),
		.amp_gain(amp_gain),
		.amp_hilo(amp_hilo),
		.target_clk(clkgen),

		.reg_reset_o(reg_rst),
		.reg_address_o(reg_addr),
		.reg_bytecnt_o(reg_bcnt),
		.reg_datao_o(reg_datao),
		.reg_datai_i( reg_datai_cw | reg_datai_glitch | reg_datai_reconfig | reg_datai_sad | reg_datai_decode),
		.reg_size_o(reg_size),
		.reg_read_o(reg_read),
		.reg_write_o(reg_write),
		.reg_addrvalid_o(reg_addrvalid),
		.reg_stream_i(1'b0),
		.reg_hypaddress_o(reg_hypaddr),
		.reg_hyplen_i(reg_hyplen_cw |  reg_hyplen_glitch | reg_hyplen_reconfig | reg_hyplen_sad | reg_hyplen_decode)
		
		,.ADC_Data_out(ADC_Data_int),
		.ADC_Clk_out(ADC_Clk_int)
		
	);	
	
		wire apatt_trigger;
		wire decode_trigger;
		wire advio_trigger_line;
		
	   wire enable_output_nrst;
	   wire output_nrst;
           wire nrst_ignore_highz;
	   wire enable_output_pdid;
	   wire output_pdid;
	   wire enable_output_pdic;
	   wire output_pdic;
	
		reg_chipwhisperer reg_chipwhisperer(
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_cw), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_cw),
		.reg_stream(),
		.extclk_fpa_io(SMAAUX),
		//.extclk_fpb_i(1'b0),
		.extclk_pll_i(1'b0),
		.extclk_rearin_i(target_hs1),
		.extclk_rearout_o(target_hs2),
		.extclk_o(extclk_mux),
		.adc_sample_clk(adc_sample_clk),
		//.trigger_fpa_i(),
		//.trigger_fpb_i(),
		.trigger_io1_i(target_io1),
		.trigger_io2_i(target_io2),
		.trigger_io3_i(target_io3),
		.trigger_io4_i(target_io4),
		.trigger_nrst_i(target_nRST),
		.trigger_ext_o(advio_trigger_line),
		.trigger_advio_i(1'b0),
		.trigger_anapattern_i(apatt_trigger),
		.trigger_decodedio_i(decode_trigger),
		.clkgen_i(clkgen),
		.glitchclk_i(glitchclk),
		
		.targetio1_io(target_io1),
		.targetio2_io(target_io2),
		.targetio3_io(target_io3),
		.targetio4_io(target_io4),
		
		.hsglitcha_o(glitchout_highpwr),
		.hsglitchb_o(glitchout_lowpwr),
		
		.enable_avrprog(enable_avrprog),
		
		.enable_output_nrst(enable_output_nrst),
	   .output_nrst(output_nrst),
           .nrst_ignore_highz(nrst_ignore_highz),
	   .enable_output_pdid(enable_output_pdid),
	   .output_pdid(output_pdid),
	   .enable_output_pdic(enable_output_pdic),
	   .output_pdic(output_pdic),
		
		.uart_tx_i(USB_TXD3),
		.uart_rx_o(USB_RXD3),
		.usi_out_i(1'b0),
		.usi_in_o(),
		.targetpower_off(target_npower),
				
		.trigger_o(ext_trigger),
		.led_auxi(LED_AUXI),
		.led_auxo(LED_AUXO)
	);

	reg_clockglitch reg_clockglitch(
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_glitch), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_glitch),
		.reg_stream(),
		.sourceclk0(target_hs1),
		.sourceclk1(clkgen),
		.glitchclk(glitchclk),
		.exttrigger(ext_trigger),
		.dcm_unlocked(Glitch_DCM_Unlock),
		.led_glitch(LED_GLITCHTRIGGERED)
		);

	reg_reconfig reg_reconfig(
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.icap_clk(clk_usb_12mhz),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_reconfig), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_reconfig),
		.reg_stream()
		);
		
			reg_sad registers_sad (
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_sad), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_sad),
		.reg_stream(),
		.ADC_data(ADC_Data_int),
		.ADC_clk(ADC_Clk_int),
		.trig_out(apatt_trigger)
	);
	
			reg_decodeiotrigger registers_decodetrigger (
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_decode), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_decode),
		.reg_stream(),
		
		.ext_iomux(advio_trigger_line),
		.sck(target_SCK),
		.mosi(target_MOSI),
		.miso(target_MISO),
		.pdid_cs(target_PDIDRX),
		.pdic(target_PDIC),
				
		.trig_out(decode_trigger)
	);
	
	 wire target_highz = target_npower;
								 
	 assign target_nRST = (target_highz && ~nrst_ignore_highz) ? 1'bZ :
	                      (enable_avrprog) ? USB_spare2 :
								 (enable_output_nrst) ? output_nrst :
								 1'bZ;
								 
	 assign target_MOSI = (target_highz) ? 1'bZ:
	                      (enable_avrprog) ? USB_TXD2 : 1'bZ;
	 assign target_SCK = (target_highz) ? 1'bZ:
							   (enable_avrprog) ? USB_SCK2 : 1'bZ;
	 assign USB_RXD2 = (enable_avrprog) ? target_MISO : 1'b0;	
	 
	 
	 //XMEGA Programming uses spare pins to select direction
	 assign target_PDIDTX = (target_highz) ? 1'bZ:
	                        (enable_output_pdid) ? output_pdid :
									(USB_PDID_WR) ? USB_TXD1 : 1'bZ;
	 assign USB_RXD1 = target_PDIDRX;
	 assign target_PDIC = (target_highz) ? 1'bZ:
	                      (enable_output_pdic) ? output_pdic :
								 (USB_PDIC_EN) ? USB_SCK1 : 1'bZ;
		
	/*
	wire [63:0] ila_trigbus;
	wire [35:0] cs_control0;
	assign ila_trigbus[7:0] = USB_D;
	assign ila_trigbus[15:8] = USB_Addr;
	assign ila_trigbus[16] = USB_RDn;
	assign ila_trigbus[17] = USB_WRn;
	assign ila_trigbus[18] = USB_ALEn;
	assign ila_trigbus[19] = USB_CEn;

	coregen_icon csicon (
    .CONTROL0(cs_control0) // INOUT BUS [35:0]
	);
	
	coregen_ila csila (
    .CONTROL(cs_control0), // INOUT BUS [35:0]
    .CLK(clk_usb_buf), // IN
    .TRIG0(ila_trigbus) // IN BUS [63:0]
	 );
	*/

	
endmodule
