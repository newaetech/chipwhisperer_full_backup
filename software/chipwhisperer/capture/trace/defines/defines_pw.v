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

`define MAIN_REG_SELECT              2'b00
`define USB_REG_SELECT               2'b01

`define REG_BUILDTIME                6'h00
`define REG_SNIFF_FIFO_STAT          6'h02
`define REG_SNIFF_FIFO_RD            6'h03
`define REG_ARM                      6'h04

`define REG_TRIGGER_ENABLE           6'h05
`define REG_NUM_TRIGGERS             6'h06
`define REG_TRIGGER_DELAY            6'h07
`define REG_TRIGGER_WIDTH            6'h08
`define REG_TRIG_CLK_PHASE_SHIFT     6'h09
`define REG_CAPTURE_LEN              6'h0a
`define REG_COUNT_WRITES             6'h0b
`define REG_COUNTER_QUICK_START      6'h0c
`define REG_BOARD_REV                6'h0d

`define REG_USERIO_DATA              6'h0e
`define REG_USERIO_PWDRIVEN          6'h0f

`define REG_MMCM_LOCKED              6'h10

`define REG_RESET_REG                6'h11
`define REG_FAST_FIFO_RD_EN          6'h12
`define REG_TIMESTAMPS_DISABLE       6'h13
`define REG_CAPTURE_WHILE_TRIG       6'h14
`define REG_MAX_TIMESTAMP            6'h15

`define REG_LED_SELECT               6'h16
`define REG_CLEAR_ERRORS             6'h17

`define REG_MULTIPLE_TRIGGERS        6'h18
`define REG_TRIGGERS_GENERATED       6'h19
`define REG_CAPTURE_OFF              6'h20
`define REG_BUFFER_SIZE              6'h21
`define REG_EXTERNAL_ARM             6'h22

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

