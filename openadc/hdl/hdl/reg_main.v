`include "includes.v"
//`define CHIPSCOPE

/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the computer interface. It provides a simple interface to the
rest of the board.

Notes on interface:

[ 1 RW A5 A4 A3 A2 A1 A0] - Header
[ Write/Read Size LSB   ] - Size of read/write
[ Write/Read Size MSB   ]
[ Data Byte 0           ] - Payload (Variable Length). On WR sent in, on RD sent out
[ Data Byte 1           ]
 .......................
[ Data Byte N           ]

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
module reg_main(
	input 			reset_i,
	input 			clk,
	
	/* Interface to computer (e.g.: serial, FTDI chip, etc) */
	input 			cmdfifo_rxf,
	input				cmdfifo_txe,
	output			cmdfifo_rd,
	output			cmdfifo_wr,	
	input [7:0]	   cmdfifo_din,
	output [7:0]	cmdfifo_dout,
	// Following is provided for units with half-duplex interface such as FTDI
	output			cmdfifo_isout,
		

	/* Interface to registers */
	output         reg_clk,
	output [5:0]   reg_address,  // Address of register
	output [15:0]  reg_bytecnt,  // Current byte count
	output [7:0]   reg_datao,    // Data to write
	input  [7:0]   reg_datai,    // Data to read
	output [15:0]  reg_size,     // Total size being read/write
	output         reg_read,     // Read flag. One clock cycle AFTER this flag is high
	                             // valid data must be present on the reg_datai bus
	output			reg_write,    // Write flag. When high on rising edge valid data is
	                             // present on reg_datao
	output         reg_addrvalid,// Address valid flag
	input				reg_stream,	
	
	output [5:0]   reg_hypaddress,
	input  [15:0]  reg_hyplen	
    );
		 
    wire       ftdi_rxf_n;
    wire       ftdi_txe_n;	 
    reg        ftdi_rd_n;
    reg        ftdi_wr_n;
	 
	 wire	  		reset;
	 assign 		reset = reset_i;
    	 
    wire [7:0] ftdi_din;
    reg [7:0]  ftdi_dout;
    reg        ftdi_isOutput;
    wire       ftdi_clk;

	 reg 			regout_addrvalid;
	 reg 			regout_read;	 
	 reg			regout_write;
	 
	 assign reg_datao = ftdi_din;
	 assign reg_write = regout_write;
	 assign reg_addrvalid = regout_addrvalid;
	 assign reg_read = regout_read;// & ~ftdi_txe_n; //FIXME???
    
    assign ftdi_clk = clk;
	 assign reg_clk = clk;
	 assign ftdi_txe_n = ~cmdfifo_txe;
	 assign cmdfifo_dout = ftdi_dout;
	 assign cmdfifo_rd = ~ftdi_rd_n;
	 assign cmdfifo_wr = ~ftdi_wr_n;
	 assign cmdfifo_isout = ftdi_isOutput; 
	 
	 assign ftdi_din = cmdfifo_din; 
	 assign ftdi_rxf_n = ~cmdfifo_rxf;

    //For FTDI interface you would do this in the main module that instiated this:
    //assign ftdi_d = ftdi_isOutput ? ftdi_dout : 8'bZ;
    //assign ftdi_din = ftdi_d;
	 
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
		 
	 `undef  IDLE
    `define IDLE            'b0000
    `define ADDR            'b0001
	 `define ADDRWAIT			 'b0010
	 `define BYTECNTLSB      'b0011
	 `define BYTECNTLSBWAIT  'b0100
	 `define BYTECNTMSB      'b0101
    `define DATAWR1         'b0110
    `define DATAWR2         'b0111
    `define DATAWRDONE      'b1000
    `define DATARD1         'b1001
    `define DATARD2         'b1010
	 `define CHECKSUM			 'b1110

	 reg [15:0]					total_bytes;    //Byte count for this transaction
	 reg [7:0]					totalbytes_lsb; //LSB from input 
	 reg [15:0]					bytecnt;
    reg [3:0]              state = `IDLE;
	 reg [3:0]              state_new;
    reg [5:0]              address;

	 assign reg_address = address;
	 assign reg_bytecnt = bytecnt;
	 assign reg_size = total_bytes;

	 //assign cmdfifo_ready = 1'b0;	 
	 assign reg_hypaddress = ftdi_din[5:0];	 	 
		 		 
	 //always @(posedge ftdi_clk) begin
	 always @(reg_datai) begin
		ftdi_dout <= reg_datai;
	 end
	 
	 reg reg_streamdly;
	 
	 always @(posedge ftdi_clk)
	 begin
		if (reset == 1) begin
			reg_streamdly <= 0;
		end else begin
			reg_streamdly <= reg_stream;
		end
	 end
	 	 
    always @(posedge ftdi_clk)
    begin
      if (reset == 1) begin
         state <= `IDLE; 
         ftdi_rd_n <= 1;
         ftdi_wr_n <= 1;
         ftdi_isOutput <= 0;
			
      end else begin
         case (state)
            `IDLE: begin
					regout_addrvalid <= 0;
					regout_read <= 0;
					regout_write <= 0;					
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  ftdi_wr_n <= 1;
                  ftdi_isOutput <= 0;
                  state <= `ADDR;						
               end else begin
						if((reg_streamdly == 0) & (reg_stream == 1)) begin
							ftdi_wr_n <= 1;
							ftdi_rd_n <= 1;
							ftdi_isOutput <= 1;
							state <= `IDLE;						
						end else if(reg_streamdly) begin
							ftdi_wr_n <= 0;
							ftdi_rd_n <= 1;
							ftdi_isOutput <= 1;
							state <= `IDLE;
						end else begin					
							ftdi_rd_n <= 1;
							ftdi_wr_n <= 1;
							ftdi_isOutput <= 0;
							state <= `IDLE;
						end
               end
            end

            `ADDR: begin	
					bytecnt <= 0;			
					total_bytes <= reg_hyplen;										
               address <= ftdi_din[5:0];
               ftdi_wr_n <= 1;
					ftdi_rd_n <= 1;
					regout_addrvalid <= 1;		
					regout_write <= 0;					
					regout_read <= 0;
					ftdi_isOutput <= 0;
               if (ftdi_din[7] == 1) begin
                  if (ftdi_din[6] == 1) begin
                     //MSB means WRITE
                     state_new <= `DATAWR1;               
                  end else begin
                     //MSB means READ                     
                     state_new <= `DATARD1; 
                  end
						
						state <= `ADDRWAIT;						
						
               end else begin
                  ftdi_isOutput <= 0;
                  state <= `IDLE;                  
               end
             end
				 
				`ADDRWAIT: begin
					ftdi_isOutput <= 0;
					regout_write <= 0;					
					regout_read <= 0;
					regout_addrvalid <= 1;
					if (ftdi_rxf_n == 0) begin
						ftdi_rd_n <= 0;
						ftdi_wr_n <= 1;
						state <= `BYTECNTLSB;
					end else begin					
						ftdi_rd_n <= 1;
						ftdi_wr_n <= 1;
						state <= `ADDRWAIT;
					end
				end
				 
				`BYTECNTLSB: begin
					regout_addrvalid <= 1;	
					ftdi_isOutput <= 0;
					ftdi_wr_n <= 1;
					ftdi_rd_n <= 1;
					totalbytes_lsb <= ftdi_din;
					state <= `BYTECNTLSBWAIT;					
					regout_write <= 0;
					regout_read <= 0;					
				end
				
				`BYTECNTLSBWAIT: begin
					ftdi_isOutput <= 0;
					regout_write <= 0;					
					regout_read <= 0;
					regout_addrvalid <= 1;
					if (ftdi_rxf_n == 0) begin
						ftdi_rd_n <= 0;
						ftdi_wr_n <= 1;
						state <= `BYTECNTMSB;
					end else begin					
						ftdi_rd_n <= 1;
						ftdi_wr_n <= 1;
						state <= `BYTECNTLSBWAIT;
					end
				end
				
				`BYTECNTMSB: begin
					regout_addrvalid <= 1;	
					ftdi_wr_n <= 1;		
					ftdi_rd_n <= 1;					
					if((ftdi_din > 0) || (totalbytes_lsb > 0)) begin
						total_bytes[7:0] <= totalbytes_lsb;
						total_bytes[15:8] <= ftdi_din;
               end
					state <= state_new;
					regout_write <= 0;
					if(state_new == `DATARD1) begin
						ftdi_isOutput <= 1;
						regout_read <= 1;
						bytecnt <= 16'h0000;
					end else begin
						bytecnt <= 16'hFFFF;
						ftdi_isOutput <= 0;
						regout_read <= 0;
					end					
				 end
					
            `DATAWR1: begin
					regout_addrvalid <= 1;	
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
					regout_read <= 0;
					regout_write <= 0;
					
               if (ftdi_rxf_n == 0) begin
                  ftdi_rd_n <= 0;
                  state <= `DATAWR2;
						bytecnt <= bytecnt + 16'd1;
               end else begin
                  ftdi_rd_n <= 1;
                  state <= `DATAWR1;						
               end			
										
             end
               
            `DATAWR2: begin
               ftdi_isOutput <= 0;
               ftdi_wr_n <= 1;
               ftdi_rd_n <= 1;
					regout_write <= 1;
					regout_read <= 0;
					regout_addrvalid <= 1;
														
					if ((bytecnt == (total_bytes-1)) || (total_bytes == 0)) begin
						state <= `IDLE;         
					end else begin					
						state <= `DATAWR1;
					end					
				end
				
				`DATARD1: begin
					ftdi_isOutput <= 1;
					ftdi_rd_n <= 1;
					regout_write <= 0;
					regout_read <= 0;
					regout_addrvalid <= 1;
					
					if ( (reg_stream == 1) || ((bytecnt < (total_bytes-16'd1)) && (total_bytes != 0)) ) begin
						//More data to read from other module
						
						if (ftdi_txe_n == 0)  begin
							//Ready for write so go ahead and get more data on bus
							ftdi_wr_n <= 0;
							regout_addrvalid <= 1;
							
							//Will select next byte
							bytecnt <= bytecnt + 16'd1;
							
							//Do read
							state <= `DATARD2;
							
						end else begin
							//Cannot write another byte, hold on
							ftdi_wr_n <= 1;
							regout_addrvalid <= 1;
						end
						
					end else begin
						//No more data to read
												
						if ((bytecnt != (total_bytes-16'd1)))  begin
							//Was in stream mode, exit immediatly
							state <= `IDLE;
							regout_addrvalid <= 0;
							ftdi_wr_n <= 1;
						end else begin
							//Was NOT in stream mode, write last byte first
							if (ftdi_txe_n == 0) begin
								//Do write
								ftdi_wr_n <= 0;
								regout_addrvalid <= 1;
								state <= `IDLE;
							end else begin
								//Hold on - cannot write yet
								regout_addrvalid <= 1;
								ftdi_wr_n <= 1;
							end							
						end							
					end
				
				end
				
				`DATARD2: begin
					ftdi_isOutput <= 1;
					ftdi_rd_n <= 1;
					ftdi_wr_n <= 1;
					regout_write <= 0;
					regout_read <= 1;
					regout_addrvalid <= 1;			
					state <= `DATARD1;
				end
				
				default: begin	
					regout_addrvalid <= 0;	
					ftdi_rd_n <= 1;
               ftdi_wr_n <= 1;
               ftdi_isOutput <= 0;
					regout_read <= 0;
               state <= `IDLE;
				end
             
         endcase
		end
    end 
		  
`ifdef CHIPSCOPE
   assign cs_data[3:0] = state;
   assign cs_data[11:4] = address;
   assign cs_data[12] = ftdi_rxf_n;
	assign cs_data[13] = ftdi_txe_n;
	assign cs_data[14] = ftdi_rd_n;
	assign cs_data[15] = ftdi_wr_n;
	assign cs_data[32:25] = ftdi_dout;
	assign cs_data[43:36] = ftdi_din;
	assign cs_data[51:44] = total_bytes;
	assign cs_data[67:52] = bytecnt;
	assign cs_data[16] = regout_read;
	assign cs_data[75:68] = reg_datai;
 `endif

 
endmodule

`undef CHIPSCOPE
