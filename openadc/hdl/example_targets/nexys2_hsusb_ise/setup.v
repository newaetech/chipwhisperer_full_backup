//Uncomment this to get code for using CHIPSCOPE
//Define the board in use
`define NEXYS2

`ifdef NEXYS2
	`define UART_CLK 50000000
	`define UART_BAUD 115200
	`define MAX_SAMPLES 11285
	`define FAST_FTDI
	`define NOBUFG_ADCCLK
`endif

//Baud rate of the UART
`ifndef UART_BAUD
`define UART_BAUD 512000
`endif

//`define USE_ETH

//`define USE_DDR

//Uncomment the following if you want the ETH pins in your UCF file
//If your design may use the ETH you should do this, so you can then
//just use the USE_ETH flag to decide if it's compiled in or not
//`define OPT_ETH

//Uncomment the following if you want the DDR pins in your UCF file
//If your design may use the DDR you should do this, so you can then
//just use the USE_DDR flag to decide if it's compiled in or not
//`define OPT_DDR