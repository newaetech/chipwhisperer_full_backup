/****************************************************************************************
*
*   Disclaimer   This software code and all associated documentation, comments or other 
*  of Warranty:  information (collectively "Software") is provided "AS IS" without 
*                warranty of any kind. MICRON TECHNOLOGY, INC. ("MTI") EXPRESSLY 
*                DISCLAIMS ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
*                TO, NONINFRINGEMENT OF THIRD PARTY RIGHTS, AND ANY IMPLIED WARRANTIES 
*                OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE. MTI DOES NOT 
*                WARRANT THAT THE SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE 
*                OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE. 
*                FURTHERMORE, MTI DOES NOT MAKE ANY REPRESENTATIONS REGARDING THE USE OR 
*                THE RESULTS OF THE USE OF THE SOFTWARE IN TERMS OF ITS CORRECTNESS, 
*                ACCURACY, RELIABILITY, OR OTHERWISE. THE ENTIRE RISK ARISING OUT OF USE 
*                OR PERFORMANCE OF THE SOFTWARE REMAINS WITH YOU. IN NO EVENT SHALL MTI, 
*                ITS AFFILIATED COMPANIES OR THEIR SUPPLIERS BE LIABLE FOR ANY DIRECT, 
*                INDIRECT, CONSEQUENTIAL, INCIDENTAL, OR SPECIAL DAMAGES (INCLUDING, 
*                WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS INTERRUPTION, 
*                OR LOSS OF INFORMATION) ARISING OUT OF YOUR USE OF OR INABILITY TO USE 
*                THE SOFTWARE, EVEN IF MTI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH 
*                DAMAGES. Because some jurisdictions prohibit the exclusion or 
*                limitation of liability for consequential or incidental damages, the 
*                above limitation may not apply to you.
*
*                Copyright 2008 Micron Technology, Inc. All rights reserved.
*
****************************************************************************************/

//SPECIFIC TO OUR SIMULATION - ADJUST AS NEEDED
`define x512Mb
`define FULL_MEM 
`define sg5
`define x16

// 07.25.08 - Added design T36N from Rev A 07/08
// 12.10.08 - Updated T36N to Rev B 11/08

    // Timing parameters based on Speed Grade

`ifdef x128Mb                                
`ifdef sg5                                //              Timing Parameters for -5 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.40; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    40.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    55.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    15.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    15.0; // tRP    ns    Precharge command period
    parameter tRRD             =    10.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     5.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg54                         //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    58.2; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    16.2; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    16.2; // tRP    ns    Precharge command period
    parameter tRRD             =    10.8; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     5.4; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg6                          //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.50; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    60.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    18.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    18.0; // tRP    ns    Precharge command period
    parameter tRRD             =    12.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `define sg75                        //              Timing Parameters for -75 (CL = 3)
    parameter tAC3_max         =     6.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.60; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     6.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    45.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    75.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    22.5; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    22.5; // tRP    ns    Precharge command period
    parameter tRRD             =    15.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     7.5; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`endif `endif `endif

    parameter tAC2_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC3_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tLZ              =     1.0; // tLZ    ns    Data-out low Z window from CK/CK#
    parameter tMRD             =     2.0; // tMRD  tCK    Load Mode Register command cycle time
    parameter tRFC             =    80.0; // tRFC   ns    Refresh to Refresh Command interval time
    parameter tSRC             =     1.0; // tSRC  tCK    SRR READ command to first valid command (Not Applicable for 128Mb, 256Mb Parts)
    parameter tSRR             =     2.0; // tSRR  tCK    SRR command to SRR READ command         (Not Applicable for 128Mb, 256Mb Parts)
    parameter tWR              =    15.0; // tWR    ns    Write recovery time

    // Size Parameters based on Part Width
`ifdef x16
    parameter ADDR_BITS        =      12; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      12; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      16; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       2; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       2; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       9; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
`else `define x32
    parameter ADDR_BITS        =      12; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      12; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       8; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
`endif

    // For use with the Multi Chip Package
