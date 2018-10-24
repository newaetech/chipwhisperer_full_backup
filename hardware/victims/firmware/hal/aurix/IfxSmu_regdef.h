/**
 * \file IfxSmu_regdef.h
 * \brief
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 * Version: TC23XADAS_UM_V1.0P1.R0
 * Specification: tc23xadas_um_sfrs_MCSFR.xml (Revision: UM_V1.0p1)
 * MAY BE CHANGED BY USER [yes/no]: No
 *
 *                                 IMPORTANT NOTICE
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * \defgroup IfxLld_Smu Smu
 * \ingroup IfxLld
 * 
 * \defgroup IfxLld_Smu_Bitfields Bitfields
 * \ingroup IfxLld_Smu
 * 
 * \defgroup IfxLld_Smu_union Union
 * \ingroup IfxLld_Smu
 * 
 * \defgroup IfxLld_Smu_struct Struct
 * \ingroup IfxLld_Smu
 * 
 */
#ifndef IFXSMU_REGDEF_H
#define IFXSMU_REGDEF_H 1
/******************************************************************************/
#include "Ifx_TypesReg.h"
/******************************************************************************/
/** \addtogroup IfxLld_Smu_Bitfields
 * \{  */

/** \\brief  SMU Access Enable Register 0 */
typedef struct _Ifx_SMU_ACCEN0_Bits
{
    unsigned int EN0:1;                     /**< \brief [0:0] Access Enable for Master TAG ID 0 (rw) */
    unsigned int EN1:1;                     /**< \brief [1:1] Access Enable for Master TAG ID 1 (rw) */
    unsigned int EN2:1;                     /**< \brief [2:2] Access Enable for Master TAG ID 2 (rw) */
    unsigned int EN3:1;                     /**< \brief [3:3] Access Enable for Master TAG ID 3 (rw) */
    unsigned int EN4:1;                     /**< \brief [4:4] Access Enable for Master TAG ID 4 (rw) */
    unsigned int EN5:1;                     /**< \brief [5:5] Access Enable for Master TAG ID 5 (rw) */
    unsigned int EN6:1;                     /**< \brief [6:6] Access Enable for Master TAG ID 6 (rw) */
    unsigned int EN7:1;                     /**< \brief [7:7] Access Enable for Master TAG ID 7 (rw) */
    unsigned int EN8:1;                     /**< \brief [8:8] Access Enable for Master TAG ID 8 (rw) */
    unsigned int EN9:1;                     /**< \brief [9:9] Access Enable for Master TAG ID 9 (rw) */
    unsigned int EN10:1;                    /**< \brief [10:10] Access Enable for Master TAG ID 10 (rw) */
    unsigned int EN11:1;                    /**< \brief [11:11] Access Enable for Master TAG ID 11 (rw) */
    unsigned int EN12:1;                    /**< \brief [12:12] Access Enable for Master TAG ID 12 (rw) */
    unsigned int EN13:1;                    /**< \brief [13:13] Access Enable for Master TAG ID 13 (rw) */
    unsigned int EN14:1;                    /**< \brief [14:14] Access Enable for Master TAG ID 14 (rw) */
    unsigned int EN15:1;                    /**< \brief [15:15] Access Enable for Master TAG ID 15 (rw) */
    unsigned int EN16:1;                    /**< \brief [16:16] Access Enable for Master TAG ID 16 (rw) */
    unsigned int EN17:1;                    /**< \brief [17:17] Access Enable for Master TAG ID 17 (rw) */
    unsigned int EN18:1;                    /**< \brief [18:18] Access Enable for Master TAG ID 18 (rw) */
    unsigned int EN19:1;                    /**< \brief [19:19] Access Enable for Master TAG ID 19 (rw) */
    unsigned int EN20:1;                    /**< \brief [20:20] Access Enable for Master TAG ID 20 (rw) */
    unsigned int EN21:1;                    /**< \brief [21:21] Access Enable for Master TAG ID 21 (rw) */
    unsigned int EN22:1;                    /**< \brief [22:22] Access Enable for Master TAG ID 22 (rw) */
    unsigned int EN23:1;                    /**< \brief [23:23] Access Enable for Master TAG ID 23 (rw) */
    unsigned int EN24:1;                    /**< \brief [24:24] Access Enable for Master TAG ID 24 (rw) */
    unsigned int EN25:1;                    /**< \brief [25:25] Access Enable for Master TAG ID 25 (rw) */
    unsigned int EN26:1;                    /**< \brief [26:26] Access Enable for Master TAG ID 26 (rw) */
    unsigned int EN27:1;                    /**< \brief [27:27] Access Enable for Master TAG ID 27 (rw) */
    unsigned int EN28:1;                    /**< \brief [28:28] Access Enable for Master TAG ID 28 (rw) */
    unsigned int EN29:1;                    /**< \brief [29:29] Access Enable for Master TAG ID 29 (rw) */
    unsigned int EN30:1;                    /**< \brief [30:30] Access Enable for Master TAG ID 30 (rw) */
    unsigned int EN31:1;                    /**< \brief [31:31] Access Enable for Master TAG ID 31 (rw) */
} Ifx_SMU_ACCEN0_Bits;

