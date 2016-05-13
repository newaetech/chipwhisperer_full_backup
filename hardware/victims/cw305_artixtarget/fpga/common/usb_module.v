/* 
ChipWhisperer Artix Target - Registers for crypto modules.

Copyright (c) 2016, NewAE Technology Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted without restriction. Note that modules within
the project may have additional restrictions, please carefully inspect
additional licenses.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of NewAE Technology Inc.
*/

`timescale 1ns / 1ps
`default_nettype none 
//Defines how long after we keep data-bus active - shouldn't need to change
`define REG_RDDLY_LEN 3

module usb_module(
    input wire        clk_usb,      // Raw clock signal from external USB connections
    inout wire [7:0]  data,     // Data bus
    input wire [21:0] addr,     // Address bus
    input wire        rd_en,    // !RD: low when addr valid for read
    input wire        wr_en,      // !WR: low when data+addr valid for write
    input wire        cen,      // !CE: not used here
    input wire        trigger,  // High when trigger requested
    
    output wire       clk_sys, //Memory bus clock (buffered)
    output reg [MEMORY_BYTES*8-1:0] memory_input,    // Memory bus for input from serial. Upper half of memory
    input wire [MEMORY_BYTES*8-1:0] memory_output    // Memory bus for output to serial.  Lower half of memory
    );
    
    // Default memory width = 256 bytes (0x100)
    parameter MEMORY_WIDTH = 8;                 // 2^8
    parameter MEMORY_BYTES = 1 << MEMORY_WIDTH; // = 256 bytes
    parameter MEMORY_MASK  = MEMORY_BYTES - 1;  // = 0xFF mask

    wire clk_bufg;
    IBUFG clkibuf (
        .O(clk_bufg),
        .I(clk_usb) 
    );
    wire clk_buf;
    BUFG clkbuf(
        .O(clk_buf),
        .I(clk_bufg)
    );
    
    assign clk_sys = clk_buf;
    
    // Internals of USB
    wire [21:0] addr_mask;          // The mask for our addresses
    assign addr_mask = MEMORY_MASK; // Give ourselves 2^8 bytes for each memory
    
    wire [21:0] addr_masked;        // The masked copy of the addresses
    assign addr_masked = addr & addr_mask;
    
    wire addr_section;              // Whether the address is in input or output memory 
    assign addr_section = (addr > addr_mask) ? 1 : 0; 

    
    reg  [7:0] data_out;
           
    always @(posedge wr_en) begin
        if (addr_section) begin
            memory_input[addr_masked*8 +: 8] <= data;
        end
    end
        
    always @(negedge rd_en) begin
        data_out <= memory_output[addr_masked*8 +: 8];
    end
    
    wire isout;
    reg [`REG_RDDLY_LEN-1:0] isoutreg;
          
    //keep high longer
    always @(posedge clk_buf) begin
        isoutreg[0] <= ~rd_en;
        isoutreg[`REG_RDDLY_LEN-1:1] <= isoutreg[`REG_RDDLY_LEN-2:0];
    end
    assign isout = (|isoutreg) | (~rd_en);
    
    wire [7:0] dataout_int;
    
    assign data = isout ? data_out : 8'hZZ;
    
endmodule
