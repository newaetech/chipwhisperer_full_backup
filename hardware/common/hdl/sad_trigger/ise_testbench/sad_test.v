`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   10:38:45 08/24/2013
// Design Name:   sad
// Module Name:   C:/e/Documents/fpga/sad_trigger/sad_test.v
// Project Name:  sad_trigger
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: sad
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module sad_test;

	// Inputs
	reg ap_clk;
	reg ap_rst;
	reg ap_start;
	reg [6:0] max_points_V;
	reg signed [10:0] temp;
	reg signed [9:0] ref_points_in_V_dout;
	reg ref_points_in_V_empty_n;
	reg signed [9:0] datain_V_dout;
	reg datain_V_ap_vld;
	reg datain_V_empty_n;
	reg sumout_V_full_n;

	// Outputs
	wire ap_done;
	wire ap_idle;
	wire ap_ready;
	wire ref_points_in_V_read;
	wire datain_V_read;
	wire [19:0] sumout_V_din;
	wire sumout_V_ap_vld;
	wire sumout_V_write;

	always  #1  ap_clk = ~ap_clk;

	// Instantiate the Unit Under Test (UUT)
	sad uut (
		.ap_clk(ap_clk), 
		.ap_rst(ap_rst), 
		.ap_start(ap_start), 
		.ap_done(ap_done), 
		.ap_idle(ap_idle), 
		.ap_ready(ap_ready), 
		.max_points_V(max_points_V), 
		.ref_points_in_V_dout(ref_points_in_V_dout), 
		.ref_points_in_V_empty_n(ref_points_in_V_empty_n), 
		.ref_points_in_V_read(ref_points_in_V_read), 
		.datain_V_dout(datain_V_dout),  
		.datain_V_empty_n(datain_V_empty_n),
		.datain_V_read(datain_V_read),
		.sumout_V_din(sumout_V_din), 
		.sumout_V_full_n(sumout_V_full_n),
		.sumout_V_write(sumout_V_write)
	);

	integer numref, numdata, rpoints, indata, sout, count;

	initial
		begin
		// Initialize Inputs
		ap_clk = 0;
		ap_rst = 0;
		ap_start = 0;
		max_points_V = 0;
		ref_points_in_V_dout = 0;
		ref_points_in_V_empty_n = 0;
		datain_V_dout = 0;
		sumout_V_full_n = 1;


		rpoints  = $fopen("feature.txt","r");
		count = $fscanf(rpoints, "%d\n", numref);
		indata = $fopen("data.txt","r");		
		count = $fscanf(indata, "%d\n", numdata);
		max_points_V = numref;
		sout = $fopen("sout_verilog.txt","w");

		// Wait 100 ns for global reset to finish
		#100;
		ap_rst = 1;
		
		#100;
		ap_rst = 0;
		
		@(posedge ap_clk);
		@(posedge ap_clk);
		ap_start = 1;
		@(posedge ap_clk);
		ap_start = 0;
		
		//Load reference stuff
		ref_points_in_V_empty_n = 1;

		count = $fscanf(rpoints,"%d\n",temp);
		ref_points_in_V_dout = temp - 512;
		
		while ( numref > 0) begin
		  @ (posedge ap_clk);
		  if (ref_points_in_V_read == 1'b1) begin
				numref = numref - 1;     
				count = $fscanf(rpoints,"%d\n",temp);
				ref_points_in_V_dout = temp - 512;
		  end
      end
		ref_points_in_V_empty_n = 0;
		
		@(posedge ap_clk);
		
		datain_V_empty_n = 1;
		while ( numdata > 0) begin
		  @ (posedge ap_clk);
		  numdata = numdata - 1;     
		  count = $fscanf(indata,"%d\n",temp);
		  datain_V_dout = temp - 512;
		  $fwrite(sout,"%d\n",sumout_V_din);
      end
		//datain_V_ap_vld = 0;
		
		$fclose(sout);
		
		#100 $finish;
		
	end
endmodule

