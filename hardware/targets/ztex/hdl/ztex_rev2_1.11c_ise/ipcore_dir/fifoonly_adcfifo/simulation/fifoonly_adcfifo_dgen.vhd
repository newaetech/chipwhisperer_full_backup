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
-- Filename: fifoonly_adcfifo_dgen.vhd
--
-- Description:
--   Used for write interface stimulus generation
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

ENTITY fifoonly_adcfifo_dgen IS
  GENERIC (
	    C_DIN_WIDTH   : INTEGER := 32;
	    C_DOUT_WIDTH  : INTEGER := 32;
	    C_CH_TYPE     : INTEGER := 0;
	    TB_SEED       : INTEGER := 2
	 );
  PORT (
        RESET     : IN STD_LOGIC;
        WR_CLK    : IN STD_LOGIC;
        PRC_WR_EN : IN STD_LOGIC;
        FULL      : IN STD_LOGIC;  
        WR_EN     : OUT STD_LOGIC;      
        WR_DATA   : OUT STD_LOGIC_VECTOR(C_DIN_WIDTH-1 DOWNTO 0)
       );
END ENTITY;


ARCHITECTURE fg_dg_arch OF fifoonly_adcfifo_dgen IS
 
  CONSTANT C_DATA_WIDTH : INTEGER := if_then_else(C_DIN_WIDTH > C_DOUT_WIDTH,C_DIN_WIDTH,C_DOUT_WIDTH);
  CONSTANT LOOP_COUNT   : INTEGER := divroundup(C_DATA_WIDTH,8);
  
  SIGNAL pr_w_en        : STD_LOGIC := '0';
  SIGNAL rand_num       : STD_LOGIC_VECTOR(8*LOOP_COUNT-1 DOWNTO 0);
  SIGNAL wr_data_i      : STD_LOGIC_VECTOR(C_DIN_WIDTH-1 DOWNTO 0);
 BEGIN
  
   WR_EN   <= PRC_WR_EN ;
   WR_DATA <= wr_data_i AFTER 100 ns;

  ----------------------------------------------
  -- Generation of DATA
  ----------------------------------------------
  gen_stim:FOR N IN LOOP_COUNT-1 DOWNTO 0 GENERATE
    rd_gen_inst1:fifoonly_adcfifo_rng
    GENERIC MAP(
    	       WIDTH => 8,
               SEED  => TB_SEED+N
               )
    PORT MAP(
              CLK        => WR_CLK,
	      RESET      => RESET,
              RANDOM_NUM => rand_num(8*(N+1)-1 downto 8*N),
              ENABLE     => pr_w_en
            );
  END GENERATE; 

     pr_w_en <= PRC_WR_EN AND NOT FULL;
     wr_data_i <= rand_num(C_DIN_WIDTH-1 DOWNTO 0);


END ARCHITECTURE;
