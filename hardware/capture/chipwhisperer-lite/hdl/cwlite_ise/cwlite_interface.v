`include "includes.v"
`default_nettype none

module cwlite_interface(  
    input wire         clk_usb,
      
    //GPIO_LED1 was removed from final version of CW-Lite design
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
	 inout wire       target_PDID,
	 inout wire       target_PDIC,
	 
	 /* Spare Lines - AVR Programming */
	 inout wire 		target_nRST,
	 input wire 		target_MISO,
	 output wire 		target_MOSI,
	 output wire 		target_SCK,
	 	 
	 /* Target IO Interfaces */
	 inout wire			target_io4, // Normally trigger
	 inout wire			target_io3, // Normally Spare / SmartCard Comms
	 inout wire			target_io2, // Normally RXD
	 inout wire			target_io1, // Normally TXD / SmartCard Reset
	 inout wire			target_hs1, // Clock from victim device
	 inout wire			target_hs2, // Clock to victim device
	 
	 inout wire			sc_rst,
	 inout wire			sc_clk,
	 inout wire			sc_io,
	 inout wire			sc_aux1,
	 inout wire			sc_aux2,

	 output wire		glitchout_highpwr, // high-speed glitch output
	 output wire		glitchout_lowpwr, // high-speed glitch output 
	 
	 output wire		target_npower,
	 
	 output wire      trigger_out, //trigger out pin CW-Lite
	 
	 /* Various connections to USB Chip */
	 input wire			USB_ser0_tx_i,
	 output wire		USB_ser0_rx_o,

	 input wire			USB_spi0_sck_i,
	 input wire 		USB_spi0_mosi_i,
	 output wire 		USB_spi0_miso_o,
	 input wire			USB_spi0_cs0,
	 input wire			USB_treset_i,
	 
	 input wire			USB_sc_rst,
	 input wire 		USB_sc_clk,
	 input wire			USB_sc_aux1,
	 input wire			USB_sc_aux2,
	 input wire 		USB_spi2_txd2,
	 output wire		USB_spi2_rxd2,
	 
	 input wire       USB_spare1, //CS pin from SAM3U
	 input wire			USB_spare2, //LCD_D/C Pin (Data/Command) or CS pin TO SAM3U
	 
	 output wire		ext_mosi, //Pin 4 of external header
	 input wire			ext_miso, //Pin 3 of external header
	 output wire		ext_sck,  //Pin 2 of external header
	 output wire		lcd_cs,
	 output wire		lcd_dc,
	 output wire      avr_cs
	 //output wire		lcd_rst
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
	
        `ifndef __ICARUS__
            IBUFG IBUFG_inst (
            .O(clk_usb_buf),
            .I(clk_usb) );
        `else
            assign clk_usb_buf = clk_usb;
        `endif
	
	//Change in ICAP - clock now generated outside. 
	//TODO: Check skew is OK still
	wire clk_slow;
	reg [2:0] slowcnt;
	always @(posedge clk_usb_buf) begin
		slowcnt <= slowcnt + 3'd1;
	end
	assign clk_slow = slowcnt[2];
	
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
	wire enable_avrprog;

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
	
		.LED_hbeat(GPIO_LED5),
		.LED_armed(GPIO_LED3),
		.LED_ADCDCMUnlock(GPIO_LED2),
		.LED_CLKGENDCMUnlock(GPIO_LED4),
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
		//.extclk_fpa_io(),
		//.extclk_fpb_i(1'b0),
		.extclk_pll_i(1'b0),
		.extclk_rearin_i(target_hs1),
		.extclk_rearout_o(target_hs2),
		.extclk_o(extclk_mux),
		.adc_sample_clk(adc_sample_clk),
		//.trigger_fpa_i(),
		.trigger_io1_i(target_io1),
		.trigger_io2_i(target_io2),
		.trigger_io3_i(target_io3),
		.trigger_io4_i(target_io4),
        .trigger_nrst_i(target_nRST),
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
		
		.enable_avrprog(enable_avrprog),
		
		.enable_output_nrst(enable_output_nrst),
	   .output_nrst(output_nrst),
           .nrst_ignore_highz(nrst_ignore_highz),
	   .enable_output_pdid(enable_output_pdid),
	   .output_pdid(output_pdid),
	   .enable_output_pdic(enable_output_pdic),
	   .output_pdic(output_pdic),
		
		.uart_tx_i(USB_ser0_tx_i),
		.uart_rx_o(USB_ser0_rx_o),
		.usi_out_i(1'b0),
		.usi_in_o(),
		.targetpower_off(target_npower),
				
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
		
	 assign trigger_out = ext_trigger;
	
`ifdef ENABLE_RECONFIG
	reg_reconfig reg_reconfig(
		.reset_i(reg_rst),
		.clk(clk_usb_buf),
		.icap_clk(clk_slow),
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

	 wire target_highz = target_npower;
	 	
	 assign target_PDID = (target_highz) ? 1'bZ :
		                   (enable_output_pdid) ? output_pdid :
								 1'bZ;
							
	 assign target_PDIC = (target_highz) ? 1'bZ:
	                      (enable_output_pdic) ? output_pdic :
								 1'bZ;
	                    
	 assign target_nRST = (target_highz && ~nrst_ignore_highz) ? 1'bZ :
	                      (enable_avrprog) ? USB_treset_i :
								 (enable_output_nrst) ? output_nrst :
								 1'bZ;
	 assign target_MOSI = (target_highz) ? 1'bZ :
	                      (enable_avrprog) ? USB_spi0_mosi_i : 1'bZ;
	 assign target_SCK = (target_highz) ? 1'bZ :
	                      (enable_avrprog) ? USB_spi0_sck_i : 1'bZ;
	 assign USB_spi0_miso_o = (enable_avrprog) ? target_MISO : ext_miso;	
	 
	 wire sc_enable = 1'b1;
	 
	 assign sc_rst = (sc_enable) ? USB_sc_rst : 1'bZ;
	 assign sc_clk = (sc_enable) ? USB_sc_clk : 1'bZ;
	 assign sc_aux1 = (sc_enable) ? USB_sc_aux1 : 1'bZ;
	 assign sc_aux2 = (sc_enable) ? USB_sc_aux2 : 1'bZ;
	 assign sc_io = (sc_enable) ? ((USB_spi2_txd2) ? 0 : 1'bZ) : 1'bZ;
	 assign USB_spi2_rxd2 = (sc_enable) ? sc_io : 1'bZ;
	 
	 assign ext_sck = USB_spi0_sck_i;
	 assign ext_mosi = USB_spi0_mosi_i;
	 assign lcd_cs = USB_spi0_cs0;
	 assign lcd_dc = USB_spare2;
	 assign avr_cs = USB_spare1; 
			
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
