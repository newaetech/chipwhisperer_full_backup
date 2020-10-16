//------------------------------------------------------------------------------
//
// tb_lowlevel_adder47.v
// -----------------------------------------------------------------------------
// Testbench for 47-bit adder.
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


module tb_lowlevel_adder47;

		//
		// Inputs
		//
	reg				clk;
	reg	[46: 0]	a;	
	reg	[46: 0]	b;

		//
		// Outputs
		//
	wire	[46: 0]	s;	

		//
		// Test Vectors {a, b}
		//
	wire	[93: 0]	vec_0		= {47'h2a87ca22be8b, 47'h05378eb1c71e};
	wire	[93: 0]	vec_1		= {47'h7320ad746e1d, 47'h3b628ba79b98};
	wire	[93: 0]	vec_2		= {47'h59f741e08254, 47'h2a385502f25d};
	wire	[93: 0]	vec_3		= {47'h3f55296c3a54, 47'h5e3872760ab7};
	wire	[93: 0]	vec_4		= {47'h3617de4a9626, 47'h2c6f5d9e98bf};
	wire	[93: 0]	vec_5		= {47'h1292dc29f8f4, 47'h1dbd289a147c};
	wire	[93: 0]	vec_6		= {47'h69da3113b5f0, 47'h38c00a60b1ce};
	wire	[93: 0]	vec_7		= {47'h1d7e819d7a43, 47'h1d7c90ea0e5f};
	
		//
		// UUT
		//
	adder47_wrapper uut
	(
		.clk		(clk),
		.a			(a),
		.b			(b),
		.s			(s)
	);


		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		//
		clk = 0;
		//
		#100;
		//
		test_adder47(vec_0);
		test_adder47(vec_1);
		test_adder47(vec_2);
		test_adder47(vec_3);
		test_adder47(vec_4);
		test_adder47(vec_5);
		test_adder47(vec_6);
		test_adder47(vec_7);
		//
		if (ok)	$display("tb_lowlevel_adder47: SUCCESS");
		else		$display("tb_lowlevel_adder47: FAILURE");
		//
		$finish;
		//
	end
      
		
		//
		// Test Routine
		//
	reg	[46: 0]	ss;		// reference value of sum
	reg				cc;		// reference value of carry
	reg				ss_ok;	// result matches reference value
	
	task test_adder47;

		input	[93: 0] vec;

		begin
				
				/* break down test vector */
			a		= vec[93:47];
			b		= vec[46: 0];
			
				/* calculate reference values */
			ss = a + b;
			
				/* send one clock tick */
			#10 clk = 1;
			#10 clk = 0;
			
				/* check outputs */
			ss_ok = (s == ss);

				/* display results */
			$display("test_adder47(): %s", ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && ss_ok;

		end

	endtask
		
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
