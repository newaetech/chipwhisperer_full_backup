//------------------------------------------------------------------------------
//
// tb_modular_mover.v
// -----------------------------------------------------------------------------
// Testbench for multi-word data mover.
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

module tb_mw_mover;


		//
		// Test Vectors
		//	
	localparam	[255:0]	X_1	= 256'h1ddbd0769df27bab1e234019dad09dccce1e87e2193b417ffa1a3465d7439ecd;
	localparam	[255:0]	X_2	= 256'h1f67cdc34bac91a072945d212f0a03442fc4855788583ecb7b2e375ad3848210;
	localparam	[255:0]	X_3	= 256'hff563f653b1392a6fa6b0295a280f7a904a11e22d8ae468e220301d8ac232fcf;
	localparam	[255:0]	X_4	= 256'hf6f53c4b57b25453b68e923fb118e4f753d74af01fc58476dd15a80933453899;
	
	
		//
		// Core Parameters
		//
	localparam	WORD_COUNTER_WIDTH	=  3;
	localparam	OPERAND_NUM_WORDS		=  8;
	

		//
		// Clock (100 MHz)
		//
	reg clk = 1'b0;
	always #5 clk = ~clk;

	
		//
		// Inputs, Outputs
		//
	reg				rst_n;
	reg				ena;
	wire				rdy;
	
	
		//
		// Buffers (X, Y)
		//
	wire	[WORD_COUNTER_WIDTH-1:0]	core_x_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_y_addr;
	wire										core_y_wren;
	
	wire	[                32-1:0]	core_x_data;
	wire	[                32-1:0]	core_y_data;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_x_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_y_addr;
	reg										tb_x_wren;
	
	reg	[                32-1:0]	tb_x_data;
	wire	[                32-1:0]	tb_y_data;
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_x
	(
		.clk		(clk),

		.a_addr	(tb_x_addr),
		.a_wr		(tb_x_wren),
		.a_in		(tb_x_data),
		.a_out	(),

		.b_addr	(core_x_addr),
		.b_out	(core_x_data)
	);
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_d
	(
		.clk		(clk),

		.a_addr	(core_y_addr),
		.a_wr		(core_y_wren),
		.a_in		(core_y_data),
		.a_out	(),

		.b_addr	(tb_y_addr),
		.b_out	(tb_y_data)
	);
	
	
		//
		// UUT
		//
	mw_mover #
	(
		.WORD_COUNTER_WIDTH	(WORD_COUNTER_WIDTH),
		.OPERAND_NUM_WORDS	(OPERAND_NUM_WORDS)
	)
	uut
	(
		.clk			(clk),
		.rst_n		(rst_n),
		
		.ena			(ena),
		.rdy			(rdy),
		
		.x_addr		(core_x_addr),
		.y_addr		(core_y_addr),
		.y_wren		(core_y_wren),
		
		.x_din		(core_x_data),
		.y_dout		(core_y_data)
	);
		
		
		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		
			/* initialize control inputs */
		rst_n			= 0;
		ena			= 0;
		
		tb_x_wren	= 0;
		
			/* wait for some time */
		#200;
		
			/* de-assert reset */
		rst_n		= 1;
		
			/* wait for some time */
		#100;		
		
			/* run tests */
		test_modular_mover(X_1);
		test_modular_mover(X_2);
		test_modular_mover(X_3);
		test_modular_mover(X_4);
		
			/* print result */
		if (ok)	$display("tb_modular_mover: SUCCESS");
		else		$display("tb_modular_mover: FAILURE");
		//
		$finish;
		//
	end
	
	
		//
		// Test Task
		//	
	reg	[255:0]	y;
	reg				y_ok;

	integer			w;
	
	reg	[255:0]	x_shreg;
	reg	[255:0]	y_shreg;
	
	task test_modular_mover;
	
		input	[255:0]	x;
				
		begin
		
				/* start filling memories */
			tb_x_wren	= 1;
			
				/* initialize shift registers */
			x_shreg = x;
			
				/* write all the words */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set addresses */
				tb_x_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* set data words */
				tb_x_data	= x_shreg[31:0];
				
					/* shift inputs */
				x_shreg = {{32{1'bX}}, x_shreg[255:32]};
				
					/* wait for 1 clock tick */
				#10;
				
			end
			
				/* wipe addresses */
			tb_x_addr	= {WORD_COUNTER_WIDTH{1'bX}};
			
				/* wipe data words */
			tb_x_data	= {32{1'bX}};
			
				/* stop filling memories */
			tb_x_wren	= 0;
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;
			
				/* read result */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set address */
				tb_y_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* wait for 1 clock tick */
				#10;
				
					/* store data word */
				y_shreg = {tb_y_data, y_shreg[255:32]};

			end				
			
				/* compare */
			y_ok = (y_shreg == x);

				/* display results */
			$display("test_modular_mover(): %s", y_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && y_ok;
		
		end
		
	endtask
	
      
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
