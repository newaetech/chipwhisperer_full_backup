//------------------------------------------------------------------------------
//
// tb_modular_subtractor_256.v
// -----------------------------------------------------------------------------
// Testbench for modular multi-word subtractor.
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

module tb_modular_subtractor_256;


		//
		// Test Vectors
		//	
	localparam	[255:0]	N		= 256'hffffffff00000001000000000000000000000000ffffffffffffffffffffffff;
		
	localparam	[255:0]	X_1	= 256'h1ddbd0769df27bab1e234019dad09dccce1e87e2193b417ffa1a3465d7439ecd;
	localparam	[255:0]	Y_1	= 256'h1f67cdc34bac91a072945d212f0a03442fc4855788583ecb7b2e375ad3848210;
	
	localparam	[255:0]	X_2	= 256'hff563f653b1392a6fa6b0295a280f7a904a11e22d8ae468e220301d8ac232fcf;
	localparam	[255:0]	Y_2	= 256'hf6f53c4b57b25453b68e923fb118e4f753d74af01fc58476dd15a80933453899;
	
	
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
		// Buffers (X, Y, N)
		//
	wire	[WORD_COUNTER_WIDTH-1:0]	core_xy_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_n_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_d_addr;
	wire										core_d_wren;
	
	wire	[                  31:0]	core_x_data;
	wire	[                  31:0]	core_y_data;
	wire	[                  31:0]	core_n_data;
	wire	[                  31:0]	core_d_data;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_xyn_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_d_addr;
	reg										tb_xyn_wren;
	
	reg	[                  31:0]	tb_x_data;
	reg	[                  31:0]	tb_y_data;
	reg	[                  31:0]	tb_n_data;
	wire	[                  31:0]	tb_d_data;
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_x
	(
		.clk		(clk),

		.a_addr	(tb_xyn_addr),
		.a_wr		(tb_xyn_wren),
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

		.a_addr	(tb_xyn_addr),
		.a_wr		(tb_xyn_wren),
		.a_in		(tb_y_data),
		.a_out	(),

		.b_addr	(core_xy_addr),
		.b_out	(core_y_data)
	);
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_n
	(
		.clk		(clk),

		.a_addr	(tb_xyn_addr),
		.a_wr		(tb_xyn_wren),
		.a_in		(tb_n_data),
		.a_out	(),

		.b_addr	(core_n_addr),
		.b_out	(core_n_data)
	);
	
	bram_1rw_1ro_readfirst #
	(
		.MEM_WIDTH			(32),
		.MEM_ADDR_BITS		(WORD_COUNTER_WIDTH)
	)
	bram_s
	(
		.clk		(clk),

		.a_addr	(core_d_addr),
		.a_wr		(core_d_wren),
		.a_in		(core_d_data),
		.a_out	(),

		.b_addr	(tb_d_addr),
		.b_out	(tb_d_data)
	);
	
	
		//
		// UUT
		//
	modular_subtractor #
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
		
		.ab_addr		(core_xy_addr),
		.n_addr		(core_n_addr),
		.d_addr		(core_d_addr),
		.d_wren		(core_d_wren),
		
		.a_din		(core_x_data),
		.b_din		(core_y_data),
		.n_din		(core_n_data),
		.d_dout		(core_d_data)
	);
		
		
		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		
			/* initialize control inputs */
		rst_n			= 0;
		ena			= 0;
		
		tb_xyn_wren	= 0;
		
			/* wait for some time */
		#200;
		
			/* de-assert reset */
		rst_n		= 1;
		
			/* wait for some time */
		#100;		
		
			/* run tests */
		test_modular_subtractor(X_1, Y_1, N);
		test_modular_subtractor(X_2, Y_2, N);
		test_modular_subtractor(Y_1, X_1, N);
		test_modular_subtractor(Y_2, X_2, N);

		test_modular_subtractor(X_1, X_2, N);
		test_modular_subtractor(X_2, X_1, N);
		test_modular_subtractor(Y_1, Y_2, N);
		test_modular_subtractor(Y_2, Y_1, N);
		
		test_modular_subtractor(X_1, Y_2, N);
		test_modular_subtractor(Y_2, X_1, N);
		test_modular_subtractor(X_2, Y_1, N);
		test_modular_subtractor(Y_1, X_2, N);		
		
			/* print result */
		if (ok)	$display("tb_modular_subtractor_256: SUCCESS");
		else		$display("tb_modular_subtractor_256: FAILURE");
		//
		$finish;
		//
	end
	
	
		//
		// Test Task
		//	
	reg	[256:0]	d;
	wire	[255:0]	d_dummy = d[255:0];
	reg				d_ok;

	integer			w;
	
	reg	[255:0]	x_shreg;
	reg	[255:0]	y_shreg;
	reg	[255:0]	n_shreg;
	reg	[255:0]	d_shreg;
	
	task test_modular_subtractor;
	
		input	[255:0]	x;
		input	[255:0]	y;
		input	[255:0]	n;
				
		begin
		
				/* start filling memories */
			tb_xyn_wren	= 1;
			
				/* initialize shift registers */
			x_shreg = x;
			y_shreg = y;
			n_shreg = n;
			
				/* write all the words */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set addresses */
				tb_xyn_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* set data words */
				tb_x_data	= x_shreg[31:0];
				tb_y_data	= y_shreg[31:0];
				tb_n_data	= n_shreg[31:0];
				
					/* shift inputs */
				x_shreg = {{32{1'bX}}, x_shreg[255:32]};
				y_shreg = {{32{1'bX}}, y_shreg[255:32]};
				n_shreg = {{32{1'bX}}, n_shreg[255:32]};
				
					/* wait for 1 clock tick */
				#10;
				
			end
			
				/* wipe addresses */
			tb_xyn_addr	= {WORD_COUNTER_WIDTH{1'bX}};
			
				/* wipe data words */
			tb_x_data	= {32{1'bX}};
			tb_y_data	= {32{1'bX}};
			tb_n_data	= {32{1'bX}};
			
				/* stop filling memories */
			tb_xyn_wren	= 0;
						
				/* calculate reference value */
			d = {1'b0, (x < y) ? n : {256{1'b0}}};
			d = d + {1'b0, x} - {1'b0, y};
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;
			
				/* read result */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set address */
				tb_d_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* wait for 1 clock tick */
				#10;
				
					/* store data word */
				d_shreg = {tb_d_data, d_shreg[255:32]};

			end				
			
				/* compare */
			d_ok = (d_shreg == d[255:0]);

				/* display results */
			$display("test_modular_subtractor(): %s", d_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && d_ok;
		
		end
		
	endtask
	
      
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
