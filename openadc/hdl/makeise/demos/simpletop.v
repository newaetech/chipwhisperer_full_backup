/* Simple Top Module */
module simpletop(
	input wire clk,
 	input wire reset	
    );
	   
   simplemodule sm(
    .clk(clk),
    .reset(reset)
   ); 
endmodule

