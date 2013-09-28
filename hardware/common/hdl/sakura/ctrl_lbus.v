/*-------------------------------------------------------------------------
 AIST-LSI compatible Local bus controller
 
 File name   : ctrl_lbus.v
 Version     : 1.3
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
 appreciate it if you can cite our webpage
 (http://www.aist.go.jp/aist_e/research_results/publications/synthesiology_e/vol3_no1/vol03_01_p86_p95.pdf)
 -------------------------------------------------------------------------*/ 


//================================================ CTRL_LBUS
module CTRL_LBUS
  (lbus_a, lbus_di, lbus_do, lbus_wrn, lbus_rdn, // Local bus
   wd, we, ful, aful, rd, re, emp, aemp,         // Control signals of FIFO
   clk, rst);                                    // Clock and reset
   
   //------------------------------------------------
   // Local bus
   output [15:0] lbus_a;   // Address
   output [15:0] lbus_di;  // Input data  (Controller -> Cryptographic module)
   output        lbus_wrn; // Assert input data
   input [15:0]  lbus_do;  // Output data (Cryptographic module -> Controller)
   output        lbus_rdn; // Assert output data

   // Control signals of FIFO
   output [7:0]  wd;
   output        we;
   input         ful, aful;
   input [7:0]   rd;
   output        re;
   input         emp, aemp;

   // Clock and reset
   input         clk, rst;

   //------------------------------------------------
   reg [4:0]     state;
   reg [3:0]     cnt;
   reg [7:0]     wd;
   reg           we, re;
   wire          wr_busy, rd_busy;
   reg [15:0]    lbus_a;
   reg [15:0]    lbus_di;
   reg           lbus_rdn, lbus_wrn;
   reg [7:0]     cmd;
   reg [15:0]    fetch;
   
   //------------------------------------------------
   parameter [4:0]
     CMD0   = 5'h00,
     CMD1   = 5'h01,
     CMD2   = 5'h02,
     ADDR0  = 5'h03,
     ADDR1  = 5'h04,
     ADDR2  = 5'h05,
     ADDR3  = 5'h06,
     ADDR4  = 5'h07,
     ADDR5  = 5'h08,
     BRANCH = 5'h09,
     RD0    = 5'h0A,
     RD1    = 5'h0B,
     RD2    = 5'h0C,
     RD3    = 5'h0D,
     RD4    = 5'h0E,
     RD5    = 5'h0F,
     WR0    = 5'h10,
     WR1    = 5'h11,
     WR2    = 5'h12,
     WR3    = 5'h13,
     WR4    = 5'h14,
     WR5    = 5'h15,
     WR6    = 5'h16,
     WR7    = 5'h17,
     WR8    = 5'h18,
     WR9    = 5'h19,
     TRAP   = 5'h1A;
   
   //------------------------------------------------
   assign wr_busy = ful | (aful & we);
   assign rd_busy = emp | (aemp & re);

   always @(posedge clk or posedge rst) begin
      if (rst) begin
         state <= CMD0; cnt <= 4'h0;
         we <= 0; wd <= 8'h0; re <= 0;
         lbus_a <= 16'h0; lbus_di <= 16'h0; lbus_rdn <= 1; lbus_wrn <= 1;
      end else
        case (state)
          //--------
          CMD0: begin
             if (~rd_busy) begin state <= CMD1; re <= 1; end
             else          begin state <= CMD0; re <= 0; end
             we <= 0;
          end
          CMD1: begin state <= CMD2; re <= 0; end
          CMD2: begin state <= ADDR0; cmd <= rd; end

          //--------
          ADDR0: begin
             if (~rd_busy) begin state <= ADDR1; re <= 1; end
             else          begin state <= ADDR0; re <= 0; end
          end
          ADDR1: begin state <= ADDR2; re <= 0; end
          ADDR2: begin state <= ADDR3; lbus_a[15:8] <= rd; end

          ADDR3: begin
             if (~rd_busy) begin state <= ADDR4; re <= 1; end
             else          begin state <= ADDR3; re <= 0; end
          end
          ADDR4: begin state <= ADDR5; re <= 0; end
          ADDR5: begin state <= BRANCH; lbus_a[7:0] <= rd; end

          //--------
          BRANCH: begin
             if      (cmd==8'h00) state <= RD0;
             else if (cmd==8'h01) state <= WR0;
             else                 state <= TRAP;
             cnt <= 0;
          end

          //--------
          RD0: begin
             if (cnt==4'h3) state <= RD1;
             else           state <= RD0;
             cnt <= cnt + 4'h1;
          end
          RD1: begin state <= RD2; cnt <= 4'h0; lbus_rdn <= 0; end

          RD2: begin
             if (cnt==4'h3) state <= RD3;
             else           state <= RD2;
             cnt <= cnt + 4'h1;
          end
          RD3: begin state <= RD4; fetch <= lbus_do; lbus_rdn <= 1; end

          RD4: begin
             if (~wr_busy) begin state <= RD5; we <= 1; end
             else          begin state <= RD4; we <= 0; end
             wd <= fetch[15:8];
          end
          RD5: begin
             if (~wr_busy) begin state <= CMD0; we <= 1; end
             else          begin state <= RD5;  we <= 0; end
             wd <= fetch[7:0];
          end

          //--------
          WR0: begin
             if (~rd_busy) begin state <= WR1; re <= 1; end
             else          begin state <= WR0; re <= 0; end
          end
          WR1: begin state <= WR2; re <= 0; end
          WR2: begin state <= WR3; lbus_di[15:8] <= rd; end

          WR3: begin
             if (~rd_busy) begin state <= WR4; re <= 1; end
             else          begin state <= WR3; re <= 0; end
          end
          WR4: begin state <= WR5; re <= 0; end
          WR5: begin state <= WR6; lbus_di[7:0] <= rd; end

          WR6: begin
             if (cnt==4'h3) state <= WR7;
             else           state <= WR6;
             cnt <= cnt + 4'h1;
          end
          WR7: begin state <= WR8; cnt <= 4'h0; lbus_wrn <= 0; end

          WR8: begin
             if (cnt==4'h3) state <= WR9;
             else           state <= WR8;
             cnt <= cnt + 4'h1;
          end
          WR9: begin state <= CMD0; lbus_wrn <= 1; end

          //--------
          TRAP: begin state <= TRAP; end
        endcase // case (state)
   end // always @ (posedge clk or posedge rst)
   
endmodule // CTRL_LBUS