/** \\brief  SMU Access Enable Register 1 */
typedef struct _Ifx_SMU_ACCEN1_Bits
{
    unsigned int reserved_0:32;             /**< \brief \internal Reserved */
} Ifx_SMU_ACCEN1_Bits;

/** \\brief  Alarm Status Register */
typedef struct _Ifx_SMU_AD_Bits
{
    unsigned int DF0:1;                     /**< \brief [0:0] Debug flag for alarm 0 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF1:1;                     /**< \brief [1:1] Debug flag for alarm 1 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF2:1;                     /**< \brief [2:2] Debug flag for alarm 2 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF3:1;                     /**< \brief [3:3] Debug flag for alarm 3 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF4:1;                     /**< \brief [4:4] Debug flag for alarm 4 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF5:1;                     /**< \brief [5:5] Debug flag for alarm 5 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF6:1;                     /**< \brief [6:6] Debug flag for alarm 6 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF7:1;                     /**< \brief [7:7] Debug flag for alarm 7 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF8:1;                     /**< \brief [8:8] Debug flag for alarm 8 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF9:1;                     /**< \brief [9:9] Debug flag for alarm 9 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF10:1;                    /**< \brief [10:10] Debug flag for alarm 10 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF11:1;                    /**< \brief [11:11] Debug flag for alarm 11 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF12:1;                    /**< \brief [12:12] Debug flag for alarm 12 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF13:1;                    /**< \brief [13:13] Debug flag for alarm 13 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF14:1;                    /**< \brief [14:14] Debug flag for alarm 14 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF15:1;                    /**< \brief [15:15] Debug flag for alarm 15 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF16:1;                    /**< \brief [16:16] Debug flag for alarm 16 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF17:1;                    /**< \brief [17:17] Debug flag for alarm 17 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF18:1;                    /**< \brief [18:18] Debug flag for alarm 18 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF19:1;                    /**< \brief [19:19] Debug flag for alarm 19 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF20:1;                    /**< \brief [20:20] Debug flag for alarm 20 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF21:1;                    /**< \brief [21:21] Debug flag for alarm 21 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF22:1;                    /**< \brief [22:22] Debug flag for alarm 22 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF23:1;                    /**< \brief [23:23] Debug flag for alarm 23 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF24:1;                    /**< \brief [24:24] Debug flag for alarm 24 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF25:1;                    /**< \brief [25:25] Debug flag for alarm 25 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF26:1;                    /**< \brief [26:26] Debug flag for alarm 26 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF27:1;                    /**< \brief [27:27] Debug flag for alarm 27 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF28:1;                    /**< \brief [28:28] Debug flag for alarm 28 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF29:1;                    /**< \brief [29:29] Debug flag for alarm 29 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF30:1;                    /**< \brief [30:30] Debug flag for alarm 30 belonging to alarm group x (x=0-6). (rh) */
    unsigned int DF31:1;                    /**< \brief [31:31] Debug flag for alarm 31 belonging to alarm group x (x=0-6). (rh) */
} Ifx_SMU_AD_Bits;

/** \\brief  Alarm and Fault Counter */
typedef struct _Ifx_SMU_AFCNT_Bits
{
    unsigned int FCNT:4;                    /**< \brief [3:0] Fault Counter. (rh) */
    unsigned int reserved_4:4;              /**< \brief \internal Reserved */
    unsigned int ACNT:8;                    /**< \brief [15:8] Alarm Counter. (rh) */
    unsigned int reserved_16:14;            /**< \brief \internal Reserved */
    unsigned int FCO:1;                     /**< \brief [30:30] Fault Counter Overflow. (rh) */
    unsigned int ACO:1;                     /**< \brief [31:31] Alarm Counter Overflow. (rh) */
} Ifx_SMU_AFCNT_Bits;

