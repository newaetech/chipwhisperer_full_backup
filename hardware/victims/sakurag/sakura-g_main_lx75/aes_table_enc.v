/*-------------------------------------------------------------------------
 AES (128-bit, table S-box, encryption)

 File name   : aes_table_enc.v
 Version     : 1.0
 Created     : MAY/30/2012
 Last update : MAY/30/2012
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


//================================================ AES_Table_enc
module AES_Table_enc
  (Kin, Din, Dout, Krdy, Drdy, Kvld, Dvld, EN, BSY, CLK, RSTn);

   //------------------------------------------------
   input  [127:0] Kin;  // Key input
   input [127:0]  Din;  // Data input
   output [127:0] Dout; // Data output
   input          Krdy; // Key input ready
   input          Drdy; // Data input ready
   output         Kvld; // Data output valid
   output         Dvld; // Data output valid

   input          EN;   // AES circuit enable
   output         BSY;  // Busy signal
   input          CLK;  // System clock
   input          RSTn; // Reset (Low active)

   //------------------------------------------------
   reg [127:0]    dat, key, rkey;
   wire [127:0]   dat_next, rkey_next;
   reg [9:0]      rnd;  
   reg [7:0]      rcon; 
   reg            sel;  // Indicate final round
   reg            Dvld, Kvld, BSY;
   wire           rst;
   
   //------------------------------------------------
   assign rst = ~RSTn;
     
   always @(posedge CLK or posedge rst) begin
      if (rst)     Dvld <= 0;
      else if (EN) Dvld <= sel;
   end

   always @(posedge CLK or posedge rst) begin
      if (rst) Kvld <= 0;
      else if (EN) Kvld <= Krdy;
   end

   always @(posedge CLK or posedge rst) begin
      if (rst) BSY <= 0;
      else if (EN) BSY <= Drdy | |rnd[9:1] | sel;
   end
   
   AES_Core aes_core 
     (.din(dat),  .dout(dat_next),  .kin(rkey_next), .sel(sel));
   KeyExpantion keyexpantion 
     (.kin(rkey), .kout(rkey_next), .rcon(rcon));
   
   always @(posedge CLK or posedge rst) begin
      if (rst)             rnd <= 10'b0000_0000_01;
      else if (EN) begin
         if (Drdy)         rnd <= {rnd[8:0], rnd[9]};
         else if (~rnd[0]) rnd <= {rnd[8:0], rnd[9]};
      end
   end
   
   always @(posedge CLK or posedge rst) begin
      if (rst)     sel <= 0;
      else if (EN) sel <= rnd[9];
   end
   
   always @(posedge CLK or posedge rst) begin
      if (rst)                 dat <= 128'h0;
      else if (EN) begin
         if (Drdy)             dat <= Din ^ key;
         else if (~rnd[0]|sel) dat <= dat_next;
      end
   end
   assign Dout = dat;
   
   always @(posedge CLK or posedge rst) begin
      if (rst)     key <= 128'h0;
      else if (EN)
        if (Krdy)  key <= Kin;
   end

   always @(posedge CLK or posedge rst) begin
      if (rst)         rkey <= 128'h0;
      else if (EN) begin
         if (Krdy)   rkey <= Kin;
         else if (rnd[0]) rkey <= key;
         else             rkey <= rkey_next;
      end
   end
   
   always @(posedge CLK or posedge rst) begin
     if (rst)          rcon <= 8'h01;
     else if (EN) begin
        if (Drdy)    rcon <= 8'h01;
        else if (~rnd[0]) rcon <= xtime(rcon);
     end
   end
   
   function [7:0] xtime;
      input [7:0] x;
      xtime = (x[7]==1'b0)? {x[6:0],1'b0} : {x[6:0],1'b0} ^ 8'h1B;
   endfunction

endmodule // AES_Table_enc



//================================================ KeyExpantion
module KeyExpantion (kin, kout, rcon);

   //------------------------------------------------
   input [127:0]  kin;
   output [127:0] kout;
   input [7:0] 	  rcon;

   //------------------------------------------------
   wire [31:0]    ws, wr, w0, w1, w2, w3;

   //------------------------------------------------
   SubBytes SB0 ({kin[23:16], kin[15:8], kin[7:0], kin[31:24]}, ws);
   assign wr = {(ws[31:24] ^ rcon), ws[23:0]};

   assign w0 = wr ^ kin[127:96];
   assign w1 = w0 ^ kin[95:64];
   assign w2 = w1 ^ kin[63:32];
   assign w3 = w2 ^ kin[31:0];

   assign kout = {w0, w1, w2, w3};

endmodule // KeyExpantion



//================================================ AES_Core
module AES_Core (din, dout, kin, sel);

   //------------------------------------------------
   input  [127:0] din, kin;
   input          sel;
   output [127:0] dout;
   
   //------------------------------------------------
   wire [31:0] st0, st1, st2, st3, // state
               sb0, sb1, sb2, sb3, // SubBytes
               sr0, sr1, sr2, sr3, // ShiftRows
               sc0, sc1, sc2, sc3, // MixColumns
               sk0, sk1, sk2, sk3; // AddRoundKey

   //------------------------------------------------
   // din -> state
   assign st0 = din[127:96];
   assign st1 = din[ 95:64];
   assign st2 = din[ 63:32];
   assign st3 = din[ 31: 0];

   // SubBytes
   SubBytes SB0 (st0, sb0);
   SubBytes SB1 (st1, sb1);
   SubBytes SB2 (st2, sb2);
   SubBytes SB3 (st3, sb3);

   // ShiftRows
   assign sr0 = {sb0[31:24], sb1[23:16], sb2[15: 8], sb3[ 7: 0]};
   assign sr1 = {sb1[31:24], sb2[23:16], sb3[15: 8], sb0[ 7: 0]};
   assign sr2 = {sb2[31:24], sb3[23:16], sb0[15: 8], sb1[ 7: 0]};
   assign sr3 = {sb3[31:24], sb0[23:16], sb1[15: 8], sb2[ 7: 0]};

   // MixColumns
   MixColumns MC0 (sr0, sc0);
   MixColumns MC1 (sr1, sc1);
   MixColumns MC2 (sr2, sc2);
   MixColumns MC3 (sr3, sc3);

   // AddRoundKey
   assign sk0 = (sel) ? sr0 ^ kin[127:96] : sc0 ^ kin[127:96];
   assign sk1 = (sel) ? sr1 ^ kin[ 95:64] : sc1 ^ kin[ 95:64];
   assign sk2 = (sel) ? sr2 ^ kin[ 63:32] : sc2 ^ kin[ 63:32];
   assign sk3 = (sel) ? sr3 ^ kin[ 31: 0] : sc3 ^ kin[ 31: 0];

   // state -> dout
   assign dout = {sk0, sk1, sk2, sk3};
endmodule // AES_Core



//================================================ MixColumns
module MixColumns(x, y);

   //------------------------------------------------
   input  [31:0]  x;
   output [31:0]  y;

   //------------------------------------------------
   wire [7:0]    a0, a1, a2, a3;
   wire [7:0]    b0, b1, b2, b3;

   assign a0 = x[31:24];
   assign a1 = x[23:16];
   assign a2 = x[15: 8];
   assign a3 = x[ 7: 0];

   assign b0 = xtime(a0);
   assign b1 = xtime(a1);
   assign b2 = xtime(a2);
   assign b3 = xtime(a3);

   assign y[31:24] =    b0 ^ a1^b1 ^ a2    ^ a3;
   assign y[23:16] = a0        ^b1 ^ a2^b2 ^ a3;
   assign y[15: 8] = a0    ^ a1        ^b2 ^ a3^b3;
   assign y[ 7: 0] = a0^b0 ^ a1    ^ a2        ^b3;
  
   function [7:0] xtime;
      input [7:0] x;
      xtime = (x[7]==1'b0)? {x[6:0],1'b0} : {x[6:0],1'b0} ^ 8'h1B;
   endfunction
   
endmodule // MixColumns



//================================================ SubBytes
module SubBytes (x, y);

   //------------------------------------------------
   input  [31:0] x;
   output [31:0] y;

   //------------------------------------------------
   assign y = {s(x[31:24]), s(x[23:16]), s(x[15:8]), s(x[7:0])};

   function [7:0] s;
      input [7:0] x;
      case (x)
        8'h00: s=8'h63;  8'h01: s=8'h7c;  8'h02: s=8'h77;  8'h03: s=8'h7b;
        8'h04: s=8'hf2;  8'h05: s=8'h6b;  8'h06: s=8'h6f;  8'h07: s=8'hc5;
        8'h08: s=8'h30;  8'h09: s=8'h01;  8'h0A: s=8'h67;  8'h0B: s=8'h2b;
        8'h0C: s=8'hfe;  8'h0D: s=8'hd7;  8'h0E: s=8'hab;  8'h0F: s=8'h76;
        
        8'h10: s=8'hca;  8'h11: s=8'h82;  8'h12: s=8'hc9;  8'h13: s=8'h7d;
        8'h14: s=8'hfa;  8'h15: s=8'h59;  8'h16: s=8'h47;  8'h17: s=8'hf0;
        8'h18: s=8'had;  8'h19: s=8'hd4;  8'h1A: s=8'ha2;  8'h1B: s=8'haf;
        8'h1C: s=8'h9c;  8'h1D: s=8'ha4;  8'h1E: s=8'h72;  8'h1F: s=8'hc0;
        
        8'h20: s=8'hb7;  8'h21: s=8'hfd;  8'h22: s=8'h93;  8'h23: s=8'h26;
        8'h24: s=8'h36;  8'h25: s=8'h3f;  8'h26: s=8'hf7;  8'h27: s=8'hcc;
        8'h28: s=8'h34;  8'h29: s=8'ha5;  8'h2A: s=8'he5;  8'h2B: s=8'hf1;
        8'h2C: s=8'h71;  8'h2D: s=8'hd8;  8'h2E: s=8'h31;  8'h2F: s=8'h15;
        
        8'h30: s=8'h04;  8'h31: s=8'hc7;  8'h32: s=8'h23;  8'h33: s=8'hc3;
        8'h34: s=8'h18;  8'h35: s=8'h96;  8'h36: s=8'h05;  8'h37: s=8'h9a;
        8'h38: s=8'h07;  8'h39: s=8'h12;  8'h3A: s=8'h80;  8'h3B: s=8'he2;
        8'h3C: s=8'heb;  8'h3D: s=8'h27;  8'h3E: s=8'hb2;  8'h3F: s=8'h75;
        
        8'h40: s=8'h09;  8'h41: s=8'h83;  8'h42: s=8'h2c;  8'h43: s=8'h1a;
        8'h44: s=8'h1b;  8'h45: s=8'h6e;  8'h46: s=8'h5a;  8'h47: s=8'ha0;
        8'h48: s=8'h52;  8'h49: s=8'h3b;  8'h4A: s=8'hd6;  8'h4B: s=8'hb3;
        8'h4C: s=8'h29;  8'h4D: s=8'he3;  8'h4E: s=8'h2f;  8'h4F: s=8'h84;
        
        8'h50: s=8'h53;  8'h51: s=8'hd1;  8'h52: s=8'h00;  8'h53: s=8'hed;
        8'h54: s=8'h20;  8'h55: s=8'hfc;  8'h56: s=8'hb1;  8'h57: s=8'h5b;
        8'h58: s=8'h6a;  8'h59: s=8'hcb;  8'h5A: s=8'hbe;  8'h5B: s=8'h39;
        8'h5C: s=8'h4a;  8'h5D: s=8'h4c;  8'h5E: s=8'h58;  8'h5F: s=8'hcf;
        
        8'h60: s=8'hd0;  8'h61: s=8'hef;  8'h62: s=8'haa;  8'h63: s=8'hfb;
        8'h64: s=8'h43;  8'h65: s=8'h4d;  8'h66: s=8'h33;  8'h67: s=8'h85;
        8'h68: s=8'h45;  8'h69: s=8'hf9;  8'h6A: s=8'h02;  8'h6B: s=8'h7f;
        8'h6C: s=8'h50;  8'h6D: s=8'h3c;  8'h6E: s=8'h9f;  8'h6F: s=8'ha8;
        
        8'h70: s=8'h51;  8'h71: s=8'ha3;  8'h72: s=8'h40;  8'h73: s=8'h8f;
        8'h74: s=8'h92;  8'h75: s=8'h9d;  8'h76: s=8'h38;  8'h77: s=8'hf5;
        8'h78: s=8'hbc;  8'h79: s=8'hb6;  8'h7A: s=8'hda;  8'h7B: s=8'h21;
        8'h7C: s=8'h10;  8'h7D: s=8'hff;  8'h7E: s=8'hf3;  8'h7F: s=8'hd2;
        
        8'h80: s=8'hcd;  8'h81: s=8'h0c;  8'h82: s=8'h13;  8'h83: s=8'hec;
        8'h84: s=8'h5f;  8'h85: s=8'h97;  8'h86: s=8'h44;  8'h87: s=8'h17;
        8'h88: s=8'hc4;  8'h89: s=8'ha7;  8'h8A: s=8'h7e;  8'h8B: s=8'h3d;
        8'h8C: s=8'h64;  8'h8D: s=8'h5d;  8'h8E: s=8'h19;  8'h8F: s=8'h73;
        
        8'h90: s=8'h60;  8'h91: s=8'h81;  8'h92: s=8'h4f;  8'h93: s=8'hdc;
        8'h94: s=8'h22;  8'h95: s=8'h2a;  8'h96: s=8'h90;  8'h97: s=8'h88;
        8'h98: s=8'h46;  8'h99: s=8'hee;  8'h9A: s=8'hb8;  8'h9B: s=8'h14;
        8'h9C: s=8'hde;  8'h9D: s=8'h5e;  8'h9E: s=8'h0b;  8'h9F: s=8'hdb;
        
        8'hA0: s=8'he0;  8'hA1: s=8'h32;  8'hA2: s=8'h3a;  8'hA3: s=8'h0a;
        8'hA4: s=8'h49;  8'hA5: s=8'h06;  8'hA6: s=8'h24;  8'hA7: s=8'h5c;
        8'hA8: s=8'hc2;  8'hA9: s=8'hd3;  8'hAA: s=8'hac;  8'hAB: s=8'h62;
        8'hAC: s=8'h91;  8'hAD: s=8'h95;  8'hAE: s=8'he4;  8'hAF: s=8'h79;
        
        8'hB0: s=8'he7;  8'hB1: s=8'hc8;  8'hB2: s=8'h37;  8'hB3: s=8'h6d;
        8'hB4: s=8'h8d;  8'hB5: s=8'hd5;  8'hB6: s=8'h4e;  8'hB7: s=8'ha9;
        8'hB8: s=8'h6c;  8'hB9: s=8'h56;  8'hBA: s=8'hf4;  8'hBB: s=8'hea;
        8'hBC: s=8'h65;  8'hBD: s=8'h7a;  8'hBE: s=8'hae;  8'hBF: s=8'h08;
        
        8'hC0: s=8'hba;  8'hC1: s=8'h78;  8'hC2: s=8'h25;  8'hC3: s=8'h2e;
        8'hC4: s=8'h1c;  8'hC5: s=8'ha6;  8'hC6: s=8'hb4;  8'hC7: s=8'hc6;
        8'hC8: s=8'he8;  8'hC9: s=8'hdd;  8'hCA: s=8'h74;  8'hCB: s=8'h1f;
        8'hCC: s=8'h4b;  8'hCD: s=8'hbd;  8'hCE: s=8'h8b;  8'hCF: s=8'h8a;

        8'hD0: s=8'h70;  8'hD1: s=8'h3e;  8'hD2: s=8'hb5;  8'hD3: s=8'h66;
        8'hD4: s=8'h48;  8'hD5: s=8'h03;  8'hD6: s=8'hf6;  8'hD7: s=8'h0e;
        8'hD8: s=8'h61;  8'hD9: s=8'h35;  8'hDA: s=8'h57;  8'hDB: s=8'hb9;
        8'hDC: s=8'h86;  8'hDD: s=8'hc1;  8'hDE: s=8'h1d;  8'hDF: s=8'h9e;
        
        8'hE0: s=8'he1;  8'hE1: s=8'hf8;  8'hE2: s=8'h98;  8'hE3: s=8'h11;
        8'hE4: s=8'h69;  8'hE5: s=8'hd9;  8'hE6: s=8'h8e;  8'hE7: s=8'h94;
        8'hE8: s=8'h9b;  8'hE9: s=8'h1e;  8'hEA: s=8'h87;  8'hEB: s=8'he9;
        8'hEC: s=8'hce;  8'hED: s=8'h55;  8'hEE: s=8'h28;  8'hEF: s=8'hdf;
        
        8'hF0: s=8'h8c;  8'hF1: s=8'ha1;  8'hF2: s=8'h89;  8'hF3: s=8'h0d;
        8'hF4: s=8'hbf;  8'hF5: s=8'he6;  8'hF6: s=8'h42;  8'hF7: s=8'h68;
        8'hF8: s=8'h41;  8'hF9: s=8'h99;  8'hFA: s=8'h2d;  8'hFB: s=8'h0f;
        8'hFC: s=8'hb0;  8'hFD: s=8'h54;  8'hFE: s=8'hbb;  8'hFF: s=8'h16;
      endcase
   endfunction

endmodule // SubBytes
