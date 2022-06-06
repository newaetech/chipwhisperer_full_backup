`include "includes.v"
`undef CHIPSCOPE

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:04:43 02/08/2012 
// Design Name: 
// Module Name:    usb_interface 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module usb_interface(reset, clk, rx_in, tx_out, 
							gain, hilow, status,
							fifo_empty, fifo_data, fifo_rd_en, fifo_rd_clk,
							cmd_arm, trigger_mode, trigger_wait,
							extclk_frequency,
							phase_o, phase_ld_o, phase_i, phase_done_i, phase_clk_o,
							adc_clk_src_o							
`ifdef CHIPSCOPE
                     ,chipscope_control
`endif                     
                     );
        
    input         				reset;
    input         				rx_in;
    output        				tx_out;
    input         				clk;
	 
	 //Rising edge arms system
	 output							cmd_arm;
	 
	 //1 = Active High trigger, 0 = Active Low trigger
	 output                    trigger_mode;
	 
	 output                    trigger_wait;
		 
	 output [7:0]					gain;
	 output							hilow;
	 input [7:0]					status;
	 
	 input							fifo_empty;
	 input [7:0]					fifo_data;
	 output							fifo_rd_en;
	 output							fifo_rd_clk;
	 
	 input [31:0]					extclk_frequency;
	 
	 output							phase_clk_o;
	 output [8:0]					phase_o;
	 output							phase_ld_o;
	 input  [8:0]					phase_i;
	 input							phase_done_i;
	 
	 output							adc_clk_src_o;
	 
	       
`ifdef CHIPSCOPE
    inout [35:0]             chipscope_control;
`endif     
		 
    wire          				ftdi_rxf_n;
    wire          				ftdi_txe_n;	 
    reg           				ftdi_rd_n;
    reg           				ftdi_wr_n;
	 reg								fifo_rd_en;
    
    wire [7:0] ftdi_din;
    reg [7:0]  ftdi_dout;
    reg        ftdi_isOutput;
    wire       ftdi_clk;
    wire       ftdi_rxf;
    
    assign ftdi_clk = clk;
    assign ftdi_rxf_n = ~ftdi_rxf;
	 assign fifo_rd_clk = clk;
	 
	 assign phase_clk_o = clk;
    
    //Reinstate following for FTDI
    //assign ftdi_d = ftdi_isOutput ? ftdi_dout : 8'bZ;
    //assign ftdi_din = ftdi_d;
    
