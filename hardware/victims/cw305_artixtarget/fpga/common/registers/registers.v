/* 
ChipWhisperer Artix Target - Registers for crypto modules.

Copyright (c) 2016, NewAE Technology Inc.
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

`timescale 1ns / 1ps
`default_nettype none 

/*  Registers:	 
	 0x00 - Identify	 	 
	   [  0  0  1  0  1  1  1  0 ]	   		
		  Always reads as 0x2E
		  
     0x01 - Crypt Type     
       [ B7 B6 B5 B4 B3 B2 B1 B0 ]
       
     0x02 - Crypt Revision       
       [ B7 B6 B5 B4 B3 B2 B1 B0 ]
         
     0x10 - Board LED Settings     
       [  x  x  x  x  x  x  x L0 ]
       
          L0 = Blue LED, 1 = On 
 
     0x20 - Clock Settings
       [  x  x  x O1 O0 I1 I0 OV ]
         
          OV = Override DIP-Switches
               1 = Clock Source & Output
                   come from this register.
               0 = Clock Source & Output
                   come from DIP switch setting.
                   
          I = Input Source
              I1 I0
               0 0 = use PLL1
               1 0 = Use 20-pin HS-In
               
          O = Output Enables
              O1 O0
               0 0 = Outputs low
               0 1 = 20pin HS-Out is crypto-clock
*/

