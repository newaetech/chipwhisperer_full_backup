`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the serial interface. It can be replaced with a variety of other
interfaces such as FTDI chip etc.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/
module serial_reg_iface(
    input reset_i,
    input clk_i,
	 input rx_i,
	 output tx_o,
	 
	 output reg		cmdfifo_rxf,
	 output			cmdfifo_txe,
	 input			cmdfifo_rd,
	 input			cmdfifo_wr,	
	 output [7:0]	cmdfifo_din,
	 input [7:0]	cmdfifo_dout
    );

	wire clk;
	wire tx_out;
	wire rx_in;
	
	assign clk = clk_i;
	assign rx_in = rx_i;
	assign tx_o = tx_out;


	reg [7:0] dataout;
	wire [7:0] data;
	wire data_ready;
	assign cmdfifo_din = dataout;

	always @(posedge clk) begin
		if (data_ready) begin
			dataout <= data;
		end
	end
	
	always @(posedge clk or posedge cmdfifo_rd) begin
		if (cmdfifo_rd) begin
			cmdfifo_rxf <= 0;
		end else begin
			if (data_ready) begin
				cmdfifo_rxf <= 1;
			end
		end
	end

	//Serial
    wire txbusy;
    async_transmitter AT (.clk(clk),
                      .TxD_start(cmdfifo_wr),
                      .TxD_data(cmdfifo_dout),
                      .TxD(tx_out),
                      .TxD_busy(txbusy));   
                
   assign cmdfifo_txe = ~(txbusy | cmdfifo_wr);
                
    async_receiver AR (.clk(clk),
                   .RxD(rx_in),
						 .RxD_idle(),
                   .RxD_data_ready(data_ready),
						 .RxD_data_error(),
                   .RxD_data(data),
						 .RxD_endofpacket());
endmodule
