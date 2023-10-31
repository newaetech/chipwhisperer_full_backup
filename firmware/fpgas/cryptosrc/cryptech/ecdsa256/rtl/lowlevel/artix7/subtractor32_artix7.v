//------------------------------------------------------------------------------
//
// subtractor32_artix7.v
// -----------------------------------------------------------------------------
// Hardware (Artix-7 DSP48E1) 32-bit subtractor.
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

module subtractor32_artix7
	(
		input					clk,
		input		[31: 0]	a,
		input		[31: 0]	b,
		output	[31: 0]	d,
		input					b_in,
		output				b_out	
	);

		//
		// Lower and higher parts of operand
		//
	wire	[17: 0]	bl = b[17: 0];
	wire	[13: 0]	bh = b[31:18];
	
		//
		// DSP48E1 Slice
		//
		
		/* Operation Mode */
	wire	[ 3: 0]	dsp48e1_alumode	= 4'b0011;
	wire	[ 6: 0]	dsp48e1_opmode		= 7'b0110011;

		/* Internal Product */	
	wire	[47: 0]	p_int;
	
	dsp48e1_wrapper dsp_subtractor
	(
		.clk			(clk),
	
		.ce			(1'b1),
		
		.carry		(b_in),
		
		.alumode		(dsp48e1_alumode),
		.opmode		(dsp48e1_opmode),
		
		.a				({{16{1'b0}}, bh}),
		.b				(bl),
		.c				({{16{1'b0}}, a}),
		
		.p				(p_int)
	);

		//
		// Output Mapping
		//
	assign d 		= p_int[31: 0];
	assign b_out	= p_int[32];

endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
