/**
 * \file IfxDma_reg.h
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
 * \defgroup IfxLld_Dma_Cfg Dma address
 * \ingroup IfxLld_Dma
 * 
 * \defgroup IfxLld_Dma_Cfg_BaseAddress Base address
 * \ingroup IfxLld_Dma_Cfg
 * 
 * \defgroup IfxLld_Dma_Cfg_Dma 2-DMA
 * \ingroup IfxLld_Dma_Cfg
 * 
 */
#ifndef IFXDMA_REG_H
#define IFXDMA_REG_H 1
/******************************************************************************/
#include "IfxDma_regdef.h"
/******************************************************************************/
/** \addtogroup IfxLld_Dma_Cfg_BaseAddress
 * \{  */

/** \\brief  DMA object */
#define MODULE_DMA /*lint --e(923)*/ ((*(Ifx_DMA*)0xF0010000u))
/** \}  */
/******************************************************************************/
/******************************************************************************/
/** \addtogroup IfxLld_Dma_Cfg_Dma
 * \{  */

/** \\brief  40, DMA Hardware Resource 0 Access Enable Register 0 */
#define DMA_ACCEN00 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN00*)0xF0010040u)

/** \\brief  44, DMA Hardware Resource 0 Access Enable Register 1 */
#define DMA_ACCEN01 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN01*)0xF0010044u)

/** \\brief  48, DMA Hardware Resource 1 Access Enable Register 0 */
#define DMA_ACCEN10 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN10*)0xF0010048u)

/** \\brief  4C, DMA Hardware Resource 1 Access Enable Register 1 */
#define DMA_ACCEN11 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN11*)0xF001004Cu)

/** \\brief  50, DMA Hardware Resource 2 Access Enable Register 0 */
#define DMA_ACCEN20 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN20*)0xF0010050u)

/** \\brief  54, DMA Hardware Resource 2 Access Enable Register 1 */
#define DMA_ACCEN21 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN21*)0xF0010054u)

/** \\brief  58, DMA Hardware Resource 3 Access Enable Register 0 */
#define DMA_ACCEN30 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN30*)0xF0010058u)

/** \\brief  5C, DMA Hardware Resource 3 Access Enable Register 1 */
#define DMA_ACCEN31 /*lint --e(923)*/ (*(volatile Ifx_DMA_ACCEN31*)0xF001005Cu)

/** \\brief  128, DMA Clear Error Register */
#define DMA_BLK0_CLRE /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_CLRE*)0xF0010128u)

/** Alias (User Manual Name) for DMA_BLK0_CLRE.
* To use register names with standard convension, please use DMA_BLK0_CLRE.
*/
#define	DMA_CLRE0	(DMA_BLK0_CLRE)

/** \\brief  120, DMA Enable Error Register */
#define DMA_BLK0_EER /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_EER*)0xF0010120u)

/** Alias (User Manual Name) for DMA_BLK0_EER.
* To use register names with standard convension, please use DMA_BLK0_EER.
*/
#define	DMA_EER0	(DMA_BLK0_EER)

/** \\brief  124, DMA Error Status Register */
#define DMA_BLK0_ERRSR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ERRSR*)0xF0010124u)

/** Alias (User Manual Name) for DMA_BLK0_ERRSR.
* To use register names with standard convension, please use DMA_BLK0_ERRSR.
*/
#define	DMA_ERRSR0	(DMA_BLK0_ERRSR)

/** \\brief  190, DMA Move Engine Channel Address and Interrupt Control Register */
#define DMA_BLK0_ME_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_ADICR*)0xF0010190u)

/** Alias (User Manual Name) for DMA_BLK0_ME_ADICR.
* To use register names with standard convension, please use DMA_BLK0_ME_ADICR.
*/
#define	DMA_ME0ADICR	(DMA_BLK0_ME_ADICR)

/** \\brief  194, DMA Move Engine Channel Control Register */
#define DMA_BLK0_ME_CHCR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_CHCR*)0xF0010194u)

/** Alias (User Manual Name) for DMA_BLK0_ME_CHCR.
* To use register names with standard convension, please use DMA_BLK0_ME_CHCR.
*/
#define	DMA_ME0CHCR	(DMA_BLK0_ME_CHCR)

/** \\brief  19C, DMA Move Engine Channel Status Register */
#define DMA_BLK0_ME_CHSR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_CHSR*)0xF001019Cu)

/** Alias (User Manual Name) for DMA_BLK0_ME_CHSR.
* To use register names with standard convension, please use DMA_BLK0_ME_CHSR.
*/
#define	DMA_ME0CHSR	(DMA_BLK0_ME_CHSR)

/** \\brief  18C, DMA Move Engine Channel Destination Address Register x */
#define DMA_BLK0_ME_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_DADR*)0xF001018Cu)

/** Alias (User Manual Name) for DMA_BLK0_ME_DADR.
* To use register names with standard convension, please use DMA_BLK0_ME_DADR.
*/
#define	DMA_ME0DADR	(DMA_BLK0_ME_DADR)

/** \\brief  140, DMA Move Engine Read Register 0 */
#define DMA_BLK0_ME_R0 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R0*)0xF0010140u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R0.
* To use register names with standard convension, please use DMA_BLK0_ME_R0.
*/
#define	DMA_ME00R	(DMA_BLK0_ME_R0)

/** \\brief  144, DMA Move Engine Read Register 1 */
#define DMA_BLK0_ME_R1 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R1*)0xF0010144u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R1.
* To use register names with standard convension, please use DMA_BLK0_ME_R1.
*/
#define	DMA_ME01R	(DMA_BLK0_ME_R1)

/** \\brief  148, DMA Move Engine Read Register 2 */
#define DMA_BLK0_ME_R2 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R2*)0xF0010148u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R2.
* To use register names with standard convension, please use DMA_BLK0_ME_R2.
*/
#define	DMA_ME02R	(DMA_BLK0_ME_R2)

/** \\brief  14C, DMA Move Engine Read Register 3 */
#define DMA_BLK0_ME_R3 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R3*)0xF001014Cu)

/** Alias (User Manual Name) for DMA_BLK0_ME_R3.
* To use register names with standard convension, please use DMA_BLK0_ME_R3.
*/
#define	DMA_ME03R	(DMA_BLK0_ME_R3)

/** \\brief  150, DMA Move Engine Read Register 4 */
#define DMA_BLK0_ME_R4 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R4*)0xF0010150u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R4.
* To use register names with standard convension, please use DMA_BLK0_ME_R4.
*/
#define	DMA_ME04R	(DMA_BLK0_ME_R4)

/** \\brief  154, DMA Move Engine Read Register 5 */
#define DMA_BLK0_ME_R5 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R5*)0xF0010154u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R5.
* To use register names with standard convension, please use DMA_BLK0_ME_R5.
*/
#define	DMA_ME05R	(DMA_BLK0_ME_R5)

/** \\brief  158, DMA Move Engine Read Register 6 */
#define DMA_BLK0_ME_R6 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R6*)0xF0010158u)

/** Alias (User Manual Name) for DMA_BLK0_ME_R6.
* To use register names with standard convension, please use DMA_BLK0_ME_R6.
*/
#define	DMA_ME06R	(DMA_BLK0_ME_R6)

/** \\brief  15C, DMA Move Engine Read Register 7 */
#define DMA_BLK0_ME_R7 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R7*)0xF001015Cu)

/** Alias (User Manual Name) for DMA_BLK0_ME_R7.
* To use register names with standard convension, please use DMA_BLK0_ME_R7.
*/
#define	DMA_ME07R	(DMA_BLK0_ME_R7)

/** \\brief  180, DMA Move Engine Channel Read Data CRC Register */
#define DMA_BLK0_ME_RDCRC /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_RDCRC*)0xF0010180u)

/** Alias (User Manual Name) for DMA_BLK0_ME_RDCRC.
* To use register names with standard convension, please use DMA_BLK0_ME_RDCRC.
*/
#define	DMA_ME0RDCRC	(DMA_BLK0_ME_RDCRC)

/** \\brief  188, DMA Move Engine Channel Source Address Register */
#define DMA_BLK0_ME_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SADR*)0xF0010188u)

/** Alias (User Manual Name) for DMA_BLK0_ME_SADR.
* To use register names with standard convension, please use DMA_BLK0_ME_SADR.
*/
#define	DMA_ME0SADR	(DMA_BLK0_ME_SADR)

/** \\brief  184, DMA Move Engine Channel Source and Destination Address CRC
 * Register */
#define DMA_BLK0_ME_SDCRC /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SDCRC*)0xF0010184u)

/** Alias (User Manual Name) for DMA_BLK0_ME_SDCRC.
* To use register names with standard convension, please use DMA_BLK0_ME_SDCRC.
*/
#define	DMA_ME0SDCRC	(DMA_BLK0_ME_SDCRC)

/** \\brief  198, DMA Move Engine Channel Shadow Address Register */
#define DMA_BLK0_ME_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SHADR*)0xF0010198u)

/** Alias (User Manual Name) for DMA_BLK0_ME_SHADR.
* To use register names with standard convension, please use DMA_BLK0_ME_SHADR.
*/
#define	DMA_ME0SHADR	(DMA_BLK0_ME_SHADR)

/** \\brief  130, DMA Move Engine Status Register */
#define DMA_BLK0_ME_SR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SR*)0xF0010130u)

/** Alias (User Manual Name) for DMA_BLK0_ME_SR.
* To use register names with standard convension, please use DMA_BLK0_ME_SR.
*/
#define	DMA_ME0SR	(DMA_BLK0_ME_SR)

/** \\brief  1128, DMA Clear Error Register */
#define DMA_BLK1_CLRE /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_CLRE*)0xF0011128u)

/** Alias (User Manual Name) for DMA_BLK1_CLRE.
* To use register names with standard convension, please use DMA_BLK1_CLRE.
*/
#define	DMA_CLRE1	(DMA_BLK1_CLRE)

