-- Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2019.1 (win64) Build 2552052 Fri May 24 14:49:42 MDT 2019
-- Date        : Mon Mar 29 22:41:34 2021
-- Host        : qed running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ dbg_hub_stub.vhdl
-- Design      : dbg_hub
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7a100tftg256-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  Port ( 
    sl_iport0_o : out STD_LOGIC_VECTOR ( 36 downto 0 );
    sl_iport1_o : out STD_LOGIC_VECTOR ( 36 downto 0 );
    sl_oport0_i : in STD_LOGIC_VECTOR ( 16 downto 0 );
    sl_oport1_i : in STD_LOGIC_VECTOR ( 16 downto 0 );
    clk : in STD_LOGIC
  );

end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture stub of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "sl_iport0_o[36:0],sl_iport1_o[36:0],sl_oport0_i[16:0],sl_oport1_i[16:0],clk";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "xsdbm_v3_0_0_xsdbm,Vivado 2019.1";
begin
end;
