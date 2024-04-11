//------------------------------------------------------------------------------
//
// tb_mw_comparator.v
// -----------------------------------------------------------------------------
// Testbench for multi-word comparator.
//
// Authors: Pavel Shatov
//
// Copyright (c) 2016, NORDUnet A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
`timescale 1ns / 1ps
//------------------------------------------------------------------------------

module tb_mw_comparator;


		//
		// Test Vectors
		//	
	localparam	[383:0]	A_0	= 384'hBDC7B53C_616B13B5_77622510_75BA95FC_475D568B_79E730D4_18A9143C_18905F76;
	
	localparam	[383:0]	A_1	= 384'hBDC7B53C_616B13B5_77622510_75BA95FC_475D568B_79E730D4_18A9143C_18905F75;
	localparam	[383:0]	A_2	= 384'hBDC7B53C_616B13B5_77622510_75BA95FC_475D568B_79E730D4_18A9143C_18905F77;
	
	localparam	[383:0]	A_3	= 384'hBDC7B53C_616B13B5_77622510_75BA95FC_375D568B_79E730D4_18A9143C_18905F76;
	localparam	[383:0]	A_4	= 384'hBDC7B53C_616B13B5_77622510_75BA95FC_575D568B_79E730D4_18A9143C_18905F76;
	
	localparam	[383:0]	A_5	= 384'hBDC7B53C_616B13B5_77622510_75BA95FB_475D568B_79E730D4_18A9143C_18905F76;
	localparam	[383:0]	A_6	= 384'hBDC7B53C_616B13B5_77622510_75BA95FD_475D568B_79E730D4_18A9143C_18905F76;
	
	localparam	[383:0]	A_7	= 384'hADC7B53C_616B13B5_77622510_75BA95FC_475D568B_79E730D4_18A9143C_18905F76;
	localparam	[383:0]	A_8	= 384'hCDC7B53C_616B13B5_77622510_75BA95FC_475D568B_79E730D4_18A9143C_18905F76;
		
	localparam	[383:0]	B_0	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C0_AD784F98_850046D4_10DDD64D_F6BB32E5;
	
	localparam	[383:0]	B_1	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C0_AD784F98_850046D4_10DDD64D_F6BB32E4;
	localparam	[383:0]	B_2	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C0_AD784F98_850046D4_10DDD64D_F6BB32E6;
	
	localparam	[383:0]	B_3	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C0_9D784F98_850046D4_10DDD64D_F6BB32E5;
	localparam	[383:0]	B_4	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C0_BD784F98_850046D4_10DDD64D_F6BB32E5;
	
	localparam	[383:0]	B_5	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3BF_AD784F98_850046D4_10DDD64D_F6BB32E5;
	localparam	[383:0]	B_6	= 384'h348A6D1F_7C66D21E_8D1490D9_AA6AE3C1_AD784F98_850046D4_10DDD64D_F6BB32E5;
	
	localparam	[383:0]	B_7	= 384'h248A6D1F_7C66D21E_8D1490D9_AA6AE3C0_AD784F98_850046D4_10DDD64D_F6BB32E5;
	localparam	[383:0]	B_8	= 384'h448A6D1F_7C66D21E_8D1490D9_AA6AE3C0_AD784F98_850046D4_10DDD64D_F6BB32E5;
	
	
		//
		// Core Parameters
		//
	localparam	WORD_COUNTER_WIDTH	=  3;
	parameter	OPERAND_NUM_WORDS		=  8;
	

		//
		// Clock (100 MHz)
		//
	reg clk = 1'b0;
	always #5 clk = ~clk;

	
		//
		// Inputs, Outputs
		//
	reg	rst_n;
	reg	ena;
	wire	rdy;
	
	wire	core_cmp_l;
	wire	core_cmp_e;
	wire	core_cmp_g;


		//
		// Buffers (X, Y)
		//
	wire	[WORD_COUNTER_WIDTH-1:0]	core_xy_addr;
	
	wire	[                32-1:0]	core_x_data;
	wire	[                32-1:0]	core_y_data;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_xy_addr;
	reg										tb_xy_wren;
	
	reg	[                32-1:0]	tb_x_data;
	reg	[                32-1:0]	tb_y_data;
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_x
	(
		.clk		(clk),

		.a_addr	(tb_xy_addr),
		.a_wr		(tb_xy_wren),
		.a_in		(tb_x_data),
		.a_out	(),

		.b_addr	(core_xy_addr),
		.b_out	(core_x_data)
	);
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_y
	(
		.clk		(clk),

		.a_addr	(tb_xy_addr),
		.a_wr		(tb_xy_wren),
		.a_in		(tb_y_data),
		.a_out	(),

		.b_addr	(core_xy_addr),
		.b_out	(core_y_data)
	);
	
	
		//
		// UUT
		//
	mw_comparator #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)
	)
	uut
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(ena),
		.rdy		(rdy),
		
		.xy_addr	(core_xy_addr),
		.x_din	(core_x_data),
		.y_din	(core_y_data),
		
		.cmp_l	(core_cmp_l),
		.cmp_e	(core_cmp_e),
		.cmp_g	(core_cmp_g)
	);
		
		
		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		
			/* initialize control inputs */
		rst_n		= 0;
		ena		= 0;
		
		tb_xy_wren	= 0;
		
			/* wait for some time */
		#200;
		
			/* de-assert reset */
		rst_n		= 1;
		
			/* wait for some time */
		#100;		
		
			/* run tests */
		test_mw_comparator(A_0, A_0);
		
		test_mw_comparator(A_0, A_1);
		test_mw_comparator(A_0, A_2);
		test_mw_comparator(A_0, A_3);
		test_mw_comparator(A_0, A_4);
		test_mw_comparator(A_0, A_5);
		test_mw_comparator(A_0, A_6);
		test_mw_comparator(A_0, A_7);
		test_mw_comparator(A_0, A_8);
		
		test_mw_comparator(B_0, B_0);
		
		test_mw_comparator(B_0, B_1);
		test_mw_comparator(B_0, B_2);
		test_mw_comparator(B_0, B_3);
		test_mw_comparator(B_0, B_4);
		test_mw_comparator(B_0, B_5);
		test_mw_comparator(B_0, B_6);
		test_mw_comparator(B_0, B_7);
		test_mw_comparator(B_0, B_8);		
		
			/* print result */
		if (ok)	$display("tb_mw_comparator: SUCCESS");
		else		$display("tb_mw_comparator: FAILURE");
		//
		$finish;
		//
	end
	
	
		//
		// Test Task
		//	
	reg	cmp_l;
	reg	cmp_e;
	reg	cmp_g;
	reg	cmp_ok;

	integer w;

	task test_mw_comparator;
	
		input	[255:0]	x;
		input	[255:0]	y;
		
		reg	[255:0]	x_shreg;
		reg	[255:0]	y_shreg;
		
		begin
		
				/* start filling memories */
			tb_xy_wren	= 1;
			
				/* initialize shift registers */
			x_shreg = x;
			y_shreg = y;
			
				/* write all the words */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set addresses */
				tb_xy_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* set data words */
				tb_x_data	= x_shreg[31:0];
				tb_y_data	= y_shreg[31:0];
				
					/* shift inputs */
				x_shreg = {{32{1'bX}}, x_shreg[255:32]};
				y_shreg = {{32{1'bX}}, y_shreg[255:32]};
				
					/* wait for 1 clock tick */
				#10;
				
			end
			
				/* wipe addresses */
			tb_xy_addr	= {WORD_COUNTER_WIDTH{1'bX}};
			
				/* wipe data words */
			tb_x_data	= {32{1'bX}};
			tb_y_data	= {32{1'bX}};
			
				/* stop filling memories */
			tb_xy_wren	= 0;
						
				/* calculate reference values */
			cmp_l = (x <  y) ? 1 : 0;
			cmp_e = (x == y) ? 1 : 0;
			cmp_g = (x >  y) ? 1 : 0;
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;
			
				/* compare */
			cmp_ok = (cmp_l == core_cmp_l) && (cmp_e == core_cmp_e) && (cmp_g == core_cmp_g);

				/* display results */
			$display("test_mw_comparator(): %s", cmp_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && cmp_ok;
		
		end
		
	endtask
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
