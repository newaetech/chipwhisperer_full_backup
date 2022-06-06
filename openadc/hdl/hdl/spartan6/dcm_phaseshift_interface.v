`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the 'phaseshift' interface. It interfaces with the delay module
(e.g.: DCM for Xilinx FPGAs) and provides a simple interface to adjust the
delay added in a line.

Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project (and file) is released under the 2-Clause BSD License:

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/
module dcm_phaseshift_interface(
    input clk_i,             //Clock for inputs & PCLK for DCM
	 input reset_i,           //Reset - must also connect to DCM so this block knows when defaults are loaded
	 input signed [8:0] default_value_i, //Default PS value in 2's complement format
    input signed [8:0] value_i,     //Requested PS Value in 2's complement format	 
    input load_i,            //When high starts a new phase shift operation    
	 output signed [8:0] value_o,    //Actual PS Value in 2's complement format, valid when done_o goes high
    output done_o,           //High for one clock cycle once operation complete
    output dcm_psen_o,       //Connect to DCM
    output dcm_psincdec_o,   //Connect to DCM
    input dcm_psdone_i,      //Connect to DCM
    input [7:0] dcm_status_i //Connect to DCM
    );
	 
	 `define RESET          'b000
    `define IDLE           'b001
    `define START          'b010
    `define PULSE          'b011
    `define WAIT1          'b100
	 `define WAIT2          'b101
    `define DONE           'b110          

    reg [3:0]              state = `RESET;
	 reg signed [8:0]              dcm_ps_count;
	 reg signed [8:0]					dcm_ps_target;
	 
	 reg							dcm_psen;
	 reg							dcm_psincdec;
	 reg							done;
	 reg signed [8:0]					value;
	 
	 assign dcm_psen_o = dcm_psen;
	 assign dcm_psincdec_o = dcm_psincdec;
	 assign done_o = done;
	 assign value_o = value;
	 
	 reg last_psincdec;
	 
	 always @(posedge clk_i or posedge reset_i)
    begin
      if (reset_i == 1) begin
         state <= `RESET;          
      end else begin
         case (state)
            `RESET: begin
					done <= 0;
					dcm_psen <= 0;
					dcm_psincdec <= 0;
					state <= `IDLE;
					dcm_ps_count <= default_value_i;
					value <= 0;
					last_psincdec <= 0;
            end
				
				`IDLE: begin
					dcm_psen <= 0;
					dcm_psincdec <= 0;
					done <= 0;
					
					if (load_i) begin
						state <= `START;					
					end else begin
						state <= `IDLE;
					end
            end
			
				`START: begin
					done <= 0;
					dcm_psen <= 0;
					dcm_psincdec <= 0;
					dcm_ps_target <= value_i;
					state <= `PULSE;
				end
				
				`PULSE: begin
					done <= 0;
					if (dcm_ps_target < dcm_ps_count) begin
						if ((last_psincdec == 0) && (dcm_status_i[0] == 1)) begin
							//Underflow & attempt to decrement
							value <= dcm_ps_count;
							state <= `DONE;
							dcm_psincdec <= 0;
							dcm_psen <= 0;
						end else begin
							//Decrement
							last_psincdec <= 0;
							dcm_psincdec <= 0;
							dcm_psen <= 1;
							dcm_ps_count <= dcm_ps_count + (-8'sd1);
							state <= `WAIT1;
						end				
					end else if (dcm_ps_target > dcm_ps_count) begin
						if ((last_psincdec == 1) && (dcm_status_i[0] == 1)) begin
							//Overflow & attempt to increment
							value <= dcm_ps_count;
							state <= `DONE;
							dcm_psincdec <= 0;
							dcm_psen <= 0;
						end else begin
							//Increment
							last_psincdec <= 1;
							dcm_psincdec <= 1;
							dcm_psen <= 1;
							dcm_ps_count <= dcm_ps_count + 8'sd1;
							state <= `WAIT1;
						end
					end else begin
						//Matched requested
						dcm_psincdec <= 0;
						dcm_psen <= 0;
						value <= dcm_ps_count;
						state <= `DONE;						
					end
				end
				
				`WAIT1: begin
					//Wait for PSDONE to go low from previous operation
					state <= `WAIT2;
					done <= 0;
					dcm_psen <= 0;
				end					

				`WAIT2: begin
					done <= 0;
					dcm_psen <= 0;					
					if (dcm_psdone_i == 1'b1) begin
						state <= `PULSE;
					end else begin
						state <= `WAIT2;
					end
				end
					
				`DONE: begin
					done <= 1;
					dcm_psen <= 0;
					dcm_psincdec <= 0;
					state <= `IDLE;
				end
				
				default: begin
					done <= 0;
					dcm_psen <= 0;
					dcm_psincdec <= 0;
					state <= `RESET;
				end
			endcase
		end
	end		


endmodule
