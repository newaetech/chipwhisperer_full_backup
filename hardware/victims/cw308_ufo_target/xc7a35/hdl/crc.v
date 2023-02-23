`timescale 1 ns / 1 ps
`default_nettype none

// THIS IS GENERATED VERILOG CODE.
// https://bues.ch/h/crcgen
// 
// This code is Public Domain.
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
// RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
// NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
// USE OR PERFORMANCE OF THIS SOFTWARE.

// CRC polynomial coefficients: x^8 + x^6 + x^3 + x^2 + 1
//                              0x4D (hex)
// CRC width:                   8 bits
// CRC shift direction:         left (big endian)
// Input word width:            8 bits

module crc (
    input wire clk,
    input wire init,
    input wire [7:0] data,
    input wire data_en,
    output reg [7:0] crcOut
);

always @(posedge clk) begin
    if (init)
        crcOut <= 8'b0;
    else if (data_en) begin
        crcOut[0] <= (crcOut[0] ^ crcOut[2] ^ crcOut[4] ^ crcOut[5] ^ data[0] ^ data[2] ^ data[4] ^ data[5]);
        crcOut[1] <= (crcOut[1] ^ crcOut[3] ^ crcOut[5] ^ crcOut[6] ^ data[1] ^ data[3] ^ data[5] ^ data[6]);
        crcOut[2] <= (crcOut[0] ^ crcOut[5] ^ crcOut[6] ^ crcOut[7] ^ data[0] ^ data[5] ^ data[6] ^ data[7]);
        crcOut[3] <= (crcOut[0] ^ crcOut[1] ^ crcOut[2] ^ crcOut[4] ^ crcOut[5] ^ crcOut[6] ^ crcOut[7] ^ data[0] ^ data[1] ^ data[2] ^ data[4] ^ data[5] ^ data[6] ^ data[7]);
        crcOut[4] <= (crcOut[1] ^ crcOut[2] ^ crcOut[3] ^ crcOut[5] ^ crcOut[6] ^ crcOut[7] ^ data[1] ^ data[2] ^ data[3] ^ data[5] ^ data[6] ^ data[7]);
        crcOut[5] <= (crcOut[2] ^ crcOut[3] ^ crcOut[4] ^ crcOut[6] ^ crcOut[7] ^ data[2] ^ data[3] ^ data[4] ^ data[6] ^ data[7]);
        crcOut[6] <= (crcOut[0] ^ crcOut[2] ^ crcOut[3] ^ crcOut[7] ^ data[0] ^ data[2] ^ data[3] ^ data[7]);
        crcOut[7] <= (crcOut[1] ^ crcOut[3] ^ crcOut[4] ^ data[1] ^ data[3] ^ data[4]);
    end
end


endmodule

`default_nettype wire
