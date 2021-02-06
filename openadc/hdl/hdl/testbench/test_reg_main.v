`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   11:45:27 01/18/2013
// Design Name:   reg_main
// Module Name:   C:/E/Documents/academic/sidechannel/chipwhisperer/openadc/hdl/hdl/testbench/test_reg_main.v
// Project Name:  sasebo-w-unified-iseproject
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: reg_main
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module test_reg_main;

	// Inputs
	reg reset_i;
	reg clk;
	reg cmdfifo_rxf;
	reg cmdfifo_txe;	
	reg [7:0] cmdfifo_din;


	// Outputs
	wire cmdfifo_rd;
	wire cmdfifo_wr;
	wire [7:0] cmdfifo_dout;
	wire cmdfifo_isout;
	
	wire reg_clk;
	wire [5:0] reg_address;
	wire [15:0] reg_bytecnt;
	wire [7:0] reg_datao;
	wire [15:0] reg_size;
	wire reg_read;
	wire reg_write;
	wire reg_addrvalid;
	wire [5:0] reg_hypaddress;
	wire reg_stream;
	wire [7:0] reg_datai;
	wire [15:0] reg_hyplen;

	// Instantiate the Unit Under Test (UUT)
	reg_main uut (
		.reset_i(reset_i), 
		.clk(clk), 
		.cmdfifo_rxf(cmdfifo_rxf), 
		.cmdfifo_txe(cmdfifo_txe), 
		.cmdfifo_rd(cmdfifo_rd), 
		.cmdfifo_wr(cmdfifo_wr), 
		.cmdfifo_din(cmdfifo_din), 
		.cmdfifo_dout(cmdfifo_dout), 
		.cmdfifo_isout(cmdfifo_isout), 
		.reg_clk(reg_clk), 
		.reg_address(reg_address), 
		.reg_bytecnt(reg_bytecnt), 
		.reg_datao(reg_datao), 
		.reg_datai(reg_datai), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_stream(reg_stream),
		.reg_hypaddress(reg_hypaddress), 
		.reg_hyplen(reg_hyplen)
	);
	
	
	reg_openadc uut2 (
		.reset_i(reset_i),
		.reset_o(),
		.clk(reg_clk),
		.reg_address(reg_address), 
		.reg_bytecnt(reg_bytecnt), 
		.reg_datao(reg_datai), 
		.reg_datai(reg_datao), 
		.reg_size(reg_size), 
		.reg_read(reg_read), 
		.reg_write(reg_write), 
		.reg_addrvalid(reg_addrvalid), 
		.reg_stream(reg_stream),
		.reg_hypaddress(reg_hypaddress), 
		.reg_hyplen(reg_hyplen),
		
		.gain(),
		.hilow(),
		.status(8'hB8),
		.cmd_arm(),
		.trigger_mode(),
		.trigger_wait(),
		.trigger_level(),
		.trigger_source(),
		.trigger_now(),
		.trigger_offset(),
		
		.extclk_frequency(32'd2198239),
		
		.phase_clk_o(),
		.phase_o(),
		.phase_ld_o(),
		.phase_i(9'd982),
		.phase_done_i(1),

		.adc_clk_src_o(),
		.maxsamples_o(),
		.maxsamples_i(32'd40000)
		);

	always
		#5 clk = !clk;

	initial begin
		// Initialize Inputs
		reset_i = 1;
		clk = 0;
		cmdfifo_rxf = 0;
		cmdfifo_txe = 0;
		cmdfifo_din = 0;

		// Wait 100 ns for global reset to finish
		#100;
		
        
		reset_i = 0;
		#50;
		
		//WRITE ONE
		cmdfifo_rxf = 1;
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'b11000100;
				
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'd1;
		cmdfifo_rxf = 1;		
		
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'd0;
		cmdfifo_rxf = 1;		

		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'hAC;
		cmdfifo_rxf = 1;			
		
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_rxf = 0;
		#4;
		
		@(posedge clk);
		
		//READ ONE
		cmdfifo_rxf = 1;
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'b10000100;
				
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'd1;
		cmdfifo_rxf = 1;		
		
		@(posedge clk) wait(cmdfifo_rd == 1'b1);
		cmdfifo_din = 8'd0;
		cmdfifo_rxf = 1;	
		@(posedge clk);
		cmdfifo_rxf = 0;
		wait(cmdfifo_wr == 1'b1);
		
		@(posedge clk) wait(cmdfifo_wr == 1'b1);
		cmdfifo_rxf = 0;

	end
      
endmodule

