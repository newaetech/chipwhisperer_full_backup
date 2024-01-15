/* Registers:
0x00 - Gain Settings (One Byte)
  [ G7 G6 G5 G4 G3 G2 G1 G0 ]

         G = 8-bit PWM setting for gain voltage.
             Voltage = G / 256 * VCCIO


0x01 - Settings (One Byte)
  [  I  F  W  S  A  T  X  R ]

         R = (bit 0) System Reset, active high
         T = (bit 2) Trigger Polarity:
             1 = Trigger when 'trig in' = 1
             0 = Trigger when 'trig in' = 0
         A = (bit 3) Arm Trigger
             1 = Arm trigger
             0 = No effect, but you must clear bit to 0
                 before next trigger cycle can be started
         S = (bit 4) Stream Mode
             1 = Stream mode
             0 = Normal mode
         W = (bit 5) Before arming wait for trigger to go inactive (e.g: edge sensitive)
             1 = Wait for trigger to go inactive before arming
             0 = Arm immediatly, which if trigger line is currently in active state
                 will also immediatly trigger

         F = (bit 6) Trigger Now
             1 = Trigger Now
             0 = Normal

         I = (bit 7) Select trigger source: int/ext
             1 = Internal (e.g.: based on ADC reading)
             0 = External (e.g.: based on trigger-in line)


0x02 - Status (One Byte)
   [  O M  X PL L  E  F  T ] 
        T = (bit 0) Triggered status
             1 = System armed
             0 = System disarmed
        F = (bit 1) Capture Status
             1 = FIFO Full / Capture Done
             0 = FIFO Not Full / Capture Not Done
        E = (bit 2) External trigger status
             1 = Trigger line high
             0 = Trigger line low
        L  = (bit 3) CLKGEN MMCM locked
             1 = locked
             0 = not locked
        PL = (bit 4) ADS4128 PLL locked
             1 = locked
             0 = not locked
        DC = (bit 5) DDR Calibration Done
             1 = Cal done OK
             0 = Cal in progress/failed
        M =  (bit 6) Memory Mode
             1 = DDR
             0 = FIFO
        O =  (bit 7) Overflow
             1 = Overflow occured on last capture
             0 = No overflow on last capture

0x03 - ADC Readings
       Data is read from this register by issuing a READ command.
       The entire contents of the FIFO will be dumped following
       that read command (e.g.: number of samples requested), or
       in DDR mode a different formatting is used (described elsewhere)

       [  1  X  X  P OR D9 D8 D7 ]
       [  0 D6 D5 D4 D3 D2 D1 D0 ]


0x04 - Echo Register (1 byte)
        [ E7 E6 E5 E4 E3 E2 E1 E0 ]

        E = Write data to this register then read back to
            confirm device connection is OK


0x05 - External Frequency Counter (4 bytes)


0x06 - Advanced Clock Registers (4 bytes)
   [  1 SP SG RA G0 C2 C1 C0 ] (Byte 0)

       1 = Always '1'

       SP = Status of DCM Block
          1 = Locked/OK
          0 = Not Locked

       SG = Status of CLKGEN Lock
          1 = Locked/OK
          0 = Not Locked

       RA = Reset All DCM/PLL Blocks
          1 = Reset Active
          0 = No Reset

       G0 = Select INPUT to CLKGEN Block
          0 = From system clock
          1 = From EXTCLK input

       C2 = Select INPUT to DCM Block.
          0 = From CLKGEN Block
          1 = From EXTCLK Input

       C1 = Select DCM Output
          0 = 4x INPUT TO DCM clock
          1 = 1x INPUT TO DCM clock 

       C0 = Select source for ADC clock
          0 = From DCM (e.g.: dependant on C1, C2, phase shift)
          1 = Direct from external input

   [ MUL7:0                 ] (Byte 1)
   [ MUL7:0                 ] (Byte 2)
       MUL7:0 = 8 Bits for 'Multiplication' setting, range 1-255
       DIV7:0 = 8 Bits for 'Division', range 0-255
       CLKOUT Frequency = ((MUL+1) / (DIV+1)) * CLKIN
       Note the DCM has certain allowed frequency ranges, see datasheet

   [ X  X  FS  FS  FM R  D  L ] (Byte 3)
       L = (Bit 0) Load bit 
          Must write this to '1' than '0' for the new MUL/DIV settings to take effect
       D = (Bit 1) Done bit
          Bit indicates state of programming cycle. Cleared on start-up (e.g. when defaults
          loaded), set after new MUL/DIV loaded. Cleared when you write the 'L' bit to '1',
          then set again automatically when new data finished loading.

       R = (Bit 2) Reset CLKGEN DCM
          1 = Set CLKGEN Reset line high
          0 = Set CLKGEN Reset line low
          NOTE: When you reset the DCM, you automatically load default MUL/DIV settings. However
                The Mul/Div registers ARE NOT cleared. Thus you need to write whatever MUL/DIV
                settings you want & perform a load operation.

       FM = (Bit 3) External Frequency Counter Source
          0 = EXTCLKIn Input
          1 = CLKGEN Output Block


0x07 - System Clock (4 Bytes) - Read Only
   Clock frequency in Hz

        
0x08 - ADC Output Clock (4 Bytes) - Read Only


0x09 - Phase Adjust (2 Bytes)


0x0A - Version Information (6 Bytes)
   [ X  X  X  X  RV RV RV RV ] (Byte 0)
   [ HT HT HT HT HT HV HV HV ] (Byte 1)
   [ Build Timestamp         ]
   [ Build Timestamp         ]
   [ Build Timestamp         ]
   [ Build Timestamp         ]
        
       RV = Register Version (4 bits)
          0 = Original, basic clock module
          1 = Updated w/ offset, adv clock module

       HT = Hardware Type (5 bits)
          0 = Unknown/Generic
          1 = LX9 Microboard
          2 = SASEBO-W
          3 = ChipWhisperer Rev2 on LX25
          4 = Reserved
          5 = ZedBoard
          8 = ChipWhisperer Lite, LX9
          9 = CW1200, LX45

       HV = Hardware Revision (3 bits)
          Specific to Hardware Type


0x10 - Number of samples to capture per operation (4 Bytes)
   On reset set to maximum number of samples possible.
   [ LSB ] (Byte 0)
   [     ] (Byte 1)
   [     ] (Byte 2)
   [ MSB ] (Byte 3)


0x11 - Number of samples to capture BEFORE trigger occurs (4 Bytes)
  This number must be less than value from register 0x10.
   [ LSB ] (Byte 0)
   [     ] (Byte 1)
   [     ] (Byte 2)
   [ MSB ] (Byte 3)


0x14 - DDR address to read from (4 Bytes)

   This must be 32-bit aligned, e.g. lower 2 bits are zero.
   This register is automatically incremented following a
   READ command. So to dump entire memory set DDR address to
   'zero' then issue read commands.
        
   [ LSB ] (Byte 0)
   [     ] (Byte 1)
   [     ] (Byte 2)
   [ MSB ] (Byte 3)

        
0x18 - ADC Trigger Level (2 Bytes)

   [ LSB ] (Byte 0)
   [ MSB ] (Byte 1)

0x1A - Offset of trigger to start of capture, ADC clock cycles (4 Bytes)
   
   [ LSB ] (Byte 0)
   [     ] (Byte 1)
   [     ] (Byte 2)
   [ MSB ] (Byte 3)

0x2c - FIFO status (1 byte)
   [  E 0 C P O1 U1 O2 U2 ]

       0 = Always '0'
       E = FIFO empty
       C = ADC clipped
       P = presample error: couldn't collect requested number of presamples before trigger
       O1 = Fast FIFO overflow
       U1 = Fast FIFO underflow
       O2 = Slow FIFO overflow
       U2 = Slow FIFO underflow

Note: some register bitfields are defined in reg_clockglitch.v and reg_chipwhisperer.v

 */
 