/** \\brief  Alarm Status Register */
typedef struct _Ifx_SMU_AG_Bits
{
    unsigned int SF0:1;                     /**< \brief [0:0] Status flag for alarm 0 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF1:1;                     /**< \brief [1:1] Status flag for alarm 1 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF2:1;                     /**< \brief [2:2] Status flag for alarm 2 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF3:1;                     /**< \brief [3:3] Status flag for alarm 3 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF4:1;                     /**< \brief [4:4] Status flag for alarm 4 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF5:1;                     /**< \brief [5:5] Status flag for alarm 5 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF6:1;                     /**< \brief [6:6] Status flag for alarm 6 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF7:1;                     /**< \brief [7:7] Status flag for alarm 7 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF8:1;                     /**< \brief [8:8] Status flag for alarm 8 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF9:1;                     /**< \brief [9:9] Status flag for alarm 9 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF10:1;                    /**< \brief [10:10] Status flag for alarm 10 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF11:1;                    /**< \brief [11:11] Status flag for alarm 11 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF12:1;                    /**< \brief [12:12] Status flag for alarm 12 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF13:1;                    /**< \brief [13:13] Status flag for alarm 13 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF14:1;                    /**< \brief [14:14] Status flag for alarm 14 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF15:1;                    /**< \brief [15:15] Status flag for alarm 15 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF16:1;                    /**< \brief [16:16] Status flag for alarm 16 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF17:1;                    /**< \brief [17:17] Status flag for alarm 17 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF18:1;                    /**< \brief [18:18] Status flag for alarm 18 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF19:1;                    /**< \brief [19:19] Status flag for alarm 19 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF20:1;                    /**< \brief [20:20] Status flag for alarm 20 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF21:1;                    /**< \brief [21:21] Status flag for alarm 21 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF22:1;                    /**< \brief [22:22] Status flag for alarm 22 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF23:1;                    /**< \brief [23:23] Status flag for alarm 23 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF24:1;                    /**< \brief [24:24] Status flag for alarm 24 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF25:1;                    /**< \brief [25:25] Status flag for alarm 25 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF26:1;                    /**< \brief [26:26] Status flag for alarm 26 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF27:1;                    /**< \brief [27:27] Status flag for alarm 27 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF28:1;                    /**< \brief [28:28] Status flag for alarm 28 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF29:1;                    /**< \brief [29:29] Status flag for alarm 29 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF30:1;                    /**< \brief [30:30] Status flag for alarm 30 belonging to alarm group x (x=0-6). (rwh) */
    unsigned int SF31:1;                    /**< \brief [31:31] Status flag for alarm 31 belonging to alarm group x (x=0-6). (rwh) */
} Ifx_SMU_AG_Bits;

/** \\brief  Alarm Global Configuration */
typedef struct _Ifx_SMU_AGC_Bits
{
    unsigned int IGCS0:3;                   /**< \brief [2:0] Interrupt Generation Configuration Set 0 (rw) */
    unsigned int reserved_3:1;              /**< \brief \internal Reserved */
    unsigned int IGCS1:3;                   /**< \brief [6:4] Interrupt Generation Configuration Set 1 (rw) */
    unsigned int reserved_7:1;              /**< \brief \internal Reserved */
    unsigned int IGCS2:3;                   /**< \brief [10:8] Interrupt Generation Configuration Set 2 (rw) */
    unsigned int reserved_11:5;             /**< \brief \internal Reserved */
    unsigned int ICS:3;                     /**< \brief [18:16] Idle Configuration Set (rw) */
    unsigned int reserved_19:5;             /**< \brief \internal Reserved */
    unsigned int PES:5;                     /**< \brief [28:24] Port Emergency Stop (rw) */
    unsigned int EFRST:1;                   /**< \brief [29:29] Enable FAULT to RUN State Transition (rw) */
    unsigned int reserved_30:2;             /**< \brief \internal Reserved */
} Ifx_SMU_AGC_Bits;

/** \\brief  Alarm Configuration Register */
typedef struct _Ifx_SMU_AGCF_Bits
{
    unsigned int CF0:1;                     /**< \brief [0:0]  (rw) */
    unsigned int CF1:1;                     /**< \brief [1:1]  (rw) */
    unsigned int CF2:1;                     /**< \brief [2:2]  (rw) */
    unsigned int CF3:1;                     /**< \brief [3:3]  (rw) */
    unsigned int CF4:1;                     /**< \brief [4:4]  (rw) */
    unsigned int CF5:1;                     /**< \brief [5:5]  (rw) */
    unsigned int CF6:1;                     /**< \brief [6:6]  (rw) */
    unsigned int CF7:1;                     /**< \brief [7:7]  (rw) */
    unsigned int CF8:1;                     /**< \brief [8:8]  (rw) */
    unsigned int CF9:1;                     /**< \brief [9:9]  (rw) */
    unsigned int CF10:1;                    /**< \brief [10:10]  (rw) */
    unsigned int CF11:1;                    /**< \brief [11:11]  (rw) */
    unsigned int CF12:1;                    /**< \brief [12:12]  (rw) */
    unsigned int CF13:1;                    /**< \brief [13:13]  (rw) */
    unsigned int CF14:1;                    /**< \brief [14:14]  (rw) */
    unsigned int CF15:1;                    /**< \brief [15:15]  (rw) */
    unsigned int CF16:1;                    /**< \brief [16:16]  (rw) */
    unsigned int CF17:1;                    /**< \brief [17:17]  (rw) */
    unsigned int CF18:1;                    /**< \brief [18:18]  (rw) */
    unsigned int CF19:1;                    /**< \brief [19:19]  (rw) */
    unsigned int CF20:1;                    /**< \brief [20:20]  (rw) */
    unsigned int CF21:1;                    /**< \brief [21:21]  (rw) */
    unsigned int CF22:1;                    /**< \brief [22:22]  (rw) */
    unsigned int CF23:1;                    /**< \brief [23:23]  (rw) */
    unsigned int CF24:1;                    /**< \brief [24:24]  (rw) */
    unsigned int CF25:1;                    /**< \brief [25:25]  (rw) */
    unsigned int CF26:1;                    /**< \brief [26:26]  (rw) */
    unsigned int CF27:1;                    /**< \brief [27:27]  (rw) */
    unsigned int CF28:1;                    /**< \brief [28:28]  (rw) */
    unsigned int CF29:1;                    /**< \brief [29:29]  (rw) */
    unsigned int CF30:1;                    /**< \brief [30:30]  (rw) */
    unsigned int CF31:1;                    /**< \brief [31:31]  (rw) */
} Ifx_SMU_AGCF_Bits;

