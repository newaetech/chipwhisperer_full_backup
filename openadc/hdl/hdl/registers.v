	 /* Registers:	 
	 0x00 - Gain Settings (One Byte)
	 
	   [ G7 G6 G5 G4 G3 G2 G1 G0 ]
		
		  G = 8-bit PWM setting for gain voltage.
		      Voltage = G / 256 * VCCIO
	 
	 0x01 - Settings (One Byte)
	 
	   [  I  F  W  S  A  T  H  R ]
	     
		  R = (bit 0) System Reset, active high
		  H = (bit 1) Hilo output to amplifier
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
	 
	    [  X M  DC DE X  E  F  T ] 
		 T = (bit 0) Triggered status
		      1 = System armed
				0 = System disarmed		
		 F = (bit 1) Capture Status
		      1 = FIFO Full / Capture Done
				0 = FIFO Not Full / Capture Not Done
		 E = (bit 2) External trigger status
		      1 = Trigger line high
				0 = Trigger line low	
		 DE = (bit 4) DDR Error
		      1 = DDR error (FIFO underflow/overflow or DDR Error)
				0 = No error		 
		 DC = (bit 5) DDR Calibration Done
		      1 = Cal done OK
				0 = Cal in progress/failed	
		 M =  (bit 6) Memory Mode
		      1 = DDR
				0 = FIFO
		 O =  (bit 7) Overflow
		      1 = Overflow happened
				0 = No overflow
		
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

              FS = (Bit 5:4) FIFO Segmentation Mode
				    00 = Normal
					 01 = Enable Mode
					 10 = Segment Mode
			
	 0x07 - System Clock (4 Bytes) - Read Only
	    Clock frequency in Hz
		 
	 0x08 - ADC Output Clock (4 Bytes) - Read Only
	 
	 0x09 - Phase Adjust (2 Bytes)
	 
	    [ P7 P6 P5 P4 P3 P2 P1 P0 ] (Byte 0)	    
		 [                    S P8 ] (Byte 1)
		 
		 S = Start (write), Status (read)
		 
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
	 */
	 
    `define GAIN_ADDR    	0
	 
    `define SETTINGS_ADDR  1
	 
	 `define STATUS_ADDR    2
	 
	 `define ADCREAD_ADDR   3
	 
    `define ECHO_ADDR      4
	 
	 `define EXTFREQ_ADDR   5	 
	 `define EXTFREQ_LEN    4
	 
	 `define ADVCLOCK_ADDR  6
	 `define ADVCLOCK_LEN   4
	 
	 `define SYSTEMCLK_ADDR 7
	 `define SYSTEMCLK_LEN	4
	 
	 `define ADCFREQ_ADDR   8	 
	 `define ADCFREQ_LEN    4
	 
	 `define PHASE_ADDR     9 	 
	 `define PHASE_LEN      2
	 
	 `define VERSION_ADDR	10
	 `define VERSION_LEN		6
	 
	 `define DECIMATE_ADDR  15
	 `define DECIMATE_LEN   2
	 
	 `define SAMPLES_ADDR   16
	 `define SAMPLES_LEN    4
	 
	 `define PRESAMPLES_ADDR 17
	 `define PRESAMPLES_LEN 4
	 
	 `define RETSAMPLES_ADDR 18
	 `define RETSAMPLES_LEN 4
	 
	 `define TRIGGER_DUR_ADDR 20
	 `define TRIGGER_DUR_LEN  4
	 
	 `define OFFSET_ADDR    26
	 `define OFFSET_LEN		4
	 
	 `define REGISTER_VERSION 1
	 
	 //`define SCARDCTRL_ADDR	30
	 //`define SCARDHDR_ADDR	31
	 //`define SCARDPLD_ADDR	32
	 
	 //`define TARGSERIALDATA_ADDR 33
	 //`define TARGSERIALLEN_ADDR	 34
	 //`define TARGSERIALBAUD_ADDR 35
	 
	 //`define IOTRIGCLKDDIV_ADDR	36
	 //`define IOTRIGPROG_ADDR		37 
	 
	 //`define CW_EXTCLK_ADDR		38
	 //`define CW_TRIGSRC_ADDR		39
	 //`define CW_TRIGMOD_ADDR		40
	 //`define CW_IOROUTE_ADDR    55
	 
	 //For SmartCard Only
	 //`define TARGSERIALDATA_ADDR 41
	 //`define TARGSERIALLEN_ADDR	 42
	 //`define TARGSERIALBAUD_ADDR 43

	 // 44 - 46 is USI

	 //`define ADDR_I2CSTATUS 47	 
	 //`define ADDR_I2CDATA 48
	 
	 //`define SAKURA_STATUS_ADDR 49
	 //`define SAKURA_FIFO_ADDR	50
	 
	 //`define GLITCHCYCLES_CNT   19

	 
	 //`define CLOCKGLITCH_SETTINGS	51
	 //`define CLOCKGLITCH_OFFSET    25
	 //`define CLOCKGLITCH_OFFSET_LEN 4
	 
	 //`define RECONFIG_REG 52
	 
	 //`define SAD_STATUSCFG_ADDR 53
	 //`define SAD_REFDATA_ADDR   54
	 
	 //`define GLITCH_RECONFIG_RB_ADDR 56
	 
	 //`define IODECODETRIG_CFG_ADDR  57
	 //`define IODECODETRIG_DATA_ADDR 58
	 
	 //``define CW_IOREAD_ADDR       59
	 
	 //NEXT ADDRESS = 60