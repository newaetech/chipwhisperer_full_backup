`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   14:29:12 07/07/2012
// Design Name:   ddr_top
// Module Name:   C:/E/Documents/openadc/openadc_git/hdl/hdl/testbench/tb_ddr_top.v
// Project Name:  avnet_iseboard_
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: ddr_top
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////



module tb_ddr_top;

// ========================================================================== //
// Parameters                                                                 //
// ========================================================================== //
   parameter DEBUG_EN                = 0;
   localparam DBG_WR_STS_WIDTH       = 32;
   localparam DBG_RD_STS_WIDTH       = 32;
   parameter C3_MEMCLK_PERIOD     = 10000;
   parameter C3_RST_ACT_LOW        = 0;
   parameter C3_INPUT_CLK_TYPE     = "SINGLE_ENDED";
   parameter C3_NUM_DQ_PINS        = 16;
   parameter C3_MEM_ADDR_WIDTH     = 13;
   parameter C3_MEM_BANKADDR_WIDTH = 2;
   parameter C3_MEM_ADDR_ORDER     = "ROW_BANK_COLUMN"; 
   parameter C3_P0_MASK_SIZE       = 4;
   parameter C3_P0_DATA_PORT_SIZE  = 32;  
   parameter C3_P1_MASK_SIZE       = 4;
   parameter C3_P1_DATA_PORT_SIZE  = 32;
   parameter C3_CALIB_SOFT_IP      = "TRUE";
   parameter C3_SIMULATION      = "TRUE";
   parameter C3_HW_TESTING      = "FALSE";

	// Inputs
	reg reset;
	reg clk_100mhz;
	reg [9:0] adc_datain;
	reg adc_sampleclk;
	reg adc_or;
	reg adc_trig_status;
	reg adc_capture_go;
	reg ddr_read_req;
	reg [31:0] ddr_read_address;
	wire ddr_read_fifoclk;
	reg ddr_read_fifoen;
	wire [31:0] max_samples;

	// Outputs
	wire adc_capture_stop;
	wire ddr_read_done;
	wire ddr_read_fifoempty;
	wire ddr_cal_done;
	wire [7:0] ddr_read_data;
	wire ddr_error;
	wire [12:0] LPDDR_A;
	wire [1:0] LPDDR_BA;
	wire LPDDR_LDM;
	wire LPDDR_UDM;
	wire LPDDR_CK_N;
	wire LPDDR_CK_P;
	wire LPDDR_CKE;
	wire LPDDR_CAS_n;
	wire LPDDR_RAS_n;
	wire LPDDR_WE_n;
	wire LPDDR_RZQ;
	
	// Bidirs
	wire [15:0] LPDDR_DQ;
	wire LPDDR_LDQS;
	wire LPDDR_UDQS;

	// Instantiate the Unit Under Test (UUT)
	ddr_top uut (
		.reset_i(reset), 
		.clk_100mhz_in(clk_100mhz), 
		.adc_datain(adc_datain), 
		.adc_sampleclk(adc_sampleclk), 
		.adc_or(adc_or), 
		.adc_trig_status(adc_trig_status), 
		.adc_capture_go(adc_capture_go), 
		.adc_capture_stop(adc_capture_stop), 
		.ddr_read_req(ddr_read_req), 
		.ddr_read_done(ddr_read_done), 
		.ddr_read_address(ddr_read_address), 
		.ddr_read_fifoclk(ddr_read_fifoclk), 
		.ddr_read_fifoen(ddr_read_fifoen), 
		.ddr_read_fifoempty(ddr_read_fifoempty), 
		.ddr_read_data(ddr_read_data), 
		.ddr_cal_done(ddr_cal_done),
		.ddr_error(ddr_error),
		.max_samples_i(max_samples),
		.LPDDR_A(LPDDR_A), 
		.LPDDR_BA(LPDDR_BA), 
		.LPDDR_DQ(LPDDR_DQ), 
		.LPDDR_LDM(LPDDR_LDM), 
		.LPDDR_UDM(LPDDR_UDM), 
		.LPDDR_LDQS(LPDDR_LDQS), 
		.LPDDR_UDQS(LPDDR_UDQS), 
		.LPDDR_CK_N(LPDDR_CK_N), 
		.LPDDR_CK_P(LPDDR_CK_P), 
		.LPDDR_CKE(LPDDR_CKE), 
		.LPDDR_CAS_n(LPDDR_CAS_n), 
		.LPDDR_RAS_n(LPDDR_RAS_n), 
		.LPDDR_WE_n(LPDDR_WE_n), 
		.LPDDR_RZQ(LPDDR_RZQ)
	);

	//NOTE: the following model is created by coregen
    lpddr_model_c3 u_mem3(
      .Dq    (LPDDR_DQ),
      .Dqs   ({LPDDR_UDQS,LPDDR_LDQS}),
      .Addr  (LPDDR_A),
      .Ba    (LPDDR_BA),
      .Clk   (LPDDR_CK_P),
      .Clk_n (LPDDR_CK_N),
      .Cke   (LPDDR_CKE),
      .Cs_n  (1'b0),
      .Ras_n (LPDDR_RAS_n),
      .Cas_n (LPDDR_CAS_n),
      .We_n  (LPDDR_WE_n),
      .Dm    ({LPDDR_UDM,LPDDR_LDM})
      );
		
	wire[9:0] adc_data0 ;
	wire[9:0] adc_data1 ;
	wire[9:0] adc_data2 ;
	wire adc_or_out;
	wire adc_ts_out;
	
	reg [31:0] adc_temp;

	assign 		max_samples = 100;

	initial begin
		// Initialize Inputs
		reset = 0;
		clk_100mhz = 0;
		adc_datain = 0;
		adc_sampleclk = 0;
		adc_or = 0;
		adc_trig_status = 0;
		adc_capture_go = 0;
		ddr_read_req = 0;
		ddr_read_address = 0;
		ddr_read_fifoen = 0;
		#1;
		reset = 1;

		// Wait 100 ns for global reset to finish
		#100;
		reset = 0;
		
		@(posedge ddr_cal_done);
        
		//READ AND WILLING
		
		//Start Capture
		adc_capture_go = 1;
		
		//Wait for done
		@(posedge adc_capture_stop);
		adc_capture_go = 0;
		
		//Dump data out to file
		ddr_read_req = 1;
		@(posedge ddr_read_done);
		ddr_read_req=0;
		ddr_read_fifoen = 1;		
			
		@(posedge ddr_read_fifoempty);
		ddr_read_fifoen = 0;
		
		$finish;
	end
	
	assign adc_or_out = adc_temp[31];
	assign adc_ts_out = adc_temp[30];
	assign adc_data2 = adc_temp[29:20]; //Sample N + 2
	assign adc_data1 = adc_temp[19:10]; //Sample N + 1
	assign adc_data0 = adc_temp[9:0];   //Sample N
	
	reg ddr_read_fifoen_delay;
	
	always @(posedge ddr_read_fifoclk) begin
		ddr_read_fifoen_delay <= ddr_read_fifoen;
	end
	
	always @(posedge ddr_read_fifoclk) begin	
	if (ddr_read_fifoclk & ddr_read_fifoen_delay) begin	
		adc_temp[31:24] = ddr_read_data;
		
		@(posedge ddr_read_fifoclk);
		adc_temp[23:16] = ddr_read_data;
		
		@(posedge ddr_read_fifoclk);
		adc_temp[15:8] = ddr_read_data;
		
		@(posedge ddr_read_fifoclk);
		adc_temp[7:0] = ddr_read_data;
	end
	end
	
	//ADC clock is slower
	always begin
		#7 adc_sampleclk = ~adc_sampleclk;
	end
	
	//ADC Data Generator
	always begin
		@(posedge adc_sampleclk);		
		#1;
		adc_datain = adc_datain + 1;
	end
		
	
	assign ddr_read_fifoclk = clk_100mhz;
	
	always begin
		#5 clk_100mhz = ~clk_100mhz;
	end
      
endmodule

