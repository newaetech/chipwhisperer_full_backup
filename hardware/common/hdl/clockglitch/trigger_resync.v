`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    19:48:25 07/04/2014 
// Design Name: 
// Module Name:    trigger_resync 
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
module trigger_resync(
	input wire  	reset,
	input wire		clk,
	input wire		exttrig,	
	input [31:0]	offset,
	
	output wire 	exttrigger_resync
	);

	 reg data_status;
	 reg async_trigger_inv;
	 always @(posedge clk or posedge exttrig) begin
			if (exttrig == 1'b1)
				async_trigger_inv <= 1'b0;
			else
				async_trigger_inv <= data_status;		
	 end
	 
	 //async_trigger_inv gets set to '0' once trigger happens
	 //and will get set back to '1' once we are no longer triggering
	 
	  /* Glitch Delay */
	 reg [31:0] glitch_delay_cnt;
	
	 //Counter is reset when trigger low
	 always @(posedge clk) begin
	  	if (async_trigger_inv == 1'b1) begin
			glitch_delay_cnt <= 0;
		end else begin
			if (glitch_delay_cnt != 32'hFFFFFFFF)
				glitch_delay_cnt <= glitch_delay_cnt + 1;
		end
    end 
	 
	 always @(posedge clk or posedge exttrig) begin
			if (exttrig == 1'b1)
				data_status <= 1'b0;
			else if (reset)
				data_status <= 1'b1;
			else if (glitch_delay_cnt >= offset)
				data_status <= 1'b1;		
	 end
	 
	 reg exttrigger_resync_reg;
	 
	 //always @(posedge clk)
	//	delayed <= ~data_status;	
		
	 always @(posedge clk)
		exttrigger_resync_reg <= (glitch_delay_cnt == offset) ? ~async_trigger_inv : 1'b0;
		
	 assign exttrigger_resync = exttrigger_resync_reg;

endmodule
