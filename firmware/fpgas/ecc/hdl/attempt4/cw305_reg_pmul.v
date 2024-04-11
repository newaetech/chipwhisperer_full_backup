/* 
ChipWhisperer Artix Target - Example of connections between example registers
and rest of system.

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

`default_nettype none
`timescale 1ns / 1ps
`include "cw305_pmul_defines.v"

module cw305_reg_pmul #(
   parameter pADDR_WIDTH = 21,
   parameter pBYTECNT_SIZE = 8,
   parameter pDONE_EDGE_SENSITIVE = 1,
   parameter pCT_WIDTH = 256,
   parameter pKEY_WIDTH = 256,
   parameter pCRYPT_TYPE = 3,
   parameter pCRYPT_REV = 4,
   parameter pIDENTIFY = 8'h2e
)(

// Interface to cw305_usb_reg_fe:
   input  wire                                  usb_clk,
   input  wire                                  crypto_clk,
   input  wire                                  reset_i,
   input  wire [pADDR_WIDTH-pBYTECNT_SIZE-1:0]  reg_address,     // Address of register
   input  wire [pBYTECNT_SIZE-1:0]              reg_bytecnt,  // Current byte count
   output reg  [7:0]                            read_data,       //
   input  wire [7:0]                            write_data,      //
   input  wire                                  reg_read,        // Read flag. One clock cycle AFTER this flag is high
                                                                 // valid data must be present on the read_data bus
   input  wire                                  reg_write,       // Write flag. When high on rising edge valid data is
                                                                 // present on write_data
   input  wire                                  reg_addrvalid,   // Address valid flag

// from top:
   input  wire                                  exttrigger_in,

// register inputs:
   input  wire [31:0]                           I_rx_word,
   input  wire [31:0]                           I_ry_word,
   input  wire                                  I_ready,  /* Crypto core ready. Tie to '1' if not used. */
   input  wire                                  I_done,   /* Crypto done. Can be high for one crypto_clk cycle or longer. */
   input  wire                                  I_busy,   /* Crypto busy. */
   input  wire [2:0]                            k_addr,
   input  wire [2:0]                            rx_addr,
   input  wire [2:0]                            ry_addr,
   input  wire                                  rx_wren,
   input  wire                                  ry_wren,
   input  wire [2:0]                            gx_addr,
   input  wire [2:0]                            gy_addr,

   input  wire [31:0]                           I_rx_word_B,
   input  wire [31:0]                           I_ry_word_B,
   output wire [31:0]                           O_k_word_B,

