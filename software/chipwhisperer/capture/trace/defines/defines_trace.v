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
// Description: Defines for trace-specific registers and logic.
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


`define TRACE_REG_SELECT        2'b10

`define REG_NAME                6'h00
`define REG_REV                 6'h01
`define REG_CLKSETTINGS         6'h02

`define REG_PATTERN_ENABLE      6'h03
`define REG_PATTERN_TRIG_ENABLE 6'h04
`define REG_TRACE_RESET_SYNC    6'h05
`define REG_TRACE_WIDTH         6'h06
`define REG_SOFT_TRIG_PASSTHRU  6'h07
`define REG_SOFT_TRIG_ENABLE    6'h08
`define REG_CAPTURE_RAW         6'h09

`define REG_SYNCHRONIZED        6'h0c

`define REG_TRACE_PATTERN0      6'h0d
`define REG_TRACE_PATTERN1      6'h0e
`define REG_TRACE_PATTERN2      6'h0f
`define REG_TRACE_PATTERN3      6'h10
`define REG_TRACE_PATTERN4      6'h11
`define REG_TRACE_PATTERN5      6'h12
`define REG_TRACE_PATTERN6      6'h13
`define REG_TRACE_PATTERN7      6'h14

`define REG_TRACE_MASK0         6'h15
`define REG_TRACE_MASK1         6'h16
`define REG_TRACE_MASK2         6'h17
`define REG_TRACE_MASK3         6'h18
`define REG_TRACE_MASK4         6'h19
`define REG_TRACE_MASK5         6'h1a
`define REG_TRACE_MASK6         6'h1b
`define REG_TRACE_MASK7         6'h1c

`define REG_TRACE_COUNT         6'h1d
`define REG_RECORD_SYNCS        6'h1e
`define REG_MATCHED_DATA        6'h1f

`define REG_SWO_ENABLE          6'h20
`define REG_SWO_BITRATE_DIV     6'h21
`define REG_UART_STOP_BITS      6'h22
`define REG_UART_DATA_BITS      6'h23
`define REG_UART_PARITY_SETTING 6'h24

`define REG_FE_CLOCK_SEL        6'h25
`define REG_STAT                6'h26
`define REG_TRIGGER_FREQ        6'h27
`define REG_FE_FREQ             6'h28
`define REG_FE_CLOCK_COUNT      6'h29

`define REG_TRIGGER_DRP_ADDR    6'h2a
`define REG_TRIGGER_DRP_DATA    6'h2b
`define REG_TRIGGER_DRP_RESET   6'h2c

`define REG_TRACE_EN            6'h2d
`define REG_TRACE_USERIO_DIR    6'h2e

`define REG_BUFFER              6'h2f
`define REG_UART                6'h30

`define REG_TRACECLK_PHASE      6'h31
`define REG_TRACECLK_DRP_ADDR   6'h32
`define REG_TRACECLK_DRP_DATA   6'h33
`define REG_TRACECLK_DRP_RESET  6'h34
`define REG_TRACECLK_SHIFT_EN   6'h35

`define REG_TRACE_TEST          6'h36

// FIFO bitfields:
//                   31 30 29 28 27 26 25 24   23 22 21 20 19 18 17 16   15 14 13 12 11 10  9  8   7  6  5  4  3  2  1  0
//                 +------------------------++------------------+-----++------------------------++------------------------+
// data command:   |         zeros          ||    FIFO status   | 0 0 ||      Rule match #      ||    short time stamp    |
//                 +------------------------++------------------+-----++------------------------++------------------------+
// raw  command:   |         zeros          ||    FIFO status   | 0 1 ||    match data byte     ||    short time stamp    |
//                 +------------------------++------------------+-----++------------------------++------------------------+
// time command:   |         zeros          ||    FIFO status   | 1 0 ||              long     time      stamp            |
//                 +------------------------++------------------+-----++------------------------++------------------------+
// stream command: |         zeros          ||    FIFO status   | 1 1 ||      stream status     ||          zeros         |
//                 +------------------------++------------------+-----++------------------------++------------------------+

`define FE_FIFO_CMD_DATA 2'b00
`define FE_FIFO_CMD_STAT 2'b01
`define FE_FIFO_CMD_TIME 2'b10
`define FE_FIFO_CMD_STRM 2'b11

`define FE_FIFO_CMD_START 16
`define FE_FIFO_CMD_BIT_LEN 2

`define FE_FIFO_TIME_START 0
`define FE_FIFO_SHORTTIME_LEN 8
`define FE_FIFO_FULLTIME_LEN 16

`define FE_FIFO_DATA_START 8
`define FE_FIFO_DATA_LEN 8

`define FE_FIFO_STRM_EMPTY 8'h0

// FIFO status register bits:
// TODO: aren't these repeated from the other defines file?
`define FIFO_STAT_EMPTY 0
`define FIFO_STAT_UNDERFLOW 1
`define FIFO_STAT_EMPTY_THRESHOLD 2
`define FIFO_STAT_FULL 3
`define FIFO_STAT_OVERFLOW_BLOCKED 4
`define FIFO_STAT_SYNC_FLAG 5
`define FIFO_STAT_CUSTOM_FLAG 5

// and this is where those FIFO status bits show up in the FIFO read,
// as opposed to register read:
// (avoiding refering to `FIFO_STAT_* to keep our Python parser stupid simple)
`define FE_FIFO_STAT_START 18
`define FE_FIFO_STAT_OFFSET 2
`define FE_FIFO_STAT_EMPTY 2
`define FE_FIFO_STAT_UNDERFLOW 3
`define FE_FIFO_STAT_EMPTY_THRESHOLD 4
`define FE_FIFO_STAT_FULL 5
`define FE_FIFO_STAT_OVERFLOW_BLOCKED 6
`define FE_FIFO_STAT_SYNC_FLAG 7

// front end select definitions
`define FE_SELECT_WIDTH 2
`define FE_TRACE 1