`ifdef DUAL_RANK
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       2; // Set this parameter to control how many Ranks on the mcp are used
`else 
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       1; // Set this parameter to control how many Ranks on the mcp are used
`endif

    parameter full_mem_bits    = BA_BITS+ADDR_BITS+COL_BITS; // Set this parameter to control how many unique addresses are used
    parameter part_mem_bits    = 10;                   // Set this parameter to control how many unique addresses are used
    parameter part_size        = 128;                  // Set this parameter to indicate part size(512Mb, 256Mb, 128Mb)
    parameter tCH_MAX          = 0.55;                 // Clk high level width
    parameter tCH_MIN          = 0.45;                 // Clk high level width
    parameter tCL_MAX          = 0.55;                 // Clk low level width
    parameter tCL_MIN          = 0.45;                 // Clk low level width
    parameter tCKE             = 2.0;                  // Minimum tCKE High/Low time (in tCK's)
    parameter CL_MAX           = 3;                    // Maximum CAS Latency
    parameter BL_MAX           =    16  ;

`else `ifdef x256Mb
`ifdef sg5                                //              Timing Parameters for -5 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.40; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    40.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    55.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    15.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    15.0; // tRP    ns    Precharge command period
    parameter tRRD             =    10.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     5.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg54                         //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    58.2; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    16.2; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    16.2; // tRP    ns    Precharge command period
    parameter tRRD             =    10.8; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     5.4; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg6                          //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.50; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    60.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    18.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    18.0; // tRP    ns    Precharge command period
    parameter tRRD             =    12.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `define sg75                        //              Timing Parameters for -75 (CL = 3)
    parameter tAC3_max         =     6.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.60; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     6.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    45.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    67.5; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    22.5; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    22.5; // tRP    ns    Precharge command period
    parameter tRRD             =    15.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     7.5; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`endif `endif `endif

    parameter tAC2_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC3_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tLZ              =     1.0; // tLZ    ns    Data-out low Z window from CK/CK#
    parameter tMRD             =     2.0; // tMRD  tCK    Load Mode Register command cycle time
    parameter tRFC             =    70.0; // tRFC   ns    Refresh to Refresh Command interval time
    parameter tSRC             =     1.0; // tSRC  tCK    SRR READ command to first valid command (Not Applicable for 128Mb, 256Mb Parts)
    parameter tSRR             =     2.0; // tSRR  tCK    SRR command to SRR READ command         (Not Applicable for 128Mb, 256Mb Parts)
    parameter tWR              =    15.0; // tWR    ns    Write recovery time
    parameter tCH_MAX          =    0.55;                 // Clk high level width
    parameter tCH_MIN          =    0.45;                 // Clk high level width
    parameter tCL_MAX          =    0.55;                 // Clk low level width
    parameter tCL_MIN          =    0.45;                 // Clk low level width
    parameter tCKE             =    1.0 ;                 // Minimum tCKE High/Low time (in tCK's)
    parameter CL_MAX           =    3   ;                 // Maximum CAS Latency
    parameter BL_MAX           =    16  ;

    // Size Parameters based on Part Width
`ifdef x16
    parameter ADDR_BITS        =      13; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      13; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      16; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       2; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       2; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       9; // Set this parameter to control how many Column bits are used
	parameter BA_BITS          =       2; // Bank bits
`else `define x32
    `ifdef RP
    parameter ADDR_BITS        =      13; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      13; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       8; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
    `else
    parameter ADDR_BITS        =      12; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      12; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       9; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
    `endif
`endif

    // For use with the Multi Chip Package
`ifdef DUAL_RANK
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       2; // Set this parameter to control how many Ranks on the mcp are used
`else 
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       1; // Set this parameter to control how many Ranks on the mcp are used
`endif

    parameter full_mem_bits    = BA_BITS+ADDR_BITS+COL_BITS; // Set this parameter to control how many unique addresses are used
    parameter part_mem_bits    = 10;                   // Set this parameter to control how many unique addresses are used
    parameter part_size        = 1024;                 // Set this parameter to indicate part size(1024Mb, 512Mb, 256Mb, 128Mb)

`else `ifdef x512Mb
`ifdef sg5                                //              Timing Parameters for -5 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.40; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    40.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    55.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    15.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    15.0; // tRP    ns    Precharge command period
    parameter tRRD             =    10.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    10.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg54                         //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    59.4; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    16.2; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    16.2; // tRP    ns    Precharge command period
    parameter tRRD             =    10.8; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    10.8; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `ifdef sg6                          //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.50; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    60.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    18.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    18.0; // tRP    ns    Precharge command period
    parameter tRRD             =    12.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`else `define sg75                        //              Timing Parameters for -75 (CL = 3)
    parameter tAC3_max         =     6.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.60; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     6.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    45.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    75.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    22.5; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    22.5; // tRP    ns    Precharge command period
    parameter tRRD             =    15.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     7.5; // tXP    ns    Exit power-down to first valid cmd *note: In data sheet this is specified as one clk, but min tck fails before tXP on the actual part