/** \\brief  1120, DMA Enable Error Register */
#define DMA_BLK1_EER /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_EER*)0xF0011120u)

/** Alias (User Manual Name) for DMA_BLK1_EER.
* To use register names with standard convension, please use DMA_BLK1_EER.
*/
#define	DMA_EER1	(DMA_BLK1_EER)

/** \\brief  1124, DMA Error Status Register */
#define DMA_BLK1_ERRSR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ERRSR*)0xF0011124u)

/** Alias (User Manual Name) for DMA_BLK1_ERRSR.
* To use register names with standard convension, please use DMA_BLK1_ERRSR.
*/
#define	DMA_ERRSR1	(DMA_BLK1_ERRSR)

/** \\brief  1190, DMA Move Engine Channel Address and Interrupt Control
 * Register */
#define DMA_BLK1_ME_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_ADICR*)0xF0011190u)

/** Alias (User Manual Name) for DMA_BLK1_ME_ADICR.
* To use register names with standard convension, please use DMA_BLK1_ME_ADICR.
*/
#define	DMA_ME1ADICR	(DMA_BLK1_ME_ADICR)

/** \\brief  1194, DMA Move Engine Channel Control Register */
#define DMA_BLK1_ME_CHCR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_CHCR*)0xF0011194u)

/** Alias (User Manual Name) for DMA_BLK1_ME_CHCR.
* To use register names with standard convension, please use DMA_BLK1_ME_CHCR.
*/
#define	DMA_ME1CHCR	(DMA_BLK1_ME_CHCR)

/** \\brief  119C, DMA Move Engine Channel Status Register */
#define DMA_BLK1_ME_CHSR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_CHSR*)0xF001119Cu)

/** Alias (User Manual Name) for DMA_BLK1_ME_CHSR.
* To use register names with standard convension, please use DMA_BLK1_ME_CHSR.
*/
#define	DMA_ME1CHSR	(DMA_BLK1_ME_CHSR)

/** \\brief  118C, DMA Move Engine Channel Destination Address Register x */
#define DMA_BLK1_ME_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_DADR*)0xF001118Cu)

/** Alias (User Manual Name) for DMA_BLK1_ME_DADR.
* To use register names with standard convension, please use DMA_BLK1_ME_DADR.
*/
#define	DMA_ME1DADR	(DMA_BLK1_ME_DADR)

/** \\brief  1140, DMA Move Engine Read Register 0 */
#define DMA_BLK1_ME_R0 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R0*)0xF0011140u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R0.
* To use register names with standard convension, please use DMA_BLK1_ME_R0.
*/
#define	DMA_ME10R	(DMA_BLK1_ME_R0)

/** \\brief  1144, DMA Move Engine Read Register 1 */
#define DMA_BLK1_ME_R1 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R1*)0xF0011144u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R1.
* To use register names with standard convension, please use DMA_BLK1_ME_R1.
*/
#define	DMA_ME11R	(DMA_BLK1_ME_R1)

/** \\brief  1148, DMA Move Engine Read Register 2 */
#define DMA_BLK1_ME_R2 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R2*)0xF0011148u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R2.
* To use register names with standard convension, please use DMA_BLK1_ME_R2.
*/
#define	DMA_ME12R	(DMA_BLK1_ME_R2)

/** \\brief  114C, DMA Move Engine Read Register 3 */
#define DMA_BLK1_ME_R3 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R3*)0xF001114Cu)

/** Alias (User Manual Name) for DMA_BLK1_ME_R3.
* To use register names with standard convension, please use DMA_BLK1_ME_R3.
*/
#define	DMA_ME13R	(DMA_BLK1_ME_R3)

/** \\brief  1150, DMA Move Engine Read Register 4 */
#define DMA_BLK1_ME_R4 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R4*)0xF0011150u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R4.
* To use register names with standard convension, please use DMA_BLK1_ME_R4.
*/
#define	DMA_ME14R	(DMA_BLK1_ME_R4)

/** \\brief  1154, DMA Move Engine Read Register 5 */
#define DMA_BLK1_ME_R5 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R5*)0xF0011154u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R5.
* To use register names with standard convension, please use DMA_BLK1_ME_R5.
*/
#define	DMA_ME15R	(DMA_BLK1_ME_R5)

/** \\brief  1158, DMA Move Engine Read Register 6 */
#define DMA_BLK1_ME_R6 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R6*)0xF0011158u)

/** Alias (User Manual Name) for DMA_BLK1_ME_R6.
* To use register names with standard convension, please use DMA_BLK1_ME_R6.
*/
#define	DMA_ME16R	(DMA_BLK1_ME_R6)

/** \\brief  115C, DMA Move Engine Read Register 7 */
#define DMA_BLK1_ME_R7 /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_R7*)0xF001115Cu)

/** Alias (User Manual Name) for DMA_BLK1_ME_R7.
* To use register names with standard convension, please use DMA_BLK1_ME_R7.
*/
#define	DMA_ME17R	(DMA_BLK1_ME_R7)

/** \\brief  1180, DMA Move Engine Channel Read Data CRC Register */
#define DMA_BLK1_ME_RDCRC /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_RDCRC*)0xF0011180u)

/** Alias (User Manual Name) for DMA_BLK1_ME_RDCRC.
* To use register names with standard convension, please use DMA_BLK1_ME_RDCRC.
*/
#define	DMA_ME1RDCRC	(DMA_BLK1_ME_RDCRC)

/** \\brief  1188, DMA Move Engine Channel Source Address Register */
#define DMA_BLK1_ME_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SADR*)0xF0011188u)

/** Alias (User Manual Name) for DMA_BLK1_ME_SADR.
* To use register names with standard convension, please use DMA_BLK1_ME_SADR.
*/
#define	DMA_ME1SADR	(DMA_BLK1_ME_SADR)

/** \\brief  1184, DMA Move Engine Channel Source and Destination Address CRC
 * Register */
#define DMA_BLK1_ME_SDCRC /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SDCRC*)0xF0011184u)

/** Alias (User Manual Name) for DMA_BLK1_ME_SDCRC.
* To use register names with standard convension, please use DMA_BLK1_ME_SDCRC.
*/
#define	DMA_ME1SDCRC	(DMA_BLK1_ME_SDCRC)

/** \\brief  1198, DMA Move Engine Channel Shadow Address Register */
#define DMA_BLK1_ME_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SHADR*)0xF0011198u)

/** Alias (User Manual Name) for DMA_BLK1_ME_SHADR.
* To use register names with standard convension, please use DMA_BLK1_ME_SHADR.
*/
#define	DMA_ME1SHADR	(DMA_BLK1_ME_SHADR)

/** \\brief  1130, DMA Move Engine Status Register */
#define DMA_BLK1_ME_SR /*lint --e(923)*/ (*(volatile Ifx_DMA_BLK_ME_SR*)0xF0011130u)

/** Alias (User Manual Name) for DMA_BLK1_ME_SR.
* To use register names with standard convension, please use DMA_BLK1_ME_SR.
*/
#define	DMA_ME1SR	(DMA_BLK1_ME_SR)

/** \\brief  2010, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH0_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012010u)

/** Alias (User Manual Name) for DMA_CH0_ADICR.
* To use register names with standard convension, please use DMA_CH0_ADICR.
*/
#define	DMA_ADICR000	(DMA_CH0_ADICR)

/** \\brief  2014, DMA Channel Configuration Register */
#define DMA_CH0_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012014u)

/** Alias (User Manual Name) for DMA_CH0_CHCFGR.
* To use register names with standard convension, please use DMA_CH0_CHCFGR.
*/
#define	DMA_CHCFGR000	(DMA_CH0_CHCFGR)

/** \\brief  201C, DMARAM Channel Control and Status Register */
#define DMA_CH0_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001201Cu)

/** Alias (User Manual Name) for DMA_CH0_CHCSR.
* To use register names with standard convension, please use DMA_CH0_CHCSR.
*/
#define	DMA_CHCSR000	(DMA_CH0_CHCSR)

/** \\brief  200C, DMA Channel Destination Address Register x */
#define DMA_CH0_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001200Cu)

/** Alias (User Manual Name) for DMA_CH0_DADR.
* To use register names with standard convension, please use DMA_CH0_DADR.
*/
#define	DMA_DADR000	(DMA_CH0_DADR)

/** \\brief  2000, DMA Channel Read Data CRC Register */
#define DMA_CH0_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012000u)

/** Alias (User Manual Name) for DMA_CH0_RDCRCR.
* To use register names with standard convension, please use DMA_CH0_RDCRCR.
*/
#define	DMA_RDCRCR000	(DMA_CH0_RDCRCR)

/** \\brief  2008, DMA Channel Source Address Register */
#define DMA_CH0_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012008u)

/** Alias (User Manual Name) for DMA_CH0_SADR.
* To use register names with standard convension, please use DMA_CH0_SADR.
*/
#define	DMA_SADR000	(DMA_CH0_SADR)

/** \\brief  2004, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH0_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012004u)

/** Alias (User Manual Name) for DMA_CH0_SDCRCR.
* To use register names with standard convension, please use DMA_CH0_SDCRCR.
*/
#define	DMA_SDCRCR000	(DMA_CH0_SDCRCR)

/** \\brief  2018, DMA Channel Shadow Address Register */
#define DMA_CH0_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012018u)

/** Alias (User Manual Name) for DMA_CH0_SHADR.
* To use register names with standard convension, please use DMA_CH0_SHADR.
*/
#define	DMA_SHADR000	(DMA_CH0_SHADR)

/** \\brief  2150, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH10_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012150u)

/** Alias (User Manual Name) for DMA_CH10_ADICR.
* To use register names with standard convension, please use DMA_CH10_ADICR.
*/
#define	DMA_ADICR010	(DMA_CH10_ADICR)

/** \\brief  2154, DMA Channel Configuration Register */
#define DMA_CH10_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012154u)

