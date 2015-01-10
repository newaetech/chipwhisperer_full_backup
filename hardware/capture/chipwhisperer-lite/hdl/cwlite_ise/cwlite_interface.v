//`include "includes.v"
`default_nettype none

module cwlite_interface(  
    input wire         clk_usb,
      
    output wire        GPIO_LED1,
    output wire        GPIO_LED2,
    output wire        GPIO_LED3,
    output wire        GPIO_LED4,
	 output wire        GPIO_LED5,
	 
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
	 
	 /* XMEGA Programming - not used, but need to ensure line is floating */
	 input wire       target_PDID,
	 input wire       target_PDIC,
	 
	 /* Spare Lines - again not used right now, but avoid unintential pull-up/downs */
	 input wire 		target_nRST,
	 input wire 		target_MISO,
	 input wire 		target_MOSI,
	 input wire 		target_SCK,
	 
	 /* Target IO Interfaces */
	 inout wire			target_io4, // Normally trigger
	 inout wire			target_io3, // Normally Spare / SmartCard Comms
	 inout wire			target_io2, // Normally RXD
	 inout wire			target_io1, // Normally TXD / SmartCard Reset
	 inout wire			target_hs1, // Clock from victim device
	 inout wire			target_hs2, // Clock to victim device

	 output wire		glitchout_highpwr, // high-speed glitch output
	 output wire		glitchout_lowpwr, // high-speed glitch output
	 
	 /* Various connections to USB Chip */
	 input wire			USB_ser0_tx_i,
	 output wire		USB_ser0_rx_o	 
	 );
	
	
	/* PDI Programming done from SAM, must float these wires
	   or programming will fail from weak pull-down on FPGA */
	//assign XMEGA_PDID = 1'bZ;
	//assign XMEGA_PDIC = 1'bZ;
	
	//wire [35:0] cs_control0;
	//wire [63:0] ila_trigbus;
	
	wire reset_i;
	assign reset_i = 0;
	
	wire reset_intermediate;
	wire clk_usb_buf;
	
		IBUFG IBUFG_inst (
	.O(clk_usb_buf),
	.I(clk_usb) );

	assign GPIO_LED1 = 1'b0;
	assign GPIO_LED5 = 1'b0;
	
	wire reg_rst;
	wire [5:0] reg_addr;
	wire [15:0] reg_bcnt;
	wire [7:0] reg_datao;
	wire [7:0] reg_datai_cw;
	wire [7:0] reg_datai_reconfig;
	wire [7:0] reg_datai_glitch;
	wire [15:0] reg_size;
	wire reg_read;
	wire reg_write;
	wire reg_addrvalid;
	wire [5:0] reg_hypaddr;
	wire [15:0] reg_hyplen_cw;
	wire [15:0] reg_hyplen_glitch;
	wire [15:0] reg_hyplen_reconfig;
	
	wire ext_trigger;
	wire adv_trigger;
	wire extclk_mux;
	wire clkgen, glitchclk, adc_sample_clk;

	openadc_interface oadc(
		.reset_i(reset_i),
		.clk_adcint(clk_usb_buf),
		.clk_iface(clk_usb_buf),
		.clk_adcsample(adc_sample_clk),
		
		.USB_D(USB_D),
		.USB_Addr(USB_Addr),
		.USB_RDn(USB_RDn),
		.USB_WRn(USB_WRn),
		.USB_CEn(USB_CEn),
		.USB_ALEn(USB_ALEn),
	
		.LED_hbeat(GPIO_LED2),
		.LED_armed(GPIO_LED3),
		.LED_capture(GPIO_LED4),
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
		.reg_datai_i( reg_datai_cw | reg_datai_glitch | reg_datai_reconfig),
		.reg_size_o(reg_size),
		.reg_read_o(reg_read),
		.reg_write_o(reg_write),
		.reg_addrvalid_o(reg_addrvalid),
		.reg_stream_i(1'b0),
		.reg_hypaddress_o(reg_hypaddr),
		.reg_hyplen_i(reg_hyplen_cw |  reg_hyplen_glitch | reg_hyplen_reconfig)
		
	);	
	
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
		//.extclk_fpa_io(),
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
		//.trigger_ext_o(advio_trigger_line),
		.trigger_advio_i(1'b0),
		.trigger_anapattern_i(1'b0),
		.clkgen_i(clkgen),
		.glitchclk_i(glitchclk),
		
		.targetio1_io(target_io1),
		.targetio2_io(target_io2),
		.targetio3_io(target_io3),
		.targetio4_io(target_io4),
		
		.hsglitcha_o(glitchout_highpwr),
		.hsglitchb_o(glitchout_lowpwr),
		
		.uart_tx_i(USB_ser0_tx_i),
		.uart_rx_o(USB_ser0_rx_o),
		.usi_out_i(1'b0),
		.usi_in_o(),
				
		.trigger_o(ext_trigger)
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
		.exttrigger(ext_trigger)	
		);
	
`ifdef ENABLE_RECONFIG
	reg_reconfig reg_reconfig(
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
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
`else
	assign reg_hyplen_reconfig = 'd0;
	assign reg_datai_reconfig = 'd0;
`endif
	
		
	/*
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
    .CLK(clk_usb), // IN
    .TRIG0(ila_trigbus) // IN BUS [63:0]
	 );
	 */
 		
endmodule