`endif `endif `endif

    parameter tAC2_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC3_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tLZ              =     1.0; // tLZ    ns    Data-out low Z window from CK/CK#
    parameter tMRD             =     2.0; // tMRD  tCK    Load Mode Register command cycle time
    parameter tRFC             =    97.5; // tRFC   ns    Refresh to Refresh Command interval time
    parameter tSRC             =     1.0; // tSRC  tCK    SRR READ command to first valid command (Not Applicable for 128Mb, 256Mb Parts)
    parameter tSRR             =     2.0; // tSRR  tCK    SRR command to SRR READ command         (Not Applicable for 128Mb, 256Mb Parts)
    parameter tWR              =    15.0; // tWR    ns    Write recovery time

     // Size Parameters based on Part Width
 `ifdef x16
     parameter ADDR_BITS        =      13; // Set this parameter to control how many Address bits are used
     parameter ROW_BITS         =      13; // Set this parameter to control how many Row bits are used
     parameter DQ_BITS          =      16; // Set this parameter to control how many Data bits are used
     parameter DQS_BITS         =       2; // Set this parameter to control how many DQS bits are used
     parameter DM_BITS          =       2; // Set this parameter to control how many DM bits are used
     parameter COL_BITS         =      10; // Set this parameter to control how many Column bits are used
     parameter BA_BITS          =       2; // Bank bits
 `else `define x32
     `ifdef RP
     parameter ADDR_BITS        =      14; // Set this parameter to control how many Address bits are used
     parameter ROW_BITS         =      14; // Set this parameter to control how many Row bits are used
     parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
     parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
     parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
     parameter COL_BITS         =       8; // Set this parameter to control how many Column bits are used
     parameter BA_BITS          =       2; // Bank bits
     `else
     parameter ADDR_BITS        =      13; // Set this parameter to control how many Address bits are used
     parameter ROW_BITS         =      13; // Set this parameter to control how many Row bits are used
     parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
     parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
     parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
     parameter COL_BITS         =       9; // Set this parameter to control how many Column bits are used
     parameter BA_BITS          =       2; // Bank bits
     `endif
`endif

    // For use with the Multi Chip Package
`ifdef DUAL_RANK
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       2; // Set this parameter to control how many Ranks on the mcp are used
`else 
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       1; // Set this parameter to control how many Ranks on the mcp are used
`endif

    parameter full_mem_bits    = BA_BITS+ADDR_BITS+COL_BITS; // Set this parameter to control how many unique addresses are used
    parameter part_mem_bits    = 10;                   // Set this parameter to control how many unique addresses are used
    parameter part_size        = 512;                  // Set this parameter to indicate part size(512Mb, 256Mb, 128Mb)
    parameter tCH_MAX          = 0.55;                 // Clk high level width
    parameter tCH_MIN          = 0.45;                 // Clk high level width
    parameter tCL_MAX          = 0.55;                 // Clk low level width
    parameter tCL_MIN          = 0.45;                 // Clk low level width
    parameter tCKE             = 1.0;                  // Minimum tCKE High/Low time (in tCK's)
    parameter CL_MAX           = 3;                    // Maximum CAS Latency
    parameter BL_MAX           =    16  ;

`else `ifdef x1Gb
`ifdef sg5                                //              Timing Parameters for -5 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.40; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    40.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    55.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    15.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    15.0; // tRP    ns    Precharge command period
    parameter tRRD             =    10.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    10.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
`else `ifdef sg54                         //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    58.2; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    16.2; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    16.2; // tRP    ns    Precharge command period
    parameter tRRD             =    10.8; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    10.8; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
`else `ifdef sg6                          //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    42.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    60.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    18.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    18.0; // tRP    ns    Precharge command period
    parameter tRRD             =    12.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    12.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
`else `define sg75                        //              Timing Parameters for -75 (CL = 3)
    parameter tAC3_max         =     6.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.60; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     6.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    45.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    75.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    22.5; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    22.5; // tRP    ns    Precharge command period
    parameter tRRD             =    15.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    15.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