/** Alias (User Manual Name) for DMA_CH10_CHCFGR.
* To use register names with standard convension, please use DMA_CH10_CHCFGR.
*/
#define	DMA_CHCFGR010	(DMA_CH10_CHCFGR)

/** \\brief  215C, DMARAM Channel Control and Status Register */
#define DMA_CH10_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001215Cu)

/** Alias (User Manual Name) for DMA_CH10_CHCSR.
* To use register names with standard convension, please use DMA_CH10_CHCSR.
*/
#define	DMA_CHCSR010	(DMA_CH10_CHCSR)

/** \\brief  214C, DMA Channel Destination Address Register x */
#define DMA_CH10_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001214Cu)

/** Alias (User Manual Name) for DMA_CH10_DADR.
* To use register names with standard convension, please use DMA_CH10_DADR.
*/
#define	DMA_DADR010	(DMA_CH10_DADR)

/** \\brief  2140, DMA Channel Read Data CRC Register */
#define DMA_CH10_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012140u)

/** Alias (User Manual Name) for DMA_CH10_RDCRCR.
* To use register names with standard convension, please use DMA_CH10_RDCRCR.
*/
#define	DMA_RDCRCR010	(DMA_CH10_RDCRCR)

/** \\brief  2148, DMA Channel Source Address Register */
#define DMA_CH10_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012148u)

/** Alias (User Manual Name) for DMA_CH10_SADR.
* To use register names with standard convension, please use DMA_CH10_SADR.
*/
#define	DMA_SADR010	(DMA_CH10_SADR)

/** \\brief  2144, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH10_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012144u)

/** Alias (User Manual Name) for DMA_CH10_SDCRCR.
* To use register names with standard convension, please use DMA_CH10_SDCRCR.
*/
#define	DMA_SDCRCR010	(DMA_CH10_SDCRCR)

/** \\brief  2158, DMA Channel Shadow Address Register */
#define DMA_CH10_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012158u)

/** Alias (User Manual Name) for DMA_CH10_SHADR.
* To use register names with standard convension, please use DMA_CH10_SHADR.
*/
#define	DMA_SHADR010	(DMA_CH10_SHADR)

/** \\brief  2170, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH11_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012170u)

/** Alias (User Manual Name) for DMA_CH11_ADICR.
* To use register names with standard convension, please use DMA_CH11_ADICR.
*/
#define	DMA_ADICR011	(DMA_CH11_ADICR)

/** \\brief  2174, DMA Channel Configuration Register */
#define DMA_CH11_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012174u)

/** Alias (User Manual Name) for DMA_CH11_CHCFGR.
* To use register names with standard convension, please use DMA_CH11_CHCFGR.
*/
#define	DMA_CHCFGR011	(DMA_CH11_CHCFGR)

/** \\brief  217C, DMARAM Channel Control and Status Register */
#define DMA_CH11_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001217Cu)

/** Alias (User Manual Name) for DMA_CH11_CHCSR.
* To use register names with standard convension, please use DMA_CH11_CHCSR.
*/
#define	DMA_CHCSR011	(DMA_CH11_CHCSR)

/** \\brief  216C, DMA Channel Destination Address Register x */
#define DMA_CH11_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001216Cu)

/** Alias (User Manual Name) for DMA_CH11_DADR.
* To use register names with standard convension, please use DMA_CH11_DADR.
*/
#define	DMA_DADR011	(DMA_CH11_DADR)

/** \\brief  2160, DMA Channel Read Data CRC Register */
#define DMA_CH11_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012160u)

/** Alias (User Manual Name) for DMA_CH11_RDCRCR.
* To use register names with standard convension, please use DMA_CH11_RDCRCR.
*/
#define	DMA_RDCRCR011	(DMA_CH11_RDCRCR)

/** \\brief  2168, DMA Channel Source Address Register */
#define DMA_CH11_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012168u)

/** Alias (User Manual Name) for DMA_CH11_SADR.
* To use register names with standard convension, please use DMA_CH11_SADR.
*/
#define	DMA_SADR011	(DMA_CH11_SADR)

/** \\brief  2164, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH11_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012164u)

/** Alias (User Manual Name) for DMA_CH11_SDCRCR.
* To use register names with standard convension, please use DMA_CH11_SDCRCR.
*/
#define	DMA_SDCRCR011	(DMA_CH11_SDCRCR)

/** \\brief  2178, DMA Channel Shadow Address Register */
#define DMA_CH11_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012178u)

/** Alias (User Manual Name) for DMA_CH11_SHADR.
* To use register names with standard convension, please use DMA_CH11_SHADR.
*/
#define	DMA_SHADR011	(DMA_CH11_SHADR)

/** \\brief  2190, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH12_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012190u)

/** Alias (User Manual Name) for DMA_CH12_ADICR.
* To use register names with standard convension, please use DMA_CH12_ADICR.
*/
#define	DMA_ADICR012	(DMA_CH12_ADICR)

/** \\brief  2194, DMA Channel Configuration Register */
#define DMA_CH12_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012194u)

/** Alias (User Manual Name) for DMA_CH12_CHCFGR.
* To use register names with standard convension, please use DMA_CH12_CHCFGR.
*/
#define	DMA_CHCFGR012	(DMA_CH12_CHCFGR)

/** \\brief  219C, DMARAM Channel Control and Status Register */
#define DMA_CH12_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001219Cu)

/** Alias (User Manual Name) for DMA_CH12_CHCSR.
* To use register names with standard convension, please use DMA_CH12_CHCSR.
*/
#define	DMA_CHCSR012	(DMA_CH12_CHCSR)

/** \\brief  218C, DMA Channel Destination Address Register x */
#define DMA_CH12_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001218Cu)

/** Alias (User Manual Name) for DMA_CH12_DADR.
* To use register names with standard convension, please use DMA_CH12_DADR.
*/
#define	DMA_DADR012	(DMA_CH12_DADR)

/** \\brief  2180, DMA Channel Read Data CRC Register */
#define DMA_CH12_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012180u)

/** Alias (User Manual Name) for DMA_CH12_RDCRCR.
* To use register names with standard convension, please use DMA_CH12_RDCRCR.
*/
#define	DMA_RDCRCR012	(DMA_CH12_RDCRCR)

/** \\brief  2188, DMA Channel Source Address Register */
#define DMA_CH12_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012188u)

/** Alias (User Manual Name) for DMA_CH12_SADR.
* To use register names with standard convension, please use DMA_CH12_SADR.
*/
#define	DMA_SADR012	(DMA_CH12_SADR)

/** \\brief  2184, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH12_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012184u)

/** Alias (User Manual Name) for DMA_CH12_SDCRCR.
* To use register names with standard convension, please use DMA_CH12_SDCRCR.
*/
#define	DMA_SDCRCR012	(DMA_CH12_SDCRCR)

/** \\brief  2198, DMA Channel Shadow Address Register */
#define DMA_CH12_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012198u)

/** Alias (User Manual Name) for DMA_CH12_SHADR.
* To use register names with standard convension, please use DMA_CH12_SHADR.
*/
#define	DMA_SHADR012	(DMA_CH12_SHADR)

/** \\brief  21B0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH13_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00121B0u)

/** Alias (User Manual Name) for DMA_CH13_ADICR.
* To use register names with standard convension, please use DMA_CH13_ADICR.
*/
#define	DMA_ADICR013	(DMA_CH13_ADICR)

/** \\brief  21B4, DMA Channel Configuration Register */
#define DMA_CH13_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00121B4u)

/** Alias (User Manual Name) for DMA_CH13_CHCFGR.
* To use register names with standard convension, please use DMA_CH13_CHCFGR.
*/
#define	DMA_CHCFGR013	(DMA_CH13_CHCFGR)

/** \\brief  21BC, DMARAM Channel Control and Status Register */
#define DMA_CH13_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00121BCu)

/** Alias (User Manual Name) for DMA_CH13_CHCSR.
* To use register names with standard convension, please use DMA_CH13_CHCSR.
*/
#define	DMA_CHCSR013	(DMA_CH13_CHCSR)

/** \\brief  21AC, DMA Channel Destination Address Register x */
#define DMA_CH13_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00121ACu)

/** Alias (User Manual Name) for DMA_CH13_DADR.
* To use register names with standard convension, please use DMA_CH13_DADR.
*/
#define	DMA_DADR013	(DMA_CH13_DADR)

/** \\brief  21A0, DMA Channel Read Data CRC Register */
#define DMA_CH13_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00121A0u)

/** Alias (User Manual Name) for DMA_CH13_RDCRCR.
* To use register names with standard convension, please use DMA_CH13_RDCRCR.
*/
#define	DMA_RDCRCR013	(DMA_CH13_RDCRCR)

/** \\brief  21A8, DMA Channel Source Address Register */
#define DMA_CH13_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00121A8u)

/** Alias (User Manual Name) for DMA_CH13_SADR.
* To use register names with standard convension, please use DMA_CH13_SADR.
*/
#define	DMA_SADR013	(DMA_CH13_SADR)

/** \\brief  21A4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH13_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00121A4u)

/** Alias (User Manual Name) for DMA_CH13_SDCRCR.
* To use register names with standard convension, please use DMA_CH13_SDCRCR.
*/
#define	DMA_SDCRCR013	(DMA_CH13_SDCRCR)

/** \\brief  21B8, DMA Channel Shadow Address Register */
#define DMA_CH13_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00121B8u)

/** Alias (User Manual Name) for DMA_CH13_SHADR.
* To use register names with standard convension, please use DMA_CH13_SHADR.
*/
#define	DMA_SHADR013	(DMA_CH13_SHADR)

/** \\brief  21D0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH14_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00121D0u)

/** Alias (User Manual Name) for DMA_CH14_ADICR.
* To use register names with standard convension, please use DMA_CH14_ADICR.
*/
#define	DMA_ADICR014	(DMA_CH14_ADICR)

/** \\brief  21D4, DMA Channel Configuration Register */
#define DMA_CH14_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00121D4u)

