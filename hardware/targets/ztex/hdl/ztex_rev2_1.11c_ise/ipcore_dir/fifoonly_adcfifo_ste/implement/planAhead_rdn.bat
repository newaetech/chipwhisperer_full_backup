#!/bin/sh
rem (c) Copyright 2009 - 2010 Xilinx, Inc. All rights reserved.
rem 
rem This file contains confidential and proprietary information
rem of Xilinx, Inc. and is protected under U.S. and
rem international copyright and other intellectual property
rem laws.
rem 
rem DISCLAIMER
rem This disclaimer is not a license and does not grant any
rem rights to the materials distributed herewith. Except as
rem otherwise provided in a valid license issued to you by
rem Xilinx, and to the maximum extent permitted by applicable
rem law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
rem WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
rem AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
rem BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
rem INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
rem (2) Xilinx shall not be liable (whether in contract or tort,
rem including negligence, or under any other theory of
rem liability) for any loss or damage of any kind or nature
rem related to, arising under or in connection with these
rem materials, including for any direct, or any indirect,
rem special, incidental, or consequential loss or damage
rem (including loss of data, profits, goodwill, or any type of
rem loss or damage suffered as a result of any action brought
rem by a third party) even if such damage or loss was
rem reasonably foreseeable or Xilinx had been advised of the
rem possibility of the same.
rem 
rem CRITICAL APPLICATIONS
rem Xilinx products are not designed or intended to be fail-
rem safe, or for use in any application requiring fail-safe
rem performance, such as life-support or safety devices or
rem systems, Class III medical devices, nuclear facilities,
rem applications related to the deployment of airbags, or any
rem other applications that could lead to death, personal
rem injury, or severe property or environmental damage
rem (individually and collectively, "Critical
rem Applications"). Customer assumes the sole risk and
rem liability of any use of Xilinx products in Critical
rem Applications, subject only to applicable laws and
rem regulations governing limitations on product liability.
rem 
rem THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
rem PART OF THIS FILE AT ALL TIMES.

rem -----------------------------------------------------------------------------
rem  Script to synthesize and implement the Coregen FIFO Generator
rem -----------------------------------------------------------------------------
rmdir /S /Q results
mkdir results
cd results
copy ..\..\..\tmp\fifoonly_adcfifo.edf .
planAhead -mode batch -source ..\planAhead_rdn.tcl
