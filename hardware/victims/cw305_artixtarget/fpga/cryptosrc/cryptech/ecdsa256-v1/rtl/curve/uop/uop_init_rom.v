`timescale 1ns / 1ps

module uop_init_rom
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
		
			6'd00:	data <= {OPCODE_MOV, UOP_SRC_ONE,  UOP_SRC_DUMMY, UOP_DST_RX, UOP_EXEC_ALWAYS};
			6'd01:	data <= {OPCODE_MOV, UOP_SRC_ONE,  UOP_SRC_DUMMY, UOP_DST_RY, UOP_EXEC_ALWAYS};
			6'd02:	data <= {OPCODE_MOV, UOP_SRC_ZERO, UOP_SRC_DUMMY, UOP_DST_RZ, UOP_EXEC_ALWAYS};
			
			default:	data <= {OPCODE_RDY, UOP_SRC_DUMMY, UOP_SRC_DUMMY, UOP_DST_DUMMY};
			
		endcase
		

endmodule