/** \\brief  FSP Configuration Register */
typedef struct _Ifx_SMU_AGFSP_Bits
{
    unsigned int FE0:1;                     /**< \brief [0:0]  (rw) */
    unsigned int FE1:1;                     /**< \brief [1:1]  (rw) */
    unsigned int FE2:1;                     /**< \brief [2:2]  (rw) */
    unsigned int FE3:1;                     /**< \brief [3:3]  (rw) */
    unsigned int FE4:1;                     /**< \brief [4:4]  (rw) */
    unsigned int FE5:1;                     /**< \brief [5:5]  (rw) */
    unsigned int FE6:1;                     /**< \brief [6:6]  (rw) */
    unsigned int FE7:1;                     /**< \brief [7:7]  (rw) */
    unsigned int FE8:1;                     /**< \brief [8:8]  (rw) */
    unsigned int FE9:1;                     /**< \brief [9:9]  (rw) */
    unsigned int FE10:1;                    /**< \brief [10:10]  (rw) */
    unsigned int FE11:1;                    /**< \brief [11:11]  (rw) */
    unsigned int FE12:1;                    /**< \brief [12:12]  (rw) */
    unsigned int FE13:1;                    /**< \brief [13:13]  (rw) */
    unsigned int FE14:1;                    /**< \brief [14:14]  (rw) */
    unsigned int FE15:1;                    /**< \brief [15:15]  (rw) */
    unsigned int FE16:1;                    /**< \brief [16:16]  (rw) */
    unsigned int FE17:1;                    /**< \brief [17:17]  (rw) */
    unsigned int FE18:1;                    /**< \brief [18:18]  (rw) */
    unsigned int FE19:1;                    /**< \brief [19:19]  (rw) */
    unsigned int FE20:1;                    /**< \brief [20:20]  (rw) */
    unsigned int FE21:1;                    /**< \brief [21:21]  (rw) */
    unsigned int FE22:1;                    /**< \brief [22:22]  (rw) */
    unsigned int FE23:1;                    /**< \brief [23:23]  (rw) */
    unsigned int FE24:1;                    /**< \brief [24:24]  (rw) */
    unsigned int FE25:1;                    /**< \brief [25:25]  (rw) */
    unsigned int FE26:1;                    /**< \brief [26:26]  (rw) */
    unsigned int FE27:1;                    /**< \brief [27:27]  (rw) */
    unsigned int FE28:1;                    /**< \brief [28:28]  (rw) */
    unsigned int FE29:1;                    /**< \brief [29:29]  (rw) */
    unsigned int FE30:1;                    /**< \brief [30:30]  (rw) */
    unsigned int FE31:1;                    /**< \brief [31:31]  (rw) */
} Ifx_SMU_AGFSP_Bits;

/** \\brief  Clock Control Register */
typedef struct _Ifx_SMU_CLC_Bits
{
    unsigned int DISR:1;                    /**< \brief [0:0] Module Disable Request Bit (rw) */
    unsigned int DISS:1;                    /**< \brief [1:1] Module Disable Status Bit (rh) */
    unsigned int FDIS:1;                    /**< \brief [2:2] Force Disable (rw) */
    unsigned int EDIS:1;                    /**< \brief [3:3] Sleep Mode Enable Control (rw) */
    unsigned int reserved_4:28;             /**< \brief \internal Reserved */
} Ifx_SMU_CLC_Bits;

/** \\brief  Command Register */
typedef struct _Ifx_SMU_CMD_Bits
{
    unsigned int CMD:4;                     /**< \brief [3:0] Implements the SMU Command Interface. (w) */
    unsigned int ARG:4;                     /**< \brief [7:4] Implements the SMU Command Interface. (w) */
    unsigned int reserved_8:24;             /**< \brief \internal Reserved */
} Ifx_SMU_CMD_Bits;

