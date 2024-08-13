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
* File Name    : r_cg_port.h
* Version      : 1.2.3
* Device(s)    : R5F565NEHxFP
* Description  : General header file for PORT peripheral.
* Creation Date: 2019-04-30
***********************************************************************************************************************/

#ifndef PORT_H
#define PORT_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
Port Mode Register (PMR)
*/
/* Pmn Pin Mode Control (B7 -B0) */
#define _00_Pm0_PIN_GPIO        (0x00U) /* Pm0 as general I/O pin */
#define _00_Pm1_PIN_GPIO        (0x00U) /* Pm1 as general I/O pin */
#define _00_Pm2_PIN_GPIO        (0x00U) /* Pm2 as general I/O pin */
#define _00_Pm3_PIN_GPIO        (0x00U) /* Pm3 as general I/O pin */
#define _00_Pm4_PIN_GPIO        (0x00U) /* Pm4 as general I/O pin */
#define _00_Pm5_PIN_GPIO        (0x00U) /* Pm5 as general I/O pin */
#define _00_Pm6_PIN_GPIO        (0x00U) /* Pm6 as general I/O pin */
#define _00_Pm7_PIN_GPIO        (0x00U) /* Pm7 as general I/O pin */

/*
    Port Direction Register (PDR)
*/
/* Pmn Direction Control (B7 - B0) */
#define _00_Pm0_MODE_INPUT      (0x00U) /* Pm0 as input */
#define _01_Pm0_MODE_OUTPUT     (0x01U) /* Pm0 as output */
#define _00_Pm1_MODE_INPUT      (0x00U) /* Pm1 as input */
#define _02_Pm1_MODE_OUTPUT     (0x02U) /* Pm1 as output */
#define _00_Pm2_MODE_INPUT      (0x00U) /* Pm2 as input */
#define _04_Pm2_MODE_OUTPUT     (0x04U) /* Pm2 as output */
#define _00_Pm3_MODE_INPUT      (0x00U) /* Pm3 as input */
#define _08_Pm3_MODE_OUTPUT     (0x08U) /* Pm3 as output */
#define _00_Pm4_MODE_INPUT      (0x00U) /* Pm4 as input */
#define _10_Pm4_MODE_OUTPUT     (0x10U) /* Pm4 as output */
#define _00_Pm5_MODE_INPUT      (0x00U) /* Pm5 as input */
#define _20_Pm5_MODE_OUTPUT     (0x20U) /* Pm5 as output */
#define _00_Pm6_MODE_INPUT      (0x00U) /* Pm6 as input */
#define _40_Pm6_MODE_OUTPUT     (0x40U) /* Pm6 as output */
#define _00_Pm7_MODE_INPUT      (0x00U) /* Pm7 as input */
#define _80_Pm7_MODE_OUTPUT     (0x80U) /* Pm7 as output */
#define _5F_PDR0_DEFAULT        (0x5FU) /* PDR0 default value */
#define _03_PDR1_DEFAULT        (0x03U) /* PDR1 default value */
#define _C0_PDR5_DEFAULT        (0xC0U) /* PDR5 default value */
#define _FF_PDR6_DEFAULT        (0xFFU) /* PDR6 default value */
#define _FF_PDR7_DEFAULT        (0xFFU) /* PDR7 default value */
#define _FF_PDR8_DEFAULT        (0xFFU) /* PDR8 default value */
#define _FF_PDR9_DEFAULT        (0xFFU) /* PDR9 default value */
#define _FF_PDRF_DEFAULT        (0xFFU) /* PDRF default value */
#define _FF_PDRG_DEFAULT        (0xFFU) /* PDRG default value */
#define _F7_PDRJ_DEFAULT        (0xF7U) /* PDRJ default value */