/** Alias (User Manual Name) for DMA_CH14_CHCFGR.
* To use register names with standard convension, please use DMA_CH14_CHCFGR.
*/
#define	DMA_CHCFGR014	(DMA_CH14_CHCFGR)

/** \\brief  21DC, DMARAM Channel Control and Status Register */
#define DMA_CH14_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00121DCu)

/** Alias (User Manual Name) for DMA_CH14_CHCSR.
* To use register names with standard convension, please use DMA_CH14_CHCSR.
*/
#define	DMA_CHCSR014	(DMA_CH14_CHCSR)

/** \\brief  21CC, DMA Channel Destination Address Register x */
#define DMA_CH14_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00121CCu)

/** Alias (User Manual Name) for DMA_CH14_DADR.
* To use register names with standard convension, please use DMA_CH14_DADR.
*/
#define	DMA_DADR014	(DMA_CH14_DADR)

/** \\brief  21C0, DMA Channel Read Data CRC Register */
#define DMA_CH14_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00121C0u)

/** Alias (User Manual Name) for DMA_CH14_RDCRCR.
* To use register names with standard convension, please use DMA_CH14_RDCRCR.
*/
#define	DMA_RDCRCR014	(DMA_CH14_RDCRCR)

/** \\brief  21C8, DMA Channel Source Address Register */
#define DMA_CH14_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00121C8u)

/** Alias (User Manual Name) for DMA_CH14_SADR.
* To use register names with standard convension, please use DMA_CH14_SADR.
*/
#define	DMA_SADR014	(DMA_CH14_SADR)

/** \\brief  21C4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH14_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00121C4u)

/** Alias (User Manual Name) for DMA_CH14_SDCRCR.
* To use register names with standard convension, please use DMA_CH14_SDCRCR.
*/
#define	DMA_SDCRCR014	(DMA_CH14_SDCRCR)

/** \\brief  21D8, DMA Channel Shadow Address Register */
#define DMA_CH14_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00121D8u)

/** Alias (User Manual Name) for DMA_CH14_SHADR.
* To use register names with standard convension, please use DMA_CH14_SHADR.
*/
#define	DMA_SHADR014	(DMA_CH14_SHADR)

/** \\brief  21F0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH15_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00121F0u)

/** Alias (User Manual Name) for DMA_CH15_ADICR.
* To use register names with standard convension, please use DMA_CH15_ADICR.
*/
#define	DMA_ADICR015	(DMA_CH15_ADICR)

/** \\brief  21F4, DMA Channel Configuration Register */
#define DMA_CH15_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00121F4u)

/** Alias (User Manual Name) for DMA_CH15_CHCFGR.
* To use register names with standard convension, please use DMA_CH15_CHCFGR.
*/
#define	DMA_CHCFGR015	(DMA_CH15_CHCFGR)

/** \\brief  21FC, DMARAM Channel Control and Status Register */
#define DMA_CH15_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00121FCu)

/** Alias (User Manual Name) for DMA_CH15_CHCSR.
* To use register names with standard convension, please use DMA_CH15_CHCSR.
*/
#define	DMA_CHCSR015	(DMA_CH15_CHCSR)

/** \\brief  21EC, DMA Channel Destination Address Register x */
#define DMA_CH15_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00121ECu)

/** Alias (User Manual Name) for DMA_CH15_DADR.
* To use register names with standard convension, please use DMA_CH15_DADR.
*/
#define	DMA_DADR015	(DMA_CH15_DADR)

/** \\brief  21E0, DMA Channel Read Data CRC Register */
#define DMA_CH15_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00121E0u)

/** Alias (User Manual Name) for DMA_CH15_RDCRCR.
* To use register names with standard convension, please use DMA_CH15_RDCRCR.
*/
#define	DMA_RDCRCR015	(DMA_CH15_RDCRCR)

/** \\brief  21E8, DMA Channel Source Address Register */
#define DMA_CH15_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00121E8u)

/** Alias (User Manual Name) for DMA_CH15_SADR.
* To use register names with standard convension, please use DMA_CH15_SADR.
*/
#define	DMA_SADR015	(DMA_CH15_SADR)

/** \\brief  21E4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH15_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00121E4u)

/** Alias (User Manual Name) for DMA_CH15_SDCRCR.
* To use register names with standard convension, please use DMA_CH15_SDCRCR.
*/
#define	DMA_SDCRCR015	(DMA_CH15_SDCRCR)

/** \\brief  21F8, DMA Channel Shadow Address Register */
#define DMA_CH15_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00121F8u)

/** Alias (User Manual Name) for DMA_CH15_SHADR.
* To use register names with standard convension, please use DMA_CH15_SHADR.
*/
#define	DMA_SHADR015	(DMA_CH15_SHADR)

/** \\brief  2030, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH1_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012030u)

/** Alias (User Manual Name) for DMA_CH1_ADICR.
* To use register names with standard convension, please use DMA_CH1_ADICR.
*/
#define	DMA_ADICR001	(DMA_CH1_ADICR)

/** \\brief  2034, DMA Channel Configuration Register */
#define DMA_CH1_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012034u)

/** Alias (User Manual Name) for DMA_CH1_CHCFGR.
* To use register names with standard convension, please use DMA_CH1_CHCFGR.
*/
#define	DMA_CHCFGR001	(DMA_CH1_CHCFGR)

/** \\brief  203C, DMARAM Channel Control and Status Register */
#define DMA_CH1_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001203Cu)

/** Alias (User Manual Name) for DMA_CH1_CHCSR.
* To use register names with standard convension, please use DMA_CH1_CHCSR.
*/
#define	DMA_CHCSR001	(DMA_CH1_CHCSR)

/** \\brief  202C, DMA Channel Destination Address Register x */
#define DMA_CH1_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001202Cu)

/** Alias (User Manual Name) for DMA_CH1_DADR.
* To use register names with standard convension, please use DMA_CH1_DADR.
*/
#define	DMA_DADR001	(DMA_CH1_DADR)

/** \\brief  2020, DMA Channel Read Data CRC Register */
#define DMA_CH1_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012020u)

/** Alias (User Manual Name) for DMA_CH1_RDCRCR.
* To use register names with standard convension, please use DMA_CH1_RDCRCR.
*/
#define	DMA_RDCRCR001	(DMA_CH1_RDCRCR)

/** \\brief  2028, DMA Channel Source Address Register */
#define DMA_CH1_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012028u)

/** Alias (User Manual Name) for DMA_CH1_SADR.
* To use register names with standard convension, please use DMA_CH1_SADR.
*/
#define	DMA_SADR001	(DMA_CH1_SADR)

/** \\brief  2024, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH1_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012024u)

/** Alias (User Manual Name) for DMA_CH1_SDCRCR.
* To use register names with standard convension, please use DMA_CH1_SDCRCR.
*/
#define	DMA_SDCRCR001	(DMA_CH1_SDCRCR)

/** \\brief  2038, DMA Channel Shadow Address Register */
#define DMA_CH1_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012038u)

/** Alias (User Manual Name) for DMA_CH1_SHADR.
* To use register names with standard convension, please use DMA_CH1_SHADR.
*/
#define	DMA_SHADR001	(DMA_CH1_SHADR)

/** \\brief  2050, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH2_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012050u)

/** Alias (User Manual Name) for DMA_CH2_ADICR.
* To use register names with standard convension, please use DMA_CH2_ADICR.
*/
#define	DMA_ADICR002	(DMA_CH2_ADICR)

/** \\brief  2054, DMA Channel Configuration Register */
#define DMA_CH2_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012054u)

/** Alias (User Manual Name) for DMA_CH2_CHCFGR.
* To use register names with standard convension, please use DMA_CH2_CHCFGR.
*/
#define	DMA_CHCFGR002	(DMA_CH2_CHCFGR)

/** \\brief  205C, DMARAM Channel Control and Status Register */
#define DMA_CH2_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001205Cu)

/** Alias (User Manual Name) for DMA_CH2_CHCSR.
* To use register names with standard convension, please use DMA_CH2_CHCSR.
*/
#define	DMA_CHCSR002	(DMA_CH2_CHCSR)

/** \\brief  204C, DMA Channel Destination Address Register x */
#define DMA_CH2_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001204Cu)

/** Alias (User Manual Name) for DMA_CH2_DADR.
* To use register names with standard convension, please use DMA_CH2_DADR.
*/
#define	DMA_DADR002	(DMA_CH2_DADR)

/** \\brief  2040, DMA Channel Read Data CRC Register */
#define DMA_CH2_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012040u)

/** Alias (User Manual Name) for DMA_CH2_RDCRCR.
* To use register names with standard convension, please use DMA_CH2_RDCRCR.
*/
#define	DMA_RDCRCR002	(DMA_CH2_RDCRCR)

/** \\brief  2048, DMA Channel Source Address Register */
#define DMA_CH2_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012048u)

/** Alias (User Manual Name) for DMA_CH2_SADR.
* To use register names with standard convension, please use DMA_CH2_SADR.
*/
#define	DMA_SADR002	(DMA_CH2_SADR)

/** \\brief  2044, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH2_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012044u)

/** Alias (User Manual Name) for DMA_CH2_SDCRCR.
* To use register names with standard convension, please use DMA_CH2_SDCRCR.
*/
#define	DMA_SDCRCR002	(DMA_CH2_SDCRCR)

/** \\brief  2058, DMA Channel Shadow Address Register */
#define DMA_CH2_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012058u)

/** Alias (User Manual Name) for DMA_CH2_SHADR.
* To use register names with standard convension, please use DMA_CH2_SHADR.
*/
#define	DMA_SHADR002	(DMA_CH2_SHADR)

/** \\brief  2070, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH3_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012070u)

/** Alias (User Manual Name) for DMA_CH3_ADICR.
* To use register names with standard convension, please use DMA_CH3_ADICR.
*/
#define	DMA_ADICR003	(DMA_CH3_ADICR)