/** \\brief  Debug Register */
typedef struct _Ifx_SMU_DBG_Bits
{
    unsigned int SSM:2;                     /**< \brief [1:0] Running state of the SMU State Machine (rh) */
    unsigned int reserved_2:30;             /**< \brief \internal Reserved */
} Ifx_SMU_DBG_Bits;

/** \\brief  Fault Signaling Protocol */
typedef struct _Ifx_SMU_FSP_Bits
{
    unsigned int PRE1:3;                    /**< \brief [2:0] Prescaler1 (rw) */
    unsigned int PRE2:2;                    /**< \brief [4:3] Prescaler2 (rw) */
    unsigned int MODE:2;                    /**< \brief [6:5] Fault Signaling Protocol configuration (rw) */
    unsigned int PES:1;                     /**< \brief [7:7] Port Emergency Stop (PES) (rw) */
    unsigned int TFSP_LOW:14;               /**< \brief [21:8] Specifies the FSP fault state duration (r) */
    unsigned int TFSP_HIGH:10;              /**< \brief [31:22] Specifies the FSP fault state duration (rw) */
} Ifx_SMU_FSP_Bits;

/** \\brief  Module Identification Register */
typedef struct _Ifx_SMU_ID_Bits
{
    unsigned int MODREV:8;                  /**< \brief [7:0] Module Revision Number (r) */
    unsigned int MODTYPE:8;                 /**< \brief [15:8] Module Type (r) */
    unsigned int MODNUMBER:16;              /**< \brief [31:16] Module Number Value (r) */
} Ifx_SMU_ID_Bits;

/** \\brief  Key Register */
typedef struct _Ifx_SMU_KEYS_Bits
{
    unsigned int CFGLCK:8;                  /**< \brief [7:0] Configuration Lock (rw) */
    unsigned int PERLCK:8;                  /**< \brief [15:8] Permanent Lock (rw) */
    unsigned int reserved_16:16;            /**< \brief \internal Reserved */
} Ifx_SMU_KEYS_Bits;

/** \\brief  SMU Reset Register 0 */
typedef struct _Ifx_SMU_KRST0_Bits
{
    unsigned int RST:1;                     /**< \brief [0:0] Kernel Reset (rwh) */
    unsigned int RSTSTAT:1;                 /**< \brief [1:1] Kernel Reset Status (rh) */
    unsigned int reserved_2:30;             /**< \brief \internal Reserved */
} Ifx_SMU_KRST0_Bits;

/** \\brief  SMU Reset Register 1 */
typedef struct _Ifx_SMU_KRST1_Bits
{
    unsigned int RST:1;                     /**< \brief [0:0] Kernel Reset (rwh) */
    unsigned int reserved_1:31;             /**< \brief \internal Reserved */
} Ifx_SMU_KRST1_Bits;

/** \\brief  SMU Reset Status Clear Register */
typedef struct _Ifx_SMU_KRSTCLR_Bits
{
    unsigned int CLR:1;                     /**< \brief [0:0] Kernel Reset Status Clear (w) */
    unsigned int reserved_1:31;             /**< \brief \internal Reserved */
} Ifx_SMU_KRSTCLR_Bits;

/** \\brief  OCDS Control and Status */
typedef struct _Ifx_SMU_OCS_Bits
{
    unsigned int TGS:2;                     /**< \brief [1:0] Trigger Set for OTGB0/1 (rw) */
    unsigned int TGB:1;                     /**< \brief [2:2] OTGB0/1 Bus Select (rw) */
    unsigned int TG_P:1;                    /**< \brief [3:3] TGS, TGB Write Protection (w) */
    unsigned int reserved_4:20;             /**< \brief \internal Reserved */
    unsigned int SUS:4;                     /**< \brief [27:24] OCDS Suspend Control (rw) */
    unsigned int SUS_P:1;                   /**< \brief [28:28] SUS Write Protection (w) */
    unsigned int SUSSTA:1;                  /**< \brief [29:29] Suspend State (rh) */
    unsigned int reserved_30:2;             /**< \brief \internal Reserved */
} Ifx_SMU_OCS_Bits;

/** \\brief  Port Control */
typedef struct _Ifx_SMU_PCTL_Bits
{
    unsigned int HWDIR:1;                   /**< \brief [0:0] Port Direction. (rw) */
    unsigned int HWEN:1;                    /**< \brief [1:1] Port Enable. (rw) */
    unsigned int reserved_2:5;              /**< \brief \internal Reserved */
    unsigned int PCS:1;                     /**< \brief [7:7] PAD Configuration Select (rw) */
    unsigned int reserved_8:8;              /**< \brief \internal Reserved */
    unsigned int PCFG:16;                   /**< \brief [31:16] PAD Configuration (rh) */
} Ifx_SMU_PCTL_Bits;

/** \\brief  Register Monitor Control */
typedef struct _Ifx_SMU_RMCTL_Bits
{
    unsigned int TE:1;                      /**< \brief [0:0] Test Enable. (rw) */
    unsigned int reserved_1:31;             /**< \brief \internal Reserved */
} Ifx_SMU_RMCTL_Bits;

