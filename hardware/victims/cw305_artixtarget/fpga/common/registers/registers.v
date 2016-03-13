`timescale 1ns / 1ps

//
`default_nettype none 
//////////////////////////////////////////////////////////////////////////////////
// Company: Diff
// Engineer: 
// 
// Create Date: 11/08/2015 04:20:59 PM
// Design Name: 
// Module Name: registers
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

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
                `REG_CLKSETTINGS:       if (addr_bytecnt == 8'h00) reg_clk_settings <= datain;
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
                data_intout <= 8'h00;
            end
        endcase
     end
    end
    
    /* Select crypto clock based on registers + DIP switches */
    assign cryptoclk = pll_clk1;
    
endmodule
