`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   20:02:26 06/12/2013
// Design Name:   trigger_system
// Module Name:   C:/E/Documents/fpga/trigger_digital_signal/trigger_system/trigger_system_tb.v
// Project Name:  trigger_system
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: trigger_system
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module trigger_system_tb;

	// Inputs
	reg clk;
	reg rst;
	reg mon_line;
	reg [17:0] clkdivider;
	reg state_prog_en;
	reg [8:0] state_prog_addr;
	reg state_prog_wr;
	reg [17:0] state_prog_data;

	// Outputs
	wire trig_out;
	
	always begin
		clk = !clk;
		#5;
	end

	// Instantiate the Unit Under Test (UUT)
	/*
	trigger_system uut (
		.clk(clk), 
		.rst(rst), 
		.mon_line(mon_line), 
		.trig_out(trig_out), 
		.clkdivider(clkdivider), 
		.state_prog_en(state_prog_en), 
		.state_prog_addr(state_prog_addr), 
		.state_prog_wr(state_prog_wr), 
		.state_prog_data(state_prog_data)
	);
	*/

		
	lx9_trigger_system uut (
		.tbclk(clk), 
		.rst(rst), 
		//.mon_line(mon_line), 
		.trig_out(trig_out), 
		.clkdivider(clkdivider), 
		.state_prog_en(state_prog_en), 
		.state_prog_addr(state_prog_addr), 
		.state_prog_wr(state_prog_wr), 
		.state_prog_data(state_prog_data)
	);
	
	

	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;
		mon_line = 0;
		clkdivider = 695;
		state_prog_en = 0;
		state_prog_addr = 0;
		state_prog_wr = 0;
		state_prog_data = 0;

		// Wait 100 ns for global reset to finish
		#100;
		
		rst = 0;
		
		#50;
		
		write_pattern();
		
		#500;
        
		// Add stimulus here
		
		//simulate_ioline();
		
		@(posedge trig_out);
		
		#100;
		
		$finish();
		
	end


task write_one;
	input linestate;
	input [7:0] low;
	input [8:0] high;
	input [5:0] addr;
	begin
	
	@(posedge clk);
	state_prog_data[7:0] = low;
	state_prog_data[16:8] = high;
	state_prog_data[17] = linestate;
	state_prog_addr = addr;
	state_prog_wr = 1;
	
	@(posedge clk);
	state_prog_wr = 0;
		
	end
endtask


task write_pattern;	
	integer file, c, r;
	reg state;
	reg [7:0] low;
	reg [8:0] high;	
	reg [5:0] addr;
	
	begin
		
	@(posedge clk);
	state_prog_en = 1;
	@(posedge clk);
	
	file = $fopen("trigger_test_ram.txt", "r");
	
   if (file == 0) begin // If error opening file
        $display("Error opening file!!!");
		  $finish();
	 end
	 
	 addr = 0;

    c = $fgetc(file);
    while (c != 32'hFFFF_FFFF) begin			
            // Push the character back to the file then read the next time
            r = $ungetc(c, file);
            r = $fscanf(file,"%d %d %d\n", state, low, high);		
				write_one(state, low, high, addr);            
				addr = addr + 1;
            c = $fgetc(file);
        end // while not EOF

	$fclose(file);

	@(posedge clk);
	state_prog_en = 0;
	@(posedge clk);
	
	end
	 
endtask
      
		
task ioline_doone;
	input state;
	input [8:0] delay;
	begin
	mon_line = state;	
	repeat (delay * (clkdivider * 2))
      @ (posedge clk) ;	
	end
endtask
		

task simulate_ioline;	
	integer file, c, r;
	reg state;
	reg [8:0] delaytime;
	
	begin
			
	file = $fopen("trigger_test_signal.txt", "r");
	
   if (file == 0) begin // If error opening file
        $display("Error opening file!!!");
		  $finish();
	 end
	 
    c = $fgetc(file);
    while (c != 32'hFFFF_FFFF) begin			
            // Push the character back to the file then read the next time
            r = $ungetc(c, file);
            r = $fscanf(file,"%d %d\n", state, delaytime);				
				$display("%d %d", state, delaytime);
				ioline_doone(state, delaytime);            
            c = $fgetc(file);
        end // while not EOF

	$fclose(file);
	
	end
	 
endtask
		
endmodule




