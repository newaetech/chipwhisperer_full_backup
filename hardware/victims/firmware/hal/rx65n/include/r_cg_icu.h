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
* File Name    : r_cg_icu.h
* Version      : 1.2.3
* Device(s)    : R5F565NEHxFP
* Description  : General header file for ICU peripheral.
* Creation Date: 2019-04-30
***********************************************************************************************************************/

#ifndef ICU_H
#define ICU_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Interrupt Request Enable Register 08 (IER08)
*/
/* Interrupt Request Enable j (IENj) (j = 0 to 7) */
#define _00_ICU_IRQ0_DISABLE                    (0x00U) /* IRQ0 interrupt request is disabled */
#define _01_ICU_IRQ0_ENABLE                     (0x01U) /* IRQ0 interrupt request is enabled */
#define _00_ICU_IRQ1_DISABLE                    (0x00U) /* IRQ1 interrupt request is disabled */
#define _02_ICU_IRQ1_ENABLE                     (0x02U) /* IRQ1 interrupt request is enabled */
#define _00_ICU_IRQ2_DISABLE                    (0x00U) /* IRQ2 interrupt request is disabled */
#define _04_ICU_IRQ2_ENABLE                     (0x04U) /* IRQ2 interrupt request is enabled */
#define _00_ICU_IRQ3_DISABLE                    (0x00U) /* IRQ3 interrupt request is disabled */
#define _08_ICU_IRQ3_ENABLE                     (0x08U) /* IRQ3 interrupt request is enabled */
#define _00_ICU_IRQ4_DISABLE                    (0x00U) /* IRQ4 interrupt request is disabled */
#define _10_ICU_IRQ4_ENABLE                     (0x10U) /* IRQ4 interrupt request is enabled */
#define _00_ICU_IRQ5_DISABLE                    (0x00U) /* IRQ5 interrupt request is disabled */
#define _20_ICU_IRQ5_ENABLE                     (0x20U) /* IRQ5 interrupt request is enabled */
#define _00_ICU_IRQ6_DISABLE                    (0x00U) /* IRQ6 interrupt request is disabled */
#define _40_ICU_IRQ6_ENABLE                     (0x40U) /* IRQ6 interrupt request is enabled */
#define _00_ICU_IRQ7_DISABLE                    (0x00U) /* IRQ7 interrupt request is disabled */
#define _80_ICU_IRQ7_ENABLE                     (0x80U) /* IRQ7 interrupt request is enabled */

/*
    Interrupt Request Enable Register 09 (IER09)
*/
/* Interrupt Request Enable j (IENj) (j = 0 to 7) */
#define _00_ICU_IRQ8_DISABLE                    (0x00U) /* IRQ8 interrupt request is disabled */
#define _01_ICU_IRQ8_ENABLE                     (0x01U) /* IRQ8 interrupt request is enabled */
#define _00_ICU_IRQ9_DISABLE                    (0x00U) /* IRQ9 interrupt request is disabled */
#define _02_ICU_IRQ9_ENABLE                     (0x02U) /* IRQ9 interrupt request is enabled */
#define _00_ICU_IRQ10_DISABLE                   (0x00U) /* IRQ10 interrupt request is disabled */
#define _04_ICU_IRQ10_ENABLE                    (0x04U) /* IRQ10 interrupt request is enabled */
#define _00_ICU_IRQ11_DISABLE                   (0x00U) /* IRQ11 interrupt request is disabled */
#define _08_ICU_IRQ11_ENABLE                    (0x08U) /* IRQ11 interrupt request is enabled */
#define _00_ICU_IRQ12_DISABLE                   (0x00U) /* IRQ12 interrupt request is disabled */
#define _10_ICU_IRQ12_ENABLE                    (0x10U) /* IRQ12 interrupt request is enabled */
#define _00_ICU_IRQ13_DISABLE                   (0x00U) /* IRQ13 interrupt request is disabled */
#define _20_ICU_IRQ13_ENABLE                    (0x20U) /* IRQ13 interrupt request is enabled */
#define _00_ICU_IRQ14_DISABLE                   (0x00U) /* IRQ14 interrupt request is disabled */
#define _40_ICU_IRQ14_ENABLE                    (0x40U) /* IRQ14 interrupt request is enabled */
#define _00_ICU_IRQ15_DISABLE                   (0x00U) /* IRQ15 interrupt request is disabled */
#define _80_ICU_IRQ15_ENABLE                    (0x80U) /* IRQ15 interrupt request is enabled */

