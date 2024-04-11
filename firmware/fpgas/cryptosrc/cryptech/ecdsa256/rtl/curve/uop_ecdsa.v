localparam	[ 4: 0]	OPCODE_CMP			= 5'b00001;
localparam	[ 4: 0]	OPCODE_MOV			= 5'b00010;
localparam	[ 4: 0]	OPCODE_ADD			= 5'b00100;
localparam	[ 4: 0]	OPCODE_SUB			= 5'b01000;
localparam	[ 4: 0]	OPCODE_MUL			= 5'b10000;
localparam	[ 4: 0]	OPCODE_RDY			= 5'b00000;

localparam	[ 4: 0]	UOP_SRC_PX			= 5'h0_0;
localparam	[ 4: 0]	UOP_SRC_PY			= 5'h0_1;
localparam	[ 4: 0]	UOP_SRC_PZ			= 5'h0_2;

localparam	[ 4: 0]	UOP_SRC_RX			= 5'h0_3;
localparam	[ 4: 0]	UOP_SRC_RY			= 5'h0_4;
localparam	[ 4: 0]	UOP_SRC_RZ			= 5'h0_5;

localparam	[ 4: 0]	UOP_SRC_T1			= 5'h0_6;
localparam	[ 4: 0]	UOP_SRC_T2			= 5'h0_7;
localparam	[ 4: 0]	UOP_SRC_T3			= 5'h0_8;
localparam	[ 4: 0]	UOP_SRC_T4			= 5'h0_9;

localparam	[ 4: 0]	UOP_SRC_ONE			= 5'h0_A;
localparam	[ 4: 0]	UOP_SRC_ZERO		= 5'h0_B;
localparam	[ 4: 0]	UOP_SRC_DELTA		= 5'h0_C;

localparam	[ 4: 0]	UOP_SRC_V			= 5'h0_F;

localparam	[ 4: 0]	UOP_SRC_G_X			= 5'h1_0;
localparam	[ 4: 0]	UOP_SRC_G_Y			= 5'h1_1;

localparam	[ 4: 0]	UOP_SRC_H_X			= 5'h1_2;
localparam	[ 4: 0]	UOP_SRC_H_Y			= 5'h1_3;

localparam	[ 4: 0]	UOP_SRC_DUMMY		= 5'hX_X;

localparam	[ 2: 0]	UOP_DST_RX			= 3'd0;
localparam	[ 2: 0]	UOP_DST_RY			= 3'd1;
localparam	[ 2: 0]	UOP_DST_RZ			= 3'd2;

localparam	[ 2: 0]	UOP_DST_T1			= 3'd3;
localparam	[ 2: 0]	UOP_DST_T2			= 3'd4;
localparam	[ 2: 0]	UOP_DST_T3			= 3'd5;
localparam	[ 2: 0]	UOP_DST_T4			= 3'd6;

localparam	[ 2: 0]	UOP_DST_DUMMY		= 3'dX;

localparam				UOP_EXEC_ALWAYS		= 2'b11;	// R
localparam				UOP_EXEC_PZT1T2_0XX	= 2'b10;	// G
localparam				UOP_EXEC_PZT1T2_100	= 2'b00;	// H
localparam				UOP_EXEC_PZT1T2_101	= 2'b01;	// O

