/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_dtc.h
* Version      : 1.2.3
* Device(s)    : R5F565NEHxFP
* Description  : General header file for DTC peripheral.
* Creation Date: 2019-04-30
***********************************************************************************************************************/

#ifndef DTC_H
#define DTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    DTC Mode Register A (MRA)
*/
/* Transfer Source Address Addressing Mode (SM[1:0]) */
#define _00_DTC_SRC_ADDRESS_FIXED           (0x00U) /* Address in the SAR register is fixed */
#define _08_DTC_SRC_ADDRESS_INCREMENTED     (0x08U) /* SAR value is incremented after data transfer */
#define _0C_DTC_SRC_ADDRESS_DECREMENTED     (0x0CU) /* SAR value is decremented after data transfer */
/* DTC Data Transfer Size (SZ[1:0]) */
#define _00_DTC_TRANSFER_SIZE_8BIT          (0x00U) /* 8-bit (byte) transfer */
#define _10_DTC_TRANSFER_SIZE_16BIT         (0x10U) /* 16-bit (word) transfer */
#define _20_DTC_TRANSFER_SIZE_32BIT         (0x20U) /* 32-bit (long) transfer */
/* DTC Transfer Mode Select (MD[1:0]) */
#define _00_DTC_TRANSFER_MODE_NORMAL        (0x00U) /* Normal transfer mode */
#define _40_DTC_TRANSFER_MODE_REPEAT        (0x40U) /* Repeat transfer mode */
#define _80_DTC_TRANSFER_MODE_BLOCK         (0x80U) /* Block transfer mode */

/*
    DTC Mode Register B (MRB)
*/
/* Transfer Destination Address Addressing Mode (DM[1:0]) */
#define _00_DTC_DST_ADDRESS_FIXED           (0x00U) /* Address in the DAR register is fixed */
#define _08_DTC_DST_ADDRESS_INCREMENTED     (0x08U) /* DAR value is incremented after data transfer */
#define _0C_DTC_DST_ADDRESS_DECREMENTED     (0x0CU) /* DAR value is decremented after data transfer */
/* DTC Transfer Mode Select (DTS) */
#define _00_DTC_REPEAT_DST_SIDE             (0x00U) /* Transfer destination side is repeat area or block area */
#define _10_DTC_REPEAT_SRC_SIDE             (0x10U) /* Transfer source side is repeat area or block area */
/* DTC Interrupt Select (DISEL) */
#define _00_DTC_INTERRUPT_COMPLETED         (0x00U) /* Generate CPU interrupt when specified data transfer completes */
#define _20_DTC_INTERRUPT_ON_TRANSFER       (0x20U) /* Generate CPU interrupt each time data transfer is performed */
/* DTC Chain Transfer Select (CHNS) */
#define _00_DTC_CHAIN_TRANSFER_CONTINUOUS   (0x00U) /* Chain transfer performed continuously */
#define _40_DTC_CHAIN_TRANSFER_ON_COUNTER   (0x40U) /* Perform transfer on counter change from 1 to 0 or 1 to CRAH */
/* DTC Chain Transfer Enable (CHNE) */
#define _00_DTC_CHAIN_TRANSFER_DISABLED     (0x00U) /* Chain transfer is disabled */
#define _80_DTC_CHAIN_TRANSFER_ENABLED      (0x80U) /* Chain transfer is enabled */

/*
    DTC Control Register (DTCCR)
*/
/* DTC Transfer Data Read Skip Enable (RRS) */
#define _08_DTC_TRANSFER_READSKIP_DISABLE   (0x08U) /* Transfer data read is not skipped */
#define _18_DTC_TRANSFER_READSKIP_ENABLE    (0x18U) /* Transfer data read is skipped when vector numbers match */

/*
    DTC Address Mode Register (DTCADMOD)
*/
/* Short-Address Mode (SHORT) */
#define _00_DTC_ADDRESS_MODE_FULL           (0x00U) /* Full-address mode */
#define _01_DTC_ADDRESS_MODE_SHORT          (0x01U) /* Short-address mode */

/*
    DTC Module Start Register (DTCST)
*/
/* DTC Module Start (DTCST) */
#define _00_DTC_MODULE_STOP                 (0x00U) /* DTC module stop */
#define _01_DTC_MODULE_START                (0x01U) /* DTC module start */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
#if 0
typedef struct
{
    uint32_t mra_mrb;
    uint32_t sar;
    uint32_t dar;
    uint32_t cra_crb;
} st_dtc_data_t;
#endif

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