/*
    Interrupt Source Priority Register r (IPRr)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_ICU_PRIORITY_LEVEL0                 (0x00U) /* Level 0 (interrupt disabled) */
#define _01_ICU_PRIORITY_LEVEL1                 (0x01U) /* Level 1 */
#define _02_ICU_PRIORITY_LEVEL2                 (0x02U) /* Level 2 */
#define _03_ICU_PRIORITY_LEVEL3                 (0x03U) /* Level 3 */
#define _04_ICU_PRIORITY_LEVEL4                 (0x04U) /* Level 4 */
#define _05_ICU_PRIORITY_LEVEL5                 (0x05U) /* Level 5 */
#define _06_ICU_PRIORITY_LEVEL6                 (0x06U) /* Level 6 */
#define _07_ICU_PRIORITY_LEVEL7                 (0x07U) /* Level 7 */
#define _08_ICU_PRIORITY_LEVEL8                 (0x08U) /* Level 8 */
#define _09_ICU_PRIORITY_LEVEL9                 (0x09U) /* Level 9 */
#define _0A_ICU_PRIORITY_LEVEL10                (0x0AU) /* Level 10 */
#define _0B_ICU_PRIORITY_LEVEL11                (0x0BU) /* Level 11 */
#define _0C_ICU_PRIORITY_LEVEL12                (0x0CU) /* Level 12 */
#define _0D_ICU_PRIORITY_LEVEL13                (0x0DU) /* Level 13 */
#define _0E_ICU_PRIORITY_LEVEL14                (0x0EU) /* Level 14 */
#define _0F_ICU_PRIORITY_LEVEL15                (0x0FU) /* Level 15 (highest) */

/*
    Fast Interrupt Set Register (FIR)
*/
/* Fast Interrupt Enable (FIEN) */
#define _0000_ICU_FAST_INTERRUPT_DISABLE        (0x0000U) /* Fast interrupt is disabled */
#define _8000_ICU_FAST_INTERRUPT_ENABLE         (0x8000U) /* Fast interrupt is enabled */

/*
    IRQ Control Register i (IRQCRi) (i = 0 to 15)
*/
/* IRQ Detection Select (IRQMD[1:0]) */
#define _00_ICU_IRQ_EDGE_LOW_LEVEL              (0x00U) /* Low level */
#define _04_ICU_IRQ_EDGE_FALLING                (0x04U) /* Falling edge */
#define _08_ICU_IRQ_EDGE_RISING                 (0x08U) /* Rising edge */
#define _0C_ICU_IRQ_EDGE_BOTH                   (0x0CU) /* Rising and falling edges */

/*
    IRQ Pin Digital Filter Enable Register 0 (IRQFLTE0)
*/
/* IRQi Digital Filter Enable (FLTENi) (i = 0 to 7) */
#define _00_ICU_IRQn_FILTER_DISABLE             (0x00U) /* IRQi digital filter is disabled */
#define _01_ICU_IRQ0_FILTER_ENABLE              (0x01U) /* IRQ0 digital filter is enabled */
#define _02_ICU_IRQ1_FILTER_ENABLE              (0x02U) /* IRQ1 digital filter is enabled */
#define _04_ICU_IRQ2_FILTER_ENABLE              (0x04U) /* IRQ2 digital filter is enabled */
#define _08_ICU_IRQ3_FILTER_ENABLE              (0x08U) /* IRQ3 digital filter is enabled */
#define _10_ICU_IRQ4_FILTER_ENABLE              (0x10U) /* IRQ4 digital filter is enabled */
#define _20_ICU_IRQ5_FILTER_ENABLE              (0x20U) /* IRQ5 digital filter is enabled */
#define _40_ICU_IRQ6_FILTER_ENABLE              (0x40U) /* IRQ6 digital filter is enabled */
#define _80_ICU_IRQ7_FILTER_ENABLE              (0x80U) /* IRQ7 digital filter is enabled */

