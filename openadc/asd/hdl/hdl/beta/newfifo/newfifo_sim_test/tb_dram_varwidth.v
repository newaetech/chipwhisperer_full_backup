`timescale 1ns / 1ps
`default_nettype none

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   14:21:25 07/19/2013
// Design Name:   dram_varwidth
// Module Name:   C:/E/Documents/openadc/openadc_git/hdl/hdl/beta/newfifo/newfifo_sim_test/tb_dram_varwidth.v
// Project Name:  newfifo_sim_test
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: dram_varwidth
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module tb_dram_varwidth;

	// Inputs
	reg [9:0] wr_data;
	reg [1:0] wr_padding;
	reg [9:0] wr_addr;
	reg [4:0] wr_ws;
	reg wr_ce;
	reg wr_clk;
	
	wire [7:0] rd_data;
	reg [9:0] rd_addr;
	reg [4:0] rd_ws;
	reg rd_ce;
	reg rd_clk;

	// Instantiate the Unit Under Test (UUT)
	dram_varwidth uut 
	(
		.wr_data(wr_data), 
		.wr_addr(wr_addr), 
		.wr_ws(wr_ws), 
		.wr_ce(wr_ce), 
		.wr_clk(wr_clk),
		
		.rd_data(rd_data),
		.rd_addr(rd_addr),
		.rd_ws(rd_ws),
		.rd_ce(rd_ce),
		.rd_clk(rd_clk)
	);

	initial begin
		// Initialize Inputs
		wr_data = 0;
		wr_padding = 0;
		wr_addr = 0;
		wr_ws = 0;
		wr_ce = 0;
		wr_clk = 0;
		
		rd_addr = 0;
		rd_ws = 0;
		rd_ce = 0;
		rd_clk = 0;

		// Wait 100 ns for global reset to finish
		#100;
        
		ram_write(10'h0, 4'h0, 10'h1AA);
		ram_write(10'h0, 4'h1, 10'h1AA);
		ram_write(10'h0, 4'h2, 10'h1AA);
		
		ram_write(10'h1, 4'h0, 10'h1BC);
		ram_write(10'h1, 4'h1, 10'h2DA);
		ram_write(10'h1, 4'h2, 10'h200);
		
		ram_write(10'h2, 4'h0, 10'h32A);
		ram_write(10'h2, 4'h1, 10'h444);
		ram_write(10'h2, 4'h2, 10'h555);
		
		ram_write(10'h1, 4'h0, 10'h175);
		ram_write(10'h1, 4'h1, 10'h0BA);
		ram_write(10'h1, 4'h2, 10'h02A);
		
		#50;
		
		ram_display(10'h0);		
		ram_display(10'h1);		
				
	end
	
	always
		#5  wr_clk = ~wr_clk;
		
	always
		#5  rd_clk = ~rd_clk;
  
task ram_write;
		input [9:0] address;
		input [4:0] ws;
		input [9:0] data;
		begin			
			@(negedge wr_clk);
			wr_addr = address;
			wr_data = data;
			wr_ws = ws;
			#1;
			wr_ce = 1;
			@(posedge wr_clk);
			#1;
			wr_ce = 0;
		end
endtask 
 
task ram_read;
		input [9:0] address;
		input [4:0] ws;
		output [7:0] data;
		
		begin			
			@(negedge rd_clk);
			rd_addr = address;
			rd_ws = ws;
			#1;
			rd_ce = 1;
			@(posedge rd_clk);
			#1;
			rd_ce = 0;
			
			data = rd_data;
		end
endtask 
 
task ram_display;
		input [9:0] address;

		reg [31:0] data;
		
		begin			
			ram_read(address, 0, data[7:0]);
			ram_read(address, 1, data[15:8]);
			ram_read(address, 2, data[23:16]);
			ram_read(address, 3, data[31:24]);
			
			$display("%h %h %h", data[9:0], data[19:10], data[29:20]);
			
			//$display("%h.%d = %h\n", address, ws, rd_data);
		end
endtask 
   
endmodule

