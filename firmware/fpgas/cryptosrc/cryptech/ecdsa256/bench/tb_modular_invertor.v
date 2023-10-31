`timescale 1ns / 1ps

module tb_modular_invertor;


		//
		// Test Vectors
		//
	localparam	[255:0]	Q		= 256'hffffffff00000001000000000000000000000000ffffffffffffffffffffffff;
	
	localparam	[255:0]	A_1 	= 256'hd3e73ccd63a5b10da308c615bb9ebd3f76e2c5fccc256fd9f629dcc956bf2382;
	localparam	[255:0]	A1_1	= 256'h93fb26d5d199bbb7232a4b7c98e97ba9bb7530d304b5f07736ea4027bbb57ecd;

	localparam	[255:0]	A_2 	= 256'h57b6c628a5c4e870740b2517975ace2216acbe094ac54568b53212ef45e69d22;
	localparam	[255:0]	A1_2	= 256'hcd2af4766642d7d2f3f3f67d92c575c496772ef7d55c75eb46bd07e8d5f9a4aa;
		
		
		//
		// Clock
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
		// Buffers (A, A1, Q)
		//
	wire	[ 2: 0]	core_a_addr;
	wire	[ 2: 0]	core_q_addr;
	wire	[ 2: 0]	core_a1_addr;
	wire				core_a1_wren;
	
	wire	[31: 0]	core_a_data;
	wire	[31: 0]	core_q_data;
	wire	[31: 0]	core_a1_data;
	
	reg	[ 2: 0]	tb_aq_addr;
	reg				tb_aq_wren;	
	reg	[ 2: 0]	tb_a1_addr;
	
	reg	[31: 0]	tb_a_data;
	reg	[31: 0]	tb_q_data;
	wire	[31: 0]	tb_a1_data;

	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
	bram_a
	(	.clk(clk),
		.a_addr(tb_aq_addr), .a_wr(tb_aq_wren), .a_in(tb_a_data), .a_out(),
		.b_addr(core_a_addr), .b_out(core_a_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
	bram_q
	(	.clk(clk),
		.a_addr(tb_aq_addr), .a_wr(tb_aq_wren), .a_in(tb_q_data), .a_out(),
		.b_addr(core_q_addr), .b_out(core_q_data)
	);
	
	bram_1rw_1ro_readfirst # (.MEM_WIDTH(32), .MEM_ADDR_BITS(3))
	bram_a1
	(	.clk(clk),
		.a_addr(core_a1_addr), .a_wr(core_a1_wren), .a_in(core_a1_data), .a_out(),
		.b_addr(tb_a1_addr), .b_out(tb_a1_data)
	);
	
	
		//
		// UUT
		//
	modular_invertor #
	(
		.MAX_OPERAND_WIDTH	(256)
	)
	uut
	(
		.clk		(clk),
		.rst_n	(rst_n),
		
		.ena		(ena),
		.rdy		(rdy),
		
		.a_addr	(core_a_addr),
		.q_addr	(core_q_addr),
		.a1_addr	(core_a1_addr),
		.a1_wren	(core_a1_wren),
		
		.a_din	(core_a_data),
		.q_din	(core_q_data),
		.a1_dout	(core_a1_data)
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
		test_modular_invertor(A_1, A1_1, Q);
		test_modular_invertor(A_2, A1_2, Q);
		
			/* print result */
		if (ok)	$display("tb_modular_invertor: SUCCESS");
		else		$display("tb_modular_invertor: FAILURE");
		//
		//$finish;
		//

	end
	
	
      //
		// Test Task
		//	
	reg		a1_ok;
	
	integer	w;

	task test_modular_invertor;
	
		input	[255:0]	a;
		input	[255:0]	a1;
		input	[255:0]	q;
				
		reg	[255:0]	a_shreg;
		reg	[255:0]	a1_shreg;
		reg	[255:0]	q_shreg;
		
		begin
		
				/* start filling memories */
			tb_aq_wren = 1;
			
				/* initialize shift registers */
			a_shreg = a;
			q_shreg = q;
			
				/* write all the words */
			for (w=0; w<8; w=w+1) begin
				
					/* set addresses */
				tb_aq_addr = w[2:0];
				
					/* set data words */
				tb_a_data	= a_shreg[31:0];
				tb_q_data	= q_shreg[31:0];
				
					/* shift inputs */
				a_shreg = {{32{1'bX}}, a_shreg[255:32]};
				q_shreg = {{32{1'bX}}, q_shreg[255:32]};
				
					/* wait for 1 clock tick */
				#10;
				
			end
			
				/* wipe addresses */
			tb_aq_addr = {3{1'bX}};
			
				/* wipe data words */
			tb_a_data = {32{1'bX}};
			tb_q_data = {32{1'bX}};
			
				/* stop filling memories */
			tb_aq_wren = 0;
			
				/* start operation */
			ena = 1;
			
				/* clear flag */
			#10 ena = 0;
			
				/* wait for operation to complete */
			while (!rdy) #10;

				/* read result */
			for (w=0; w<8; w=w+1) begin
				
					/* set address */
				tb_a1_addr = w[2:0];
				
					/* wait for 1 clock tick */
				#10;
				
					/* store data word */
				a1_shreg = {tb_a1_data, a1_shreg[255:32]};

			end
			
				/* compare */
			a1_ok =	(a1_shreg == a1);

				/* display results */
			$display("test_modular_invertor(): %s", a1_ok ? "OK" : "ERROR");
			
				/* update global flag */
			ok = ok && a1_ok;
		
		end
		
	endtask
	
	
endmodule

