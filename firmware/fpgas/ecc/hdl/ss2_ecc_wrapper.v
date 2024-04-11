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

module ss2_ecc_wrapper #(
    parameter pBIT_RATE = 32  // number of clocks per UART bit; 230400 bps @ 7.37 MHz
)(
    input  wire         clk,
    output wire         clkout,
    input  wire         resetn,
    input  wire         rxd,
    output wire         txd,

    output wire         led1,
    output wire         led2,
    output wire         led3,
    output wire         io3,
    output wire         io4
);

    wire [7:0] dut_data;
    wire [7:0] dut_datao;
    wire [31:0] dut_address;
    wire dut_rdn;
    wire dut_wrn;
    wire dut_cen;
    wire ss2_error;
    wire clk_buf;
    
    assign led2 = ss2_error;

    ss2 #(
        .pBIT_RATE              (pBIT_RATE),
        .pDATA_BITS             (8),
        .pSTOP_BITS             (1),
        .pPARITY_BITS           (0),
        .pPARITY_ENABLED        (0)
    ) U_ss2 (
        .clk                    (clk_buf     ),
        .resetn                 (resetn      ),
        .rxd                    (rxd         ),
        .txd                    (txd         ),

        .dut_data               (dut_datao   ),
        .dut_wdata              (dut_data    ),
        .dut_address            (dut_address ),
        .dut_rdn                (dut_rdn     ),
        .dut_wrn                (dut_wrn     ),
        .dut_cen                (dut_cen     ),
        .error                  (ss2_error   )
    );


    cw305_ecc_p256_pmul_top #(
        .pBYTECNT_SIZE                  (8),
        .pADDR_WIDTH                    (32)
    ) U_cw305_dut (
        .usb_clk                        (clk),
        .usb_clk_buf                    (clk_buf),
        .usb_data                       (dut_data),
        .usb_dout                       (dut_datao),
        .usb_addr                       (dut_address),
        .usb_rdn                        (dut_rdn ),
        .usb_wrn                        (dut_wrn ),
        .usb_cen                        (dut_cen ),
        .usb_trigger                    (1'b0),

        .j16_sel                        (1'b0),
        .k16_sel                        (1'b0),
        .k15_sel                        (1'b0),
        .l14_sel                        (1'b0),
        .pushbutton                     (resetn),

        .led1                           (led1),
        .led2                           (),
        .led3                           (led3),

        .pll_clk1                       (clk_buf),
        .tio_trigger                    (io4),
        .tio_clkout                     (clkout),
        .tio_clkin                      (clk_buf)
    );

    assign io3 = 1'bz;

endmodule
`default_nettype wire

