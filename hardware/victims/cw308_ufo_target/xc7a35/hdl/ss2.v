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

module ss2 #(
    parameter pBIT_RATE = 8, //TODO!
    parameter pDATA_BITS = 8,
    parameter pSTOP_BITS = 1,
    parameter pPARITY_BITS = 0,
    parameter pPARITY_ENABLED = 0,
    parameter pADDRESS_WIDTH = 32
)(
    input  wire         clk,
    input  wire         reset,
`ifdef COCOTB_SIM
    // for testbench/debug: (TODO: needed?)
    output wire         debug_rx_valid_byte,
    output wire [7:0]   debug_rx_data,
`endif
    input  wire         rxd,
    output wire         txd,

    output wire         led1,
    output wire         led2,
    output wire         led3
);

    wire rx_data_ready;
    wire [8:0] rx_data_word;
    reg  rx_ack;
    wire rx_syn;
    reg  tx_syn;
    wire [7:0] tx_data_byte;
    wire tx_ready;

    reg [7:0] rx_command;
    reg [7:0] rx_subcommand;
    reg [7:0] rx_len;
    reg rx_len_acquired;
    reg [2:0] rx_state;
    reg [7:0] rx_byte_count;
    reg [7:0] rx_zero_offset;
    reg [7:0] rx_zero_count;
    //wire rx_insert_zero = (rx_state == pS_RX_INSERT_ZERO);
    reg rx_insert_zero;
    reg rx_invalid_command = 1'b0;
    reg rx_crc_error;
    reg rx_delimiter_error = 1'b0;
    reg rx_crc_ok;
    wire [7:0] calculated_crc;
    reg crc_init = 1'b0;

    reg [31:0] dut_address;
    reg dut_cen;
    reg dut_cen_r;
    reg dut_wrn;
    reg dut_rdn;
    reg [7:0] dut_wdata;
    wire [7:0] dut_data = dut_wrn? 8'bz : dut_wdata;

    reg  error = 1'b0;
    always @ (posedge clk) begin// TODO: OR in other error conditions here
        if ( rx_invalid_command || 
             rx_crc_error || 
             rx_delimiter_error || 
            (rx_data_ready && rx_insert_zero) ||
            (fifo_underflow || fifo_overflow) 
           )
            error <= 1'b1;
    end

    wire [7:0] rx_data_byte = rx_data_word[8:1];
    wire [7:0] rx_data = (rx_insert_zero)? 8'b0 : rx_data_byte;

