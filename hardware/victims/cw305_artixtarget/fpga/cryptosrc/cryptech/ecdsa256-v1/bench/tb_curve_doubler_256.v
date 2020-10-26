//------------------------------------------------------------------------------
//
// tb_curve_doubler_256.v
// -----------------------------------------------------------------------------
// Testbench for 256-bit curve point doubler.
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

module tb_curve_doubler_256;


		//
		// Test Vectors
		//
	localparam	[255:0]	PX_1	= 256'h6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296;
	localparam	[255:0]	PY_1	= 256'h4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5;
	localparam	[255:0]	PZ_1	= 256'h0000000000000000000000000000000000000000000000000000000000000001;

	localparam	[255:0]	RX_1	= 256'h9a978f59acd1b5ad570e7d52dcfcde43804b42274f61ddcf1e7d848391d6c70f;
	localparam	[255:0]	RY_1	= 256'h4126885e7f786af905338238e5346d5fe77fc46388668bd0fd59be3190d2f5d1;
	localparam	[255:0]	RZ_1	= 256'h9fc685c5fc34ff371dcfd694f81f3c2c579c66aed662bd9d976c80d06f7ea3ea;

	localparam	[255:0]	PX_2	= 256'h0ec88440c8b00a9e572bf1bceb7d0c5906bd65990a9b7081130bd72e2c136ca0;
	localparam	[255:0]	PY_2	= 256'hc0bc77e1339e899101f8e8eccf79c3f7f4bbdd1bf96f6446199bd423026a60d6;
	localparam	[255:0]	PZ_2	= 256'hdd27cb52a31d1f6e041accf1103de05ba0a5edd74b738d51fe3397de0e3fc306;

	localparam	[255:0]	RX_2	= 256'he6afae63e774df21244609cb4c35d17d28b36b8b9fb7c58929af247f34ac72f9;
	localparam	[255:0]	RY_2	= 256'h061076db7a5745adc90b2e9eebe0ad6482309690f50b60835c265cf83a1b34eb;
	localparam	[255:0]	RZ_2	= 256'h1b6bfd04f2a41d68e85423655db1142d97ebaec0c67c450408f427e35c4f054f;
	
	localparam	[255:0]	PX_3	= 256'hb0f824c88ec62df89912ca9ffbcbbb4ffb4d80f8a7d7b4a992273261a2f7be7f;
	localparam	[255:0]	PY_3	= 256'h403e34c78c2b816fce2b1f8d73cfeef28113b8de8bda4a447d17b619bef73705;
	localparam	[255:0]	PZ_3	= 256'h0e3e81bb8e954f3164ae54a6cffa7fcc9631dfddee55fac61e46415f1f5fe5e2;

	localparam	[255:0]	RX_3	= 256'hd4e725920c88cc2f57847a315f3b6c180abb278b8fa2a47da3d1a191a8c29e19;
	localparam	[255:0]	RY_3	= 256'ha798ad8dbd66c98b53414ab1d04b0f871929a90fea996c88b96d9d68eb8eb0dc;
	localparam	[255:0]	RZ_3	= 256'ha7ead72c01294eaf2899bb6b84f7d26417e6758e3db29f3b5c2ca8e9911067f5;
	
	localparam	[255:0]	PX_4	= 256'hXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;
	localparam	[255:0]	PY_4	= 256'hXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX;
	localparam	[255:0]	PZ_4	= 256'h0000000000000000000000000000000000000000000000000000000000000000;

	localparam	[255:0]	RX_4	= 256'h0000000000000000000000000000000000000000000000000000000000000001;
	localparam	[255:0]	RY_4	= 256'h0000000000000000000000000000000000000000000000000000000000000001;
	localparam	[255:0]	RZ_4	= 256'h0000000000000000000000000000000000000000000000000000000000000000;

	localparam	[255:0]	Q		= 256'hffffffff00000001000000000000000000000000ffffffffffffffffffffffff;

	
	
		//
		// TODO: Test special cases!
		//
		
		
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
	wire	[ 5: 0]	dbl_uop_addr;
	wire	[19: 0]	dbl_uop;
	
	uop_dbl_rom dbl_rom
	(
		.clk		(clk),
		.addr		(dbl_uop_addr),
		.data		(dbl_uop)
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
		
		.uop_addr	(dbl_uop_addr),
		.uop			(dbl_uop),

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
		test_curve_doubler(PX_1, PY_1, PZ_1, RX_1, RY_1, RZ_1);
		test_curve_doubler(PX_2, PY_2, PZ_2, RX_2, RY_2, RZ_2);
		test_curve_doubler(PX_3, PY_3, PZ_3, RX_3, RY_3, RZ_3);
		test_curve_doubler(PX_4, PY_4, PZ_4, RX_4, RY_4, RZ_4);
		
			/* print result */
		if (ok)	$display("tb_curve_doubler_256: SUCCESS");
		else		$display("tb_curve_doubler_256: FAILURE");
		//
//		$finish;
		//
	end
	
	
		//
		// Test Task
		//	
	reg		t_ok;
	
	integer	w;

	task test_curve_doubler;
	
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
			$display("test_curve_doubler(): %s", t_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && t_ok;
		
		end
		
	endtask
	
      
endmodule

//------------------------------------------------------------------------------
// End-of-File
//------------------------------------------------------------------------------
