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
               
     0x30 - 0x33 - Offset for WRITE registers (default = 0x400)
       [ LSB  = 0x00 (default)   ] 0x30
       [      = 0x04 (default)   ] 0x31
       [      = 0    (default)   ] 0x32
       [      = 0    (default)   ] 0x33
*/

`define REG_IDENTIFY        21'h000
`define REG_CRYPT_TYPE      21'h001
`define REG_CRYPT_REV       21'h002
`define REG_WRITEOFFSET     21'h030


/* 256 bytes reserved for each possible register */
//Output
`define REG_CRYPT_STATUS    21'h050
`define REG_CRYPT_TEXTOUT   21'h100
`define REG_CRYPT_CIPHEROUT 21'h200

//Input (NOTE - these are OFFSETs from value that is first read at address 0x030)
`define REG_LED             21'h010
`define REG_CLKSETTINGS     21'h020
`define REG_CRYPT_SETTINGS  21'h040
`define REG_CRYPT_KEY       21'h100
`define REG_CRYPT_TEXTIN    21'h200
`define REG_CRYPT_CIPHERIN  21'h300


module registers(
    /* Interface to Memory Bus */
    input wire           mem_clk,
    output wire [MEMORY_BYTES*8-1:0]  mem_output,
    input wire  [MEMORY_BYTES*8-1:0]  mem_input,
    
    /* LEDs                     */
    output wire          user_led,
    
    /* DIP Switches */
    input wire           dipsw_1,
    input wire           dipsw_2,
    
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
    input  wire [PT_WIDTH-1:0]  textout,
    output wire [CT_WIDTH-1:0]  cipherin,
    input wire  [CT_WIDTH-1:0]  cipherout,
    
    //TODO: Init wire doesn't work
    output wire          init,   /* High for one cryptoclk cycle, key valid (i.e.: do init stuff) */
    input wire           ready,  /* Crypto core ready. Tie to '1' if not used. */
    output wire          start,  /* High for one cryptoclk cycle, indicates text ready. */
    input wire           done    /* Crypto done. Can be high for one cryptoclk cycle or longer. */
    );
    
    parameter MEMORY_WIDTH = 8;                 // 2^8
    parameter MEMORY_BYTES = 1 << MEMORY_WIDTH; // = 256 bytes
    
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
    wire [7:0] reg_led;
    wire [7:0] reg_clk_settings;
    wire [7:0] reg_crypt_settings;
    reg [7:0] reg_crypt_status;
    wire [KEY_WIDTH-1:0] reg_crypt_key;
    reg [PT_WIDTH-1:0] reg_crypt_textout;
    wire [PT_WIDTH-1:0] reg_crypt_textin;
    reg [CT_WIDTH-1:0] reg_crypt_cipherout;
    wire [CT_WIDTH-1:0] reg_crypt_cipherin;
    
    assign user_led = reg_led[0];
    
    reg start_int;
    reg start_int2;
    
    assign key = reg_crypt_key;
    assign textin = reg_crypt_textin;
    assign cipherin = reg_crypt_cipherin;
    
    reg done_old;
    always @(posedge cryptoclk) begin
        done_old <= done & done_edge_sensitive;
    end
    
    always @(posedge cryptoclk) begin        
        if ((done) && (done_old == 1'b0)) begin
            reg_crypt_cipherout <= cipherout;
            reg_crypt_textout   <= textout;
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
   
    /* Outputs (addresses are directly read) */
    assign mem_output[`REG_IDENTIFY*8 +: 8] = 8'h2E; //Fixed for CW305 Examples
    assign mem_output[`REG_CRYPT_TYPE*8 +: 8] = crypt_type; //Crypt core type (AES, DES, etc)
    assign mem_output[`REG_CRYPT_REV*8 +: 8] = crypt_rev; //Crypt core revision
    
    assign mem_output[`REG_WRITEOFFSET*8 +: 32] = MEMORY_BYTES; //Offset for input registers
    
    assign mem_output[`REG_CRYPT_TEXTOUT*8 +: PT_WIDTH] = reg_crypt_textout;
    assign mem_output[`REG_CRYPT_CIPHEROUT*8 +: CT_WIDTH] = reg_crypt_cipherout;
    assign mem_output[`REG_CRYPT_STATUS*8 +: 8] = reg_crypt_status;

    /*** INPUTS - Must apply offset at REG_WRITEOFFSET */
    /* General board settings */
    assign reg_clk_settings = mem_input[`REG_CLKSETTINGS*8 +: 8];
    assign reg_led = mem_input[`REG_LED*8 +: 8];
    
    /* Cryptographic settings */
    assign reg_crypt_settings = mem_input[`REG_CRYPT_SETTINGS*8 +: 8];
    assign reg_crypt_textin =  mem_input[`REG_CRYPT_TEXTIN*8 +: PT_WIDTH];
    assign reg_crypt_cipherin =  mem_input[`REG_CRYPT_CIPHERIN*8 +: CT_WIDTH];
    assign reg_crypt_key =  mem_input[`REG_CRYPT_KEY*8 +: KEY_WIDTH];
    
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