/*
    Port Output Data Register (PODR)
*/
/* Pmn Output Data Store (B7 - B0) */
#define _00_Pm0_OUTPUT_0        (0x00U) /* output low at B0 */
#define _01_Pm0_OUTPUT_1        (0x01U) /* output high at B0 */
#define _00_Pm1_OUTPUT_0        (0x00U) /* output low at B1 */
#define _02_Pm1_OUTPUT_1        (0x02U) /* output high at B1 */
#define _00_Pm2_OUTPUT_0        (0x00U) /* output low at B2 */
#define _04_Pm2_OUTPUT_1        (0x04U) /* output high at B2 */
#define _00_Pm3_OUTPUT_0        (0x00U) /* output low at B3 */
#define _08_Pm3_OUTPUT_1        (0x08U) /* output high at B3 */
#define _00_Pm4_OUTPUT_0        (0x00U) /* output low at B4 */
#define _10_Pm4_OUTPUT_1        (0x10U) /* output high at B4 */
#define _00_Pm5_OUTPUT_0        (0x00U) /* output low at B5 */
#define _20_Pm5_OUTPUT_1        (0x20U) /* output high at B5 */
#define _00_Pm6_OUTPUT_0        (0x00U) /* output low at B6 */
#define _40_Pm6_OUTPUT_1        (0x40U) /* output high at B6 */
#define _00_Pm7_OUTPUT_0        (0x00U) /* output low at B7 */
#define _80_Pm7_OUTPUT_1        (0x80U) /* output high at B7 */

/*
    Open Drain Control Register 0 (ODR0)
*/
/* Pmn Output Type Select (Pm0 to Pm3) */
#define _00_Pm0_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _01_Pm0_NCH_OPEN_DRAIN  (0x01U) /* N-channel open-drain output */
#define _00_Pm1_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _04_Pm1_NCH_OPEN_DRAIN  (0x04U) /* N-channel open-drain output */
#define _08_Pm1_PCH_OPEN_DRAIN  (0x08U) /* P-channel open-drain output, for PE1 only*/
#define _00_Pm2_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _10_Pm2_NCH_OPEN_DRAIN  (0x10U) /* N-channel open-drain output */
#define _00_Pm3_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _40_Pm3_NCH_OPEN_DRAIN  (0x40U) /* N-channel open-drain output */

/*
    Open Drain Control Register 1 (ODR1)
*/
/* Pmn Output Type Select (Pm4 to Pm7) */
#define _00_Pm4_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _01_Pm4_NCH_OPEN_DRAIN  (0x01U) /* N-channel open-drain output */
#define _00_Pm5_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _04_Pm5_NCH_OPEN_DRAIN  (0x04U) /* N-channel open-drain output */
#define _00_Pm6_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _10_Pm6_NCH_OPEN_DRAIN  (0x10U) /* N-channel open-drain output */
#define _00_Pm7_CMOS_OUTPUT     (0x00U) /* CMOS output */
#define _40_Pm7_NCH_OPEN_DRAIN  (0x40U) /* N-channel open-drain output */

/*
    Pull-Up Control Register (PCR)
*/
/* Pmn Input Pull-Up Resistor Control (B7 - B0) */
#define _00_Pm0_PULLUP_OFF      (0x00U) /* Pn0 pull-up resistor not connected */
#define _01_Pm0_PULLUP_ON       (0x01U) /* Pn0 pull-up resistor connected */
#define _00_Pm1_PULLUP_OFF      (0x00U) /* Pn1 pull-up resistor not connected */
#define _02_Pm1_PULLUP_ON       (0x02U) /* Pn1 pull-up resistor connected */
#define _00_Pm2_PULLUP_OFF      (0x00U) /* Pn2 Pull-up resistor not connected */
#define _04_Pm2_PULLUP_ON       (0x04U) /* Pn2 pull-up resistor connected */
#define _00_Pm3_PULLUP_OFF      (0x00U) /* Pn3 pull-up resistor not connected */
#define _08_Pm3_PULLUP_ON       (0x08U) /* Pn3 pull-up resistor connected */
#define _00_Pm4_PULLUP_OFF      (0x00U) /* Pn4 pull-up resistor not connected */
#define _10_Pm4_PULLUP_ON       (0x10U) /* Pn4 pull-up resistor connected */
#define _00_Pm5_PULLUP_OFF      (0x00U) /* Pn5 pull-up resistor not connected */
#define _20_Pm5_PULLUP_ON       (0x20U) /* Pn5 pull-up resistor connected */
#define _00_Pm6_PULLUP_OFF      (0x00U) /* Pn6 pull-up resistor not connected */
#define _40_Pm6_PULLUP_ON       (0x40U) /* Pn6 pull-up resistor connected */
#define _00_Pm7_PULLUP_OFF      (0x00U) /* Pn7 pull-up resistor not connected */
#define _80_Pm7_PULLUP_ON       (0x80U) /* Pn7 pull-up resistor connected */