// register outputs:
   output reg  [4:0]                            O_clksettings,
   output reg                                   O_user_led,
   output wire [31:0]                           O_k_word,
   output reg  [31:0]                           O_gx_word,
   output reg  [31:0]                           O_gy_word,
   output reg                                   O_bram_rd_msb,
   output reg                                   O_lfsr_go,
   output reg  [31:0]                           O_lfsr_state,
   output reg  [1:0]                            O_lfsr_rd,
   output reg  [15:0]                           O_noise_enable,
   output reg  [3:0]                            O_lfsr_rd_bank,
   output wire                                  O_start   /* High for one crypto_clk cycle, indicates text ready. */

);

   reg  [7:0]                   reg_read_data;
   reg                          reg_crypt_go_pulse;
   wire                         reg_crypt_go_pulse_crypt;
   reg                          busy_usb;
   reg                          done_r;
   wire                         done_pulse;
   wire                         crypt_go_pulse;
   reg                          go_r;
   reg                          go;
   wire [31:0]                  buildtime;

   reg  [pKEY_WIDTH-1:0]        reg_k;
   reg  [pCT_WIDTH-1:0]         reg_rx;
   reg  [pCT_WIDTH-1:0]         reg_ry;
   reg  [pCT_WIDTH-1:0]         reg_gx;
   reg  [pCT_WIDTH-1:0]         reg_gy;
   
   reg                          rb_sel;
   reg  [pKEY_WIDTH-1:0]        reg_kb;

   (* ASYNC_REG = "TRUE" *) reg  [pKEY_WIDTH-1:0] reg_k_crypt;
   (* ASYNC_REG = "TRUE" *) reg  [pKEY_WIDTH-1:0] reg_kb_crypt;
   (* ASYNC_REG = "TRUE" *) reg  [pKEY_WIDTH-1:0] reg_gx_crypt;
   (* ASYNC_REG = "TRUE" *) reg  [pKEY_WIDTH-1:0] reg_gy_crypt;
   (* ASYNC_REG = "TRUE" *) reg  [pCT_WIDTH-1:0] reg_rx_usb;
   (* ASYNC_REG = "TRUE" *) reg  [pCT_WIDTH-1:0] reg_ry_usb;

   (* ASYNC_REG = "TRUE" *) reg  [1:0] go_pipe;
   (* ASYNC_REG = "TRUE" *) reg  [1:0] busy_pipe;


   always @(posedge crypto_clk) begin
       done_r <= I_done & pDONE_EDGE_SENSITIVE;
   end
   assign done_pulse = I_done & ~done_r;

   always @(posedge crypto_clk) begin
       reg_k_crypt <= reg_k;
       reg_kb_crypt <= reg_kb;
       reg_gx_crypt <= reg_gx;
       reg_gy_crypt <= reg_gy;
       if (rx_wren)
          reg_rx[rx_addr*32 +: 32] <= rb_sel? I_rx_word_B : I_rx_word;
       if (ry_wren)
          reg_ry[ry_addr*32 +: 32] <= rb_sel? I_ry_word_B :I_ry_word;
   end

   always @(posedge usb_clk) begin
       reg_rx_usb <= reg_rx;
       reg_ry_usb <= reg_ry;
   end

   assign O_start = crypt_go_pulse || reg_crypt_go_pulse_crypt;
   assign O_k_word = reg_k_crypt[k_addr*32 +: 32];
   assign O_k_word_B = reg_kb_crypt[k_addr*32 +: 32];

   always @(posedge crypto_clk) begin
      O_gx_word <= reg_gx_crypt[gx_addr*32 +: 32];
      O_gy_word <= reg_gy_crypt[gy_addr*32 +: 32];
   end

   //////////////////////////////////
   // read logic:
   //////////////////////////////////

   always @(*) begin
      if (reg_addrvalid && reg_read) begin
         case (reg_address)
            `REG_CLKSETTINGS:           reg_read_data = O_clksettings;
            `REG_USER_LED:              reg_read_data = O_user_led;
            `REG_CRYPT_TYPE:            reg_read_data = pCRYPT_TYPE;
            `REG_CRYPT_REV:             reg_read_data = pCRYPT_REV;
            `REG_IDENTIFY:              reg_read_data = pIDENTIFY;
            `REG_BUILDTIME:             reg_read_data = buildtime[reg_bytecnt*8 +: 8];
            `REG_CRYPT_GO:              reg_read_data = busy_usb;
            `REG_CRYPT_K:               reg_read_data = reg_k[reg_bytecnt*8 +: 8];
            `REG_KB:                    reg_read_data = reg_kb[reg_bytecnt*8 +: 8];
            `REG_CRYPT_GX:              reg_read_data = reg_gx[reg_bytecnt*8 +: 8];
            `REG_CRYPT_GY:              reg_read_data = reg_gy[reg_bytecnt*8 +: 8];
            `REG_CRYPT_RX:              reg_read_data = reg_rx_usb[reg_bytecnt*8 +: 8];
            `REG_CRYPT_RY:              reg_read_data = reg_ry_usb[reg_bytecnt*8 +: 8];
            default:                    reg_read_data = 0;
         endcase
      end
      else
         reg_read_data = 0;
   end

   // Register output read data to ease timing. If you need read data one clock
   // cycle earlier, simply remove this stage:
   always @(posedge usb_clk)
      read_data <= reg_read_data;

   //////////////////////////////////
   // write logic (USB clock domain):
   //////////////////////////////////
   always @(posedge usb_clk) begin
      if (reset_i) begin
         O_clksettings <= 0;
         O_user_led <= 0;
         reg_crypt_go_pulse <= 1'b0;
         O_bram_rd_msb <= 1'b0;
         O_lfsr_go <= 1'b0;
         O_lfsr_state <= 32'b0;
         O_lfsr_rd <= 2'b0;
         O_lfsr_rd_bank <= 0;
         O_noise_enable <= 0;
         rb_sel <= 0;
      end

      else begin
         if (reg_addrvalid && reg_write) begin
            case (reg_address)
               `REG_CLKSETTINGS:        O_clksettings <= write_data;
               `REG_USER_LED:           O_user_led <= write_data;
               `REG_CRYPT_K:            reg_k[reg_bytecnt*8 +: 8] <= write_data;
               `REG_KB:                 reg_kb[reg_bytecnt*8 +: 8] <= write_data;
               `REG_CRYPT_GX:           reg_gx[reg_bytecnt*8 +: 8] <= write_data;
               `REG_CRYPT_GY:           reg_gy[reg_bytecnt*8 +: 8] <= write_data;
               `REG_BRAM_RD_MSB:        O_bram_rd_msb <= write_data[0];
               `REG_LFSR_STATE:         O_lfsr_state[reg_bytecnt*8 +: 8] <= write_data;
               `REG_LFSR_RD:            O_lfsr_rd <= write_data[1:0];
               `REG_LFSR_RD_BANK:       O_lfsr_rd_bank <= write_data[3:0];
               `REG_NOISE_ENABLE:       O_noise_enable[reg_bytecnt*8 +: 8] <= write_data;
               `REG_RB_SELECT:          rb_sel <= write_data[0];
            endcase
         end
         // REG_CRYPT_GO register is special: writing it creates a pulse. Reading it gives you the "busy" status.
         if ( (reg_addrvalid && reg_write && (reg_address == `REG_CRYPT_GO)) )
            reg_crypt_go_pulse <= 1'b1;
         else
            reg_crypt_go_pulse <= 1'b0;

         // REG_LFSR_GO register is special: writing it creates a pulse. Reading it gives you the "busy" status.
         if ( (reg_addrvalid && reg_write && (reg_address == `REG_LFSR_GO)) )
            O_lfsr_go <= 1'b1;
         else
            O_lfsr_go <= 1'b0;
      end
   end

   always @(posedge crypto_clk) begin
      {go_r, go, go_pipe} <= {go, go_pipe, exttrigger_in};
   end
   assign crypt_go_pulse = go & !go_r;

   cdc_pulse U_go_pulse (
      .reset_i       (reset_i),
      .src_clk       (usb_clk),
      .src_pulse     (reg_crypt_go_pulse),
      .dst_clk       (crypto_clk),
      .dst_pulse     (reg_crypt_go_pulse_crypt)
   );

   always @(posedge usb_clk)
      {busy_usb, busy_pipe} <= {busy_pipe, I_busy};


   `ifdef ILA_REG
       ila_0 U_reg_ila (
	.clk            (usb_clk),                      // input wire clk
	.probe0         (reg_address[7:0]),             // input wire [7:0]  probe0  
	.probe1         (reg_bytecnt),                  // input wire [6:0]  probe1 
	.probe2         (read_data),                    // input wire [7:0]  probe2 
	.probe3         (write_data),                   // input wire [7:0]  probe3 
	.probe4         (reg_read),                     // input wire [0:0]  probe4 
	.probe5         (reg_write),                    // input wire [0:0]  probe5 
	.probe6         (reg_addrvalid),                // input wire [0:0]  probe6 
	.probe7         (reg_read_data),                // input wire [7:0]  probe7 
	.probe8         (exttrigger_in),                // input wire [0:0]  probe8 
	.probe9         (1'b0),                         // input wire [0:0]  probe9
	.probe10        (reg_crypt_go_pulse)            // input wire [0:0]  probe10
       );
   `endif

   `ifdef ILA_CRYPTO
       ila_1 U_reg_aes (
	.clk            (crypto_clk),                   // input wire clk
	.probe0         (O_start),                      // input wire [0:0]  probe0  
	.probe1         (I_done),                       // input wire [0:0]  probe1 
	.probe2         (reg_rx[7:0]),                  // input wire [7:0]  probe2 
	.probe3         (O_k_word[7:0]),                // input wire [7:0]  probe3 
	.probe4         (done_pulse)                    // input wire [0:0]  probe4 
       );
   `endif

   `ifndef __ICARUS__
      USR_ACCESSE2 U_buildtime (
         .CFGCLK(),
         .DATA(buildtime),
         .DATAVALID()
      );
   `else
      assign buildtime = 0;
   `endif


endmodule

`default_nettype wire