/** \\brief  Register Monitor Error Flags */
typedef struct _Ifx_SMU_RMEF_Bits
{
    unsigned int EF0:1;                     /**< \brief [0:0] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF1:1;                     /**< \brief [1:1] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF2:1;                     /**< \brief [2:2] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF3:1;                     /**< \brief [3:3] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF4:1;                     /**< \brief [4:4] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF5:1;                     /**< \brief [5:5] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF6:1;                     /**< \brief [6:6] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF7:1;                     /**< \brief [7:7] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF8:1;                     /**< \brief [8:8] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF9:1;                     /**< \brief [9:9] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF10:1;                    /**< \brief [10:10] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF11:1;                    /**< \brief [11:11] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF12:1;                    /**< \brief [12:12] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF13:1;                    /**< \brief [13:13] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF14:1;                    /**< \brief [14:14] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF15:1;                    /**< \brief [15:15] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF16:1;                    /**< \brief [16:16] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF17:1;                    /**< \brief [17:17] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF18:1;                    /**< \brief [18:18] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF19:1;                    /**< \brief [19:19] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF20:1;                    /**< \brief [20:20] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF21:1;                    /**< \brief [21:21] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF22:1;                    /**< \brief [22:22] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF23:1;                    /**< \brief [23:23] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF24:1;                    /**< \brief [24:24] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF25:1;                    /**< \brief [25:25] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF26:1;                    /**< \brief [26:26] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF27:1;                    /**< \brief [27:27] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF28:1;                    /**< \brief [28:28] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF29:1;                    /**< \brief [29:29] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF30:1;                    /**< \brief [30:30] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int EF31:1;                    /**< \brief [31:31] Status flag related to the different instances of the register monitor safety mechanism. (rwh) */
} Ifx_SMU_RMEF_Bits;

/** \\brief  Register Monitor Self Test Status */
typedef struct _Ifx_SMU_RMSTS_Bits
{
    unsigned int STS0:1;                    /**< \brief [0:0] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS1:1;                    /**< \brief [1:1] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS2:1;                    /**< \brief [2:2] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS3:1;                    /**< \brief [3:3] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS4:1;                    /**< \brief [4:4] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS5:1;                    /**< \brief [5:5] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS6:1;                    /**< \brief [6:6] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS7:1;                    /**< \brief [7:7] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS8:1;                    /**< \brief [8:8] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS9:1;                    /**< \brief [9:9] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS10:1;                   /**< \brief [10:10] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS11:1;                   /**< \brief [11:11] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS12:1;                   /**< \brief [12:12] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS13:1;                   /**< \brief [13:13] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS14:1;                   /**< \brief [14:14] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS15:1;                   /**< \brief [15:15] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS16:1;                   /**< \brief [16:16] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS17:1;                   /**< \brief [17:17] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS18:1;                   /**< \brief [18:18] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS19:1;                   /**< \brief [19:19] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS20:1;                   /**< \brief [20:20] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS21:1;                   /**< \brief [21:21] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS22:1;                   /**< \brief [22:22] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS23:1;                   /**< \brief [23:23] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS24:1;                   /**< \brief [24:24] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS25:1;                   /**< \brief [25:25] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS26:1;                   /**< \brief [26:26] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS27:1;                   /**< \brief [27:27] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS28:1;                   /**< \brief [28:28] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS29:1;                   /**< \brief [29:29] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS30:1;                   /**< \brief [30:30] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
    unsigned int STS31:1;                   /**< \brief [31:31] Ready flag related to the different instances of the register monitor safety mechanism. (rwh) */
} Ifx_SMU_RMSTS_Bits;

/** \\brief  Recovery Timer Alarm Configuration */
typedef struct _Ifx_SMU_RTAC0_Bits
{
    unsigned int GID0:3;                    /**< \brief [2:0] Group Index 0. (rw) */
    unsigned int ALID0:5;                   /**< \brief [7:3] Alarm Identifier 0. (rw) */
    unsigned int GID1:3;                    /**< \brief [10:8] Group Index 1. (rw) */
    unsigned int ALID1:5;                   /**< \brief [15:11] Alarm Identifier 1. (rw) */
    unsigned int GID2:3;                    /**< \brief [18:16] Group Index 2. (rw) */
    unsigned int ALID2:5;                   /**< \brief [23:19] Alarm Identifier 2. (rw) */
    unsigned int GID3:3;                    /**< \brief [26:24] Group Index 3. (rw) */
    unsigned int ALID3:5;                   /**< \brief [31:27] Alarm Identifier 3. (rw) */
} Ifx_SMU_RTAC0_Bits;

