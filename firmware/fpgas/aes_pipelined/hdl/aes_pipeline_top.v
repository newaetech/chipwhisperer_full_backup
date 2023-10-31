`default_nettype none
`timescale 1ns / 1ps

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

module aes_pipeline_top #(
    parameter pROUNDS = 10,
    parameter pFIFO_DEPTH = 512
)(
    input wire clk,
    input wire usb_clk,
    input wire reset,
    input wire encrypt_go,
    input wire write_data,
    input wire read_data,
    input wire load_key,
    input wire [255:0] key_i,
    input wire [127:0] data_i,
    output wire [127:0] data_o,
    output wire busy_o,
    output wire fifo_out_empty,
    input  wire clear_fifo_errors,
    output wire [3:0] fifo_errors
);

reg [127:0] ks;
reg ks_en = 1'b0;
wire [127:0] ks_val;

reg crypt_start [0:pROUNDS];
reg round_key_saved [0:pROUNDS];
reg  [127:0] round_key_val [0:pROUNDS];
wire [127:0] round_key [0:pROUNDS];
wire [127:0] round_in  [0:pROUNDS];
wire [127:0] round_out [0:pROUNDS];

wire fifo_in_wr;
wire [127:0] fifo_in_din;
wire fifo_in_full;
wire fifo_in_overflow;
reg  fifo_in_rd = 1'b0;
wire [127:0] fifo_in_dout;
wire fifo_in_empty;
wire fifo_in_almost_empty;
wire fifo_in_underflow;

reg  fifo_out_wr;
wire [127:0] fifo_out_din;
wire fifo_out_full;
wire fifo_out_overflow;
wire fifo_out_rd = read_data;
wire [127:0] fifo_out_dout;
wire fifo_out_underflow;


//////////////////////
// key expansion:

wire [1:0] ks_size = (pROUNDS == 10)? 0 :
                     (pROUNDS == 12)? 1 :
                     (pROUNDS == 14)? 2 : 0;

reg load_key_r;
//reg [pROUNDS-1:0] load_key_pipe;
reg  load_key_pipe [0:pROUNDS];
always @(posedge clk) load_key_r <= load_key;

aes_ks ks_inst (
    .clk        (clk),
    .load_i     (load_key_r),
    .en_i       (ks_en),
    .size_i     (ks_size),
    .key_i      (key_i),
    .ks_o       (ks_val)
);

genvar j;
generate
    for (j = 0; j <= pROUNDS; j = j + 1) begin: gen_keys
        always @(posedge clk) begin
            if (j == 0)
                load_key_pipe[j] <= load_key_r;
            else
                load_key_pipe[j] <= load_key_pipe[j-1];

            if (load_key)
                round_key_saved[j] <= 1'b0;
            //else if (crypt_start[j] && ~round_key_saved[j]) begin
            else if (load_key_pipe[j] && ~round_key_saved[j]) begin
                round_key_saved[j] <= 1'b1;
                round_key_val[j] <= ks_val;
            end
        end
        assign round_key[j] = round_key_saved[j]? round_key_val[j] : ks_val;
    end
endgenerate

always @(posedge clk) begin
    if (load_key)
        ks_en <= 1'b1;
    else if (round_key_saved[pROUNDS-2])
        ks_en <= 1'b0;
end



//////////////////////
// pipeline + control:

assign round_in[0] = fifo_in_dout;

always @(posedge clk) begin
    if (fifo_in_almost_empty && fifo_in_rd)
        fifo_in_rd <= 1'b0;
    else if (encrypt_go)
        fifo_in_rd <= 1'b1;
    if (fifo_in_rd)
        crypt_start[0] <= 1'b1;
    else
        crypt_start[0] <= 1'b0;
end

genvar i;
generate
    for (i = 0; i <= pROUNDS; i = i + 1) begin: gen_rounds

        if (i > 0) begin
            assign round_in[i] = round_out[i-1];
            always @(posedge clk) crypt_start[i] <= crypt_start[i-1];
        end

        aes_round U_aes_round (
            .clk        (clk),
            .round      (i),
            .round_max  (4'd10),
            .load_i     (crypt_start[i]),
            .key_i      (round_key[i]),
            .data_i     (round_in[i]),
            .data_o     (round_out[i])
        );

    end
endgenerate

// ugh Verilog sucks here...
assign busy_o = crypt_start[0] ||
                crypt_start[1] ||
                crypt_start[2] ||
                crypt_start[3] ||
                crypt_start[4] ||
                crypt_start[5] ||
                crypt_start[6] ||
                crypt_start[7] ||
                crypt_start[8] ||
                crypt_start[9] ||
                crypt_start[10];


/////////////////
// input FIFO:

assign fifo_in_din = data_i;
assign fifo_in_wr = write_data;

// TODO: fifo_sync option for A35 target
fifo_async #(
    .pDATA_WIDTH                (128),
    .pDEPTH                     (pFIFO_DEPTH),
    .pFALLTHROUGH               (0),
    .pFLOPS                     (1),
    .pDISTRIBUTED               (0),
    .pBRAM                      (0)
) U_fifo_in (
    .wclk                       (usb_clk),
    .rclk                       (clk),
    .wrst_n                     (~reset),
    .rrst_n                     (~reset),
    .wfull_threshold_value      (0),
    .rempty_threshold_value     (0),
    .wen                        (fifo_in_wr),
    .wdata                      (fifo_in_din),
    .wfull                      (fifo_in_full),
    .woverflow                  (fifo_in_overflow),
    .wfull_threshold            (),
    .rempty_threshold           (),
    .ren                        (fifo_in_rd),
    .rdata                      (fifo_in_dout),
    .rempty                     (fifo_in_empty),
    .ralmost_empty              (fifo_in_almost_empty),
    .walmost_full               (),
    .runderflow                 (fifo_in_underflow)
);


