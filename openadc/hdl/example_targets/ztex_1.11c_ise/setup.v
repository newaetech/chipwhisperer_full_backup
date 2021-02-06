
//Uncomment this to get code for using CHIPSCOPE
//`define CHIPSCOPE

//Define the board in use
`define ZTEX

`ifdef ZTEX
	`define UART_CLK 30000000
	`define MAX_SAMPLES 24573
	`define FAST_FTDI
	`define NOBUFG_ADCCLK
`endif

//`define USE_DDR

//Uncomment the following if you want the DDR pins in your UCF file
//If your design may use the DDR you should do this, so you can then
//just use the USE_DDR flag to decide if it's compiled in or not
//`define OPT_DDR