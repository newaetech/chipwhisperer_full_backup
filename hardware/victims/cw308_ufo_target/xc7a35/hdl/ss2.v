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
    parameter pBIT_RATE = 32, // number of clocks per UART bit; 230400 bps @ 7.37 MHz
    parameter pDATA_BITS = 8,
    parameter pSTOP_BITS = 1,
    parameter pPARITY_BITS = 0,
    parameter pPARITY_ENABLED = 0
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
    //wire io3, io4;

    wire clk_buf;
    wire rx_data_ready;
    wire [8:0] rx_data_word;
    reg  rx_ack;
    wire rx_syn;
    reg  tx_syn;
    reg  [7:0] tx_data_byte;
    wire tx_busy;

    reg [7:0] rx_command;
    reg [7:0] rx_subcommand;
    reg [7:0] rx_len;
    reg rx_len_acquired;
    reg [2:0] rx_state;
    reg [7:0] rx_byte_count;
    reg [7:0] rx_zero_offset;
    reg [7:0] rx_zero_count;
    reg rx_insert_zero;
    reg rx_frame_done;
    reg rx_invalid_command = 1'b0;
    reg rx_crc_error = 1'b0;
    reg rx_delimiter_error = 1'b0;
    wire [7:0] calculated_crc;
    reg rx_crc_init = 1'b0;

    // to help debug:
    // verilator lint_off UNUSED
    reg rx_crc_ok;
    wire fifo_full;
    wire fifo_almost_full;
    wire fifo_almost_empty;
    // verilator lint_on UNUSED

    reg  fifo_wen;
    wire fifo_overflow;
    reg  fifo_ren;
    wire [7:0] fifo_rdata;
    wire fifo_empty;
    wire fifo_underflow;


    reg [31:0] dut_address;
    reg rx_dut_cen;
    reg dut_cen_r;
    reg dut_wrn;
    reg dut_rdn;
    reg [7:0] dut_wdata;
    wire [7:0] dut_data = dut_wrn? 8'bz : dut_wdata;

    reg [2:0] tx_state;
    reg tx_crc_init = 1'b0;
    reg tx_dut_cen;

    reg  error = 1'b0;
    always @ (posedge clk_buf) begin
        if (~resetn)
            error <= 1'b0;
        else if ( rx_invalid_command || 
                  rx_crc_error || 
                  rx_delimiter_error || 
                 (rx_data_ready && rx_insert_zero) ||
                 (fifo_underflow || fifo_overflow) ||
                 ((tx_state == pS_TX_IDLE) && ~fifo_empty) ||
                 fifo_overflow ||
                 fifo_underflow
                )
            error <= 1'b1;
    end

    assign led2 = error;

    wire [7:0] rx_data_byte = rx_data_word[8:1];
    wire [7:0] rx_data = (rx_insert_zero)? 8'b0 : rx_data_byte;

    localparam pS_RX_IDLE = 3'd0;
    localparam pS_RX_NUM_BYTES = 3'd1;
    localparam pS_RX_RX = 3'd2;
    localparam pS_RX_INSERT_ZERO = 3'd3;
    localparam pS_RX_WAIT_NUM_BYTES = 3'd4;
    localparam pS_RX_DELIMITER = 3'd6;

    wire rx_valid_byte = rx_insert_zero || (rx_data_ready && (rx_state != pS_RX_IDLE) && 
                                                             (rx_state != pS_RX_DELIMITER) &&
                                                             (rx_state != pS_RX_WAIT_NUM_BYTES));

    wire writing = (rx_subcommand == 8'h57); // 'W' for Write
    wire reading = (rx_subcommand == 8'h52); // 'R' for Read
    wire echo    = (rx_subcommand == 8'h45); // 'E' for Echo

    reg [7:0] dut_read_len;

    always @ (posedge clk_buf) begin
        dut_cen_r <= dut_cen;
        if (dut_cen && ~dut_cen_r)
            // note this handles incremeting the address for both the write (Rx) logic and read (Tx response) logic:
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
                7: dut_read_len <= rx_data;
            endcase
        end

        if ((rx_byte_count > 6) && (rx_byte_count < rx_len + 3) && writing && rx_valid_byte) begin
            rx_dut_cen <= 1'b0;
            dut_wdata <= rx_data;
        end
        else 
            rx_dut_cen <= 1'b1;


        if (rx_state == pS_RX_IDLE)
            rx_byte_count <= 0;
        else if (rx_valid_byte)
            rx_byte_count <= rx_byte_count + 1;

        if (rx_state == pS_RX_IDLE)
            rx_len_acquired <= 1'b0;
        else if (rx_byte_count == 3)
            rx_len_acquired <= 1'b1;

        if (clear_rx_errors) begin
            rx_crc_error <= 1'b0;
            rx_crc_ok <= 1'b0;
        end
        else if (rx_valid_byte && (rx_byte_count == rx_len + 3) ) begin
            if (rx_data == calculated_crc)
                rx_crc_ok <= 1'b1;
            else
                rx_crc_error <= 1'b1;
        end

        if (rx_state == pS_RX_IDLE)
            dut_wrn <= 1'b1;
        else if (rx_byte_count == 2) begin
            if ( (rx_command == 8'h23) && (reading || echo))
                dut_wrn <= 1'b1;
            else if ( (rx_command == 8'h23) && writing)
                dut_wrn <= 1'b0;
            else
                dut_wrn <= 1'b1;
        end

        if (clear_rx_errors)
            rx_invalid_command <= 1'b0;
        else if ( (rx_byte_count == 2) && ~((rx_command == 8'h23) && (reading || writing || echo)) )
            rx_invalid_command <= 1'b1;

    end

    // Rx FSM: strategy is that FSM deals with COBS, not content
    always @ (posedge clk_buf) begin
        if (~resetn)
            rx_state <= pS_RX_IDLE;
        else begin

            case (rx_state)
                pS_RX_IDLE: begin
                    rx_insert_zero <= 1'b0;
                    rx_frame_done <= 1'b0;
                    rx_crc_init <= 1'b1;
                    rx_delimiter_error <= 1'b0;
                    if (rx_data_ready && (rx_data_byte != 0)) // ignore extra frame delimiters
                        rx_state <= pS_RX_NUM_BYTES;
                end

                pS_RX_NUM_BYTES: begin
                    rx_crc_init <= 1'b0;
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
                        else
                            rx_frame_done <= 1'b1;
                    end
                end

            endcase

        end
    end


////////////////////////
//   Transmit Logic   //
////////////////////////

    localparam pS_TX_IDLE = 3'd0;
    localparam pS_TX_0WAIT = 3'd1;
    localparam pS_TX_WAIT_READY = 3'd2;
    localparam pS_TX_EMPTY_FIFO = 3'd3;
    localparam pS_TX_DONE = 3'd4;

    reg tx_next_byte;
    reg [3:0] tx_next_byte_pipe;
    reg [7:0] tx_cobs_counter;
    reg [7:0] tx_raw_index;
    reg [7:0] tx_raw_byte = 8'hFF; // to make sure we don't flag a delimiter byte right when we start
    reg tx_read_response;
    reg tx_write_response;
    reg tx_echo_response;
    reg [7:0] tx_len;
    reg tx_next_byte_r;
    reg [7:0] dut_data_r;
    reg dut_register_read_pre;
    reg dut_register_read;

    always @ (posedge clk_buf) begin
        tx_next_byte_r <= tx_next_byte;
        if (dut_register_read) dut_data_r <= dut_data;
        dut_register_read_pre <= ~dut_rdn && ~tx_dut_cen;
        dut_register_read <= dut_register_read_pre;
        tx_next_byte_pipe <= {tx_next_byte_pipe[2:0], tx_next_byte};
        if (tx_next_byte)
            tx_raw_index <= tx_raw_index + 1;
        else if (tx_state == pS_TX_IDLE)
            tx_raw_index <= 0;

        if (tx_read_response && (tx_raw_index > 0) && (tx_raw_index < dut_read_len + 1) && tx_next_byte) begin
            dut_rdn <= 1'b0;
            tx_dut_cen <= 1'b0;
        end
        else begin
            dut_rdn <= 1'b1;
            tx_dut_cen <= 1'b1;
        end

        if (tx_write_response) begin
            case (tx_raw_index)
                0: tx_raw_byte <= 8'h65; // 'e'
                1: tx_raw_byte <= 8'h01; // constant
                2: tx_raw_byte <= (~error)?             8'h00 :
                                  (rx_invalid_command)? 8'h01 :
                                  (rx_crc_error)?       8'h02 :
                                                        8'h05; // default to unexpected frame byte error
                3: tx_raw_byte <= calculated_crc;
                4: tx_raw_byte <= 8'h00; // frame delimiter
                5: tx_raw_byte <= 8'hFF; // frame delimiter
            endcase
            tx_len <= 5;
        end

        else if (tx_read_response) begin
            if      (tx_raw_index == 0)                tx_raw_byte <= rx_command;
            else if (tx_raw_index == 1)                tx_raw_byte <= dut_read_len;
            else if (tx_raw_index <  dut_read_len + 2) tx_raw_byte <= dut_data_r;
            else if (tx_raw_index == dut_read_len + 2) tx_raw_byte <= calculated_crc;
            else if (tx_raw_index == dut_read_len + 3) tx_raw_byte <= 8'h00; // frame delimiter
            else if (tx_raw_index == dut_read_len + 4) tx_raw_byte <= 8'hFF; // to prevent error on next response
            tx_len <= dut_read_len + 4;
        end

        else if (tx_echo_response) begin
            case (tx_raw_index)
                0: tx_raw_byte <= rx_command;
                1: tx_raw_byte <= rx_len;
                2: tx_raw_byte <= dut_address[7:0];
                3: tx_raw_byte <= dut_address[15:8];
                4: tx_raw_byte <= dut_address[23:16];
                5: tx_raw_byte <= dut_address[31:24];
                6: tx_raw_byte <= calculated_crc;
                7: tx_raw_byte <= 8'h00; // frame delimiter
                8: tx_raw_byte <= 8'hFF; // to prevent error on next respponse
            endcase
            tx_len <= 8;
        end

    end

    wire tx_ready = ~tx_syn && ~tx_busy;
    reg status_needed;
    reg clear_rx_errors = 1'b0;

    // Tx FSM: strategy is that FSM deals with COBS, not content
    always @ (posedge clk_buf) begin
        if (~resetn)
            tx_state <= pS_TX_IDLE;
        else begin

            case (tx_state)
                pS_TX_IDLE: begin
                    tx_read_response <= 1'b0;
                    tx_write_response <= 1'b0;
                    tx_echo_response <= 1'b0;
                    tx_next_byte <= 1'b0;
                    tx_cobs_counter <= 1;
                    fifo_wen <= 1'b0;
                    fifo_ren <= 1'b0;
                    tx_syn <= 1'b0;
                    tx_crc_init <= 1'b1;
                    status_needed <= 1'b0;
                    clear_rx_errors <= 1'b0;
                    if (rx_frame_done || status_needed) begin
                        // NOTE: we're assuming that another Rx frame can't come in before we've 
                        // finished replying to the previous one. Otherwise now would be a good time
                        // to copy over any parameters from the Rx frame we're responding to.
                        if (writing || status_needed || rx_invalid_command)
                            tx_write_response <= 1'b1;
                        else if (reading) 
                            tx_read_response <= 1'b1;
                        else if (echo)
                            tx_echo_response <= 1'b1;
                        tx_next_byte <= 1'b1;
                        tx_crc_init <= 1'b0;
                        tx_state <= pS_TX_0WAIT;
                    end
                end

                pS_TX_0WAIT: begin
                    fifo_ren <= 1'b0;
                    if (tx_next_byte && (tx_raw_byte == 8'd0)) begin
                        fifo_wen <= 1'b0;
                        tx_next_byte <= 1'b0;
                        //tx_syn <= 1'b1;
                        tx_data_byte <= tx_cobs_counter;
                        tx_state <= pS_TX_WAIT_READY;
                    end
                    else begin
                        fifo_wen <= tx_next_byte;
                        //tx_next_byte <= ~tx_next_byte; // read every other cycle so we can react to a 8'd0 byte in time
                        // We *could* assert tx_next_byte every other cycle, but it's easier to slow it
                        // down to deal with the high-latency register reads:
                        tx_next_byte <= tx_next_byte_pipe[3];
                        if (tx_next_byte)
                            tx_cobs_counter <= tx_cobs_counter + 1;
                    end
                end

                pS_TX_WAIT_READY: begin
                    if (tx_ready) begin
                        tx_syn <= 1'b1;
                        tx_state <= pS_TX_EMPTY_FIFO;
                    end
                end

                pS_TX_EMPTY_FIFO: begin
                    if (fifo_empty) begin
                        fifo_ren <= 1'b0;
                        tx_syn <= 1'b0;
                        tx_next_byte <= 1'b1;
                        tx_cobs_counter <= 1;
                        if (tx_raw_index == tx_len)
                            tx_state <= pS_TX_DONE;
                        else
                            tx_state <= pS_TX_0WAIT;
                    end
                    else if (tx_ready && ~fifo_ren) begin
                        fifo_ren <= 1'b1;
                        tx_syn <= 1'b1;
                        tx_data_byte <= fifo_rdata;
                    end
                    else begin
                        tx_syn <= 1'b0;
                        fifo_ren <= 1'b0;
                    end
                end

                pS_TX_DONE: begin
                    fifo_ren <= 1'b0;
                    tx_next_byte <= 1'b0;
                    if (tx_ready) begin
                        tx_syn <= 1'b1;
                        tx_data_byte <= 8'd0; // frame delimiter
                        if (tx_read_response || tx_echo_response) begin
                            // if we responded with something other than an error packet, now we need to provide the error packet:
                            tx_crc_init <= 1'b1;
                            status_needed <= 1'b1;
                        end
                        else
                            clear_rx_errors <= 1'b1;
                        tx_state <= pS_TX_IDLE;
                    end
                end

            endcase

        end
    end


    wire rx_active = (rx_state != pS_RX_IDLE);
    wire [7:0] crc_data = (rx_active)? rx_data : tx_raw_byte;
    wire crc_en = (rx_active)? rx_valid_byte : tx_next_byte_r;
    wire crc_init = (rx_active)? rx_crc_init : tx_crc_init;
    crc U_crc (
        .clk            (clk_buf),
        .init           (crc_init),
        .data           (crc_data),
        .data_en        (crc_en),
        .crcOut         (calculated_crc)
    );

    uart_core U_uart (
       .clk                      (clk_buf),
       .reset_n                  (resetn),
       // Configuration inputs
       .bit_rate                 (pBIT_RATE),
       .data_bits                (pDATA_BITS),
       .stop_bits                (pSTOP_BITS),
       .parity_bit               (pPARITY_BITS),
       .parity_enabled           (pPARITY_ENABLED),
       .parity_accept_errors     (1'b0),
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
       .txd_busy                 (tx_busy)
    );
    always @(posedge clk_buf) rx_ack <= rx_syn;
    assign rx_data_ready = rx_syn && rx_ack;

    fifo_sync #(
        .pDATA_WIDTH            (8),
        .pDEPTH                 (256),
        .pFALLTHROUGH           (1),
        .pFLOPS                 (1),
        .pBRAM                  (0),    // NOTE: consider switching to BRAM for implementation?
        .pDISTRIBUTED           (0)
    ) U_tx_fifo (
        .clk                    (clk_buf              ),
        .rst_n                  (resetn               ),
        .full_threshold_value   (                     ),
        .empty_threshold_value  (                     ),
        .wen                    (fifo_wen             ),
        .wdata                  (tx_raw_byte          ),
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


    wire dut_cen = (reading)? tx_dut_cen : rx_dut_cen;
    cw305_top #(
        .pBYTECNT_SIZE                  (8),
        .pADDR_WIDTH                    (32)
    ) U_cw305_dut (
        .usb_clk                        (clk),
        .usb_clk_buf                    (clk_buf),
        .usb_data                       (dut_data),
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

    `ifdef SS2_EXAMPLE
        .io3                            (io3),
        .ss2_error                      (error),
    `endif

        .pll_clk1                       (clk_buf),
        .tio_trigger                    (io4),
        .tio_clkout                     (clkout),
        .tio_clkin                      (clk_buf)
    );

    `ifndef SS2_EXAMPLE
        assign io3 = 1'bz;
    `endif


endmodule
`default_nettype wire
