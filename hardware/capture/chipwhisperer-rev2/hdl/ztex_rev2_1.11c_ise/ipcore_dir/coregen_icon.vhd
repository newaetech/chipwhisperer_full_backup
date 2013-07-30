-------------------------------------------------------------------------------
-- Copyright (c) 2013 Xilinx, Inc.
-- All Rights Reserved
-------------------------------------------------------------------------------
--   ____  ____
--  /   /\/   /
-- /___/  \  /    Vendor     : Xilinx
-- \   \   \/     Version    : 14.4
--  \   \         Application: XILINX CORE Generator
--  /   /         Filename   : coregen_icon.vhd
-- /___/   /\     Timestamp  : Mon Jan 28 21:26:32 Atlantic Standard Time 2013
-- \   \  /  \
--  \___\/\___\
--
-- Design Name: VHDL Synthesis Wrapper
-------------------------------------------------------------------------------
-- This wrapper is used to integrate with Project Navigator and PlanAhead

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
ENTITY coregen_icon IS
  port (
    CONTROL0: inout std_logic_vector(35 downto 0));
END coregen_icon;

ARCHITECTURE coregen_icon_a OF coregen_icon IS
BEGIN

END coregen_icon_a;
