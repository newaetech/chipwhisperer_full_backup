/* 
ChipWhisperer Artix Target - SPI Feed-Through to allow programming of on-board SPI Flash

Copyright (c) 2020, NewAE Technology Inc.
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
    
    /****** USB Interface ******/
    input wire        usb_clk, /* Clock */
    
    input wire        usb_spare1, /* Routes to MOSI pin on SPI Flash */
    input wire        usb_spare2, /* Routes to CS pin on SPI Flash */
    
    /****** SPI Interface *****/
    //input wire        flash_din, //Not used - routes to SAM3U already
    output wire       flash_dout,
    output wire       flash_cs,
    //output wire       flash_sck, //Not used - routes to SAM3U already
    
    /****** Buttons/LEDs on Board ******/
    
    output wire led1, /* red LED */
    output wire led2, /* green LED */
    output wire led3,  /* blue LED */
    
    /****** PLL ******/
    input wire pll_clk1 //PLL Clock Channel #1
    );
    
    wire usb_clk_buf;
    
    /* USB CLK Heartbeat */
    reg [24:0] usb_timer_heartbeat;
    always @(posedge usb_clk_buf) usb_timer_heartbeat <= usb_timer_heartbeat +  25'd1;
    assign led1 = usb_timer_heartbeat[24];
    
    assign flash_dout = usb_spare1;
    assign flash_cs = usb_spare2;
    
    assign led2 = usb_spare1;
    assign led3 = usb_spare2;
    
endmodule
