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
-- Filename: fifoonly_adcfifo_pkg.vhd
--
-- Description:
--   This is the demo testbench package file for FIFO Generator core.
--
--------------------------------------------------------------------------------
-- Library Declarations
--------------------------------------------------------------------------------
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE ieee.std_logic_arith.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;

PACKAGE fifoonly_adcfifo_pkg IS

 FUNCTION divroundup (
    data_value : INTEGER;
    divisor : INTEGER)
 RETURN INTEGER;
 ------------------------
 FUNCTION if_then_else (
   condition : BOOLEAN;
   true_case : INTEGER;
   false_case : INTEGER)
 RETURN INTEGER;
 ------------------------
 FUNCTION if_then_else (
   condition : BOOLEAN;
   true_case : STD_LOGIC;
   false_case : STD_LOGIC)
 RETURN STD_LOGIC;
  ------------------------
 FUNCTION if_then_else (
   condition : BOOLEAN;
   true_case : TIME;
   false_case : TIME)
 RETURN TIME;
 ------------------------ 
 FUNCTION log2roundup (
      data_value : INTEGER)
 RETURN INTEGER; 
 ------------------------ 
 FUNCTION hexstr_to_std_logic_vec(
   arg1 : string;
   size : integer )
 RETURN std_logic_vector;
 ------------------------
 COMPONENT fifoonly_adcfifo_rng IS
    GENERIC (WIDTH : integer :=  8;
             SEED  : integer := 3);
    PORT (
      CLK : IN STD_LOGIC;
      RESET : IN STD_LOGIC;
      ENABLE : IN STD_LOGIC;
      RANDOM_NUM : OUT STD_LOGIC_VECTOR (WIDTH-1 DOWNTO 0)         
     );
 END COMPONENT; 
 ------------------------ 
 
 COMPONENT fifoonly_adcfifo_dgen IS
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
 END COMPONENT;
 ------------------------ 

 COMPONENT fifoonly_adcfifo_dverif IS
  GENERIC(
   C_DIN_WIDTH        : INTEGER := 0;
   C_DOUT_WIDTH       : INTEGER := 0;
   C_USE_EMBEDDED_REG : INTEGER := 0;
   C_CH_TYPE          : INTEGER := 0;
   TB_SEED            : INTEGER := 2
  );
  PORT(
       RESET       : IN STD_LOGIC;
       RD_CLK      : IN STD_LOGIC;
       PRC_RD_EN   : IN STD_LOGIC;
       EMPTY       : IN STD_LOGIC;
       DATA_OUT    : IN STD_LOGIC_VECTOR(C_DOUT_WIDTH-1 DOWNTO 0);
       RD_EN       : OUT STD_LOGIC;
       DOUT_CHK    : OUT STD_LOGIC
      );
 END COMPONENT; 
 ------------------------ 

 COMPONENT fifoonly_adcfifo_pctrl IS
   GENERIC(
     AXI_CHANNEL         : STRING  := "NONE";
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
 END COMPONENT;
 ------------------------
 COMPONENT fifoonly_adcfifo_synth IS
  GENERIC(
  	   FREEZEON_ERROR : INTEGER := 0;
	   TB_STOP_CNT    : INTEGER := 0;
	   TB_SEED        : INTEGER := 1
	 );
  PORT(
	WR_CLK     :  IN  STD_LOGIC;
	RD_CLK     :  IN  STD_LOGIC;
        RESET      :  IN  STD_LOGIC;
        SIM_DONE   :  OUT STD_LOGIC;
        STATUS     :  OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
      );
 END COMPONENT;
 ------------------------
 COMPONENT fifoonly_adcfifo_exdes IS
   PORT (
           WR_CLK                    : IN  std_logic;
     	   RD_CLK                    : IN  std_logic;
           RST                       : IN  std_logic;
           WR_EN 		     : IN  std_logic;
           RD_EN                     : IN  std_logic;
           DIN                       : IN  std_logic_vector(32-1 DOWNTO 0);
           DOUT                      : OUT std_logic_vector(8-1 DOWNTO 0);
           FULL                      : OUT std_logic;
           EMPTY                     : OUT std_logic);

 END COMPONENT;
 ------------------------ 


END fifoonly_adcfifo_pkg;



PACKAGE BODY fifoonly_adcfifo_pkg IS

 FUNCTION divroundup (
    data_value : INTEGER;
    divisor : INTEGER)
  RETURN INTEGER IS
    VARIABLE div                   : INTEGER;
  BEGIN
    div   := data_value/divisor;
    IF ( (data_value MOD divisor) /= 0) THEN
      div := div+1;
    END IF;
    RETURN div;
  END divroundup;
  ---------------------------------
  FUNCTION if_then_else (
    condition : BOOLEAN;
    true_case : INTEGER;
    false_case : INTEGER)
  RETURN INTEGER IS
    VARIABLE retval : INTEGER := 0;
  BEGIN
    IF condition=false THEN
      retval:=false_case;
    ELSE
      retval:=true_case;
    END IF;
    RETURN retval;
  END if_then_else;
    ---------------------------------
  FUNCTION if_then_else (
    condition : BOOLEAN;
    true_case : STD_LOGIC;
    false_case : STD_LOGIC)
  RETURN STD_LOGIC IS
    VARIABLE retval : STD_LOGIC := '0';
  BEGIN
    IF condition=false THEN
      retval:=false_case;
    ELSE
      retval:=true_case;
    END IF;
    RETURN retval;
  END if_then_else;
  ---------------------------------
  FUNCTION if_then_else (
    condition : BOOLEAN;
    true_case : TIME;
    false_case : TIME)
  RETURN TIME IS
    VARIABLE retval : TIME := 0 ps;
  BEGIN
    IF condition=false THEN
      retval:=false_case;
    ELSE
      retval:=true_case;
    END IF;
    RETURN retval;
  END if_then_else;
  ------------------------------- 
  FUNCTION log2roundup (
      data_value : INTEGER)
    RETURN INTEGER IS

      VARIABLE width       : INTEGER := 0;
      VARIABLE cnt         : INTEGER := 1;
    BEGIN
      IF (data_value <= 1) THEN
        width   := 1;
      ELSE
        WHILE (cnt < data_value) LOOP
          width := width + 1;
          cnt   := cnt *2;
        END LOOP;
      END IF;

      RETURN width;
    END log2roundup;
  ------------------------------------------------------------------------------
  -- hexstr_to_std_logic_vec
  --  This function converts a hex string to a std_logic_vector
  ------------------------------------------------------------------------------
  FUNCTION hexstr_to_std_logic_vec( 
    arg1 : string; 
    size : integer ) 
  RETURN std_logic_vector IS
    VARIABLE result : std_logic_vector(size-1 DOWNTO 0) := (OTHERS => '0');
    VARIABLE bin    : std_logic_vector(3 DOWNTO 0);
    VARIABLE index  : integer                           := 0;
  BEGIN
    FOR i IN arg1'reverse_range LOOP
      CASE arg1(i) IS
        WHEN '0' => bin := (OTHERS => '0');
        WHEN '1' => bin := (0 => '1', OTHERS => '0');
        WHEN '2' => bin := (1 => '1', OTHERS => '0');
        WHEN '3' => bin := (0 => '1', 1 => '1', OTHERS => '0');
        WHEN '4' => bin := (2 => '1', OTHERS => '0');
        WHEN '5' => bin := (0 => '1', 2 => '1', OTHERS => '0');
        WHEN '6' => bin := (1 => '1', 2 => '1', OTHERS => '0');
        WHEN '7' => bin := (3 => '0', OTHERS => '1');
        WHEN '8' => bin := (3 => '1', OTHERS => '0');
        WHEN '9' => bin := (0 => '1', 3 => '1', OTHERS => '0');
        WHEN 'A' => bin := (0 => '0', 2 => '0', OTHERS => '1');
        WHEN 'a' => bin := (0 => '0', 2 => '0', OTHERS => '1');
        WHEN 'B' => bin := (2 => '0', OTHERS => '1');
        WHEN 'b' => bin := (2 => '0', OTHERS => '1');
        WHEN 'C' => bin := (0 => '0', 1 => '0', OTHERS => '1');
        WHEN 'c' => bin := (0 => '0', 1 => '0', OTHERS => '1');
        WHEN 'D' => bin := (1 => '0', OTHERS => '1');
        WHEN 'd' => bin := (1 => '0', OTHERS => '1');
        WHEN 'E' => bin := (0 => '0', OTHERS => '1');
        WHEN 'e' => bin := (0 => '0', OTHERS => '1');
        WHEN 'F' => bin := (OTHERS => '1');
        WHEN 'f' => bin := (OTHERS => '1');
        WHEN OTHERS =>
          FOR j IN 0 TO 3 LOOP
            bin(j) := 'X';
          END LOOP;
      END CASE;
      FOR j IN 0 TO 3 LOOP
        IF (index*4)+j < size THEN
          result((index*4)+j) := bin(j);
        END IF;
      END LOOP;
      index := index + 1;
    END LOOP;
    RETURN result;
  END hexstr_to_std_logic_vec;

END fifoonly_adcfifo_pkg;
