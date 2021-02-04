`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
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
module dcm_clkgen_load(
    input wire clk_i,             //Clock for inputs & SPI
	 input wire reset_i,           //Reset - must also connect to DCM so this block knows when defaults are loaded
	 input wire [7:0] mult,        //Mult-1 (e.g.: value of 1 means Mult=2)
	 input wire [7:0] div,         //Div-1 (e.g.: value of 0 means Div=1)
	 input wire       load,
	 output reg       done,

	 input wire  PROGDONE,			//Connect to DCM
	 output reg  PROGDATA,			//Connect to DCM
	 output reg  PROGEN,				//Connect to DCM
	 output wire PROGCLK				//Connect to DCM
    );

	 wire clk;
	 wire reset;
	 
	 assign clk = clk_i;
	 assign reset = reset_i;	 
	 assign PROGCLK = clk;
	 
	 `define RESET          'b000
    `define IDLE           'b001
    `define GO					'b010
	 `define WAITING			'b011
	 `define DONE				'b100

    reg [3:0]              state = `RESET;
 
	 reg [24:0]	dataline;
	 reg [24:0] enline;
	 reg go;
	 
	 always @(posedge clk)
	 begin
	   if (load)
			dataline <= {1'b0, 2'b00, {mult, 2'b11}, 2'b00 ,{div, 2'b01}};
		else if (go)
			dataline <= {1'b0, dataline[24:1]};
	 end
	 
	 always @(posedge clk)
	 begin
	   if (load)
			enline <= {1'b1, 2'b00, {10'b1111111111}, 2'b00 ,{10'b1111111111}};
		else if (go)
			enline <= {1'b0, enline[24:1]};
	 end
	 
	 always @(posedge clk)
	 begin
		if (go) begin
			PROGDATA <= dataline[0];
			PROGEN <= enline[0];
		end else begin
			PROGDATA <= 1'b0;
			PROGEN <= 1'b0;
		end
	 end
	 
	 always @(posedge clk)
    begin
      if (reset == 1) begin
         state <= `RESET;          
			go <= 0;
      end else begin
         case (state)
            `RESET: begin
					go <= 0;
					done <= 0;
					state <= `IDLE;
            end
				
				`IDLE: begin	
					go <= 0;	
					done <= 0;		
					if (load) begin
						state <= `GO;					
					end else begin
						state <= `IDLE;
					end
            end
			
				`GO: begin
					go <= 1;
					done <= 0;
					if (enline[0] == 1'b0)
						state <= `WAITING;
					else
						state <= `GO;					
				end
				
				`WAITING: begin
					go <= 1;
					done <= 0;
					if (PROGDONE == 1'b1)
						state <= `DONE;
					else
						state <= `WAITING;
				 end
					
				`DONE: begin
					go <= 0;
					done <= 1;
					state <= `IDLE;
				end
				
				default: begin
					go <= 0;
					done <= 0;
					state <= `RESET;
				end
			endcase
		end
	end		


endmodule
