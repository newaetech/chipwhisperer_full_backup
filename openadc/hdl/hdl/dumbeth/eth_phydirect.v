`timescale 1ns / 1ps
/***********************************************************************
This file is part of the OpenADC Project. See www.newae.com for more details,
or the codebase at http://www.assembla.com/spaces/openadc .

This file is the UDP interface. There is three main parts to this interface:

1) The PHY interface lines
2) The high-level data interface
3) Configuration information

This is a very dumb module. It doesn't configure the PHY, and assumes you've
used bootstrapping pins to get everything set up properly.

Copyright (c) 2012, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
This project is released under the Modified FreeBSD License. See LICENSE
file which should have came with this code.
*************************************************************************/

module eth_phydirect(
	 input		 reset_i,      //Active high reset input

    input 		 eth_col,      //Connect to PHY
    input 		 eth_crs,      //Connect to PHY
	 output 		 eth_mdc,      //Connect to PHY
	 inout  		 eth_mdio,     //Connect to PHY
	 
	 output      eth_reset_n,  //Connect to PHY
	 
	 input 		 eth_rx_clk,   //Connect to PHY
	 input [3:0] eth_rx_data,  //Connect to PHY
	 input       eth_rx_dv,    //Connect to PHY
	 input       eth_rx_er,    //Connect to PHY
	 
	 input       eth_tx_clk,   //Connect to PHY
	 output[3:0] eth_tx_data,  //Connect to PHY
	 output      eth_tx_en,    //Connect to PHY
	 
	 output		  usr_clk_o,   //Data at usr_data_i must be valid on falling edge
									   //You can just use the rising edge to clock new data out
	 output		  usr_clken_o, //Valid on rising edge of usr_clk_o
	 input		  usr_start_i, //Set this to '1' to start a new capture, some time later
										//usr_clken_o will be high when your payload should be sent.
										//Before setting this high be sure everything else has already
										//been configured or bad things happen!
	 output       usr_done_o,
	 input [47:0] usr_ethsrc_i,
	 input [47:0] usr_ethdst_i,
	 input [31:0] usr_ipsrc_i,
	 input [31:0] usr_ipdst_i,
	 input [15:0] usr_data_len_i,
	 input [15:0] usr_udpport_i,	  
	 input [7:0]  usr_data_i 
    );
	 
	 wire [3:0] eth_tx_data_reg;
	 
	 reg [1:0] clkdiv_reg;
	 wire tx_clk;
	 
	 assign tx_clk = eth_tx_clk;
	 	 
	 assign eth_reset_n = ~reset_i;	 
	 	 
	 reg usr_datard;
	 assign usr_datard_o = usr_datard;
	 
	 reg usr_done_reg;
	 assign usr_done_o = usr_done_reg;
	   
	 assign eth_mdc = 1'b0;
	 assign eth_mdio = 1'bZ;
	 
	 reg [7:0] tx_data;
	 reg tx_nibble_cnt;
	  	 
	 /* Follow EN logic is designed to ensure EN stays high until final
       nibble is transmitted over the wire */
	 reg tx_en;	 
	 reg tx_en_rst;
	 reg eth_tx_en_reg;
	 
	 always @(negedge tx_clk) begin
		if (tx_en) begin
			tx_en_rst <= 1'b1;
			eth_tx_en_reg <= 1'b1;
		end else begin
			tx_en_rst <= 1'b0;
			eth_tx_en_reg <= ~((~tx_en) | tx_en_rst);
		end
	end
	
	assign eth_tx_en = eth_tx_en_reg;
	
	 
	 always @(posedge tx_clk) begin
		tx_nibble_cnt <= ~tx_nibble_cnt;
	 end
	 
	 assign eth_tx_data_reg = tx_nibble_cnt ? tx_data[7:4] : tx_data[3:0];
	 assign eth_tx_data = eth_tx_data_reg;
	 
	 wire [7:0] dest_address [0:5];
	 wire [7:0] src_address [0:5];
	 wire [7:0] fcs;
	 
	 assign dest_address[0] = 8'hD0;
	 assign dest_address[1] = 8'h67;
	 assign dest_address[2] = 8'hE5;
	 assign dest_address[3] = 8'h45;
	 assign dest_address[4] = 8'h51;
	 assign dest_address[5] = 8'h71;
	 
	 assign src_address[0] = 8'h00;
	 assign src_address[1] = 8'h02;
	 assign src_address[2] = 8'h03;
	 assign src_address[3] = 8'h04;
	 assign src_address[4] = 8'h05;
	 assign src_address[5] = 8'h06;	
	 
	 `define PHY_IDLE            'b0000
	 `define PHY_WAIT            'b0001
	 `define PHY_PREAMBLE		  'b0010
	 `define MAC_DST				  'b0011
	 `define MAC_SRC				  'b0100
	 `define MAC_TYPE				  'b0101
	 `define MAC_PAYLOAD         'b0110
	 `define MAC_FCS             'b0111
	 
	 
	 /* UDP Header */
	 wire [7:0] udp_hdr [0:7];
	 wire [15:0] udp_total_len;
	 assign udp_total_len = usr_data_len_i + 16'd8;
	 assign udp_hdr[0] = 8'h00; //source port unused
	 assign udp_hdr[1] = 8'h00; //source port unused
	 assign udp_hdr[2] = usr_udpport_i[15:8];
	 assign udp_hdr[3] = usr_udpport_i[7:0];
	 assign udp_hdr[4] = udp_total_len[15:8];
	 assign udp_hdr[5] = udp_total_len[7:0];
	 assign udp_hdr[6] = 8'h00; //checksum unused
	 assign udp_hdr[7] = 8'h00;

	 /* IPv4 Header */
	 wire [7:0] ip_hdr [0:19];
	 wire [15:0] ip_total_len;
	 wire [15:0] ip_hdr_checksum;
	 assign ip_total_len = udp_total_len + 16'd20; 	 
	 assign ip_hdr[0] = 8'h45;
	 assign ip_hdr[1] = 8'h00;
	 assign ip_hdr[2] = ip_total_len[15:8];
	 assign ip_hdr[3] = ip_total_len[7:0];
	 assign ip_hdr[4] = 8'h00;
	 assign ip_hdr[5] = 8'h00;
	 assign ip_hdr[6] = 8'h00;
	 assign ip_hdr[7] = 8'h00;
	 assign ip_hdr[8] = 8'h80;
	 assign ip_hdr[9] = 8'h11;
	 assign ip_hdr[10] = ip_hdr_checksum[15:8]; //Checksum MSB
	 assign ip_hdr[11] = ip_hdr_checksum[7:0]; //Checksum LSB
	 assign ip_hdr[12] = usr_ipsrc_i[31:24];
	 assign ip_hdr[13] = usr_ipsrc_i[23:16];
	 assign ip_hdr[14] = usr_ipsrc_i[15:8];
	 assign ip_hdr[15] = usr_ipsrc_i[7:0];
	 assign ip_hdr[16] = usr_ipdst_i[31:24];
	 assign ip_hdr[17] = usr_ipdst_i[23:16];
	 assign ip_hdr[18] = usr_ipdst_i[15:8];
	 assign ip_hdr[19] = usr_ipdst_i[7:0];
	 
	 /* IPv4 Header Checksum Calculation */
	 reg [23:0] ip_chksum_int1;
	 reg [5:0]  ip_chksum_cnt;
	 
	 always @(posedge tx_clk) begin
		if (crc_init) begin
			ip_chksum_int1 <= 24'h000000;
			ip_chksum_cnt <= 6'd0;
		end else begin		
			if (ip_chksum_cnt == 6'd20) begin
				ip_chksum_cnt <= 6'd20;
			end else begin
				ip_chksum_cnt <= ip_chksum_cnt + 6'd2;
				ip_chksum_int1 <= ip_chksum_int1 + {ip_hdr[ip_chksum_cnt], ip_hdr[ip_chksum_cnt+1]};
			end			
		end
	 end
	 
	 assign ip_hdr_checksum = (ip_chksum_cnt == 6'd20) ? ~(ip_chksum_int1[23:16] + ip_chksum_int1[15:0]) : 16'h0000;
	 
	 `define IP_HDR				  'b1000
	 `define UDP_HDR             'b1001
	 `define UDP_PAYLOAD         'b1010
	  
	 reg [3:0] state;
	 reg [15:0] stcnt;	
	 
	 //State machine
	 always @(negedge tx_clk) begin
		if (reset_i) begin
			state <= `PHY_IDLE;
			stcnt <= 16'd0;
			tx_en <= 1'b0;
			usr_clken_reg <= 1'b0;
			usr_done_reg <= 1'b0;
		end else if (tx_nibble_cnt) begin
			case (state) 
				`PHY_IDLE: begin
					tx_data <= 8'h55; /* 0x55 Preamble */
					if (stcnt == 16'd45) begin
						state <= `PHY_WAIT;
						stcnt <= 16'd0;
					end else begin
						stcnt <= stcnt + 16'd1;
					end	
					tx_en <= 1'b0;					
					crc_calc <= 1'b0;
					crc_init <= 1'b1;
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`PHY_WAIT: begin
					tx_data <= 8'h55; /* 0x55 Preamble */
					if (usr_start_i == 1'b1) begin
						state <= `PHY_PREAMBLE;
						tx_en <= 1'b1;
					end else begin
						tx_en <= 1'b0;
					end	
					stcnt <= 16'd0;					
					crc_calc <= 1'b0;
					crc_init <= 1'b1;
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`PHY_PREAMBLE: begin
					if (stcnt == 16'd7) begin
						stcnt <= 16'd0;
						state <= `MAC_DST;
						tx_data <= 8'hD5; /* 0xD5 SFD */
					end else begin
						tx_data <= 8'h55;
						stcnt <= stcnt + 16'd1;
						state <= `PHY_PREAMBLE;
					end
					tx_en <= 1'b1;
					crc_calc <= 1'b0;
					crc_init <= 1'b0;
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`MAC_DST: begin
					if (stcnt == 16'd5) begin
						stcnt <= 16'd0;
						state <= `MAC_SRC;
					end else begin					
						stcnt <= stcnt + 16'd1;
						state <= `MAC_DST;
					end
					tx_data <= dest_address[stcnt];
					tx_en <= 1'b1;	
					crc_calc <= 1'b1;
					crc_init <= 1'b0;		
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`MAC_SRC: begin
					if (stcnt == 16'd5) begin
						stcnt <= 16'd0;
						state <= `MAC_TYPE;
					end else begin					
						stcnt <= stcnt + 16'd1;
						state <= `MAC_SRC;
					end
					tx_data <= src_address[stcnt];
					tx_en <= 1'b1;	
					crc_calc <= 1'b1;
					crc_init <= 1'b0;		
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`MAC_TYPE: begin
					if (stcnt == 16'd1) begin
						stcnt <= 16'd0;
						tx_data <= 8'h00;
						state <= `IP_HDR; /* `MAC_PAYLOAD; */
					end else begin					
						stcnt <= stcnt + 16'd1;
						tx_data <= 8'h08;
						state <= `MAC_TYPE;
					end					
					tx_en <= 1'b1;
					crc_calc <= 1'b1;
					crc_init <= 1'b0;	
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;					
				end
				
				/*
				`MAC_PAYLOAD: begin
					if (stcnt == 16'd99) begin
						stcnt <= 16'd0;
						state <= `MAC_FCS;
					end else begin					
						stcnt <= stcnt + 16'd1;
					end
					tx_data <= 8'hCC;
					tx_en <= 1'b1;		
					crc_calc <= 1'b1;
					crc_init <= 1'b0;						
				end
				*/
				
				`IP_HDR: begin
					if (stcnt == 16'd19) begin
						stcnt <= 16'd0;
						state <= `UDP_HDR;
					end else begin
						stcnt <= stcnt + 16'd1;
					end
					tx_data <= ip_hdr[stcnt];
					tx_en <= 1'b1;
					crc_calc <= 1'b1;
					crc_init <= 1'b0;
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`UDP_HDR: begin
					if (stcnt == 16'd7) begin
						stcnt <= 16'd0;
						state <= `UDP_PAYLOAD;
						usr_clken_reg <= 1'b1;
					end else begin
						stcnt <= stcnt + 16'd1;
						usr_clken_reg <= 1'b0;
					end
					tx_data <= udp_hdr[stcnt];
					tx_en <= 1'b1;
					crc_calc <= 1'b1;
					crc_init <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`UDP_PAYLOAD: begin
					if (stcnt == (usr_data_len_i - 1)) begin
						stcnt <= 16'd0;
						state <= `MAC_FCS;
						usr_clken_reg <= 1'b0;
					end else begin
						stcnt <= stcnt + 16'd1;
						usr_clken_reg <= 1'b1;
					end
					tx_data <= usr_data_i;
					tx_en <= 1'b1;
					crc_calc <= 1'b1;
					crc_init <= 1'b0;
					usr_done_reg <= 1'b0;
				end
				
				`MAC_FCS: begin
					if (stcnt == 16'd3) begin
						stcnt <= 16'd0;
						state <= `PHY_IDLE;
					end else begin					
						stcnt <= stcnt + 16'd1;
						state <= `MAC_FCS;
					end
					tx_data <= fcs;
					tx_en <= 1'b1;	
					crc_calc <= 1'b0;
					crc_init <= 1'b0;	
					usr_clken_reg <= 1'b0;	
					usr_done_reg <= 1'b1;
				end
				
				default: begin
					state <= `PHY_IDLE;
					tx_data <= 8'd0;
					tx_en <= 1'b0;
					stcnt <= 16'd0;
					usr_clken_reg <= 1'b0;
					usr_done_reg <= 1'b0;					
				end
			endcase
		end
	 end	 
	 
	 reg crc_calc;
	 reg crc_init;
	 
	 crc32 crc32_eth (
		.crc(fcs),
		.d(tx_data),
		.calc(crc_calc),
		.init(crc_init),
		.d_valid(tx_nibble_cnt),
		.clk(tx_clk),
		.reset(reset_i)
   );
	 
	 reg usr_clken_reg;
	 reg usr_clken_reg2;
	 always @(negedge tx_clk) begin
		usr_clken_reg2 <= usr_clken_reg & tx_nibble_cnt;
	 end
	 assign usr_clken_o = usr_clken_reg2;
	 assign usr_clk_o = tx_clk;
	 
	wire [35:0]                          chipscope_control;
	coregen_icon icon (
    .CONTROL0(chipscope_control) // INOUT BUS [35:0]
   ); 
  
	wire [127:0] cs_data;
    
   coregen_ila ila (
    .CONTROL(chipscope_control), // INOUT BUS [35:0]
    .CLK(tx_clk), // IN
    .TRIG0(cs_data) // IN BUS [127:0]
   );
	
	assign cs_data[3:0] = eth_tx_data_reg;
	assign cs_data[7:4] = state;
	assign cs_data[31:16] = stcnt;
	assign cs_data[32] = reset_i;
	assign cs_data[33] = tx_en;
	assign cs_data[34] = tx_nibble_cnt;

endmodule