/** \\brief  Recovery Timer Alarm Configuration */
typedef struct _Ifx_SMU_RTAC1_Bits
{
    unsigned int GID0:3;                    /**< \brief [2:0] Group Index 0. (rw) */
    unsigned int ALID0:5;                   /**< \brief [7:3] Alarm Identifier 0. (rw) */
    unsigned int GID1:3;                    /**< \brief [10:8] Group Index 1. (rw) */
    unsigned int ALID1:5;                   /**< \brief [15:11] Alarm Identifier 1. (rw) */
    unsigned int GID2:3;                    /**< \brief [18:16] Group Index 2. (rw) */
    unsigned int ALID2:5;                   /**< \brief [23:19] Alarm Identifier 2. (rw) */
    unsigned int GID3:3;                    /**< \brief [26:24] Group Index 3. (rw) */
    unsigned int ALID3:5;                   /**< \brief [31:27] Alarm Identifier 3. (rw) */
} Ifx_SMU_RTAC1_Bits;

/** \\brief  Fault Signaling Protocol */
typedef struct _Ifx_SMU_RTC_Bits
{
    unsigned int RT0E:1;                    /**< \brief [0:0] RT0 Enable Bit (rw) */
    unsigned int RT1E:1;                    /**< \brief [1:1] RT1 Enable Bit (rw) */
    unsigned int reserved_2:6;              /**< \brief \internal Reserved */
    unsigned int RTD:24;                    /**< \brief [31:8] Recovery Timer Duration (rw) */
} Ifx_SMU_RTC_Bits;

/** \\brief  Status Register */
typedef struct _Ifx_SMU_STS_Bits
{
    unsigned int CMD:4;                     /**< \brief [3:0] Last command received (rwh) */
    unsigned int ARG:4;                     /**< \brief [7:4] Last command argument received (rwh) */
    unsigned int RES:1;                     /**< \brief [8:8] Result of last received command (rwh) */
    unsigned int ASCE:1;                    /**< \brief [9:9] Alarm Status Clear Enable (rwh) */
    unsigned int FSP:2;                     /**< \brief [11:10] Fault Signaling Protocol status (rh) */
    unsigned int FSTS:1;                    /**< \brief [12:12] Fault State Timing Status (rwh) */
    unsigned int reserved_13:3;             /**< \brief \internal Reserved */
    unsigned int RTS0:1;                    /**< \brief [16:16] Recovery Timer 0 Status (rwh) */
    unsigned int RTME0:1;                   /**< \brief [17:17] Recovery Timer 0 Missed Event (rwh) */
    unsigned int RTS1:1;                    /**< \brief [18:18] Recovery Timer 1 Status (rwh) */
    unsigned int RTME1:1;                   /**< \brief [19:19] Recovery Timer 1 Missed Event (rwh) */
    unsigned int reserved_20:12;            /**< \brief \internal Reserved */
} Ifx_SMU_STS_Bits;
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Smu_union
 * \{  */

/** \\brief  SMU Access Enable Register 0 */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_ACCEN0_Bits B;
} Ifx_SMU_ACCEN0;

/** \\brief  SMU Access Enable Register 1 */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_ACCEN1_Bits B;
} Ifx_SMU_ACCEN1;

/** \\brief  Alarm Status Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AD_Bits B;
} Ifx_SMU_AD;

/** \\brief  Alarm and Fault Counter */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AFCNT_Bits B;
} Ifx_SMU_AFCNT;

/** \\brief  Alarm Status Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AG_Bits B;
} Ifx_SMU_AG;

/** \\brief  Alarm Global Configuration */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AGC_Bits B;
} Ifx_SMU_AGC;

/** \\brief  Alarm Configuration Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AGCF_Bits B;
} Ifx_SMU_AGCF;

/** \\brief  FSP Configuration Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_AGFSP_Bits B;
} Ifx_SMU_AGFSP;

/** \\brief  Clock Control Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_CLC_Bits B;
} Ifx_SMU_CLC;

/** \\brief  Command Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_CMD_Bits B;
} Ifx_SMU_CMD;

/** \\brief  Debug Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_DBG_Bits B;
} Ifx_SMU_DBG;

/** \\brief  Fault Signaling Protocol */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_FSP_Bits B;
} Ifx_SMU_FSP;

/** \\brief  Module Identification Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_ID_Bits B;
} Ifx_SMU_ID;

/** \\brief  Key Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_KEYS_Bits B;
} Ifx_SMU_KEYS;

/** \\brief  SMU Reset Register 0 */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_KRST0_Bits B;
} Ifx_SMU_KRST0;

/** \\brief  SMU Reset Register 1 */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_KRST1_Bits B;
} Ifx_SMU_KRST1;

/** \\brief  SMU Reset Status Clear Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_KRSTCLR_Bits B;
} Ifx_SMU_KRSTCLR;

/** \\brief  OCDS Control and Status */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_OCS_Bits B;
} Ifx_SMU_OCS;

/** \\brief  Port Control */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_PCTL_Bits B;
} Ifx_SMU_PCTL;