/*
Drive Capacity Control Register (DSCR)
*/
/* Pmn Drive Capacity Control (B7 - B0) */
#define _00_Pm0_HIDRV_OFF       (0x00U) /* Pm0 Normal drive output */
#define _01_Pm0_HIDRV_ON        (0x01U) /* Pm0 High-drive output */
#define _00_Pm1_HIDRV_OFF       (0x00U) /* Pm1 Normal drive output */
#define _02_Pm1_HIDRV_ON        (0x02U) /* Pm1 High-drive output */
#define _00_Pm2_HIDRV_OFF       (0x00U) /* Pm2 Normal drive output */
#define _04_Pm2_HIDRV_ON        (0x04U) /* Pm2 High-drive output */
#define _00_Pm3_HIDRV_OFF       (0x00U) /* Pm3 Normal drive output */
#define _08_Pm3_HIDRV_ON        (0x08U) /* Pm3 High-drive output */
#define _00_Pm4_HIDRV_OFF       (0x00U) /* Pm4 Normal drive output */
#define _10_Pm4_HIDRV_ON        (0x10U) /* Pm4 High-drive output */
#define _00_Pm5_HIDRV_OFF       (0x00U) /* Pm5 Normal drive output */
#define _20_Pm5_HIDRV_ON        (0x20U) /* Pm5 High-drive output */
#define _00_Pm6_HIDRV_OFF       (0x00U) /* Pm6 Normal drive output */
#define _40_Pm6_HIDRV_ON        (0x40U) /* Pm6 High-drive output */
#define _00_Pm7_HIDRV_OFF       (0x00U) /* Pm7 Normal drive output */
#define _80_Pm7_HIDRV_ON        (0x80U) /* Pm7 High-drive output */

/*
     Drive Capacity Control Register 2 (DSCR2)
*/
/* Pmn Drive Capacity Control 2 (B7 - B0) */
#define _00_Pm0_HISPEED_OFF     (0x00U) /* Pm0 Normal drive/high-drive output */
#define _01_Pm0_HISPEED_ON      (0x01U) /* Pm0 High-speed interface high-drive output */
#define _00_Pm1_HISPEED_OFF     (0x00U) /* Pm1 Normal drive/high-drive output */
#define _02_Pm1_HISPEED_ON      (0x02U) /* Pm1 High-speed interface high-drive output */
#define _00_Pm2_HISPEED_OFF     (0x00U) /* Pm2 Normal drive/high-drive output */
#define _04_Pm2_HISPEED_ON      (0x04U) /* Pm2 High-speed interface high-drive output */
#define _00_Pm3_HISPEED_OFF     (0x00U) /* Pm3 Normal drive/high-drive output */
#define _08_Pm3_HISPEED_ON      (0x08U) /* Pm3 High-speed interface high-drive output */
#define _00_Pm4_HISPEED_OFF     (0x00U) /* Pm4 Normal drive/high-drive output */
#define _10_Pm4_HISPEED_ON      (0x10U) /* Pm4 High-speed interface high-drive output */
#define _00_Pm5_HISPEED_OFF     (0x00U) /* Pm5 Normal drive/high-drive output */
#define _20_Pm5_HISPEED_ON      (0x20U) /* Pm5 High-speed interface high-drive output */
#define _00_Pm6_HISPEED_OFF     (0x00U) /* Pm6 Normal drive/high-drive output */
#define _40_Pm6_HISPEED_ON      (0x40U) /* Pm6 High-speed interface high-drive output */
#define _00_Pm7_HISPEED_OFF     (0x00U) /* Pm7 Normal drive/high-drive output */
#define _80_Pm7_HISPEED_ON      (0x80U) /* Pm7 High-speed interface high-drive output */

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