`endif `endif `endif

    parameter tAC2_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC3_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tLZ              =     1.0; // tLZ    ns    Data-out low Z window from CK/CK#
    parameter tMRD             =     2.0; // tMRD  tCK    Load Mode Register command cycle time
    parameter tRFC             =   110.0; // tRFC   ns    Refresh to Refresh Command interval time
    parameter tSRC             =     1.0; // tSRC  tCK    SRR READ command to first valid command Note: model adds CL to this value                                           
    parameter tSRR             =     2.0; // tSRR  tCK    SRR command to SRR READ command                                                  
    parameter tWR              =    15.0; // tWR    ns    Write recovery time
    parameter tCH_MAX          =    0.55;                 // Clk high level width
    parameter tCH_MIN          =    0.45;                 // Clk high level width
    parameter tCL_MAX          =    0.55;                 // Clk low level width
    parameter tCL_MIN          =    0.45;                 // Clk low level width
    parameter tCKE             =    1.0 ;                 // Minimum tCKE High/Low time (in tCK's)
    parameter CL_MAX           =    3   ;                 // Maximum CAS Latency
    parameter BL_MAX           =    16  ;

     // Size Parameters based on Part Width
`ifdef x16
    parameter ADDR_BITS        =      14; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      14; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      16; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       2; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       2; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =      10; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Set this parameter to control how many Bank bits are used
`else `define x32
  `ifdef RP // reduced page mode
    parameter ADDR_BITS        =      14; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      14; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =       9; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
  `else
    parameter ADDR_BITS        =      13; // Set this parameter to control how many Address bits are used
    parameter ROW_BITS         =      13; // Set this parameter to control how many Row bits are used
    parameter DQ_BITS          =      32; // Set this parameter to control how many Data bits are used
    parameter DQS_BITS         =       4; // Set this parameter to control how many DQS bits are used
    parameter DM_BITS          =       4; // Set this parameter to control how many DM bits are used
    parameter COL_BITS         =      10; // Set this parameter to control how many Column bits are used
    parameter BA_BITS          =       2; // Bank bits
  `endif
`endif

    // For use with the Multi Chip Package
`ifdef DUAL_RANK
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       2; // Set this parameter to control how many Ranks on the mcp are used
`else 
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       1; // Set this parameter to control how many Ranks on the mcp are used
`endif

    parameter full_mem_bits    = BA_BITS+ADDR_BITS+COL_BITS; // Set this parameter to control how many unique addresses are used
    parameter part_mem_bits    = 10;                   // Set this parameter to control how many unique addresses are used
    parameter part_size        = 1024;                 // Set this parameter to indicate part size(1024Mb, 512Mb, 256Mb, 128Mb)


`else `define x2Gb
`ifdef sg5                                //              Timing Parameters for -5 (CL = 3)
    parameter tAC3_max         =     4.8; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     4.8; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     4.8; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.40; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     4.8; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    40.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    55.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    14.4; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    14.4; // tRP    ns    Precharge command period
    parameter tRRD             =    10.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
    parameter tWR              =    14.4; // tWR    ns    Write recovery time
`else `ifdef sg54                         //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     5.4; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    41.8; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    58.2; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    16.2; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    16.2; // tRP    ns    Precharge command period
    parameter tRRD             =    10.8; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     2.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
    parameter tWR              =    15.0; // tWR    ns    Write recovery time
`else `ifdef sg6                          //              Timing Parameters for -6 (CL = 3)
    parameter tAC3_max         =     5.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     6.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.45; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     5.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    41.8; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    60.0; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    18.0; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    18.0; // tRP    ns    Precharge command period
    parameter tRRD             =    12.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =     6.0; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
    parameter tWR              =    15.0; // tWR    ns    Write recovery time
`else `define sg75                        //              Timing Parameters for -75 (CL = 3)
    parameter tAC3_max         =     6.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC2_max         =     6.5; // tAC    ns    Access window of DQ from CK/CK#
    parameter tCK              =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK3_min         =     7.5; // tCK    ns    Nominal Clock Cycle Time
    parameter tCK2_min         =    12.0; // tCK    ns    Nominal Clock Cycle Time
    parameter tDQSQ            =    0.60; // tDQSQ  ns    DQS-DQ skew, DQS to last DQ valid, per group, per access
    parameter tHZ3_max         =     6.0; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tHZ2_max         =     6.5; // tHZ    ns    Data-out high Z window from CK/CK#
    parameter tRAS             =    45.0; // tRAS   ns    Active to Precharge command time
    parameter tRC              =    67.5; // tRC    ns    Active to Active/Auto Refresh command time
    parameter tRCD             =    22.5; // tRCD   ns    Active to Read/Write command time
    parameter tRP              =    22.5; // tRP    ns    Precharge command period
    parameter tRRD             =    15.0; // tRRD   ns    Active bank a to Active bank b command time
    parameter tWTR             =     1.0; // tWTR  tCK    Internal Write-to-Read command delay
    parameter tXP              =    7.50; // tXP    ns    Exit power-down to first valid cmd Note: spec'd as 2 * tCK
    parameter tWR              =    15.0; // tWR    ns    Write recovery time