/** \\brief  Register Monitor Control */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RMCTL_Bits B;
} Ifx_SMU_RMCTL;

/** \\brief  Register Monitor Error Flags */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RMEF_Bits B;
} Ifx_SMU_RMEF;

/** \\brief  Register Monitor Self Test Status */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RMSTS_Bits B;
} Ifx_SMU_RMSTS;

/** \\brief  Recovery Timer Alarm Configuration */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RTAC0_Bits B;
} Ifx_SMU_RTAC0;

/** \\brief  Recovery Timer Alarm Configuration */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RTAC1_Bits B;
} Ifx_SMU_RTAC1;

/** \\brief  Fault Signaling Protocol */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_RTC_Bits B;
} Ifx_SMU_RTC;

/** \\brief  Status Register */
typedef union
{
    /** \brief Unsigned access */
    unsigned int U;
    /** \brief Signed access */
    signed int I;
    /** \brief Bitfield access */
    Ifx_SMU_STS_Bits B;
} Ifx_SMU_STS;
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Smu_struct
 * \{  */
/******************************************************************************/
/** \name Object L0
 * \{  */

/** \\brief  SMU object */
typedef volatile struct _Ifx_SMU
{
    Ifx_SMU_CLC CLC;                        /**< \brief 0, Clock Control Register */
    unsigned char reserved_4[4];            /**< \brief 4, \internal Reserved */
    Ifx_SMU_ID ID;                          /**< \brief 8, Module Identification Register */
    unsigned char reserved_C[20];           /**< \brief C, \internal Reserved */
    Ifx_SMU_CMD CMD;                        /**< \brief 20, Command Register */
    Ifx_SMU_STS STS;                        /**< \brief 24, Status Register */
    Ifx_SMU_FSP FSP;                        /**< \brief 28, Fault Signaling Protocol */
    Ifx_SMU_AGC AGC;                        /**< \brief 2C, Alarm Global Configuration */
    Ifx_SMU_RTC RTC;                        /**< \brief 30, Fault Signaling Protocol */
    Ifx_SMU_KEYS KEYS;                      /**< \brief 34, Key Register */
    Ifx_SMU_DBG DBG;                        /**< \brief 38, Debug Register */
    Ifx_SMU_PCTL PCTL;                      /**< \brief 3C, Port Control */
    Ifx_SMU_AFCNT AFCNT;                    /**< \brief 40, Alarm and Fault Counter */
    unsigned char reserved_44[28];          /**< \brief 44, \internal Reserved */
    Ifx_SMU_RTAC0 RTAC0;                    /**< \brief 60, Recovery Timer Alarm Configuration */
    Ifx_SMU_RTAC1 RTAC1;                    /**< \brief 64, Recovery Timer Alarm Configuration */
    unsigned char reserved_68[152];         /**< \brief 68, \internal Reserved */
    Ifx_SMU_AGCF AGCF[7][3];                /**< \brief 100, Alarm Configuration Register */
    unsigned char reserved_154[44];         /**< \brief 154, \internal Reserved */
    Ifx_SMU_AGFSP AGFSP[7];                 /**< \brief 180, FSP Configuration Register */
    unsigned char reserved_19C[36];         /**< \brief 19C, \internal Reserved */
    Ifx_SMU_AG AG[7];                       /**< \brief 1C0, Alarm Status Register */
    unsigned char reserved_1DC[36];         /**< \brief 1DC, \internal Reserved */
    Ifx_SMU_AD AD[7];                       /**< \brief 200, Alarm Status Register */
    unsigned char reserved_21C[228];        /**< \brief 21C, \internal Reserved */
    Ifx_SMU_RMCTL RMCTL;                    /**< \brief 300, Register Monitor Control */
    Ifx_SMU_RMEF RMEF;                      /**< \brief 304, Register Monitor Error Flags */
    Ifx_SMU_RMSTS RMSTS;                    /**< \brief 308, Register Monitor Self Test Status */
    unsigned char reserved_30C[1244];       /**< \brief 30C, \internal Reserved */
    Ifx_SMU_OCS OCS;                        /**< \brief 7E8, OCDS Control and Status */
    Ifx_SMU_KRSTCLR KRSTCLR;                /**< \brief 7EC, SMU Reset Status Clear Register */
    Ifx_SMU_KRST1 KRST1;                    /**< \brief 7F0, SMU Reset Register 1 */
    Ifx_SMU_KRST0 KRST0;                    /**< \brief 7F4, SMU Reset Register 0 */
    Ifx_SMU_ACCEN1 ACCEN1;                  /**< \brief 7F8, SMU Access Enable Register 1 */
    Ifx_SMU_ACCEN0 ACCEN0;                  /**< \brief 7FC, SMU Access Enable Register 0 */
} Ifx_SMU;
/** \}  */
/******************************************************************************/
/** \}  */
/******************************************************************************/
/******************************************************************************/
#endif /* IFXSMU_REGDEF_H */
