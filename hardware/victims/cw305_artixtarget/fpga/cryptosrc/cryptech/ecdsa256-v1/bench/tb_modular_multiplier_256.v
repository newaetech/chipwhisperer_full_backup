//------------------------------------------------------------------------------
//
// tb_modular_multiplier_256.v
// -----------------------------------------------------------------------------
// Testbench for modular multi-word multiplier.
//
// Authors: Pavel Shatov
//
// Copyright (c) 2015-2016, NORDUnet A/S
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

module tb_modular_multiplier_256;


		//
		// Test Vectors
		//	
	localparam	[255:0]	N		= 256'hffffffff00000001000000000000000000000000ffffffffffffffffffffffff;
	
	localparam	[255:0]	X_1	= 256'h6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296;
	localparam	[255:0]	Y_1	= 256'h4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5;
	localparam	[255:0]	P_1	= 256'h823cd15f6dd3c71933565064513a6b2bd183e554c6a08622f713ebbbface98be;
	
	localparam	[255:0]	X_2	= 256'h29d05c193da77b710e86323538b77e1b11f904fea42998be16bd8d744ece7ad0;
	localparam	[255:0]	Y_2	= 256'hb01cbd1c01e58065711814b583f061e9d431cca994cea1313449bf97c840ae07;
	localparam	[255:0]	P_2	= 256'h76b2571d1d009ab0e7d1cc086c7d3648f08755b2e2585e780d11f053b06fb6ec;
	
	localparam	[255:0]	X_3	= 256'h8101ece47464a6ead70cf69a6e2bd3d88691a3262d22cba4f7635eaff26680a8;
	localparam	[255:0]	Y_3	= 256'hd8a12ba61d599235f67d9cb4d58f1783d3ca43e78f0a5abaa624079936c0c3a9;
	localparam	[255:0]	P_3	= 256'h944fea6a4fac7ae475a6bb211db4bbd394bd9b3ee9a038f6c17125a00b3a5375;
	
	localparam	[255:0]	X_4	= 256'h7214bc9647160bbd39ff2f80533f5dc6ddd70ddf86bb815661e805d5d4e6f27c;
	localparam	[255:0]	Y_4	= 256'h8b81e3e977597110c7cf2633435b2294b72642987defd3d4007e1cfc5df84541;
	localparam	[255:0]	P_4	= 256'h78d3e33c81ab9c652679363c76df004ea6f9a9e3a242a0fb71a4e8fdf41ab519;
	
	
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
	reg	rst_n;
	reg	ena;
	wire	rdy;
	
	
		//
		// Buffers (X, Y, N, P)
		//
	wire	[WORD_COUNTER_WIDTH-1:0]	core_x_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_y_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_n_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_p_addr;
	
	wire										core_p_wren;
	
	wire	[                  31:0]	core_x_data;
	wire	[                  31:0]	core_y_data;
	wire	[                  31:0]	core_n_data;
	wire	[                  31:0]	core_p_data;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_xyn_addr;
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_p_addr;
	
	reg										tb_xyn_wren;
	
	reg	[                  31:0]	tb_x_data;
	reg	[                  31:0]	tb_y_data;
	reg	[                  31:0]	tb_n_data;
	wire	[                  31:0]	tb_p_data;
	
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

		.b_addr	(core_x_addr),
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

		.b_addr	(core_y_addr),
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

		.a_addr	(core_p_addr),
		.a_wr		(core_p_wren),
		.a_in		(core_p_data),
		.a_out	(),

		.b_addr	(tb_p_addr),
		.b_out	(tb_p_data)
	);
	
	
		//
		// UUT
		//
	modular_multiplier_256 uut
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(ena),
		.rdy		(rdy),

		.a_addr	(core_x_addr),
		.b_addr	(core_y_addr),
		.n_addr	(core_n_addr),
		.p_addr	(core_p_addr),
		.p_wren	(core_p_wren),
		
		.a_din	(core_x_data),
		.b_din	(core_y_data),
		.n_din	(core_n_data),
		.p_dout	(core_p_data)
	);

		
		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		
			/* initialize control inputs */
		rst_n				= 0;
		ena				= 0;
		
		tb_xyn_wren		= 0;
		
			/* wait for some time */
		#200;
		
			/* de-assert reset */
		rst_n				= 1;
		
			/* wait for some time */
		#100;		
		
			/* run tests */
		test_modular_multiplier(X_1, Y_1, N, P_1);
		test_modular_multiplier(X_2, Y_2, N, P_2);
		test_modular_multiplier(X_3, Y_3, N, P_3);
		test_modular_multiplier(X_4, Y_4, N, P_4);
		
			/* print result */
		if (ok)	$display("tb_modular_multiplier_256: SUCCESS");
		else		$display("tb_modular_multiplier_256: FAILURE");
		//
		//$finish;
		//
	end
	
	
		//
		// Test Task
		//
	reg	[255:0]	p;
	reg				p_ok;
	
	integer			w;
	
	reg	[511:0]	pp_full;
	reg	[255:0]	pp_ref;
	
	task test_modular_multiplier;
	
		input	[255:0] x;
		input	[255:0] y;
		input	[255:0] n;
		input	[255:0] pp;
		
		reg	[255:0]	x_shreg;
		reg	[255:0]	y_shreg;
		reg	[255:0]	n_shreg;
		reg	[255:0]	p_shreg;
	
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
			pp_full = {{256{1'b0}}, x} * {{256{1'b0}}, y};
			pp_ref = pp_full % {{256{1'b0}}, n};
			
				/* compare reference value against hard-coded one */
			if (pp_ref != pp) begin
				$display("ERROR: pp_ref != pp");
				$finish;
			end
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;
			
				/* read result */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set address */
				tb_p_addr	= w[WORD_COUNTER_WIDTH-1:0];
				
					/* wait for 1 clock tick */
				#10;
				
					/* store data word */
				p_shreg = {tb_p_data, p_shreg[255:32]};

			end				
			
				/* compare */
			p_ok = (p_shreg == pp);

				/* display results */
			$display("test_modular_multiplier_256(): %s", p_ok ? "OK" : "ERROR");
			
				/* update flag */
			ok = ok && p_ok;
		
		end
		
	endtask
	

	
      
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
