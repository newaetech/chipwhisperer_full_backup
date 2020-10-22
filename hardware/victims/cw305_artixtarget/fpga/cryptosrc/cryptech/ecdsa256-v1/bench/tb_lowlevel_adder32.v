//------------------------------------------------------------------------------
//
// tb_lowlevel_adder32.v
// -----------------------------------------------------------------------------
// Testbench for 32-bit adder.
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


module tb_lowlevel_adder32;

		//
		// Inputs
		//
	reg				clk;
	reg	[31: 0]	a;	
	reg	[31: 0]	b;
	reg				c_in;

		//
		// Outputs
		//
	wire	[31: 0]	s;	
	wire				c_out;

		//
		// Test Vectors {a, b, c_in}
		//
	wire	[64: 0]	vec_0		= {32'h00000000, 32'h00000000, 1'b0};	// all zeroes, no carry
	wire	[64: 0]	vec_1		= {32'h00000000, 32'h00000000, 1'b1};	// all zeroes with carry
	wire	[64: 0]	vec_2		= {32'h00000000, 32'hFFFFFFFF, 1'b0};	// zeroes and ones, no carry
	wire	[64: 0]	vec_3		= {32'h00000000, 32'hFFFFFFFF, 1'b1};	// zeroes and ones with carry
	wire	[64: 0]	vec_4		= {32'hFFFFFFFF, 32'h00000000, 1'b0};	// ones and zeroes, no carry
	wire	[64: 0]	vec_5		= {32'hFFFFFFFF, 32'h00000000, 1'b1};	// ones and zeroes with carry
	wire	[64: 0]	vec_6		= {32'hFFFFFFFF, 32'hFFFFFFFF, 1'b0};	// all ones, no carry
	wire	[64: 0]	vec_7		= {32'hFFFFFFFF, 32'hFFFFFFFF, 1'b1};	// all ones with carry
	
	wire	[64: 0]	vec_8		= {32'hd898c296, 32'h37bf51f5, 1'b0};	// random values, no carry
	wire	[64: 0]	vec_9		= {32'hf4a13945, 32'hcbb64068, 1'b0};	// random values, no carry
	wire	[64: 0]	vec_10	= {32'h2deb33a0, 32'h6b315ece, 1'b0};	// random values, no carry
	wire	[64: 0]	vec_11	= {32'h77037d81, 32'h2bce3357, 1'b0};	// random values, no carry
	wire	[64: 0]	vec_12	= {32'h63a440f2, 32'h7c0f9e16, 1'b1};	// random values with carry
	wire	[64: 0]	vec_13	= {32'hf8bce6e5, 32'h8ee7eb4a, 1'b1};	// random values with carry
	wire	[64: 0]	vec_14	= {32'he12c4247, 32'hfe1a7f9b, 1'b1};	// random values with carry
	wire	[64: 0]	vec_15	= {32'h6b17d1f2, 32'h4fe342e2, 1'b1};	// random values with carry
	

		//
		// UUT
		//
	adder32_wrapper uut
	(
		.clk		(clk),
		.a			(a),
		.b			(b),
		.s			(s),
		.c_in		(c_in),
		.c_out	(c_out)
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
		test_adder32(vec_0);
		test_adder32(vec_1);
		test_adder32(vec_2);
		test_adder32(vec_3);
		test_adder32(vec_4);
		test_adder32(vec_5);
		test_adder32(vec_6);
		test_adder32(vec_7);
		//
		test_adder32(vec_8);
		test_adder32(vec_9);
		test_adder32(vec_10);
		test_adder32(vec_11);
		test_adder32(vec_12);
		test_adder32(vec_13);
		test_adder32(vec_14);
		test_adder32(vec_15);
		//
		if (ok)	$display("tb_lowlevel_adder32: SUCCESS");
		else		$display("tb_lowlevel_adder32: FAILURE");
		//
		$finish;
		//
	end
      
		
		//
		// Test Routine
		//
	reg	[31: 0]	ss;		// reference value of sum
	reg				cc;		// reference value of carry
	reg				ss_ok;	// result matches reference value
	
	task test_adder32;

		input	[64: 0] vec;

		begin
				
				/* break down test vector */
			a		= vec[64:33];
			b		= vec[32: 1];
			c_in	= vec[ 0: 0];
			
				/* calculate reference values */
			{cc, ss} = {1'b0, a} + {1'b0, b} + {32'd0, c_in};
			
				/* send one clock tick */
			#10 clk = 1;
			#10 clk = 0;
			
				/* check outputs */
			ss_ok = (s == ss) && (c_out == cc);

				/* display results */
			$display("test_adder32(): 0x%08X + 0x%08X + %01d = {%01d, 0x%08X} [%0s]", a, b, c_in, c_out, s, ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && ss_ok;

		end

	endtask
		
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
