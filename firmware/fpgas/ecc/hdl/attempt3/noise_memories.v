//////////////////////////////////////////////////////////////////////////////////
// Company: NewAE
// Engineer: Jean-Pierre Thibault
// 
// Create Date: 
// Design Name: 
// Module Name: lfsr
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

`default_nettype none
`timescale 1ns / 1ns

module noise_memories #(
   parameter pMEMORY_INSTANCES = 16
)(
   input  wire          clk,
   input  wire          rst,
   input  wire [2:0]    bram_rx_wr_addr,
   input  wire [2:0]    bram_ry_wr_addr,
   input  wire [2:0]    bram_rz_wr_addr,
   input  wire [2:0]    bram_rx_rd_addr,
   input  wire [2:0]    bram_ry_rd_addr,
   input  wire [2:0]    bram_rz_rd_addr,
   input  wire          I_wr_en,
   input  wire [15:0]   I_enable,
   input  wire [31:0]   I_lfsr_state,
   input  wire [3:0]    I_rd_bank,
   output wire [31:0]   O_rd_data_ax,
   output wire [31:0]   O_rd_data_ay,
   output wire [31:0]   O_rd_data_az,
   output wire [31:0]   O_rd_data_bx,
   output wire [31:0]   O_rd_data_by,
   output wire [31:0]   O_rd_data_bz
);

    wire [15:0] wr_en = {16{I_wr_en}};

    wire [31:0] bram_rx_lfsr_data [0:15];
    wire [31:0] bram_ry_lfsr_data [0:15];
    wire [31:0] bram_rz_lfsr_data [0:15];

    wire [31:0] bram_rxa_rd_data_lfsr [0:15];
    wire [31:0] bram_rya_rd_data_lfsr [0:15];
    wire [31:0] bram_rza_rd_data_lfsr [0:15];
    wire [31:0] bram_rxb_rd_data_lfsr [0:15];
    wire [31:0] bram_ryb_rd_data_lfsr [0:15];
    wire [31:0] bram_rzb_rd_data_lfsr [0:15];

    wire [31:0] base_x_wr_data = I_lfsr_state ^ {2'b01, {10{bram_rx_wr_addr}}};
    wire [31:0] base_y_wr_data = I_lfsr_state ^ {2'b10, {10{bram_rx_wr_addr}}};
    wire [31:0] base_z_wr_data = I_lfsr_state ^ {2'b11, {10{bram_rx_wr_addr}}};

    assign O_rd_data_ax = bram_rxa_rd_data_lfsr[I_rd_bank];
    assign O_rd_data_ay = bram_rya_rd_data_lfsr[I_rd_bank];
    assign O_rd_data_az = bram_rza_rd_data_lfsr[I_rd_bank];
    assign O_rd_data_bx = bram_rxb_rd_data_lfsr[I_rd_bank];
    assign O_rd_data_by = bram_ryb_rd_data_lfsr[I_rd_bank];
    assign O_rd_data_bz = bram_rzb_rd_data_lfsr[I_rd_bank];
    /* need this?
    always @(*) begin
       case (I_rd_bank):
          0: begin
            O_rd_data_x = bram_rxa_rd_data_lfsr[0]
          end
       endcase
    end
    */


    genvar i;
    generate for (i = 0; i < pMEMORY_INSTANCES; i = i + 1) begin: memory_instance
        assign bram_rx_lfsr_data[i] = base_x_wr_data ^ i;
        assign bram_ry_lfsr_data[i] = base_y_wr_data ^ i;
        assign bram_rz_lfsr_data[i] = base_z_wr_data ^ i;

        `ifdef __ICARUS__
            bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
            bram_rx_lfsr (
                 .clk       (clk),
                .a_addr     (bram_rx_wr_addr), 
                .a_wr       (wr_en[i] & I_enable[i]), 
                .a_in       (bram_rx_lfsr_data[i]), 
                .a_out      (bram_rxb_rd_data_lfsr[i]),
                .b_addr     (bram_rx_rd_addr),                                          
                .b_out      (bram_rxa_rd_data_lfsr[i])
            );

            bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
            bram_ry_lfsr (
                .clk        (clk),
                .a_addr     (bram_ry_wr_addr), 
                .a_wr       (wr_en[i] & I_enable[i]), 
                .a_in       (bram_ry_lfsr_data[i]), 
                .a_out      (bram_ryb_rd_data_lfsr[i]),
                .b_addr     (bram_ry_rd_addr),                                          
                .b_out      (bram_rya_rd_data_lfsr[i])
            );

            bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
            bram_rz_lfsr (
                .clk        (clk),
                .a_addr     (bram_rz_wr_addr), 
                .a_wr       (wr_en[i] & I_enable[i]), 
                .a_in       (bram_rz_lfsr_data[i]), 
                .a_out      (bram_rzb_rd_data_lfsr[i]),
                .b_addr     (bram_rz_rd_addr),                                          
                .b_out      (bram_rza_rd_data_lfsr[i])
            );
        `else

            BRAM_TDP_MACRO #(
               .BRAM_SIZE("36Kb"), // Target BRAM: "18Kb" or "36Kb"
               .DEVICE("7SERIES"), // Target device: "7SERIES"
               .DOA_REG(0), // Optional port A output register (0 or 1)
               .DOB_REG(0), // Optional port B output register (0 or 1)
               .INIT_A(36'h0000000), // Initial values on port A output port
               .INIT_B(36'h00000000), // Initial values on port B output port
               .INIT_FILE ("NONE"),
               .READ_WIDTH_A (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .READ_WIDTH_B (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .SIM_COLLISION_CHECK ("ALL"), // Collision check enable "ALL", "WARNING_ONLY",
               // "GENERATE_X_ONLY" or "NONE"
               .SRVAL_A(36'h00000000), // Set/Reset value for port A output
               .SRVAL_B(36'h00000000), // Set/Reset value for port B output
               .WRITE_MODE_A("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_MODE_B("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_WIDTH_A(32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .WRITE_WIDTH_B(32)  // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
            ) bram_rx_lfsr (
               .DOA         (bram_rxb_rd_data_lfsr[i]), // Output port-A data, width defined by READ_WIDTH_A parameter
               .DOB         (bram_rxa_rd_data_lfsr[i]), // Output port-B data, width defined by READ_WIDTH_B parameter
               .ADDRA       (bram_rx_wr_addr), // Input port-A address, width defined by Port A depth
               .ADDRB       (bram_rx_rd_addr), // Input port-B address, width defined by Port B depth
               .CLKA        (clk), // 1-bit input port-A clock
               .CLKB        (clk), // 1-bit input port-B clock
               .DIA         (bram_rx_lfsr_data[i]), // Input port-A data, width defined by WRITE_WIDTH_A parameter
               .DIB         (32'b0), // Input port-B data, width defined by WRITE_WIDTH_B parameter
               .ENA         (1'b1), // 1-bit input port-A enable
               .ENB         (1'b1), // 1-bit input port-B enable
               //.REGCEA      (REGCEA), // 1-bit input port-A output register enable
               //.REGCEB      (REGCEB), // 1-bit input port-B output register enable
               .RSTA        (rst), // 1-bit input port-A reset
               .RSTB        (rst), // 1-bit input port-B reset
               .WEA         ({4{wr_en[i] & I_enable[i]}}), // Input port-A write enable, width defined by Port A depth
               .WEB         (4'b0) // Input port-B write enable, width defined by Port B depth
            );


            BRAM_TDP_MACRO #(
               .BRAM_SIZE("36Kb"), // Target BRAM: "18Kb" or "36Kb"
               .DEVICE("7SERIES"), // Target device: "7SERIES"
               .DOA_REG(0), // Optional port A output register (0 or 1)
               .DOB_REG(0), // Optional port B output register (0 or 1)
               .INIT_A(36'h0000000), // Initial values on port A output port
               .INIT_B(36'h00000000), // Initial values on port B output port
               .INIT_FILE ("NONE"),
               .READ_WIDTH_A (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .READ_WIDTH_B (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .SIM_COLLISION_CHECK ("ALL"), // Collision check enable "ALL", "WARNING_ONLY",
               // "GENERATE_X_ONLY" or "NONE"
               .SRVAL_A(36'h00000000), // Set/Reset value for port A output
               .SRVAL_B(36'h00000000), // Set/Reset value for port B output
               .WRITE_MODE_A("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_MODE_B("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_WIDTH_A(32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .WRITE_WIDTH_B(32)  // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
            ) bram_ry_lfsr (
               .DOA         (bram_ryb_rd_data_lfsr[i]), // Output port-A data, width defined by READ_WIDTH_A parameter
               .DOB         (bram_rya_rd_data_lfsr[i]), // Output port-B data, width defined by READ_WIDTH_B parameter
               .ADDRA       (bram_ry_wr_addr), // Input port-A address, width defined by Port A depth
               .ADDRB       (bram_ry_rd_addr), // Input port-B address, width defined by Port B depth
               .CLKA        (clk), // 1-bit input port-A clock
               .CLKB        (clk), // 1-bit input port-B clock
               .DIA         (bram_ry_lfsr_data[i]), // Input port-A data, width defined by WRITE_WIDTH_A parameter
               .DIB         (32'b0), // Input port-B data, width defined by WRITE_WIDTH_B parameter
               .ENA         (1'b1), // 1-bit input port-A enable
               .ENB         (1'b1), // 1-bit input port-B enable
               //.REGCEA      (REGCEA), // 1-bit input port-A output register enable
               //.REGCEB      (REGCEB), // 1-bit input port-B output register enable
               .RSTA        (rst), // 1-bit input port-A reset
               .RSTB        (rst), // 1-bit input port-B reset
               .WEA         ({4{wr_en[i] & I_enable[i]}}), // Input port-A write enable, width defined by Port A depth
               .WEB         (4'b0) // Input port-B write enable, width defined by Port B depth
            );


            BRAM_TDP_MACRO #(
               .BRAM_SIZE("36Kb"), // Target BRAM: "18Kb" or "36Kb"
               .DEVICE("7SERIES"), // Target device: "7SERIES"
               .DOA_REG(0), // Optional port A output register (0 or 1)
               .DOB_REG(0), // Optional port B output register (0 or 1)
               .INIT_A(36'h0000000), // Initial values on port A output port
               .INIT_B(36'h00000000), // Initial values on port B output port
               .INIT_FILE ("NONE"),
               .READ_WIDTH_A (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .READ_WIDTH_B (32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .SIM_COLLISION_CHECK ("ALL"), // Collision check enable "ALL", "WARNING_ONLY",
               // "GENERATE_X_ONLY" or "NONE"
               .SRVAL_A(36'h00000000), // Set/Reset value for port A output
               .SRVAL_B(36'h00000000), // Set/Reset value for port B output
               .WRITE_MODE_A("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_MODE_B("WRITE_FIRST"), // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
               .WRITE_WIDTH_A(32), // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
               .WRITE_WIDTH_B(32)  // Valid values are 1-36 (19-36 only valid when BRAM_SIZE="36Kb")
            ) bram_rz_lfsr (
               .DOA         (bram_rzb_rd_data_lfsr[i]), // Output port-A data, width defined by READ_WIDTH_A parameter
               .DOB         (bram_rza_rd_data_lfsr[i]), // Output port-B data, width defined by READ_WIDTH_B parameter
               .ADDRA       (bram_rz_wr_addr), // Input port-A address, width defined by Port A depth
               .ADDRB       (bram_rz_rd_addr), // Input port-B address, width defined by Port B depth
               .CLKA        (clk), // 1-bit input port-A clock
               .CLKB        (clk), // 1-bit input port-B clock
               .DIA         (bram_rz_lfsr_data[i]), // Input port-A data, width defined by WRITE_WIDTH_A parameter
               .DIB         (32'b0), // Input port-B data, width defined by WRITE_WIDTH_B parameter
               .ENA         (1'b1), // 1-bit input port-A enable
               .ENB         (1'b1), // 1-bit input port-B enable
               //.REGCEA      (REGCEA), // 1-bit input port-A output register enable
               //.REGCEB      (REGCEB), // 1-bit input port-B output register enable
               .RSTA        (rst), // 1-bit input port-A reset
               .RSTB        (rst), // 1-bit input port-B reset
               .WEA         ({4{wr_en[i] & I_enable[i]}}), // Input port-A write enable, width defined by Port A depth
               .WEB         (4'b0) // Input port-B write enable, width defined by Port B depth
            );


        `endif
    end
    endgenerate

endmodule

`default_nettype wire

