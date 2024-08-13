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
* File Name    : r_cg_ppg.h
* Version      : 1.2.3
* Device(s)    : R5F565NEHxFP
* Description  : General header file for PPG peripheral.
* Creation Date: 2019-04-30
***********************************************************************************************************************/

#ifndef PPG_H
#define PPG_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    PPG Trigger Select Register (PTRSLR)
*/
/* PPG Trigger Select (PTRSL) */
#define _00_PPG1_TRIGGER_SEL_MTU            (0x00U) /* Selects MTU0 to MTU3 as trigger channels for PPG1 */
#define _01_PPG1_TRIGGER_SEL_TPU            (0x01U) /* Selects TPU0 to TPU3 as trigger channels for PPG1 */

/*
    Next Data Enable Registers L (NDERL)
*/
/* PPG0 NDERL */
#define _01_PPG_NEXT_DATA_PO0_ENABLED       (0x01U) /* PO0 Next Data Transfer Enable */
#define _02_PPG_NEXT_DATA_PO1_ENABLED       (0x02U) /* PO1 Next Data Transfer Enable */
#define _04_PPG_NEXT_DATA_PO2_ENABLED       (0x04U) /* PO2 Next Data Transfer Enable */
#define _08_PPG_NEXT_DATA_PO3_ENABLED       (0x08U) /* PO3 Next Data Transfer Enable */
#define _10_PPG_NEXT_DATA_PO4_ENABLED       (0x10U) /* PO4 Next Data Transfer Enable */
#define _20_PPG_NEXT_DATA_PO5_ENABLED       (0x20U) /* PO5 Next Data Transfer Enable */
#define _40_PPG_NEXT_DATA_PO6_ENABLED       (0x40U) /* PO6 Next Data Transfer Enable */
#define _80_PPG_NEXT_DATA_PO7_ENABLED       (0x80U) /* PO7 Next Data Transfer Enable */
/* PPG1 NDERL */
#define _01_PPG_NEXT_DATA_PO16_ENABLED      (0x01U) /* PO16 Next Data Transfer Enable */
#define _02_PPG_NEXT_DATA_PO17_ENABLED      (0x02U) /* PO17 Next Data Transfer Enable */
#define _04_PPG_NEXT_DATA_PO18_ENABLED      (0x04U) /* PO18 Next Data Transfer Enable */
#define _08_PPG_NEXT_DATA_PO19_ENABLED      (0x08U) /* PO19 Next Data Transfer Enable */
#define _10_PPG_NEXT_DATA_PO20_ENABLED      (0x10U) /* PO20 Next Data Transfer Enable */
#define _20_PPG_NEXT_DATA_PO21_ENABLED      (0x20U) /* PO21 Next Data Transfer Enable */
#define _40_PPG_NEXT_DATA_PO22_ENABLED      (0x40U) /* PO22 Next Data Transfer Enable */
#define _80_PPG_NEXT_DATA_PO23_ENABLED      (0x80U) /* PO23 Next Data Transfer Enable */

/*
    Next Data Enable Registers H (NDERH)
*/
/* PPG0 NDERH */
#define _01_PPG_NEXT_DATA_PO8_ENABLED       (0x01U) /* PO8 Next Data Transfer Enable */
#define _02_PPG_NEXT_DATA_PO9_ENABLED       (0x02U) /* PO9 Next Data Transfer Enable */
#define _04_PPG_NEXT_DATA_PO10_ENABLED      (0x04U) /* PO10 Next Data Transfer Enable */
#define _08_PPG_NEXT_DATA_PO11_ENABLED      (0x08U) /* PO11 Next Data Transfer Enable */
#define _10_PPG_NEXT_DATA_PO12_ENABLED      (0x10U) /* PO12 Next Data Transfer Enable */
#define _20_PPG_NEXT_DATA_PO13_ENABLED      (0x20U) /* PO13 Next Data Transfer Enable */
#define _40_PPG_NEXT_DATA_PO14_ENABLED      (0x40U) /* PO14 Next Data Transfer Enable */
#define _80_PPG_NEXT_DATA_PO15_ENABLED      (0x80U) /* PO15 Next Data Transfer Enable */
/* PPG1 NDERH */
#define _01_PPG_NEXT_DATA_PO24_ENABLED      (0x01U) /* PO24 Next Data Transfer Enable */
#define _02_PPG_NEXT_DATA_PO25_ENABLED      (0x02U) /* PO25 Next Data Transfer Enable */
#define _04_PPG_NEXT_DATA_PO26_ENABLED      (0x04U) /* PO26 Next Data Transfer Enable */
#define _08_PPG_NEXT_DATA_PO27_ENABLED      (0x08U) /* PO27 Next Data Transfer Enable */
#define _10_PPG_NEXT_DATA_PO28_ENABLED      (0x10U) /* PO28 Next Data Transfer Enable */
#define _20_PPG_NEXT_DATA_PO29_ENABLED      (0x20U) /* PO29 Next Data Transfer Enable */
#define _40_PPG_NEXT_DATA_PO30_ENABLED      (0x40U) /* PO30 Next Data Transfer Enable */
#define _80_PPG_NEXT_DATA_PO31_ENABLED      (0x80U) /* PO31 Next Data Transfer Enable */

