`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the OpenADC main registers. Does not include the actual data
transfer register which is in a seperate file.

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

`define SAKURA_STATUS_ADDR 49
`define SAKURA_FIFO_ADDR	50

module reg_sakura_lbus(
	input wire			reset_i,
	input wire			clk,
	input wire [5:0]  reg_address,  // Address of register
	input wire [15:0] reg_bytecnt,  // Current byte count
	input wire [7:0]  reg_datai,    // Data to write
	inout wire [7:0]  reg_datao,    // Data to read
	input wire [15:0] reg_size,     // Total size being read/write
	input wire        reg_read,     // Read flag
	input wire 			reg_write,    // Write flag
	input wire        reg_addrvalid,// Address valid flag
	output wire			reg_stream,	
	
	input [5:0]    reg_hypaddress,
	output  [15:0] reg_hyplen,
				
	/* SAKURA Local Bus Interface */
   output reg [15:0]  lbus_di_a, // Address and input data  (Controller -> Cryptographic module)
   input wire  [15:0] lbus_do,   // Output data (Cryptographic module -> Controller)  
   output reg         lbus_wrn,  // input data
   output reg         lbus_rdn,  // Assert output data
   output wire	       lbus_clkn, // Clock
	output wire        lbus_rstn, // Reset			              
	output wire 		 lbus_clkint
    );
	 
	wire rdfifo_en;
	wire rdfifo_empty;
	wire [7:0] rdfifo_data;
	
	reg wrfifo_en;
	wire wrfifo_full;
	reg [7:0] wrfifo_data;
	
	wire lbus_clk;
	 
	wire	  reset;
	assign reset = reset_i;	
	assign reg_stream = 0;	 
	     
`ifdef CHIPSCOPE
   wire [35:0] cs_data;   
   wire [35:0]  chipscope_control;
   chipscope_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   chipscope_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(lbus_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
`endif
        	  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 
	 /* 
	 [ X X X X X RD_E WR_F RST]	 
	 */
	 reg [7:0] sakura_status_write;
	 wire [7:0] sakura_status_read;	 
	 wire lbus_rst;

	 assign lbus_rstn = ~lbus_rst;
	 assign lbus_rst = reset | sakura_status_write[0];
	 assign sakura_status_read[7:3] = sakura_status_write[7:3];
	 assign sakura_status_read[2] = rdfifo_empty;
	 assign sakura_status_read[1] = wrfifo_full;
	 assign sakura_status_read[0] = sakura_status_write[0];
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
				`SAKURA_STATUS_ADDR: reg_hyplen_reg <= 1;
            `SAKURA_FIFO_ADDR: reg_hyplen_reg <= 1;
				default: reg_hyplen_reg<= 0;
		endcase
	 end
	
	 reg [7:0] reg_datao_reg;
	 reg reg_datao_valid_reg;
	 reg reg_fifo_datao_valid_reg;
	 assign reg_datao = (reg_datao_valid_reg) ? reg_datao_reg :
							  (reg_fifo_datao_valid_reg) ? rdfifo_data : 8'd0;


	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`SAKURA_STATUS_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 
	 /* Special handling for FIFO */
	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`SAKURA_FIFO_ADDR: begin reg_fifo_datao_valid_reg <= 1; end
				default: begin reg_fifo_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_fifo_datao_valid_reg <= 0;
		end
	 end
	  
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)
				`SAKURA_STATUS_ADDR: reg_datao_reg <= sakura_status_read;
				default: reg_datao_reg <= 0;	
			endcase
		end
	 end
 
 	 always @(posedge clk) begin
		if (reset) begin
			sakura_status_write <= 0;
		end else if (reg_write) begin
			case (reg_address)
				`SAKURA_STATUS_ADDR: sakura_status_write <= reg_datai;
				//SAKURA_FIFO_ADDR done in another area
				default: ;
			endcase
		end
	 end
 
 /*
	 always @(reg_read, reg_address, reg_bytecnt) begin
		 if ((reg_read == 1) && (reg_address == `SAKURA_FIFO_ADDR) && (reg_bytecnt > 16'd0)) begin
			rdfifo_en <= 1;
		 end else begin
			rdfifo_en <= 0;
		 end
	 end
*/
	 
	 always @(reg_write, reg_address, reg_bytecnt) begin
		 if ((reg_write == 1) && (reg_address == `SAKURA_FIFO_ADDR)) begin
			wrfifo_en <= 1;
		 end else begin
			wrfifo_en <= 0;
		 end
	 end
  	 
	wire [7:0] sf_wd;
	wire sf_we, sf_ful, sf_aful;		
	wire [7:0] sf_rd;
	wire sf_re, sf_emp, sf_aemp;
	wire lbus_wrn0, lbus_rdn0;
	wire [15:0] lbus_a;
	wire [15:0] lbus_di;

	assign rdfifo_en = reg_read & (reg_address == `SAKURA_FIFO_ADDR);

   //------------------------------------------------   
	syncfifo_8x31 fifo_rd (
	.rst(reset), // input rst
	.wr_clk(lbus_clk), // input wr_clk
	.rd_clk(clk), // input rd_clk
	.din(sf_wd), // input [7 : 0] din
	.wr_en(sf_we), // input wr_en
	.rd_en(rdfifo_en), // input rd_en
	.dout(rdfifo_data), // output [7 : 0] dout
	.full(sf_ful), // output full
	.almost_full(sf_aful), // output almost_full
	.empty(rdfifo_empty), // output empty
	.almost_empty() // output almost_empty
	);

	syncfifo_8x31 fifo_wr (
	.rst(reset), // input rst
	.wr_clk(clk), // input wr_clk
	.rd_clk(lbus_clk), // input rd_clk
	.din(reg_datai), // input [7 : 0] din
	.wr_en(wrfifo_en), // input wr_en
	.rd_en(sf_re), // input rd_en
	.dout(sf_rd), // output [7 : 0] dout
	.full(wrfifo_full), // output full
	.almost_full(), // output almost_full
	.empty(sf_emp), // output empty
	.almost_empty(sf_aemp) // output almost_empty
	);

   CTRL_LBUS ctrl_lbus
     (.wd(sf_wd), .we(sf_we), .ful(sf_ful), .aful(sf_aful),
      .rd(sf_rd), .re(sf_re), .emp(sf_emp), .aemp(sf_aemp),
      .lbus_a(lbus_a), .lbus_di(lbus_di), .lbus_do(lbus_do),
      .lbus_wrn(lbus_wrn0), .lbus_rdn(lbus_rdn0),
      .clk(lbus_clk), .rst(reset));

   always @(posedge lbus_clk or posedge reset)
     if (reset)  lbus_wrn <= 1;
     else      lbus_wrn <= lbus_wrn0;

   always @(posedge lbus_clk or posedge reset)
     if (reset)  lbus_rdn <= 1;
     else      lbus_rdn <= lbus_rdn0;

   always @(posedge lbus_clk)
     if (lbus_wrn0) lbus_di_a <= lbus_a;
     else           lbus_di_a <= lbus_di;


   MK_CLKRST mk_clkrst  (.clkin(clk), .rst(reset), .clk(lbus_clk));
   
   ODDR2 u0 (.D0(1'b0), .D1(1'b1), .C0(lbus_clk), .C1(~lbus_clk),
             .Q(lbus_clkn),    .CE(1'b1), .R(1'b0), .S(1'b0));
				 
	assign lbus_clkint = lbus_clk;

 `ifdef CHIPSCOPE
	 assign cs_data[15:0] = lbus_di_a;
	 assign cs_data[31:16] = lbus_do;
	 assign cs_data[32] = lbus_wrn;
	 assign cs_data[33] = lbus_rdn;
	 assign cs_data[34] = lbus_rstn;	 
	 
	 
	 /*
	 assign cs_data[7:0] = sf_rd;
	 assign cs_data[8] = sf_emp;
	 assign cs_data[9] = sf_aemp;
	 assign cs_data[10] = sf_re;
	 */
	 
 `endif
 
endmodule

`define CLOCK_DIVIDE 16

//================================================ MK_CLKRST
module MK_CLKRST (clkin, rst, clk);
   //synthesis attribute keep_hierarchy of MK_CLKRST is no;
   
   //------------------------------------------------
   input  clkin, rst;
   output clk;
   
   //------------------------------------------------
   wire   rst_dll;
   wire   refclk;
   wire   clk1x, clk1x_dcm, clkdv_dcm, locked;

   //------------------------------------------------ dll reset
   assign rst_dll = rst;

   DCM_SP #(.CLKIN_PERIOD(20.833),  // Source clock: 48 MHz
	         .CLKIN_DIVIDE_BY_2("TRUE"),
            .CLKDV_DIVIDE(`CLOCK_DIVIDE), // 48 / 16 / 2  = 1.5 MHz
            .CLK_FEEDBACK("1X"))
   u11 (.CLKIN(clkin), .CLKFB(clk1x), .RST(rst_dll),
        .PSEN(1'b0), .PSINCDEC (1'b0), .PSCLK(1'b0), .DSSEN(1'b0),
        .CLK0(clk1x_dcm),     .CLKDV(clkdv_dcm),
        .CLK90(), .CLK180(), .CLK270(),
        .CLK2X(), .CLK2X180(), .CLKFX(), .CLKFX180(),
        .STATUS(), .LOCKED(locked), .PSDONE());
   
   BUFG  u12 (.I(clk1x_dcm), .O(clk1x));
   BUFG  u13 (.I(clkdv_dcm), .O(clk));
endmodule // MK_CLKRST


`undef CHIPSCOPE
