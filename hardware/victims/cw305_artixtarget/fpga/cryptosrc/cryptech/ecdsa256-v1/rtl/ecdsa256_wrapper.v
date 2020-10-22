//======================================================================
//
// Copyright (c) 2016, NORDUnet A/S All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// - Neither the name of the NORDUnet nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//======================================================================

module ecdsa256_wrapper
  (
   input wire          clk,
   input wire          rst_n,

   input wire          cs,
   input wire          we,

   input wire [5: 0]   address,
   input wire [31: 0]  write_data,
   output wire [31: 0] read_data
   );


   //
   // Address Decoder
   //
   localparam ADDR_MSB_REGS = 1'b0;
	localparam ADDR_MSB_CORE = 1'b1;
	
   wire [0:0] addr_msb = address[5];
   wire [4:0] addr_lsb = address[4:0];


   //
   // Output Mux
   //
   wire [31: 0] read_data_regs;
   wire [31: 0] read_data_core;


   //
   // Registers
   //
   localparam ADDR_NAME0        = 5'h00;
   localparam ADDR_NAME1        = 5'h01;
   localparam ADDR_VERSION      = 5'h02;

   localparam ADDR_CONTROL      = 5'h08;               // {next, init}
   localparam ADDR_STATUS       = 5'h09;               // {valid, ready}
	localparam ADDR_DUMMY        = 5'h0F;               // don't care

// localparam CONTROL_INIT_BIT  = 0; -- not used
   localparam CONTROL_NEXT_BIT  = 1;

// localparam STATUS_READY_BIT  = 0; -- hardcoded to always read 1
   localparam STATUS_VALID_BIT  = 1;

   localparam CORE_NAME0        = 32'h65636473; // "ecds"
   localparam CORE_NAME1        = 32'h61323536; // "a256"
   localparam CORE_VERSION      = 32'h302E3131; // "0.11"


   //
   // Registers
   //
   reg        reg_control;
	reg [31:0] reg_dummy;


   //
   // Wires
   //
   wire reg_status;


   //
   // ECDSA256
   //
   ecdsa256 ecdsa256_inst
	(
      .clk                      (clk),
		.rst_n                    (rst_n),

      .next                     (reg_control),
      .valid                    (reg_status),

      .bus_cs                   (cs && (addr_msb == ADDR_MSB_CORE)),
      .bus_we                   (we),
      .bus_addr                 (addr_lsb),
      .bus_data_wr              (write_data),
      .bus_data_rd              (read_data_core)
	);


   //
   // Read Latch
   //
   reg [31: 0]         tmp_read_data;


   //
   // Read/Write Interface
   //
   always @(posedge clk)
     //
     if (!rst_n) begin
        //
        reg_control <= 1'b0;
        //
     end else if (cs && (addr_msb == ADDR_MSB_REGS)) begin
        //
        if (we) begin
           //
           // Write Handler
           //
           case (addr_lsb)
             //
             ADDR_CONTROL: reg_control <= write_data[CONTROL_NEXT_BIT];
				 ADDR_DUMMY:   reg_dummy   <= write_data;
             //
           endcase
           //
        end else begin
           //
           // Read Handler
           //
           case (address)
             //
             ADDR_NAME0:        tmp_read_data <= CORE_NAME0;
             ADDR_NAME1:        tmp_read_data <= CORE_NAME1;
             ADDR_VERSION:      tmp_read_data <= CORE_VERSION;
             ADDR_CONTROL:      tmp_read_data <= {{30{1'b0}}, reg_control, 1'b0};
             ADDR_STATUS:       tmp_read_data <= {{30{1'b0}}, reg_status,  1'b1};
				 ADDR_DUMMY:        tmp_read_data <= reg_dummy;
             //
             default:           tmp_read_data <= 32'h00000000;
             //
           endcase
           //
        end
        //
     end


   //
   // Register / Core Memory Selector
   //
   reg addr_msb_last;
   always @(posedge clk) addr_msb_last <= addr_msb;

   assign read_data = (addr_msb_last == ADDR_MSB_REGS) ? tmp_read_data : read_data_core;


endmodule
