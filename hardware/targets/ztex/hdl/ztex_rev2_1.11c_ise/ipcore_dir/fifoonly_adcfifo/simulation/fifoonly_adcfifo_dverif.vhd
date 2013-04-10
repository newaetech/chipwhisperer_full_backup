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
-- Filename: fifoonly_adcfifo_dverif.vhd
--
-- Description:
--   Used for FIFO read interface stimulus generation and data checking
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

ENTITY fifoonly_adcfifo_dverif IS
  GENERIC(
   C_DIN_WIDTH        : INTEGER := 0;
   C_DOUT_WIDTH       : INTEGER := 0;
   C_USE_EMBEDDED_REG : INTEGER := 0;
   C_CH_TYPE          : INTEGER := 0;
   TB_SEED            : INTEGER := 2
  );
  PORT(
       RESET          : IN STD_LOGIC;
       RD_CLK         : IN STD_LOGIC;
       PRC_RD_EN      : IN STD_LOGIC;
       EMPTY          : IN STD_LOGIC;
       DATA_OUT       : IN STD_LOGIC_VECTOR(C_DOUT_WIDTH-1 DOWNTO 0);
       RD_EN          : OUT STD_LOGIC;
       DOUT_CHK       : OUT STD_LOGIC
      );
END ENTITY;


ARCHITECTURE fg_dv_arch OF fifoonly_adcfifo_dverif IS
 
 CONSTANT C_DATA_WIDTH    : INTEGER := if_then_else(C_DIN_WIDTH > C_DOUT_WIDTH,C_DIN_WIDTH,C_DOUT_WIDTH);
 CONSTANT EXTRA_WIDTH     : INTEGER := if_then_else(C_CH_TYPE = 2,1,0);
 CONSTANT LOOP_COUNT      : INTEGER := divroundup(C_DATA_WIDTH+EXTRA_WIDTH,8);
  CONSTANT D_WIDTH_DIFF   : INTEGER := log2roundup(C_DIN_WIDTH/C_DOUT_WIDTH);

 SIGNAL expected_dout     : STD_LOGIC_VECTOR(C_DOUT_WIDTH-1 DOWNTO 0) := (OTHERS => '0');
 SIGNAL data_chk          : STD_LOGIC := '1';
 SIGNAL rand_num          : STD_LOGIC_VECTOR(8*LOOP_COUNT-1 downto 0);
 SIGNAL rd_en_i           : STD_LOGIC := '0';
 SIGNAL pr_r_en           : STD_LOGIC := '0';
 SIGNAL rd_en_d1          : STD_LOGIC := '1';
 SIGNAL rd_d_sel_d1       : STD_LOGIC_VECTOR(D_WIDTH_DIFF-1 DOWNTO 0):= (OTHERS => '0');
BEGIN

 
  DOUT_CHK <= data_chk;
  RD_EN    <= rd_en_i;
  rd_en_i  <= PRC_RD_EN;
  rd_en_d1 <= '1';
  
 
  data_fifo_chk:IF(C_CH_TYPE /=2) GENERATE 
  -------------------------------------------------------
  -- Expected data generation and checking for data_fifo
  -------------------------------------------------------
   
     PROCESS (RD_CLK,RESET)
     BEGIN
       IF (RESET = '1') THEN
         rd_d_sel_d1 <= (OTHERS => '0');
       ELSIF (RD_CLK'event AND RD_CLK='1') THEN
         IF (rd_en_i = '1' AND EMPTY = '0' AND rd_en_d1 = '1') THEN
           rd_d_sel_d1 <= rd_d_sel_d1+"1";
         END IF;
       END IF;
     END PROCESS;

     pr_r_en       <= (AND_REDUCE(rd_d_sel_d1)) AND rd_en_i AND NOT EMPTY;
     expected_dout <= rand_num(C_DIN_WIDTH-C_DOUT_WIDTH*conv_integer(rd_d_sel_d1)-1 DOWNTO C_DIN_WIDTH-C_DOUT_WIDTH*(conv_integer(rd_d_sel_d1)+1));

    gen_num:FOR N IN LOOP_COUNT-1 DOWNTO 0 GENERATE
      rd_gen_inst2:fifoonly_adcfifo_rng
      GENERIC MAP(
      	        WIDTH => 8,
                SEED  => TB_SEED+N
                 )
      PORT MAP(
                CLK        => RD_CLK,
	        RESET      => RESET,
                RANDOM_NUM => rand_num(8*(N+1)-1 downto 8*N),
                ENABLE     => pr_r_en 	    
              );
    END GENERATE;    
    
      PROCESS (RD_CLK,RESET)
      BEGIN
        IF(RESET = '1') THEN
          data_chk <= '0';
        ELSIF (RD_CLK'event AND RD_CLK='1') THEN
          IF(EMPTY = '0') THEN
            IF(DATA_OUT = expected_dout) THEN
              data_chk <= '0';
            ELSE
              data_chk <= '1';
            END IF;
          END IF;
        END IF;
      END PROCESS;
  END GENERATE data_fifo_chk;

END ARCHITECTURE;
