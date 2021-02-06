`timescale 1ns / 1ps
`default_nettype none

//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    16:39:37 07/19/2013 
// Design Name: 
// Module Name:    varwidth_fifo_withpre 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module varwidth_fifo_withpre #(
	 parameter max_samples = 2000, 
	 parameter addr_width = 11, //TODO: calculate this from max_samples
	 
	 parameter wr_data_width = 10,
	 parameter wr_pad_width = 2,
	 parameter wr_addr_width = 32,
	 parameter wr_data_per_addr = 3,
	 
	 parameter rd_data_width = 8,
	 parameter rd_data_per_addr = 4,
	 parameter rd_pad_width = 0
    )
	 (
	 input wire rst,
	 
	 input wire [31:0] number_samples,
	 
	 input wire [wr_data_width-1:0] wr_data,
	 input wire wr_ce,
	 input wire wr_clk,
	 
	 input wire [31:0] wr_circular_depth, //ABSOLUTELY MUST be a multiple of wr_data_per_addr 
	 output wire wr_done,
	 input wire wr_trigger, //set to 1 for a single clock cycle to store in main fifo
	 
	 output wire [rd_data_width-1:0] rd_data,
	 input wire rd_ce,
	 input wire rd_clk,
	 output wire rd_done
	 );
	 	 	 
	 //Goes to '1' when capture done
	 reg capture_done;			 
	 //Goes to '1' when capturing to main buffer (e.g. after trigger)
	 reg capture_in_progress;
	 
	 reg reading_done;
	 //Goes to '1' when reading
	 reg reading_in_progress;
	 
	 
	 wire localrst;
	 
	 assign wr_done = capture_done;
	 assign rd_done = reading_done;
	 
	 //Localrst gets asserted if circ_cnt > max_circ_cnt, which will happen if the user
	 //changes the size of the max_circ_cnt dynamically
	 assign localrst = rst;
	 
	 /***** Circular Buffer Write Logic *****/
	 reg [wr_addr_width-1:0] max_circ_addr;
	 reg [wr_addr_width-1:0] circ_addr;	
	 reg [4:0] circ_ws;
	 wire [31:0] max_circ_cnt;
	 reg [31:0] circ_cnt;
	 
	 assign max_circ_cnt = (wr_circular_depth) ? (wr_circular_depth-1) : 0;
	 
	 always @(posedge wr_clk)
		if (localrst) begin
			circ_cnt <= 0;
		//valid write cycle
		end else if (wr_ce & ~capture_done) begin
			//If haven't counted all the way until top yet
			if (max_circ_cnt != circ_cnt)
				circ_cnt <= circ_cnt + 1;
		end
			
	 
	 always @(posedge wr_clk)
		if (localrst) begin
			circ_ws <= 0;
		end			
		//valid write cycle
		else if (wr_ce & ~capture_done & ~capture_in_progress) begin
			if (circ_ws == (wr_data_per_addr-1))
				circ_ws <= 0;
			else
				circ_ws <= circ_ws + 1;
		end				
		
	  			
	 always @(posedge wr_clk)
		if (localrst) begin
			max_circ_addr <= 0;
			circ_addr <= 0;
		//Address increment requested
		end else if (wr_ce) begin
			if ((circ_ws == (wr_data_per_addr-1)) & ~capture_done & ~capture_in_progress) begin				
				//If we are below the maximum value, just increment
				if (circ_addr != max_circ_addr) begin
					circ_addr <= circ_addr + 1;
				//at maximum value
				end else begin
				//Have we actually used all bytes in circ part of buffer yet?
				//If not the max_circ_addr is NOT yet valid, need to increment
				//it as well
					if (max_circ_cnt != circ_cnt) begin
						max_circ_addr <= max_circ_addr + 1;
						circ_addr <= circ_addr + 1;
					end else begin
						circ_addr <= 0;
					end
				end
			end
		end
	
	/***** Main Buffer Write Logic *****/
	reg [4:0] main_ws;
	reg [wr_addr_width-1:0] main_addr;
	reg [wr_addr_width-1:0] max_main_addr;
	reg [31:0] main_cnt;	
			
	always @(posedge wr_clk)
		if (capture_done | localrst) begin
			capture_in_progress <= 0;
		end else if (wr_ce) begin
			if (wr_trigger)
				capture_in_progress <= 1;
		end
			
	always @(posedge wr_clk)
		if (capture_in_progress) begin
			if (wr_ce) begin
				main_cnt <= main_cnt + 1;
			end
		end else begin
			main_cnt <= circ_cnt;
		end
			
	
	always @(posedge wr_clk)
		if (capture_in_progress || wr_trigger) begin
			if (wr_ce) begin
				if (main_ws == (wr_data_per_addr-1)) begin
					main_ws <= 0;
				end else begin
					main_ws <= main_ws + 1;
				end
			end
		end else begin
			main_ws <= 0;
		end

	always @(posedge wr_clk)
		if (capture_in_progress) begin
			if (wr_ce) begin
				if (main_ws == (wr_data_per_addr-1)) begin
					main_addr <= main_addr + 1;
				end
			end
		end else begin
			main_addr <= max_circ_addr + 1;
		end
		
	always @(posedge wr_clk)
		if (capture_in_progress)
			if (wr_ce)
				max_main_addr <= main_addr;
		
	/***** Generic Logic *****/
	always @(posedge wr_clk)
		if (localrst || reading_done) begin
			capture_done <= 0;
		end else if (wr_ce) begin
			if (main_cnt == number_samples)
				capture_done <= 1;
		end
			
	always @(posedge rd_clk)
		if (localrst || (reading_done == 1) || ((rd_main_addr == max_main_addr)  && (rd_circ_cnt == 0)))
			reading_in_progress <= 0;
		else if (rd_ce)
			reading_in_progress <= 1;
			
	always @(posedge rd_clk)
		if (localrst || capture_in_progress)
			reading_done <= 0;
		else
			if ((rd_circ_cnt == 0) && (rd_main_addr == max_main_addr) && rd_ce)
				reading_done <= 1;
			
	/***** Circular Buffer Read Logic *****/
	reg [31:0] rd_circ_cnt;
	reg [4:0] rd_circ_ws;
	reg [addr_width-1:0] rd_circ_addr;
	
	reg circ_read;
		
	wire circ_done;
	assign circ_done = (rd_circ_cnt <= wr_data_per_addr) & (rd_circ_ws == (rd_data_per_addr-1));
		
	always @(posedge rd_clk)
		if (localrst || (capture_done == 0) || (circ_done) || (main_read))
			circ_read <= 0;
		else if (rd_ce)
			circ_read <= 1;
		
	always @(posedge rd_clk)
		if (localrst) begin
			rd_circ_cnt <= 0;
		end else if (circ_read) begin
			if ((rd_circ_ws == (rd_data_per_addr-1)) & (rd_ce))
				rd_circ_cnt <= (rd_circ_cnt <= wr_data_per_addr) ? 0 : (rd_circ_cnt - wr_data_per_addr);
		end else if (reading_in_progress == 0) begin
			rd_circ_cnt <= circ_cnt;
		end	
	
	
	always @(posedge rd_clk)
		if (circ_read || (rd_ce && capture_done && ~reading_in_progress)) begin
			if (rd_ce)
				rd_circ_ws <= #1 (rd_circ_ws == (rd_data_per_addr-1)) ? 0 : (rd_circ_ws + 1);
		end else begin
			rd_circ_ws <= #1 0;
		end
	
	always @(posedge rd_clk)		
		if (circ_read) begin	
			if ((rd_circ_ws == (rd_data_per_addr-1)) && rd_ce) begin
				rd_circ_addr <= #1 (rd_circ_addr == max_circ_addr) ? 0 : (rd_circ_addr + 1);
			end			
		end else begin
			rd_circ_addr <= #1 (circ_addr == max_circ_addr) ? 0 : (circ_addr + 1);
		end	
	
	/***** Main Buffer Read Logic *****/
	reg [4:0] rd_main_ws;
	reg [addr_width-1:0] rd_main_addr;
	reg main_read;
	
	always @(posedge rd_clk)
		if (localrst || (reading_done == 1) || (rd_main_addr == max_main_addr) || (capture_in_progress))
			main_read <= 0;
		else if (rd_ce && reading_in_progress && circ_done)
			main_read <= 1;
				
	always @(posedge rd_clk)
		if (main_read || (rd_ce && reading_in_progress && ~circ_read)) begin
			if (rd_ce)
				rd_main_ws <= (rd_main_ws == (rd_data_per_addr-1)) ? 0 : (rd_main_ws + 1);
		end else begin
			rd_main_ws <= 0;
		end
		
	always @(posedge rd_clk)
		if (main_read) begin
			if ((rd_main_ws == (rd_data_per_addr-1)) && rd_ce) begin
				rd_main_addr <= rd_main_addr + 1;
			end
		end else begin
			rd_main_addr <= max_circ_addr + 1;
		end
		
	
	/***** BRAM Connections *****/	
	reg [wr_data_width-1:0] bram_wr_data;
	wire [addr_width-1:0] bram_wr_addr;
	reg [addr_width-1:0] bram_wr_addr_reg;
	reg [4:0] bram_wr_ws;
	reg bram_wr_ce;
	wire [rd_data_width-1:0] bram_rd_data;
	wire [addr_width-1:0] bram_rd_addr;
	wire [4:0] bram_rd_ws;
	wire bram_rd_ce;
	
	always @(posedge wr_clk)
		bram_wr_ce <= wr_ce & ~capture_done;
	
	//if capture_in_progress == 1 we select the 'main' ws & addr, otherwise 'circ'
	always @(posedge wr_clk)
		bram_wr_ws <= (capture_in_progress) ? main_ws : circ_ws;
	
	always @(posedge wr_clk)
		//bram_wr_addr_reg <= circ_addr;
		bram_wr_addr_reg <= (capture_in_progress || wr_trigger) ? main_addr : circ_addr;
		
	always @(posedge wr_clk)
		bram_wr_data = wr_data;
	
	//assign bram_wr_addr = (capture_in_progress) ? main_addr : bram_wr_addr_reg;
	assign bram_wr_addr = bram_wr_addr_reg;
	
	assign rd_data = bram_rd_data;
	assign bram_rd_ce = rd_ce;
	assign bram_rd_addr = (main_read) ? rd_main_addr : rd_circ_addr;
	assign bram_rd_ws = (main_read) ? rd_main_ws : rd_circ_ws;	
	
	dram_varwidth #(.addr_width(addr_width),
						 .wr_data_width(wr_data_width),
						 .wr_pad_width(wr_pad_width),
						 .wr_data_per_addr(wr_data_per_addr),
						 .rd_data_width(rd_data_width),
						 .rd_data_per_addr(rd_data_per_addr),
						 .rd_pad_width(rd_pad_width))
	bram 
	(
		.wr_data(bram_wr_data), 
		.wr_addr(bram_wr_addr), 
		.wr_ws(bram_wr_ws), 
		.wr_ce(bram_wr_ce), 
		.wr_clk(wr_clk),
		
		.rd_data(bram_rd_data),
		.rd_addr(bram_rd_addr),
		.rd_ws(bram_rd_ws),
		.rd_ce(bram_rd_ce),
		.rd_clk(rd_clk)
	);

	
endmodule