/** \\brief  2074, DMA Channel Configuration Register */
#define DMA_CH3_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012074u)

/** Alias (User Manual Name) for DMA_CH3_CHCFGR.
* To use register names with standard convension, please use DMA_CH3_CHCFGR.
*/
#define	DMA_CHCFGR003	(DMA_CH3_CHCFGR)

/** \\brief  207C, DMARAM Channel Control and Status Register */
#define DMA_CH3_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001207Cu)

/** Alias (User Manual Name) for DMA_CH3_CHCSR.
* To use register names with standard convension, please use DMA_CH3_CHCSR.
*/
#define	DMA_CHCSR003	(DMA_CH3_CHCSR)

/** \\brief  206C, DMA Channel Destination Address Register x */
#define DMA_CH3_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001206Cu)

/** Alias (User Manual Name) for DMA_CH3_DADR.
* To use register names with standard convension, please use DMA_CH3_DADR.
*/
#define	DMA_DADR003	(DMA_CH3_DADR)

/** \\brief  2060, DMA Channel Read Data CRC Register */
#define DMA_CH3_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012060u)

/** Alias (User Manual Name) for DMA_CH3_RDCRCR.
* To use register names with standard convension, please use DMA_CH3_RDCRCR.
*/
#define	DMA_RDCRCR003	(DMA_CH3_RDCRCR)

/** \\brief  2068, DMA Channel Source Address Register */
#define DMA_CH3_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012068u)

/** Alias (User Manual Name) for DMA_CH3_SADR.
* To use register names with standard convension, please use DMA_CH3_SADR.
*/
#define	DMA_SADR003	(DMA_CH3_SADR)

/** \\brief  2064, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH3_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012064u)

/** Alias (User Manual Name) for DMA_CH3_SDCRCR.
* To use register names with standard convension, please use DMA_CH3_SDCRCR.
*/
#define	DMA_SDCRCR003	(DMA_CH3_SDCRCR)

/** \\brief  2078, DMA Channel Shadow Address Register */
#define DMA_CH3_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012078u)

/** Alias (User Manual Name) for DMA_CH3_SHADR.
* To use register names with standard convension, please use DMA_CH3_SHADR.
*/
#define	DMA_SHADR003	(DMA_CH3_SHADR)

/** \\brief  2090, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH4_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012090u)

/** Alias (User Manual Name) for DMA_CH4_ADICR.
* To use register names with standard convension, please use DMA_CH4_ADICR.
*/
#define	DMA_ADICR004	(DMA_CH4_ADICR)

/** \\brief  2094, DMA Channel Configuration Register */
#define DMA_CH4_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012094u)

/** Alias (User Manual Name) for DMA_CH4_CHCFGR.
* To use register names with standard convension, please use DMA_CH4_CHCFGR.
*/
#define	DMA_CHCFGR004	(DMA_CH4_CHCFGR)

/** \\brief  209C, DMARAM Channel Control and Status Register */
#define DMA_CH4_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001209Cu)

/** Alias (User Manual Name) for DMA_CH4_CHCSR.
* To use register names with standard convension, please use DMA_CH4_CHCSR.
*/
#define	DMA_CHCSR004	(DMA_CH4_CHCSR)

/** \\brief  208C, DMA Channel Destination Address Register x */
#define DMA_CH4_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001208Cu)

/** Alias (User Manual Name) for DMA_CH4_DADR.
* To use register names with standard convension, please use DMA_CH4_DADR.
*/
#define	DMA_DADR004	(DMA_CH4_DADR)

/** \\brief  2080, DMA Channel Read Data CRC Register */
#define DMA_CH4_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012080u)

/** Alias (User Manual Name) for DMA_CH4_RDCRCR.
* To use register names with standard convension, please use DMA_CH4_RDCRCR.
*/
#define	DMA_RDCRCR004	(DMA_CH4_RDCRCR)

/** \\brief  2088, DMA Channel Source Address Register */
#define DMA_CH4_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012088u)

/** Alias (User Manual Name) for DMA_CH4_SADR.
* To use register names with standard convension, please use DMA_CH4_SADR.
*/
#define	DMA_SADR004	(DMA_CH4_SADR)

/** \\brief  2084, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH4_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012084u)

/** Alias (User Manual Name) for DMA_CH4_SDCRCR.
* To use register names with standard convension, please use DMA_CH4_SDCRCR.
*/
#define	DMA_SDCRCR004	(DMA_CH4_SDCRCR)

/** \\brief  2098, DMA Channel Shadow Address Register */
#define DMA_CH4_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012098u)

/** Alias (User Manual Name) for DMA_CH4_SHADR.
* To use register names with standard convension, please use DMA_CH4_SHADR.
*/
#define	DMA_SHADR004	(DMA_CH4_SHADR)

/** \\brief  20B0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH5_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00120B0u)

/** Alias (User Manual Name) for DMA_CH5_ADICR.
* To use register names with standard convension, please use DMA_CH5_ADICR.
*/
#define	DMA_ADICR005	(DMA_CH5_ADICR)

/** \\brief  20B4, DMA Channel Configuration Register */
#define DMA_CH5_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00120B4u)

/** Alias (User Manual Name) for DMA_CH5_CHCFGR.
* To use register names with standard convension, please use DMA_CH5_CHCFGR.
*/
#define	DMA_CHCFGR005	(DMA_CH5_CHCFGR)

/** \\brief  20BC, DMARAM Channel Control and Status Register */
#define DMA_CH5_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00120BCu)

/** Alias (User Manual Name) for DMA_CH5_CHCSR.
* To use register names with standard convension, please use DMA_CH5_CHCSR.
*/
#define	DMA_CHCSR005	(DMA_CH5_CHCSR)

/** \\brief  20AC, DMA Channel Destination Address Register x */
#define DMA_CH5_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00120ACu)

/** Alias (User Manual Name) for DMA_CH5_DADR.
* To use register names with standard convension, please use DMA_CH5_DADR.
*/
#define	DMA_DADR005	(DMA_CH5_DADR)

/** \\brief  20A0, DMA Channel Read Data CRC Register */
#define DMA_CH5_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00120A0u)

/** Alias (User Manual Name) for DMA_CH5_RDCRCR.
* To use register names with standard convension, please use DMA_CH5_RDCRCR.
*/
#define	DMA_RDCRCR005	(DMA_CH5_RDCRCR)

/** \\brief  20A8, DMA Channel Source Address Register */
#define DMA_CH5_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00120A8u)

/** Alias (User Manual Name) for DMA_CH5_SADR.
* To use register names with standard convension, please use DMA_CH5_SADR.
*/
#define	DMA_SADR005	(DMA_CH5_SADR)

/** \\brief  20A4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH5_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00120A4u)

/** Alias (User Manual Name) for DMA_CH5_SDCRCR.
* To use register names with standard convension, please use DMA_CH5_SDCRCR.
*/
#define	DMA_SDCRCR005	(DMA_CH5_SDCRCR)

/** \\brief  20B8, DMA Channel Shadow Address Register */
#define DMA_CH5_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00120B8u)

/** Alias (User Manual Name) for DMA_CH5_SHADR.
* To use register names with standard convension, please use DMA_CH5_SHADR.
*/
#define	DMA_SHADR005	(DMA_CH5_SHADR)

/** \\brief  20D0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH6_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00120D0u)

/** Alias (User Manual Name) for DMA_CH6_ADICR.
* To use register names with standard convension, please use DMA_CH6_ADICR.
*/
#define	DMA_ADICR006	(DMA_CH6_ADICR)

/** \\brief  20D4, DMA Channel Configuration Register */
#define DMA_CH6_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00120D4u)

/** Alias (User Manual Name) for DMA_CH6_CHCFGR.
* To use register names with standard convension, please use DMA_CH6_CHCFGR.
*/
#define	DMA_CHCFGR006	(DMA_CH6_CHCFGR)

/** \\brief  20DC, DMARAM Channel Control and Status Register */
#define DMA_CH6_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00120DCu)

/** Alias (User Manual Name) for DMA_CH6_CHCSR.
* To use register names with standard convension, please use DMA_CH6_CHCSR.
*/
#define	DMA_CHCSR006	(DMA_CH6_CHCSR)

/** \\brief  20CC, DMA Channel Destination Address Register x */
#define DMA_CH6_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00120CCu)

/** Alias (User Manual Name) for DMA_CH6_DADR.
* To use register names with standard convension, please use DMA_CH6_DADR.
*/
#define	DMA_DADR006	(DMA_CH6_DADR)

/** \\brief  20C0, DMA Channel Read Data CRC Register */
#define DMA_CH6_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00120C0u)

/** Alias (User Manual Name) for DMA_CH6_RDCRCR.
* To use register names with standard convension, please use DMA_CH6_RDCRCR.
*/
#define	DMA_RDCRCR006	(DMA_CH6_RDCRCR)

/** \\brief  20C8, DMA Channel Source Address Register */
#define DMA_CH6_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00120C8u)

/** Alias (User Manual Name) for DMA_CH6_SADR.
* To use register names with standard convension, please use DMA_CH6_SADR.
*/
#define	DMA_SADR006	(DMA_CH6_SADR)

/** \\brief  20C4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH6_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00120C4u)

/** Alias (User Manual Name) for DMA_CH6_SDCRCR.
* To use register names with standard convension, please use DMA_CH6_SDCRCR.
*/
#define	DMA_SDCRCR006	(DMA_CH6_SDCRCR)

/** \\brief  20D8, DMA Channel Shadow Address Register */
#define DMA_CH6_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00120D8u)

/** Alias (User Manual Name) for DMA_CH6_SHADR.
* To use register names with standard convension, please use DMA_CH6_SHADR.
*/
#define	DMA_SHADR006	(DMA_CH6_SHADR)

