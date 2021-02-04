/****************************************************************************************
*
*    File Name:  mobile_ddr.v
*      Version:  5.90
*        Model:  BUS Functional
*
* Dependencies:  lpddr_model_parameters.vh
*
*  Description:  Micron MOBILE DDR SDRAM
*
*   Limitation:  - Doesn't check for 8K-cycle refresh
*
*         Note:  - Set simulator resolution to "ps" accuracy
*                - Set Debug = 0 to disable $display messages
*                - Model assume Clk and Clk# crossing at both edge
*
*   Disclaimer   This software code and all associated documentation, comments or other 
*  of Warranty:  information (collectively "Software") is provided "AS IS" without 
*                warranty of any kind. MICRON TECHNOLOGY, INC. ("MTI") EXPRESSLY 
*                DISCLAIMS ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
*                TO, NONINFRINGEMENT OF THIRD PARTY RIGHTS, AND ANY IMPLIED WARRANTIES 
*                OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE. MTI DOES NOT 
*                WARRANT THAT THE SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE 
*                OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE. 
*                FURTHERMORE, MTI DOES NOT MAKE ANY REPRESENTATIONS REGARDING THE USE OR 
*                THE RESULTS OF THE USE OF THE SOFTWARE IN TERMS OF ITS CORRECTNESS, 
*                ACCURACY, RELIABILITY, OR OTHERWISE. THE ENTIRE RISK ARISING OUT OF USE 
*                OR PERFORMANCE OF THE SOFTWARE REMAINS WITH YOU. IN NO EVENT SHALL MTI, 
*                ITS AFFILIATED COMPANIES OR THEIR SUPPLIERS BE LIABLE FOR ANY DIRECT, 
*                INDIRECT, CONSEQUENTIAL, INCIDENTAL, OR SPECIAL DAMAGES (INCLUDING, 
*                WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS INTERRUPTION, 
*                OR LOSS OF INFORMATION) ARISING OUT OF YOUR USE OF OR INABILITY TO USE 
*                THE SOFTWARE, EVEN IF MTI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH 
*                DAMAGES. Because some jurisdictions prohibit the exclusion or 
*                limitation of liability for consequential or incidental damages, the 
*                above limitation may not apply to you.
*
*                Copyright 2008 Micron Technology, Inc. All rights reserved.
*
* Rev  Author Date        Changes
* ---  ------ ----------  ---------------------------------------
* 1.0  NMB    03/19/02    - Initial Release of Mobile DDR model
*                           based off of version 5.0 of DDR model
* 1.1  ritz   12/03/04    - New feature:  1/8th strength driver in Drive Strength (Ext Mode Reg).
*                           Bugfix - ba[0] ba[1] were swapped for determening ext_mode_enable
*                           thus ext_mode_reg wasnt being programmed.
* 1.2  ritz   12/07/04    - Logging transactions in transcript for automated testing
* 1.3  ritz   01/31/05    - updated to SMG DDR model version 5.2 (dqs edge checking errors fix)
* 1.4  ritz   02/15/05    - Fixed display.*WRITE to use hex for "data".
* 1.5  ritz   03/22/05    - Fixed read latency (#0.5 and 2*Read_latency-1) for MMG latency
* 2.0  bas    07/19/06    - Added PASR support and clk_n checking
* 3.0  bas    08/07/06    - Added tXP check, tCke check, Power-down/Deep power down enter/exit messages
                            FULL_MEM fix
* 3.11 bas    10/18/06    - Added clk spd chk, added RP support, added T48M part, added SRR functionality, changed tMRD checker to measure in tck pos edge, DPD optimization for FULL_MEM mode
* 3.12 bas    10/19/06    - Fixed PASR in FULL_MEM mode
* 3.20 bas    10/23/06    - changed tXP check to tPDX check for T25L, Initial release to web
* 3.30 bas    01/15/07    - Updated T48M Parameters (updated as of 12/06)
* 3.35 bas    02/28/07    - Model uses tAC correctly to calculate strobe/data launch
* 3.36 bas    03/05/07    - fixed error messages for different banks interrupting 
                            reads/writes w/autoprecharge
* 3.37 bas    03/21/07    - Added T47M Part to 512Mb parameter file
* 3.40 bas    06/25/07    - Removed RP options from 1024Mb
                          - Updated 128Mb, 256Mb, and 512Mb parts to 05/07 datasheet
                          - Updated 1024Mb part to 02/07
                          - Added illegal Cas Latency check per speed grade
* 3.40 jwm    08/02/07    - Support for 512Mb T47M
* 3.80 clk    10/29/07    - Support for 1024Mb T48M
* 4.00 clk    12/30/07    - Fixed Read terminated by precharge testcase
* 4.70 clk    03/30/08    - Fixed typo in SRR code
* 4.80 clk    04/03/08    - Disable clk checking during initialization
* 4.90 clk    04/16/08    - Fixed tInit, added mpc support, updated t35m timing
* 5.00 clk    05/14/08    - Fixed back to back auto precharge commands          
* 5.20 clk    05/21/08    - Fixed read interrupt by pre (BL8), fixed 1024Mb parameter file
* 5.30 clk    05/22/08    - Fixed DM signal which cause false tWTR errors                    
              05/27/08    - Rewrote write and read pipelins, strobes
* 5.40 clk    05/28/08    - Fixed Addressing problem in Burst Order logic
* 5.50 clk    07/25/08    - Added T36N part type                                   
* 5.60 clk    09/05/08    - Fixed tXP in 256Mb part type                           
* 5.70 clk    09/17/08    - Fixed burst term check for write w/ all DM active       
* 5.80 clk    11/18/08    - Fixed internally latched dq & mask widths
* 5.90 clk    12/10/08    - Updated T36N parameters to latest datasheet
****************************************************************************************/