/*
    Output Data Registers / Next Data Registers L (PODRL/NDRL)
*/
/* PPG0 PODRL/NDRL */
#define _00_PPG_OUTPUT_DATA_PO0_LOW         (0x00U) /* PO0 low level is output */
#define _01_PPG_OUTPUT_DATA_PO0_HIGH        (0x01U) /* PO0 high level is output */
#define _00_PPG_OUTPUT_DATA_PO1_LOW         (0x00U) /* PO1 low level is output */
#define _02_PPG_OUTPUT_DATA_PO1_HIGH        (0x02U) /* PO1 high level is output */
#define _00_PPG_OUTPUT_DATA_PO2_LOW         (0x00U) /* PO2 low level is output */
#define _04_PPG_OUTPUT_DATA_PO2_HIGH        (0x04U) /* PO2 high level is output */
#define _00_PPG_OUTPUT_DATA_PO3_LOW         (0x00U) /* PO3 low level is output */
#define _08_PPG_OUTPUT_DATA_PO3_HIGH        (0x08U) /* PO3 high level is output */
#define _00_PPG_OUTPUT_DATA_PO4_LOW         (0x00U) /* PO4 low level is output */
#define _10_PPG_OUTPUT_DATA_PO4_HIGH        (0x10U) /* PO4 high level is output */
#define _00_PPG_OUTPUT_DATA_PO5_LOW         (0x00U) /* PO5 low level is output */
#define _20_PPG_OUTPUT_DATA_PO5_HIGH        (0x20U) /* PO5 high level is output */
#define _00_PPG_OUTPUT_DATA_PO6_LOW         (0x00U) /* PO6 low level is output */
#define _40_PPG_OUTPUT_DATA_PO6_HIGH        (0x40U) /* PO6 high level is output */
#define _00_PPG_OUTPUT_DATA_PO7_LOW         (0x00U) /* PO7 low level is output */
#define _80_PPG_OUTPUT_DATA_PO7_HIGH        (0x80U) /* PO7 high level is output */
/* PPG1 PODRL/NDRL */
#define _00_PPG_OUTPUT_DATA_PO16_LOW        (0x00U) /* PO16 low level is output */
#define _01_PPG_OUTPUT_DATA_PO16_HIGH       (0x01U) /* PO16 high level is output */
#define _00_PPG_OUTPUT_DATA_PO17_LOW        (0x00U) /* PO17 low level is output */
#define _02_PPG_OUTPUT_DATA_PO17_HIGH       (0x02U) /* PO17 high level is output */
#define _00_PPG_OUTPUT_DATA_PO18_LOW        (0x00U) /* PO18 low level is output */
#define _04_PPG_OUTPUT_DATA_PO18_HIGH       (0x04U) /* PO18 high level is output */
#define _00_PPG_OUTPUT_DATA_PO19_LOW        (0x00U) /* PO19 low level is output */
#define _08_PPG_OUTPUT_DATA_PO19_HIGH       (0x08U) /* PO19 high level is output */
#define _00_PPG_OUTPUT_DATA_PO20_LOW        (0x00U) /* PO20 low level is output */
#define _10_PPG_OUTPUT_DATA_PO20_HIGH       (0x10U) /* PO20 high level is output */
#define _00_PPG_OUTPUT_DATA_PO21_LOW        (0x00U) /* PO21 low level is output */
#define _20_PPG_OUTPUT_DATA_PO21_HIGH       (0x20U) /* PO21 high level is output */
#define _00_PPG_OUTPUT_DATA_PO22_LOW        (0x00U) /* PO22 low level is output */
#define _40_PPG_OUTPUT_DATA_PO22_HIGH       (0x40U) /* PO22 high level is output */
#define _00_PPG_OUTPUT_DATA_PO23_LOW        (0x00U) /* PO23 low level is output */
#define _80_PPG_OUTPUT_DATA_PO23_HIGH       (0x80U) /* PO23 high level is output */
#define _0F_PPG_NEXT_DATA_DEFAULT           (0x0FU) /* Write value for b3 to b0 should be 1 */
#define _F0_PPG_NEXT_DATA_2_DEFAULT         (0xF0U) /* Write value for b7 to b4 should be 1 */

