//------------------------------------------------------------------------------
//
// tb_curve_adder_256.v
// -----------------------------------------------------------------------------
// Testbench for 256-bit curve point adder.
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

module tb_curve_adder_256;


		//
		// Test Vectors
		//
	localparam	[255:0]	PX_1	= 256'ha536512112e4bb911ae72744a914761ddc53700f889c88e583e0edd45c179b08;
	localparam	[255:0]	PY_1	= 256'h239e73bf40f4831ab71ccea072291893ac8582982ea6fec6bd6aaf36ac32d22e;
	localparam	[255:0]	PZ_1	= 256'h32258ae04c5498bb34b29c54a7f95afc10c009540c51731eae164750ca385029;

	localparam	[255:0]	RX_1	= 256'he4fcdd1a151b405b2a567d20d7674031c6d5b207b0b5dcf277015d81784492d5;
	localparam	[255:0]	RY_1	= 256'h4782c540b58988b07bb8e0c5ad3ff562dd45c075a39ee71896d5eb33702dd656;
	localparam	[255:0]	RZ_1	= 256'hae637ff2fd5468780241afb3a8ebaeb8618e86b4a1a211b350546c9e6fea93d4;

	localparam	[255:0]	PX_2	= 256'he58a6470e038f6b261d5a9a72fb2bd96b6bad433ff7baea6a40b5facf5085189;
	localparam	[255:0]	PY_2	= 256'h03dd8785b592307811ee5512e2d713c5dc65f60f01883340fe0f56f858a39474;
	localparam	[255:0]	PZ_2	= 256'h1b4657b1e79c9074fbf7f63f96ce2854db4808afc72841fac623dc68d9bff64d;

	localparam	[255:0]	RX_2	= 256'hc354e99a827a3f1c30f29f6b1d72273eb0daaeb06bb373ed315e305b89d857ca;
	localparam	[255:0]	RY_2	= 256'h0cb054f95589c1fcbe763df3b8d7badd568d5e93a667076dddfc70dcfab74948;
	localparam	[255:0]	RZ_2	= 256'hd79d9170dd628aee82d149715a6ec6cc44426ccae236d2a146edbd15a564ea53;

	localparam	[255:0]	PX_3	= 256'hbf5fe30c79025a0b638b0fd62bf1349aee0a9fc7fc2719291b0c23535c16eb52;
	localparam	[255:0]	PY_3	= 256'h8a637c7c0b9459de664d40a717e1abc0f843f03169fae943e0835cbe767da06b;
	localparam	[255:0]	PZ_3	= 256'h0871d93601d654216912866514a788a92e8a9b6047611bf185d459e204727377;

	localparam	[255:0]	RX_3	= 256'h1ba6259b5b750e4d6e4f490f661646cd9491be16965f47044ac2688048e567c5;
	localparam	[255:0]	RY_3	= 256'h80e55c16f403f8d7282bca628477771a45330567caa5aaab9a54919dbe05e3e4;
	localparam	[255:0]	RZ_3	= 256'hb99663f045c9602b05f23aaaa508e6167d15740be900175dbeceb957a9dad951;

	localparam	[255:0]	PX_4	= 256'hxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;
	localparam	[255:0]	PY_4	= 256'hxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;
	localparam	[255:0]	PZ_4	= 256'h0000000000000000000000000000000000000000000000000000000000000000;

	localparam	[255:0]	RX_4	= 256'h6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296; // G.x
	localparam	[255:0]	RY_4	= 256'h4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5; // G.y
	localparam	[255:0]	RZ_4	= 256'h0000000000000000000000000000000000000000000000000000000000000001;

	localparam	[255:0]	PX_5	= 256'h6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296; // G.x
	localparam	[255:0]	PY_5	= 256'h4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5; // G.y
	localparam	[255:0]	PZ_5	= 256'h0000000000000000000000000000000000000000000000000000000000000001;

	localparam	[255:0]	RX_5	= 256'h29d05c193da77b710e86323538b77e1b11f904fea42998be16bd8d744ece7ad0; // H.x
	localparam	[255:0]	RY_5	= 256'hb01cbd1c01e58065711814b583f061e9d431cca994cea1313449bf97c840ae07; // H.y
	localparam	[255:0]	RZ_5	= 256'h0000000000000000000000000000000000000000000000000000000000000001;
	
	localparam	[255:0]	PX_6	= 256'h6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296; // G.x
	localparam	[255:0]	PY_6	= 256'h4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5; // G.y
	localparam	[255:0]	PZ_6	= 256'h0000000000000000000000000000000000000000000000000000000000000001;

	localparam	[255:0]	RX_6	= 256'h0000000000000000000000000000000000000000000000000000000000000001;
	localparam	[255:0]	RY_6	= 256'h0000000000000000000000000000000000000000000000000000000000000001;
	localparam	[255:0]	RZ_6	= 256'h0000000000000000000000000000000000000000000000000000000000000000;

	localparam	[255:0]	Q		= 256'hffffffff00000001000000000000000000000000ffffffffffffffffffffffff;
		
		
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
		// Buffers (PX, PY, PZ, RX, RY, RZ, Q)
		//
	wire	[WORD_COUNTER_WIDTH-1:0]	core_px_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_py_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_pz_addr;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	core_rx_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_ry_addr;
	wire	[WORD_COUNTER_WIDTH-1:0]	core_rz_addr;
	
	wire	[WORD_COUNTER_WIDTH-1:0]	core_q_addr;
	
	wire										core_rx_wren;
	wire										core_ry_wren;
	wire										core_rz_wren;
	
	wire	[                32-1:0]	core_px_data;
	wire	[                32-1:0]	core_py_data;
	wire	[                32-1:0]	core_pz_data;
	
	wire	[                32-1:0]	core_rx_data_wr;
	wire	[                32-1:0]	core_ry_data_wr;
	wire	[                32-1:0]	core_rz_data_wr;
	
	wire	[                32-1:0]	core_rx_data_rd;
	wire	[                32-1:0]	core_ry_data_rd;
	wire	[                32-1:0]	core_rz_data_rd;	
	
	wire	[                32-1:0]	core_q_data;
	
	reg	[WORD_COUNTER_WIDTH-1:0]	tb_xyzq_addr;
	reg										tb_xyzq_wren;
	
	reg	[                  31:0]	tb_px_data;
	reg	[                  31:0]	tb_py_data;
	reg	[                  31:0]	tb_pz_data;
	wire	[                  31:0]	tb_rx_data;
	wire	[                  31:0]	tb_ry_data;
	wire	[                  31:0]	tb_rz_data;
	reg	[                  31:0]	tb_q_data;
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_px
	(	.clk(clk),
		.a_addr(tb_xyzq_addr), .a_wr(tb_xyzq_wren), .a_in(tb_px_data), .a_out(),
		.b_addr(core_px_addr), .b_out(core_px_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_py
	(	.clk(clk),
		.a_addr(tb_xyzq_addr), .a_wr(tb_xyzq_wren), .a_in(tb_py_data), .a_out(),
		.b_addr(core_py_addr), .b_out(core_py_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_pz
	(	.clk(clk),
		.a_addr(tb_xyzq_addr), .a_wr(tb_xyzq_wren), .a_in(tb_pz_data), .a_out(),
		.b_addr(core_pz_addr), .b_out(core_pz_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_q
	(	.clk(clk),
		.a_addr(tb_xyzq_addr), .a_wr(tb_xyzq_wren), .a_in(tb_q_data), .a_out(),
		.b_addr(core_q_addr), .b_out(core_q_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_rx
	(	.clk(clk),
		.a_addr(core_rx_addr), .a_wr(core_rx_wren), .a_in(core_rx_data_wr), .a_out(core_rx_data_rd),
		.b_addr(tb_xyzq_addr), .b_out(tb_rx_data)
	);	
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_ry
	(	.clk(clk),
		.a_addr(core_ry_addr), .a_wr(core_ry_wren), .a_in(core_ry_data_wr), .a_out(core_ry_data_rd),
		.b_addr(tb_xyzq_addr), .b_out(tb_ry_data)
	);	
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(WORD_COUNTER_WIDTH))
	bram_rz
	(	.clk(clk),
		.a_addr(core_rz_addr), .a_wr(core_rz_wren), .a_in(core_rz_data_wr), .a_out(core_rz_data_rd),
		.b_addr(tb_xyzq_addr), .b_out(tb_rz_data)
	);
	
	
		//
		// Opcode
		//
	wire	[ 5: 0]	add_uop_addr;
	wire	[19: 0]	add_uop;
	
	uop_add_rom add_rom
	(
		.clk		(clk),
		.addr		(add_uop_addr),
		.data		(add_uop)
    );
	
		//
		// UUT
		//
	curve_dbl_add_256 uut
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(ena),
		.rdy		(rdy),
		
		.uop_addr	(add_uop_addr),
		.uop			(add_uop),

		.px_addr	(core_px_addr),
		.py_addr	(core_py_addr),
		.pz_addr	(core_pz_addr),
		.rx_addr	(core_rx_addr),
		.ry_addr	(core_ry_addr),
		.rz_addr	(core_rz_addr),
		.q_addr	(core_q_addr),
		
		.rx_wren	(core_rx_wren),
		.ry_wren	(core_ry_wren),
		.rz_wren	(core_rz_wren),
		
		.px_din	(core_px_data),
		.py_din	(core_py_data),
		.pz_din	(core_pz_data),
		.rx_din	(core_rx_data_rd),
		.ry_din	(core_ry_data_rd),
		.rz_din	(core_rz_data_rd),		
		.rx_dout	(core_rx_data_wr),
		.ry_dout	(core_ry_data_wr),
		.rz_dout	(core_rz_data_wr),
		.q_din	(core_q_data)
	);
		
		
		//
		// Testbench Routine
		//
	reg ok = 1;
	initial begin
		
			/* initialize control inputs */
		rst_n		= 0;
		ena		= 0;
		
			/* wait for some time */
		#200;
		
			/* de-assert reset */
		rst_n		= 1;
		
			/* wait for some time */
		#100;		
		
			/* run tests */
		test_curve_adder(PX_1,     PY_1, PZ_1, RX_1, RY_1, RZ_1);
		test_curve_adder(PX_2,     PY_2, PZ_2, RX_2, RY_2, RZ_2);
		test_curve_adder(PX_3,     PY_3, PZ_3, RX_3, RY_3, RZ_3);
		test_curve_adder(PX_4,     PY_4, PZ_4, RX_4, RY_4, RZ_4);
		test_curve_adder(PX_5,     PY_5, PZ_5, RX_5, RY_5, RZ_5);
		test_curve_adder(PX_6, Q - PY_6, PZ_6, RX_6, RY_6, RZ_6);
		
			/* print result */
		if (ok)	$display("tb_curve_adder_256: SUCCESS");
		else		$display("tb_curve_adder_256: FAILURE");
		//
		$finish;
		//
	end
	
	
		//
		// Test Task
		//	
	reg		t_ok;
	
	integer	w;

	task test_curve_adder;
	
		input	[255:0]	px;
		input	[255:0]	py;
		input	[255:0]	pz;
		
		input	[255:0]	rx;
		input	[255:0]	ry;
		input	[255:0]	rz;		
		
		reg	[255:0]	px_shreg;
		reg	[255:0]	py_shreg;
		reg	[255:0]	pz_shreg;
		
		reg	[255:0]	rx_shreg;
		reg	[255:0]	ry_shreg;
		reg	[255:0]	rz_shreg;
		
		reg	[255:0]	q_shreg;
		
		begin
		
				/* start filling memories */
			tb_xyzq_wren = 1;
			
				/* initialize shift registers */
			px_shreg = px;
			py_shreg = py;
			pz_shreg = pz;
			q_shreg  = Q;
			
				/* write all the words */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set addresses */
				tb_xyzq_addr = w[WORD_COUNTER_WIDTH-1:0];
				
					/* set data words */
				tb_px_data	= px_shreg[31:0];
				tb_py_data	= py_shreg[31:0];
				tb_pz_data	= pz_shreg[31:0];
				tb_q_data	= q_shreg[31:0];
				
					/* shift inputs */
				px_shreg = {{32{1'bX}}, px_shreg[255:32]};
				py_shreg = {{32{1'bX}}, py_shreg[255:32]};
				pz_shreg = {{32{1'bX}}, pz_shreg[255:32]};
				q_shreg  = {{32{1'bX}}, q_shreg[255:32]};
				
					/* wait for 1 clock tick */
				#10;
				
			end
			
				/* wipe addresses */
			tb_xyzq_addr = {WORD_COUNTER_WIDTH{1'bX}};
			
				/* wipe data words */
			tb_px_data = {32{1'bX}};
			tb_py_data = {32{1'bX}};
			tb_pz_data = {32{1'bX}};
			tb_q_data  = {32{1'bX}};
			
				/* stop filling memories */
			tb_xyzq_wren = 0;
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;
			
				/* read result */
			for (w=0; w<OPERAND_NUM_WORDS; w=w+1) begin
				
					/* set address */
				tb_xyzq_addr = w[WORD_COUNTER_WIDTH-1:0];
				
					/* wait for 1 clock tick */
				#10;
				
					/* store data word */
				rx_shreg = {tb_rx_data, rx_shreg[255:32]};
				ry_shreg = {tb_ry_data, ry_shreg[255:32]};
				rz_shreg = {tb_rz_data, rz_shreg[255:32]};

			end
			
				/* compare */
			t_ok =	(rx_shreg == rx) &&
						(ry_shreg == ry) &&
						(rz_shreg == rz);

				/* display results */
			$display("test_curve_adder(): %s", t_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && t_ok;
		
		end
		
	endtask
	
endmodule


//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
