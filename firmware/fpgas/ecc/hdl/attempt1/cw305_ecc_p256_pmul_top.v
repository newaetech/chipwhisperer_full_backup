/* 
ChipWhisperer Artix Target - Example of connections between example registers
and rest of system.

Copyright (c) 2016-2020, NewAE Technology Inc.
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

module cw305_ecc_p256_pmul_top #(
    parameter pBYTECNT_SIZE = 8,
    parameter pADDR_WIDTH = 21,
    parameter pPT_WIDTH = 256,
    parameter pCT_WIDTH = 256,
    parameter pKEY_WIDTH = 256
)(
    // USB Interface
    input wire                          usb_clk,        // Clock
`ifdef SS2_WRAPPER
    output wire                         usb_clk_buf,    // if needed by parent module
    input  wire [7:0]                   usb_data,
    output wire [7:0]                   usb_dout,
`else
    inout wire [7:0]                    usb_data,       // Data for write/read
`endif
    input wire [pADDR_WIDTH-1:0]        usb_addr,       // Address
    input wire                          usb_rdn,        // !RD, low when addr valid for read
    input wire                          usb_wrn,        // !WR, low when data+addr valid for write
    input wire                          usb_cen,        // !CE, active low chip enable
    input wire                          usb_trigger,    // High when trigger requested

    // Buttons/LEDs on Board
    input wire                          j16_sel,        // DIP switch J16
    input wire                          k16_sel,        // DIP switch K16
    input wire                          k15_sel,        // DIP switch K15
    input wire                          l14_sel,        // DIP Switch L14
    input wire                          pushbutton,     // Pushbutton SW4, connected to R1, used here as reset
    output wire                         led1,           // red LED
    output wire                         led2,           // green LED
    output wire                         led3,           // blue LED

    // PLL
    input wire                          pll_clk1,       //PLL Clock Channel #1
    //input wire                        pll_clk2,       //PLL Clock Channel #2 (unused in this example)

    // 20-Pin Connector Stuff
    output wire                         tio_trigger,
    output wire                         tio_clkout,
    input  wire                         tio_clkin

    // Block Interface to Crypto Core
`ifdef USE_BLOCK_INTERFACE
   ,output wire                         crypto_clk,
    output wire                         crypto_rst,
    output wire [pPT_WIDTH-1:0]         crypto_textout,
    output wire [pKEY_WIDTH-1:0]        crypto_keyout,
    input  wire [pCT_WIDTH-1:0]         crypto_cipherin,
    output wire                         crypto_start,
    input wire                          crypto_ready,
    input wire                          crypto_done,
    input wire                          crypto_busy,
    input wire                          crypto_idle
`endif
    );

`ifndef SS2_WRAPPER
    wire usb_clk_buf;
    wire [7:0] usb_dout;
    assign usb_data = isout? usb_dout : 8'bZ;
`endif

    wire crypt_init;
    wire crypt_ready;
    wire crypt_start;
    wire crypt_done;
    wire crypt_busy;

    wire isout;
    wire [pADDR_WIDTH-pBYTECNT_SIZE-1:0] reg_address;
    wire [pBYTECNT_SIZE-1:0] reg_bytecnt;
    wire reg_addrvalid;
    wire [7:0] write_data;
    wire [7:0] read_data;
    wire reg_read;
    wire reg_write;
    wire [4:0] clk_settings;
    wire crypt_clk;    

    wire resetn = pushbutton;
    wire reset = !resetn;


    // USB CLK Heartbeat
    reg [24:0] usb_timer_heartbeat;
    always @(posedge usb_clk_buf) usb_timer_heartbeat <= usb_timer_heartbeat +  25'd1;
    assign led1 = usb_timer_heartbeat[24];

    // CRYPT CLK Heartbeat
    reg [22:0] crypt_clk_heartbeat;
    always @(posedge crypt_clk) crypt_clk_heartbeat <= crypt_clk_heartbeat +  23'd1;
    assign led2 = crypt_clk_heartbeat[22];


    cw305_usb_reg_fe #(
       .pBYTECNT_SIZE           (pBYTECNT_SIZE),
       .pADDR_WIDTH             (pADDR_WIDTH)
    ) U_usb_reg_fe (
       .rst                     (reset),
       .usb_clk                 (usb_clk_buf), 
       .usb_din                 (usb_data), 
       .usb_dout                (usb_dout), 
       .usb_rdn                 (usb_rdn), 
       .usb_wrn                 (usb_wrn),
       .usb_cen                 (usb_cen),
       .usb_alen                (1'b0),                 // unused
       .usb_addr                (usb_addr),
       .usb_isout               (isout), 
       .reg_address             (reg_address), 
       .reg_bytecnt             (reg_bytecnt), 
       .reg_datao               (write_data), 
       .reg_datai               (read_data),
       .reg_read                (reg_read), 
       .reg_write               (reg_write), 
       .reg_addrvalid           (reg_addrvalid)
    );

    wire [31:0] k_word;
    wire [31:0] rx_word;
    wire [31:0] ry_word;
    wire [31:0] gx_word;
    wire [31:0] gy_word;
    wire [2:0] k_addr;
    wire [2:0] rx_addr;
    wire [2:0] ry_addr;
    wire [2:0] gx_addr;
    wire [2:0] gy_addr;
    wire rx_wren;
    wire ry_wren;
    wire bram_rd_msb;


    cw305_reg_pmul #(
       .pBYTECNT_SIZE           (pBYTECNT_SIZE),
       .pADDR_WIDTH             (pADDR_WIDTH),
       .pCT_WIDTH               (pCT_WIDTH),
       .pKEY_WIDTH              (pKEY_WIDTH)
    ) U_reg_pmul (
       .reset_i                 (reset),
       .crypto_clk              (crypt_clk),
       .usb_clk                 (usb_clk_buf), 
       .reg_address             (reg_address[pADDR_WIDTH-pBYTECNT_SIZE-1:0]), 
       .reg_bytecnt             (reg_bytecnt), 
       .read_data               (read_data), 
       .write_data              (write_data),
       .reg_read                (reg_read), 
       .reg_write               (reg_write), 
       .reg_addrvalid           (reg_addrvalid),

       .exttrigger_in           (usb_trigger),

       .I_rx_word               (rx_word),
       .I_ry_word               (ry_word),
       .I_ready                 (crypt_ready),
       .I_done                  (crypt_done),
       .I_busy                  (crypt_busy),
       .k_addr                  (k_addr),
       .gx_addr                 (gx_addr),
       .gy_addr                 (gy_addr),
       .rx_addr                 (rx_addr),
       .ry_addr                 (ry_addr),
       .rx_wren                 (rx_wren),
       .ry_wren                 (ry_wren),
       .O_k_word                (k_word),
       .O_gx_word               (gx_word),
       .O_gy_word               (gy_word),
       .O_bram_rd_msb           (bram_rd_msb),

       .O_clksettings           (clk_settings),
       .O_user_led              (led3),
       .O_start                 (crypt_start)
    );


    clocks U_clocks (
       .usb_clk                 (usb_clk),
       .usb_clk_buf             (usb_clk_buf),
       .I_j16_sel               (j16_sel),
       .I_k16_sel               (k16_sel),
       .I_clock_reg             (clk_settings),
       .I_cw_clkin              (tio_clkin),
       .I_pll_clk1              (pll_clk1),
       .O_cw_clkout             (tio_clkout),
       .O_cryptoclk             (crypt_clk)

    );



   wire ecc_rdy;
   assign crypt_ready = 1'b1;
   assign crypt_done = ecc_rdy;
   assign crypt_busy = ~ecc_rdy;
   assign tio_trigger = crypt_busy;

   curve_mul_256 U_curve_mul_256 (
      .clk      (crypt_clk),
      .rst_n    (resetn),
      .ena      (crypt_start),
      .rdy      (ecc_rdy),
      .k_addr   (k_addr),
      .rx_addr  (rx_addr),
      .ry_addr  (ry_addr),
      .rx_wren  (rx_wren),
      .ry_wren  (ry_wren),
      .k_din    (k_word),
      .rx_dout  (rx_word),
      .ry_dout  (ry_word),

      /// XXX NEW
      .brom_g_x_addr           (gx_addr),
      .brom_g_y_addr           (gy_addr),
      .brom_g_x_dout           (gx_word),
      .brom_g_y_dout           (gy_word),
      .bram_rd_msb             (bram_rd_msb)

   );


endmodule

`default_nettype wire

