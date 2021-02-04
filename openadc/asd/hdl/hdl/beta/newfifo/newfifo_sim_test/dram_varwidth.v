`timescale 1ns / 1ps
`default_nettype none

module dram_varwidth #(
	 parameter addr_width = 10,
	 
	 parameter wr_data_width = 10,
	 parameter wr_pad_width = 2,
	 parameter wr_data_per_addr = 3,
	 
	 parameter rd_data_width = 8,
	 parameter rd_data_per_addr = 4,
	 parameter rd_pad_width = 0
)	 
(
    input wire [wr_data_width-1:0] wr_data,
`ifdef BRAM_ENABLE_WR_PADDING
	 input wire [wr_pad_width-1:0] wr_padding,
`endif
    input wire [addr_width-1:0] wr_addr,
    input wire [4:0] wr_ws,	 
    input wire wr_ce,
    input wire wr_clk,
    output wire [rd_data_width-1:0] rd_data,
`ifdef BRAM_ENABLE_RD_PADDING
	 output wire [rd_pad_width-1:0] rd_padding,
`endif
	 input wire  [addr_width-1:0] rd_addr,
	 input wire  [4:0] rd_ws,
	 input wire rd_ce,
	 input wire rd_clk	 
    );
	 
	 /******* WRITE LOGIC *******/
	 reg [addr_width-1:0] wr_addr_reg;
	 reg [31:0] wr_data_reg;

	 // Get padding if used last one
`ifdef BRAM_ENABLE_WR_PADDING
	 reg [31-wr_pad_width:0] wr_temp_low;	
	 reg [wr_pad_width-1:0] wr_temp_high;
	 wire [31:0] wr_temp;
	 assign wr_temp = {wr_temp_high, wr_temp_low};	 

	 always @(posedge wr_clk)
		if (wr_ce & (wr_ws == (wr_data_per_addr-1)))
			wr_temp_high <= wr_padding;
			
	 always @(posedge wr_clk)
		if (wr_ce)
			wr_temp_low[wr_ws*wr_data_width +: wr_data_width] <= wr_data;	
`else
	 reg [31:0] wr_temp;
	 initial
		wr_temp <= 32'd0;
	 
	 always @(posedge wr_clk)
		if (wr_ce)
			wr_temp[wr_ws*wr_data_width +: wr_data_width] <= wr_data;	
`endif	 	 
		 	 
    // When last word written, write into BRAM		
	 wire wr_done;
	 
	 assign wr_done = wr_ce & (wr_ws == (wr_data_per_addr-1));
	 
	 reg wr_en;
	 always @(posedge wr_clk)
		wr_en <= wr_done;
			
	 always @(posedge wr_clk)
		if (wr_done)
			wr_addr_reg <= wr_addr;		
			
	 always @(posedge wr_clk)
		if (wr_done)
			wr_data_reg <= wr_temp;
	
	 reg wr_clken;	
	 always @(posedge wr_clk)
		wr_clken <= wr_en;
	
    /******* READ LOGIC *******/
	
	 wire rd_clken;
	 wire [31:0] rd_temp;

	 assign rd_data = rd_temp[rd_ws*rd_data_width +: rd_data_width];

`ifdef BRAM_ENABLE_RD_PADDING
	 assign rd_padding = rd_temp[31-rd_pad_width +: rd_pad_width];
`endif
	 
	 assign rd_clken = rd_ce;

	 /******** BRAM Connection ********/

	 //Instiate main BRAM
	 bram_tdp #(.ADDR(addr_width)) bram
	           (.a_clk(wr_clk),
				  .a_clken(wr_clken),
				  .a_wr(1'b1),
				  .a_addr(wr_addr_reg),
				  .a_din(wr_data_reg),
				  .a_dout(),
				  
				  .b_clk(rd_clk),
				  .b_clken(rd_clken),
				  .b_wr(1'b0),
				  .b_addr(rd_addr),
				  .b_din(),
				  .b_dout(rd_temp));


endmodule

/* Following used to infer BRAM devices.

WARNING: This code will have sim mismatch if read & write address are same. For our eventual FIFO implementation
we don't care, since we will capture first, then dump. If you wish to capture while dumping this may give you
errors if you aren't careful. If so switch to manually instiated BRAM modules.

This example code from http://danstrother.com/2010/09/11/inferring-rams-in-fpgas/ 
*/

module bram_tdp #(
    parameter DATA = 32,
    parameter ADDR = 10
) (
    // Port A
    input   wire                a_clk,
	 input   wire                a_clken,
    input   wire                a_wr,
    input   wire    [ADDR-1:0]  a_addr,
    input   wire    [DATA-1:0]  a_din,
    output  reg     [DATA-1:0]  a_dout,
     
    // Port B
    input   wire                b_clk,
	 input   wire                b_clken,
    input   wire                b_wr,
    input   wire    [ADDR-1:0]  b_addr,
    input   wire    [DATA-1:0]  b_din,
    output  reg     [DATA-1:0]  b_dout
);
 
// Shared memory
reg [DATA-1:0] mem [(2**ADDR)-1:0];
 
// Port A
always @(posedge a_clk) begin
	 if (a_clken) begin
		 a_dout      <= mem[a_addr];
		 if(a_wr) begin
			  a_dout      <= a_din;
			  mem[a_addr] <= a_din;
		 end
	 end
end
 
// Port B
always @(posedge b_clk) begin
	 if (b_clken) begin
		 b_dout      <= mem[b_addr];
		 if(b_wr) begin
			  b_dout      <= b_din;
			  mem[b_addr] <= b_din;
		 end
	 end
end
 
endmodule

`undef BRAM_ENABLE_RD_PADDING
`undef BRAM_ENABLE_WR_PADDING
