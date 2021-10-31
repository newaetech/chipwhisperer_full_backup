`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more details,
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
module reg_serialtarget(
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
	output  [15:0] reg_hyplen,
	
	
	output			target_tx,
	input				target_rx									              
   );
	 
	 assign reg_stream = 1'b0;
	 

`ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
`endif
        	  
/*
	 0x21 - FIFO Read/Write
	 
	 0x22 - Transmit FIFO Status
	 
	 0x23 - Receive FIFO Status	  
*/
	 `define TARGSERIALDATA_ADDR	33
	 `define TARGSERIALLEN_ADDR	34
	 `define TARGSERIALBAUD_ADDR  35
  
  	 reg 			rxfifo_rd;
 	 reg		   txfifo_wr;
 	 wire [7:0] rxfifo_data;
	 reg  [31:0] baud_settings;
  
	 reg [15:0] reg_hyplen_reg;
	 assign reg_hyplen = reg_hyplen_reg;
	 
	 always @(reg_hypaddress) begin
		case (reg_hypaddress)
            `TARGSERIALDATA_ADDR: reg_hyplen_reg <= 1;
				`TARGSERIALLEN_ADDR: reg_hyplen_reg <= 2;
				`TARGSERIALBAUD_ADDR: reg_hyplen_reg <= 4;
				default: reg_hyplen_reg<= 0;
		endcase
	 end    
	
	 reg [7:0] reg_datao_reg;	 
	 assign reg_datao = reg_datao_reg;
	 
 
	 /*
	 reg reg_datao_valid_reg;
	 always @(posedge clk) begin
		if (reg_addrvalid) begin
			case (reg_address)
				`TARGSERIALDATA_ADDR: begin reg_datao_valid_reg <= 1; end
				`TARGSERIALLEN_ADDR: begin reg_datao_valid_reg <= 1; end
				`TARGSERIALBAUD_ADDR: begin reg_datao_valid_reg <= 1; end
				default: begin reg_datao_valid_reg <= 0; end	
			endcase
		end else begin
			reg_datao_valid_reg <= 0;
		end
	 end
	 */
		
	 always @(posedge clk) begin
		if (reg_read) begin
			case (reg_address)		
				`TARGSERIALDATA_ADDR: begin reg_datao_reg <= rxfifo_data; end
				`TARGSERIALLEN_ADDR: begin reg_datao_reg <= fifo_count[reg_bytecnt*8 +: 8];	end
				`TARGSERIALBAUD_ADDR: begin reg_datao_reg <= baud_settings[reg_bytecnt*8 +: 8];	end
				default: begin reg_datao_reg <= 0; end
			endcase
		end
	 end
	 
	 wire [6:0] rdfifo_count_tmp;
	 reg  [6:0] rdfifo_count_reg;	 
	 assign fifo_count[14:8] = (rdfifo_count_reg > rdfifo_count_tmp) ? rdfifo_count_reg : rdfifo_count_tmp;
	 
	 always @(posedge clk) begin
	  if ((reg_read) & (reg_address == `TARGSERIALDATA_ADDR)) begin
			rxfifo_rd <= 1'b1;
			rdfifo_count_reg <= rdfifo_count_tmp;
	  end else begin
	      rxfifo_rd <= 1'b0;
	  end
	 end
	 	 
	 reg [7:0] txfifo_data;
	 always @(posedge clk) begin
	  if ((reg_write) & (reg_address == `TARGSERIALDATA_ADDR)) begin	  
			txfifo_wr <= 1'b1;			
			txfifo_data <= reg_datai;
	  end else begin
			txfifo_wr <= 1'b0;
	  end
	 end

	 always @(posedge clk) begin
		if (reset_i) begin
			//16 is width of RxD_Baud8GeneratorInc
			baud_settings[13:0] <=  (((`TARG_UART_BAUD*8)<<(16-7))+((`UART_CLK)>>8))/((`UART_CLK)>>7); //RX
			baud_settings[15:14] <= 2'b00;
			
			//16 is width of TxD_BaudGeneratorInc
			baud_settings[29:16] <=  (((`TARG_UART_BAUD)<<(16-4))+((`UART_CLK)>>5))/((`UART_CLK)>>4); //TX
			baud_settings[31:30] <= 2'b11;
		end else if (reg_write) begin
			if (reg_address == `TARGSERIALBAUD_ADDR) begin
				baud_settings[reg_bytecnt*8 +: 8] <= reg_datai;
			end
		end
	 end
	 
	 wire [15:0] TxD_BaudGeneratorInc;
	 wire [15:0] RxD_Baud8GeneratorInc;
	 assign TxD_BaudGeneratorInc = {2'b00 , baud_settings[29:16]};
	 assign RxD_Baud8GeneratorInc = {2'b00 , baud_settings[13:0]};
	 
	 wire even_parity = baud_settings[15];
	 wire two_stopbits = baud_settings[14];
	 
 `ifdef CHIPSCOPE
	 assign cs_data[5:0] = reg_address;
	 assign cs_data[21:6] = reg_bytecnt;
	 assign cs_data[29:22] = reg_datai;
	 assign cs_data[37:30] = reg_datao;
	 assign cs_data[38] = reg_read;
	 assign cs_data[39] = reg_write;
	 assign cs_data[40] = reg_addrvalid;
	 assign cs_data[46:41] = reg_hypaddress;
	 assign cs_data[62:47] = reg_hyplen;
	 assign cs_data[70:63] = rx_data;
	 assign cs_data[71] = rx_data_rdy;
	 assign cs_data[78:72] = fifo_count[14:8];	
	 assign cs_data[79] = rxfifo_rd;
	 assign cs_data[87:80] = rxfifo_data; 
 `endif
 
	wire [15:0] fifo_count;
	assign fifo_count[7] = 1'b0;
	assign fifo_count[15] = 1'b0;
	

	reg tx_start;
	reg fifo_go;
	
	wire [7:0] tx_data;
	wire       tx_empty;
	wire       tx_busy;
	wire		  txfifo_empty;
	
	always @(posedge clk) begin
		tx_start <= ~tx_busy & ~txfifo_empty & ~tx_start;
		fifo_go <= tx_start;
	end
 
	targ_async_transmitter targ_tx(
	 .clk(clk),
	 .TxD_start(tx_start),
	 .parity_even(even_parity),
	 .two_stopbits(two_stopbits),
	 .TxD_data(tx_data),
	 .TxD(target_tx),
	 .TxD_busy(tx_busy),
	 .TxD_BaudGeneratorInc(TxD_BaudGeneratorInc)
	 );
	 
	fifo_target_tx tx_fifo (
    .clk(clk),
    .rst(reset_i),
    .din(txfifo_data),
    .wr_en(txfifo_wr),
    .rd_en(fifo_go),
    .dout(tx_data),
    .full(),
    .empty(txfifo_empty),
    .data_count(fifo_count[6:0])  );

	wire [7:0] rx_data;
	wire       rx_data_rdy;
	targ_async_receiver targ_rx(
	 .clk(clk),
	 .RxD(target_rx),
	 .parity_even(even_parity),
	 .two_stopbits(two_stopbits),
	 .RxD_data_ready(rx_data_rdy),
	 .RxD_data_error(),
	 .RxD_data(rx_data),
	 .RxD_endofpacket(),
	 .RxD_idle(),
	 .RxD_Baud8GeneratorInc(RxD_Baud8GeneratorInc)
	);	
	
 	fifo_target_tx rx_fifo (
    .clk(clk),
    .rst(reset_i),
    .din(rx_data),
    .wr_en(rx_data_rdy),
    .rd_en(rxfifo_rd),
    .dout(rxfifo_data),
    .full(),
    .empty(),
    .data_count(rdfifo_count_tmp)
	 );
 
 
endmodule

`undef CHIPSCOPE
