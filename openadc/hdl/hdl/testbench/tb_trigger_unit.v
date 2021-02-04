`timescale 1ns / 1ps

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the test for the trigger unit.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/

module tb_trigger_unit;

	// Inputs
	reg reset;
	reg clk;
	reg adc_clk;
	reg [9:0] adc_data = 0;
	reg ext_trigger_i;
	reg trigger_level_i;
	reg trigger_wait_i;
	reg [9:0] trigger_adclevel_i;
	reg trigger_source_i;
	reg trigger_now_i;
	reg arm_i;
	reg capture_done_i;

	// Outputs
	wire arm_o;
	wire capture_go_o;

	// Instantiate the Unit Under Test (UUT)
	trigger_unit uut (
		.reset(reset), 
		.clk(clk), 
		.adc_clk(adc_clk), 
		.adc_data(adc_data), 
		.ext_trigger_i(ext_trigger_i), 
		.trigger_level_i(trigger_level_i), 
		.trigger_wait_i(trigger_wait_i), 
		.trigger_adclevel_i(trigger_adclevel_i), 
		.trigger_source_i(trigger_source_i), 
		.trigger_now_i(trigger_now_i), 
		.arm_i(arm_i), 
		.arm_o(arm_o), 
		.capture_go_o(capture_go_o), 
		.capture_done_i(capture_done_i)
	);

	initial begin
		// Initialize Inputs
		reset = 0;
		clk = 0;
		adc_clk = 0;
		adc_data = 0;
		ext_trigger_i = 0;
		trigger_level_i = 0;
		trigger_wait_i = 0;
		trigger_adclevel_i = 0;
		trigger_source_i = 0;
		trigger_now_i = 0;
		arm_i = 0;
		capture_done_i = 0;
		
		#50;
		reset = 1;
		
		#50;
		reset = 0;
		
		// TEST #1: External input trigger
		$display ("*** Test #1: External Input Trigger: Level Sensitive ***");
		
		$display (" Test 1-A: Level = HIGH, initial level = LOW ");
		trigger_level_i = 1;
		trigger_wait_i = 0;
		trigger_source_i = 0;
		ext_trigger_i = 0;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;
		
		#50;
		
		ext_trigger_i = 1;			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
		
		#50;
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
		
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 1-A Done");

		
		$display (" Test 1-B: Level = HIGH, initial level = HIGH ");
		trigger_level_i = 1;
		trigger_wait_i = 0;
		trigger_source_i = 0;
		ext_trigger_i = 1;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;
		
		#50;
		
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
				
		capture_done_i = 1;		
		arm_i = 0;
		#50;
		$display("Test 1-B Done");


		$display (" Test 1-C: Level = LOW, initial level = LOW ");
		trigger_level_i = 0;
		trigger_wait_i = 0;
		trigger_source_i = 0;
		ext_trigger_i = 0;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;
		
		#50;
		
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
				
		capture_done_i = 1;		
		arm_i = 0;
		#50;
		$display("Test 1-C Done");


		$display (" Test 1-D: Level = LOW, initial level = HIGH ");
		trigger_level_i = 0;
		trigger_wait_i = 0;
		trigger_source_i = 0;
		ext_trigger_i = 1;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;
		
		#50;
		
		ext_trigger_i = 0;			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
		
		#50;
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
		
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 1-D Done");
		
		// TEST #2: External input trigger
		$display ("*** Test #2: External Input Trigger: Edge Sensitive ***");    

		$display (" Test 2-A: Level = RISING EDGE, initial level = LOW ");
		reset = 1;
		#50;
		reset = 0;
		#50;
		trigger_level_i = 1;
		trigger_wait_i = 1;
		trigger_source_i = 0;
		ext_trigger_i = 0;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;
		
		#50;
		
		ext_trigger_i = 1;			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
		
		#50;
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
		
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 2-A Done");
		

		$display (" Test 2-B: Level = RISING EDGE, initial level = HIGH ");
		trigger_level_i = 1;
		trigger_wait_i = 1;
		trigger_source_i = 0;
		ext_trigger_i = 1;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;


		ext_trigger_i = 0;			
		#50;	
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
				
		ext_trigger_i = 1;			
		#50;	
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
			
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 2-B Done");
		
		$display (" Test 2-C: Level = FALLING EDGE, initial level = HIGH ");
		reset = 1;
		#50;
		reset = 0;
		#50;
		trigger_level_i = 0;
		trigger_wait_i = 1;
		trigger_source_i = 0;
		ext_trigger_i = 1;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		arm_i = 1;

		#50;	
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
				
		ext_trigger_i = 0;			
		#50;	
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
			
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 2-C Done");


		$display (" Test 2-D: Level = FALLING EDGE, initial level = LOW ");
		reset = 1;
		#50;
		reset = 0;
		#50;
		trigger_level_i = 0;
		trigger_wait_i = 1;
		trigger_source_i = 0;
		ext_trigger_i = 0;
		trigger_now_i = 0;
		capture_done_i = 0;
		
		#50;
		if (arm_o != 0)
			$display(" Error: arm_o should be 0");
			
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");		
		arm_i = 1;	
		#50;	
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
				
		ext_trigger_i = 1;			
		#50;	
		if (capture_go_o != 0)
			$display(" Error: capture_go_o should be 0");
			
		ext_trigger_i = 0;			
		#50;	
		if (capture_go_o != 1)
			$display(" Error: capture_go_o should be 1");
			
		capture_done_i = 1;		
		arm_i = 0;		
		#50;
		$display("Test 2-D Done");

    
		$finish;
		
	end
	
	//System clock
	always begin
		#17 clk = ~clk;
	end
	
	//ADC clock
	always begin
		#5 adc_clk = ~adc_clk;
	end
	
	//ADC slope
	reg up_down = 0;
	always @(posedge adc_clk) begin
		if (up_down == 0) begin
			adc_data <= adc_data + 1;
		end else begin
			adc_data <= adc_data - 1;
		end	
	end
	
	always @(posedge adc_clk) begin
		if (adc_data == 0) begin
			up_down <= 0;
		end else if (adc_data == 10'b1111111111) begin
			up_down <= 1;
		end
	end
	
			      
endmodule

