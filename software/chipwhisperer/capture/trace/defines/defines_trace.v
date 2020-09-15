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


`define TRACE_REG_SELECT        3'b010

`define REG_NAME                5'h00
`define REG_REV                 5'h01
`define REG_CLKSETTINGS         5'h02

`define REG_PATTERN_ENABLE      5'h03
`define REG_PATTERN_TRIG_ENABLE 5'h04
`define REG_TRACE_RESET_SYNC    5'h05
`define REG_TRACE_WIDTH         5'h06
`define REG_SOFT_TRIG_PASSTHRU  5'h07
`define REG_SOFT_TRIG_ENABLE    5'h08
`define REG_CAPTURE_RAW         5'h09

`define REG_SYNCHRONIZED        5'h0c

`define REG_TRACE_PATTERN0      5'h0d
`define REG_TRACE_PATTERN1      5'h0e
`define REG_TRACE_PATTERN2      5'h0f
`define REG_TRACE_PATTERN3      5'h10
`define REG_TRACE_PATTERN4      5'h11
`define REG_TRACE_PATTERN5      5'h12
`define REG_TRACE_PATTERN6      5'h13
`define REG_TRACE_PATTERN7      5'h14

`define REG_TRACE_MASK0         5'h15
`define REG_TRACE_MASK1         5'h16
`define REG_TRACE_MASK2         5'h17
`define REG_TRACE_MASK3         5'h18
`define REG_TRACE_MASK4         5'h19
`define REG_TRACE_MASK5         5'h1a
`define REG_TRACE_MASK6         5'h1b
`define REG_TRACE_MASK7         5'h1c

`define REG_TRACE_COUNT         5'h1d
`define REG_RECORD_SYNCS        5'h1e
`define REG_MATCHED_DATA        5'h1f

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