/** \\brief  20F0, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH7_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF00120F0u)

/** Alias (User Manual Name) for DMA_CH7_ADICR.
* To use register names with standard convension, please use DMA_CH7_ADICR.
*/
#define	DMA_ADICR007	(DMA_CH7_ADICR)

/** \\brief  20F4, DMA Channel Configuration Register */
#define DMA_CH7_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF00120F4u)

/** Alias (User Manual Name) for DMA_CH7_CHCFGR.
* To use register names with standard convension, please use DMA_CH7_CHCFGR.
*/
#define	DMA_CHCFGR007	(DMA_CH7_CHCFGR)

/** \\brief  20FC, DMARAM Channel Control and Status Register */
#define DMA_CH7_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF00120FCu)

/** Alias (User Manual Name) for DMA_CH7_CHCSR.
* To use register names with standard convension, please use DMA_CH7_CHCSR.
*/
#define	DMA_CHCSR007	(DMA_CH7_CHCSR)

/** \\brief  20EC, DMA Channel Destination Address Register x */
#define DMA_CH7_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF00120ECu)

/** Alias (User Manual Name) for DMA_CH7_DADR.
* To use register names with standard convension, please use DMA_CH7_DADR.
*/
#define	DMA_DADR007	(DMA_CH7_DADR)

/** \\brief  20E0, DMA Channel Read Data CRC Register */
#define DMA_CH7_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF00120E0u)

/** Alias (User Manual Name) for DMA_CH7_RDCRCR.
* To use register names with standard convension, please use DMA_CH7_RDCRCR.
*/
#define	DMA_RDCRCR007	(DMA_CH7_RDCRCR)

/** \\brief  20E8, DMA Channel Source Address Register */
#define DMA_CH7_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF00120E8u)

/** Alias (User Manual Name) for DMA_CH7_SADR.
* To use register names with standard convension, please use DMA_CH7_SADR.
*/
#define	DMA_SADR007	(DMA_CH7_SADR)

/** \\brief  20E4, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH7_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF00120E4u)

/** Alias (User Manual Name) for DMA_CH7_SDCRCR.
* To use register names with standard convension, please use DMA_CH7_SDCRCR.
*/
#define	DMA_SDCRCR007	(DMA_CH7_SDCRCR)

/** \\brief  20F8, DMA Channel Shadow Address Register */
#define DMA_CH7_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF00120F8u)

/** Alias (User Manual Name) for DMA_CH7_SHADR.
* To use register names with standard convension, please use DMA_CH7_SHADR.
*/
#define	DMA_SHADR007	(DMA_CH7_SHADR)

/** \\brief  2110, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH8_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012110u)

/** Alias (User Manual Name) for DMA_CH8_ADICR.
* To use register names with standard convension, please use DMA_CH8_ADICR.
*/
#define	DMA_ADICR008	(DMA_CH8_ADICR)

/** \\brief  2114, DMA Channel Configuration Register */
#define DMA_CH8_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012114u)

/** Alias (User Manual Name) for DMA_CH8_CHCFGR.
* To use register names with standard convension, please use DMA_CH8_CHCFGR.
*/
#define	DMA_CHCFGR008	(DMA_CH8_CHCFGR)

/** \\brief  211C, DMARAM Channel Control and Status Register */
#define DMA_CH8_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001211Cu)

/** Alias (User Manual Name) for DMA_CH8_CHCSR.
* To use register names with standard convension, please use DMA_CH8_CHCSR.
*/
#define	DMA_CHCSR008	(DMA_CH8_CHCSR)

/** \\brief  210C, DMA Channel Destination Address Register x */
#define DMA_CH8_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001210Cu)

/** Alias (User Manual Name) for DMA_CH8_DADR.
* To use register names with standard convension, please use DMA_CH8_DADR.
*/
#define	DMA_DADR008	(DMA_CH8_DADR)

/** \\brief  2100, DMA Channel Read Data CRC Register */
#define DMA_CH8_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012100u)

/** Alias (User Manual Name) for DMA_CH8_RDCRCR.
* To use register names with standard convension, please use DMA_CH8_RDCRCR.
*/
#define	DMA_RDCRCR008	(DMA_CH8_RDCRCR)

/** \\brief  2108, DMA Channel Source Address Register */
#define DMA_CH8_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012108u)

/** Alias (User Manual Name) for DMA_CH8_SADR.
* To use register names with standard convension, please use DMA_CH8_SADR.
*/
#define	DMA_SADR008	(DMA_CH8_SADR)

/** \\brief  2104, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH8_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012104u)

/** Alias (User Manual Name) for DMA_CH8_SDCRCR.
* To use register names with standard convension, please use DMA_CH8_SDCRCR.
*/
#define	DMA_SDCRCR008	(DMA_CH8_SDCRCR)

/** \\brief  2118, DMA Channel Shadow Address Register */
#define DMA_CH8_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012118u)

/** Alias (User Manual Name) for DMA_CH8_SHADR.
* To use register names with standard convension, please use DMA_CH8_SHADR.
*/
#define	DMA_SHADR008	(DMA_CH8_SHADR)

/** \\brief  2130, DMA Channel Address and Interrupt Control Register x */
#define DMA_CH9_ADICR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_ADICR*)0xF0012130u)

/** Alias (User Manual Name) for DMA_CH9_ADICR.
* To use register names with standard convension, please use DMA_CH9_ADICR.
*/
#define	DMA_ADICR009	(DMA_CH9_ADICR)

/** \\brief  2134, DMA Channel Configuration Register */
#define DMA_CH9_CHCFGR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCFGR*)0xF0012134u)

/** Alias (User Manual Name) for DMA_CH9_CHCFGR.
* To use register names with standard convension, please use DMA_CH9_CHCFGR.
*/
#define	DMA_CHCFGR009	(DMA_CH9_CHCFGR)

/** \\brief  213C, DMARAM Channel Control and Status Register */
#define DMA_CH9_CHCSR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_CHCSR*)0xF001213Cu)

/** Alias (User Manual Name) for DMA_CH9_CHCSR.
* To use register names with standard convension, please use DMA_CH9_CHCSR.
*/
#define	DMA_CHCSR009	(DMA_CH9_CHCSR)

/** \\brief  212C, DMA Channel Destination Address Register x */
#define DMA_CH9_DADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_DADR*)0xF001212Cu)

/** Alias (User Manual Name) for DMA_CH9_DADR.
* To use register names with standard convension, please use DMA_CH9_DADR.
*/
#define	DMA_DADR009	(DMA_CH9_DADR)

/** \\brief  2120, DMA Channel Read Data CRC Register */
#define DMA_CH9_RDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_RDCRCR*)0xF0012120u)

/** Alias (User Manual Name) for DMA_CH9_RDCRCR.
* To use register names with standard convension, please use DMA_CH9_RDCRCR.
*/
#define	DMA_RDCRCR009	(DMA_CH9_RDCRCR)

/** \\brief  2128, DMA Channel Source Address Register */
#define DMA_CH9_SADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SADR*)0xF0012128u)

/** Alias (User Manual Name) for DMA_CH9_SADR.
* To use register names with standard convension, please use DMA_CH9_SADR.
*/
#define	DMA_SADR009	(DMA_CH9_SADR)

/** \\brief  2124, DMA Channel Source and Destination Address CRC Register */
#define DMA_CH9_SDCRCR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SDCRCR*)0xF0012124u)

/** Alias (User Manual Name) for DMA_CH9_SDCRCR.
* To use register names with standard convension, please use DMA_CH9_SDCRCR.
*/
#define	DMA_SDCRCR009	(DMA_CH9_SDCRCR)

/** \\brief  2138, DMA Channel Shadow Address Register */
#define DMA_CH9_SHADR /*lint --e(923)*/ (*(volatile Ifx_DMA_CH_SHADR*)0xF0012138u)

/** Alias (User Manual Name) for DMA_CH9_SHADR.
* To use register names with standard convension, please use DMA_CH9_SHADR.
*/
#define	DMA_SHADR009	(DMA_CH9_SHADR)

/** \\brief  0, DMA Clock Control Register */
#define DMA_CLC /*lint --e(923)*/ (*(volatile Ifx_DMA_CLC*)0xF0010000u)

/** \\brief  1204, DMA Error Interrupt Set Register */
#define DMA_ERRINTR /*lint --e(923)*/ (*(volatile Ifx_DMA_ERRINTR*)0xF0011204u)

/** \\brief  1800, DMA Channel Hardware Resource Register */
#define DMA_HRR0 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011800u)

/** Alias (User Manual Name) for DMA_HRR0.
* To use register names with standard convension, please use DMA_HRR0.
*/
#define	DMA_HRR000	(DMA_HRR0)

/** \\brief  1804, DMA Channel Hardware Resource Register */
#define DMA_HRR1 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011804u)

/** Alias (User Manual Name) for DMA_HRR1.
* To use register names with standard convension, please use DMA_HRR1.
*/
#define	DMA_HRR001	(DMA_HRR1)

/** \\brief  1828, DMA Channel Hardware Resource Register */
#define DMA_HRR10 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011828u)

/** Alias (User Manual Name) for DMA_HRR10.
* To use register names with standard convension, please use DMA_HRR10.
*/
#define	DMA_HRR010	(DMA_HRR10)

/** \\brief  182C, DMA Channel Hardware Resource Register */
#define DMA_HRR11 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF001182Cu)

/** Alias (User Manual Name) for DMA_HRR11.
* To use register names with standard convension, please use DMA_HRR11.
*/
#define	DMA_HRR011	(DMA_HRR11)

/** \\brief  1830, DMA Channel Hardware Resource Register */
#define DMA_HRR12 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011830u)

/** Alias (User Manual Name) for DMA_HRR12.
* To use register names with standard convension, please use DMA_HRR12.
*/
#define	DMA_HRR012	(DMA_HRR12)

/** \\brief  1834, DMA Channel Hardware Resource Register */
#define DMA_HRR13 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011834u)

