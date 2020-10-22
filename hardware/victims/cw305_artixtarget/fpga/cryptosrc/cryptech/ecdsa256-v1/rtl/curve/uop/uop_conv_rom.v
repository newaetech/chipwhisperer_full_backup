`timescale 1ns / 1ps

module uop_conv_rom
	(
		input		wire				clk,
		input		wire	[ 5: 0]	addr,
		output	reg	[19: 0]	data
    );


		//
		// Microcode
		//
`include "uop_ecdsa.v"


   	//
		// Doubling Microprogram
		//
   always @(posedge clk)

		case (addr)
		
			6'd00:	data <= {OPCODE_CMP, UOP_SRC_PZ,   UOP_SRC_ZERO,  UOP_DST_DUMMY, UOP_EXEC_ALWAYS};
			6'd01:	data <= {OPCODE_MOV, UOP_SRC_V,    UOP_SRC_DUMMY, UOP_DST_T1,    UOP_EXEC_ALWAYS};
			6'd02:	data <= {OPCODE_MUL, UOP_SRC_V,    UOP_SRC_T1,    UOP_DST_T2,    UOP_EXEC_ALWAYS};
			6'd03:	data <= {OPCODE_MUL, UOP_SRC_V,    UOP_SRC_T2,    UOP_DST_T3,    UOP_EXEC_ALWAYS};
			6'd04:	data <= {OPCODE_MUL, UOP_SRC_PX,   UOP_SRC_T2,    UOP_DST_RX,    UOP_EXEC_ALWAYS};
			6'd05:	data <= {OPCODE_MUL, UOP_SRC_PY,   UOP_SRC_T3,    UOP_DST_RY,    UOP_EXEC_ALWAYS};
			6'd06:	data <= {OPCODE_MOV, UOP_SRC_ZERO, UOP_SRC_DUMMY, UOP_DST_RX,    UOP_EXEC_PZT1T2_0XX};
			6'd07:	data <= {OPCODE_MOV, UOP_SRC_ZERO, UOP_SRC_DUMMY, UOP_DST_RY,    UOP_EXEC_PZT1T2_0XX};
			
			default:	data <= {OPCODE_RDY, UOP_SRC_DUMMY, UOP_SRC_DUMMY, UOP_DST_DUMMY};
			
		endcase
		

endmodule