/*
    IRQ Pin Digital Filter Enable Register 1 (IRQFLTE1)
*/
/* IRQi Digital Filter Enable (FLTENi) (i = 8 to 15) */
#define _01_ICU_IRQ8_FILTER_ENABLE              (0x01U) /* IRQ8 digital filter is enabled */
#define _02_ICU_IRQ9_FILTER_ENABLE              (0x02U) /* IRQ9 digital filter is enabled */
#define _04_ICU_IRQ10_FILTER_ENABLE             (0x04U) /* IRQ10 digital filter is enabled */
#define _08_ICU_IRQ11_FILTER_ENABLE             (0x08U) /* IRQ11 digital filter is enabled */
#define _10_ICU_IRQ12_FILTER_ENABLE             (0x10U) /* IRQ12 digital filter is enabled */
#define _20_ICU_IRQ13_FILTER_ENABLE             (0x20U) /* IRQ13 digital filter is enabled */
#define _40_ICU_IRQ14_FILTER_ENABLE             (0x40U) /* IRQ14 digital filter is enabled */
#define _80_ICU_IRQ15_FILTER_ENABLE             (0x80U) /* IRQ15 digital filter is enabled */

/*
    IRQ Pin Digital Filter Setting Register 0 and 1 (IRQFLTC0 and IRQFLTC1)
*/
/* IRQi Digital Filter Sampling Clock (FCLKSELi) (i = 0 to 15) */
#define _0000_ICU_IRQ_FILTER_PCLK               (0x0000U) /* IRQi sample clock is run at every PCLK cycle */
#define _0001_ICU_IRQ_FILTER_PCLK_8             (0x0001U) /* IRQi sample clock is run at every PCLK/8 cycle */
#define _0002_ICU_IRQ_FILTER_PCLK_32            (0x0002U) /* IRQi sample clock is run at every PCLK/32 cycle */
#define _0003_ICU_IRQ_FILTER_PCLK_64            (0x0003U) /* IRQi sample clock is run at every PCLK/64 cycle */

/*
    NMI Pin Interrupt Control Register (NMICR)
*/
/* NMI Detection Set (NMIMD) */
#define _00_ICU_NMI_EDGE_FALLING                (0x00U) /* Falling edge */
#define _08_ICU_NMI_EDGE_RISING                 (0x08U) /* Rising edge */

/*
    NMI Pin Digital Filter Setting Register (NMIFLTC)
*/
/* NMI Digital Filter Sampling Clock (NFCLKSEL[1:0]) */
#define _00_ICU_NMI_FILTER_PCLK                 (0x00U) /* NMI sample clock is run at every PCLK cycle */
#define _01_ICU_NMI_FILTER_PCLK_8               (0x01U) /* NMI sample clock is run at every PCLK/8 cycle */
#define _02_ICU_NMI_FILTER_PCLK_32              (0x02U) /* NMI sample clock is run at every PCLK/32 cycle */
#define _03_ICU_NMI_FILTER_PCLK_64              (0x03U) /* NMI sample clock is run at every PCLK/64 cycle */

/*
    EXDMAC Activation Peripheral Interrupt Select Register (SELEXDR)
*/
/* EXDMAC0 Trigger Select (SELEXD0) */
#define _00_ICU_EXDMAC0_SLIBR144                (0x00U) /* Interrupt source selected in SLIBR144 triggers EXDMAC0 */
#define _01_ICU_EXDMAC0_SLIAR208                (0x01U) /* Interrupt source selected in SLIAR208 triggers EXDMAC0 */
/* EXDMAC1 Trigger Select (SELEXD1) */
#define _00_ICU_EXDMAC1_SLIBR145                (0x00U) /* Interrupt source selected in SLIBR145 triggers EXDMAC1 */
#define _02_ICU_EXDMAC1_SLIAR209                (0x02U) /* Interrupt source selected in SLIAR209 triggers EXDMAC1 */

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

