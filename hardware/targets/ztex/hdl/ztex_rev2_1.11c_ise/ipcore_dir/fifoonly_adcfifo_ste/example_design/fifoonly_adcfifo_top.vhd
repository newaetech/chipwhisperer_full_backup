--------------------------------------------------------------------------------
--
-- FIFO Generator v8.2 Core - Top-level core wrapper
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
-- Filename: <componenet name>_top.vhd
--
-- Description:
--   This is the actual FIFO core wrapper.
--
--------------------------------------------------------------------------------
-- Library Declarations
--------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library unisim;
use unisim.vcomponents.all;

--------------------------------------------------------------------------------
-- Entity Declaration
--------------------------------------------------------------------------------
entity fifoonly_adcfifo_top is
  PORT (
      CLK                            : IN STD_LOGIC;
      BACKUP                         : IN STD_LOGIC;
      BACKUP_MARKER                  : IN STD_LOGIC;
      DIN                            : IN STD_LOGIC_VECTOR(32-1 downto 0);
      PROG_EMPTY_THRESH              : IN STD_LOGIC_VECTOR(15-1 downto 0);
      PROG_EMPTY_THRESH_ASSERT       : IN STD_LOGIC_VECTOR(15-1 downto 0);
      PROG_EMPTY_THRESH_NEGATE       : IN STD_LOGIC_VECTOR(15-1 downto 0);
      PROG_FULL_THRESH               : IN STD_LOGIC_VECTOR(13-1 downto 0);
      PROG_FULL_THRESH_ASSERT        : IN STD_LOGIC_VECTOR(13-1 downto 0);
      PROG_FULL_THRESH_NEGATE        : IN STD_LOGIC_VECTOR(13-1 downto 0);
      RD_CLK                         : IN STD_LOGIC;
      RD_EN                          : IN STD_LOGIC;
      RD_RST                         : IN STD_LOGIC;
      RST                            : IN STD_LOGIC;
      SRST                           : IN STD_LOGIC;
      WR_CLK                         : IN STD_LOGIC;
      WR_EN                          : IN STD_LOGIC;
      WR_RST                         : IN STD_LOGIC;
      INJECTDBITERR                  : IN STD_LOGIC;
      INJECTSBITERR                  : IN STD_LOGIC;
      ALMOST_EMPTY                   : OUT STD_LOGIC;
      ALMOST_FULL                    : OUT STD_LOGIC;
      DATA_COUNT                     : OUT STD_LOGIC_VECTOR(13-1 downto 0);
      DOUT                           : OUT STD_LOGIC_VECTOR(8-1 downto 0);
      EMPTY                          : OUT STD_LOGIC;
      FULL                           : OUT STD_LOGIC;
      OVERFLOW                       : OUT STD_LOGIC;
      PROG_EMPTY                     : OUT STD_LOGIC;
      PROG_FULL                      : OUT STD_LOGIC;
      VALID                          : OUT STD_LOGIC;
      RD_DATA_COUNT                  : OUT STD_LOGIC_VECTOR(15-1 downto 0);
      UNDERFLOW                      : OUT STD_LOGIC;
      WR_ACK                         : OUT STD_LOGIC;
      WR_DATA_COUNT                  : OUT STD_LOGIC_VECTOR(13-1 downto 0);
      SBITERR                        : OUT STD_LOGIC;
      DBITERR                        : OUT STD_LOGIC;
      -- AXI Global Signal
      M_ACLK                         : IN  std_logic;
      S_ACLK                         : IN  std_logic;
      S_ARESETN                      : IN  std_logic;
      M_ACLK_EN                      : IN  std_logic;
      S_ACLK_EN                      : IN  std_logic;
      -- AXI Full/Lite Slave Write Channel (write side)
      S_AXI_AWID                     : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_AWADDR                   : IN  std_logic_vector(32-1 DOWNTO 0);
      S_AXI_AWLEN                    : IN  std_logic_vector(8-1 DOWNTO 0);
      S_AXI_AWSIZE                   : IN  std_logic_vector(3-1 DOWNTO 0);
      S_AXI_AWBURST                  : IN  std_logic_vector(2-1 DOWNTO 0);
      S_AXI_AWLOCK                   : IN  std_logic_vector(2-1 DOWNTO 0);
      S_AXI_AWCACHE                  : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_AWPROT                   : IN  std_logic_vector(3-1 DOWNTO 0);
      S_AXI_AWQOS                    : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_AWREGION                 : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_AWUSER                   : IN  std_logic_vector(1-1 DOWNTO 0);
      S_AXI_AWVALID                  : IN  std_logic;
      S_AXI_AWREADY                  : OUT std_logic;
      S_AXI_WID                      : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_WDATA                    : IN  std_logic_vector(64-1 DOWNTO 0);
      S_AXI_WSTRB                    : IN  std_logic_vector(8-1 DOWNTO 0);
      S_AXI_WLAST                    : IN  std_logic;
      S_AXI_WUSER                    : IN  std_logic_vector(1-1 DOWNTO 0);
      S_AXI_WVALID                   : IN  std_logic;
      S_AXI_WREADY                   : OUT std_logic;
      S_AXI_BID                      : OUT std_logic_vector(4-1 DOWNTO 0);
      S_AXI_BRESP                    : OUT std_logic_vector(2-1 DOWNTO 0);
      S_AXI_BUSER                    : OUT std_logic_vector(1-1 DOWNTO 0);
      S_AXI_BVALID                   : OUT std_logic;
      S_AXI_BREADY                   : IN  std_logic;
      -- AXI Full/Lite Master Write Channel (Read side)
      M_AXI_AWID                     : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_AWADDR                   : OUT std_logic_vector(32-1 DOWNTO 0);
      M_AXI_AWLEN                    : OUT std_logic_vector(8-1 DOWNTO 0);
      M_AXI_AWSIZE                   : OUT std_logic_vector(3-1 DOWNTO 0);
      M_AXI_AWBURST                  : OUT std_logic_vector(2-1 DOWNTO 0);
      M_AXI_AWLOCK                   : OUT std_logic_vector(2-1 DOWNTO 0);
      M_AXI_AWCACHE                  : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_AWPROT                   : OUT std_logic_vector(3-1 DOWNTO 0);
      M_AXI_AWQOS                    : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_AWREGION                 : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_AWUSER                   : OUT std_logic_vector(1-1 DOWNTO 0);
      M_AXI_AWVALID                  : OUT std_logic;
      M_AXI_AWREADY                  : IN  std_logic;
      M_AXI_WID                      : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_WDATA                    : OUT std_logic_vector(64-1 DOWNTO 0);
      M_AXI_WSTRB                    : OUT std_logic_vector(8-1 DOWNTO 0);
      M_AXI_WLAST                    : OUT std_logic;
      M_AXI_WUSER                    : OUT std_logic_vector(1-1 DOWNTO 0);
      M_AXI_WVALID                   : OUT std_logic;
      M_AXI_WREADY                   : IN  std_logic;
      M_AXI_BID                      : IN  std_logic_vector(4-1 DOWNTO 0);
      M_AXI_BRESP                    : IN  std_logic_vector(2-1 DOWNTO 0);
      M_AXI_BUSER                    : IN  std_logic_vector(1-1 DOWNTO 0);
      M_AXI_BVALID                   : IN  std_logic;
      M_AXI_BREADY                   : OUT std_logic;
      -- AXI Full/Lite Slave Read Channel (Write side)
      S_AXI_ARID                     : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_ARADDR                   : IN  std_logic_vector(32-1 DOWNTO 0);
      S_AXI_ARLEN                    : IN  std_logic_vector(8-1 DOWNTO 0);
      S_AXI_ARSIZE                   : IN  std_logic_vector(3-1 DOWNTO 0);
      S_AXI_ARBURST                  : IN  std_logic_vector(2-1 DOWNTO 0);
      S_AXI_ARLOCK                   : IN  std_logic_vector(2-1 DOWNTO 0);
      S_AXI_ARCACHE                  : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_ARPROT                   : IN  std_logic_vector(3-1 DOWNTO 0);
      S_AXI_ARQOS                    : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_ARREGION                 : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXI_ARUSER                   : IN  std_logic_vector(1-1 DOWNTO 0);
      S_AXI_ARVALID                  : IN  std_logic;
      S_AXI_ARREADY                  : OUT std_logic;
      S_AXI_RID                      : OUT std_logic_vector(4-1 DOWNTO 0);
      S_AXI_RDATA                    : OUT std_logic_vector(64-1 DOWNTO 0);
      S_AXI_RRESP                    : OUT std_logic_vector(2-1 DOWNTO 0);
      S_AXI_RLAST                    : OUT std_logic;
      S_AXI_RUSER                    : OUT std_logic_vector(1-1 DOWNTO 0);
      S_AXI_RVALID                   : OUT std_logic;
      S_AXI_RREADY                   : IN  std_logic;
      -- AXI Full/Lite Master Read Channel (Read side)
      M_AXI_ARID                     : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_ARADDR                   : OUT std_logic_vector(32-1 DOWNTO 0);
      M_AXI_ARLEN                    : OUT std_logic_vector(8-1 DOWNTO 0);
      M_AXI_ARSIZE                   : OUT std_logic_vector(3-1 DOWNTO 0);
      M_AXI_ARBURST                  : OUT std_logic_vector(2-1 DOWNTO 0);
      M_AXI_ARLOCK                   : OUT std_logic_vector(2-1 DOWNTO 0);
      M_AXI_ARCACHE                  : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_ARPROT                   : OUT std_logic_vector(3-1 DOWNTO 0);
      M_AXI_ARQOS                    : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_ARREGION                 : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXI_ARUSER                   : OUT std_logic_vector(1-1 DOWNTO 0);
      M_AXI_ARVALID                  : OUT std_logic;
      M_AXI_ARREADY                  : IN  std_logic;
      M_AXI_RID                      : IN  std_logic_vector(4-1 DOWNTO 0);
      M_AXI_RDATA                    : IN  std_logic_vector(64-1 DOWNTO 0);
      M_AXI_RRESP                    : IN  std_logic_vector(2-1 DOWNTO 0);
      M_AXI_RLAST                    : IN  std_logic;
      M_AXI_RUSER                    : IN  std_logic_vector(1-1 DOWNTO 0);
      M_AXI_RVALID                   : IN  std_logic;
      M_AXI_RREADY                   : OUT std_logic;
      -- AXI Streaming Slave Signals (Write side)
      S_AXIS_TVALID                  : IN  std_logic;
      S_AXIS_TREADY                  : OUT std_logic;
      S_AXIS_TDATA                   : IN  std_logic_vector(64-1 DOWNTO 0);
      S_AXIS_TSTRB                   : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXIS_TKEEP                   : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXIS_TLAST                   : IN  std_logic;
      S_AXIS_TID                     : IN  std_logic_vector(8-1 DOWNTO 0);
      S_AXIS_TDEST                   : IN  std_logic_vector(4-1 DOWNTO 0);
      S_AXIS_TUSER                   : IN  std_logic_vector(4-1 DOWNTO 0);
      -- AXI Streaming Master Signals (Read side)
      M_AXIS_TVALID                  : OUT std_logic;
      M_AXIS_TREADY                  : IN  std_logic;
      M_AXIS_TDATA                   : OUT std_logic_vector(64-1 DOWNTO 0);
      M_AXIS_TSTRB                   : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXIS_TKEEP                   : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXIS_TLAST                   : OUT std_logic;
      M_AXIS_TID                     : OUT std_logic_vector(8-1 DOWNTO 0);
      M_AXIS_TDEST                   : OUT std_logic_vector(4-1 DOWNTO 0);
      M_AXIS_TUSER                   : OUT std_logic_vector(4-1 DOWNTO 0);
      -- AXI Full/Lite Write Address Channel Signals
      AXI_AW_INJECTSBITERR           : IN  std_logic;
      AXI_AW_INJECTDBITERR           : IN  std_logic;
      AXI_AW_PROG_FULL_THRESH        : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_AW_PROG_EMPTY_THRESH       : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_AW_DATA_COUNT              : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AW_WR_DATA_COUNT           : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AW_RD_DATA_COUNT           : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AW_SBITERR                 : OUT std_logic;
      AXI_AW_DBITERR                 : OUT std_logic;
      AXI_AW_OVERFLOW                : OUT std_logic;
      AXI_AW_UNDERFLOW               : OUT std_logic;
      -- AXI Full/Lite Write Data Channel Signals
      AXI_W_INJECTSBITERR            : IN  std_logic;
      AXI_W_INJECTDBITERR            : IN  std_logic;
      AXI_W_PROG_FULL_THRESH         : IN  std_logic_vector(10-1 DOWNTO 0);
      AXI_W_PROG_EMPTY_THRESH        : IN  std_logic_vector(10-1 DOWNTO 0);
      AXI_W_DATA_COUNT               : OUT std_logic_vector(10 DOWNTO 0);
      AXI_W_WR_DATA_COUNT            : OUT std_logic_vector(10 DOWNTO 0);
      AXI_W_RD_DATA_COUNT            : OUT std_logic_vector(10 DOWNTO 0);
      AXI_W_SBITERR                  : OUT std_logic;
      AXI_W_DBITERR                  : OUT std_logic;
      AXI_W_OVERFLOW                 : OUT std_logic;
      AXI_W_UNDERFLOW                : OUT std_logic;
      -- AXI Full/Lite Write Response Channel Signals
      AXI_B_INJECTSBITERR            : IN  std_logic;
      AXI_B_INJECTDBITERR            : IN  std_logic;
      AXI_B_PROG_FULL_THRESH         : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_B_PROG_EMPTY_THRESH        : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_B_DATA_COUNT               : OUT std_logic_vector(4 DOWNTO 0);
      AXI_B_WR_DATA_COUNT            : OUT std_logic_vector(4 DOWNTO 0);
      AXI_B_RD_DATA_COUNT            : OUT std_logic_vector(4 DOWNTO 0);
      AXI_B_SBITERR                  : OUT std_logic;
      AXI_B_DBITERR                  : OUT std_logic;
      AXI_B_OVERFLOW                 : OUT std_logic;
      AXI_B_UNDERFLOW                : OUT std_logic;
      -- AXI Full/Lite Read Address Channel Signals
      AXI_AR_INJECTSBITERR           : IN  std_logic;
      AXI_AR_INJECTDBITERR           : IN  std_logic;
      AXI_AR_PROG_FULL_THRESH        : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_AR_PROG_EMPTY_THRESH       : IN  std_logic_vector(4-1 DOWNTO 0);
      AXI_AR_DATA_COUNT              : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AR_WR_DATA_COUNT           : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AR_RD_DATA_COUNT           : OUT std_logic_vector(4 DOWNTO 0);
      AXI_AR_SBITERR                 : OUT std_logic;
      AXI_AR_DBITERR                 : OUT std_logic;
      AXI_AR_OVERFLOW                : OUT std_logic;
      AXI_AR_UNDERFLOW               : OUT std_logic;
      -- AXI Full/Lite Read Data Channel Signals
      AXI_R_INJECTSBITERR            : IN  std_logic;
      AXI_R_INJECTDBITERR            : IN  std_logic;
      AXI_R_PROG_FULL_THRESH         : IN  std_logic_vector(10-1 DOWNTO 0);
      AXI_R_PROG_EMPTY_THRESH        : IN  std_logic_vector(10-1 DOWNTO 0);
      AXI_R_DATA_COUNT               : OUT std_logic_vector(10 DOWNTO 0);
      AXI_R_WR_DATA_COUNT            : OUT std_logic_vector(10 DOWNTO 0);
      AXI_R_RD_DATA_COUNT            : OUT std_logic_vector(10 DOWNTO 0);
      AXI_R_SBITERR                  : OUT std_logic;
      AXI_R_DBITERR                  : OUT std_logic;
      AXI_R_OVERFLOW                 : OUT std_logic;
      AXI_R_UNDERFLOW                : OUT std_logic;
      -- AXI Streaming FIFO Related Signals
      AXIS_INJECTSBITERR             : IN  std_logic;
      AXIS_INJECTDBITERR             : IN  std_logic;
      AXIS_PROG_FULL_THRESH          : IN  std_logic_vector(10-1 DOWNTO 0);
      AXIS_PROG_EMPTY_THRESH         : IN  std_logic_vector(10-1 DOWNTO 0);
      AXIS_DATA_COUNT                : OUT std_logic_vector(10 DOWNTO 0);
      AXIS_WR_DATA_COUNT             : OUT std_logic_vector(10 DOWNTO 0);
      AXIS_RD_DATA_COUNT             : OUT std_logic_vector(10 DOWNTO 0);
      AXIS_SBITERR                   : OUT std_logic;
      AXIS_DBITERR                   : OUT std_logic;
      AXIS_OVERFLOW                  : OUT std_logic;
      AXIS_UNDERFLOW                 : OUT std_logic);

end fifoonly_adcfifo_top;



architecture xilinx of fifoonly_adcfifo_top is

   SIGNAL WR_CLK_i : std_logic;
   SIGNAL RD_CLK_i : std_logic;



  component fifoonly_adcfifo is
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
  end component;


begin

  fg0 : fifoonly_adcfifo
    port map (
           WR_CLK                    => WR_CLK_i,
           RD_CLK                    => RD_CLK_i,
           RST                       => RST,
           WR_EN 		     => WR_EN,
           RD_EN                     => RD_EN,
           DIN                       => DIN,
           DOUT                      => DOUT,
           FULL                      => FULL,
           EMPTY                     => EMPTY);


wr_clk_buf: bufg
    PORT map(
      i => WR_CLK,
      o => WR_CLK_i
      );

rd_clk_buf: bufg
    PORT map(
      i => RD_CLK,
      o => RD_CLK_i
      );


end xilinx;
