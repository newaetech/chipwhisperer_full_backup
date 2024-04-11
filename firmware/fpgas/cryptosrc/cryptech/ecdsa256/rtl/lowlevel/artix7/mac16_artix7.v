//------------------------------------------------------------------------------
//
// mac16_artix7.v
// -----------------------------------------------------------------------------
// Hardware (Artix-7 DSP48E1) 16-bit multiplier and 47-bit accumulator.
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

module mac16_artix7
	(
		input					clk,		// clock
		input					clr,		// clear accumulator (active-high)
		input					ce,		// enable clock (active-high)
		input		[15: 0]	a,			// operand input
		input		[15: 0]	b,			// operand input
		output	[46: 0]	s			// sum output
	);
	
			
		//
		// DSP48E1 Slice
		//
		
		/* Operation Mode */
	wire	[ 3: 0]	dsp48e1_alumode	= 4'b0000;
	wire	[ 6: 0]	dsp48e1_opmode		= {2'b01, clr, 4'b0101};
		
		/* Internal Product */
	wire	[47: 0]	p_int;

	dsp48e1_wrapper dsp_adder
	(
		.clk			(clk),
		
		.ce			(ce),
		
		.carry		(1'b0),
		
		.alumode		(dsp48e1_alumode),
		.opmode		(dsp48e1_opmode),
		
		.a				({{14{1'b0}}, a}),
		.b				({{ 2{1'b0}}, b}),
		.c				({48{1'b0}}),
		
		.p				(p_int)
	);

		//
		// Output Mapping
		//
	assign s = p_int[46:0];
	

endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
