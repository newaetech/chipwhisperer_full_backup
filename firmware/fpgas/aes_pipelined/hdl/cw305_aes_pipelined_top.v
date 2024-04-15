/* 
ChipWhisperer Artix Target - Example of connections between example registers
and rest of system.

Copyright (c) 2023, NewAE Technology Inc.
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

module cw305_aes_pipelined_top #(
    parameter pBYTECNT_SIZE = 7,
    parameter pADDR_WIDTH = 21,
    parameter pPT_WIDTH = 128,
    parameter pCT_WIDTH = 128,
    parameter pKEY_WIDTH = 128,
    parameter pFIFO_DEPTH = 8192
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

`ifdef A35
    localparam pACTUAL_FIFO_DEPTH = pFIFO_DEPTH/2;
`else
    localparam pACTUAL_FIFO_DEPTH = pFIFO_DEPTH;
`endif


    wire [pKEY_WIDTH-1:0] crypt_key;
    wire [pPT_WIDTH-1:0] crypt_textout;
    wire [pCT_WIDTH-1:0] crypt_cipherin;
    wire crypt_init;
    wire crypt_ready;
    wire crypt_start;
    wire crypt_busy;
    wire load_key;
    wire aes_write_data;
    wire aes_read_data;

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

`ifndef SS2_WRAPPER
    wire usb_clk_buf;
    wire [7:0] usb_dout;
    assign usb_data = isout? usb_dout : 8'bZ;
`endif

    assign tio_trigger = crypt_busy;

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

    cw305_aes_pipelined_reg #(
       .pBYTECNT_SIZE           (pBYTECNT_SIZE),
       .pADDR_WIDTH             (pADDR_WIDTH),
       .pPT_WIDTH               (pPT_WIDTH),
       .pCT_WIDTH               (pCT_WIDTH),
       .pKEY_WIDTH              (pKEY_WIDTH),
       .pFIFO_DEPTH             (pACTUAL_FIFO_DEPTH)
    ) U_reg_aes (
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

       .I_cipherout             (crypt_cipherin),

       .O_clksettings           (clk_settings),
       .O_user_led              (led3),
       .O_key                   (crypt_key),
       .O_textin                (crypt_textout),
       .O_start                 (crypt_start),
       .O_load_key              (load_key),
       .O_aes_write_data        (aes_write_data),
       .O_aes_read_data         (aes_read_data),
       .I_fifo_out_empty        (fifo_out_empty),
       .O_clear_fifo_errors     (clear_fifo_errors),
       .I_fifo_errors           (fifo_errors)
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


   wire [pCT_WIDTH-1:0] crypt_cipherin_single;
   wire crypt_busy_single;

   wire fifo_out_empty;
   wire clear_fifo_errors;
   wire [3:0] fifo_errors;

if (`HALF_PIPE == 0) begin: gen_full_pipe
   aes_pipeline_top #(
       .pFIFO_DEPTH     (pACTUAL_FIFO_DEPTH)
   ) U_aes_pipeline (
       .clk             (crypt_clk),
       .usb_clk         (usb_clk_buf),
       .reset           (reset),
       .encrypt_go      (crypt_start),
       .write_data      (aes_write_data),
       .read_data       (aes_read_data),
       .load_key        (load_key),
       .key_i           ({crypt_key, 128'h0}),
       .data_i          (crypt_textout),
       .data_o          (crypt_cipherin),
       .busy_o          (crypt_busy),
       .fifo_out_empty  (fifo_out_empty),
       .clear_fifo_errors (clear_fifo_errors),
       .fifo_errors     (fifo_errors)
   );
end

else if (`HALF_PIPE == 3) begin: gen_half_pipe
   aes_half_pipeline_top_fixed #(
       .pFIFO_DEPTH     (pACTUAL_FIFO_DEPTH)
   ) U_aes_pipeline (
       .clk             (crypt_clk),
       .usb_clk         (usb_clk_buf),
       .reset           (reset),
       .encrypt_go      (crypt_start),
       .write_data      (aes_write_data),
       .read_data       (aes_read_data),
       .load_key        (load_key),
       .key_i           ({crypt_key, 128'h0}),
       .data_i          (crypt_textout),
       .data_o          (crypt_cipherin),
       .busy_o          (crypt_busy),
       .fifo_out_empty  (fifo_out_empty),
       .clear_fifo_errors (clear_fifo_errors),
       .fifo_errors     (fifo_errors)
   );
end

else begin: gen_half_pipe
   aes_half_pipeline_top #(
       .pFIFO_DEPTH     (pACTUAL_FIFO_DEPTH)
   ) U_aes_pipeline (
       .clk             (crypt_clk),
       .usb_clk         (usb_clk_buf),
       .reset           (reset),
       .encrypt_go      (crypt_start),
       .write_data      (aes_write_data),
       .read_data       (aes_read_data),
       .load_key        (load_key),
       .key_i           ({crypt_key, 128'h0}),
       .data_i          (crypt_textout),
       .data_o          (crypt_cipherin),
       .busy_o          (crypt_busy),
       .fifo_out_empty  (fifo_out_empty),
       .clear_fifo_errors (clear_fifo_errors),
       .fifo_errors     (fifo_errors)
   );
end



endmodule

`default_nettype wire

