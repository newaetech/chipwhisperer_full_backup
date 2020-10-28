//------------------------------------------------------------------------------
//
// dsp48e1_wrapper.v
// -----------------------------------------------------------------------------
// Hardware (Artix-7 DSP48E1) tile wrapper.
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

module dsp48e1_wrapper
	(
		input					clk,
		
		input					ce,
		
		input		[ 6: 0]	opmode,
		input		[ 3: 0]	alumode,
		
		input					carry,
		
		input		[29: 0]	a,
		input		[17: 0]	b,
		input		[47: 0]	c,
		
		output	[47: 0]	p
	);
	
	
		//
		// Tile instantiation
		//
	DSP48E1 #
	(
		.AREG						(0),
		.BREG						(0),
		.CREG						(0),
		.DREG						(0),
		.MREG						(0),
		.PREG						(1),
		.ADREG					(0),
		
		.ACASCREG				(0),
		.BCASCREG				(0),
		.ALUMODEREG				(0),
		.INMODEREG				(0),
		.OPMODEREG				(0),
		.CARRYINREG				(0),
		.CARRYINSELREG			(0),

		.A_INPUT					("DIRECT"),
		.B_INPUT					("DIRECT"),
		
		.USE_DPORT				("FALSE"),
		.USE_MULT				("DYNAMIC"),
		.USE_SIMD				("ONE48"),

		.USE_PATTERN_DETECT	("NO_PATDET"),
		.SEL_PATTERN			("PATTERN"),
		.SEL_MASK				("MASK"),
		.PATTERN					(48'h000000000000),
		.MASK						(48'h3fffffffffff),
		.AUTORESET_PATDET		("NO_RESET")
	)
	DSP48E1_inst
	(
		.CLK					(clk),

		.RSTA					(1'b0),
		.RSTB					(1'b0),
		.RSTC					(1'b0),
		.RSTD					(1'b0),
		.RSTM					(1'b0),
		.RSTP					(1'b0),

		.RSTCTRL				(1'b0),
		.RSTINMODE			(1'b0),
		.RSTALUMODE			(1'b0),
		.RSTALLCARRYIN		(1'b0),

		.CEA1					(1'b0),
		.CEA2					(1'b0),
		.CEB1					(1'b0),
		.CEB2					(1'b0),
		.CEC					(1'b0),
		.CED					(1'b0),
		.CEM					(1'b0),
		.CEP					(ce),
		.CEAD					(1'b0),
		.CEALUMODE			(1'b0),
		.CEINMODE			(1'b0),

		.CECTRL				(1'b0),
		.CECARRYIN			(1'b0),

		.A						(a),
		.B						(b),
		.C						(c),
		.D						({25{1'b1}}),
		.P						(p),

		.CARRYIN				(carry),
		.CARRYOUT			(),
		.CARRYINSEL			(3'b000),

		.CARRYCASCIN		(1'b0),
		.CARRYCASCOUT		(),

		.PATTERNDETECT		(),
		.PATTERNBDETECT	(),

		.OPMODE				(opmode),
		.ALUMODE				(alumode),
		.INMODE				(5'b00000),

		.MULTSIGNIN			(1'b0),
		.MULTSIGNOUT		(),

		.UNDERFLOW			(),
		.OVERFLOW			(),

		.ACIN					(30'd0),
		.BCIN					(18'd0),
		.PCIN					(48'd0),

		.ACOUT				(),
		.BCOUT				(),
		.PCOUT				()
  );

endmodule
