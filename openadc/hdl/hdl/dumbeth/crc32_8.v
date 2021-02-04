//////////////////////////////////////////////////////////////////////////////
//
// crc calculation
// This VERILOG code was generated using CRCGEN.PL version 1.7
// Last Modified: 01/02/2002
// Options Used:
//    Module Name = crc32
//      CRC Width = 32
//     Data Width = 8
//     CRC Init   = F
//     Polynomial = [0 -> 32]
//        1 1 1 0 1 1 0 1 1 0 1 1 1 0 0 0 1 0 0 0 0 0 1 1 0 0 1 0 0 0 0 0 1
//
// Disclaimer: THESE DESIGNS ARE PROVIDED "AS IS" WITH NO WARRANTY 
//             WHATSOEVER AND XILINX SPECIFICALLY DISCLAIMS ANY 
//             IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
//             A PARTICULAR PURPOSE, OR AGAINST INFRINGEMENT.
//
// Copyright (c) 2001,2002 Xilinx, Inc.  All rights reserved.
//
//
//////////////////////////////////////////////////////////////////////////////

module crc32 (
   crc_reg, 
   crc,
   d,
   calc,
   init,
   d_valid,
   clk,
   reset
   );

output [31:0] crc_reg;
output [7:0]  crc;

input  [7:0]  d;
input         calc;
input         init;
input         d_valid;
input         clk;
input         reset;

reg    [31:0] crc_reg;
reg    [7:0]  crc;

//////////////////////////////////////////////////////////////////////////////
// Internal Signals
//////////////////////////////////////////////////////////////////////////////
wire   [31:0] next_crc;

//////////////////////////////////////////////////////////////////////////////
// Infer CRC-32 registers
// 
// The crc_reg register stores the CRC-32 value.
// The crc register is the most significant 8 bits of the 
// CRC-32 value.
//
// Truth Table:
// -----+---------+----------+----------------------------------------------
// calc | d_valid | crc_reg  | crc 
// -----+---------+----------+----------------------------------------------
//  0   |     0   | crc_reg  | crc 
//  0   |     1   |  shift   | bit-swapped, complimented msbyte of crc_reg
//  1   |     0   | crc_reg  | crc 
//  1   |     1   | next_crc | bit-swapped, complimented msbyte of next_crc
// -----+---------+----------+----------------------------------------------
// 
//////////////////////////////////////////////////////////////////////////////
 
