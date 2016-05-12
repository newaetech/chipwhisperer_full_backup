/* 
ChipWhisperer Artix Target - Example of communicating through USB

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

module cw305_top(
    // USB interface 
    input wire        usb_clk,      // Clock 
    inout wire [7:0]  usb_data,     // Data for write/read
    input wire [20:0] usb_addr,     // Address data
    input wire        usb_rdn,      // !RD, low when addr valid for read
    input wire        usb_wrn,      // !WR, low when data+addr valid for write
    input wire        usb_cen,      // !CE not used
    input wire        usb_trigger,  // High when trigger requested
    
    
    // Switches and LEDs
    input wire sw1,         // Switch J16
    input wire sw2,         // Switch K16
    input wire sw3,         // Switch K15
    input wire sw4,         // Switch L14
    input wire pushbutton,  // Pushbutton SW4, connected to R1
    output wire led1,       // Red LED,   LED7
    output wire led2,       // Green LED, LED5
    output wire led3,       // Blue LED,  LED6
    
    
    // PLL
    input wire pll_clk1,    //PLL Clock Channel #1
    
    // 20-Pin connector
    output wire tio_trigger,
    output wire tio_clkout,
    input  wire tio_clkin
);

    // Memory connections for USB
    // Assign these to your own wires/registers
    wire [1024*8-1:0] memory_output;    // 1024 bytes, from 0x000 to 0x3FF
    wire [1024*8-1:0] memory_input;     // 1024 bytes, from 0x400 to 0x7FF
    
      
    // Set up USB with memory registers
    usb_module #(
        .MEMORY_WIDTH(10) // 2^10 = 1024 = 0x400 bytes each for input and output memory
    )my_usb(
        .clk(usb_clk),
        .data(usb_data),
        .addr(usb_addr),
        .rd_en(usb_rdn),
        .wr_en(usb_wrn),
        .cen(usb_cen),
        .trigger(usb_trigger),
        .memory_input(memory_input),
        .memory_output(memory_output)
    );    
    
    
    // Set up clock selector
    wire main_clk;
    BUFGMUX_CTRL clock_selector (
        .O(main_clk),
        .I0(pll_clk1),  // Clock from on-board PLL
        .I1(tio_clkin), // Clock from external connector
        .S(0)           // Pick input 0 for now
    );
    
    
    // Put your own stuff below here
    // ------------------------------
    
    // Identifier: always reads as 0x2E 
    // ie: reading from USB address 0x000 returns 0x2E
    wire [7:0] ID;
    assign ID = 8'h2E;
    assign memory_output[0 +: 8] = ID;
    
    // LEDs: show contents of low memory
    // ie: byte at address 0x400 is
    // [ x x x x x LED1 LED2 LED3]
    assign led1 = memory_input[2];
    assign led2 = memory_input[1];
    assign led3 = memory_input[0];             
endmodule
