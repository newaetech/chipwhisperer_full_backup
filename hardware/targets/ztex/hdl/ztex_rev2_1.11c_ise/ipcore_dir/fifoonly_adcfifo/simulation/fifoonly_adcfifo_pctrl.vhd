
--------------------------------------------------------------------------------
--
-- FIFO Generator Core Demo Testbench 
--
--------------------------------------------------------------------------------
--
-- (c) Copyright 2009 - 2010 Xilinx, Inc. All rights reserved.
-- 
-- This file contains confidential and proprietary information
-- of Xilinx, Inc. and is protected under U.S. and
-- international copyright and other intellectual property
-- laws.
-- 
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- Xilinx, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) Xilinx shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or Xilinx had been advised of the
-- possibility of the same.
-- 
-- CRITICAL APPLICATIONS
-- Xilinx products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of Xilinx products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
-- 
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES.
--------------------------------------------------------------------------------
--
-- Filename: fifoonly_adcfifo_pctrl.vhd
--
-- Description:
--   Used for protocol control on write and read interface stimulus and status generation
--
--------------------------------------------------------------------------------
-- Library Declarations
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE IEEE.std_logic_arith.all;
USE IEEE.std_logic_misc.all;

LIBRARY work;
USE work.fifoonly_adcfifo_pkg.ALL;

ENTITY fifoonly_adcfifo_pctrl IS
  GENERIC(
   AXI_CHANNEL         : STRING  :="NONE";
   C_APPLICATION_TYPE  : INTEGER := 0;
   C_DIN_WIDTH         : INTEGER := 0;
   C_DOUT_WIDTH        : INTEGER := 0;
   C_WR_PNTR_WIDTH     : INTEGER := 0;
   C_RD_PNTR_WIDTH     : INTEGER := 0;
   C_CH_TYPE           : INTEGER := 0;
   FREEZEON_ERROR      : INTEGER := 0;
   TB_STOP_CNT         : INTEGER := 2;
   TB_SEED             : INTEGER := 2
  );
  PORT(
       RESET_WR        : IN STD_LOGIC;
       RESET_RD        : IN STD_LOGIC;
       WR_CLK          : IN STD_LOGIC;
       RD_CLK          : IN STD_LOGIC;
       FULL            : IN STD_LOGIC;
       EMPTY           : IN STD_LOGIC;
       ALMOST_FULL     : IN STD_LOGIC;
       ALMOST_EMPTY    : IN STD_LOGIC;
       DATA_IN         : IN STD_LOGIC_VECTOR(C_DIN_WIDTH-1 DOWNTO 0);
       DATA_OUT        : IN STD_LOGIC_VECTOR(C_DOUT_WIDTH-1 DOWNTO 0);
       DOUT_CHK        : IN STD_LOGIC;
       PRC_WR_EN       : OUT STD_LOGIC;
       PRC_RD_EN       : OUT STD_LOGIC;
       RESET_EN        : OUT STD_LOGIC;
       SIM_DONE        : OUT STD_LOGIC;
       STATUS          : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
      );
END ENTITY;


