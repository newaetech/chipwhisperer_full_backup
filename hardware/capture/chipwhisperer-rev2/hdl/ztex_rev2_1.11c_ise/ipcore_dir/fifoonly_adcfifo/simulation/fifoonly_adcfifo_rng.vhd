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
-- Filename: fifoonly_adcfifo_rng.vhd
--
-- Description:
--   Used for generation of pseudo random numbers
--
--------------------------------------------------------------------------------
-- Library Declarations
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE IEEE.std_logic_arith.all;
USE IEEE.std_logic_misc.all;

ENTITY fifoonly_adcfifo_rng IS
    GENERIC (
      WIDTH        : integer :=  8;
      SEED         : integer :=  3);
    PORT (
      CLK         : IN STD_LOGIC;
      RESET       : IN STD_LOGIC;
      ENABLE      : IN STD_LOGIC;
      RANDOM_NUM  : OUT STD_LOGIC_VECTOR (WIDTH-1 DOWNTO 0));
END ENTITY;

ARCHITECTURE rg_arch OF fifoonly_adcfifo_rng IS
BEGIN
PROCESS (CLK,RESET)
  VARIABLE rand_temp : STD_LOGIC_VECTOR(width-1 DOWNTO 0):=conv_std_logic_vector(SEED,width);
  VARIABLE temp      : STD_LOGIC := '0';
BEGIN
  IF(RESET = '1') THEN
    rand_temp := conv_std_logic_vector(SEED,width);
    temp      := '0';
  ELSIF (CLK'event AND CLK = '1') THEN
    IF (ENABLE = '1') THEN
      temp := rand_temp(width-1) xnor rand_temp(width-3) xnor rand_temp(width-4) xnor rand_temp(width-5);
      rand_temp(width-1 DOWNTO 1) := rand_temp(width-2 DOWNTO 0);
      rand_temp(0) := temp;
    END IF;
  END IF;

  RANDOM_NUM <= rand_temp;

END PROCESS;

END ARCHITECTURE;
