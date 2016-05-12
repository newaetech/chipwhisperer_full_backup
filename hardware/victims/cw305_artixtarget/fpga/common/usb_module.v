`timescale 1ns / 1ps

// usb_module.v


module usb_module(
    input wire        clk,      // Raw clock signal from external USB connections
    inout wire [7:0]  data,     // Data bus
    input wire [21:0] addr,     // Address bus
    input wire        rd_en,    // !RD: low when addr valid for read
    input wire        wr_en,      // !WR: low when data+addr valid for write
    input wire        cen,      // !CE: not used here
    input wire        trigger,  // High when trigger requested
    
    output reg [MEMORY_BYTES*8-1:0] memory_input,    // Memory bus for input from serial. Upper half of memory
    input wire [MEMORY_BYTES*8-1:0] memory_output    // Memory bus for output to serial.  Lower half of memory
    );
    
    // Default memory width = 256 bytes (0x100)
    parameter MEMORY_WIDTH = 8;                 // 2^8
    parameter MEMORY_BYTES = 1 << MEMORY_WIDTH; // = 256 bytes
    parameter MEMORY_MASK  = MEMORY_BYTES - 1;  // = 0xFF mask

    wire clk_bufg;
    IBUFG clkibuf (
        .O(clk_bufg),
        .I(clk) 
    );
    wire clk_buf;
    BUFG clkbuf(
        .O(clk_buf),
        .I(clk_bufg)
    ); 
    
    // Internals of USB
    wire [21:0] addr_mask;          // The mask for our addresses
    assign addr_mask = MEMORY_MASK; // Give ourselves 2^8 bytes for each memory
    
    wire [21:0] addr_masked;        // The masked copy of the addresses
    assign addr_masked = addr & addr_mask;
    
    wire addr_section;              // Whether the address is in input or output memory 
    assign addr_section = (addr > addr_mask) ? 1 : 0; 

    
    reg  [7:0] data_out;
    
    // Implementation
    always @ (posedge clk_buf) begin
        if (addr_section) begin  // top half of memory
            if(~wr_en) begin
                memory_input[addr_masked*8 +: 8] <= data;
            end
        end
        else begin
            data_out <= memory_output[addr_masked*8 +: 8];
        end
    end
    
    assign data = ~rd_en ? data_out : 8'bz;
    
endmodule
