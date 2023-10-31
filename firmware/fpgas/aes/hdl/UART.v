// RS-232 RX module
// (c) fpga4fun.com KNJN LLC - 2003, 2004, 2005, 2006

`define UART_CLK 7372800;
`define UART_BAUD 38400;

module async_receiver(clk, RxD, RxD_data_ready, RxD_data_error, RxD_data, RxD_endofpacket, RxD_idle);
input clk, RxD;
output RxD_data_ready, RxD_data_error;  // onc clock pulse when RxD_data is valid
output [7:0] RxD_data;

parameter ClkFrequency = `UART_CLK;
parameter Baud = `UART_BAUD;

// We also detect if a gap occurs in the received stream of characters
// That can be useful if multiple characters are sent in burst
//  so that multiple characters can be treated as a "packet"
output RxD_endofpacket;  // one clock pulse, when no more data is received (RxD_idle is going high)
output RxD_idle;  // no data is being received

// Baud generator (we use 8 times oversampling)
parameter Baud8 = Baud*8;
parameter Baud8GeneratorAccWidth = 16;
wire [Baud8GeneratorAccWidth:0] Baud8GeneratorInc = ((Baud8<<(Baud8GeneratorAccWidth-7))+(ClkFrequency>>8))/(ClkFrequency>>7);
reg [Baud8GeneratorAccWidth:0] Baud8GeneratorAcc;
always @(posedge clk) Baud8GeneratorAcc <= Baud8GeneratorAcc[Baud8GeneratorAccWidth-1:0] + Baud8GeneratorInc;
wire Baud8Tick = Baud8GeneratorAcc[Baud8GeneratorAccWidth];

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
	4'b1111: if(next_bit) state <= 4'b0001;  // bit 7
	4'b0001: if(next_bit) state <= 4'b0000;  // stop bit
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

endmodule


// RS-232 TX module
// (c) fpga4fun.com KNJN LLC - 2003, 2004, 2005, 2006

//`define DEBUG   // in DEBUG mode, we output one bit per clock cycle (useful for faster simulations)

module async_transmitter(clk, TxD_start, TxD_data, TxD, TxD_busy);
input clk, TxD_start;
input [7:0] TxD_data;
output TxD, TxD_busy;

parameter ClkFrequency = `UART_CLK;	// 40MHz
parameter Baud = `UART_BAUD;
parameter RegisterInputData = 1;	// in RegisterInputData mode, the input doesn't have to stay valid while the character is been transmitted

// Baud generator
parameter BaudGeneratorAccWidth = 16;
reg [BaudGeneratorAccWidth:0] BaudGeneratorAcc;
wire [BaudGeneratorAccWidth:0] BaudGeneratorInc = ((Baud<<(BaudGeneratorAccWidth-4))+(ClkFrequency>>5))/(ClkFrequency>>4);


wire BaudTick = BaudGeneratorAcc[BaudGeneratorAccWidth];
wire TxD_busy;
always @(posedge clk) if(TxD_busy) BaudGeneratorAcc <= BaudGeneratorAcc[BaudGeneratorAccWidth-1:0] + BaudGeneratorInc;

// Transmitter state machine
reg [3:0] state;
wire TxD_ready = (state==0);
assign TxD_busy = ~TxD_ready;

reg [7:0] TxD_dataReg;
always @(posedge clk) if(TxD_ready & TxD_start) TxD_dataReg <= TxD_data;
wire [7:0] TxD_dataD = RegisterInputData ? TxD_dataReg : TxD_data;

always @(posedge clk)
case(state)
	4'b0000: if(TxD_start) state <= 4'b0001;
	4'b0001: if(BaudTick) state <= 4'b0100;
	4'b0100: if(BaudTick) state <= 4'b1000;  // start
	4'b1000: if(BaudTick) state <= 4'b1001;  // bit 0
	4'b1001: if(BaudTick) state <= 4'b1010;  // bit 1
	4'b1010: if(BaudTick) state <= 4'b1011;  // bit 2
	4'b1011: if(BaudTick) state <= 4'b1100;  // bit 3
	4'b1100: if(BaudTick) state <= 4'b1101;  // bit 4
	4'b1101: if(BaudTick) state <= 4'b1110;  // bit 5
	4'b1110: if(BaudTick) state <= 4'b1111;  // bit 6
	4'b1111: if(BaudTick) state <= 4'b0010;  // bit 7
	4'b0010: if(BaudTick) state <= 4'b0011;  // stop1
	4'b0011: if(BaudTick) state <= 4'b0000;  // stop2
	default: if(BaudTick) state <= 4'b0000;
endcase

// Output mux
reg muxbit;
always @( * )
case(state[2:0])
	3'd0: muxbit <= TxD_dataD[0];
	3'd1: muxbit <= TxD_dataD[1];
	3'd2: muxbit <= TxD_dataD[2];
	3'd3: muxbit <= TxD_dataD[3];
	3'd4: muxbit <= TxD_dataD[4];
	3'd5: muxbit <= TxD_dataD[5];
	3'd6: muxbit <= TxD_dataD[6];
	3'd7: muxbit <= TxD_dataD[7];
endcase

// Put together the start, data and stop bits
reg TxD;
always @(posedge clk) TxD <= (state<4) | (state[3] & muxbit);  // register the output to make it glitch free

endmodule