/*
    Output Data Registers / Next Data Registers H (PODRH/NDRH)
*/
/* PPG0 PODRH/NDRH */
#define _00_PPG_OUTPUT_DATA_PO8_LOW         (0x00U) /* PO8 low level is output */
#define _01_PPG_OUTPUT_DATA_PO8_HIGH        (0x01U) /* PO8 high level is output */
#define _00_PPG_OUTPUT_DATA_PO9_LOW         (0x00U) /* PO9 low level is output */
#define _02_PPG_OUTPUT_DATA_PO9_HIGH        (0x02U) /* PO9 high level is output */
#define _00_PPG_OUTPUT_DATA_PO10_LOW        (0x00U) /* PO10 low level is output */
#define _04_PPG_OUTPUT_DATA_PO10_HIGH       (0x04U) /* PO10 high level is output */
#define _00_PPG_OUTPUT_DATA_PO11_LOW        (0x00U) /* PO11 low level is output */
#define _08_PPG_OUTPUT_DATA_PO11_HIGH       (0x08U) /* PO11 high level is output */
#define _00_PPG_OUTPUT_DATA_PO12_LOW        (0x00U) /* PO12 low level is output */
#define _10_PPG_OUTPUT_DATA_PO12_HIGH       (0x10U) /* PO12 high level is output */
#define _00_PPG_OUTPUT_DATA_PO13_LOW        (0x00U) /* PO13 low level is output */
#define _20_PPG_OUTPUT_DATA_PO13_HIGH       (0x20U) /* PO13 high level is output */
#define _00_PPG_OUTPUT_DATA_PO14_LOW        (0x00U) /* PO14 low level is output */
#define _40_PPG_OUTPUT_DATA_PO14_HIGH       (0x40U) /* PO14 high level is output */
#define _00_PPG_OUTPUT_DATA_PO15_LOW        (0x00U) /* PO15 low level is output */
#define _80_PPG_OUTPUT_DATA_PO15_HIGH       (0x80U) /* PO15 high level is output */
/* PPG1 PODRH/NDRH */
#define _00_PPG_OUTPUT_DATA_PO24_LOW        (0x00U) /* PO24 low level is output */
#define _01_PPG_OUTPUT_DATA_PO24_HIGH       (0x01U) /* PO24 high level is output */
#define _00_PPG_OUTPUT_DATA_PO25_LOW        (0x00U) /* PO25 low level is output */
#define _02_PPG_OUTPUT_DATA_PO25_HIGH       (0x02U) /* PO25 high level is output */
#define _00_PPG_OUTPUT_DATA_PO26_LOW        (0x00U) /* PO26 low level is output */
#define _04_PPG_OUTPUT_DATA_PO26_HIGH       (0x04U) /* PO26 high level is output */
#define _00_PPG_OUTPUT_DATA_PO27_LOW        (0x00U) /* PO27 low level is output */
#define _08_PPG_OUTPUT_DATA_PO27_HIGH       (0x08U) /* PO27 high level is output */
#define _00_PPG_OUTPUT_DATA_PO28_LOW        (0x00U) /* PO28 low level is output */
#define _10_PPG_OUTPUT_DATA_PO28_HIGH       (0x10U) /* PO28 high level is output */
#define _00_PPG_OUTPUT_DATA_PO29_LOW        (0x00U) /* PO29 low level is output */
#define _20_PPG_OUTPUT_DATA_PO29_HIGH       (0x20U) /* PO29 high level is output */
#define _00_PPG_OUTPUT_DATA_PO30_LOW        (0x00U) /* PO30 low level is output */
#define _40_PPG_OUTPUT_DATA_PO30_HIGH       (0x40U) /* PO30 high level is output */
#define _00_PPG_OUTPUT_DATA_PO31_LOW        (0x00U) /* PO31 low level is output */
#define _80_PPG_OUTPUT_DATA_PO31_HIGH       (0x80U) /* PO31 high level is output */