/** Alias (User Manual Name) for DMA_HRR13.
* To use register names with standard convension, please use DMA_HRR13.
*/
#define	DMA_HRR013	(DMA_HRR13)

/** \\brief  1838, DMA Channel Hardware Resource Register */
#define DMA_HRR14 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011838u)

/** Alias (User Manual Name) for DMA_HRR14.
* To use register names with standard convension, please use DMA_HRR14.
*/
#define	DMA_HRR014	(DMA_HRR14)

/** \\brief  183C, DMA Channel Hardware Resource Register */
#define DMA_HRR15 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF001183Cu)

/** Alias (User Manual Name) for DMA_HRR15.
* To use register names with standard convension, please use DMA_HRR15.
*/
#define	DMA_HRR015	(DMA_HRR15)

/** \\brief  1808, DMA Channel Hardware Resource Register */
#define DMA_HRR2 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011808u)

/** Alias (User Manual Name) for DMA_HRR2.
* To use register names with standard convension, please use DMA_HRR2.
*/
#define	DMA_HRR002	(DMA_HRR2)

/** \\brief  180C, DMA Channel Hardware Resource Register */
#define DMA_HRR3 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF001180Cu)

/** Alias (User Manual Name) for DMA_HRR3.
* To use register names with standard convension, please use DMA_HRR3.
*/
#define	DMA_HRR003	(DMA_HRR3)

/** \\brief  1810, DMA Channel Hardware Resource Register */
#define DMA_HRR4 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011810u)

/** Alias (User Manual Name) for DMA_HRR4.
* To use register names with standard convension, please use DMA_HRR4.
*/
#define	DMA_HRR004	(DMA_HRR4)

/** \\brief  1814, DMA Channel Hardware Resource Register */
#define DMA_HRR5 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011814u)

/** Alias (User Manual Name) for DMA_HRR5.
* To use register names with standard convension, please use DMA_HRR5.
*/
#define	DMA_HRR005	(DMA_HRR5)

/** \\brief  1818, DMA Channel Hardware Resource Register */
#define DMA_HRR6 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011818u)

/** Alias (User Manual Name) for DMA_HRR6.
* To use register names with standard convension, please use DMA_HRR6.
*/
#define	DMA_HRR006	(DMA_HRR6)

/** \\brief  181C, DMA Channel Hardware Resource Register */
#define DMA_HRR7 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF001181Cu)

/** Alias (User Manual Name) for DMA_HRR7.
* To use register names with standard convension, please use DMA_HRR7.
*/
#define	DMA_HRR007	(DMA_HRR7)

/** \\brief  1820, DMA Channel Hardware Resource Register */
#define DMA_HRR8 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011820u)

/** Alias (User Manual Name) for DMA_HRR8.
* To use register names with standard convension, please use DMA_HRR8.
*/
#define	DMA_HRR008	(DMA_HRR8)

/** \\brief  1824, DMA Channel Hardware Resource Register */
#define DMA_HRR9 /*lint --e(923)*/ (*(volatile Ifx_DMA_HRR*)0xF0011824u)

/** Alias (User Manual Name) for DMA_HRR9.
* To use register names with standard convension, please use DMA_HRR9.
*/
#define	DMA_HRR009	(DMA_HRR9)

/** \\brief  8, Module Identification Register */
#define DMA_ID /*lint --e(923)*/ (*(volatile Ifx_DMA_ID*)0xF0010008u)

/** \\brief  20, DMA Memory Control Register */
#define DMA_MEMCON /*lint --e(923)*/ (*(volatile Ifx_DMA_MEMCON*)0xF0010020u)

/** \\brief  1300, DMA Mode Register */
#define DMA_MODE0 /*lint --e(923)*/ (*(volatile Ifx_DMA_MODE*)0xF0011300u)

/** \\brief  1304, DMA Mode Register */
#define DMA_MODE1 /*lint --e(923)*/ (*(volatile Ifx_DMA_MODE*)0xF0011304u)

/** \\brief  1308, DMA Mode Register */
#define DMA_MODE2 /*lint --e(923)*/ (*(volatile Ifx_DMA_MODE*)0xF0011308u)

/** \\brief  130C, DMA Mode Register */
#define DMA_MODE3 /*lint --e(923)*/ (*(volatile Ifx_DMA_MODE*)0xF001130Cu)

/** \\brief  1200, DMA OCDS Trigger Set Select */
#define DMA_OTSS /*lint --e(923)*/ (*(volatile Ifx_DMA_OTSS*)0xF0011200u)

/** \\brief  1208, Pattern Read Register 0 */
#define DMA_PRR0 /*lint --e(923)*/ (*(volatile Ifx_DMA_PRR0*)0xF0011208u)

/** \\brief  120C, Pattern Read Register 1 */
#define DMA_PRR1 /*lint --e(923)*/ (*(volatile Ifx_DMA_PRR1*)0xF001120Cu)

/** \\brief  1C00, DMA Suspend Acknowledge Register */
#define DMA_SUSACR0 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C00u)

/** Alias (User Manual Name) for DMA_SUSACR0.
* To use register names with standard convension, please use DMA_SUSACR0.
*/
#define	DMA_SUSACR000	(DMA_SUSACR0)

/** \\brief  1C04, DMA Suspend Acknowledge Register */
#define DMA_SUSACR1 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C04u)

/** Alias (User Manual Name) for DMA_SUSACR1.
* To use register names with standard convension, please use DMA_SUSACR1.
*/
#define	DMA_SUSACR001	(DMA_SUSACR1)

/** \\brief  1C28, DMA Suspend Acknowledge Register */
#define DMA_SUSACR10 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C28u)

/** Alias (User Manual Name) for DMA_SUSACR10.
* To use register names with standard convension, please use DMA_SUSACR10.
*/
#define	DMA_SUSACR010	(DMA_SUSACR10)

/** \\brief  1C2C, DMA Suspend Acknowledge Register */
#define DMA_SUSACR11 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C2Cu)

/** Alias (User Manual Name) for DMA_SUSACR11.
* To use register names with standard convension, please use DMA_SUSACR11.
*/
#define	DMA_SUSACR011	(DMA_SUSACR11)

/** \\brief  1C30, DMA Suspend Acknowledge Register */
#define DMA_SUSACR12 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C30u)

/** Alias (User Manual Name) for DMA_SUSACR12.
* To use register names with standard convension, please use DMA_SUSACR12.
*/
#define	DMA_SUSACR012	(DMA_SUSACR12)

/** \\brief  1C34, DMA Suspend Acknowledge Register */
#define DMA_SUSACR13 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C34u)

/** Alias (User Manual Name) for DMA_SUSACR13.
* To use register names with standard convension, please use DMA_SUSACR13.
*/
#define	DMA_SUSACR013	(DMA_SUSACR13)

/** \\brief  1C38, DMA Suspend Acknowledge Register */
#define DMA_SUSACR14 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C38u)

/** Alias (User Manual Name) for DMA_SUSACR14.
* To use register names with standard convension, please use DMA_SUSACR14.
*/
#define	DMA_SUSACR014	(DMA_SUSACR14)

/** \\brief  1C3C, DMA Suspend Acknowledge Register */
#define DMA_SUSACR15 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C3Cu)

/** Alias (User Manual Name) for DMA_SUSACR15.
* To use register names with standard convension, please use DMA_SUSACR15.
*/
#define	DMA_SUSACR015	(DMA_SUSACR15)

/** \\brief  1C08, DMA Suspend Acknowledge Register */
#define DMA_SUSACR2 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C08u)

/** Alias (User Manual Name) for DMA_SUSACR2.
* To use register names with standard convension, please use DMA_SUSACR2.
*/
#define	DMA_SUSACR002	(DMA_SUSACR2)

/** \\brief  1C0C, DMA Suspend Acknowledge Register */
#define DMA_SUSACR3 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C0Cu)

/** Alias (User Manual Name) for DMA_SUSACR3.
* To use register names with standard convension, please use DMA_SUSACR3.
*/
#define	DMA_SUSACR003	(DMA_SUSACR3)

/** \\brief  1C10, DMA Suspend Acknowledge Register */
#define DMA_SUSACR4 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C10u)

/** Alias (User Manual Name) for DMA_SUSACR4.
* To use register names with standard convension, please use DMA_SUSACR4.
*/
#define	DMA_SUSACR004	(DMA_SUSACR4)

/** \\brief  1C14, DMA Suspend Acknowledge Register */
#define DMA_SUSACR5 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C14u)

/** Alias (User Manual Name) for DMA_SUSACR5.
* To use register names with standard convension, please use DMA_SUSACR5.
*/
#define	DMA_SUSACR005	(DMA_SUSACR5)

/** \\brief  1C18, DMA Suspend Acknowledge Register */
#define DMA_SUSACR6 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C18u)

/** Alias (User Manual Name) for DMA_SUSACR6.
* To use register names with standard convension, please use DMA_SUSACR6.
*/
#define	DMA_SUSACR006	(DMA_SUSACR6)

/** \\brief  1C1C, DMA Suspend Acknowledge Register */
#define DMA_SUSACR7 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C1Cu)

/** Alias (User Manual Name) for DMA_SUSACR7.
* To use register names with standard convension, please use DMA_SUSACR7.
*/
#define	DMA_SUSACR007	(DMA_SUSACR7)

/** \\brief  1C20, DMA Suspend Acknowledge Register */
#define DMA_SUSACR8 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C20u)

/** Alias (User Manual Name) for DMA_SUSACR8.
* To use register names with standard convension, please use DMA_SUSACR8.
*/
#define	DMA_SUSACR008	(DMA_SUSACR8)

/** \\brief  1C24, DMA Suspend Acknowledge Register */
#define DMA_SUSACR9 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSACR*)0xF0011C24u)

/** Alias (User Manual Name) for DMA_SUSACR9.
* To use register names with standard convension, please use DMA_SUSACR9.
*/
#define	DMA_SUSACR009	(DMA_SUSACR9)