`define REG_IDENTIFY        21'h000
`define REG_LED             21'h010
`define REG_CLKSETTINGS     21'h020

/* 256 bytes reserved for each possible register */
`define REG_CRYPT_MASK      21'h0FF
`define REG_CRYPT_SETTINGS  21'h100
`define REG_CRYPT_STATUS    21'h110
`define REG_CRYPT_KEY       21'h200
`define REG_CRYPT_TEXTIN    21'h300
`define REG_CRYPT_TEXTOUT   21'h400
`define REG_CRYPT_CIPHERIN  21'h500
`define REG_CRYPT_CIPHEROUT 21'h600

//Module Information


module registers(
    /* Interface to USB Chip */
    input wire           clk,
    input wire [7:0]     datain,
    output wire [7:0]    dataout,
    input wire [20:0]    addr,
    input wire           wren,
    input wire           rden,
    
    /* LEDs                     */
    output wire         user_led,
    
    /* DIP Switches */
    input wire          dipsw_1,
    input wire          dipsw_2,
    
    /* Other Hardware           */
    input wire           exttrigger_in, /* External trigger */
    
    /* Clock sources            */
    input wire           pll_clk1,
    input wire           cw_clkin,
    output wire          cw_clkout,
    
    /* Module Type              */
    input wire [7:0]     crypt_type,
    input wire [7:0]     crypt_rev,
    
    /* Interface to Crypto Core */
    output wire          cryptoclk,
    output wire [KEY_WIDTH-1:0]  key,
    output wire [PT_WIDTH-1:0]  textin,
    input wire  [CT_WIDTH-1:0]  cipherout,
    
    output wire          init,   /* High for one cryptoclk cycle, key valid (i.e.: do init stuff) */
    input wire           ready,  /* Crypto core ready. Tie to '1' if not used. */
    output wire          start,  /* High for one cryptoclk cycle, indicates text ready. */
    input wire           done    /* Crypto done. Can be high for one cryptoclk cycle or longer. */
    );
    
    parameter KEY_WIDTH = 128 ;
    parameter PT_WIDTH = 128 ;
    parameter CT_WIDTH = 128;
    
    /* If set to '1', the 'done' flag is edge sensitive, and only on a rising edge
       is the value copied from cipherout/textout into internal registers.
       
       If set to '0', the value is copied over as long as 'done' flag is high.
       
       If using an inverted 'busy' flag as the done flag, you will require the
       edge-sensitive option for example.
    */       
    parameter done_edge_sensitive = 1;
    
    /* Registers */
    wire [32:0] reg_identity;
    reg [7:0] reg_led;
    reg [7:0] reg_clk_settings;
    reg [7:0] reg_crypt_settings;
    reg [7:0] reg_crypt_status;
    reg [KEY_WIDTH-1:0] reg_crypt_key;
    reg [PT_WIDTH-1:0] reg_crypt_textout;
    reg [PT_WIDTH-1:0] reg_crypt_textin;
    reg [CT_WIDTH-1:0] reg_crypt_cipherout;
    reg [CT_WIDTH-1:0] reg_crypt_cipherin;
    
    assign user_led = reg_led[0];
    
    reg start_int;
    reg start_int2;
    
    assign key = reg_crypt_key;
    assign textin = reg_crypt_textin;
    
    reg done_old;
    always @(posedge cryptoclk) begin
        done_old <= done & done_edge_sensitive;
    end
    
    always @(posedge cryptoclk) begin        
        if ((done) && (done_old == 1'b0)) begin
            reg_crypt_cipherout <= cipherout;
        end
    end
    
    always @(posedge cryptoclk) begin
        if (start_int == 1'b1)
            reg_crypt_status[0] <= 1'b0;
        else if (done == 1'b1)
            reg_crypt_status[0] <= 1'b1;       
    end
    
    always @(posedge cryptoclk) begin
        if ((reg_crypt_settings[0] | exttrigger_in) == 1'b1) begin
            start_int <= ~start_int2 & (reg_crypt_settings[0] | exttrigger_in);
            start_int2 <= 1'b1;               
        end else begin
            start_int2 <= 1'b0;
            start_int <= 1'b0;
        end
    end
    
    assign start = start_int;
    
    reg [7:0] data_intout;
    assign dataout = data_intout;
    
    always @(posedge clk) begin
        if (wren) begin
            if (addr == `REG_LED) begin
                reg_led <= datain;
            end else if (addr == `REG_CLKSETTINGS) begin
                reg_clk_settings <= datain;
            end
        end
    end
    
    wire [21:0] addr_masked;
    wire [7:0]  addr_bytecnt;
    assign addr_masked = (~`REG_CRYPT_MASK) & addr;
    assign addr_bytecnt = `REG_CRYPT_MASK & addr;
    
    always @(posedge clk)
        if (wren) begin
            case (addr_masked)
                `REG_CRYPT_SETTINGS:    if (addr_bytecnt == 8'h00) reg_crypt_settings <= datain; 
                `REG_CRYPT_TEXTIN:      reg_crypt_textin[addr_bytecnt*8 +: 8] <= datain;
                //`REG_CRYPT_TEXTOUT:     reg_crypt_textout[addr_bytecnt*8 +: 8] <= datain;
                `REG_CRYPT_CIPHERIN:    reg_crypt_cipherin[addr_bytecnt*8 +: 8] <= datain;
                //`REG_CRYPT_CIPHEROUT:   reg_crypt_cipherout[addr_bytecnt*8 +: 8] <= datain;
                `REG_CRYPT_KEY:         reg_crypt_key[addr_bytecnt*8 +: 8] <= datain;
            endcase
        end       
    
    /* NOTE:
       addr_masked has lower 8-bit masked out, which is designed when we will primarily be using larger
       registers sizes. If you don't need a bunch of larger registers, it instead makes more sense to directly
       used the 'addr' variable and the case is on each register */
    always @(posedge clk) begin
      if (rden) begin
        case (addr_masked)
            `REG_IDENTIFY:          if (addr_bytecnt == 8'h00) data_intout <= 8'h2E;
            /* Module ID */         else if (addr_bytecnt == 8'h01) data_intout <= crypt_type;
            /* Module Rev */        else if (addr_bytecnt == 8'h02) data_intout <= crypt_rev;
            /*`REG_CLKSETTINGS*/    else if (addr_bytecnt == 8'h20) data_intout <= reg_clk_settings;
                                    else data_intout <= 0;            
            `REG_CRYPT_SETTINGS:    if (addr_bytecnt == 8'h00) data_intout <= reg_crypt_settings;
            /*REG_CRYPT_STATUS */   else if (addr_bytecnt == 8'h10) data_intout <= reg_crypt_status;
                                    else data_intout <= 0;
            `REG_CRYPT_TEXTIN:      data_intout <= reg_crypt_textin[addr_bytecnt*8 +: 8];
            `REG_CRYPT_TEXTOUT:     data_intout <= reg_crypt_textout[addr_bytecnt*8 +: 8];
            `REG_CRYPT_CIPHERIN:    data_intout <= reg_crypt_cipherin[addr_bytecnt*8 +: 8];
            `REG_CRYPT_CIPHEROUT:   data_intout <= reg_crypt_cipherout[addr_bytecnt*8 +: 8];
            `REG_CRYPT_KEY:         data_intout <= reg_crypt_key[addr_bytecnt*8 +: 8];
            default: begin
                data_intout <= 8'hBA; //0xBA marks bad data for debug purposes
            end
        endcase
     end
    end
    
    /* Select crypto clock based on registers + DIP switches */
    wire cclk_src_is_ext;
    assign cclk_src_is_ext = (reg_clk_settings[2:0] == 3'b001) ? 0 : //Registers = PLL1
                             (reg_clk_settings[2:0] == 3'b101) ? 1 : //Registers = 20pin
                             ((reg_clk_settings[0] == 1'b0) && (dipsw_1 == 1'b1)) ? 1: //DIP = 20pin
                             0; //Default PLL1
                             
    wire cclk_output_ext;              
    assign cclk_output_ext = ((reg_clk_settings[0] == 1'b1) && (reg_clk_settings[4:3] == 2'b00)) ? 0 : //Registers = off
                             ((reg_clk_settings[0] == 1'b1) && (reg_clk_settings[4:3] == 2'b01)) ? 1 : //Registers = on
                             ((reg_clk_settings[0] == 1'b0) && (dipsw_2 == 1'b1)) ? 1 : //DIP = on
                             0; //Default off
    BUFGCTRL CCLK_MUX (
       .O(cryptoclk),      // 1-bit output: Clock output
       .CE0(1'b1),         // 1-bit input: Clock enable input for I0
       .CE1(1'b1),         // 1-bit input: Clock enable input for I1
       .I0(pll_clk1),      // 1-bit input: Primary clock
       .I1(cw_clkin),      // 1-bit input: Secondary clock
       .IGNORE0(1'b1),     // 1-bit input: Clock ignore input for I0
       .IGNORE1(1'b1),     // 1-bit input: Clock ignore input for I1
       .S0(~cclk_src_is_ext),  // 1-bit input: Clock select for I0
       .S1(cclk_src_is_ext)  // 1-bit input: Clock select for I1
    );    
    
    ODDR CWOUT_ODDR (
       .Q(cw_clkout),   // 1-bit DDR output
       .C(cryptoclk),   // 1-bit clock input
       .CE(cclk_output_ext), // 1-bit clock enable input
       .D1(1'b1), // 1-bit data input (positive edge)
       .D2(1'b0), // 1-bit data input (negative edge)
       .R(1'b0),   // 1-bit reset
       .S(1'b0)    // 1-bit set
    );
    
endmodule