/*
    PPG Output Control Register (PCR)
*/
/* Group 0 Compare Match Select G0CMS[1:0] */
#define _00_PPG_GROUP0_COMPARE_MTU0         (0x00U) /* Compare match in MTU0 */
#define _01_PPG_GROUP0_COMPARE_MTU1         (0x01U) /* Compare match in MTU1 */
#define _02_PPG_GROUP0_COMPARE_MTU2         (0x02U) /* Compare match in MTU2 */
#define _03_PPG_GROUP0_COMPARE_MTU3         (0x03U) /* Compare match in MTU3 */
/* Group 1 Compare Match Select G1CMS[1:0] */ 
#define _00_PPG_GROUP1_COMPARE_MTU0         (0x00U) /* Compare match in MTU0 */
#define _04_PPG_GROUP1_COMPARE_MTU1         (0x04U) /* Compare match in MTU1 */
#define _08_PPG_GROUP1_COMPARE_MTU2         (0x08U) /* Compare match in MTU2 */
#define _0C_PPG_GROUP1_COMPARE_MTU3         (0x0CU) /* Compare match in MTU3 */
/* Group 2 Compare Match Select G2CMS[1:0] */ 
#define _00_PPG_GROUP2_COMPARE_MTU0         (0x00U) /* Compare match in MTU0 */
#define _10_PPG_GROUP2_COMPARE_MTU1         (0x10U) /* Compare match in MTU1 */
#define _20_PPG_GROUP2_COMPARE_MTU2         (0x20U) /* Compare match in MTU2 */
#define _30_PPG_GROUP2_COMPARE_MTU3         (0x30U) /* Compare match in MTU3 */
/* Group 3 Compare Match Select G3CMS[1:0] */ 
#define _00_PPG_GROUP3_COMPARE_MTU0         (0x00U) /* Compare match in MTU0 */
#define _40_PPG_GROUP3_COMPARE_MTU1         (0x40U) /* Compare match in MTU1 */
#define _80_PPG_GROUP3_COMPARE_MTU2         (0x80U) /* Compare match in MTU2 */
#define _C0_PPG_GROUP3_COMPARE_MTU3         (0xC0U) /* Compare match in MTU3 */
/* Group 4 Compare Match Select G4CMS[1:0] */
#define _00_PPG_GROUP4_COMPARE_MTU0         (0x00U) /* Compare match in MTU0/TPU0 */
#define _01_PPG_GROUP4_COMPARE_MTU1         (0x01U) /* Compare match in MTU1/TPU1 */
#define _02_PPG_GROUP4_COMPARE_MTU2         (0x02U) /* Compare match in MTU2/TPU2 */
#define _03_PPG_GROUP4_COMPARE_MTU3         (0x03U) /* Compare match in MTU3/TPU3 */
/* Group 5 Compare Match Select G5CMS[1:0] */ 
#define _00_PPG_GROUP5_COMPARE_MTU0         (0x00U) /* Compare match in MTU0/TPU0 */
#define _04_PPG_GROUP5_COMPARE_MTU1         (0x04U) /* Compare match in MTU1/TPU1 */
#define _08_PPG_GROUP5_COMPARE_MTU2         (0x08U) /* Compare match in MTU2/TPU2 */
#define _0C_PPG_GROUP5_COMPARE_MTU3         (0x0CU) /* Compare match in MTU3/TPU3 */
/* Group 6 Compare Match Select G6CMS[1:0] */ 
#define _00_PPG_GROUP6_COMPARE_MTU0         (0x00U) /* Compare match in MTU0/TPU0 */
#define _10_PPG_GROUP6_COMPARE_MTU1         (0x10U) /* Compare match in MTU1/TPU1 */
#define _20_PPG_GROUP6_COMPARE_MTU2         (0x20U) /* Compare match in MTU2/TPU2 */
#define _30_PPG_GROUP6_COMPARE_MTU3         (0x30U) /* Compare match in MTU3/TPU3 */
/* Group 7 Compare Match Select G7CMS[1:0] */ 
#define _00_PPG_GROUP7_COMPARE_MTU0         (0x00U) /* Compare match in MTU0/TPU0 */
#define _40_PPG_GROUP7_COMPARE_MTU1         (0x40U) /* Compare match in MTU1/TPU1 */
#define _80_PPG_GROUP7_COMPARE_MTU2         (0x80U) /* Compare match in MTU2/TPU2 */
#define _C0_PPG_GROUP7_COMPARE_MTU3         (0xC0U) /* Compare match in MTU3/TPU3 */

