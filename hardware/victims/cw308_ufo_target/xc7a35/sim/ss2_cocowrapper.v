`timescale 1 ns / 1 ps
`default_nettype none

/***********************************************************************
This file is part of the ChipWhisperer Project. See www.newae.com for more
details, or the codebase at http://www.chipwhisperer.com

Copyright (c) 2023, NewAE Technology Inc. All rights reserved.
Author: Jean-Pierre Thibault <jpthibault@newae.com>

  chipwhisperer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  chipwhisperer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/

module ss2_cocowrapper(
    input  wire                         clk, 
    input  wire                         resetn,
    input  wire                         rxd,
    input  wire                         txd,
    // testbench stuff:
    input  wire [31:0]                  errors
);

   parameter pDUMP = 0;

   initial begin
      if (pDUMP) begin
          $dumpfile("ss2.fst");
          $dumpvars(0, ss2_cocowrapper);
      end
   end

   ss2_wrapper #(
       .pBIT_RATE               (6)     // few clocks per bit for faster simulation
   ) U_dut (
       .clk                     (clk       ),
       .resetn                  (resetn    ),
       .rxd                     (rxd       ),
       .txd                     (txd       )
   );

endmodule
`default_nettype wire
