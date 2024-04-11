//////////////////////////////////////////////////////////////////////////////////
// Simple Serial Interface
//
// This function implements the SimpleSerial v1.0 protocol.
//
//////////////////////////////////////////////////////////////////////////////////

module SimpleSerial(
    /* Hardware */    
    input clk,
    output TxD,
    input  RxD,
    
    output LED_rx,
    output LED_go,
    
    /* AES Block Interface */
    output [127:0] pt,
    output [127:0] key,
    output reg     load_key,
    output reg     load_pt,
    output reg     do_enc,
    
    input [127:0]  ct,
    input          ct_ready    
);
    /* Serial interface */
    wire TxD_busy;
    wire RxD_data_ready;
    wire [7:0] RxD_data;
    reg [7:0] txd_buffer;
    reg TxD_start;
    async_receiver RX(.clk(clk), .RxD(RxD), .RxD_data_ready(RxD_data_ready), .RxD_data(RxD_data));
    async_transmitter TX(.clk(clk), .TxD(TxD), .TxD_start(TxD_start), .TxD_data(txd_buffer), .TxD_busy(TxD_busy));

    /* Encryption go command generated automatically */
    reg [7:0] gocnt;
    always @(posedge clk) begin
        if (load_pt == 1'b1) begin
            gocnt <= 1;
        end else begin
            if (gocnt > 0) begin
                gocnt <= gocnt + 1;
            end else begin
                gocnt <= 0;
            end 
        end
    end
    
	 //Adjust when 'gocnt' happens if you want
    always @(posedge clk) begin
        if (gocnt == 8'hFF) begin
            do_enc <= 1;
        end else begin
            do_enc <= 0;
        end
    end
    

    /* Stretch pulses on RX & go pins for LEDs to human lengths */
    reg [12:0] stretched_ledrx;
    reg led_rx_reg;
    assign LED_rx = led_rx_reg;
    always @(posedge clk) begin
        if (RxD_data_ready) begin
            stretched_ledrx <= 1;
            led_rx_reg <= 1;
        end else begin
            if (stretched_ledrx == 0) begin
                led_rx_reg <= 0;
                stretched_ledrx <= 0;
            end else begin
                led_rx_reg <= 1;
                stretched_ledrx <= stretched_ledrx + 1;
            end
        end
    end
    
    reg [12:0] stretched_ledgo;
    reg led_go_reg;
    assign LED_go = led_go_reg;
    always @(posedge clk) begin
        if (do_enc) begin
            stretched_ledgo <= 1;
            led_go_reg <= 1;
        end else begin
            if (stretched_ledgo == 0) begin
                led_go_reg <= 0;
                stretched_ledgo <= 0;
            end else begin
                led_go_reg <= 1;
                stretched_ledgo <= stretched_ledgo + 1;
            end
        end
        end
    
    
    
    /*** Simpleserial v1.0 Protocol Engine ****/
   localparam IDLE =   4'b0000;
   localparam KEY_RX = 4'b0001;
   localparam PT_RX =  4'b0010;
   localparam CT_TX =  4'b0011;
        
   wire [7:0] rxdata;
   wire       rxd_ready;
   
   assign rxdata = RxD_data;
   assign rxd_ready = RxD_data_ready;

   reg [127:0] ss_key;
   reg [127:0] ss_pt;
   reg [127:0] ss_ct;
   
   always @(posedge clk) begin
    if (ct_ready) begin
        ss_ct <= ct;        
    end
   end
   
   always @(posedge clk) resp_ready <= ct_ready;

   assign key = ss_key;
   assign pt = ss_pt;
   
   reg resp_ready;

   reg [3:0] state;
   reg [3:0] next_state;
    
   reg [7:0] rx_data_hex;
   reg rx_data_newline;
   reg rx_data_invalid;
   
   reg txd_done;
   
   //Count RX 'nibbles', i.e., part of an ASCII hex character
   reg [5:0] nibble_cnt;
    always @(posedge clk) begin
        if ((state == KEY_RX) || (state == PT_RX)) begin
            if (rxd_ready) begin
                nibble_cnt <= nibble_cnt + 1;
            end else begin
                nibble_cnt <= nibble_cnt;
            end
        end else begin
            nibble_cnt <= 0;
        end
   end
   
   reg [5:0] txd_nibble_cnt;
     
   reg [7:0] txdata_ascii;
   wire [3:0] txdata_hex;
   //txd_nibble_cnt starts at '1' as the first nibble is the 'r' character
   assign txdata_hex = ss_ct[ ((31-(txd_nibble_cnt-1))*4 + 3) -: 4 ];
   always @* begin
     if (txdata_hex < 4'h0A) begin
        txdata_ascii = txdata_hex + 8'h30;
     end else begin
        txdata_ascii = (txdata_hex - 4'h0A) + 8'h61;
     end
   end
   
   //The format of response is "rXXXXXXXXXX\n"
   always @(posedge clk) begin
    if(txd_nibble_cnt == 0) begin
        txd_buffer <= 8'h72;
    end else if (txd_nibble_cnt == 33) begin
        txd_buffer <= 8'h0A;
    end else if(txd_nibble_cnt < 33) begin
        txd_buffer <= txdata_ascii;
    end
    end
    
   always @(posedge clk) begin
    if (state == IDLE) begin
        txd_nibble_cnt <= 0;
    end else if (TxD_start) begin
        txd_nibble_cnt <= txd_nibble_cnt + 1;
    end
   end
   
   always @(posedge clk) begin
    if (state == IDLE) begin
        txd_done <= 1'b0;
        TxD_start <= 1'b0;
    end else begin
        if ((state == CT_TX) && (TxD_busy == 1'b0) && (TxD_start == 1'b0) && (txd_nibble_cnt == 6'h22)) begin
            TxD_start <= 1'b0;
            txd_done <= 1'b1;
        end else if ((state == CT_TX) && (TxD_busy == 1'b0) && (TxD_start == 1'b0)) begin
            TxD_start <= 1'b1;
            txd_done <= 1'b0;
        end else begin
            TxD_start <= 1'b0;
            txd_done <= 1'b0;
        end
    end
   end 

   
   always @(posedge clk) begin
    if ((state == KEY_RX) && (rx_data_newline) && (nibble_cnt == 6'h20)) begin
        load_key <= 1'b1;
    end else begin
        load_key <= 1'b0;
    end
   end

   always @(posedge clk) begin
    if ((state == PT_RX) && (rx_data_newline) && (nibble_cnt == 6'h20)) begin
        load_pt <= 1'b1;
    end else begin
        load_pt <= 1'b0;
    end
   end

   always @(posedge clk) begin
    if((state == PT_RX) && (rxd_ready == 1'b1) && (nibble_cnt < 32)) begin
        ss_pt[ ((31-nibble_cnt)*4 + 3) -: 4 ] <= rx_data_hex;
    end
   end

   always @(posedge clk) begin
    if((state == KEY_RX) && (rxd_ready == 1'b1) && (nibble_cnt < 32)) begin
        ss_key[ ((31-nibble_cnt)*4 + 3) -: 4 ] <= rx_data_hex;
    end
   end
   always @(posedge clk) begin
    state <= next_state;
   end


   always @* begin
      if ((rxdata >= 8'h30) && (rxdata <= 8'h39)) begin
          //0 - 9
          rx_data_hex = rxdata - 8'h30;
          rx_data_newline = 0;
          rx_data_invalid = 0;
      end else if ((rxdata >= 8'h61) && (rxdata <= 8'h68)) begin
         //a-h
         rx_data_hex = rxdata - (8'h61 - 8'h0A);
         rx_data_newline = 0;
         rx_data_invalid = 0;
      end else if ((rxdata >= 8'h41) && (rxdata <= 8'h48)) begin
         //A-H
        rx_data_hex = rxdata - (8'h41 - 8'h0A);
        rx_data_newline = 0;
        rx_data_invalid = 0;
      end else if (rxdata == 8'h0A) begin
        rx_data_hex = 8'h00;
        rx_data_newline = 1;
        rx_data_invalid = 0;
      end else begin
        rx_data_newline = 0;
        rx_data_invalid = 1;
        rx_data_hex = 8'h00;
      end
   end
 
   always @* begin
       case(state)
            IDLE: if (rxd_ready == 1'b1) begin
                        if (rxdata == 8'h6B) begin //'k'
                            next_state = KEY_RX;
                        end else if (rxdata == 8'h70) begin //'p'
                            next_state = PT_RX;
                        end else begin
                            next_state = IDLE;
                        end
                  end else if (resp_ready) begin
                    next_state = CT_TX; 
                  end else begin
                    next_state = IDLE;
                  end
                  
            KEY_RX:  if (rxd_ready == 1'b1) begin
                        if ((rx_data_newline == 1'b1) || (rx_data_invalid == 1'b1)) begin
                            next_state = IDLE;
                        end else begin
                            next_state = KEY_RX;
                        end
                     end else begin
                        next_state = KEY_RX;
                     end

            PT_RX:  if (rxd_ready == 1'b1) begin
                       if ((rx_data_newline == 1'b1) || (rx_data_invalid == 1'b1)) begin
                           next_state = IDLE;
                       end else begin
                           next_state = PT_RX;
                       end
                    end else begin
                       next_state = PT_RX;
                    end
                  
            CT_TX: if (txd_done == 1'b1) begin
                        next_state = IDLE;
                   end else begin
                        next_state = CT_TX;
                   end
                  
            default: next_state = IDLE; 
        endcase
    end          

endmodule