`define GAIN_ADDR               0
`define SETTINGS_ADDR           1
`define STATUS_ADDR             2
`define ADCREAD_ADDR            3
`define ECHO_ADDR               4
`define EXTFREQ_ADDR            5 
`define ADVCLOCK_ADDR           6
`define SYSTEMCLK_ADDR          7
`define ADCFREQ_ADDR            8 
`define PHASE_ADDR              9  
`define VERSION_ADDR            10
`define FPGA_BUILDTIME_ADDR     11
`define DECIMATE_ADDR           15
`define SAMPLES_ADDR            16
`define PRESAMPLES_ADDR         17
`define TRIGGER_DUR_ADDR        20
`define ADC_TRIGGER_LEVEL       21
`define CLOCKGLITCH_OFFSET      25
`define OFFSET_ADDR             26
`define FIFO_CONFIG             27
`define RESET                   28
`define ADC_LOW_RES             29

`define DRP_ADDR                30
`define DRP_DATA                31

`define NUM_SEGMENTS            32
`define SEGMENT_CYCLES          33

`define LED_SELECT              34
`define STREAM_SEGMENT_THRESHOLD 35
`define FAST_FIFO_READ_MODE     36

`define CW_AUX_IO               37
`define CW_EXTCLK_ADDR          38
`define CW_TRIGSRC_ADDR         39
`define CW_TRIGMOD_ADDR         40

`define XADC_DRP_ADDR           41
`define XADC_DRP_DATA           42
`define XADC_STAT               43
`define FIFO_STAT               44
`define NO_CLIP_ERRORS          45
`define DEBUG_FIFO_READS        46
`define DEBUG_FIFO_READS_FREEZE 47
`define CLKGEN_POWERDOWN        48

`define CLOCKGLITCH_POWERDOWN   49
`define CLOCKGLITCH_REPEATS     50
`define CLOCKGLITCH_SETTINGS    51
`define CLOCKGLITCH_NUM_GLITCHES 52
`define CLOCKGLITCH_MULTIPLE_STATE 53
`define CW_IOROUTE_ADDR         55
`define CW_IOREAD_ADDR          59