`ifdef CHIPSCOPE
   wire cs_trig;
   wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .DATA(cs_data), // IN BUS [127:0]
    .TRIG0(cs_trig) // IN BUS [0:0]
   );
  
`endif
        
    //Serial
    wire txbusy;
    async_transmitter AT (.clk(clk),
                      .TxD_start(~ftdi_wr_n),
                      .TxD_data(ftdi_dout),
                      .TxD(tx_out),
                      .TxD_busy(txbusy));   
                
   assign ftdi_txe_n = txbusy | ~ftdi_wr_n;
                
    async_receiver AR (.clk(clk),
                   .RxD(rx_in),
                   .RxD_data_ready(ftdi_rxf),
                   .RxD_data(ftdi_din));
		  
    //Register definitions
    reg [7:0]  registers_gain;
    reg [7:0]  registers_settings;
	 reg [7:0]  registers_echo;
	 reg [31:0] registers_extclk_frequency;
	 reg [8:0]	phase_out;
	 reg [8:0]  phase_in;
	 reg        phase_loadout;
	 reg			phase_done;
	 
	 assign phase_o = phase_out;
	 assign phase_ld_o = phase_loadout;
	 
	 always @(posedge ftdi_clk) begin
		if (reset | phase_loadout) begin
			phase_in <= 0;
			phase_done <= 0;
		end else if (phase_done_i) begin
			phase_in <= phase_i;
			phase_done <= 1;
		end
	end
	 
	 /* Registers:
	 
	 0x00 - GAIN SETTING
	 
	   [ G7 G6 G5 G4 G3 G2 G1 G0 ]
		
		  G = 8-bit PWM setting for gain voltage.
		      Voltage = G / 256 * VCCIO
	 
	 0x01 - SETTINGS
	 
	   [  X  C  W  P  A  T  H  R ]
	     
		  R = (bit 0) System Reset, active high
		  H = (bit 1) Hilo output to amplifier
		  T = (bit 2) Trigger Polarity:
		      1 = Trigger when 'trig in' = 1
				0 = Trigger when 'trig in' = 0
		  A = (bit 3) Arm Trigger
		      1 = Arm trigger
				0 = No effect, but you must clear bit to 0
				    before next trigger cycle can be started
		  P = (bit 4) DUT Clkin PLL Reset
		      1 = Reset PLL
				0 = Do not reset PLL
		  W = (bit 5) Before arming wait for trigger to go inactive (e.g: edge sensitive)
		      1 = Wait for trigger to go inactive before arming
				0 = Arm immediatly, which if trigger line is currently in active state
				    will also immediatly trigger
		  C = (bit 6) Select clock source for ADC
		      1 = External x4
				0 = Internal 100 MHz
		  
	 0x02 - STATUS
	 
	    [  X  X  X  X  X  E  F  T ] 
		 T = (bit 0) Triggered status
		      1 = System armed
				0 = System disarmed		
		 F = (bit 1) FIFO Status
		      1 = FIFO Full (ready to read)
				0 = FIFO not full (do not read yet)			
		 E = (bit 2) External trigger status
		      1 = Trigger line high
				0 = Trigger line low
		 P = (bit 3) DUT Clkin PLL Status
		      1 = Locked / OK
				0 = Unlocked
		
	 0x03 - ADC Readings

       Data is read from this register by issuing a READ command.
		 The entire contents of the FIFO will be dumped following
		 that read command (4096 bytes)		 
	 
	    [  1  X  X  P OR D9 D8 D7 ]
		 
		 [  0 D6 D5 D4 D3 D2 D1 D0 ]
	 
	 0x04 - Echo Register
	 
		 [ E7 E6 E5 E4 E3 E2 E1 E0 ]
		 
		 E = Write data to this register then read back to
		     confirm device connection is OK	

	 0x05 - 0x08 - External Frequency Counter
	 
	 0x09 - Phase Adjust LSB
	 
	    [ P7 P6 P5 P4 P3 P2 P1 P0 ]
		 
	 0x0A - Phase Adjust MSB
	    
		 [                    S P8 ]
		 
		 S = Start (write), Status (read)
	 
	*/
	 
    `define GAIN_ADDR    	0
    `define SETTINGS_ADDR  1
	 `define STATUS_ADDR    2
    `define ADCDATA_ADDR	3
    `define ECHO_ADDR      4
	 `define EXTFREQ_ADDR1  5
	 `define EXTFREQ_ADDR2  6
	 `define EXTFREQ_ADDR3  7
	 `define EXTFREQ_ADDR4  8
	 `define PHASE_ADDR1    9
	 `define PHASE_ADDR2   10
    
    `define IDLE           'b0000
    `define ADDR           'b0001
    `define DATAWR1        'b0010
    `define DATAWR2        'b0011
    `define DATAWRDONE     'b0100
    `define DATARDSTART    'b1000
    `define DATARD1        'b1001
    `define DATARD2        'b1010
	 
          

    reg [3:0]              state = `IDLE;
    reg [5:0]              address;
	 reg							extclk_locked;
    
	 assign hilow = registers_settings[1];
	 assign trigger_mode = registers_settings[2];
	 assign cmd_arm = registers_settings[3];
	 assign trigger_wait = registers_settings[5];
	 assign adc_clk_src_o = registers_settings[6];
	 
	 assign gain = registers_gain;
	  
	 always @(posedge ftdi_clk)
	 begin
		if (extclk_locked == 0) begin
			registers_extclk_frequency <= extclk_frequency;
		end
	 end
	 
    always @(posedge ftdi_clk or posedge reset)
    begin
      if (reset == 1) begin
         state <= `IDLE; 
         ftdi_rd_n <= 1;
         ftdi_wr_n <= 1;
         ftdi_isOutput <= 0;
			extclk_locked <= 0;
      end else begin
         case (state)
            `IDLE: begin
					fifo_rd_en <= 0;
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  ftdi_wr_n <= 1;
                  ftdi_isOutput <= 0;
                  state <= `ADDR;
               end else begin
                  ftdi_rd_n <= 1;
                  ftdi_wr_n <= 1;
                  ftdi_isOutput <= 0;
                  state <= `IDLE;
               end
            end

            `ADDR: begin
               address <= ftdi_din[5:0];
               ftdi_rd_n <= 1;
               ftdi_wr_n <= 1;
					fifo_rd_en <= 0;
               if (ftdi_din[7] == 1) begin
                  if (ftdi_din[6] == 1) begin
                     //MSB means WRITE
                     ftdi_isOutput <= 0;
                     state <= `DATAWR1;               
                  end else begin
                     //MSB means READ
                     ftdi_isOutput <= 1;
                     state <= `DATARDSTART;               
                  end
               end else begin
                  ftdi_isOutput <= 0;
                  state <= `IDLE;                  
               end
             end
               
            `DATAWR1: begin
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
					fifo_rd_en <= 0;
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  state <= `DATAWR2;
               end else begin
                  ftdi_rd_n <= 1;
                  state <= `DATAWR1;
               end
             end
               
            `DATAWR2: begin
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
               ftdi_rd_n <= 1;
					fifo_rd_en <= 0;
               			
					if (address == `GAIN_ADDR) begin
                  registers_gain <= ftdi_din;
               end else if (address == `SETTINGS_ADDR) begin
                  registers_settings <= ftdi_din;
               end else if (address == `ECHO_ADDR) begin
                  registers_echo <= ftdi_din;
					end
					
               state <= `IDLE;                         
             end

            
            `DATARDSTART: begin
               ftdi_isOutput <= 1;               
               ftdi_rd_n <= 1;
					fifo_rd_en <= 0;
            					
					if (address == `GAIN_ADDR) begin
                  ftdi_dout <= registers_gain;
						ftdi_wr_n <= 0;
						state <= `IDLE;
						extclk_locked <= 0;
               end else if (address == `SETTINGS_ADDR) begin
                  ftdi_dout <= registers_settings;
						ftdi_wr_n <= 0;
						state <= `IDLE;
						extclk_locked <= 0;
               end else if (address == `ECHO_ADDR) begin
                  ftdi_dout <= registers_echo;
						ftdi_wr_n <= 0;
						state <= `IDLE;
						extclk_locked <= 0;
					end else if (address == `ADCDATA_ADDR) begin						
						ftdi_dout <= 8'hAC;
						ftdi_wr_n <= 1;
						state <= `DATARD1;
						extclk_locked <= 0;
					end else if (address == `STATUS_ADDR) begin
						ftdi_dout <= status;
						ftdi_wr_n <= 0;
						state <= `IDLE;
						extclk_locked <= 0;
					end else if (address == `EXTFREQ_ADDR1) begin
						ftdi_dout <= registers_extclk_frequency[7:0];
						extclk_locked <= 1;
						ftdi_wr_n <= 0;
						state <= `IDLE;
					end else if (address == `EXTFREQ_ADDR2) begin
						ftdi_dout <= registers_extclk_frequency[15:8];
						extclk_locked <= 1;
						ftdi_wr_n <= 0;
						state <= `IDLE;
					end else if (address == `EXTFREQ_ADDR3) begin
						ftdi_dout <= registers_extclk_frequency[23:16];
						extclk_locked <= 1;
						ftdi_wr_n <= 0;
						state <= `IDLE;
					end else if (address == `EXTFREQ_ADDR4) begin
						ftdi_dout <= registers_extclk_frequency[31:24];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						state <= `IDLE;
					end else if (address == `PHASE_ADDR1) begin
						ftdi_dout <= phase_in[7:0];
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						state <= `IDLE;
					end else if (address == `PHASE_ADDR2) begin
						ftdi_dout[0] <= phase_in[8];
						ftdi_dout[1] <= phase_done;
						extclk_locked <= 0;
						ftdi_wr_n <= 0;
						state <= `IDLE;
               end  else begin
						extclk_locked <= 0;
						ftdi_dout <= 8'bx;						
						ftdi_wr_n <= 1;
						state <= `IDLE;
					end
            end
            
            `DATARD1: begin
               ftdi_isOutput <= 1;
               ftdi_rd_n <= 1;    										
									
					if (ftdi_txe_n == 0) begin
						ftdi_wr_n <= 0;
						if (fifo_empty == 0) begin
							fifo_rd_en <= 1;
							state <= `DATARD2;
						end else begin
							fifo_rd_en <= 0;
							state <= `IDLE;
						end
					end else begin
						ftdi_wr_n <= 1;
						fifo_rd_en <= 0;
						state <= `DATARD1;
					end
            end
            
            `DATARD2: begin
               ftdi_isOutput <= 1;
               ftdi_wr_n <= 1;
               ftdi_rd_n <= 1;               
					fifo_rd_en <= 0;
					ftdi_dout <= fifo_data;
               state <= `DATARD1;
               end    
						
				default: begin
					fifo_rd_en <= 0;
					ftdi_rd_n <= 1;
               ftdi_wr_n <= 1;
               ftdi_isOutput <= 0;
               state <= `IDLE;
				end
             
         endcase
      end                  
    end 
    

    always @(posedge ftdi_clk or posedge reset)
    begin
      if (reset == 1) begin
         
      end else begin
         case (state)               
            `DATAWR2: begin
               if (address == `PHASE_ADDR1) begin
						phase_out[7:0] <= ftdi_din;
					end else if (address == `PHASE_ADDR2) begin
						phase_out[8] <= ftdi_din[0];	
						phase_loadout <= ftdi_din[1];
					end                     
             end
						
				default: begin
					phase_loadout <= 0;

				end
             
         endcase
      end                  
    end 

	  
`ifdef CHIPSCOPE
   assign cs_trig = (state == `IDLE) ? 0 : 1;
   assign cs_data[3:0] = state;
   assign cs_data[11:4] = address;
   assign cs_data[12] = ftdi_rxf_n;
   assign cs_data[127:16] = registers_echo[127:16]; 
 `endif
 
endmodule
