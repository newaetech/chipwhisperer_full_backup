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

`default_nettype none
`timescale 1ns / 1ps

module cw305_reg_test_ss2 #(
   parameter pADDR_WIDTH = 32,
   parameter pBYTECNT_SIZE = 8
)(

// Interface to cw305_usb_reg_fe:
   input  wire                                  usb_clk,
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

// register outputs:
   input  wire                                  ss2_error,
   output reg                                   O_user_led,
   output wire                                  io3,
   output wire                                  io4
);

   reg  [7:0]   reg_read_data;
   reg  [7:0]   reg_test1 = 8'ha0;
   reg  [31:0]  reg_test2 = 32'h1234_5678;
   reg  [127:0] reg_test3 = 128'h9876_5432_1234_5678_0987_6543_2123_4567;
   reg  [127:0] reg_test4 = 128'habcd_efed_cba0_9876_5432_1012_3456_789a;
   reg  [1:0]   reg_io34;
   //reg  [1:0]   reg_io34 = 2'b11;
   wire [31:0]  buildtime;

   assign io3 = reg_io34[1];
   assign io4 = reg_io34[0];

   //////////////////////////////////
   // read logic:
   //////////////////////////////////
   always @(*) begin
      if (reg_addrvalid && reg_read) begin
         case (reg_address)
            24'h000000:                 reg_read_data = {7'b0, O_user_led};
            24'h000001:                 reg_read_data = reg_test1;
            24'h000002:                 reg_read_data = {7'b0, ss2_error};
            24'h000010:                 reg_read_data = reg_test2[reg_bytecnt*8 +: 8];
            24'h112233:                 reg_read_data = reg_test3[reg_bytecnt*8 +: 8];
            24'h45678a:                 reg_read_data = reg_test4[reg_bytecnt*8 +: 8];
            24'haabbcc:                 reg_read_data = buildtime[reg_bytecnt*8 +: 8];
            24'h000034:                 reg_read_data = {6'b0, reg_io34};
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
      if (reset_i)
         O_user_led <= 0;

      else begin
         if (reg_addrvalid && reg_write) begin
            case (reg_address)
               24'h000000:              O_user_led <= write_data[0];
               24'h000001:              reg_test1 <= write_data;
               24'h000010:              reg_test2[reg_bytecnt*8 +: 8] <= write_data;
               24'h112233:              reg_test3[reg_bytecnt*8 +: 8] <= write_data;
               24'h45678a:              reg_test4[reg_bytecnt*8 +: 8] <= write_data;
               24'h000034:              reg_io34 <= write_data[1:0];
            endcase
         end
      end
   end

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
