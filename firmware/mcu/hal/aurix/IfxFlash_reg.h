/**
 * \file IfxFlash_reg.h
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
 * \defgroup IfxLld_Flash_Cfg Flash address
 * \ingroup IfxLld_Flash
 * 
 * \defgroup IfxLld_Flash_Cfg_BaseAddress Base address
 * \ingroup IfxLld_Flash_Cfg
 * 
 * \defgroup IfxLld_Flash_Cfg_Flash0 2-FLASH0
 * \ingroup IfxLld_Flash_Cfg
 * 
 */
#ifndef IFXFLASH_REG_H
#define IFXFLASH_REG_H 1
/******************************************************************************/
#include "IfxFlash_regdef.h"
/******************************************************************************/
/** \addtogroup IfxLld_Flash_Cfg_BaseAddress
 * \{  */

/** \\brief  FLASH object. */
#define MODULE_FLASH0 /*lint --e(923)*/ ((*(Ifx_FLASH*)0xF8001000u))
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Flash_Cfg_Flash0
 * \{  */

/** \\brief  13FC, Access Enable Register 0 */
#define FLASH0_ACCEN0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_ACCEN0*)0xF80023FCu)

/** \\brief  13F8, Access Enable Register 1 */
#define FLASH0_ACCEN1 /*lint --e(923)*/ (*(volatile Ifx_FLASH_ACCEN1*)0xF80023F8u)

/** \\brief  10B4, CBAB Configuration */
#define FLASH0_CBAB0_CFG /*lint --e(923)*/ (*(volatile Ifx_FLASH_CBAB_CFG*)0xF80020B4u)

/** Alias (User Manual Name) for FLASH0_CBAB0_CFG.
* To use register names with standard convension, please use FLASH0_CBAB0_CFG.
*/
#define	FLASH0_CBABCFG0	(FLASH0_CBAB0_CFG)

/** \\brief  10B8, CBAB Status */
#define FLASH0_CBAB0_STAT /*lint --e(923)*/ (*(volatile Ifx_FLASH_CBAB_STAT*)0xF80020B8u)

/** Alias (User Manual Name) for FLASH0_CBAB0_STAT.
* To use register names with standard convension, please use FLASH0_CBAB0_STAT.
*/
#define	FLASH0_CBABSTAT0	(FLASH0_CBAB0_STAT)

/** \\brief  10BC, CBAB FIFO TOP Entry */
#define FLASH0_CBAB0_TOP /*lint --e(923)*/ (*(volatile Ifx_FLASH_CBAB_TOP*)0xF80020BCu)

/** Alias (User Manual Name) for FLASH0_CBAB0_TOP.
* To use register names with standard convension, please use FLASH0_CBAB0_TOP.
*/
#define	FLASH0_CBABTOP0	(FLASH0_CBAB0_TOP)

/** \\brief  0, FSI Communication Register 0 */
#define FLASH0_COMM0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_COMM0*)0xF8001000u)

/** \\brief  4, FSI Communication Register 1 */
#define FLASH0_COMM1 /*lint --e(923)*/ (*(volatile Ifx_FLASH_COMM1*)0xF8001004u)

/** \\brief  8, FSI Communication Register 2 */
#define FLASH0_COMM2 /*lint --e(923)*/ (*(volatile Ifx_FLASH_COMM2*)0xF8001008u)

/** \\brief  10A4, ECC Read Register DF */
#define FLASH0_ECCRD /*lint --e(923)*/ (*(volatile Ifx_FLASH_ECCRD*)0xF80020A4u)

/** \\brief  1094, ECC Read Register for ports */
#define FLASH0_ECCRP0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_ECCRP*)0xF8002094u)

/** \\brief  1090, ECC Write Register */
#define FLASH0_ECCW /*lint --e(923)*/ (*(volatile Ifx_FLASH_ECCW*)0xF8002090u)

/** \\brief  1014, Flash Configuration Register */
#define FLASH0_FCON /*lint --e(923)*/ (*(volatile Ifx_FLASH_FCON*)0xF8002014u)

/** \\brief  101C, Flash Protection Control and Status Register */
#define FLASH0_FPRO /*lint --e(923)*/ (*(volatile Ifx_FLASH_FPRO*)0xF800201Cu)

/** \\brief  1010, Flash Status Register */
#define FLASH0_FSR /*lint --e(923)*/ (*(volatile Ifx_FLASH_FSR*)0xF8002010u)

/** \\brief  1204, HSM Flash Configuration Register */
#define FLASH0_HSMFCON /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMFCON*)0xF8002204u)

/** \\brief  1200, Flash Status Register */
#define FLASH0_HSMFSR /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMFSR*)0xF8002200u)

/** \\brief  1208, Margin Control Register HSM DFlash */
#define FLASH0_HSMMARD /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMMARD*)0xF8002208u)

/** \\brief  1218, HSM Requested Read Address Register */
#define FLASH0_HSMRRAD /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMRRAD*)0xF8002218u)

/** \\brief  120C, Requested Read Control Register HSM */
#define FLASH0_HSMRRCT /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMRRCT*)0xF800220Cu)

/** \\brief  1210, HSM Requested Read Data Register 0 */
#define FLASH0_HSMRRD0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMRRD0*)0xF8002210u)

/** \\brief  1214, HSM Requested Read Data Register 1 */
#define FLASH0_HSMRRD1 /*lint --e(923)*/ (*(volatile Ifx_FLASH_HSMRRD1*)0xF8002214u)

/** \\brief  1008, Flash Module Identification Register */
#define FLASH0_ID /*lint --e(923)*/ (*(volatile Ifx_FLASH_ID*)0xF8002008u)

/** \\brief  10AC, Margin Control Register DFlash */
#define FLASH0_MARD /*lint --e(923)*/ (*(volatile Ifx_FLASH_MARD*)0xF80020ACu)

/** \\brief  10A8, Margin Control Register PFlash */
#define FLASH0_MARP /*lint --e(923)*/ (*(volatile Ifx_FLASH_MARP*)0xF80020A8u)

/** \\brief  1030, DFlash Protection Configuration */
#define FLASH0_PROCOND /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCOND*)0xF8002030u)

/** \\brief  1058, Debug Interface Protection Configuration */
#define FLASH0_PROCONDBG /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONDBG*)0xF8002058u)

/** \\brief  105C, HSM Interface Configuration */
#define FLASH0_PROCONHSM /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONHSM*)0xF800205Cu)

/** \\brief  1034, HSM Code Flash OTP Protection Configuration */
#define FLASH0_PROCONHSMCOTP /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONHSMCOTP*)0xF8002034u)

/** \\brief  1038, OTP Protection Configuration for ports */
#define FLASH0_PROCONOTP0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONOTP*)0xF8002038u)

/** \\brief  1020, PFlash Protection Configuration for ports */
#define FLASH0_PROCONP0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONP*)0xF8002020u)

/** \\brief  1048, Write-Once Protection Configuration for ports */
#define FLASH0_PROCONWOP0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_PROCONWOP*)0xF8002048u)

/** \\brief  1060, Read Buffer Cfg 0 */
#define FLASH0_RDBCFG0_CFG0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_RDB_CFG0*)0xF8002060u)

/** Alias (User Manual Name) for FLASH0_RDBCFG0_CFG0.
* To use register names with standard convension, please use FLASH0_RDBCFG0_CFG0.
*/
#define	FLASH0_RDBCFG00	(FLASH0_RDBCFG0_CFG0)

/** \\brief  1064, Read Buffer Cfg 1 */
#define FLASH0_RDBCFG0_CFG1 /*lint --e(923)*/ (*(volatile Ifx_FLASH_RDB_CFG1*)0xF8002064u)

/** Alias (User Manual Name) for FLASH0_RDBCFG0_CFG1.
* To use register names with standard convension, please use FLASH0_RDBCFG0_CFG1.
*/
#define	FLASH0_RDBCFG01	(FLASH0_RDBCFG0_CFG1)

/** \\brief  1068, Read Buffer Cfg 2 */
#define FLASH0_RDBCFG0_CFG2 /*lint --e(923)*/ (*(volatile Ifx_FLASH_RDB_CFG2*)0xF8002068u)

/** Alias (User Manual Name) for FLASH0_RDBCFG0_CFG2.
* To use register names with standard convension, please use FLASH0_RDBCFG0_CFG2.
*/
#define	FLASH0_RDBCFG02	(FLASH0_RDBCFG0_CFG2)

/** \\brief  114C, Requested Read Address Register */
#define FLASH0_RRAD /*lint --e(923)*/ (*(volatile Ifx_FLASH_RRAD*)0xF800214Cu)

/** \\brief  1140, Requested Read Control Register */
#define FLASH0_RRCT /*lint --e(923)*/ (*(volatile Ifx_FLASH_RRCT*)0xF8002140u)

/** \\brief  1144, Requested Read Data Register 0 */
#define FLASH0_RRD0 /*lint --e(923)*/ (*(volatile Ifx_FLASH_RRD0*)0xF8002144u)

/** \\brief  1148, Requested Read Data Register 1 */
#define FLASH0_RRD1 /*lint --e(923)*/ (*(volatile Ifx_FLASH_RRD1*)0xF8002148u)

/** \\brief  10E4, UBAB Configuration */
#define FLASH0_UBAB0_CFG /*lint --e(923)*/ (*(volatile Ifx_FLASH_UBAB_CFG*)0xF80020E4u)

/** Alias (User Manual Name) for FLASH0_UBAB0_CFG.
* To use register names with standard convension, please use FLASH0_UBAB0_CFG.
*/
#define	FLASH0_UBABCFG0	(FLASH0_UBAB0_CFG)

/** \\brief  10E8, UBAB Status */
#define FLASH0_UBAB0_STAT /*lint --e(923)*/ (*(volatile Ifx_FLASH_UBAB_STAT*)0xF80020E8u)

/** Alias (User Manual Name) for FLASH0_UBAB0_STAT.
* To use register names with standard convension, please use FLASH0_UBAB0_STAT.
*/
#define	FLASH0_UBABSTAT0	(FLASH0_UBAB0_STAT)

/** \\brief  10EC, UBAB FIFO TOP Entry */
#define FLASH0_UBAB0_TOP /*lint --e(923)*/ (*(volatile Ifx_FLASH_UBAB_TOP*)0xF80020ECu)

/** Alias (User Manual Name) for FLASH0_UBAB0_TOP.
* To use register names with standard convension, please use FLASH0_UBAB0_TOP.
*/
#define	FLASH0_UBABTOP0	(FLASH0_UBAB0_TOP)
/** \}  */
/******************************************************************************/
/******************************************************************************/
#endif /* IFXFLASH_REG_H */
