//`include "includes.v"
`default_nettype none

module cwlite_interface(  
    input wire         clk_usb,
      
    output wire        GPIO_LED1,
    output wire        GPIO_LED2,
    output wire        GPIO_LED3,
    output wire        GPIO_LED4,
	 output wire        GPIO_LED5,
	 
	 inout wire [7:0]	USB_D,
	 input wire [7:0]	USB_Addr,
	 input wire			USB_RDn,
	 input wire			USB_WRn,
	 input wire			USB_CEn,
	 input wire			USB_ALEn,

	 input wire [9:0]   ADC_Data,
	 input wire        ADC_OR,
	 output wire       ADC_clk,
	 input wire			 ADC_clk_fb,
//	 input         DUT_CLK_i,
//	 input         DUT_trigger_i,
	 output wire       amp_gain,
	 output wire       amp_hilo,
	 
	 input wire       XMEGA_PDID,
	 input wire       XMEGA_PDIC

	 );
	
	
	/* PDI Programming done from SAM, must float these wires
	   or programming will fail from weak pull-down on FPGA */
	//assign XMEGA_PDID = 1'bZ;
	//assign XMEGA_PDIC = 1'bZ;
	
	wire [35:0] cs_control0;
	wire [63:0] ila_trigbus;
	
	wire reset_i;
	assign reset_i = 0;
	
	wire reset_intermediate;
	wire clk_usb_buf;
	
		IBUFG IBUFG_inst (
	.O(clk_usb_buf),
	.I(clk_usb) );

	assign GPIO_LED1 = 1'b0;
	assign GPIO_LED5 = 1'b0;


	openadc_interface oadc(
		.reset_i(reset_i),
		.clk_adcint(clk_usb_buf),
		.clk_iface(clk_usb_buf),
		
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
		.DUT_CLK_i(1'b0),
		.DUT_trigger_i(1'b0),
		.amp_gain(amp_gain),
		.amp_hilo(amp_hilo),

		.reg_stream_i(1'b0),
		.reg_datai_i(8'd0),
		.reg_hyplen_i(16'd0)
	);	
	
	
	
	
	assign ila_trigbus[7:0] = USB_D;
	assign ila_trigbus[15:8] = USB_Addr;
	assign ila_trigbus[16] = USB_RDn;
	assign ila_trigbus[17] = USB_WRn;
	assign ila_trigbus[18] = USB_ALEn;
	assign ila_trigbus[19] = USB_CEn;
	
	/*
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
