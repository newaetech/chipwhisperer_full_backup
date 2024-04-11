`timescale 1ns / 1ps
//`define CHIPSCOPE
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the serial scard interface. It can be replaced with a variety of other
interfaces such as FTDI chip etc.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
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
module serial_scard_hls_iface(
    input reset_i,
    input clk_i,
	 inout scard_io,
	 input [7:0] scard_cla,
	 input [7:0]  scard_ins,
	 input [7:0]  scard_p1,
	 input [7:0]  scard_p2,
	 input [4:0]  scard_len_command,
	 input [127:0]scard_command,
	 input [4:0]  scard_len_response,
	 output  [127:0]scard_response,
	 output         scard_status,
	 output  [15:0] scard_resp_code,
	 output [7:0] async_data,
	 output       async_datardy,
	 input do_cmd,
	 output busy
    );

	wire clk;
	wire reset;
	wire tx_out;
	reg rx_in;
	wire scardfifo_rxf;
	
	assign clk = clk_i;
	assign reset = reset_i;	
	wire [7:0] datafromrx;

	assign scard_io = (reset_i) ? 1'bz:
	                  (tx_out==1'b0)? 1'b0 : 1'bz;
	always @(posedge clk) rx_in  <= scard_io | ~tx_out;

	wire [7:0] datatotx;
	wire txstart;

	//Serial
   wire txbusy;
   async_transmitter_scard AT (.clk(clk),
							 .rst(reset),
                      .TxD_start(txstart),
                      .TxD_data(datatotx),
                      .TxD(tx_out),
                      .TxD_busy(txbusy));   

	//wire scardtx_ready;
   //assign scardtx_ready = (txbusy | scardfifo_wr);
                
   async_receiver_scard AR (.clk(clk),
						 .rst(reset),
                   .RxD(rx_in),
                   .RxD_data_ready(scardfifo_rxf),
                   .RxD_data(datafromrx),
						 .RxD_endofpacket());

	reg [7:0] async_data_reg;
	reg async_datardy_reg;
	
	always @(posedge clk) begin
		if (scardfifo_rxf) begin
			async_data_reg <= datafromrx;
		end
		async_datardy_reg <= scardfifo_rxf;
	end

	assign async_data = async_data_reg;
	assign async_datardy = async_datardy_reg;

`ifdef CHIPSCOPE
  wire [35:0]                          chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
	 //.CLK(scardfifo_rxf),
    .TRIG0(cs_data) // IN BUS [127:0]
   );
		
	assign cs_data[7:0] = datatotx;
	assign cs_data[8] = txbusy;
	assign cs_data[9] = txstart;
	assign cs_data[17:10] = datafromrx;
	assign cs_data[18] = scardfifo_rxf;	
	assign cs_data[35:20] = scard_resp_code;
	assign cs_data[36] = ap_ready;
	assign cs_data[37] = ap_start;
	assign cs_data[38] = do_cmd;
	assign cs_data[40] = status;
	assign cs_data[41] = scard_io;
	assign cs_data[42] = tx_out;
`endif
	
	
	wire ap_done, ap_ready;
	reg ap_start;
	wire [15:0] resp;
	wire status;
	
	always @(posedge clk) begin
		if (ap_ready | reset) begin
			ap_start <= 1'b0;			
		end else begin
			if (do_cmd) begin
				ap_start <= 1'b1;
			end
		end		
	end
	
	reg uart_in_empty_n;
	wire uart_in_read;
	
	/*
	always @(posedge clk) begin
		if (reset | uart_in_read) begin
			uart_in_empty_n <= 1'b1;
		end else if (scardfifo_rxf) begin
			uart_in_empty_n <= 1'b0;
		end
	end
	*/
	
	scard_hls_generated scard_iface(
        .ap_clk(clk),
        .ap_rst(reset),
        .ap_start(ap_start),
        .ap_done(),
        .ap_idle(),
        .ap_ready(ap_ready),
        .cla(scard_cla),
        .ins(scard_ins),
        .p1(scard_p1),
        .p2(scard_p2),
        .len_command(scard_len_command),
        .command(scard_command),
        .len_response(scard_len_response),
        .response(scard_response),
		  .status(scard_status),
        .uart_out_din(datatotx),
        .uart_out_full_n(~txbusy),
        .uart_out_write(txstart),
        .uart_in_dout(datafromrx),
        .uart_in_empty_n(scardfifo_rxf),
        .uart_in_read(),
        .ap_return(scard_resp_code)
);

	assign busy = ap_start | do_cmd;

endmodule

`undef CHIPSCOPE
