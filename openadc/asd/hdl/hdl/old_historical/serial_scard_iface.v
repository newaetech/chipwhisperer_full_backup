`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the serial scard interface. It can be replaced with a variety of other
interfaces such as FTDI chip etc.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/
module serial_scard_iface(
    input reset_i,
    input clk_i,
	 inout scard_io,
	 
	 output 			scardfifo_rxe,	 
	 output			scardfifo_txf,
	 output			scardfifo_txe,
	 input			scardfifo_rd,
	 input			scardfifo_wr,	
	 output [7:0]	scardfifo_din,
	 input [7:0]	scardfifo_dout	 
    );

	wire clk;
	wire reset;
	wire tx_out;
	reg rx_in;
	wire scardfifo_rxf;
	
	assign clk = clk_i;
	assign reset = reset_i;	
	wire [7:0] datafromrx;

	assign scard_io = (tx_out==1'b0)? 1'b0 : 1'bz;
	always @(posedge clk) rx_in  <= scard_io | ~tx_out;

	wire [7:0] datatotx;
	reg txstart;

	//Serial
   wire txbusy;
   async_transmitter_scard AT (.clk(clk),
                      .TxD_start(txstart),
                      .TxD_data(datatotx),
                      .TxD(tx_out),
                      .TxD_busy(txbusy));   

	//wire scardtx_ready;
   //assign scardtx_ready = (txbusy | scardfifo_wr);
                
   async_receiver_scard AR (.clk(clk),
                   .RxD(rx_in),
                   .RxD_data_ready(scardfifo_rxf),
                   .RxD_data(datafromrx),
						 .RxD_endofpacket());

	scfifo_write rxfifo (
	  .clk(clk), // input clk
	  .rst(reset), // input rst
	  .din(datafromrx), // input [7 : 0] din
	  .wr_en(scardfifo_rxf), // input wr_en
	  .rd_en(scardfifo_rd), // input rd_en
	  .dout(scardfifo_din), // output [7 : 0] dout
	  //.full(full), // output full
	  .empty(scardfifo_rxe) // output empty
	);

	reg txfifo_rd;

	scfifo_write txfifo (
	  .clk(clk), // input clk
	  .rst(reset), // input rst
	  .din(scardfifo_dout), // input [7 : 0] din
	  .wr_en(scardfifo_wr), // input wr_en
	  .rd_en(txfifo_rd), // input rd_en
	  .dout(datatotx), // output [7 : 0] dout
	  .full(scardfifo_txf), // output full
	  .empty(scardfifo_txe) // output empty
	);
	reg [13:0] state;
	
	always @(posedge clk)
		if (state == 14'b000010) begin
			txfifo_rd <= 1'b0;
			txstart <= 1'b1;
		end else if (state == 14'b000011) begin
			txfifo_rd <= 1'b1;
			txstart <= 1'b0;
		end else begin
			txfifo_rd <= 1'b0;
			txstart <= 1'b0;
		end

	always @(posedge clk)
	case(state)
		14'b00000000000000: if(~scardfifo_txe & ~txbusy) state <= 14'b000001; else state <= 14'b0;
		14'b00000000000001: state <= 14'b000010; //Transmit current byte
		14'b00000000000010: state <= 14'b000011; //Load next byte
		14'b00000000000011: if (~txbusy) state <= 14'b000100; else state <= 14'b11; //Wait for TX to finish
		14'b11111111111111: state <= 14'b000000; //Wait for inter-symbol time
		default:  state <= state +14'b000001;
	endcase


/*
  wire [35:0]                          chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );
	
	assign cs_data[7:0] = datafromrx;
	assign cs_data[8] = scardfifo_rxf;
	assign cs_data[9] = rx_in;
	assign cs_data[17:10] = scardfifo_dout;
	assign cs_data[18] = scardfifo_txf;
	assign cs_data[19] = scardfifo_wr;
*/


endmodule
