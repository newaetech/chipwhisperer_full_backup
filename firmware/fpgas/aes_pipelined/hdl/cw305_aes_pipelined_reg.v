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
`include "cw305_aes_pipelined_defines.v"

module cw305_aes_pipelined_reg #(
   parameter pADDR_WIDTH = 21,
   parameter pBYTECNT_SIZE = 7,
   parameter pDONE_EDGE_SENSITIVE = 1,
   parameter pPT_WIDTH = 128,
   parameter pCT_WIDTH = 128,
   parameter pKEY_WIDTH = 128,
   parameter pCRYPT_TYPE = 4,
   parameter pCRYPT_REV = 1,
   parameter pIDENTIFY = 8'h2e,
   parameter pFIFO_DEPTH = 512
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
   input  wire [pCT_WIDTH-1:0]                  I_cipherout,

// register outputs:
   output reg  [4:0]                            O_clksettings,
   output reg                                   O_user_led,
   output wire [pKEY_WIDTH-1:0]                 O_key,
   output wire [pPT_WIDTH-1:0]                  O_textin,
   output wire                                  O_start,  /* High for one crypto_clk cycle, indicates text ready. */
   output wire                                  O_load_key,
   output reg                                   O_aes_write_data,
   output reg                                   O_aes_read_data,
   input  wire                                  I_fifo_out_empty,
   output reg                                   O_clear_fifo_errors,
   input  wire [3:0]                            I_fifo_errors
);

   reg  [7:0]                   reg_read_data;
   reg  [pCT_WIDTH-1:0]         reg_crypt_cipherin;
   reg  [pKEY_WIDTH-1:0]        reg_crypt_key;
   reg  [pPT_WIDTH-1:0]         reg_crypt_textin;
   reg                          reg_crypt_go_pulse;
   wire                         reg_crypt_go_pulse_crypt;
   reg                          reg_key_go_pulse;
   wire                         reg_key_go_pulse_crypt;
   reg                          reg_pre_expand_key;

   wire                         crypt_go_pulse;
   reg                          go_r;
   reg                          go;
   wire [31:0]                  buildtime;

   (* ASYNC_REG = "TRUE" *) reg  [pKEY_WIDTH-1:0] reg_crypt_key_crypt;
   (* ASYNC_REG = "TRUE" *) reg  [1:0] go_pipe;
   (* ASYNC_REG = "TRUE" *) reg  [1:0] busy_pipe;
   (* ASYNC_REG = "TRUE" *) reg  reg_pre_expand_key_crypt;

   wire [31:0] fifo_depth = pFIFO_DEPTH;


   always @(posedge crypto_clk) begin
       reg_crypt_key_crypt <= reg_crypt_key;
       reg_pre_expand_key_crypt <= reg_pre_expand_key;
   end

   assign O_textin = reg_crypt_textin;
   assign O_key = reg_crypt_key_crypt;
   assign O_start = crypt_go_pulse || reg_crypt_go_pulse_crypt;
   assign O_load_key = (reg_pre_expand_key_crypt)? reg_key_go_pulse_crypt : O_start;

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
            `REG_CRYPT_GO:              reg_read_data = I_fifo_out_empty;
            `REG_CRYPT_KEY:             reg_read_data = reg_crypt_key[reg_bytecnt*8 +: 8];
            `REG_CRYPT_TEXTIN:          reg_read_data = reg_crypt_textin[reg_bytecnt*8 +: 8];
            `REG_CRYPT_CIPHERIN:        reg_read_data = reg_crypt_cipherin[reg_bytecnt*8 +: 8];
            `REG_CRYPT_CIPHEROUT:       reg_read_data = I_cipherout[reg_bytecnt*8 +: 8];
            `REG_CRYPT_PRE_EXPAND_KEY:  reg_read_data = {7'b0, reg_pre_expand_key};
            `REG_FIFO_ERRORS:           reg_read_data = {4'b0, I_fifo_errors};
            `REG_BUILDTIME:             reg_read_data = buildtime[reg_bytecnt*8 +: 8];
            `REG_FIFO_DEPTH:            reg_read_data = fifo_depth[reg_bytecnt*8 +: 8];
            `REG_HALF_PIPE:             reg_read_data = `HALF_PIPE;
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
         reg_key_go_pulse <= 1'b0;
      end

      else begin
         if (reg_addrvalid && reg_write) begin
            case (reg_address)
               `REG_CLKSETTINGS:        O_clksettings <= write_data;
               `REG_USER_LED:           O_user_led <= write_data;
               `REG_CRYPT_TEXTIN:       reg_crypt_textin[reg_bytecnt*8 +: 8] <= write_data;
               `REG_CRYPT_CIPHERIN:     reg_crypt_cipherin[reg_bytecnt*8 +: 8] <= write_data;
               `REG_CRYPT_KEY:          reg_crypt_key[reg_bytecnt*8 +: 8] <= write_data;
               `REG_CRYPT_PRE_EXPAND_KEY: reg_pre_expand_key <= write_data[0];
               `REG_FIFO_ERRORS:        O_clear_fifo_errors <= write_data[0];
            endcase
         end

         // REG_CRYPT_GO register is special: writing it creates a pulse. Reading it gives you the "busy" status.
         if ( (reg_addrvalid && reg_write && (reg_address == `REG_CRYPT_GO)) )
            reg_crypt_go_pulse <= 1'b1;
         else
            reg_crypt_go_pulse <= 1'b0;

         // REG_CRYPT_GO register is special: writing it creates a pulse. Reading it gives you the "busy" status.
         if ( (reg_addrvalid && reg_write && (reg_address == `REG_KEY_GO)) && ~reg_key_go_pulse)
            reg_key_go_pulse <= 1'b1;
         else
            reg_key_go_pulse <= 1'b0;

         // REG_CRYPT_CIPHEROUT register is special: writing it creates a pulse.
         if (reg_addrvalid && reg_write && (reg_address == `REG_CRYPT_CIPHEROUT) && ~O_aes_read_data)
            O_aes_read_data <= 1'b1;
         else
            O_aes_read_data <= 1'b0;

         // REG_CRYPT_WRITE_PT register is special: writing it creates a pulse.
         if (reg_addrvalid && reg_write && (reg_address == `REG_CRYPT_WRITE_PT) && ~O_aes_write_data)
            O_aes_write_data <= 1'b1;
         else
            O_aes_write_data <= 1'b0;


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

   cdc_pulse U_key_pulse (
      .reset_i       (reset_i),
      .src_clk       (usb_clk),
      .src_pulse     (reg_key_go_pulse),
      .dst_clk       (crypto_clk),
      .dst_pulse     (reg_key_go_pulse_crypt)
   );


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
	.probe2         (I_cipherout[7:0]),             // input wire [7:0]  probe2 
	.probe3         (O_textin[7:0]),                // input wire [7:0]  probe3 
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
