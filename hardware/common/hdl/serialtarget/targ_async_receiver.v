// RS-232 RX module
// (c) fpga4fun.com KNJN LLC - 2003, 2004, 2005, 2006

`include "includes.v"

//`define CHIPSCOPE

module targ_async_receiver(clk, RxD, parity_even, two_stopbits, RxD_data_ready, RxD_data_error, RxD_data, RxD_endofpacket, RxD_idle, RxD_Baud8GeneratorInc);
input clk, RxD;
input parity_even, two_stopbits;
output RxD_data_ready;  // onc clock pulse when RxD_data is valid
output RxD_data_error;
output [7:0] RxD_data;

// Baud generator (we use 8 times oversampling)
//parameter ClkFrequency = `UART_CLK; // 40MHz
//parameter Baud = `TARG_UART_BAUD;
//parameter Baud8 = Baud*8;
// = ((Baud8<<(Baud8GeneratorAccWidth-7))+(ClkFrequency>>8))/(ClkFrequency>>7);
parameter Baud8GeneratorAccWidth = 16;
input [Baud8GeneratorAccWidth-1:0] RxD_Baud8GeneratorInc; 

wire [Baud8GeneratorAccWidth-1:0] Baud8GeneratorInc; 
assign Baud8GeneratorInc = RxD_Baud8GeneratorInc;


// We also detect if a gap occurs in the received stream of characters
// That can be useful if multiple characters are sent in burst
//  so that multiple characters can be treated as a "packet"
output RxD_endofpacket;  // one clock pulse, when no more data is received (RxD_idle is going high)
output RxD_idle;  // no data is being received

reg [Baud8GeneratorAccWidth:0] Baud8GeneratorAcc;
always @(posedge clk) Baud8GeneratorAcc <= Baud8GeneratorAcc[Baud8GeneratorAccWidth-1:0] + Baud8GeneratorInc;
wire Baud8Tick = Baud8GeneratorAcc[Baud8GeneratorAccWidth];

 `ifdef CHIPSCOPE
   wire [127:0] cs_data;   
   wire [35:0]  chipscope_control;
  coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 

   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );  
`endif

////////////////////////////
reg [1:0] RxD_sync_inv;
always @(posedge clk) if(Baud8Tick) RxD_sync_inv <= {RxD_sync_inv[0], ~RxD};
// we invert RxD, so that the idle becomes "0", to prevent a phantom character to be received at startup

reg [1:0] RxD_cnt_inv;
reg RxD_bit_inv;

always @(posedge clk)
if(Baud8Tick)
begin
	if( RxD_sync_inv[1] && RxD_cnt_inv!=2'b11) RxD_cnt_inv <= RxD_cnt_inv + 2'h1;
	else 
	if(~RxD_sync_inv[1] && RxD_cnt_inv!=2'b00) RxD_cnt_inv <= RxD_cnt_inv - 2'h1;

	if(RxD_cnt_inv==2'b00) RxD_bit_inv <= 1'b0;
	else
	if(RxD_cnt_inv==2'b11) RxD_bit_inv <= 1'b1;
end

reg [3:0] state;
reg [3:0] bit_spacing;

// "next_bit" controls when the data sampling occurs
// depending on how noisy the RxD is, different values might work better
// with a clean connection, values from 8 to 11 work
wire next_bit = (bit_spacing==4'd10);

always @(posedge clk)
if(state==0)
	bit_spacing <= 4'b0000;
else
if(Baud8Tick)
	bit_spacing <= {bit_spacing[2:0] + 4'b0001} | {bit_spacing[3], 3'b000};

always @(posedge clk)
if(Baud8Tick)
case(state)
	4'b0000: if(RxD_bit_inv) state <= 4'b1000;  // start bit found?
	4'b1000: if(next_bit) state <= 4'b1001;  // bit 0
	4'b1001: if(next_bit) state <= 4'b1010;  // bit 1
	4'b1010: if(next_bit) state <= 4'b1011;  // bit 2
	4'b1011: if(next_bit) state <= 4'b1100;  // bit 3
	4'b1100: if(next_bit) state <= 4'b1101;  // bit 4
	4'b1101: if(next_bit) state <= 4'b1110;  // bit 5
	4'b1110: if(next_bit) state <= 4'b1111;  // bit 6
	4'b1111: if(next_bit) state <= (parity_even) ? 4'b0011 : {(two_stopbits) ? 4'b0010 : 4'b0001};  // bit 7
	4'b0011: if(next_bit) state <= (two_stopbits) ? 4'b0010 : 4'b0001;  // parity bit
	4'b0010: if(next_bit) state <= 4'b0001;  // stop bit 1
	4'b0001: if(next_bit) state <= 4'b0000;  // stop bit 2
	default: state <= 4'b0000;
endcase

reg [7:0] RxD_data;
always @(posedge clk)
if(Baud8Tick && next_bit && state[3]) RxD_data <= {~RxD_bit_inv, RxD_data[7:1]};

reg RxD_data_ready, RxD_data_error;
always @(posedge clk)
begin
	RxD_data_ready <= (Baud8Tick && next_bit && state==4'b0001 && ~RxD_bit_inv);  // ready only if the stop bit is received
	RxD_data_error <= (Baud8Tick && next_bit && state==4'b0001 &&  RxD_bit_inv);  // error if the stop bit is not received
end

reg [4:0] gap_count;
always @(posedge clk) if (state!=0) gap_count<=5'h00; else if(Baud8Tick & ~gap_count[4]) gap_count <= gap_count + 5'h01;
assign RxD_idle = gap_count[4];
reg RxD_endofpacket; always @(posedge clk) RxD_endofpacket <= Baud8Tick & (gap_count==5'h0F);


`ifdef CHIPSCOPE
	 assign cs_data[0] = RxD;
	 assign cs_data[1] = RxD_data_error;
	 assign cs_data[2] = RxD_data_ready;
	 assign cs_data[3] = Baud8Tick;
	 assign cs_data[4] = next_bit;
	 assign cs_data[12:5] = RxD_data;
	 assign cs_data[16:13] = state;
	 assign cs_data[20:17] = bit_spacing;
`endif

endmodule

//Don't forget to clear the define!
`undef CHIPSCOPE