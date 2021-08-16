/*-------------------------------------------------------------------------
 Synchronous FIFO, 8-bit, 31 depth
 
 File name   : syncfifo_8x31.v
 Version     : 1.0
 Created     : APR/02/2012
 Last update : APR/11/2012
 Desgined by : Toshihiro Katashita

 
 Copyright (C) 2012 AIST
 
 By using this code, you agree to the following terms and conditions.

 This code is copyrighted by AIST ("us").

 Permission is hereby granted to copy, reproduce, redistribute or
 otherwise use this code as long as: there is no monetary profit gained
 specifically from the use or reproduction of this code, it is not sold,
 rented, traded or otherwise marketed, and this copyright notice is
 included prominently in any copy made.

 We shall not be liable for any damages, including without limitation
 direct, indirect, incidental, special or consequential damages arising
 from the use of this code.

 When you publish any results arising from the use of this code, we will
 appreciate it if you can cite our webpage.
 (http://www.aist.go.jp/aist_e/research_results/publications/synthesiology_e/vol3_no1/vol03_01_p86_p95.pdf)
 -------------------------------------------------------------------------*/ 


//================================================ SYNCFIFO_8x31
module SYNCFIFO_8x31
  (wd, we, ful, aful, // Write port
   rd, re, emp, aemp, // Read port
   cnt, clk, rst);    // Clock, reset, and data count
   parameter WIDTH = 8;
   parameter DEPTH = 5;
   parameter MEM_DEPTH = (1<<DEPTH)-1;

   //------------------------------------------------
   // Write port
   input [WIDTH-1:0]  wd;        // Write data
   input              we;        // Write enable
   output             ful, aful; // FIFO full / almost full

   // Read port
   output [WIDTH-1:0] rd;        // Read data
   input              re;        // Read enable
   output             emp, aemp; // FIFO empty / almost empty
   output [DEPTH-1:0] cnt;       // Data count

   input              clk, rst;  // Clock and reset

   //------------------------------------------------
   wire               wr_enable;
   reg [DEPTH-1:0]    wa;
   //
   wire               rd_enable;
   reg [DEPTH-1:0]    ra;
   //
   wire               cnt_fwd, cnt_back;
   reg [DEPTH-1:0]    cnt;
   wire               async_ful, async_aful, async_bful;
   wire               async_emp, async_aemp, async_bemp;
   reg                ful, aful;
   reg                emp, aemp;

   //------------------------------------------------
   reg [WIDTH-1:0]    mem [0:MEM_DEPTH];
   reg [DEPTH-1:0]    ra1;

   //------------------------------------------------
   // write address
   assign wr_enable = we & ~ful;

   always @(posedge clk or posedge rst)
     if (rst)            wa <= 0;
     else if (wr_enable) wa <= wa + 1;

   // read address
   assign rd_enable = re & ~emp;

   always @(posedge clk or posedge rst)
     if (rst)            ra <= 0;
     else if (rd_enable) ra <= ra + 1;

   // data count
   assign cnt_fwd  = wr_enable & ~rd_enable;
   assign cnt_back = ~wr_enable & rd_enable;

   always @(posedge clk or posedge rst)
     if (rst)           cnt <= 0;
     else if (cnt_fwd)  cnt <= cnt + 1;
     else if (cnt_back) cnt <= cnt - 1;

   //------------------------------------------------
   // full, almost_full
   assign async_ful  = (cnt==MEM_DEPTH);
   assign async_aful = (cnt==MEM_DEPTH-1);
   assign async_bful = (cnt==MEM_DEPTH-2);

   always @(posedge clk or posedge rst)
     if (rst)                         ful <= 1;
     else if (async_ful & ~rd_enable) ful <= 1;
     else if (async_aful & cnt_fwd)   ful <= 1;
     else                             ful <= 0;

   always @(posedge clk or posedge rst)
     if (rst)                         aful <= 1;
     else if (async_ful)              aful <= 1;
     else if (async_aful & ~cnt_back) aful <= 1;
     else if (async_bful & cnt_fwd)   aful <= 1;
     else                             aful <= 0;

   //------------------------------------------------
   // empty, almost_empty
   assign async_emp  = (cnt==0);
   assign async_aemp = (cnt==1);
   assign async_bemp = (cnt==2);

   always @(posedge clk or posedge rst)
     if (rst)                         emp <= 1;
     else if (async_emp & ~wr_enable) emp <= 1;
     else if (async_aemp & cnt_back)  emp <= 1;
     else                             emp <= 0;

   always @(posedge clk or posedge rst)
     if (rst)                        aemp <= 1;
     else if (async_emp)             aemp <= 1;
     else if (async_aemp & ~cnt_fwd) aemp <= 1;
     else if (async_bemp & cnt_back) aemp <= 1;
     else                            aemp <= 0;

   //------------------------------------------------
   // FIFO memory
   always @(posedge clk) if (we) mem[wa] <= wd;
   always @(posedge clk) ra1 <= ra;
   assign rd = mem[ra1];
endmodule // SYNCFIFO_8x31