`endif `endif `endif

    parameter tAC2_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tAC3_min         =     2.0; // tAC    ns    Access window of DQ from CK/CK#
    parameter tLZ              =     1.0; // tLZ    ns    Data-out low Z window from CK/CK#
    parameter tMRD             =     2.0; // tMRD  tCK    Load Mode Register command cycle time
    parameter tRFC             =    72.0; // tRFC   ns    Refresh to Refresh Command interval time
    parameter tSRC             =     1.0; // tSRC  tCK    SRR READ command to first valid command Note: model adds CL to this value                                           
    parameter tSRR             =     2.0; // tSRR  tCK    SRR command to SRR READ command                                                  
    parameter tCH_MAX          =    0.55;                 // Clk high level width
    parameter tCH_MIN          =    0.45;                 // Clk high level width
    parameter tCL_MAX          =    0.55;                 // Clk low level width
    parameter tCL_MIN          =    0.45;                 // Clk low level width
    parameter tCKE             =    1.0 ;                 // Minimum tCKE High/Low time (in tCK's)
    parameter CL_MAX           =    3   ;                 // Maximum CAS Latency
    parameter BL_MAX           =    16  ;

     // Size Parameters based on Part Width
`ifdef x16
    `ifdef RP
        parameter ADDR_BITS    =      15; // Set this parameter to control how many Address bits are used
        parameter ROW_BITS     =      15; // Set this parameter to control how many Row bits are used
        parameter DQ_BITS      =      16; // Set this parameter to control how many Data bits are used
        parameter DQS_BITS     =       2; // Set this parameter to control how many DQS bits are used
        parameter DM_BITS      =       2; // Set this parameter to control how many DM bits are used
        parameter COL_BITS     =      10; // Set this parameter to control how many Column bits are used
        parameter BA_BITS      =       2; // Set this parameter to control how many Bank bits are used
    `else
        parameter ADDR_BITS    =      14; // Set this parameter to control how many Address bits are used
        parameter ROW_BITS     =      14; // Set this parameter to control how many Row bits are used
        parameter DQ_BITS      =      16; // Set this parameter to control how many Data bits are used
        parameter DQS_BITS     =       2; // Set this parameter to control how many DQS bits are used
        parameter DM_BITS      =       2; // Set this parameter to control how many DM bits are used
        parameter COL_BITS     =      11; // Set this parameter to control how many Column bits are used
        parameter BA_BITS      =       2; // Set this parameter to control how many Bank bits are used
    `endif
`else `define x32
    `ifdef RP // reduced page mode
        parameter ADDR_BITS    =      15; // Set this parameter to control how many Address bits are used
        parameter ROW_BITS     =      15; // Set this parameter to control how many Row bits are used
        parameter DQ_BITS      =      32; // Set this parameter to control how many Data bits are used
        parameter DQS_BITS     =       4; // Set this parameter to control how many DQS bits are used
        parameter DM_BITS      =       4; // Set this parameter to control how many DM bits are used
        parameter COL_BITS     =       9; // Set this parameter to control how many Column bits are used
        parameter BA_BITS      =       2; // Bank bits
    `else
        parameter ADDR_BITS    =      14; // Set this parameter to control how many Address bits are used
        parameter ROW_BITS     =      14; // Set this parameter to control how many Row bits are used
        parameter DQ_BITS      =      32; // Set this parameter to control how many Data bits are used
        parameter DQS_BITS     =       4; // Set this parameter to control how many DQS bits are used
        parameter DM_BITS      =       4; // Set this parameter to control how many DM bits are used
        parameter COL_BITS     =      10; // Set this parameter to control how many Column bits are used
        parameter BA_BITS      =       2; // Bank bits
    `endif
`endif

    // For use with the Multi Chip Package
`ifdef DUAL_RANK
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       2; // Set this parameter to control how many Ranks on the mcp are used
`else 
    parameter CS_BITS          =       2; // Set this parameter to control how many Chip Select bits are used
    parameter RANKS            =       1; // Set this parameter to control how many Ranks on the mcp are used
`endif

    parameter full_mem_bits    = BA_BITS+ADDR_BITS+COL_BITS; // Set this parameter to control how many unique addresses are used
    parameter part_mem_bits    = 10;                   // Set this parameter to control how many unique addresses are used
    parameter part_size        = 2048;                 // Set this parameter to indicate part size(1024Mb, 512Mb, 256Mb, 128Mb)
 
`endif `endif `endif `endif