`define CW_ADC_CTRL             60
`define CW_VMAG_CTRL            61

`define CG1_DRP_ADDR            62
`define CG1_DRP_DATA            63
`define CG2_DRP_ADDR            64
`define CG2_DRP_DATA            65

`define FIFO_UNDERFLOW_COUNT    66
`define FIFO_NO_UNDERFLOW_ERROR 67

`define LA_DRP_ADDR             68
`define LA_DRP_DATA             69
`define LA_CLOCK_SOURCE         71
`define LA_TRIGGER_SOURCE       72
`define LA_POWERDOWN            73
`define LA_DRP_RESET            74
`define LA_MANUAL_CAPTURE       75
`define LA_CAPTURE_GROUP        76
`define LA_CAPTURE_DEPTH        77
`define LA_DOWNSAMPLE           78
`define LA_ARM                  98
`define LA_ENABLED              99
`define LA_SOURCE_FREQ          112

`define CG1_DRP_RESET           79
`define CG2_DRP_RESET           80
`define DRP_RESET               81

`define EXTCLK_MONITOR          82

`define USERIO_CW_DRIVEN        86
`define USERIO_DEBUG_DRIVEN     87
`define USERIO_DRIVE_DATA       88
`define USERIO_READ             97
`define USERIO_DEBUG_SELECT     109

`define CAPTURE_DONE            89

`define FIFO_FIRST_ERROR        90

`define SEGMENT_CYCLE_COUNTER_EN 92

`define MAX_SAMPLES_ADDR        93
`define MAX_SEGMENT_SAMPLES_ADDR 94

`define EXTERNAL_CLOCK          95
`define COMPONENTS_EXIST        96

`define SAD_REFERENCE           100
`define SAD_THRESHOLD           101
`define SAD_STATUS              102
`define SAD_BITS_PER_SAMPLE     103
`define SAD_REF_SAMPLES         104
`define SAD_COUNTER_WIDTH       105
`define SAD_MULTIPLE_TRIGGERS   106
`define SAD_SHORT               107
`define SAD_REFERENCE_BASE      108
`define SAD_VERSION             12 
`define SAD_ALWAYS_ARMED        13

`define FIFO_STATE              110
`define CLOCKGLITCH_POWERED_DOWN 111
`define EDGE_TRIGGER_COUNT      113
`define SOFTPOWER_CONTROL       91

`define NUM_TRIGGERS_STAT       14
`define NUM_TRIGGERS_DATA       18

`define SEQ_TRIGGERS_CONFIG     19
`define SEQ_TRIGGERS_MINMAX     22
`define SEQ_TRIGGERS_UART_EDGE_CHOOSER 23

`define REG_DDR3_STAT           114
`define REG_DDR3_TEST_LOOPS     115
`define REG_XO_EN               117
`define REG_DDR3_RW_STATS       118

`define REG_CW310_SPECIFIC      120

// temporary, for CW310/Pro development:
`define UIFREQ_ADDR             122

`define REG_DDR_SINGLE_RW_ADDR  123
`define REG_DDR_SINGLE_RW_DATA  124
`define REG_DDR_LA_START_ADDR   116
`define REG_DDR_TRACE_START_ADDR 119
`define REG_DDR_START_READ      125

// list of holes in the register address space (to be used for new features):
//`define UNUSED_ADDRESS 24
//`define UNUSED_ADDRESS 54
//`define UNUSED_ADDRESS 56
//`define UNUSED_ADDRESS 57
//`define UNUSED_ADDRESS 58
//`define UNUSED_ADDRESS 70
//`define UNUSED_ADDRESS 83
//`define UNUSED_ADDRESS 84
//`define UNUSED_ADDRESS 126
//`define UNUSED_ADDRESS 127

`define REGISTER_VERSION 2

`define TW_MAIN_REG_SELECT      2'b10
`define TW_TRACE_REG_SELECT     2'b11

