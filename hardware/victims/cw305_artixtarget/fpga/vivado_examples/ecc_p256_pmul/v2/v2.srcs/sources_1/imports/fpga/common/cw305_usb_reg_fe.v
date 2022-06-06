/* 
ChipWhisperer Artix Target - Example frontend to USB interface.

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

module cw305_usb_reg_fe #(
   parameter pADDR_WIDTH = 21,
   parameter pBYTECNT_SIZE = 7,
   parameter pREG_RDDLY_LEN = 3
)(
   input  wire                          usb_clk,
   input  wire                          rst,

   // Interface to host
   input  wire [7:0]                    usb_din,
   output wire [7:0]                    usb_dout,
   output wire                          usb_isout,
   input  wire [pADDR_WIDTH-1:0]        usb_addr,
   input  wire                          usb_rdn,
   input  wire                          usb_wrn,
   input  wire                          usb_alen,        // unused here
   input  wire                          usb_cen,

 // Interface to registers
   output wire [pADDR_WIDTH-1:pBYTECNT_SIZE] reg_address,  // Address of register
   output wire [pBYTECNT_SIZE-1:0]      reg_bytecnt,  // Current byte count
   output reg  [7:0]                    reg_datao,    // Data to write
   input  wire [7:0]                    reg_datai,    // Data to read
   output reg                           reg_read,     // Read flag. One clock cycle AFTER this flag is high
                                                      // valid data must be present on the reg_datai bus
   output wire                          reg_write,    // Write flag. When high on rising edge valid data is
                                                      // present on reg_datao
   output wire                          reg_addrvalid // Address valid flag
);

   reg  [pADDR_WIDTH-1:0] usb_addr_r;
   reg  usb_rdn_r;
   reg  usb_wrn_r;
   reg  usb_cen_r;
   reg  [pREG_RDDLY_LEN-1:0] isoutreg;

   // register USB interface inputs:
   always @(posedge usb_clk) begin
      usb_addr_r <= usb_addr;
      usb_rdn_r <= usb_rdn;
      usb_wrn_r <= usb_wrn;
      usb_cen_r <= usb_cen;
   end

   assign reg_addrvalid = 1'b1;

   // reg_address selects the register:
   assign reg_address = usb_addr_r[pADDR_WIDTH-1:pBYTECNT_SIZE];

   // reg_bytecnt selects the byte within the register:
   assign reg_bytecnt = usb_addr_r[pBYTECNT_SIZE-1:0];

   assign reg_write = ~usb_cen_r & ~usb_wrn_r;

   always @(posedge usb_clk) begin
      if (~usb_cen & ~usb_rdn)
         reg_read <= 1'b1;
      else if (usb_rdn)
         reg_read <= 1'b0;
   end

   // drive output data bus:
   always @(posedge usb_clk) begin
       if (rst) begin
           isoutreg <= 0;
       end
       else begin
           isoutreg[0] <= ~usb_rdn_r;
           isoutreg[pREG_RDDLY_LEN-1:1] <= isoutreg[pREG_RDDLY_LEN-2:0];
       end
   end
   assign usb_isout = (|isoutreg) | (~usb_rdn_r);


   assign usb_dout = reg_datai;

   always @(posedge usb_clk)
      reg_datao <= usb_din;


endmodule

`default_nettype wire