`ifdef COCOTB_SIM
    assign debug_rx_valid_byte = rx_valid_byte;
    assign debug_rx_data = rx_data;
`endif


    localparam pS_RX_IDLE = 3'd0;
    localparam pS_RX_NUM_BYTES = 3'd1;
    localparam pS_RX_RX = 3'd2;
    localparam pS_RX_INSERT_ZERO = 3'd3;
    localparam pS_RX_WAIT_NUM_BYTES = 3'd4;
    localparam pS_RX_DONE = 3'd5;
    localparam pS_RX_DELIMITER = 3'd6;

    wire rx_valid_byte = rx_insert_zero || (rx_data_ready && (rx_state != pS_RX_IDLE) && 
                                                             (rx_state != pS_RX_DELIMITER) &&
                                                             (rx_state != pS_RX_WAIT_NUM_BYTES));

    wire writing = (rx_subcommand == 8'h57);
    wire reading = (rx_subcommand == 8'h52);

    always @ (posedge clk) begin
        dut_cen_r <= dut_cen;
        if (dut_cen && ~dut_cen_r)
            dut_address <= dut_address + 1;
        else if (rx_valid_byte) begin
            case (rx_byte_count)
                0: rx_command <= rx_data;
                1: rx_subcommand <= rx_data;
                2: rx_len <= rx_data;
                3: dut_address[7:0]   <= rx_data;
                4: dut_address[15:8]  <= rx_data;
                5: dut_address[23:16] <= rx_data;
                6: dut_address[31:24] <= rx_data;
            endcase

            if ((rx_byte_count > 6) && (rx_byte_count < rx_len + 3) && writing) begin
                dut_cen <= 1'b0;
                dut_wdata <= rx_data;
            end
            else 
                dut_cen <= 1'b1;

        end
        else 
            dut_cen <= 1'b1;

        if (rx_state == pS_RX_IDLE)
            rx_byte_count <= 0;
        else if (rx_valid_byte)
            rx_byte_count <= rx_byte_count + 1;

        if (rx_state == pS_RX_IDLE)
            rx_len_acquired <= 1'b0;
        else if (rx_byte_count == 3)
            rx_len_acquired <= 1'b1;

        if (rx_state == pS_RX_IDLE)
            rx_crc_error <= 1'b0;
        else if (rx_valid_byte && (rx_byte_count == rx_len + 3) ) begin
            if (rx_data == calculated_crc)
                rx_crc_ok <= 1'b1;
            else
                rx_crc_error <= 1'b1;
        end

        if (rx_state == pS_RX_IDLE) begin
            dut_wrn <= 1'b1;
            dut_rdn <= 1'b1;
            rx_crc_ok <= 1'b0;
        end
        else if (rx_byte_count == 2) begin
            if ( (rx_command == 8'd35) && reading) begin // 'R' for Read
                dut_wrn <= 1'b1;
                dut_rdn <= 1'b0;
            end
            else if ( (rx_command == 8'd35) && writing) begin // 'W' for Write
                dut_wrn <= 1'b0;
                dut_rdn <= 1'b1;
            end
            else begin
                rx_invalid_command <= 1'b1;
                dut_wrn <= 1'b1;
                dut_rdn <= 1'b1;
            end
        end

    end


    always @ (posedge clk) begin
        if (reset)
            rx_state <= pS_RX_IDLE;
        else begin

            case (rx_state)
                pS_RX_IDLE: begin
                    rx_insert_zero <= 1'b0;
                    crc_init <= 1'b1;
                    if (rx_data_ready)
                        rx_state <= pS_RX_NUM_BYTES;
                end

                pS_RX_NUM_BYTES: begin
                    crc_init <= 1'b0;
                    rx_zero_offset <= rx_data;
                    rx_zero_count <= 0;
                    if (rx_len_acquired && (rx_byte_count == rx_len + 4))
                        rx_state <= pS_RX_DELIMITER;
                    else if (rx_data == 1)
                        rx_state <= pS_RX_INSERT_ZERO;
                    else
                        rx_state <= pS_RX_RX;
                end

                pS_RX_RX: begin
                    if (rx_data_ready) begin
                        rx_zero_count <= rx_zero_count + 1;
                        if (rx_byte_count == rx_len + 3)
                            rx_state <= pS_RX_DELIMITER;
                        else if (rx_zero_count+2 == rx_zero_offset)
                            rx_state <= pS_RX_INSERT_ZERO;
                        else
                            rx_state <= pS_RX_RX;
                    end
                end

                pS_RX_INSERT_ZERO: begin
                    rx_insert_zero <= 1'b1;
                    rx_state <= pS_RX_WAIT_NUM_BYTES;
                end

                pS_RX_WAIT_NUM_BYTES: begin
                    rx_insert_zero <= 1'b0;
                    if (rx_data_ready)
                        rx_state <= pS_RX_NUM_BYTES;
                end

                pS_RX_DELIMITER: begin
                    if (rx_data_ready) begin
                        rx_state <= pS_RX_IDLE;
                        if (rx_data != 0)
                            rx_delimiter_error <= 1'b1;
                    end
                end

                pS_RX_DONE: begin
                    rx_state <= pS_RX_IDLE;
                end

            endcase

        end
    end


    /* TODO: Tx
    always @ (posedge clk) begin
        if (reset)
            tx_state <= pS_TX_IDLE;
        else begin

            case (tx_state)
                pS_TX_IDLE: begin
                    tx_byte_count <= 0;
                    crc_init <= 1'b1;
                    if (tx_data_ready)
                        tx_state <= pS_TX_NUM_BYTES;
                end

            endcase

        end
    end
    */

    crc U_crc (
        .clk            (clk),
        .init           (crc_init),
        .data           (rx_data),
        .data_en        (rx_valid_byte),
        .crcOut         (calculated_crc)
    );

    uart_core U_uart (
       .clk                      (clk),
       .reset_n                  (~reset),
       // Configuration inputs
       .bit_rate                 (pBIT_RATE),
       .data_bits                (pDATA_BITS),
       .stop_bits                (pSTOP_BITS),
       .parity_bit               (pPARITY_BITS),
       .parity_enabled           (pPARITY_ENABLED),
       .parity_accept_errors     (1'b0),

       //.bit_rate                 (16'd8),
       //.data_bits                (4'd8),
       //.stop_bits                (2'd1),
       //.parity_bit               (1'b0),
       //.parity_enabled           (1'b0),
       //.parity_accept_errors     (1'b0),
       // External data interface
       .rxd                      (rxd),
       .txd                      (txd),
       // UART Rx
       .rxd_syn                  (rx_syn),
       .rxd_data                 (rx_data_word),
       .rxd_ack                  (rx_ack),
       .rxd_state                (),
       // UART Tx
       .txd_syn                  (tx_syn),
       .txd_data                 (tx_data_byte),
       .txd_ack                  (tx_ready)
    );
    always @(posedge clk) rx_ack <= rx_syn;
    assign rx_data_ready = rx_syn && rx_ack;

    wire fifo_wen;
    wire [7:0] fifo_wdata;
    wire fifo_full;
    wire fifo_almost_full;
    wire fifo_overflow;
    wire fifo_ren;
    wire [7:0] fifo_rdata;
    wire fifo_empty;
    wire fifo_almost_empty;
    wire fifo_underflow;

    /* TODO: Tx
    fifo_sync #(
        .pDATA_WIDTH            (8),
        .pDEPTH                 (250),  // TODO: confirm actual max
        .pFALLTHROUGH           (1),
        .pFLOPS                 (1),
        .pBRAM                  (0),    // TODO: switch to BRAM?
        .pDISTRIBUTED           (0),
    ) U_tx_fifo (
        .clk                    (clk                  ),
        .rst_n                  (~reset               ),
        .full_threshold_value   (                     ),
        .empty_threshold_value  (                     ),
        .wen                    (fifo_wen             ),
        .wdata                  (fifo_wdata           ),
        .full                   (fifo_full            ),
        .almost_full            (fifo_almost_full     ),
        .overflow               (fifo_overflow        ),
        .full_threshold         (                     ),
        .ren                    (fifo_ren             ),
        .rdata                  (fifo_rdata           ),
        .empty                  (fifo_empty           ),
        .almost_empty           (fifo_almost_empty    ),
        .empty_threshold        (                     ),
        .underflow              (fifo_underflow       )
    );
    */

    cw305_top #(
        .pBYTECNT_SIZE                  (8),
        .pADDR_WIDTH                    (32)
    ) U_cw305_dut (
        .usb_clk                        (clk),
        .usb_data                       (dut_data),
        .usb_addr                       (dut_address),
        .usb_rdn                        (dut_rdn ),
        .usb_wrn                        (dut_wrn ),
        .usb_cen                        (dut_cen ),

        .j16_sel                        (1'b0),
        .k16_sel                        (1'b0),
        .k15_sel                        (1'b0),
        .l14_sel                        (1'b0),
        .pushbutton                     (1'b1),

        .led1                           (led1),
        .led2                           (led2),
        .led3                           (led3),

        .pll_clk1                       (clk),
        .tio_trigger                    (),
        .tio_clkout                     (),
        .tio_clkin                      (1'b0)
    );


endmodule
`default_nettype wire
