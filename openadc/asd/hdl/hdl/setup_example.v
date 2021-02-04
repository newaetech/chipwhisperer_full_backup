
//Uncomment this to get code for using CHIPSCOPE
//`define CHIPSCOPE

//Define the board in use
//`define AVNET
//`define DLP_HS_FPGA
//`define NEXYS2
`define SASEBOW

//Clock frequency input to the UART, used for calculating the baud rate
`ifdef AVNET
	`define UART_CLK 40000000
	//Baud rate of the UART
	`define UART_BAUD 512000
	`define MAX_SAMPLES 24573
`endif

`ifdef DLP_HS_FPGA
	`define UART_CLK 66666000
	//Baud rate of the UART
	`define UART_BAUD 115200
	`define MAX_SAMPLES 6141
`endif

`ifdef NEXYS2
	`define UART_CLK 50000000
	`define UART_BAUD 115200
	`define MAX_SAMPLES 11285
`endif

`ifdef SASEBOW
	`define FAST_FTDI
	//`define UART_CLK 24000000
	`define UART_CLK 60000000
	`define UART_BAUD 512000
	`define MAX_SAMPLES 49149
	`define USE_SCARD
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