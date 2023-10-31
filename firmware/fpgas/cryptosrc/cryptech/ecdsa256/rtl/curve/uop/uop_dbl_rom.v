`timescale 1ns / 1ps

module uop_dbl_rom
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
		
/*  1. */6'd00:	data <= {OPCODE_CMP, UOP_SRC_PZ,    UOP_SRC_ZERO,  UOP_DST_DUMMY, UOP_EXEC_ALWAYS};
/*  2. */6'd01:	data <= {OPCODE_MOV, UOP_SRC_PZ,    UOP_SRC_DUMMY, UOP_DST_T1,    UOP_EXEC_ALWAYS};
			5'd02:	data <= {OPCODE_MUL, UOP_SRC_PZ,    UOP_SRC_T1,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
/*  3. */6'd03:	data <= {OPCODE_SUB, UOP_SRC_PX,    UOP_SRC_T1,    UOP_DST_T2,    UOP_EXEC_ALWAYS};
/*  4. */6'd04:	data <= {OPCODE_ADD, UOP_SRC_PX,    UOP_SRC_T1,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
/*  5. */6'd05:	data <= {OPCODE_MUL, UOP_SRC_T1,    UOP_SRC_T2,    UOP_DST_T2,    UOP_EXEC_ALWAYS};
/*  6. */6'd06:	data <= {OPCODE_ADD, UOP_SRC_T2,    UOP_SRC_T2,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
         6'd07:	data <= {OPCODE_ADD, UOP_SRC_T1,    UOP_SRC_T2,    UOP_DST_T2,    UOP_EXEC_ALWAYS};
/*  7. */6'd08:	data <= {OPCODE_ADD, UOP_SRC_PY,    UOP_SRC_PY,    UOP_DST_RY,    UOP_EXEC_ALWAYS};
/*  8. */6'd09:	data <= {OPCODE_MUL, UOP_SRC_PZ,    UOP_SRC_RY,    UOP_DST_RZ,    UOP_EXEC_ALWAYS};
/*  9. */6'd10:	data <= {OPCODE_MOV, UOP_SRC_RY,    UOP_SRC_DUMMY, UOP_DST_T1,    UOP_EXEC_ALWAYS};
         6'd11:	data <= {OPCODE_MOV, UOP_SRC_RY,    UOP_SRC_DUMMY, UOP_DST_T3,    UOP_EXEC_ALWAYS};
         6'd12:	data <= {OPCODE_MUL, UOP_SRC_T1,    UOP_SRC_T3,    UOP_DST_RY,    UOP_EXEC_ALWAYS};
/* 10. */6'd13:	data <= {OPCODE_MUL, UOP_SRC_PX,    UOP_SRC_RY,    UOP_DST_T3,    UOP_EXEC_ALWAYS};
/* 11. */6'd14:	data <= {OPCODE_MOV, UOP_SRC_RY,    UOP_SRC_DUMMY, UOP_DST_T1,    UOP_EXEC_ALWAYS};
         6'd15:	data <= {OPCODE_MUL, UOP_SRC_RY,    UOP_SRC_T1,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
/* 12. */6'd16:	data <= {OPCODE_MUL, UOP_SRC_T1,    UOP_SRC_DELTA, UOP_DST_RY,    UOP_EXEC_ALWAYS};
/* 13. */6'd17:	data <= {OPCODE_MOV, UOP_SRC_T2,    UOP_SRC_DUMMY, UOP_DST_T1,    UOP_EXEC_ALWAYS};
         6'd18:	data <= {OPCODE_MUL, UOP_SRC_T1,    UOP_SRC_T2,    UOP_DST_RX,    UOP_EXEC_ALWAYS};
/* 14. */6'd19:	data <= {OPCODE_ADD, UOP_SRC_T3,    UOP_SRC_T3,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
/* 15. */6'd20:	data <= {OPCODE_SUB, UOP_SRC_RX,    UOP_SRC_T1,    UOP_DST_RX,    UOP_EXEC_ALWAYS};
/* 16. */6'd21:	data <= {OPCODE_SUB, UOP_SRC_T3,    UOP_SRC_RX,    UOP_DST_T1,    UOP_EXEC_ALWAYS};	
/* 17. */6'd22:	data <= {OPCODE_MUL, UOP_SRC_T1,    UOP_SRC_T2,    UOP_DST_T1,    UOP_EXEC_ALWAYS};
/* 18. */6'd23:	data <= {OPCODE_SUB, UOP_SRC_T1,    UOP_SRC_RY,    UOP_DST_RY,    UOP_EXEC_ALWAYS};

			6'd24:	data <= {OPCODE_MOV, UOP_SRC_ONE,   UOP_SRC_DUMMY, UOP_DST_RX,    UOP_EXEC_PZT1T2_0XX};
			6'd25:	data <= {OPCODE_MOV, UOP_SRC_ONE,   UOP_SRC_DUMMY, UOP_DST_RY,    UOP_EXEC_PZT1T2_0XX};
			6'd26:	data <= {OPCODE_MOV, UOP_SRC_ZERO,  UOP_SRC_DUMMY, UOP_DST_RZ,    UOP_EXEC_PZT1T2_0XX};
			
			default:	data <= {OPCODE_RDY, UOP_SRC_DUMMY, UOP_SRC_DUMMY, UOP_DST_DUMMY};
			
		endcase
		

endmodule
