/* 
ChipWhisperer Artix Target - Example of connections between example registers and rest of system.

Copyright (c) 2016, NewAE Technology Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
   
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

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

`include "board.v"

//Defines how long after we keep data-bus active - shouldn't need to change
`define REG_RDDLY_LEN 3

module cw305_top(
    
    /****** USB Interface ******/
    input wire        usb_clk, /* Clock */
    inout wire [7:0]  usb_data,/* Data for write/read */
    input wire [20:0] usb_addr,/* Address data */
    input wire        usb_rdn, /* !RD, low when addr valid for read */
    input wire        usb_wrn, /* !WR, low when data+addr valid for write */
    input wire        usb_cen, /* !CE not used */
    input wire        usb_trigger, /* High when trigger requested */
    
    /****** Buttons/LEDs on Board ******/
    input wire sw1, /* DIP switch J16 */
    input wire sw2, /* DIP switch K16 */
    input wire sw3, /* DIP switch K15 */
    input wire sw4, /* DIP Switch L14 */
    
    input wire pushbutton, /* Pushbutton SW4, connected to R1 */
    
    output wire led1, /* red LED */
    output wire led2, /* green LED */
    output wire led3,  /* blue LED */
    
    /****** PLL ******/
    input wire pll_clk1, //PLL Clock Channel #1
    //input wire pll_clk2, //PLL Clock Channel #2
    
    /****** 20-Pin Connector Stuff ******/
    output wire tio_trigger,
    output wire tio_clkout,
    input  wire tio_clkin
    
    /***** Block Interface to Crypto Core *****/
`ifdef USE_BLOCK_INTERFACE
    ,output wire crypto_clk,
    output wire crypto_rst,
    output wire [`CRYPTO_TEXT_WIDTH-1:0] crypto_textout,
    output wire [`CRYPTO_KEY_WIDTH-1:0] crypto_keyout,
    input  wire [`CRYPTO_CIPHER_WIDTH-1:0] crypto_cipherin,
    output wire crypto_start,
    input wire crypto_ready,
    input wire crypto_done,
    input wire crypto_idle
`endif
    );
    
    wire usb_clk_bufg;
    wire usb_clk_buf;
    IBUFG usbclkibuf (
      .O(usb_clk_bufg),
      .I(usb_clk) );
      
    BUFG usbclkbuf(
      .O(usb_clk_buf),
      .I(usb_clk_bufg)
    );
         
    /* USB CLK Heartbeat */
    reg [24:0] usb_timer_heartbeat;
    always @(posedge usb_clk_buf) usb_timer_heartbeat <= usb_timer_heartbeat +  25'd1;
    assign led1 = usb_timer_heartbeat[24];
    
    /* CRYPT CLK Heartbeat */
    reg [22:0] crypt_clk_heartbeat;
    always @(posedge crypt_clk) crypt_clk_heartbeat <= crypt_clk_heartbeat +  23'd1;
    assign led2 = crypt_clk_heartbeat[22];
                
    reg [`REG_RDDLY_LEN-1:0] isoutreg;
    wire isout;        
    always @(posedge usb_clk_buf) begin
        isoutreg[0] <= ~usb_rdn;
        isoutreg[`REG_RDDLY_LEN-1:1] <= isoutreg[`REG_RDDLY_LEN-2:0];
    end
    assign isout = (|isoutreg) | (~usb_rdn);
    
    wire [7:0] dataout_int;
    assign usb_data = (isout) ? dataout_int : 8'hZZ;
    
    /* Connections between crypto module & registers */
    wire crypt_clk;    
    wire [`CRYPTO_TEXT_WIDTH-1:0] crypt_key;
    wire [`CRYPTO_TEXT_WIDTH-1:0] crypt_textout;
    wire [`CRYPTO_CIPHER_WIDTH-1:0] crypt_cipherin;
    wire crypt_init;
    wire crypt_ready;
    wire crypt_start;
    wire crypt_done;
    
    /******* REGISTERS ********/
    registers reg_inst(
        .clk(usb_clk_buf),
        .datain(usb_data),
        .dataout(dataout_int),
        .addr(usb_addr),
        .wren(~usb_wrn),
        .rden(~usb_rdn),
        
        .user_led(led3),
        .dipsw_1(sw1),
        .dipsw_2(sw2),
                
        .exttrigger_in(usb_trigger),
        
        .pll_clk1(pll_clk1),
        .cw_clkin(tio_clkin),
        .cw_clkout(tio_clkout),
       
        .crypt_type(8'h02),
        .crypt_rev(8'h03),
        
        .cryptoclk(crypt_clk),
        .key(crypt_key),
        .textin(crypt_textout),
        .cipherout(crypt_cipherin),
               
        .init(crypt_init),
        .ready(crypt_ready),
        .start(crypt_start),
        .done(crypt_done)        
    );
  
  /* Block interface is used by the IP Catalog. If you are using block-based
     design define USE_BLOCK_INTERFACE in board.v .
  */
`ifdef USE_BLOCK_INTERFACE
    assign crypto_clk = crypt_clk;
    assign crypto_rst = crypt_init;
    assign crypto_keyout = crypt_key;
    assign crypto_textout = crypt_textout;
    assign crypt_cipherin = crypto_cipherin;
    assign crypto_start = crypt_start;
    assign crypt_ready = crypto_ready;
    assign crypt_done = crypto_done;
    assign tio_trigger = ~crypto_idle;
`endif
  /******** START CRYPTO MODULE CONNECTIONS ****************/  
   /* The following can have your crypto module inserted.
   
      This is an example of the Goolge Vault AES module.
      
      You can use the ILA to view waveforms if needed, which
      requires an external USB-JTAG adapter (such as Xilinx Platform
      Cable USB).
   */


`ifdef GOOGLE_VAULT_AES
   wire aes_clk;
   wire [127:0] aes_key;
   wire [127:0] aes_pt;
   wire [127:0] aes_ct;
   wire aes_load;
   wire aes_busy;
  
   assign aes_clk = crypt_clk;
   assign aes_key = crypt_key;
   assign aes_pt = crypt_textout;
   assign crypt_cipherin = aes_ct;
   assign aes_load = crypt_start;
   assign crypt_ready = 1'b1;
   assign crypt_done = ~aes_busy;
 
   /* Example AES Core */
   aes_core aes_core (
       .clk(aes_clk),
       .load_i(aes_load),
       .key_i({aes_key, 128'h0}),
       .data_i(aes_pt),
       .size_i(2'd0), //AES128
       .dec_i(1'b0),//enc mode
       .data_o(aes_ct),
       .busy_o(aes_busy)
   );
   assign tio_trigger = aes_busy;
`endif
         
   /******** END CRYPTO MODULE CONNECTIONS ****************/
    
endmodule