ARCHITECTURE fg_pc_arch OF fifoonly_adcfifo_pctrl IS
 
 CONSTANT C_DATA_WIDTH   : INTEGER := if_then_else(C_DIN_WIDTH > C_DOUT_WIDTH,C_DIN_WIDTH,C_DOUT_WIDTH);
 CONSTANT LOOP_COUNT     : INTEGER := divroundup(C_DATA_WIDTH,8);
 CONSTANT D_WIDTH_DIFF   :   INTEGER := log2roundup(C_DIN_WIDTH/C_DOUT_WIDTH);

 SIGNAL data_chk_i       : STD_LOGIC := if_then_else(C_CH_TYPE /= 2,'1','0');
 SIGNAL full_chk_i       : STD_LOGIC := if_then_else(C_CH_TYPE /= 2,'1','0');
 SIGNAL empty_chk_i      : STD_LOGIC := if_then_else(C_CH_TYPE /= 2,'1','0');
 SIGNAL status_i         : STD_LOGIC_VECTOR(4 DOWNTO 0):= (OTHERS => '0');
 SIGNAL status_d1_i      : STD_LOGIC_VECTOR(4 DOWNTO 0):= (OTHERS => '0');
 SIGNAL wr_en_gen        : STD_LOGIC_VECTOR(7 DOWNTO 0):= (OTHERS => '0');
 SIGNAL rd_en_gen        : STD_LOGIC_VECTOR(7 DOWNTO 0):= (OTHERS => '0');
 SIGNAL wr_cntr          : STD_LOGIC_VECTOR(C_WR_PNTR_WIDTH-2 DOWNTO 0)   := (OTHERS => '0');
 SIGNAL full_as_timeout  : STD_LOGIC_VECTOR(C_WR_PNTR_WIDTH DOWNTO 0)   := (OTHERS => '0');
 SIGNAL full_ds_timeout  : STD_LOGIC_VECTOR(C_WR_PNTR_WIDTH DOWNTO 0) := (OTHERS => '0');
 SIGNAL rd_cntr          : STD_LOGIC_VECTOR(C_RD_PNTR_WIDTH-2 DOWNTO 0)   := (OTHERS => '0'); 
 SIGNAL empty_as_timeout : STD_LOGIC_VECTOR(C_RD_PNTR_WIDTH DOWNTO 0)  := (OTHERS => '0'); 
 SIGNAL empty_ds_timeout : STD_LOGIC_VECTOR(C_RD_PNTR_WIDTH DOWNTO 0):= (OTHERS => '0'); 
 SIGNAL wr_en_i          : STD_LOGIC := '0';
 SIGNAL rd_en_i          : STD_LOGIC := '0';
 SIGNAL state            : STD_LOGIC := '0';
 SIGNAL wr_control       : STD_LOGIC := '0';
 SIGNAL rd_control       : STD_LOGIC := '0';
 SIGNAL stop_on_err      : STD_LOGIC := '0';
 SIGNAL sim_stop_cntr    : STD_LOGIC_VECTOR(7 DOWNTO 0):= conv_std_logic_vector(if_then_else(C_CH_TYPE=2,64,TB_STOP_CNT),8);
 SIGNAL sim_done_i       : STD_LOGIC := '0';
 SIGNAL rdw_gt_wrw       : STD_LOGIC_VECTOR(D_WIDTH_DIFF-1 DOWNTO 0) := (OTHERS => '1');
 SIGNAL wrw_gt_rdw       : STD_LOGIC_VECTOR(D_WIDTH_DIFF-1 DOWNTO 0) := (OTHERS => '1');
 SIGNAL rd_activ_cont    : STD_LOGIC_VECTOR(25 downto 0):= (OTHERS => '0');
 SIGNAL prc_we_i         : STD_LOGIC := '0';
 SIGNAL prc_re_i         : STD_LOGIC := '0';
 SIGNAL reset_en_i       : STD_LOGIC := '0';
 SIGNAL sim_done_d1      : STD_LOGIC := '0';
 SIGNAL sim_done_wr1     : STD_LOGIC := '0';
 SIGNAL sim_done_wr2     : STD_LOGIC := '0'; 
 SIGNAL empty_d1         : STD_LOGIC := '0';
 SIGNAL empty_wr_dom1    : STD_LOGIC := '0';
 SIGNAL state_d1         : STD_LOGIC := '0';
 SIGNAL state_rd_dom1    : STD_LOGIC := '0';
 SIGNAL rd_en_d1         : STD_LOGIC := '0';
 SIGNAL rd_en_wr1        : STD_LOGIC := '0';
 SIGNAL wr_en_d1         : STD_LOGIC := '0';
 SIGNAL wr_en_rd1        : STD_LOGIC := '0';
 SIGNAL full_chk_d1      : STD_LOGIC := '0';
 SIGNAL full_chk_rd1     : STD_LOGIC := '0';
 SIGNAL empty_wr_dom2    : STD_LOGIC := '0';

 SIGNAL state_rd_dom2    : STD_LOGIC := '0';
 SIGNAL state_rd_dom3    : STD_LOGIC := '0';
 SIGNAL rd_en_wr2        : STD_LOGIC := '0';
 SIGNAL wr_en_rd2        : STD_LOGIC := '0';
 SIGNAL full_chk_rd2     : STD_LOGIC := '0';
 SIGNAL reset_en_d1      : STD_LOGIC := '0';
 SIGNAL reset_en_rd1     : STD_LOGIC := '0';
 SIGNAL reset_en_rd2     : STD_LOGIC := '0';

 SIGNAL data_chk_wr_d1   : STD_LOGIC := '0';
 SIGNAL data_chk_rd1     : STD_LOGIC := '0';
 SIGNAL data_chk_rd2     : STD_LOGIC := '0';
 SIGNAL post_rst_dly_wr  : STD_LOGIC_VECTOR(4 DOWNTO 0) := (OTHERS => '1');
 SIGNAL post_rst_dly_rd  : STD_LOGIC_VECTOR(4 DOWNTO 0) := (OTHERS => '1');