always @ (posedge clk or posedge reset)
begin
   if (reset) begin
      crc_reg <= 32'hFFFFFFFF;
      crc     <= 8'hFF;
   end
   
   else if (init) begin
      crc_reg <= 32'hFFFFFFFF;
      crc     <=  8'hFF;
   end

   else if (calc & d_valid) begin
      crc_reg <= next_crc;
      crc     <= ~{next_crc[24], next_crc[25], next_crc[26], next_crc[27],
                   next_crc[28], next_crc[29], next_crc[30], next_crc[31]};
   end
   
   else if (~calc & d_valid) begin
      crc_reg <=  {crc_reg[23:0], 8'hFF};
      crc     <= ~{crc_reg[16], crc_reg[17], crc_reg[18], crc_reg[19],
                   crc_reg[20], crc_reg[21], crc_reg[22], crc_reg[23]};
   end
end

//////////////////////////////////////////////////////////////////////////////
// CRC XOR equations
//////////////////////////////////////////////////////////////////////////////

assign next_crc[0] = crc_reg[30] ^ d[1] ^ crc_reg[24] ^ d[7];
assign next_crc[1] = d[6] ^ d[7] ^ d[0] ^ crc_reg[30] ^ crc_reg[31] ^ d[1] ^ crc_reg[24] ^ crc_reg[25];
assign next_crc[2] = crc_reg[26] ^ d[5] ^ d[6] ^ d[7] ^ crc_reg[30] ^ d[0] ^ d[1] ^ crc_reg[31] ^ crc_reg[24] ^ crc_reg[25];
assign next_crc[3] = d[4] ^ crc_reg[26] ^ d[5] ^ crc_reg[27] ^ d[6] ^ d[0] ^ crc_reg[31] ^ crc_reg[25];
assign next_crc[4] = d[4] ^ crc_reg[26] ^ d[5] ^ crc_reg[27] ^ crc_reg[28] ^ d[7] ^ crc_reg[30] ^ d[1] ^ crc_reg[24] ^ d[3];
assign next_crc[5] = d[4] ^ crc_reg[27] ^ d[6] ^ crc_reg[28] ^ d[7] ^ crc_reg[29] ^ crc_reg[30] ^ d[0] ^ d[1] ^ crc_reg[31] ^ d[2] ^ crc_reg[24] ^ d[3] ^ crc_reg[25];
assign next_crc[6] = crc_reg[26] ^ d[5] ^ d[6] ^ crc_reg[28] ^ crc_reg[29] ^ d[0] ^ crc_reg[30] ^ crc_reg[31] ^ d[1] ^ d[2] ^ d[3] ^ crc_reg[25];
assign next_crc[7] = d[4] ^ crc_reg[26] ^ d[5] ^ crc_reg[27] ^ d[7] ^ crc_reg[29] ^ d[0] ^ crc_reg[31] ^ d[2] ^ crc_reg[24];
assign next_crc[8] = d[4] ^ crc_reg[27] ^ d[6] ^ crc_reg[28] ^ d[7] ^ crc_reg[24] ^ crc_reg[0] ^ d[3] ^ crc_reg[25];
assign next_crc[9] = crc_reg[26] ^ d[5] ^ d[6] ^ crc_reg[28] ^ crc_reg[29] ^ d[2] ^ d[3] ^ crc_reg[25] ^ crc_reg[1];
assign next_crc[10] = d[4] ^ crc_reg[26] ^ crc_reg[2] ^ d[5] ^ crc_reg[27] ^ d[7] ^ crc_reg[29] ^ d[2] ^ crc_reg[24];
assign next_crc[11] = d[4] ^ crc_reg[27] ^ d[6] ^ crc_reg[3] ^ crc_reg[28] ^ d[7] ^ crc_reg[24] ^ d[3] ^ crc_reg[25];
assign next_crc[12] = crc_reg[26] ^ d[5] ^ d[6] ^ crc_reg[28] ^ d[7] ^ crc_reg[4] ^ crc_reg[29] ^ crc_reg[30] ^ d[1] ^ d[2] ^ crc_reg[24] ^ d[3] ^ crc_reg[25];
assign next_crc[13] = d[4] ^ crc_reg[26] ^ d[5] ^ crc_reg[27] ^ d[6] ^ crc_reg[29] ^ d[0] ^ crc_reg[30] ^ crc_reg[5] ^ crc_reg[31] ^ d[1] ^ d[2] ^ crc_reg[25];
assign next_crc[14] = d[4] ^ crc_reg[26] ^ d[5] ^ crc_reg[27] ^ crc_reg[28] ^ crc_reg[30] ^ d[0] ^ d[1] ^ crc_reg[31] ^ crc_reg[6] ^ d[3];
assign next_crc[15] = d[4] ^ crc_reg[27] ^ crc_reg[28] ^ crc_reg[29] ^ d[0] ^ crc_reg[31] ^ d[2] ^ crc_reg[7] ^ d[3];
assign next_crc[16] = crc_reg[28] ^ d[7] ^ crc_reg[29] ^ d[2] ^ crc_reg[24] ^ d[3] ^ crc_reg[8];
assign next_crc[17] = crc_reg[9] ^ d[6] ^ crc_reg[29] ^ crc_reg[30] ^ d[1] ^ d[2] ^ crc_reg[25];
assign next_crc[18] = crc_reg[26] ^ d[5] ^ crc_reg[10] ^ crc_reg[30] ^ d[0] ^ d[1] ^ crc_reg[31];
assign next_crc[19] = d[4] ^ crc_reg[27] ^ crc_reg[11] ^ d[0] ^ crc_reg[31];
assign next_crc[20] = crc_reg[28] ^ crc_reg[12] ^ d[3];
assign next_crc[21] = crc_reg[29] ^ crc_reg[13] ^ d[2];
assign next_crc[22] = d[7] ^ crc_reg[14] ^ crc_reg[24];
assign next_crc[23] = d[6] ^ d[7] ^ crc_reg[30] ^ d[1] ^ crc_reg[15] ^ crc_reg[24] ^ crc_reg[25];
assign next_crc[24] = crc_reg[26] ^ d[5] ^ d[6] ^ d[0] ^ crc_reg[31] ^ crc_reg[16] ^ crc_reg[25];
assign next_crc[25] = d[4] ^ crc_reg[17] ^ crc_reg[26] ^ d[5] ^ crc_reg[27];
assign next_crc[26] = d[4] ^ crc_reg[18] ^ crc_reg[27] ^ crc_reg[28] ^ d[7] ^ crc_reg[30] ^ d[1] ^ crc_reg[24] ^ d[3];
assign next_crc[27] = d[6] ^ crc_reg[19] ^ crc_reg[28] ^ crc_reg[29] ^ d[0] ^ crc_reg[31] ^ d[2] ^ d[3] ^ crc_reg[25];
assign next_crc[28] = crc_reg[26] ^ d[5] ^ crc_reg[20] ^ crc_reg[29] ^ crc_reg[30] ^ d[1] ^ d[2];
assign next_crc[29] = d[4] ^ crc_reg[27] ^ crc_reg[21] ^ crc_reg[30] ^ d[0] ^ d[1] ^ crc_reg[31];
assign next_crc[30] = crc_reg[28] ^ d[0] ^ crc_reg[22] ^ crc_reg[31] ^ d[3];
assign next_crc[31] = crc_reg[29] ^ crc_reg[23] ^ d[2];
endmodule
