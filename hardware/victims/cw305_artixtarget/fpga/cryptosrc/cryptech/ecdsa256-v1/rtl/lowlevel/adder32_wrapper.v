//------------------------------------------------------------------------------
//
// adder32_wrapper.v
// -----------------------------------------------------------------------------
// Wrapper for 32-bit adder.
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

module adder32_wrapper
	(
		input					clk,		// clock
		input		[31: 0]	a,			// operand input
		input		[31: 0]	b,			// operand input
		output	[31: 0]	s,			// sum output
		input					c_in,		// carry input
		output				c_out		// carry output
	);
	
		//
		// Include Primitive Selector
		//
`include "ecdsa_lowlevel_settings.v"


		//
		// Instantiate Vendor/Generic Primitive
		//
	`ADDER32_PRIMITIVE adder32_inst
	(
		.clk(clk),
		.a(a),
		.b(b),
		.s(s),
		.c_in(c_in),
		.c_out(c_out)
	);
	

endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