// DO NOT CHANGE THE TIMESCALE
// MAKE SURE YOUR SIMULATOR USES "PS" RESOLUTION
`timescale 1ns / 1ps

module lpddr_model_c3(Dq, Dqs, Addr, Ba, Clk, Clk_n, Cke, Cs_n, Ras_n, Cas_n, We_n, Dm);

    `include "lpddr_model_parameters_c3.vh"
    `define MAX_PIPE   2*(CL_MAX + BL_MAX)

    // Port Declarations
    input                         Clk;
    input                         Clk_n;
    input                         Cke;
    input                         Cs_n;
    input                         Ras_n;
    input                         Cas_n;
    input                         We_n;
    input     [ADDR_BITS - 1 : 0] Addr;
    input                 [1 : 0] Ba;
    inout       [DQ_BITS - 1 : 0] Dq;
    inout      [DQS_BITS - 1 : 0] Dqs;
    input       [DM_BITS - 1 : 0] Dm;

    //time variables
    realtime tXP_chk ;
    reg  enter_DPD   ;
    reg  enter_PD    ;
    reg  enter_APD   ;

    //integer clk checks

    // Internal Wires (fixed width)
    wire                 [31 : 0] Dq_in;
    wire                  [3 : 0] Dqs_in;
    wire                  [3 : 0] Dm_in;
    
    assign Dq_in   [DQ_BITS - 1 : 0] = Dq;
    assign Dqs_in [DQS_BITS - 1 : 0] = Dqs;
    assign Dm_in   [DM_BITS - 1 : 0] = Dm;

    // Data pair
    reg           [DQ_BITS-1 : 0] dq_rise;
    reg           [DM_BITS-1 : 0] dm_rise;
    reg           [DQ_BITS-1 : 0] dq_fall;
    reg           [DM_BITS-1 : 0] dm_fall;
    reg         [DM_BITS*2-1 : 0] dm_pair;
    reg           [DQ_BITS-1 : 0] Dq_buf;

    // Power-down cycle counter
    reg  [03:00]                  PD_cntr    ;

    // prev cmd value

    reg                           prev_Cs_n  ;
    reg                           prev_Ras_n ;
    reg                           prev_Cas_n ;
    reg                           prev_We_n  ;
    reg  [01:00]                  prev_Ba    ;
    reg                           prev_cke   ;

    wire prev_nop = ~prev_Cs_n &  prev_Ras_n &  prev_Cas_n &  prev_We_n ;
    wire prev_des =  prev_Cs_n ;
    wire prev_bt  = ~prev_Cs_n &  prev_Ras_n &  prev_Cas_n & ~prev_We_n ;
    wire prev_rd  = ~prev_Cs_n &  prev_Ras_n & ~prev_Cas_n &  prev_We_n ;
    reg       Clk_Chk_enable  = 1'b0 ;

    //differential clk
    reg            diff_ck;
    always @(posedge Clk)   diff_ck <= Clk;
    always @(posedge Clk_n) diff_ck <= ~Clk_n;

    //measure clock period
    realtime clk_period ;
    realtime pos_clk_edge ;
    integer  clk_pos_edge_cnt ;
    always @(posedge diff_ck) begin
        clk_period = $realtime - pos_clk_edge ;
        pos_clk_edge = $realtime ;
        if ((Cke == 1'b1) && (clk_pos_edge_cnt < 2)) begin
            clk_pos_edge_cnt = clk_pos_edge_cnt + 1 ;
        end else if (Cke == 1'b0) begin
            clk_pos_edge_cnt = 2'b00 ;
        end
    end

    //measure duty cycle
    realtime neg_clk_edge ;
    always @(negedge diff_ck) begin
        neg_clk_edge = $realtime ;
    end
    realtime pos_clk_time ;
    realtime neg_clk_time ;
    always @(diff_ck) begin
        if (diff_ck) begin
            neg_clk_time = $realtime - neg_clk_edge ;
        end
        if (~diff_ck) begin
            pos_clk_time = $realtime - pos_clk_edge ;
        end
        if (Cke & Clk_Chk_enable) begin
            if ((pos_clk_time/clk_period)<tCH_MIN) begin
                $display ("%m: at time %t ERROR: tCH minimum violation on CLK by %t", $time, tCH_MIN*clk_period - pos_clk_time);
            end
            if ((pos_clk_time/clk_period)>tCH_MAX) begin
                $display ("%m: at time %t ERROR: tCH maximum violation on CLK by %t", $time, pos_clk_time - tCH_MAX*clk_period);
            end
            if ((neg_clk_time/clk_period)<tCL_MIN) begin
                $display ("%m: at time %t ERROR: tCL minimum violation on CLK by %t", $time, tCL_MIN*clk_period - pos_clk_time);
            end
            if ((neg_clk_time/clk_period)>tCL_MAX) begin
                $display ("%m: at time %t ERROR: tCL maximum violation on CLK by %t", $time, pos_clk_time - tCL_MAX*clk_period);
            end
        end
    end

    // Mode Register
    reg       [ADDR_BITS - 1 : 0] Mode_reg;
    reg       [ADDR_BITS - 1 : 0] Ext_Mode_reg;
    reg       [2*DQ_BITS - 1 : 0] Srr_reg;


    // SRR Registers
    reg                           SRR_read;

    // Internal System Clock
    reg                           CkeZ, Sys_clk;

    // Internal Dqs initialize
//    reg                           Dqs_int;

    // Dqs buffer
    reg                           Dqs_out;
//    reg        [DQS_BITS - 1 : 0] Dqs_gen;
    reg                           Dqs_out_en;
    // Dq buffer
    reg         [DQ_BITS - 1 : 0] Dq_out_temp;
    reg         [DQ_BITS - 1 : 0] Dq_out;
    reg                           Dq_out_en;

    // Read pipeline variables
    reg           [`MAX_PIPE : 0] Read_pipeline        ;
    reg                   [1 : 0] Read_bank [0 : 6];
    reg        [COL_BITS - 1 : 0] Read_cols [0 : 6];

    // Write pipeline variables
    reg           [`MAX_PIPE :-2] Write_pipeline;
    reg           [BA_BITS-1 : 0] Write_bank_pipeline [`MAX_PIPE :-2];
    reg        [COL_BITS - 1 : 0] Write_col_pipeline [`MAX_PIPE :-2];

    // Auto precharge variables
    reg                     [3:0] Read_precharge_access  ;
    reg                     [3:0] Read_precharge_pre     ;
    reg                     [3:0] Write_precharge_access ;
    reg                     [3:0] Write_precharge_pre    ;
    integer                       Count_precharge [0:3];
    reg                           SelfRefresh;
    reg                     [3:0] Read_precharge_count [3:0] ;
    reg                     [3:0] Write_precharge_count [3:0];


    reg                           wr_ap_display_msg   ;
    reg                           rd_ap_display_msg   ;

    // Manual precharge variables
//    reg                   [0 : 6] A10_precharge         ;
//    reg                   [1 : 0] Bank_precharge [0 : 6];
//    reg                   [0 : 6] Cmnd_precharge        ;

    // Burst terminate variables
//    reg                           Cmnd_bst [0 : 6];

    // tMRD counter
    integer MRD_cntr  ;
    integer SRR_cntr  ;
    integer SRC_cntr  ;
    integer tWTR_cntr ;

    // Memory Banks
    `ifdef FULL_MEM
        reg         [DQ_BITS - 1 : 0] mem_array  [0 : (1<<full_mem_bits)-1];
    `else
        reg         [DQ_BITS - 1 : 0] mem_array  [0 : (1<<part_mem_bits)-1];
        reg   [full_mem_bits - 1 : 0] addr_array [0 : (1<<part_mem_bits)-1];
        reg   [part_mem_bits     : 0] mem_used;
        reg   [part_mem_bits     : 0] memory_index;
        initial mem_used = 0;
    `endif

    // Dqs edge checking
    integer i;
    reg  [3:0] expect_pos_dqs;
    reg  [3:0] expect_neg_dqs;

    // Burst counter
    reg        [COL_BITS - 1 : 0] Burst_counter;

    // Burst counter delay
    reg [COL_BITS - 1 : 0] Burst_counter_dly;
    always @(Mode_reg or Burst_counter) begin
       if (Mode_reg[6:4] == 3'b010) begin
//           Burst_counter_dly <= #tAC2_max Burst_counter;
       end else if (Mode_reg[6:4] == 3'b011) begin
//           Burst_counter_dly <= #tAC3_max Burst_counter;
       end
    end

    // Precharge variables
    reg                           Pc_b0, Pc_b1, Pc_b2, Pc_b3;

    // Activate variables
    reg                           Act_b0, Act_b1, Act_b2, Act_b3;

    // Data IO variables
//    reg                           Data_in_enable;
    wire                          Data_in_enable = Write_pipeline[-2];
//    reg                           Data_out_enable;
    wire                          Data_out_enable = Read_pipeline[0];
    wire                          tWTR_en = ((|Write_pipeline[`MAX_PIPE : 0]) & ~(&Dm));

    // Data Out Enable delay
//    reg Data_out_enable_dly;
//    always @(Mode_reg or Data_out_enable) begin
//       if (Mode_reg[6:4] == 3'b010) begin
//        Data_out_enable_dly <= #tAC2_max Data_out_enable;
//       end else if (Mode_reg[6:4] == 3'b011) begin
//        Data_out_enable_dly <= #tAC3_max Data_out_enable;
//       end
//    end

    // Internal address mux variables
    reg                   [1 : 0] Prev_bank;
    reg                   [1 : 0] Bank_addr;
    reg        [COL_BITS - 1 : 0] Cols_addr, Cols_brst, Cols_temp;
    reg       [ADDR_BITS - 1 : 0] Rows_addr;
    reg       [ADDR_BITS - 1 : 0] B0_row_addr;
    reg       [ADDR_BITS - 1 : 0] B1_row_addr;
    reg       [ADDR_BITS - 1 : 0] B2_row_addr;
    reg       [ADDR_BITS - 1 : 0] B3_row_addr;

    integer                       aref_count;
    reg                           ext_mode_load_done;
    reg                           mode_load_done;
    reg                           power_up_done;

    // Write DQS for tDSS , tDSH, tDQSH, tDQSL checks
    wire      wdqs_valid = Write_pipeline[`MAX_PIPE:0];

    // Commands Decode
    wire      Active_enable   = ~Cs_n & ~Ras_n &  Cas_n &  We_n;
    wire      Aref_enable     = ~Cs_n & ~Ras_n & ~Cas_n &  We_n & Cke;
    wire      Sref_enable     = ~Cs_n & ~Ras_n & ~Cas_n &  We_n & ~Cke;
    wire      Burst_term      = ~Cs_n &  Ras_n &  Cas_n & ~We_n;
    wire      Ext_mode_enable = ~Cs_n & ~Ras_n & ~Cas_n & ~We_n &  Ba[1] & ~Ba[0];
    wire      Mode_reg_enable = ~Cs_n & ~Ras_n & ~Cas_n & ~We_n & ~Ba[1] & ~Ba[0];
    wire      Prech_enable    = ~Cs_n & ~Ras_n &  Cas_n & ~We_n;
    wire      Read_enable     = ~Cs_n &  Ras_n & ~Cas_n &  We_n;
    wire      Write_enable    = ~Cs_n &  Ras_n & ~Cas_n & ~We_n;
    wire      DPD_enable      = ~Cs_n &  Ras_n &  Cas_n & ~We_n & ~Cke;
    wire      PD_enable       = ((~Cs_n &  Ras_n &  Cas_n &  We_n) | Cs_n) & ~Cke;
    wire      nop_enable      = ~Cs_n &  Ras_n &  Cas_n &  We_n ;
    wire      des_enable      =  Cs_n ;
    wire      srr_enable      = ~Cs_n & ~Ras_n & ~Cas_n & ~We_n & ~Ba[1] &  Ba[0] & ((part_size==128)|(part_size==512)|(part_size==1024)) ;

    reg       Cke_Chk = 0      ;
    parameter tInit = 200000   ;
    reg       Cke_Print = 1'b0 ;
    real      Init_Cmd_Chk     ;
    
    // make sure 200 us has passed since cke for first command
    always @ (Clk_Chk_enable or nop_enable or des_enable)
    begin
        if (~Clk_Chk_enable) begin
            if (~nop_enable & ~des_enable) begin
                Clk_Chk_enable = 1'b1 ;
                if (~Cke_Chk) begin
                    if (($realtime - Init_Cmd_Chk < tInit - 0.001) & ~Cke_Print) begin
                        $display("Warning: You must wait 200 us from CKE assertion before you perform a command.  Current wait time: %3.0f ns", $realtime - (Init_Cmd_Chk));                    
                        Cke_Print = 1'b1 ;
                    end
                end                    
            end    
        end
    end

    // Burst Length Decode
//    reg  [4:0] burst_length = 1 << (Mode_reg[2:0]);
    reg  [4:0] burst_length ;
    reg       read_precharge_truncation;

    // CAS Latency Decode
    wire [2:0] cas_latency_x2 = ((2*Mode_reg[6:4])-1);

    real                          tAC_max ;
    always @ (Mode_reg)
    begin
        if (Mode_reg[6:4] == 3'b011)
            tAC_max = tAC3_max ;
        else if (Mode_reg[6:4] == 3'b010)
            tAC_max = tAC2_max ;
    end

    // DQS Buffer
    reg [DQS_BITS - 1 : 0] dqs_delayed ;
//    always@* begin
//       dqs_delayed <= Dqs_out ;
//    end

    assign    Dqs = (Dqs_out_en) ? {DQS_BITS{Dqs_out}} : 'bz;
    assign    Dq  = (Dq_out_en ) ? Dq_out              : 'bz;

    // Debug message
    wire      Debug = 1'b1;

    // Timing Check
//    realtime      MRD_chk;
    realtime      RFC_chk;
    realtime      RRD_chk;
    realtime      RAS_chk0, RAS_chk1, RAS_chk2, RAS_chk3;
    realtime      RAP_chk0, RAP_chk1, RAP_chk2, RAP_chk3;
    realtime      RC_chk0, RC_chk1, RC_chk2, RC_chk3;
    realtime      RCD_chk0, RCD_chk1, RCD_chk2, RCD_chk3;
    realtime      RP_chk0, RP_chk1, RP_chk2, RP_chk3;
    realtime      WR_chk0, WR_chk1, WR_chk2, WR_chk3;
    realtime      SRR_chk;

    // 
    reg       [2:0] current_init_state ;
    parameter [2:0] begin_init      = 3'b000 ;
    parameter [2:0] cke_init        = 3'b001 ;
    parameter [2:0] prech_init      = 3'b010 ;
    parameter [2:0] begin_mode_init = 3'b011 ;
    parameter [2:0] mode_init       = 3'b100 ;
    parameter [2:0] ext_mode_init   = 3'b101 ;
    parameter [2:0] mode_done_init  = 3'b110 ;

    initial begin
        CkeZ = 1'b0;
        Sys_clk = 1'b0;
        {Pc_b0, Pc_b1, Pc_b2, Pc_b3} = 4'b0000;
        {Act_b0, Act_b1, Act_b2, Act_b3} = 4'b1111;
//        Dqs_int = 1'b0;
        Dqs_out_en = {DQS_BITS{1'b0}};
        Dqs_out = {DQS_BITS{1'bz}};
//        Dqs_gen = {DQS_BITS{1'bz}};
        Dq_out = {DQ_BITS{1'bz}};
        Dq_out_en = {DQ_BITS{1'b0}};
//        dq_delayed = {DQ_BITS{1'bz}};
//        Data_in_enable = 1'b0;
//        Data_out_enable = 1'b0;
        aref_count = 0;
        SelfRefresh = 1'b0;
        power_up_done = 0;
        ext_mode_load_done = 0;
        mode_load_done = 0;
//        MRD_chk = 0;
        RFC_chk = 0;
        RRD_chk = 0;
        RAS_chk0 = 0;
        RAS_chk1 = 0;
        RAS_chk2 = 0;
        RAS_chk3 = 0;
        RAP_chk0 = 0;
        RAP_chk1 = 0;
        RAP_chk2 = 0;
        RAP_chk3 = 0;
        RC_chk0 = 0;
        RC_chk1 = 0;
        RC_chk2 = 0;
        RC_chk3 = 0;
        RCD_chk0 = 0;
        RCD_chk1 = 0;
        RCD_chk2 = 0;
        RCD_chk3 = 0;
        RP_chk0 = 0;
        RP_chk1 = 0;
        RP_chk2 = 0;
        RP_chk3 = 0;
        WR_chk0 = 0;
        WR_chk1 = 0;
        WR_chk2 = 0;
        WR_chk3 = 0;
        SRR_chk = 0;
        $timeformat (-9, 3, " ns", 12);
        pos_clk_time = 0;
        neg_clk_time = 0;
        enter_DPD    = 0;
        enter_PD     = 0;
        enter_APD    = 0;
        current_init_state = begin_init ;
        SRR_read  = 1'b0;
        MRD_cntr  = 8;
        SRR_cntr  = 8;
        SRC_cntr  = 8;
        tWTR_cntr = 8;
        Read_precharge_access[0]  = 1'b0 ;
        Read_precharge_access[1]  = 1'b0 ;
        Read_precharge_access[2]  = 1'b0 ;
        Read_precharge_access[3]  = 1'b0 ;
        Read_precharge_pre[0]  = 1'b0 ;
        Read_precharge_pre[1]  = 1'b0 ;
        Read_precharge_pre[2]  = 1'b0 ;
        Read_precharge_pre[3]  = 1'b0 ;
        Write_precharge_access[0] = 1'b0 ;
        Write_precharge_access[1] = 1'b0 ;
        Write_precharge_access[2] = 1'b0 ;
        Write_precharge_access[3] = 1'b0 ;
        Write_precharge_pre[0] = 1'b0 ;
        Write_precharge_pre[1] = 1'b0 ;
        Write_precharge_pre[2] = 1'b0 ;
        Write_precharge_pre[3] = 1'b0 ;
        wr_ap_display_msg  = 1'b0 ;
        rd_ap_display_msg  = 1'b0 ;
        Read_precharge_count[0] = 4'hf;
        Read_precharge_count[1] = 4'hf;
        Read_precharge_count[2] = 4'hf;
        Read_precharge_count[3] = 4'hf;
        Write_precharge_count[0] = 4'hf;
        Write_precharge_count[1] = 4'hf;
        Write_precharge_count[2] = 4'hf;
        Write_precharge_count[3] = 4'hf;
    end

    //clock Frequency Check
    always @(posedge diff_ck) begin
        if (clk_pos_edge_cnt > 1) begin
            if (Mode_reg[6:4] == 3'b011) begin
                if (clk_period < (tCK3_min-0.001)) begin
                    $display ("%m : at time %t ERROR : Illegal clk period for CAS Latency 3", $realtime);
                    $display ("%m : at time %t CLK PERIOD = %t", $realtime, clk_period);
                end
            end
            if (Mode_reg[6:4] == 3'b010) begin
                if (clk_period < (tCK2_min-0.001)) begin
                    $display ("%m : at time %t ERROR : Illegal clk period for CAS Latency 2", $realtime);
                    $display ("%m : at time %t CLK PERIOD = %t", $realtime, clk_period);
                end
            end
        end
    end

    //SRR reg settings
    always @(posedge power_up_done) begin
        Srr_reg        = 'b0 ;
        Srr_reg[3:0]   = 4'b1111 ;                       //Manufacturer(Micron)
        Srr_reg[7:4]   = 4'b0000 ;                       //Revision ID(Default to 0 in model)
        Srr_reg[10:8]  = 3'b100 ;                        //Refresh Rate(based on temp sensor - will default to 1x in model)
        Srr_reg[11]    = (DQ_BITS == 32)? 1'b1 : 1'b0 ;  //Part width(x32 or x16)
        Srr_reg[12]    = 1'b0 ;                          //Device Type (LP DDR)
        Srr_reg[15:13] = (part_size == 1024)? 3'b011 : 
                         (part_size == 512 )? 3'b010 : 
                         (part_size == 256 )? 3'b001 : 
                                              3'b000 ;   //Density(1024Mb, 512Mb, 256Mb, 128Mb)
    end

    // System Clock
    always begin
        @ (posedge diff_ck) begin
            Sys_clk = CkeZ;
            CkeZ = Cke;
        end
        @ (negedge diff_ck) begin
            Sys_clk = 1'b0;
        end
    end

    task store_prev_cmd;
    begin
        prev_Cs_n  <= Cs_n  ;
        prev_Ras_n <= Ras_n ;
        prev_Cas_n <= Cas_n ;
        prev_We_n  <= We_n  ;
        prev_Ba[1] <= Ba[1] ;
        prev_Ba[0] <= Ba[0] ;
        prev_cke   <= Cke   ;
    end
    endtask

    task MRD_counter;
    begin
        if (Cke) begin
            if (MRD_cntr < tMRD) begin
                MRD_cntr = MRD_cntr + 1'b1;
            end
        end
    end
    endtask

    task SRR_counter;
    begin
        if (Cke) begin
            if (SRR_cntr < tSRR) begin
                SRR_cntr = SRR_cntr + 1'b1;
            end
        end
    end
    endtask

    task SRC_counter;
    begin
        if (Cke) begin
            if (SRC_cntr < ((Mode_reg[6:4])+1)) begin
                SRC_cntr = SRC_cntr + 1'b1;
            end
        end
    end
    endtask

    task tWTR_counter;
    begin
        if (Cke) begin
            if (tWTR_en) begin
                tWTR_cntr = 0               ;
            end else begin
                tWTR_cntr = tWTR_cntr + 1'b1;
            end
        end
    end
    endtask

    task command_counter;
    begin
        if (Cke) begin
            for (i=0; i<4;i=i+1) begin
                if (Read_precharge_count[i] < 4'hf) begin
                    Read_precharge_count[i] = Read_precharge_count[i] + 1'b1;
                end
            end
            for (i=0; i<4;i=i+1) begin
                if (Write_precharge_count[i] < 4'hf) begin
                    Write_precharge_count[i] = Write_precharge_count[i] + 1'b1;
                end
            end
        end
    end
    endtask


    task PD_counter;
    begin
        if (~Cke) begin
            if (PD_cntr < tCKE) begin
                PD_cntr = PD_cntr + (enter_DPD | enter_PD | DPD_enable | PD_enable);
            end
        end else begin
            PD_cntr = 4'h0 ;
        end
    end
    endtask

    task tXP_check;
    begin
        if (Cke == 1'b1 && prev_cke == 1'b0) begin
            tXP_chk = $realtime ;
        end
        if (Cke) begin
            if (~nop_enable && ~des_enable) begin
                if ($realtime-tXP_chk < tXP - 0.001) begin
`ifdef T25L
                    $display ("%m: At time %t ERROR: tPDX violation", $realtime);
`else
                    $display ("%m: At time %t ERROR: tXP violation", $realtime);
`endif
                end
            end
        end
    end
    endtask

    // DPD pos edge clk cntr
    always begin
        @ (posedge diff_ck) begin
            tXP_check      ;
            Power_down_chk ;
            PD_counter     ;
            store_prev_cmd ;
        end
    end

    // Check to make sure that we have a Deselect or NOP command on the bus when CKE is brought high
    always @(Cke) begin
        if (Cke === 1'b1) begin
            Init_Cmd_Chk = $realtime ;
            if (SelfRefresh === 1'b1) begin
                SelfRefresh = 1'b0;
            end
            if (!((Cs_n) || (~Cs_n &  Ras_n & Cas_n &  We_n))) begin
                $display ("%m: At time %t MEMORY ERROR:  You must have a Deselect or NOP command applied", $realtime);
                $display ("%m:           when the Clock Enable is brought High.");
            end 
        end
    end

    //BL Mode Reg settings
    always@(Mode_reg[2:0] or mode_load_done) begin
        if (mode_load_done) begin
            case (Mode_reg[2:0])
                3'b001 : burst_length = 5'b00010;
                3'b010 : burst_length = 5'b00100;
                3'b011 : burst_length = 5'b01000;
                3'b100 : burst_length = 5'b10000;
                default : burst_length = 5'bxxxxx;
            endcase
        end
    end

// Init sequence
always @(current_init_state or Cke or Prech_enable or ext_mode_load_done or mode_load_done or aref_count) begin
    if (current_init_state == begin_init) begin
        if (Cke) begin
            current_init_state = cke_init ;
            power_up_done = 1'b0 ;
        end
    end
    if (current_init_state == cke_init) begin
        if (Prech_enable) begin
            current_init_state = prech_init ;
            aref_count = 0 ;
        end
    end
    if (current_init_state == prech_init) begin
        if (~Prech_enable) begin
            current_init_state = begin_mode_init ;
        end
    end
    if (current_init_state == begin_mode_init) begin
        if (ext_mode_load_done) begin
            current_init_state = ext_mode_init ;
        end
        if (mode_load_done) begin
            current_init_state = mode_init ;
        end
    end
    if (current_init_state == mode_init) begin
        if (ext_mode_load_done) begin
            current_init_state = mode_done_init ;
        end
    end
    if (current_init_state == ext_mode_init) begin
        if (mode_load_done) begin
            current_init_state = mode_done_init ;
        end
    end
    if (current_init_state == mode_done_init && aref_count >= 2) begin
        power_up_done = 1'b1;
    end
end


    // this task will erase the contents of 0 or more banks
    task erase_mem;
        input  [BA_BITS+1:0] bank_MSB_row; //bank bits + 2 row MSB
        input                DPD_mode ; //erase all memory locations
        integer i;
        begin

            if (DPD_mode) begin
`ifdef FULL_MEM
                for (i=0; i<{(BA_BITS+ROW_BITS+COL_BITS){1'b1}}; i=i+1) begin
                    mem_array[i] = 'bx;
                end
`else
                memory_index = 0;
                i = 0;
                // remove the selected banks
                for (memory_index=0; memory_index<mem_used; memory_index=memory_index+1) begin
                    addr_array[i] = 'bx;
                    mem_array[i] = 'bx;
                    i = i + 1;
                end
`endif
            end else begin
`ifdef FULL_MEM
                for (i={bank_MSB_row, {(ROW_BITS+COL_BITS-2){1'b1}}}; i<={1'b0, {(BA_BITS+ROW_BITS+COL_BITS){1'b1}}}; i=i+1) begin
                    mem_array[i] = 'bx;
                end
`else
                memory_index = 0;
                i = 0;
                // remove the selected banks
                for (memory_index=0; memory_index<mem_used; memory_index=memory_index+1) begin
                    if (addr_array[memory_index]>({bank_MSB_row, {(ROW_BITS+COL_BITS-2){1'b1}}})) begin
                        addr_array[i] = 'bx;
                        mem_array[i] = 'bx;
                        i = i + 1;
                    end else begin
                        i = i + 1;
                    end
                end
`endif
            end
        end
    endtask

    // Write Memory
    task write_mem;
        input [full_mem_bits - 1 : 0] addr;
        input       [DQ_BITS - 1 : 0] data;
        reg       [part_mem_bits : 0] i;
        begin
`ifdef FULL_MEM
            mem_array[addr] = data;
`else
            begin : loop
                for (i = 0; i < mem_used; i = i + 1) begin
                    if (addr_array[i] === addr) begin
                        disable loop;
                    end
                end
            end
            if (i === mem_used) begin
                if (i === (1<<part_mem_bits)) begin
                    $display ("%m: At time %t ERROR: Memory overflow.\n  Write to Address %d with Data %d will be lost.\n You must increase the part_mem_bits parameter or `define FULL_MEM.", $realtime, addr, data);
                end else begin
                    mem_used = mem_used + 1;
                    addr_array[i] = addr;
                end
            end
            mem_array[i] = data;
`endif
        end
    endtask

    // Read Memory
    task read_mem;
        input [full_mem_bits - 1 : 0] addr;
        output      [DQ_BITS - 1 : 0] data;
        reg       [part_mem_bits : 0] i;
        begin
`ifdef FULL_MEM
            data = mem_array[addr];
`else
            begin : loop
                for (i = 0; i < mem_used; i = i + 1) begin
                    if (addr_array[i] === addr) begin
                        disable loop;
                    end
                end
            end
            if (i <= mem_used) begin
                data = mem_array[i];
            end else begin
                data = 'bx;
            end
`endif
        end
    endtask

    // Burst Decode
    task Burst_Decode;
    begin

        // Advance Burst Counter
//        if (Burst_counter < burst_length) begin
        if (Write_pipeline[-2]) begin
            Burst_counter = Burst_counter + 1;
        end

        // Burst Type
        if (Mode_reg[3] === 1'b0) begin                         // Sequential Burst
            Cols_temp = Cols_addr + 1;
        end else if (Mode_reg[3] === 1'b1) begin                // Interleaved Burst
            Cols_temp[2] =  Burst_counter[2] ^ Cols_brst[2];
            Cols_temp[1] =  Burst_counter[1] ^ Cols_brst[1];
            Cols_temp[0] =  Burst_counter[0] ^ Cols_brst[0];
        end

        // Burst Length
        if (burst_length === 2) begin
            Cols_addr [0] = Cols_temp [0];
        end else if (burst_length === 4) begin
            Cols_addr [1 : 0] = Cols_temp [1 : 0];
        end else if (burst_length === 8) begin
            Cols_addr [2 : 0] = Cols_temp [2 : 0];
        end else if (burst_length === 16) begin
            Cols_addr [3 : 0] = Cols_temp [3 : 0];
        end else begin
            Cols_addr = Cols_temp;
        end

        // Data Counter
//        if (Burst_counter >= burst_length) begin
//            Data_in_enable = 1'b0;
//            Data_out_enable = 1'b0;
//            read_precharge_truncation = 1'b0;
//            SRR_read = 1'b0;
//        end
    end
    endtask


    // SRC check
    task Timing_chk_SRC;
    begin
        if (Active_enable   || Aref_enable     || Sref_enable  || Burst_term  ||
            Ext_mode_enable || Mode_reg_enable || Prech_enable || Read_enable ||
            Write_enable    || DPD_enable      || PD_enable    || srr_enable) begin
               if (part_size == 1024) begin
                   if (SRC_cntr < ((Mode_reg[6:4])+tSRC)) begin
                       $display ("%m: At time %t ERROR: tSRC Violation", $realtime);
                   end
               end
        end
    end
    endtask

    task ShiftPipelines;
    begin
        // read command pipeline
        Read_pipeline = Read_pipeline >> 1;
        Write_pipeline = Write_pipeline >> 1;
        for (i = -2; i < `MAX_PIPE-1; i = i + 1)
        begin
            Write_col_pipeline [i]  = Write_col_pipeline[i+1];
            Write_bank_pipeline [i] = Write_bank_pipeline[i+1];
        end
    end
    endtask

    // Dq and Dqs Drivers
    task Dq_Dqs_Drivers;
    begin

        // Initialize Read command
        if (Read_pipeline [0] === 1'b1) begin
//            Data_out_enable = 1'b1;
            Bank_addr = Write_bank_pipeline[0];
            Cols_addr = Write_col_pipeline [0];
            Cols_brst = Cols_addr [2 : 0];
//            if (SRR_read == 1'b1) begin
//                Burst_counter = burst_length - 2;
//            end else begin
//                Burst_counter = 0;
//            end

            // Row Address Mux
            case (Bank_addr)
                2'd0    : Rows_addr = B0_row_addr;
                2'd1    : Rows_addr = B1_row_addr;
                2'd2    : Rows_addr = B2_row_addr;
                2'd3    : Rows_addr = B3_row_addr;
                default : $display ("%m: At time %t ERROR: Invalid Bank Address", $realtime);
            endcase
        end

        // Read latch
        if (Read_pipeline[0] === 1'b1) begin
            // output data
            if (SRR_read == 1'b1) begin
                if (Cols_addr == 0) begin
                    Dq_out_temp = Srr_reg[DQ_BITS-1:0];
                end else begin
                    Dq_out_temp = Srr_reg[2*DQ_BITS-1:DQ_BITS];
                    SRR_read = 1'b0 ;
                end
            end else begin
                read_mem({Bank_addr, Rows_addr, Cols_addr}, Dq_out_temp);
            end
            if (Debug) begin
                $display ("At time %t %m:READ: Bank = %d, Row = %d, Col = %d, Data = %d", $realtime, Bank_addr, Rows_addr, Cols_addr, Dq_out);
            end

        end

        Dq_out <= # (tAC_max) Dq_out_temp ;
        Dqs_out <= #(tAC_max) ((|Read_pipeline[0]) & Sys_clk) ; // used in place of tLZ for a one clock preamble

        if (Sys_clk) begin
            Dqs_out_en <= #(tAC_max) (|Read_pipeline[3:0]);
            Dq_out_en <= #(tAC_max) (Read_pipeline[0]);
        end

    end
    endtask

    // Write FIFO and DM Mask Logic
    task Write_FIFO_DM_Mask_Logic;
    begin

        // Initialize Write command
        if (Write_pipeline [-2] === 1'b1) begin
//            Data_in_enable = 1'b1;
            Bank_addr = Write_bank_pipeline [-2];
            Cols_addr = Write_col_pipeline [-2];
            Cols_brst = Cols_addr [2 : 0];
//            Burst_counter = 0;

            // Row address mux
            case (Bank_addr)
                2'd0    : Rows_addr = B0_row_addr;
                2'd1    : Rows_addr = B1_row_addr;
                2'd2    : Rows_addr = B2_row_addr;
                2'd3    : Rows_addr = B3_row_addr;
                default : $display ("%m: At time %t ERROR: Invalid Row Address", $realtime);
            endcase
        end

        // Write data
//        if (Data_in_enable === 1'b1) begin
        if (Write_pipeline[-2] === 1'b1) begin

            // Data Buffer
            read_mem({Bank_addr, Rows_addr, Cols_addr}, Dq_buf);

            // write negedge Dqs on posedge Sys_clk
            if (Sys_clk) begin
                if (!dm_fall[0]) begin
                    Dq_buf [ 7 : 0] = dq_fall [ 7 : 0];
                end
                if (!dm_fall[1]) begin
                    Dq_buf [15 : 8] = dq_fall [15 : 8];
                end
`ifdef x32
                if (!dm_fall[2]) begin
                    Dq_buf [23 : 16] = dq_fall [23 : 16];
                end
                if (!dm_fall[3]) begin
                    Dq_buf [31 : 24] = dq_fall [31 : 24];
                end
`endif
                if (~&dm_fall) begin
                    if (Debug) begin
                        $display ("At time %t %m:WRITE: Bank = %d, Row = %d, Col = %d, Data = %h", $realtime, Bank_addr, Rows_addr, Cols_addr, Dq_buf[DQ_BITS-1:0]);
                    end
                end
            // write posedge Dqs on negedge Sys_clk
            end else begin
                if (!dm_rise[0]) begin
                    Dq_buf [ 7 : 0] = dq_rise [ 7 : 0];
                end
                if (!dm_rise[1]) begin
                    Dq_buf [15 : 8] = dq_rise [15 : 8];
                end
`ifdef x32
                if (!dm_rise[2]) begin
                    Dq_buf [23 : 16] = dq_rise [23 : 16];
                end
                if (!dm_rise[3]) begin
                    Dq_buf [31 : 24] = dq_rise [31 : 24];
                end
`endif
                if (~&dm_rise) begin
                    if (Debug) begin
                        $display ("At time %t %m:WRITE: Bank = %d, Row = %d, Col = %d, Data = %h", $realtime, Bank_addr, Rows_addr, Cols_addr, Dq_buf[DQ_BITS-1:0]);
                    end
                end
            end

            // Write Data
            write_mem({Bank_addr, Rows_addr, Cols_addr}, Dq_buf);
            // tWR start and tWTR check
            if (Sys_clk && &dm_pair === 1'b0)  begin
                case (Bank_addr)
                    2'd0    : WR_chk0 = $realtime;
                    2'd1    : WR_chk1 = $realtime;
                    2'd2    : WR_chk2 = $realtime;
                    2'd3    : WR_chk3 = $realtime;
                    default : $display ("%m: At time %t ERROR: Invalid Bank Address (tWR)", $realtime);
                endcase

//                // tWTR check
//                if (Read_enable === 1'b1) begin
//                    $display ("%m: At time %t ERROR: tWTR violation during Read", $realtime);
//                end
            end
        end
    end
    endtask

    // Auto Precharge Calculation
    task Auto_Precharge_Calculation;
    begin
        // Precharge counter
        if (Read_precharge_access [0] === 1'b1 || Write_precharge_access [0] === 1'b1) begin
            Count_precharge [0] = Count_precharge [0] + 1;
        end
        if (Read_precharge_access [1] === 1'b1 || Write_precharge_access [1] === 1'b1) begin
            Count_precharge [1] = Count_precharge [1] + 1;
        end
        if (Read_precharge_access [2] === 1'b1 || Write_precharge_access [2] === 1'b1) begin
            Count_precharge [2] = Count_precharge [2] + 1;
        end
        if (Read_precharge_access [3] === 1'b1 || Write_precharge_access [3] === 1'b1) begin
            Count_precharge [3] = Count_precharge [3] + 1;
        end

        // Read with AutoPrecharge Calculation
        //      The device start internal precharge when:
        //          1.  BL/2 cycles after command
        //          2.  Meet tRAS requirement
        if (Read_precharge_access[0] & (Count_precharge[0] >= burst_length/2)) begin
            Read_precharge_access[0] = 1'b0 ;
            Read_precharge_pre[0] = 1'b1 ;
        end
        if ((Read_precharge_pre[0] === 1'b1) && ($realtime - RAS_chk0 >= tRAS - 0.001)) begin
            Pc_b0 = 1'b1;
            Act_b0 = 1'b0;
            RP_chk0 = $realtime;
            Read_precharge_pre[0] = 1'b0;
        end

        if (Read_precharge_access[1] & (Count_precharge[1] >= burst_length/2)) begin
            Read_precharge_access[1] = 1'b0 ;
            Read_precharge_pre[1] = 1'b1 ;
        end
        if ((Read_precharge_pre[1] === 1'b1) && ($realtime - RAS_chk1 >= tRAS - 0.001)) begin
            Pc_b1 = 1'b1;
            Act_b1 = 1'b0;
            RP_chk1 = $realtime;
            Read_precharge_pre[1] = 1'b0;
        end

        if (Read_precharge_access[2] & (Count_precharge[2] >= burst_length/2)) begin
            Read_precharge_access[2] = 1'b0 ;
            Read_precharge_pre[2] = 1'b1 ;
        end
        if ((Read_precharge_pre[2] === 1'b1) && ($realtime - RAS_chk2 >= tRAS - 0.001)) begin
            Pc_b2 = 1'b1;
            Act_b2 = 1'b0;
            RP_chk2 = $realtime;
            Read_precharge_pre[2] = 1'b0;
        end

        if (Read_precharge_access[3] & (Count_precharge[3] >= burst_length/2)) begin
            Read_precharge_access[3] = 1'b0 ;
            Read_precharge_pre[3] = 1'b1 ;
        end
        if ((Read_precharge_pre[3] === 1'b1) && ($realtime - RAS_chk3 >= tRAS - 0.001)) begin
            Pc_b3 = 1'b1;
            Act_b3 = 1'b0;
            RP_chk3 = $realtime;
            Read_precharge_pre[3] = 1'b0;
        end

        // Write with AutoPrecharge Calculation
        //      The device start internal precharge when:
        //          1.  Meet tRAS requirement
        //          2.  Two clock after last burst
        // Since tWR is time base, the model will compensate tRP
        if (Write_precharge_access[0] & (Count_precharge[0] >= burst_length/2+3)) begin
            Write_precharge_access[0] = 1'b0 ;
            Write_precharge_pre[0] = 1'b1 ;
        end
        if (Write_precharge_pre[0] & ($realtime - RAS_chk0 >= tRAS - 0.001)) begin
                Write_precharge_pre[0] = 1'b0;
                Pc_b0 = 1'b1;
                Act_b0 = 1'b0;
                RP_chk0 = $realtime - ((2 * clk_period) - tWR);
        end

        if (Write_precharge_access[1] & (Count_precharge[1] >= burst_length/2+3)) begin
            Write_precharge_access[1] = 1'b0 ;
            Write_precharge_pre[1] = 1'b1 ;
        end
        if (Write_precharge_pre[1] & ($realtime - RAS_chk1 >= tRAS - 0.001)) begin
                Write_precharge_pre[1] = 1'b0;
                Pc_b1 = 1'b1;
                Act_b1 = 1'b0;
                RP_chk1 = $realtime - ((2 * clk_period) - tWR);
        end

        if (Write_precharge_access[2] & (Count_precharge[2] >= burst_length/2+3)) begin
            Write_precharge_access[2] = 1'b0 ;
            Write_precharge_pre[2] = 1'b1 ;
        end
        if (Write_precharge_pre[2] & ($realtime - RAS_chk2 >= tRAS - 0.001)) begin
                Write_precharge_pre[2] = 1'b0;
                Pc_b2 = 1'b1;
                Act_b2 = 1'b0;
                RP_chk2 = $realtime - ((2 * clk_period) - tWR);
        end

        if (Write_precharge_access[3] & (Count_precharge[3] >= burst_length/2+3)) begin
            Write_precharge_access[3] = 1'b0 ;
            Write_precharge_pre[3] = 1'b1 ;
        end
        if (Write_precharge_pre[3] & ($realtime - RAS_chk3 >= tRAS - 0.001)) begin
                Write_precharge_pre[3] = 1'b0;
                Pc_b3 = 1'b1;
                Act_b3 = 1'b0;
                RP_chk3 = $realtime - ((2 * clk_period) - tWR);
        end
    end
    endtask

    task Power_down_chk;
    begin
        if (DPD_enable == 1'b1 && enter_DPD == 1'b0) begin
            if (prev_cke & Pc_b0 & Pc_b1 & Pc_b2 & Pc_b3) begin
                erase_mem(4'b0000, 1'b1);
                current_init_state = begin_init ;
                ext_mode_load_done = 1'b0 ;
                mode_load_done = 1'b0 ;
                enter_DPD = 1'b1;
                $display ("%m: at time %t Entering Deep Power-Down Mode", $realtime);
            end
        end
        if (enter_DPD == 1'b1) begin
            if (Cke == 1'b1 && prev_cke == 1'b0) begin
                if (PD_cntr < tCKE) begin
                    $display ("%m: At time %t ERROR: tCKE violation during exiting of Deep Power-Down Mode", $realtime);
                end
                $display ("%m: at time %t Exiting Deep Power-Down Mode - A 200 us delay is required with either DESELECT or NOP commands present before the initialization sequence may begin", $realtime);
                enter_DPD = 1'b0;
            end
        end
        if (PD_enable == 1'b1 && enter_PD == 1'b0) begin
            if (prev_cke) begin
                if (Pc_b0 & Pc_b1 & Pc_b2 & Pc_b3) begin
                    $display ("%m: at time %t Entering Power-Down Mode", $realtime);
                    enter_PD = 1'b1;
                end else if (~Pc_b0 | ~Pc_b1 | ~Pc_b2 | ~Pc_b3) begin
                    $display ("%m: at time %t Entering Active Power-Down Mode", $realtime);
                    enter_APD = 1'b1;
                end
            end
        end
        if (enter_PD == 1'b1 || enter_APD == 1'b1) begin
            if (Cke == 1'b1 && prev_cke == 1'b0) begin
                if (PD_cntr < tCKE) begin
                    if (enter_PD == 1'b1) begin
                        $display ("%m: At time %t ERROR: tCKE violation during exiting of Power-Down Mode", $realtime);
                    end else if (enter_APD == 1'b1) begin
                        $display ("%m: At time %t ERROR: tCKE violation during exiting of Active Power-Down Mode", $realtime);
                    end
                end
                if (enter_PD == 1'b1) begin
                    $display ("%m: at time %t Exiting Power-Down Mode", $realtime);
                    enter_PD = 1'b0 ;
                end else if (enter_APD == 1'b1) begin
                    $display ("%m: at time %t Exiting Active Power-Down Mode", $realtime);
                    enter_APD = 1'b0 ;
                end
            end
        end

    end
    endtask

    reg [31:0] xx ;
    function [COL_BITS-1:0] Burst_Order;
        input [COL_BITS-1:0] Col;
        input [31:0]         i;
    begin
        if (Mode_reg[3] == 1'b1) //interleaved
            Burst_Order = (Col & -1*burst_length) + (Col%burst_length ^ i);
        else // sequential
        begin
            xx = -1*burst_length;
            Burst_Order = (Col & xx) + (Col%burst_length + i) % (burst_length);
        end
    end
    endfunction

    // Control Logic
    task Control_Logic;
    begin

      // Self Refresh
        if (Sref_enable === 1'b1) begin
            // Partial Array Self Refresh
            if (part_size == 128) begin
                case (Ext_Mode_reg[2:0])
                    3'b000 : ;//keep Bank 0-7
                    3'b001 : begin $display("%m: at time %t INFO: Banks 2-3 will be lost due to Partial Array Self Refresh", $realtime)                  ; erase_mem(4'b0111, 1'b0); end
                    3'b010 : begin $display("%m: at time %t INFO: Banks 1-3 will be lost due to Partial Array Self Refresh", $realtime)                  ; erase_mem(4'b0011, 1'b0); end
                    3'b011 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                    3'b100 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                    3'b101 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                    3'b110 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                endcase
            end else begin
                case (Ext_Mode_reg[2:0])
                    3'b000 : ;//keep Bank 0-7
                    3'b001 : begin $display("%m: at time %t INFO: Banks 2-3 will be lost due to Partial Array Self Refresh", $realtime)                  ; erase_mem(4'b0111, 1'b0); end
                    3'b010 : begin $display("%m: at time %t INFO: Banks 1-3 will be lost due to Partial Array Self Refresh", $realtime)                  ; erase_mem(4'b0011, 1'b0); end
                    3'b011 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                    3'b100 : begin $display("%m: at time %t INFO: Reserved", $realtime)                                                                                      ; end
                    3'b101 : begin $display("%m: at time %t INFO: Banks 1-3 and 1/2 of bank 0 will be lost due to Partial Array Self Refresh", $realtime); erase_mem(4'b0001, 1'b0); end
                    3'b110 : begin $display("%m: at time %t INFO: Banks 1-3 and 3/4 of bank 0 will be lost due to Partial Array Self Refresh", $realtime); erase_mem(4'b0000, 1'b0); end
                endcase
            end
            SelfRefresh = 1'b1;
        end
        if (Aref_enable === 1'b1) begin
            if (Debug) begin
                $display ("Debug: At time %t %m:AUTOREFRESH: Auto Refresh", $realtime);
            end
            // aref_count is to make sure we have met part of the initialization sequence
            if (~power_up_done) begin
                aref_count = aref_count + 1;
            end

            // Auto Refresh to Auto Refresh
            if ($realtime - RFC_chk < tRFC - 0.001) begin
                $display ("%m: At time %t ERROR: tRFC violation during Auto Refresh", $realtime);
            end
            
            // Precharge to Auto Refresh
            if (($realtime - RP_chk0 < tRP - 0.001) || ($realtime - RP_chk1 < tRP - 0.001) ||
                ($realtime - RP_chk2 < tRP - 0.001) || ($realtime - RP_chk3 < tRP - 0.001)) begin
                $display ("%m: At time %t ERROR: tRP violation during Auto Refresh", $realtime);
            end
            
            // Precharge to Auto Refresh
            if (Pc_b0 === 1'b0 || Pc_b1 === 1'b0 || Pc_b2 === 1'b0 || Pc_b3 === 1'b0) begin
                $display ("%m: At time %t ERROR: All banks must be Precharged before Auto Refresh", $realtime);
            end
            
            // Record Current tRFC time
            RFC_chk = $realtime;
        end

        // SRR Register
        if (srr_enable == 1'b1) begin
            if (Pc_b0 === 1'b1 && Pc_b1 === 1'b1 && Pc_b2 === 1'b1 && Pc_b3 === 1'b1 &&
                Data_out_enable === 1'b0 && Data_in_enable === 1'b0) begin
                SRR_read = 1'b1;
                SRR_chk = $realtime;
                SRR_cntr = 0;
            end
        end


        // Extended Mode Register
        if (Ext_mode_enable == 1'b1) begin
            if (Debug) begin
                $display ("Debug: At time %t %m:EMR  : Extended Mode Register", $realtime);
            end

            // Register Mode
            Ext_Mode_reg = Addr;

            if (Pc_b0 === 1'b1 && Pc_b1 === 1'b1 && Pc_b2 === 1'b1 && Pc_b3 === 1'b1) begin
                // ensure that power sequence is met properly
                if (~power_up_done) begin
                    ext_mode_load_done = 1'b1;
                end
                $display ("At time %t %m:ELMR  : Extended Load Mode Register", $realtime);
                if (part_size == 128) begin
                    // Self Refresh Coverage
                    case (Addr[2 : 0])
                        3'b000  : $display ("%m :                             Self Refresh Cov = 4 banks");
                        3'b001  : $display ("%m :                             Self Refresh Cov = 2 banks");
                        3'b010  : $display ("%m :                             Self Refresh Cov = 1 bank");
                        3'b101  : $display ("%m :                             PASR = Reserved");
                        3'b110  : $display ("%m :                             PASR = Reserved");
                        default : $display ("%m :                             PASR = Reserved");
                    endcase
                end else begin
                    // Self Refresh Coverage
                    case (Addr[2 : 0])
                        3'b000  : $display ("%m :                             Self Refresh Cov = 4 banks");
                        3'b001  : $display ("%m :                             Self Refresh Cov = 2 banks");
                        3'b010  : $display ("%m :                             Self Refresh Cov = 1 bank");
                        3'b101  : $display ("%m :                             Self Refresh Cov = 1/2 bank");
                        3'b110  : $display ("%m :                             Self Refresh Cov = 1/4 bank");
                        default : $display ("%m :                             PASR = Reserved");
                    endcase
                end
                // Maximum Case Temp
//                case (Addr[4 : 3])
//                    2'b11  : $display ("%m :                             Maximum Case Temp = 85C");
//                    2'b00  : $display ("%m :                             Maximum Case Temp = 70C");
//                    2'b01  : $display ("%m :                             Maximum Case Temp = 45C");
//                    2'b10  : $display ("%m :                             Maximum Case Temp = 15C");
//                endcase

                // Drive Strength
                case (Addr[7 : 5])  
                    3'b000  : $display ("%m :                             Drive Strength    = Full Strength");
                    3'b001  : $display ("%m :                             Drive Strength    = Half Strength");
                    3'b010  : $display ("%m :                             Drive Strength    = Quarter Strength");
                    3'b011  : $display ("%m :                             Drive Strength    = Three Quarter Strength");
                    3'b100  : $display ("%m :                             Drive Strength    = Three Quarter Strength");
                endcase

            end else begin
                $display ("%m: At time %t ERROR: all banks must be Precharged before Extended Mode Register", $realtime);
            end

            // Precharge to EMR
            if (($realtime - RP_chk0 < tRP - 0.001) || ($realtime - RP_chk1 < tRP - 0.001) ||
                ($realtime - RP_chk2 < tRP - 0.001) || ($realtime - RP_chk3 < tRP - 0.001)) begin
                $display ("%m: At time %t ERROR: tRP violation during Extended Mode Register", $realtime);
            end

            // LMR/EMR to LMR/EMR
//            if ($realtime - MRD_chk < tMRD) begin
//                $display ("%m: At time %t ERROR: tMRD violation during Extended Mode Register", $realtime);
//            end

            if (MRD_cntr < tMRD) begin
                $display ("%m: At time %t ERROR: tMRD violation during Extended Mode Register", $realtime);
            end

            // Record current tMRD time
//            MRD_chk = $realtime;
            MRD_cntr = 0;
        end
    
        // Load Mode Register
        if (Mode_reg_enable === 1'b1) begin
            if (Debug) begin
                $display ("Debug: At time %t %m:LMR  : Load Mode Register", $realtime);
            end

            // Register Mode
            Mode_reg = Addr;

            if (Mode_reg[6:4] == 3'b010) begin
                if (tCK2_min == 0) begin
                    $display ("%m : at time %t ERROR : Illegal CAS Latency of 2 set for current speed grade", $realtime);
                end
            end

            // Precharge to LMR
            if (Pc_b0 === 1'b0 || Pc_b1 === 1'b0 || Pc_b2 === 1'b0 || Pc_b3 === 1'b0) begin
                $display ("%m: At time %t ERROR: all banks must be Precharged before Load Mode Register", $realtime);
            end

            // Precharge to LMR
            if (($realtime - RP_chk0 < tRP - 0.001) || ($realtime - RP_chk1 < tRP - 0.001) ||
                ($realtime - RP_chk2 < tRP - 0.001) || ($realtime - RP_chk3 < tRP - 0.001)) begin
                $display ("%m: At time %t ERROR: tRP violation during Load Mode Register", $realtime);
            end

            // LMR/EMR to LMR/EMR
//            if ($realtime - MRD_chk < tMRD) begin
//                $display ("%m: At time %t ERROR: tMRD violation during Load Mode Register", $realtime);
//            end
            if (MRD_cntr < tMRD) begin
                $display ("%m: At time %t ERROR: tMRD violation during Load Mode Register", $realtime);
            end

            if (Pc_b0 === 1'b1 && Pc_b1 === 1'b1 && Pc_b2 === 1'b1 && Pc_b3 === 1'b1) begin
                // ensure that power sequence is met properly
                if (~power_up_done) begin
                    mode_load_done = 1'b1;
                end
               // Burst Length
               case (Addr [2 : 0])
                   3'b001  : $display ("At time %t %m:LMR  : Burst Length = 2", $realtime); 
                   3'b010  : $display ("At time %t %m:LMR  : Burst Length = 4", $realtime);
                   3'b011  : $display ("At time %t %m:LMR  : Burst Length = 8", $realtime);
                   3'b100  : $display ("At time %t %m:LMR  : Burst Length = 16",$realtime);
                   default : 
                     begin
                       $display ("%m: At time %t ERROR: Undefined burst length selection", $realtime);
                       $stop;
                     end
               endcase
   
               // CAS Latency
               case (Addr [6 : 4])
                   3'b010  : $display ("At time %t %m:LMR  : CAS Latency = 2", $realtime);
                   3'b011  : $display ("At time %t %m:LMR  : CAS Latency = 3", $realtime);
                   default : begin
                       $display ("%m: At time %t ERROR: CAS Latency not supported", $realtime);
                       $stop;
                   end
               endcase
   
            end
            // Record current tMRD time
//            MRD_chk = $realtime;
            MRD_cntr = 0;
        end

        // Activate Block
        if (Active_enable === 1'b1) begin
            if (!(power_up_done)) begin
                $display ("%m: At time %t ERROR: Power Up and Initialization Sequence not completed before executing Activate command", $realtime);
            end
            // Display Debug Message
            if (Debug) begin
                $display ("Debug: At time %t %m:ACTIVATE: Bank = %d, Row = %d", $realtime, Ba, Addr);
            end

            // Activating an open bank can cause corruption.
            if ((Ba === 2'b00 && Pc_b0  === 1'b0) || (Ba === 2'b01 && Pc_b1  === 1'b0) ||
                (Ba === 2'b10 && Pc_b2  === 1'b0) || (Ba === 2'b11 && Pc_b3  === 1'b0)) begin
                $display ("%m: At time %t ERROR: Bank = %d is already activated - data can be corrupted", $realtime, Ba);
            end
            
            // Activate Bank 0
            if (Ba === 2'b00 && Pc_b0 === 1'b1) begin
                // Activate to Activate (same bank)
                if ($realtime - RC_chk0 < tRC - 0.001) begin
                    $display ("%m: At time %t ERROR: tRC violation during Activate bank %d", $realtime, Ba);
                end

                // Precharge to Activate
                if ($realtime - RP_chk0 < tRP - 0.001) begin
                    $display ("%m: At time %t ERROR: tRP violation during Activate bank %d", $realtime, Ba);
                end

                // Record variables for checking violation
                Act_b0 = 1'b1;
                Pc_b0 = 1'b0;
                B0_row_addr = Addr;
                RC_chk0  = $realtime;
                RCD_chk0 = $realtime;
                RAS_chk0 = $realtime;
                RAP_chk0 = $realtime;
            end

            // Activate Bank 1
            if (Ba === 2'b01 && Pc_b1 === 1'b1) begin
                // Activate to Activate (same bank)
                if ($realtime - RC_chk1 < tRC - 0.001) begin
                    $display ("%m: At time %t ERROR: tRC violation during Activate bank %d", $realtime, Ba);
                end

                // Precharge to Activate
                if ($realtime - RP_chk1 < tRP - 0.001) begin
                    $display ("%m: At time %t ERROR: tRP violation during Activate bank %d", $realtime, Ba);
                end

                // Record variables for checking violation
                Act_b1 = 1'b1;
                Pc_b1 = 1'b0;
                B1_row_addr = Addr;
                RC_chk1  = $realtime;
                RCD_chk1 = $realtime;
                RAS_chk1 = $realtime;
                RAP_chk1 = $realtime;
            end

            // Activate Bank 2
            if (Ba === 2'b10 && Pc_b2 === 1'b1) begin
                // Activate to Activate (same bank)
                if ($realtime - RC_chk2 < tRC - 0.001) begin
                    $display ("%m: At time %t ERROR: tRC violation during Activate bank %d", $realtime, Ba);
                end

                // Precharge to Activate
                if ($realtime - RP_chk2 < tRP - 0.001) begin
                    $display ("%m: At time %t ERROR: tRP violation during Activate bank %d", $realtime, Ba);
                end

                // Record variables for checking violation
                Act_b2 = 1'b1;
                Pc_b2 = 1'b0;
                B2_row_addr = Addr;
                RC_chk2  = $realtime;
                RCD_chk2 = $realtime;
                RAS_chk2 = $realtime;
                RAP_chk2 = $realtime;
            end

            // Activate Bank 3
            if (Ba === 2'b11 && Pc_b3 === 1'b1) begin
                // Activate to Activate (same bank)
                if ($realtime - RC_chk3 < tRC - 0.001) begin
                    $display ("%m:  t time %t ERROR: tRC violation during Activate bank %d", $realtime, Ba);
                end

                // Precharge to Activate
                if ($realtime - RP_chk3 < tRP - 0.001) begin
                    $display ("%m: At time %t ERROR: tRP violation during Activate bank %d", $realtime, Ba);
                end

                // Record variables for checking violation
                Act_b3 = 1'b1;
                Pc_b3 = 1'b0;
                B3_row_addr = Addr;
                RC_chk3  = $realtime;
                RCD_chk3 = $realtime;
                RAS_chk3 = $realtime;
                RAP_chk3 = $realtime;
            end

            // Activate to Activate (different bank)
            if ((Prev_bank != Ba) && ($realtime - RRD_chk < tRRD - 0.001)) begin
                $display ("%m: At time %t ERROR: tRRD violation during Activate bank = %d", $realtime, Ba);
            end
            
            // AutoRefresh to Activate
            if ($realtime - RFC_chk < tRFC - 0.001) begin
                $display ("%m: At time %t ERROR: tRFC violation during Activate bank %d", $realtime, Ba);
            end

            // Record variable for checking violation
            RRD_chk = $realtime;
            Prev_bank = Ba;
        end
    
        // Precharge Block - consider NOP if bank already precharged or in process of precharging
        if (Prech_enable === 1'b1) begin
            // Display Debug Message
            if (Debug) begin
                $display ("Debug: At time %t %m:PRE: Addr[10] = %b, Bank = %b", $realtime, Addr[10], Ba);
            end

            // EMR or LMR to Precharge
//            if ($realtime - MRD_chk < tMRD) begin
//                $display ("%m: At time %t ERROR: tMRD violation during Precharge", $realtime);
//            end
            if (MRD_cntr < tMRD) begin
                $display ("%m: At time %t ERROR: tMRD violation during Precharge", $realtime);
            end

            // Precharge bank 0
            if ((Addr[10] === 1'b1 || (Addr[10] === 1'b0 && Ba === 2'b00)) && Act_b0 === 1'b1) begin
                Act_b0 = 1'b0;
                Pc_b0 = 1'b1;
                RP_chk0 = $realtime;
                
                // Activate to Precharge Bank
                if ($realtime - RAS_chk0 < tRAS - 0.001) begin
                    $display ("%m: At time %t ERROR: tRAS violation during Precharge", $realtime);
                end
                
                // tWR violation check for Write
                if ($realtime - WR_chk0 < tWR - 0.001) begin
                    $display ("%m: At time %t ERROR: tWR violation during Precharge", $realtime);
                end
            end

            // Precharge bank 1
            if ((Addr[10] === 1'b1 || (Addr[10] === 1'b0 && Ba === 2'b01)) && Act_b1 === 1'b1) begin
                Act_b1 = 1'b0;
                Pc_b1 = 1'b1;
                RP_chk1 = $realtime;

                // Activate to Precharge Bank 1
                if ($realtime - RAS_chk1 < tRAS - 0.001) begin
                    $display ("%m: At time %t ERROR: tRAS violation during Precharge", $realtime);
                end
                
                // tWR violation check for Write
                if ($realtime - WR_chk1 < tWR - 0.001) begin
                    $display ("%m: At time %t ERROR: tWR violation during Precharge", $realtime);
                end
            end

            // Precharge bank 2
            if ((Addr[10] === 1'b1 || (Addr[10] === 1'b0 && Ba === 2'b10)) && Act_b2 === 1'b1) begin
                Act_b2 = 1'b0;
                Pc_b2 = 1'b1;
                RP_chk2 = $realtime;
                
                // Activate to Precharge Bank 2
                if ($realtime - RAS_chk2 < tRAS - 0.001) begin
                    $display ("%m: At time %t ERROR: tRAS violation during Precharge", $realtime);
                end
                
                // tWR violation check for Write
                if ($realtime - WR_chk2 < tWR - 0.001) begin
                    $display ("%m: At time %t ERROR: tWR violation during Precharge", $realtime);
                end
            end

            // Precharge bank 3
            if ((Addr[10] === 1'b1 || (Addr[10] === 1'b0 && Ba === 2'b11)) && Act_b3 === 1'b1) begin
                Act_b3 = 1'b0;
                Pc_b3 = 1'b1;
                RP_chk3 = $realtime;
                
                // Activate to Precharge Bank 3
                if ($realtime - RAS_chk3 < tRAS - 0.001) begin
                    $display ("%m: At time %t ERROR: tRAS violation during Precharge", $realtime);
                end
                
                // tWR violation check for Write
                if ($realtime - WR_chk3 < tWR - 0.001) begin
                    $display ("%m: At time %t ERROR: tWR violation during Precharge", $realtime);
                end
            end

            // Pipeline for READ
            if ((Addr[10] === 1'b1) | (Ba == Write_bank_pipeline[4]))
                for (i = 4; i < `MAX_PIPE; i = i + 1)
                    Read_pipeline[i] = 1'b0 ;
    
//            A10_precharge [cas_latency_x2] = Addr[10];
//            Bank_precharge[cas_latency_x2] = Ba;
//            Cmnd_precharge[cas_latency_x2] = 1'b1;
        end
    
        // Burst terminate
        if (Burst_term === 1'b1) begin
            // Display Debug Message
            if (Debug) begin
                $display ("Debug: %m: At time %t BURST_TERMINATE): Burst Terminate",$realtime);
            end

            // Burst Terminate Command Pipeline for Read
//            Cmnd_bst[cas_latency_x2-1] = 1'b1;
            for (i = 4; i < `MAX_PIPE; i = i + 1)
                Read_pipeline[i] = 1'b0 ;
            // Illegal to burst terminate a Write
            if ((Data_in_enable === 1'b1) & ~((&dm_rise) & (&dm_fall))) begin
                $display ("%m: At time %t ERROR: It's illegal to burst terminate a Write", $realtime);
            end
            
            // Illegal to burst terminate a Read with Auto Precharge
            if (|Read_precharge_access) begin
                $display ("%m: At time %t ERROR: It's illegal to burst terminate a Read with Auto Precharge", $realtime);
            end
        end
        
        // Read Command
        if (Read_enable === 1'b1) begin
            if (!(power_up_done)) begin
                $display ("%m: At time %t ERROR: Power Up and Initialization Sequence not completed before executing Read Command", $realtime);
            end
            // Display Debug Message
            if (Debug) begin
                $display ("Debug: At time %t %m:READ: Bank = %d, Col = %d", $realtime, Ba, {Addr [11], Addr [9 : 0]});
            end
            if (part_size == 1024) begin
                if (SRR_read == 1'b1) begin
                    if (SRR_cntr < tSRR) begin
                        $display ("%m: At time %t ERROR: tSRR Violation", $realtime);
                    end
                    SRC_cntr = 0 ;
                end
            end else begin
                if (SRR_read == 1'b1) begin
                    if ($realtime - SRR_chk < tSRR-0.01) begin
                        $display ("%m: At time %t ERROR: tSRR Violation", $realtime);
                    end
                    SRC_cntr = 0;
                end
            end
            // CAS Latency pipeline
            if (SRR_read) begin
                if ({Ba, Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]} > 0) begin
                   $display ("%m: At time %t ERROR: Address must be all 0 during SRR Read", $realtime);
                end
                for (i=0; i<2; i=i+1)          
                begin
                    Read_pipeline[cas_latency_x2 - 2 + i + 1] = 1'b1;
                    Write_col_pipeline [cas_latency_x2 - 2 + i + 1] = i;
                    Write_bank_pipeline [cas_latency_x2 - 2 + i + 1] = 0;
                end
            end else begin
                for (i=0; i<burst_length; i=i+1)          
                begin
                    Read_pipeline[cas_latency_x2 - 2 + i + 1] = 1'b1;
                    Write_bank_pipeline [cas_latency_x2 - 2 + i + 1] = Ba;
//                    if (Mode_reg) // interleaved
//                        Write_col_pipeline [cas_latency_x2 - 2 + i + 1] = {Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]} + i;
                    Write_col_pipeline [cas_latency_x2 - 2 + i + 1] = Burst_Order({Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]}, i);
//                    else          // sequential
//                        Write_col_pipeline [cas_latency_x2 - 2 + i + 1] = {Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]} + (i % burst_length);
                end
            end
            // Interrupt a Read with Auto Precharge
            if (Read_precharge_access [Ba] === 1'b1) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a Read with Auto Precharge (same bank)", $realtime);
            end else if ((Read_precharge_access [0] === 1'b1) |
                         (Read_precharge_access [1] === 1'b1) |
                         (Read_precharge_access [2] === 1'b1) |
                         (Read_precharge_access [3] === 1'b1) ) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a Read with Auto Precharge (different banks)", $realtime);
            end

            // Interrupt a Write with Auto Precharge
            if (Write_precharge_access [Ba] === 1'b1) begin
//                $display ("%m: At time %t ERROR: It's illegal to interrupt a Write with Auto Precharge (same banks)", $realtime);
            end else if ((Write_precharge_count [0] < (burst_length/2  )) & (Mode_reg[6:4] == 3'b010) |
                         (Write_precharge_count [1] < (burst_length/2  )) & (Mode_reg[6:4] == 3'b010) |
                         (Write_precharge_count [2] < (burst_length/2  )) & (Mode_reg[6:4] == 3'b010) |
                         (Write_precharge_count [3] < (burst_length/2  )) & (Mode_reg[6:4] == 3'b010) |
                         (Write_precharge_count [0] < (burst_length/2-1)) & (Mode_reg[6:4] == 3'b011) |
                         (Write_precharge_count [1] < (burst_length/2-1)) & (Mode_reg[6:4] == 3'b011) |
                         (Write_precharge_count [2] < (burst_length/2-1)) & (Mode_reg[6:4] == 3'b011) |
                         (Write_precharge_count [3] < (burst_length/2-1)) & (Mode_reg[6:4] == 3'b011) ) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a data transfer on a Write with Auto Precharge (different banks)", $realtime);
            end

            // Activate to Read
            if (((Ba === 2'b00 && Pc_b0 === 1'b1) || (Ba === 2'b01 && Pc_b1 === 1'b1) ||
                 (Ba === 2'b10 && Pc_b2 === 1'b1) || (Ba === 2'b11 && Pc_b3 === 1'b1)) &&
                (SRR_read == 1'b0)) begin
                $display("%m: At time %t ERROR: Bank is not Activated for Read", $realtime);
            end

            // Activate to Read without Auto Precharge
            if ((Addr [10] === 1'b0 && Ba === 2'b00 && $realtime - RCD_chk0 < tRCD - 0.001) ||
                (Addr [10] === 1'b0 && Ba === 2'b01 && $realtime - RCD_chk1 < tRCD - 0.001) ||
                (Addr [10] === 1'b0 && Ba === 2'b10 && $realtime - RCD_chk2 < tRCD - 0.001) ||
                (Addr [10] === 1'b0 && Ba === 2'b11 && $realtime - RCD_chk3 < tRCD - 0.001)) begin
                $display("%m: At time %t ERROR: tRCD violation during Read", $realtime);
            end

            // Auto Precharge
            if (Addr[10] === 1'b1) begin
                Read_precharge_access [Ba]= 1'b1;
                Count_precharge [Ba]= 0;
                Read_precharge_count[Ba] = 4'h0;
            end
            
            // tWTR
            if (tWTR_cntr < tWTR - 0.001) begin
                $display("%m: At time %t ERROR: tWTR violation during Read", $realtime);
            end
        end

        // Write Command
        if (Write_enable === 1'b1) begin
            if (!(power_up_done)) begin
                $display ("%m: At time %t ERROR: Power Up and Initialization Sequence not completed before executing Write Command", $realtime);
            end
            // display debug message
            if (Debug) begin
                $display ("Debug: At time %t %m:WRITE: Bank = %d, Col = %d", $realtime, Ba, {Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]});
            end

            // Pipeline for Write
//            Write_pipeline [3] = 1'b1;
            for (i=0; i<burst_length; i=i+1)          
            begin
                Write_pipeline[1 + i] = 1'b1;
//                Write_col_pipeline [1 + i] = {Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]} + i;
                Write_col_pipeline [1 + i] = Burst_Order({Addr [ADDR_BITS - 1 : 11], Addr [9 : 0]}, i);
                Write_bank_pipeline [1 + i] = Ba;

            end
//            Write_pipeline [3] = 1'b1;
//            Write_bank_pipeline [3] = Ba;
            

            // Interrupt a Write with Auto Precharge (same bank only)
            if (Write_precharge_access [Ba] === 1'b1) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a Write with Auto Precharge", $realtime);
            end

            // Activate to Write
            if ((Ba === 2'b00 && Pc_b0 === 1'b1) || (Ba === 2'b01 && Pc_b1 === 1'b1) ||
                (Ba === 2'b10 && Pc_b2 === 1'b1) || (Ba === 2'b11 && Pc_b3 === 1'b1)) begin
                $display("%m: At time %t ERROR: Bank is not Activated for Write", $realtime);
            end

            // Activate to Write
            if ((Ba === 2'b00 && $realtime - RCD_chk0 < tRCD - 0.001) ||
                (Ba === 2'b01 && $realtime - RCD_chk1 < tRCD - 0.001) ||
                (Ba === 2'b10 && $realtime - RCD_chk2 < tRCD - 0.001) ||
                (Ba === 2'b11 && $realtime - RCD_chk3 < tRCD - 0.001)) begin
                $display("%m: At time %t ERROR: tRCD violation during Write to Bank %d", $realtime, Ba);
            end

            // Read to Write
//            if (Read_pipeline[0] || Read_pipeline[1] || Read_pipeline[2] || Read_pipeline[3] || 
//                Read_pipeline[4] || Read_pipeline[5] || Read_pipeline[6] || (Burst_counter_dly < burst_length)) begin
//                if (Data_out_enable_dly || read_precharge_truncation) begin
//                    $display("%m: At time %t ERROR: Read to Write violation", $realtime);
//                end
//            end
            if (Read_pipeline[0]) begin
                $display("%m: At time %t ERROR: Read to Write violation", $realtime);
            end

            // Interrupt a Write with Auto Precharge
            if (Write_precharge_access [Ba] === 1'b1) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a Write with Auto Precharge (same bank)", $realtime);
            end else if (((Write_precharge_access [0] === 1'b1) & (Count_precharge[0] < (burst_length/2))) |
                         ((Write_precharge_access [1] === 1'b1) & (Count_precharge[1] < (burst_length/2))) |
                         ((Write_precharge_access [2] === 1'b1) & (Count_precharge[2] < (burst_length/2))) |
                         ((Write_precharge_access [3] === 1'b1) & (Count_precharge[3] < (burst_length/2))) ) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a Write with Auto Precharge (different bank)", $realtime);
            end

            // Interrupt a Read with Auto Precharge
            if (((Read_precharge_count [Ba] < (4'h2 + (burst_length/2))) & (Mode_reg[6:4] == 3'b010)) | 
                ((Read_precharge_count [Ba] < (4'h3 + (burst_length/2))) & (Mode_reg[6:4] == 3'b011)) ) begin
//                $display ("%m: At time %t ERROR: It's illegal to interrupt a Read with Auto Precharge (same bank)", $realtime);
            end else if (((Read_precharge_count [0] < (4'h2 + (burst_length/2))) & (Mode_reg[6:4] == 3'b010)) |
                         ((Read_precharge_count [1] < (4'h2 + (burst_length/2))) & (Mode_reg[6:4] == 3'b010)) |
                         ((Read_precharge_count [2] < (4'h2 + (burst_length/2))) & (Mode_reg[6:4] == 3'b010)) |
                         ((Read_precharge_count [3] < (4'h2 + (burst_length/2))) & (Mode_reg[6:4] == 3'b010)) |
                         ((Read_precharge_count [0] < (4'h3 + (burst_length/2))) & (Mode_reg[6:4] == 3'b011)) |
                         ((Read_precharge_count [1] < (4'h3 + (burst_length/2))) & (Mode_reg[6:4] == 3'b011)) |
                         ((Read_precharge_count [2] < (4'h3 + (burst_length/2))) & (Mode_reg[6:4] == 3'b011)) |
                         ((Read_precharge_count [3] < (4'h3 + (burst_length/2))) & (Mode_reg[6:4] == 3'b011)) ) begin
                $display ("%m: At time %t ERROR: It's illegal to interrupt a data transfer on a Read with Auto Precharge (different bank)", $realtime);
            end
            
            // Auto Precharge
            if (Addr[10] === 1'b1) begin
                Write_precharge_access [Ba]= 1'b1;
                Count_precharge [Ba]= 0;
                Write_precharge_count[Ba] = 4'h0;
            end
        end

    end
    endtask

    // Main Logic
    always @ (posedge Sys_clk) begin
        ShiftPipelines;
//        Manual_Precharge_Pipeline;
//        Burst_Terminate_Pipeline;
        Dq_Dqs_Drivers;
        Write_FIFO_DM_Mask_Logic;
        Burst_Decode;
        Auto_Precharge_Calculation;
        Timing_chk_SRC;
        Control_Logic;
        MRD_counter;
        SRR_counter;
        SRC_counter;
        tWTR_counter;
        command_counter;
    end

    always @ (negedge Sys_clk) begin
        ShiftPipelines;
//        Manual_Precharge_Pipeline;
//        Burst_Terminate_Pipeline;
        Dq_Dqs_Drivers;
        Write_FIFO_DM_Mask_Logic;
        Burst_Decode;
    end

    // Dqs Receiver
    always @ (posedge Dqs_in[0]) begin
        // Latch data at posedge Dqs
        dq_rise[7 : 0] = Dq_in[7 : 0];
        dm_rise[0] = Dm_in[0];
        expect_pos_dqs[0] = 0;
    end

    always @ (posedge Dqs_in[1]) begin
        // Latch data at posedge Dqs
        dq_rise[15 : 8] = Dq_in[15 : 8];
        dm_rise[1] = Dm_in[1];
        expect_pos_dqs[1] = 0;
    end

`ifdef x32
    always @ (posedge Dqs_in[2]) begin
        // Latch data at posedge Dqs
        dq_rise[23 : 16] = Dq_in[23 : 16];
        dm_rise[2] = Dm_in[2];
        expect_pos_dqs[2] = 0;
    end

    always @ (posedge Dqs_in[3]) begin
        // Latch data at posedge Dqs
        dq_rise[31 : 24] = Dq_in[31 : 24];
        dm_rise[3] = Dm_in[3];
        expect_pos_dqs[3] = 0;
    end
`endif

    always @ (negedge Dqs_in[0]) begin
        // Latch data at negedge Dqs
        dq_fall[7 : 0] = Dq_in[7 : 0];
        dm_fall[0] = Dm_in[0];
        dm_pair[1:0]  = {dm_rise[0], dm_fall[0]};
        expect_neg_dqs[0] = 0;
    end

    always @ (negedge Dqs_in[1]) begin
        // Latch data at negedge Dqs
        dq_fall[15: 8] = Dq_in[15 : 8];
        dm_fall[1] = Dm_in[1];
        dm_pair[3:2]  = {dm_rise[1], dm_fall[1]};
        expect_neg_dqs[1] = 0;
    end

`ifdef x32
    always @ (negedge Dqs_in[2]) begin
        // Latch data at negedge Dqs
        dq_fall[23: 16] = Dq_in[23 : 16];
        dm_fall[2] = Dm_in[2];
        dm_pair[5:4]  = {dm_rise[2], dm_fall[2]};
        expect_neg_dqs[2] = 0;
    end

    always @ (negedge Dqs_in[3]) begin
        // Latch data at negedge Dqs
        dq_fall[31: 24] = Dq_in[31 : 24];
        dm_fall[3] = Dm_in[3];
        dm_pair[7:6]  = {dm_rise[3], dm_fall[3]};
        expect_neg_dqs[3] = 0;
    end
`endif

    // Dqs edge checking
    always @ (posedge Sys_clk) begin
//        if (Write_pipeline[2] || Write_pipeline[1] || Data_in_enable) begin
        if (Write_pipeline[-1]) begin
            for (i=0; i<DQS_BITS; i=i+1) begin
                if (expect_neg_dqs[i]) begin
                    $display ("%m: At time %t ERROR: Negative DQS[%1d] transition required.", $realtime, i);
                end
                expect_neg_dqs[i] = 1'b1;
            end
        end else begin
           expect_neg_dqs = 0;
           expect_pos_dqs = 0;
        end
    end

    always @ (negedge Sys_clk) begin
//        if (Write_pipeline[2] || Write_pipeline[1] || Data_in_enable) begin
        if (Write_pipeline[-1]) begin
            for (i=0; i<DQS_BITS; i=i+1) begin
                if (expect_pos_dqs[i]) begin
                    $display ("%m: At time %t ERROR: Positive DQS[%1d] transition required.", $realtime, i);
                end
                expect_pos_dqs[i] = 1'b1;
            end
        end else begin
           expect_neg_dqs = 0;
           expect_pos_dqs = 0;
        end
    end

    specify
                                                   // SYMBOL UNITS DESCRIPTION
                                                   // ------ ----- -----------
`ifdef sg5                                         //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     5.0 ;     // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.0 ;     // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.0 ;     // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.0 ;     // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.0 ;     // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     1.75;     // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     1.75;     // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg54                                  //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     5.4;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.08;     // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.08;     // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.0;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.0;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     2.16;     // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     2.16;     // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg6                                   //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     6.0;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.2;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.2;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.1;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.1;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     2.1;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     2.1;      // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg75                                  //              specparams for -75  (CL = 3)
        specparam tCLK_MIN         =     7.5;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.5;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.5;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.3;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.3;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     3.0;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     3.0;      // tDQSL  ns    DQS input Low Pulse Width
`else`ifdef sg5v18                                 //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     5.0 ;     // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.0 ;     // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.0 ;     // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     0.9 ;     // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     0.9 ;     // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     2.0 ;     // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     2.0 ;     // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg6v18                                //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     6.0;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.2;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.2;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.1;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.1;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     2.4;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     2.4;      // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg75v18                               //              specparams for -75  (CL = 3)
        specparam tCLK_MIN         =     7.5;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.5;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.5;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.3;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.3;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     3.0;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     3.0;      // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg6v12                                //              specparams for -6 (CL = 3)
        specparam tCLK_MIN         =     6.0;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.2;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.2;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.1;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.1;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     2.1;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     2.1;      // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg75v12                               //              specparams for -75  (CL = 3)
        specparam tCLK_MIN         =     7.5;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.5;      // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.5;      // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.3;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.3;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     3.0;      // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     3.0;      // tDQSL  ns    DQS input Low Pulse Width
`else `ifdef sg10v12                               //              specparams for -10 (CL = 3)
        specparam tCLK_MIN         =     9.6;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.92;     // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.92;     // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.7;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.7;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     3.84;     // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     3.84;     // tDQSL  ns    DQS input Low Pulse Width
`else `define sg10                                 //              specparams for -10 (CL = 3)
        specparam tCLK_MIN         =     9.6;      // tCLK   ns    minimum clk cycle time
        specparam tDSS             =     1.92;     // tDSS   ns    DQS falling edge to CLK rising (setup time)
        specparam tDSH             =     1.92;     // tDSH   ns    DQS falling edge from CLK rising (hold time)
        specparam tIH              =     1.5;      // tIH    ns    Input Hold Time (fast)
        specparam tIS              =     1.5;      // tIS    ns    Input Setup Time (fast)
        specparam tDQSH            =     3.84;     // tDQSH  ns    DQS input High Pulse Width
        specparam tDQSL            =     3.84;     // tDQSL  ns    DQS input Low Pulse Width
`endif `endif `endif `endif `endif `endif `endif `endif `endif `endif
        $period   (posedge Clk, tCLK_MIN);
        $width    (posedge Dqs_in[0] &&& wdqs_valid, tDQSH);
        $width    (posedge Dqs_in[1] &&& wdqs_valid, tDQSH);
        $width    (negedge Dqs_in[0] &&& wdqs_valid, tDQSL);
        $width    (negedge Dqs_in[1] &&& wdqs_valid, tDQSL);
        $setuphold(posedge Clk,   Cke,   tIS, tIH);
        $setuphold(posedge Clk,   Cs_n,  tIS, tIH);
        $setuphold(posedge Clk,   Cas_n, tIS, tIH);
        $setuphold(posedge Clk,   Ras_n, tIS, tIH);
        $setuphold(posedge Clk,   We_n,  tIS, tIH);
        $setuphold(posedge Clk,   Addr,  tIS, tIH);
        $setuphold(posedge Clk,   Ba,    tIS, tIH);
        $setuphold(posedge Clk, negedge Dqs &&& wdqs_valid, tDSS , tDSH);
    endspecify

endmodule
