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
-- Filename: fifoonly_adcfifo_tb.vhd
--
-- Description:
--   This is the demo testbench top file for fifo_generator core.
--
--------------------------------------------------------------------------------
-- Library Declarations
--------------------------------------------------------------------------------
LIBRARY ieee;
LIBRARY std;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.ALL;
USE IEEE.std_logic_arith.ALL;
USE IEEE.std_logic_misc.ALL;
USE ieee.numeric_std.ALL;
USE ieee.std_logic_textio.ALL;
USE std.textio.ALL;

LIBRARY work;
USE work.fifoonly_adcfifo_pkg.ALL;

ENTITY fifoonly_adcfifo_tb IS
END ENTITY;


ARCHITECTURE fifoonly_adcfifo_arch OF fifoonly_adcfifo_tb IS
 SIGNAL  status              : STD_LOGIC_VECTOR(7 DOWNTO 0) := "00000000";
 SIGNAL  wr_clk              : STD_LOGIC;
 SIGNAL  rd_clk              : STD_LOGIC;
 SIGNAL  reset 	             : STD_LOGIC;
 SIGNAL  sim_done            : STD_LOGIC := '0';
 SIGNAL  end_of_sim          : STD_LOGIC_VECTOR(4 DOWNTO 0) := (OTHERS => '0');
 -- Write and Read clock periods
 CONSTANT wr_clk_period_by_2 : TIME := 200 ns;
 CONSTANT rd_clk_period_by_2 : TIME := 100 ns;
 -- Procedures to display strings
 PROCEDURE disp_str(CONSTANT str:IN STRING) IS
    variable dp_l : line := null;   
 BEGIN
  write(dp_l,str);
  writeline(output,dp_l);
 END PROCEDURE;
 
 PROCEDURE disp_hex(signal hex:IN STD_LOGIC_VECTOR(7 DOWNTO 0)) IS
    variable dp_lx : line := null;   
 BEGIN
  hwrite(dp_lx,hex);
  writeline(output,dp_lx);
 END PROCEDURE;

BEGIN
  
  -- Generation of clock

  PROCESS BEGIN
    WAIT FOR 400 ns; -- Wait for global reset
    WHILE 1 = 1 LOOP
      wr_clk <= '0';
      WAIT FOR wr_clk_period_by_2;
      wr_clk <= '1'; 
      WAIT FOR wr_clk_period_by_2;
    END LOOP;
  END PROCESS;

  PROCESS BEGIN
    WAIT FOR 200 ns;-- Wait for global reset
    WHILE 1 = 1 LOOP
      rd_clk <= '0';
      WAIT FOR rd_clk_period_by_2;
      rd_clk <= '1'; 
      WAIT FOR rd_clk_period_by_2;
    END LOOP;
  END PROCESS;
  
  -- Generation of Reset
  
  PROCESS BEGIN
    reset <= '1';
    WAIT FOR 4200 ns;
    reset <= '0';
    WAIT;
  END PROCESS;
  
  
  -- Error message printing based on STATUS signal from fifoonly_adcfifo_synth

  PROCESS(status)
  BEGIN
    IF(status /= "0" AND status /= "1") THEN
      disp_str("STATUS:");
      disp_hex(status);
    END IF;

    IF(status(7) = '1') THEN
      assert false
       report "Data mismatch found"
       severity error;
    END IF;

    IF(status(1) = '1') THEN
    END IF;
    
    IF(status(5) = '1') THEN
      assert false
       report "Empty flag Mismatch/timeout"
       severity error;
    END IF;

    IF(status(6) = '1') THEN
      assert false
       report "Full Flag Mismatch/timeout"
       severity error;
    END IF;
  END PROCESS;


  PROCESS 
  BEGIN
    wait until sim_done = '1';
    IF(status /= "0" AND status /= "1") THEN
      assert false
      report "Simulation failed"
      severity failure;
    ELSE
      assert false
      report "Test Completed Successfully"
      severity failure;
    END IF;
  END PROCESS;
  
  PROCESS
  BEGIN
    wait for 400 ms;
    assert false
    report "Test bench timed out"
    severity failure;
  END PROCESS;

  -- Instance of fifoonly_adcfifo_synth
  
  fifoonly_adcfifo_synth_inst:fifoonly_adcfifo_synth
   GENERIC MAP(
              FREEZEON_ERROR => 0,
 	      TB_STOP_CNT    => 2,
 	      TB_SEED        => 12 
 	      )
  PORT MAP(
	   WR_CLK        => wr_clk,
	   RD_CLK        => rd_clk,
	   RESET         => reset,
	   SIM_DONE      => sim_done,
           STATUS        => status
	  );

END ARCHITECTURE;