/*
    PPG Output Mode Register (PMR)
*/
/* Group n Non-Overlap (GnNOV) */
#define _00_PPG_GROUP0_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _01_PPG_GROUP0_NON_OVERLAP_ENABLE   (0x01U) /* Non-overlapping operation */
#define _00_PPG_GROUP1_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _02_PPG_GROUP1_NON_OVERLAP_ENABLE   (0x02U) /* Non-overlapping operation */
#define _00_PPG_GROUP2_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _04_PPG_GROUP2_NON_OVERLAP_ENABLE   (0x04U) /* Non-overlapping operation */
#define _00_PPG_GROUP3_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _08_PPG_GROUP3_NON_OVERLAP_ENABLE   (0x08U) /* Non-overlapping operation */
#define _00_PPG_GROUP4_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _01_PPG_GROUP4_NON_OVERLAP_ENABLE   (0x01U) /* Non-overlapping operation */
#define _00_PPG_GROUP5_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _02_PPG_GROUP5_NON_OVERLAP_ENABLE   (0x02U) /* Non-overlapping operation */
#define _00_PPG_GROUP6_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _04_PPG_GROUP6_NON_OVERLAP_ENABLE   (0x04U) /* Non-overlapping operation */
#define _00_PPG_GROUP7_NON_OVERLAP_DISABLE  (0x00U) /* Normal operation */
#define _08_PPG_GROUP7_NON_OVERLAP_ENABLE   (0x08U) /* Non-overlapping operation */
/* Group n Output Polarity Change (GnINV) */
#define _00_PPG_GROUP0_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _10_PPG_GROUP0_POLARITY_DIRECT      (0x10U) /* Direct output */
#define _00_PPG_GROUP1_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _20_PPG_GROUP1_POLARITY_DIRECT      (0x20U) /* Direct output */
#define _00_PPG_GROUP2_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _40_PPG_GROUP2_POLARITY_DIRECT      (0x40U) /* Direct output */
#define _00_PPG_GROUP3_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _80_PPG_GROUP3_POLARITY_DIRECT      (0x80U) /* Direct output */
#define _00_PPG_GROUP4_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _10_PPG_GROUP4_POLARITY_DIRECT      (0x10U) /* Direct output */
#define _00_PPG_GROUP5_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _20_PPG_GROUP5_POLARITY_DIRECT      (0x20U) /* Direct output */
#define _00_PPG_GROUP6_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _40_PPG_GROUP6_POLARITY_DIRECT      (0x40U) /* Direct output */
#define _00_PPG_GROUP7_POLARITY_INVERT      (0x00U) /* Inverted output */
#define _80_PPG_GROUP7_POLARITY_DIRECT      (0x80U) /* Direct output */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
