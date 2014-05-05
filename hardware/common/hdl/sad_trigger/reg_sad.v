`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2014, NewAE Technology Inc. All rights reserved.
Author: Colin O'Flynn <coflynn@newae.com>

  chipwhisperer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  chipwhisperer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/
module reg_sad(
	input 			reset_i,
	input 			clk,
	input [5:0]    reg_address,  // Address of register
	input [15:0]   reg_bytecnt,  // Current byte count
	input [7:0]    reg_datai,    // Data to write
	output [7:0]   reg_datao,    // Data to read
	input [15:0]   reg_size,     // Total size being read/write
	input          reg_read,     // Read flag
	input  			reg_write,    // Write flag
	input          reg_addrvalid,// Address valid flag
	output			reg_stream,	
	input [5:0]    reg_hypaddress,
	output [15:0]  reg_hyplen,
	
	input [9:0]    ADC_data,
	input          ADC_clk,
	output reg		trig_out									              
   );
	 
	 wire	  reset;
	 assign reset = reset_i;
	 assign reg_stream = 1'b0;
 	  
	 `define SAD_STATUSCFG_ADDR 53
	 `define SAD_REFDATA_ADDR   54
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `SAD_STATUSCFG_ADDR: reg_hyplen_reg <= 4;
				`SAD_REFDATA_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;
	 assign reg_datao = reg_datao_reg;
	 
	 reg [31:0] statuscfg_reg;
	 wire [31:0] statuscfg_reg_read;
	 wire			rst_core;
	
	 assign rst_core = statuscfg_reg[0];
	 
	 assign statuscfg_reg_read[2:0] = statuscfg_reg[2:0];
	 assign statuscfg_reg_read[3] = sad_wr;
	 assign statuscfg_reg_read[31:4] = statuscfg_reg[31:4];
	  	 
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`SAD_STATUSCFG_ADDR: begin reg_datao_reg <= statuscfg_reg_read[reg_bytecnt*8 +: 8]; end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 always @(posedge clk) begin
		if (reset) begin
			statuscfg_reg <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`SAD_STATUSCFG_ADDR: statuscfg_reg[reg_bytecnt*8 +: 8] <= reg_datai;	
				default: ;
			endcase
		end
	 end	 	 
						  					
			 
	wire fifo_rd;
	wire fifo_empty;
	wire fifo_wr;
	wire [15:0] fifo_dout;
	
	assign fifo_wr = (reg_address == `SAD_REFDATA_ADDR) ? reg_write : 1'b0;

	
	fifo_sad_ref fifo_sad_ref_inst (
		.rst(rst_core | reset), // input rst
		.wr_clk(clk), // input wr_clk
		.rd_clk(ADC_clk), // input rd_clk
		.din(reg_datai), // input [9 : 0] din
		.wr_en(fifo_wr), // input wr_en
		.rd_en(fifo_rd), // input rd_en
		.dout(fifo_dout), // output [9 : 0] dout
		.full(), // output full
		.empty(fifo_empty) // output empty
	);
	 	 
	wire [19:0] sad_out;
	wire 			sad_wr;
		 
	sad sad_inst (
        .ap_clk(ADC_clk),
        .ap_rst(rst_core|reset),
        .ap_start(statuscfg_reg[1]),
        .ap_done(),
        .ap_idle(),
        .ap_ready(),
        .max_points_V(7'd128), //6:0
        .ref_points_in_V_dout(fifo_dout[9:0]), //9:0
        .ref_points_in_V_empty_n(~fifo_empty), 
        .ref_points_in_V_read(fifo_rd),
        .datain_V_dout(ADC_data), //9:0
        .datain_V_empty_n(1'b1),
        .datain_V_read(),
        .sumout_V_din(sad_out), //19:0
        .sumout_V_full_n(1'b1),
        .sumout_V_write(sad_wr)
);


	wire [19:0] sad_threshold;
	
	assign sad_threshold = statuscfg_reg[27:8];

	always @(posedge ADC_clk) begin
		if (sad_out < sad_threshold) begin
			trig_out <= 1;
		end else begin
			trig_out <= 0;
		end
	end
	 
 `ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(ADC_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
	 assign cs_data[2:0] = statuscfg_reg[2:0];	 
	 assign cs_data[3] = fifo_rd;
	 assign cs_data[4] = fifo_empty;	 
	 assign cs_data[5] = fifo_wr;
	 assign cs_data[18:9] = fifo_dout[9:0];	 
	 assign cs_data[27:20] = reg_datai;
	 assign cs_data[29] = sad_wr;
	 assign cs_data[49:30] = sad_out;	 	
	 assign cs_data[59:50] = ADC_data;
 `endif
 
endmodule

`undef CHIPSCOPE