/** \\brief  1A00, DMA Suspend Enable Register */
#define DMA_SUSENR0 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A00u)

/** Alias (User Manual Name) for DMA_SUSENR0.
* To use register names with standard convension, please use DMA_SUSENR0.
*/
#define	DMA_SUSENR000	(DMA_SUSENR0)

/** \\brief  1A04, DMA Suspend Enable Register */
#define DMA_SUSENR1 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A04u)

/** Alias (User Manual Name) for DMA_SUSENR1.
* To use register names with standard convension, please use DMA_SUSENR1.
*/
#define	DMA_SUSENR001	(DMA_SUSENR1)

/** \\brief  1A28, DMA Suspend Enable Register */
#define DMA_SUSENR10 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A28u)

/** Alias (User Manual Name) for DMA_SUSENR10.
* To use register names with standard convension, please use DMA_SUSENR10.
*/
#define	DMA_SUSENR010	(DMA_SUSENR10)

/** \\brief  1A2C, DMA Suspend Enable Register */
#define DMA_SUSENR11 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A2Cu)

/** Alias (User Manual Name) for DMA_SUSENR11.
* To use register names with standard convension, please use DMA_SUSENR11.
*/
#define	DMA_SUSENR011	(DMA_SUSENR11)

/** \\brief  1A30, DMA Suspend Enable Register */
#define DMA_SUSENR12 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A30u)

/** Alias (User Manual Name) for DMA_SUSENR12.
* To use register names with standard convension, please use DMA_SUSENR12.
*/
#define	DMA_SUSENR012	(DMA_SUSENR12)

/** \\brief  1A34, DMA Suspend Enable Register */
#define DMA_SUSENR13 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A34u)

/** Alias (User Manual Name) for DMA_SUSENR13.
* To use register names with standard convension, please use DMA_SUSENR13.
*/
#define	DMA_SUSENR013	(DMA_SUSENR13)

/** \\brief  1A38, DMA Suspend Enable Register */
#define DMA_SUSENR14 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A38u)

/** Alias (User Manual Name) for DMA_SUSENR14.
* To use register names with standard convension, please use DMA_SUSENR14.
*/
#define	DMA_SUSENR014	(DMA_SUSENR14)

/** \\brief  1A3C, DMA Suspend Enable Register */
#define DMA_SUSENR15 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A3Cu)

/** Alias (User Manual Name) for DMA_SUSENR15.
* To use register names with standard convension, please use DMA_SUSENR15.
*/
#define	DMA_SUSENR015	(DMA_SUSENR15)

/** \\brief  1A08, DMA Suspend Enable Register */
#define DMA_SUSENR2 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A08u)

/** Alias (User Manual Name) for DMA_SUSENR2.
* To use register names with standard convension, please use DMA_SUSENR2.
*/
#define	DMA_SUSENR002	(DMA_SUSENR2)

/** \\brief  1A0C, DMA Suspend Enable Register */
#define DMA_SUSENR3 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A0Cu)

/** Alias (User Manual Name) for DMA_SUSENR3.
* To use register names with standard convension, please use DMA_SUSENR3.
*/
#define	DMA_SUSENR003	(DMA_SUSENR3)

/** \\brief  1A10, DMA Suspend Enable Register */
#define DMA_SUSENR4 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A10u)

/** Alias (User Manual Name) for DMA_SUSENR4.
* To use register names with standard convension, please use DMA_SUSENR4.
*/
#define	DMA_SUSENR004	(DMA_SUSENR4)

/** \\brief  1A14, DMA Suspend Enable Register */
#define DMA_SUSENR5 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A14u)

/** Alias (User Manual Name) for DMA_SUSENR5.
* To use register names with standard convension, please use DMA_SUSENR5.
*/
#define	DMA_SUSENR005	(DMA_SUSENR5)

/** \\brief  1A18, DMA Suspend Enable Register */
#define DMA_SUSENR6 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A18u)

/** Alias (User Manual Name) for DMA_SUSENR6.
* To use register names with standard convension, please use DMA_SUSENR6.
*/
#define	DMA_SUSENR006	(DMA_SUSENR6)

/** \\brief  1A1C, DMA Suspend Enable Register */
#define DMA_SUSENR7 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A1Cu)

/** Alias (User Manual Name) for DMA_SUSENR7.
* To use register names with standard convension, please use DMA_SUSENR7.
*/
#define	DMA_SUSENR007	(DMA_SUSENR7)

/** \\brief  1A20, DMA Suspend Enable Register */
#define DMA_SUSENR8 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A20u)

/** Alias (User Manual Name) for DMA_SUSENR8.
* To use register names with standard convension, please use DMA_SUSENR8.
*/
#define	DMA_SUSENR008	(DMA_SUSENR8)

/** \\brief  1A24, DMA Suspend Enable Register */
#define DMA_SUSENR9 /*lint --e(923)*/ (*(volatile Ifx_DMA_SUSENR*)0xF0011A24u)

/** Alias (User Manual Name) for DMA_SUSENR9.
* To use register names with standard convension, please use DMA_SUSENR9.
*/
#define	DMA_SUSENR009	(DMA_SUSENR9)

/** \\brief  1210, Time Register */
#define DMA_TIME /*lint --e(923)*/ (*(volatile Ifx_DMA_TIME*)0xF0011210u)

/** \\brief  1E00, DMA Transaction State Register */
#define DMA_TSR0 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E00u)

/** Alias (User Manual Name) for DMA_TSR0.
* To use register names with standard convension, please use DMA_TSR0.
*/
#define	DMA_TSR000	(DMA_TSR0)

/** \\brief  1E04, DMA Transaction State Register */
#define DMA_TSR1 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E04u)

/** Alias (User Manual Name) for DMA_TSR1.
* To use register names with standard convension, please use DMA_TSR1.
*/
#define	DMA_TSR001	(DMA_TSR1)

/** \\brief  1E28, DMA Transaction State Register */
#define DMA_TSR10 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E28u)

/** Alias (User Manual Name) for DMA_TSR10.
* To use register names with standard convension, please use DMA_TSR10.
*/
#define	DMA_TSR010	(DMA_TSR10)

/** \\brief  1E2C, DMA Transaction State Register */
#define DMA_TSR11 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E2Cu)

/** Alias (User Manual Name) for DMA_TSR11.
* To use register names with standard convension, please use DMA_TSR11.
*/
#define	DMA_TSR011	(DMA_TSR11)

/** \\brief  1E30, DMA Transaction State Register */
#define DMA_TSR12 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E30u)

/** Alias (User Manual Name) for DMA_TSR12.
* To use register names with standard convension, please use DMA_TSR12.
*/
#define	DMA_TSR012	(DMA_TSR12)

/** \\brief  1E34, DMA Transaction State Register */
#define DMA_TSR13 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E34u)

/** Alias (User Manual Name) for DMA_TSR13.
* To use register names with standard convension, please use DMA_TSR13.
*/
#define	DMA_TSR013	(DMA_TSR13)

/** \\brief  1E38, DMA Transaction State Register */
#define DMA_TSR14 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E38u)

/** Alias (User Manual Name) for DMA_TSR14.
* To use register names with standard convension, please use DMA_TSR14.
*/
#define	DMA_TSR014	(DMA_TSR14)

/** \\brief  1E3C, DMA Transaction State Register */
#define DMA_TSR15 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E3Cu)

/** Alias (User Manual Name) for DMA_TSR15.
* To use register names with standard convension, please use DMA_TSR15.
*/
#define	DMA_TSR015	(DMA_TSR15)

/** \\brief  1E08, DMA Transaction State Register */
#define DMA_TSR2 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E08u)

/** Alias (User Manual Name) for DMA_TSR2.
* To use register names with standard convension, please use DMA_TSR2.
*/
#define	DMA_TSR002	(DMA_TSR2)

/** \\brief  1E0C, DMA Transaction State Register */
#define DMA_TSR3 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E0Cu)

/** Alias (User Manual Name) for DMA_TSR3.
* To use register names with standard convension, please use DMA_TSR3.
*/
#define	DMA_TSR003	(DMA_TSR3)

/** \\brief  1E10, DMA Transaction State Register */
#define DMA_TSR4 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E10u)

/** Alias (User Manual Name) for DMA_TSR4.
* To use register names with standard convension, please use DMA_TSR4.
*/
#define	DMA_TSR004	(DMA_TSR4)

/** \\brief  1E14, DMA Transaction State Register */
#define DMA_TSR5 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E14u)

/** Alias (User Manual Name) for DMA_TSR5.
* To use register names with standard convension, please use DMA_TSR5.
*/
#define	DMA_TSR005	(DMA_TSR5)

/** \\brief  1E18, DMA Transaction State Register */
#define DMA_TSR6 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E18u)

/** Alias (User Manual Name) for DMA_TSR6.
* To use register names with standard convension, please use DMA_TSR6.
*/
#define	DMA_TSR006	(DMA_TSR6)

/** \\brief  1E1C, DMA Transaction State Register */
#define DMA_TSR7 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E1Cu)

/** Alias (User Manual Name) for DMA_TSR7.
* To use register names with standard convension, please use DMA_TSR7.
*/
#define	DMA_TSR007	(DMA_TSR7)

/** \\brief  1E20, DMA Transaction State Register */
#define DMA_TSR8 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E20u)

/** Alias (User Manual Name) for DMA_TSR8.
* To use register names with standard convension, please use DMA_TSR8.
*/
#define	DMA_TSR008	(DMA_TSR8)

/** \\brief  1E24, DMA Transaction State Register */
#define DMA_TSR9 /*lint --e(923)*/ (*(volatile Ifx_DMA_TSR*)0xF0011E24u)

/** Alias (User Manual Name) for DMA_TSR9.
* To use register names with standard convension, please use DMA_TSR9.
*/
#define	DMA_TSR009	(DMA_TSR9)
/** \}  */
/******************************************************************************/
/******************************************************************************/
#endif /* IFXDMA_REG_H */