/////////////////
// output FIFO:

assign data_o = fifo_out_dout;
assign fifo_out_din = round_out[pROUNDS];

always @(posedge clk) begin
    if (crypt_start[pROUNDS])
        fifo_out_wr <= 1'b1;
    else
        fifo_out_wr <= 1'b0;
end

// TODO: fifo_sync option for A35 target
fifo_async #(
    .pDATA_WIDTH                (128),
    .pDEPTH                     (pFIFO_DEPTH),
    .pFALLTHROUGH               (0),
    .pFLOPS                     (1),
    .pDISTRIBUTED               (0),
    .pBRAM                      (0)
) U_fifo_out (
    .wclk                       (clk),
    .rclk                       (usb_clk),
    .wrst_n                     (~reset),
    .rrst_n                     (~reset),
    .wfull_threshold_value      (0),
    .rempty_threshold_value     (0),
    .wen                        (fifo_out_wr),
    .wdata                      (fifo_out_din),
    .wfull                      (fifo_out_full),
    .woverflow                  (fifo_out_overflow),
    .wfull_threshold            (),
    .rempty_threshold           (),
    .ren                        (fifo_out_rd),
    .rdata                      (fifo_out_dout),
    .rempty                      (fifo_out_empty),
    .ralmost_empty              (),
    .walmost_full               (),
    .runderflow                 (fifo_out_underflow)
);

/////////////////
// FIFO errors:
reg fifo_out_underflow_reg = 1'b0;
reg fifo_in_overflow_reg = 1'b0;
reg fifo_in_underflow_reg = 1'b0;
reg fifo_out_overflow_reg = 1'b0;

always @(posedge usb_clk) begin
    if (clear_fifo_errors)
        fifo_out_underflow_reg <= 1'b0;
    else if (fifo_out_underflow)
        fifo_out_underflow_reg <= 1'b1;

    if (clear_fifo_errors)
        fifo_in_overflow_reg <= 1'b0;
    else if (fifo_in_overflow)
        fifo_in_overflow_reg <= 1'b1;
end

always @(posedge clk) begin
    if (clear_fifo_errors)
        fifo_in_underflow_reg <= 1'b0;
    else if (fifo_in_underflow)
        fifo_in_underflow_reg <= 1'b1;

    if (clear_fifo_errors)
        fifo_out_overflow_reg <= 1'b0;
    else if (fifo_out_overflow)
        fifo_out_overflow_reg <= 1'b1;
end

assign fifo_errors = {fifo_in_overflow_reg,
                      fifo_in_underflow_reg,
                      fifo_out_underflow_reg,
                      fifo_out_overflow_reg};


/////////////////
// debug only:

wire [127:0] round_out0 = round_out[0];
wire [127:0] round_out1 = round_out[1];
wire [127:0] round_out2 = round_out[2];
wire [127:0] round_out3 = round_out[3];
wire [127:0] round_out4 = round_out[4];
wire [127:0] round_out5 = round_out[5];
wire [127:0] round_out6 = round_out[6];
wire [127:0] round_out7 = round_out[7];
wire [127:0] round_out8 = round_out[8];
wire [127:0] round_out9 = round_out[9];
wire [127:0] round_out10 = round_out[10];

wire [127:0] round_key0 = round_key[0];
wire [127:0] round_key1 = round_key[1];
wire [127:0] round_key2 = round_key[2];
wire [127:0] round_key3 = round_key[3];
wire [127:0] round_key4 = round_key[4];
wire [127:0] round_key5 = round_key[5];
wire [127:0] round_key6 = round_key[6];
wire [127:0] round_key7 = round_key[7];
wire [127:0] round_key8 = round_key[8];
wire [127:0] round_key9 = round_key[9];
wire [127:0] round_key10 = round_key[10];

wire round_key_saved0 = round_key_saved[0];
wire round_key_saved1 = round_key_saved[1];

wire [127:0] round_key_val0 = round_key_val[0];
wire [127:0] round_key_val1 = round_key_val[1];

wire crypt_start0 = crypt_start[0];
wire crypt_start1 = crypt_start[1];
wire crypt_start2 = crypt_start[2];
wire crypt_start3 = crypt_start[3];
wire crypt_start4 = crypt_start[4];
wire crypt_start5 = crypt_start[5];
wire crypt_start6 = crypt_start[6];
wire crypt_start7 = crypt_start[7];
wire crypt_start8 = crypt_start[8];
wire crypt_start9 = crypt_start[9];
wire crypt_start10 = crypt_start[10];


endmodule
`default_nettype wire
