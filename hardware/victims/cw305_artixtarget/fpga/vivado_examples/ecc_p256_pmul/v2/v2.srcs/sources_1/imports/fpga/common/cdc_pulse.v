`default_nettype none
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: NewAE
// Engineer: Jean-Pierre Thibault
// 
// Create Date: 
// Design Name: 
// Module Name: cdc_pulse
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module cdc_pulse #(
   parameter pSYNC_STAGES = 2
)(
   input  wire         reset_i,
   input  wire         src_clk,
   input  wire         src_pulse,
   input  wire         dst_clk,
   output reg          dst_pulse
);

   (* ASYNC_REG = "TRUE" *) reg  [pSYNC_STAGES-1:0] req_pipe = 0;
   (* ASYNC_REG = "TRUE" *) reg  [pSYNC_STAGES-1:0] ack_pipe = 0;
   reg  src_req = 0;
   reg  dst_req = 0;
   reg  dst_req_r;
   reg  src_ack;
   wire busy;

   always @(posedge src_clk) begin
      if (reset_i) begin
         src_ack <= 0;
         ack_pipe <= 0;
         src_req <= 0;
      end
      else begin
         {src_ack, ack_pipe} <= {ack_pipe, dst_req};
         if (~busy & src_pulse)
            src_req <= 1'b1;
         else if (src_ack)
            src_req <= 1'b0;
         end
   end

   assign busy = src_req | src_ack;

   always @(posedge dst_clk) begin
      if (reset_i) begin
         dst_req_r <= 0;
         dst_req <= 0;
         req_pipe <= 0;
         dst_pulse <= 0;
      end
      else begin
         {dst_req_r, dst_req, req_pipe} <= {dst_req, req_pipe, src_req};
         dst_pulse <= ~dst_req_r & dst_req;
      end
   end

endmodule

`default_nettype wire
