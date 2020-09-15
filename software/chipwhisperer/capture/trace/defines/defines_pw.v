//////////////////////////////////////////////////////////////////////////////////
// Company: NewAE
// Engineer: Jean-Pierre Thibault
// 
// Create Date: 
// Design Name: 
// Module Name: 
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: Main PhyWhisperer defines file. Front-end specific defines
// elsewhere (e.g. defines_usb.v).
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

`define MAIN_REG_SELECT              3'b000
`define USB_REG_SELECT               3'b001

`define REG_BUILDTIME                5'h00
`define REG_FE_SELECT                5'h01
`define REG_SNIFF_FIFO_STAT          5'h02
`define REG_SNIFF_FIFO_RD            5'h03
`define REG_ARM                      5'h04

`define REG_TRIGGER_ENABLE           5'h05
`define REG_NUM_TRIGGERS             5'h06
`define REG_TRIGGER_DELAY            5'h07
`define REG_TRIGGER_WIDTH            5'h08
`define REG_TRIG_CLK_PHASE_SHIFT     5'h09
`define REG_CAPTURE_LEN              5'h0a
`define REG_COUNT_WRITES             5'h0b
`define REG_COUNTER_QUICK_START      5'h0c

// front end select definitions
`define FE_SELECT_WIDTH 2
`define FE_USB 0

// FIFO status register bits:
`define FIFO_STAT_EMPTY 0
`define FIFO_STAT_UNDERFLOW 1
`define FIFO_STAT_EMPTY_THRESHOLD 2
`define FIFO_STAT_FULL 3
`define FIFO_STAT_OVERFLOW_BLOCKED 4
`define FIFO_STAT_CAPTURE_DONE 5
`define FIFO_STAT_CUSTOM_FLAG 5

// FIFO bitfields which are common for all:
`define FE_FIFO_CMD_START 16
`define FE_FIFO_CMD_BIT_LEN 2

`define FE_FIFO_DATA_START 8
`define FE_FIFO_DATA_LEN 8

`define FE_FIFO_CMD_DATA 2'b00
`define FE_FIFO_CMD_STAT 2'b01
`define FE_FIFO_CMD_TIME 2'b10
`define FE_FIFO_CMD_STRM 2'b11

`define FE_FIFO_STRM_EMPTY 8'h0