BEGIN
 status_i  <= data_chk_i & full_chk_rd2 & empty_chk_i & '0' & '0';
 STATUS    <= status_d1_i & '0' & '0' & rd_activ_cont(rd_activ_cont'high);

 prc_we_i <= wr_en_i  WHEN sim_done_wr2 = '0' ELSE '0';
 prc_re_i <= rd_en_i  WHEN sim_done_i   = '0' ELSE '0';

 SIM_DONE   <= sim_done_i;
 rdw_gt_wrw <= (OTHERS => '1');

 PROCESS(RD_CLK)
 BEGIN
   IF (RD_CLK'event AND RD_CLK='1') THEN
     IF(prc_re_i = '1') THEN
       rd_activ_cont <= rd_activ_cont + "1";
     END IF;
   END IF;
 END PROCESS;


 PROCESS(sim_done_i)
 BEGIN
    assert sim_done_i = '0'
    report "Simulation Complete for:" & AXI_CHANNEL
    severity note;
 END PROCESS;

-----------------------------------------------------
-- SIM_DONE SIGNAL GENERATION
-----------------------------------------------------
PROCESS (RD_CLK,RESET_RD)
BEGIN
    IF(RESET_RD = '1') THEN
      --sim_done_i <= '0';
    ELSIF(RD_CLK'event AND RD_CLK='1') THEN
      IF((OR_REDUCE(sim_stop_cntr) = '0' AND TB_STOP_CNT /= 0) OR stop_on_err = '1') THEN
     	sim_done_i <= '1';
      END IF;
    END IF;
END PROCESS;

 -- TB Timeout/Stop
 fifo_tb_stop_run:IF(TB_STOP_CNT /= 0) GENERATE
   PROCESS (RD_CLK)
   BEGIN
       IF (RD_CLK'event AND RD_CLK='1') THEN
         IF(state_rd_dom2 = '0' AND state_rd_dom3 = '1') THEN
           sim_stop_cntr <= sim_stop_cntr - "1";
         END IF;
       END IF;
   END PROCESS;
 END GENERATE fifo_tb_stop_run;


  -- Stop when error found
  PROCESS (RD_CLK)
  BEGIN
    IF (RD_CLK'event AND RD_CLK='1') THEN
      IF(sim_done_i = '0') THEN
        status_d1_i <= status_i OR status_d1_i;
      END IF;
      IF(FREEZEON_ERROR = 1 AND status_i /= "0") THEN
        stop_on_err <= '1';
      END IF;
    END IF;
  END PROCESS;
  -----------------------------------------------------

  -----------------------------------------------------
  -- CHECKS FOR FIFO
  -----------------------------------------------------


  PROCESS(RD_CLK,RESET_RD)
  BEGIN
    IF(RESET_RD = '1') THEN
      post_rst_dly_rd <= (OTHERS => '1');
    ELSIF (RD_CLK'event AND RD_CLK='1') THEN
      post_rst_dly_rd <= post_rst_dly_rd-post_rst_dly_rd(4);
    END IF;
  END PROCESS;
  
  PROCESS(WR_CLK,RESET_WR)
  BEGIN
    IF(RESET_WR = '1') THEN
      post_rst_dly_wr <= (OTHERS => '1');
    ELSIF (WR_CLK'event AND WR_CLK='1') THEN
      post_rst_dly_wr <= post_rst_dly_wr-post_rst_dly_wr(4);
    END IF;
  END PROCESS;


   PROCESS(WR_CLK,RESET_WR) 
   BEGIN
     IF(RESET_WR = '1') THEN
      wrw_gt_rdw <= (OTHERS => '1');
     ELSIF (WR_CLK'event AND WR_CLK='1') THEN
        IF(rd_en_wr2 = '1' AND wr_en_i = '0' AND FULL = '1') THEN
         wrw_gt_rdw <= wrw_gt_rdw + '1';
       END IF;
     END IF;
   END PROCESS;

  -- FULL de-assert Counter
  PROCESS(WR_CLK,RESET_WR) 
  BEGIN
    IF(RESET_WR = '1') THEN
      full_ds_timeout <= (OTHERS => '0');
    ELSIF(WR_CLK'event AND WR_CLK='1') THEN
      IF(state = '1') THEN
        IF(rd_en_wr2 = '1' AND wr_en_i = '0' AND FULL = '1' AND AND_REDUCE(wrw_gt_rdw) = '1') THEN
          full_ds_timeout <= full_ds_timeout + '1';
        END IF;
      ELSE
  	full_ds_timeout <= (OTHERS => '0');
      END IF;
    END IF;
  END PROCESS;
   
 
 -- EMPTY deassert counter
  PROCESS(RD_CLK,RESET_RD) 
  BEGIN
    IF(RESET_RD = '1') THEN
      empty_ds_timeout <= (OTHERS => '0');
    ELSIF(RD_CLK'event AND RD_CLK='1') THEN
      IF(state = '0') THEN
        IF(wr_en_rd2 = '1' AND rd_en_i = '0' AND EMPTY = '1' AND AND_REDUCE(rdw_gt_wrw) = '1') THEN
          empty_ds_timeout <= empty_ds_timeout + '1';
        END IF;
      ELSE
  	empty_ds_timeout <= (OTHERS => '0');
      END IF;
    END IF;
  END PROCESS;

  -- Full check signal generation
  PROCESS(WR_CLK,RESET_WR) 
  BEGIN
    IF(RESET_WR = '1') THEN
      full_chk_i <= '0';
    ELSIF(WR_CLK'event AND WR_CLK='1') THEN
      IF(C_APPLICATION_TYPE = 1 AND (AXI_CHANNEL = "WACH" OR AXI_CHANNEL = "RACH" OR AXI_CHANNEL = "AXI4_Stream")) THEN
        full_chk_i <= '0';
      ELSE
        full_chk_i <= AND_REDUCE(full_as_timeout) OR 
		      AND_REDUCE(full_ds_timeout);
      END IF;
    END IF;
  END PROCESS;

  -- Empty checks
  PROCESS(RD_CLK,RESET_RD) 
  BEGIN
    IF(RESET_RD = '1') THEN
      empty_chk_i <= '0';
    ELSIF(RD_CLK'event AND RD_CLK='1') THEN
      IF(C_APPLICATION_TYPE = 1 AND (AXI_CHANNEL = "WACH" OR AXI_CHANNEL = "RACH" OR AXI_CHANNEL = "AXI4_Stream")) THEN
        empty_chk_i <= '0';
      ELSE
        empty_chk_i <= AND_REDUCE(empty_as_timeout) OR 
		       AND_REDUCE(empty_ds_timeout);
      END IF;
    END IF;
  END PROCESS;

  fifo_d_chk:IF(C_CH_TYPE /= 2) GENERATE
    PRC_WR_EN  <= prc_we_i  AFTER 100 ns;
    PRC_RD_EN  <= prc_re_i  AFTER 50 ns;
    data_chk_i <= dout_chk;
  END GENERATE fifo_d_chk;
  -----------------------------------------------------
 

 -----------------------------------------------------
 -- SYNCHRONIZERS B/W WRITE AND READ DOMAINS
 -----------------------------------------------------
   PROCESS(WR_CLK,RESET_WR)
   BEGIN
     IF(RESET_WR = '1') THEN
       empty_wr_dom1  <= '1';
       empty_wr_dom2  <= '1';
       state_d1       <= '0';
       wr_en_d1       <= '0';
       rd_en_wr1      <= '0';
       rd_en_wr2      <= '0';
       full_chk_d1    <= '0';
       reset_en_d1    <= '0';
       sim_done_wr1   <= '0';
       sim_done_wr2   <= '0';
     ELSIF (WR_CLK'event AND WR_CLK='1') THEN
       sim_done_wr1   <= sim_done_d1;
       sim_done_wr2   <= sim_done_wr1;
       reset_en_d1    <= reset_en_i;
       state_d1       <= state;
       empty_wr_dom1  <= empty_d1;
       empty_wr_dom2  <= empty_wr_dom1;
       wr_en_d1       <= wr_en_i;
       rd_en_wr1      <= rd_en_d1;
       rd_en_wr2      <= rd_en_wr1;
       full_chk_d1    <= full_chk_i;
     END IF;
   END PROCESS;

   PROCESS(RD_CLK,RESET_RD)
   BEGIN
     IF(RESET_RD = '1') THEN
         empty_d1       <= '1';
         state_rd_dom1  <= '0';
         state_rd_dom2  <= '0';
         state_rd_dom3  <= '0';
         wr_en_rd1      <= '0';
         wr_en_rd2      <= '0';
         rd_en_d1       <= '0';
         full_chk_rd1   <= '0';
         full_chk_rd2   <= '0';
         reset_en_rd1   <= '0';
         reset_en_rd2   <= '0';
         sim_done_d1    <= '0';
     ELSIF (RD_CLK'event AND RD_CLK='1') THEN
         sim_done_d1    <= sim_done_i;
         reset_en_rd1   <= reset_en_d1;
         reset_en_rd2   <= reset_en_rd1;
         empty_d1       <= EMPTY;
         rd_en_d1       <= rd_en_i;
         state_rd_dom1  <= state_d1;
         state_rd_dom2  <= state_rd_dom1;
         state_rd_dom3  <= state_rd_dom2;
         wr_en_rd1      <= wr_en_d1;
         wr_en_rd2      <= wr_en_rd1;
         full_chk_rd1   <= full_chk_d1;
         full_chk_rd2   <= full_chk_rd1;
     END IF;
   END PROCESS;
   
   RESET_EN   <= reset_en_rd2;
 

   data_fifo_en:IF(C_CH_TYPE /= 2) GENERATE  
    -----------------------------------------------------
    -- WR_EN GENERATION
    -----------------------------------------------------
    gen_rand_wr_en:fifoonly_adcfifo_rng
    GENERIC MAP(
    	         WIDTH => 8,
                 SEED  => TB_SEED+1
               )
    PORT MAP(
              CLK        => WR_CLK,
              RESET      => RESET_WR,
              RANDOM_NUM => wr_en_gen,
              ENABLE     => '1'
            );

    PROCESS(WR_CLK,RESET_WR)
    BEGIN
      IF(RESET_WR = '1') THEN
        wr_en_i   <=  '0';
      ELSIF(WR_CLK'event AND WR_CLK='1') THEN
        IF(state = '1') THEN
          wr_en_i <= wr_en_gen(0) AND wr_en_gen(7) AND wr_en_gen(2) AND wr_control;
        ELSE
          wr_en_i <= (wr_en_gen(3) OR wr_en_gen(4) OR wr_en_gen(2)) AND (NOT post_rst_dly_wr(4)); 
        END IF;
      END IF;
    END PROCESS;
    
    -----------------------------------------------------
    -- WR_EN CONTROL
    -----------------------------------------------------
    PROCESS(WR_CLK,RESET_WR)
    BEGIN
      IF(RESET_WR = '1') THEN
        wr_cntr         <= (OTHERS => '0');
	wr_control      <= '1';
	full_as_timeout <= (OTHERS => '0');
      ELSIF(WR_CLK'event AND WR_CLK='1') THEN
	IF(state = '1') THEN
	  IF(wr_en_i = '1') THEN
	    wr_cntr <= wr_cntr + "1";
	  END IF;
	  full_as_timeout <= (OTHERS => '0');
	ELSE
	  wr_cntr <= (OTHERS => '0');
	  IF(rd_en_wr2 = '0') THEN
	    IF(wr_en_i = '1') THEN
  	      full_as_timeout <= full_as_timeout + "1";
  	    END IF;
	  ELSE 
	    full_as_timeout <= (OTHERS => '0');
	  END IF;
	END IF;

	wr_control <= NOT wr_cntr(wr_cntr'high);

      END IF;
    END PROCESS;

    -----------------------------------------------------
    -- RD_EN GENERATION
    -----------------------------------------------------
    gen_rand_rd_en:fifoonly_adcfifo_rng
    GENERIC MAP(
    	         WIDTH => 8,
                 SEED  => TB_SEED
               )
    PORT MAP(
              CLK        => RD_CLK,
              RESET      => RESET_RD,
              RANDOM_NUM => rd_en_gen,
              ENABLE     => '1'
            );

    PROCESS(RD_CLK,RESET_RD)
    BEGIN
      IF(RESET_RD = '1') THEN
	rd_en_i    <= '0';
      ELSIF(RD_CLK'event AND RD_CLK='1') THEN
        IF(state_rd_dom2 = '0') THEN
  	    rd_en_i <= rd_en_gen(1) AND rd_en_gen(5) AND rd_en_gen(3) AND rd_control AND (NOT post_rst_dly_rd(4));
        ELSE
          rd_en_i <= rd_en_gen(0) OR rd_en_gen(6);
        END IF;
      END IF;
    END PROCESS;

    -----------------------------------------------------
    -- RD_EN CONTROL
    -----------------------------------------------------
    PROCESS(RD_CLK,RESET_RD)
    BEGIN
      IF(RESET_RD = '1') THEN
        rd_cntr    <= (OTHERS => '0');
	rd_control <= '1';
	empty_as_timeout <= (OTHERS => '0');
      ELSIF(RD_CLK'event AND RD_CLK='1') THEN
	IF(state_rd_dom2 = '0') THEN
	  IF(rd_en_i = '1') THEN
	    rd_cntr <= rd_cntr + "1";
	  END IF;
	  empty_as_timeout <= (OTHERS => '0');
	ELSE
	  rd_cntr <= (OTHERS => '0');
	  IF(wr_en_rd2 = '0') THEN
	    IF(rd_en_i = '1') THEN
  	      empty_as_timeout <= empty_as_timeout + "1";
  	    END IF;
	  ELSE 
	    empty_as_timeout <= (OTHERS => '0');
	  END IF;
	END IF;
	
	rd_control <= NOT rd_cntr(rd_cntr'high);

      END IF;
    END PROCESS;

    -----------------------------------------------------
    -- STIMULUS CONTROL
    -----------------------------------------------------
    PROCESS(WR_CLK,RESET_WR)
    BEGIN
      IF(RESET_WR = '1') THEN
        state      <= '0';
	reset_en_i <= '0';
      ELSIF(WR_CLK'event AND WR_CLK='1') THEN
        CASE state IS
          WHEN '0' => 
            IF(FULL = '1' AND empty_wr_dom2 = '0') THEN
              state      <= '1';
	      reset_en_i <= '0';
    	    END IF;
          WHEN '1' => 
            IF(empty_wr_dom2 = '1' AND FULL = '0') THEN
              state    	 <= '0';
	      reset_en_i <= '1';
    	    END IF;
          WHEN OTHERS => state <= state;
	END CASE;
      END IF;
    END PROCESS;
  END GENERATE data_fifo_en;

END ARCHITECTURE;
