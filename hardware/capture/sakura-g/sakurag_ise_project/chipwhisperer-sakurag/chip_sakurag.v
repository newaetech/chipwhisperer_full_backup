/*-------------------------------------------------------------------------
 SAKURA-G With OpenADC - ChipWhisperer Build
 
 Copyright (C) 2013 Colin O'Flynn
 -------------------------------------------------------------------------*/ 

module CHIP_SAKURAG 
  (
	// USB FT2232H
	input wire			 usb_clk,
   inout wire [7:0]   usb_d,
   input wire         usb_rxfn,
	input wire         usb_txen,
   output wire        usb_rdn,
	output wire        usb_wrn,
	output wire        usb_oen,
	output wire			 usb_rstn,

   // Local bus for GII
   output wire [15:0] lbus_di_a, // Address and input data  (Controller -> Cryptographic module)
   input wire  [15:0] lbus_do,   // Output data (Cryptographic module -> Controller)  
   output wire        lbus_wrn,  // input data
   output wire        lbus_rdn,  // Assert output data
   output wire	       lbus_clkn, // Clock
	output wire        lbus_rstn, // Reset
	
	// OpenADC Connection
	input wire [9:0] ADC_Data,
	input wire       ADC_OR,
	output wire      ADC_clk,
	input wire       DUT_CLK_i,
	input wire       DUT_trigger_i,
	output wire      amp_gain,
	output wire      amp_hilo,

	//SelectMap Configuration
	//(Unused inputs)
   input wire [7:0] cfg_din,
	input wire       cfg_mosi,
	input wire       cfg_fcsb,
	input wire       cfg_cclk,
   input wire       cfg_progn,
	input wire       cfg_initn,
	input wire       cfg_cson,
	input wire       cfg_rdwrn,
	input wire       cfg_busy,
   input wire       cfg_done,
	input wire       cfg_m0_remote,
	input wire       cfg_m1_remote,

	//System Connections
   output wire [7:0]  led,
	input wire 			 clkin,  //48-MHz Clock
	input wire			 rstnin, //Push-Button (SW8)	
	
	// SPI Flash
	input wire       flash_DIN,
	output wire      flash_DOUT,
	output wire      flash_CS,
	output wire      flash_CLK
	);
		
   //------------------------------------------------
   wire         clk, rst;

	//Enable SPI Flash Programming via USB
	wire         spi_flash_mode; 

	//TODO: make this some DIP setting
   assign spi_flash_mode = 1'b0;

	//Unconnected
	assign led[3] = 1;
	assign led[2] = 1;
	assign led[1] = 1;
	assign led[0] = 1;
	
	//------------------------------------------------OpenADC
	wire [3:0] gpio_led;
	wire clkin_bufg;
	assign rst = ~rstnin;
	
	assign led[7] = ~rst;
	assign led[6] = ~gpio_led[1];
	assign led[5] = ~gpio_led[2];
	assign led[4] = ~gpio_led[3];
	
	wire reg_rst;
	wire [5:0] reg_addr;
	wire [15:0] reg_bcnt;
	wire [7:0] reg_datao;
	wire [7:0] reg_datai_fpgaic;
	wire [15:0] reg_size;
	wire reg_read;
	wire reg_write;
	wire reg_addrvalid;
	wire [5:0] reg_hypaddr;
	wire [15:0] reg_hyplen_fpgaic;
	wire disable_openadc;	
	wire lbus_clkint;

	assign disable_openadc = 1'b0;

	 openadc_interface openadc_inst(
    .reset_i(rst | disable_openadc), 
	 .clk_adcint(clkin),
	 .clk_iface(usb_clk), 
	 .ftdi_data(usb_d),
	 .ftdi_rxfn(usb_rxfn),
	 .ftdi_txen(usb_txen),
	 .ftdi_rdn(usb_rdn),
	 .ftdi_wrn(usb_wrn),
	 .ftdi_oen(usb_oen),
	 .ftdi_siwua(usb_siwua),
	 .LED_hbeat(gpio_led[1]),
	 .LED_armed(gpio_led[2]),
	 .LED_capture(gpio_led[3]),
	 .ADC_Data(ADC_Data),
	 .ADC_OR(ADC_OR),
	 .ADC_clk(ADC_clk),
	 .DUT_CLK_i(lbus_clkint),
	 .DUT_trigger_i(DUT_trigger_i),
	 .amp_gain(amp_gain),
	 .amp_hilo(amp_hilo),
	 
	 .reg_reset_o(reg_rst),
	 .reg_address_o(reg_addr),
	 .reg_bytecnt_o(reg_bcnt),
	 .reg_datao_o(reg_datao),
	 .reg_datai_i(reg_datai_fpgaic),
	 .reg_size_o(reg_size),
	 .reg_read_o(reg_read),
	 .reg_write_o(reg_write),
	 .reg_addrvalid_o(reg_addrvalid),
	 .reg_stream_i(1'b0),
	 .reg_hypaddress_o(reg_hypaddr),
	 .reg_hyplen_i(reg_hyplen_fpgaic)  
	 );
	
	reg_sakura_lbus registers_sakura_lbus (
		.reset_i(reg_rst),
		.clk(usb_clk),
		.reg_address(reg_addr), 
		.reg_bytecnt(reg_bcnt), 
		.reg_datao(reg_datai_fpgaic), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_hypaddress(reg_hypaddr), 
		.reg_hyplen(reg_hyplen_fpgaic),
		
		.lbus_di_a(lbus_di_a),
		.lbus_do(lbus_do),
		.lbus_wrn(lbus_wrn),
		.lbus_rdn(lbus_rdn),
		.lbus_clkn(lbus_clkn),
		.lbus_rstn(lbus_rstn),
		.lbus_clkint(lbus_clkint)
	);
	
	//SPI Flash
	assign flash_DOUT = (spi_flash_mode) ? usb_d[1] : 1'bZ;
	assign flash_CLK  = (spi_flash_mode) ? usb_d[0] : 1'bZ;
	assign flash_CS   = (spi_flash_mode) ? usb_d[3] : 1'bZ;
	/*
	
	wire [35:0] cs_data;   
   wire [35:0]  chipscope_control;
   chipscope_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   chipscope_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(usb_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   ); 
	
	assign cs_data[0] = DUT_trigger_i;
	assign cs_data[1] = DUT_CLK_i;
	*/

	//Reset USB only on switch push
	assign usb_rstn = 1'b1; //rstnin;

endmodule // CHIP_SAKURAG
