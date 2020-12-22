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
* File Name    : r_cg_mtu3.h
* Version      : 1.2.3
* Device(s)    : R5F565NEHxFP
* Description  : General header file for MTU3 peripheral.
* Creation Date: 2019-04-30
***********************************************************************************************************************/

#ifndef MTU3_H
#define MTU3_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Timer Control Register (TCR)
*/
/* Time Pre-scaler Select (TPSC[2:0]) */
#define _00_MTU_PCLK_1                  (0x00U) /* Internal clock: counts on PCLK/1 (for all channels) */
#define _01_MTU_PCLK_4                  (0x01U) /* Internal clock: counts on PCLK/4 (for all channels) */
#define _02_MTU_PCLK_16                 (0x02U) /* Internal clock: counts on PCLK/16 (for all channels) */
#define _03_MTU_PCLK_64                 (0x03U) /* Internal clock: counts on PCLK/64 (for all channels) */
#define _04_MTU_PCLK_256                (0x04U) /* Internal clock: counts on PCLK/256 (for MTU3,4,6,7,8) */
#define _06_MTU_PCLK_256                (0x06U) /* Internal clock: counts on PCLK/256 (for MTU1) */
#define _05_MTU_PCLK_1024               (0x05U) /* Internal clock: counts on PCLK/1024 (for MTU3,4,6,7,8) */
#define _07_MTU_PCLK_1024               (0x07U) /* Internal clock: counts on PCLK/1024 (for MTU2) */
#define _07_MTU_COUNT                   (0x07U) /* Counts on MTU2.TCNT overflow/underflow (for MTU1) */
#define _04_MTU_MTCLKA                  (0x04U) /* External clock: counts on MTCLKA pin input (for MTU0,1,2) */
#define _06_MTU_MTCLKA                  (0x06U) /* External clock: counts on MTCLKA pin input (for MTU3,4,6,7,8) */
#define _05_MTU_MTCLKB                  (0x05U) /* External clock: counts on MTCLKB pin input (for MTU0,1,2) */
#define _07_MTU_MTCLKB                  (0x07U) /* External clock: counts on MTCLKB pin input (for MTU3,4,6,7,8) */
#define _06_MTU_MTCLKC                  (0x06U) /* External clock: counts on MTCLKC pin input (for MTU0,2) */
#define _07_MTU_MTCLKD                  (0x07U) /* External clock: counts on MTCLKD pin input (for MTU0) */
/* Clock Edge Select (CKEG[1:0]) */
#define _00_MTU_CKEG_RISE               (0x00U) /* Count at rising edge */
#define _08_MTU_CKEG_FALL               (0x08U) /* Count at falling edge */
#define _10_MTU_CKEG_BOTH               (0x10U) /* Count at both edges */
/* Counter Clear Select (CCLR[2:0]) */
#define _00_MTU_CKCL_DIS                (0x00U) /* TCNT clearing disabled */
#define _20_MTU_CKCL_A                  (0x20U) /* TCNT cleared by TGRA compare match/input capture */
#define _40_MTU_CKCL_B                  (0x40U) /* TCNT cleared by TGRB compare match/input capture */
#define _60_MTU_CKCL_SYN                (0x60U) /* TCNT cleared by counter clearing in another synchronous channel */
#define _A0_MTU_CKCL_C                  (0xA0U) /* TCNT cleared by TGRC compare match/input capture */
#define _C0_MTU_CKCL_D                  (0xC0U) /* TCNT cleared by TGRD compare match/input capture */

/*
    Timer Control Register 2 (TCR2)
*/
/* Time Pre-scaler Select (TPSC2[2:0]) */
#define _01_MTU_PCLK_2                  (0x01U) /* Internal clock: counts on PCLK/2 (for all channels) */
#define _02_MTU_PCLK_8                  (0x02U) /* Internal clock: counts on PCLK/8 (for all channels) */
#define _03_MTU_PCLK_32                 (0x03U) /* Internal clock: counts on PCLK/32 (for all channels) */
#define _04_MTU_PCLK_256                (0x04U) /* Internal clock: counts on PCLK/256 (for MTU0,2,5) */
#define _04_MTU_PCLK_1024               (0x04U) /* Internal clock: counts on PCLK/1024 (for MTU1) */
#define _05_MTU_PCLK_1024               (0x05U) /* Internal clock: counts on PCLK/1024 (for MTU0,5) */
#define _07_MTU_MTIOC1A                 (0x07U) /* External clock: counts on MTIOC1A pin output (for MTU0,5) */
/* Phase Counting Mode Function Expansion Control (PCB[1:0]) */
#define _00_MTU_PCB_00                  (0x00U) /* Count condition 1 of phase counting mode 2,3,5 */
#define _08_MTU_PCB_01                  (0x08U) /* Count condition 2 of phase counting mode 2,3 */
#define _10_MTU_PCB_10                  (0x10U) /* Count condition 2 of phase counting mode 5 */
#define _18_MTU_PCB_11                  (0x18U) /* Count condition 3 of phase counting mode 2,3 */

/*
    Timer Mode Register 1 (TMDR1)
*/
/* Mode Select (MD[3:0]) */
#define _00_MTU_NORMAL                  (0x00U) /* Normal mode */
#define _02_MTU_PWM1                    (0x02U) /* PWM mode 1 */
#define _03_MTU_PWM2                    (0x03U) /* PWM mode 2 */
#define _04_MTU_COT1                    (0x04U) /* Phase counting mode 1 */
#define _05_MTU_COT2                    (0x05U) /* Phase counting mode 2 */
#define _06_MTU_COT3                    (0x06U) /* Phase counting mode 3 */
#define _07_MTU_COT4                    (0x07U) /* Phase counting mode 4 */
#define _08_MTU_REST                    (0x08U) /* Reset-synchronized PWM mode */
#define _09_MTU_COT5                    (0x09U) /* Phase counting mode 5 */
#define _0D_MTU_CMT1                    (0x0DU) /* Complementary PWM mode 1 (transfer at crest) */
#define _0E_MTU_CMT2                    (0x0EU) /* Complementary PWM mode 2 (transfer at trough) */
#define _0F_MTU_CMT3                    (0x0FU) /* Complementary PWM mode 3 (transfer at crest and trough) */
/* Buffer Operation A (BFA) */
#define _00_MTU_BFA_NORMAL              (0x00U) /* TGRA and TGRC operate normally */
#define _10_MTU_BFA_BUFFER              (0x10U) /* TGRA and TGRC used together for buffer operation */
/* Buffer Operation B (BFB) */
#define _00_MTU_BFB_NORMAL              (0x00U) /* TGRB and TGRD operate normally */
#define _20_MTU_BFB_BUFFER              (0x20U) /* TGRB and TGRD used together for buffer operation */
/* Buffer Operation E (BFE) */
#define _00_MTU_BFE_NORMAL              (0x00U) /* MTU0.TGRE and MTU0.TGRF operate normally */
#define _40_MTU_BFE_BUFFER              (0x40U) /* MTU0.TGRE and MTU0.TGRF used together for buffer operation */

/*
    Timer Mode Registers 2 (TMDR2A and TMDR2B)
*/
/* Double Buffer Select (DRS) */
#define _00_MTU_DRS_DISABLE             (0x00U) /* Double buffer function is disabled */
#define _01_MTU_DRS_ENABLE              (0x01U) /* Double buffer function is enabled */

/*
    Timer I/O Control Register (TIOR)
*/
/* I/O Control A (IOA[3:0]) */
#define _00_MTU_IOA_DISABLE             (0x00U) /* Output prohibited */
#define _01_MTU_IOA_LL                  (0x01U) /* Initial output is low. Low output at compare match */
#define _02_MTU_IOA_LH                  (0x02U) /* Initial output is low. High output at compare match */
#define _03_MTU_IOA_LT                  (0x03U) /* Initial output is low. Toggle output at compare match */
#define _05_MTU_IOA_HL                  (0x05U) /* Initial output is high. Low output at compare match */
#define _06_MTU_IOA_HH                  (0x06U) /* Initial output is high. High output at compare match */
#define _07_MTU_IOA_HT                  (0x07U) /* Initial output is high. Toggle output at compare match */
#define _08_MTU_IOA_IR                  (0x08U) /* Input capture at rising edge */
#define _09_MTU_IOA_IF                  (0x09U) /* Input capture at falling edge */
#define _0A_MTU_IOA_IB                  (0x0AU) /* Input capture at both edges */
#define _0C_MTU_IOA_EX                  (0x0CU) /* Input capture at MTU1.TCNT up-count/down-count */
#define _0D_MTU_IOA_TGRA                (0x0DU) /* Input capture at MTU0.TGRA compare match/input capture */
#define _0E_MTU_IOA_TGRC                (0x0EU) /* Input capture at MTU8.TGRC compare match */
/* I/O Control B (IOB[3:0]) */
#define _00_MTU_IOB_DISABLE             (0x00U) /* Output prohibited */
#define _10_MTU_IOB_LL                  (0x10U) /* Initial output is low. Low output at compare match */
#define _20_MTU_IOB_LH                  (0x20U) /* Initial output is low. High output at compare match */
#define _30_MTU_IOB_LT                  (0x30U) /* Initial output is low. Toggle output at compare match */
#define _50_MTU_IOB_HL                  (0x50U) /* Initial output is high. Low output at compare match */
#define _60_MTU_IOB_HH                  (0x60U) /* Initial output is high. High output at compare match */
#define _70_MTU_IOB_HT                  (0x70U) /* Initial output is high. Toggle output at compare match */
#define _80_MTU_IOB_IR                  (0x80U) /* Input capture at rising edge */
#define _90_MTU_IOB_IF                  (0x90U) /* Input capture at falling edge */
#define _A0_MTU_IOB_IB                  (0xA0U) /* Input capture at both edges */
#define _C0_MTU_IOB_EX                  (0xC0U) /* Input capture at MTU1.TCNT up-count/down-count */
#define _C0_MTU_IOB_TGRC0               (0xC0U) /* Input capture at MTU0.TGRC compare match/input capture */
#define _E0_MTU_IOB_TGRC8               (0xE0U) /* Input capture at MTU8.TGRC compare match */
/* I/O Control C (IOC[3:0]) for MTU0.TIORL, MTU3.TIORL, MTU4.TIORL, MTU6.TIORL, MTU7.TIORL, MTU8.TIORL */
#define _00_MTU_IOC_DISABLE             (0x00U) /* Output prohibited */
#define _01_MTU_IOC_LL                  (0x01U) /* Initial output is low. Low output at compare match */
#define _02_MTU_IOC_LH                  (0x02U) /* Initial output is low. High output at compare match */
#define _03_MTU_IOC_LT                  (0x03U) /* Initial output is low. Toggle output at compare match */
#define _05_MTU_IOC_HL                  (0x05U) /* Initial output is high. Low output at compare match */
#define _06_MTU_IOC_HH                  (0x06U) /* Initial output is high. High output at compare match */
#define _07_MTU_IOC_HT                  (0x07U) /* Initial output is high. Toggle output at compare match */
#define _08_MTU_IOC_IR                  (0x08U) /* Input capture at rising edge */
#define _09_MTU_IOC_IF                  (0x09U) /* Input capture at falling edge */
#define _0A_MTU_IOC_IB                  (0x0AU) /* Input capture at both edges */
#define _0C_MTU_IOC_EX                  (0x0CU) /* Input capture at MTU1.TCNT up-count/down-count */
/* I/O Control C (IOC[4:0]) for MTU5.TIORU, MTU5.TIORV, MTU5.TIORW */
#define _00_MTU5_IOC_CP                 (0x00U) /* No function */
#define _11_MTU5_IOC_R                  (0x11U) /* Input capture at rising edge */
#define _12_MTU5_IOC_F                  (0x12U) /* Input capture at falling edge */
#define _13_MTU5_IOC_B                  (0x13U) /* Input capture at both edges */
#define _14_MTU5_IOC_TGRC               (0x14U) /* Input capture at MTU8.TGRC compare match */
#define _19_MTU5_IOC_PWM_LOW_TROUGH     (0x19U) /* Low pulse input signal. Capture at trough in 
                                                   complementary PWM mode */
#define _1A_MTU5_IOC_PWM_LOW_CREST      (0x1AU) /* Low pulse input signal. Capture at crest of 
                                                   complementary PWM mode */
#define _1B_MTU5_IOC_PWM_LOW_BOTH       (0x1BU) /* Low pulse input signal. Capture at crest and trough of 
                                                   complementary PWM mode */
#define _1D_MTU5_IOC_PWM_HIGH_TROUGH    (0x1DU) /* High pulse input signal. Capture at trough in 
                                                   complementary PWM mode */
#define _1E_MTU5_IOC_PWM_HIGH_CREST     (0x1EU) /* High pulse input signal. Capture at crest of 
                                                   complementary PWM mode */
#define _1F_MTU5_IOC_PWM_HIGH_BOTH      (0x1FU) /* High pulse input signal. Capture at crest and trough of 
                                                   complementary PWM mode */
/* I/O Control D (IOD[3:0]) for MTU0.TIORL, MTU3.TIORL, MTU4.TIORL, MTU6.TIORL, MTU7.TIORL, MTU8.TIORL */
#define _00_MTU_IOD_DISABLE             (0x00U) /* Output prohibited */
#define _10_MTU_IOD_LL                  (0x10U) /* Initial output is low. Low output at compare match */
#define _20_MTU_IOD_LH                  (0x20U) /* Initial output is low. High output at compare match */
#define _30_MTU_IOD_LT                  (0x30U) /* Initial output is low. Toggle output at compare match */
#define _50_MTU_IOD_HL                  (0x50U) /* Initial output is high. Low output at compare match */
#define _60_MTU_IOD_HH                  (0x60U) /* Initial output is high. High output at compare match */
#define _70_MTU_IOD_HT                  (0x70U) /* Initial output is high. Toggle output at compare match */
#define _80_MTU_IOD_IR                  (0x80U) /* Input capture at rising edge */
#define _90_MTU_IOD_IF                  (0x90U) /* Input capture at falling edge */
#define _A0_MTU_IOD_IB                  (0xA0U) /* Input capture at both edges */
#define _C0_MTU_IOD_EX                  (0xC0U) /* Input capture at MTU1.TCNT up-count/down-count */

/*
    Timer Buffer Operation Transfer Mode Register (TBTM)
*/
/* Timing Select A (TTSA) */
#define _00_MTU_TTSA_CMMA               (0x00U) /* When compare match A occurs in each channel, data is transferred */
#define _01_MTU_TTSA_TCNT               (0x01U) /* When TCNT is cleared in each channel, data is transferred */
/* Timing Select B (TTSB) */
#define _00_MTU_TTSB_CMMB               (0x00U) /* When compare match B occurs in each channel, data is transferred */
#define _02_MTU_TTSB_TCNT               (0x02U) /* When TCNT is cleared in each channel, data is transferred */
/* Timing Select E (TTSE) */
#define _00_MTU_TTSE_CMME               (0x00U) /* When compare match E occurs in MTU0, data is transferred */
#define _04_MTU_TTSE_TCNT               (0x04U) /* When MTU0.TCNT is cleared in MTU0, data is transferred */

/*
    Timer Input Capture Control Register (TICCR)
*/
/* Input Capture Enable (I1AE) */
#define _00_MTU_I1AE_DISABLE            (0x00U) /* Does not include MTIOC1A pin in MTU2.TGRA input capture condition */
#define _01_MTU_I1AE_ENABLE             (0x01U) /* Includes MTIOC1A pin in MTU2.TGRA input capture condition */
/* Input Capture Enable (I1BE) */
#define _00_MTU_I1BE_DISABLE            (0x00U) /* Does not include MTIOC1B pin in MTU2.TGRB input capture condition */
#define _02_MTU_I1BE_ENABLE             (0x02U) /* Includes MTIOC1B pin in MTU2.TGRB input capture condition */
/* Input Capture Enable (I2AE) */
#define _00_MTU_I2AE_DISABLE            (0x00U) /* Does not include MTIOC2A pin in MTU1.TGRA input capture condition */
#define _04_MTU_I2AE_ENABLE             (0x04U) /* Includes MTIOC2A pin in MTU1.TGRA input capture condition */
/* Input Capture Enable (I2BE) */
#define _00_MTU_I2BE_DISABLE            (0x00U) /* Does not include MTIOC2B pin in MTU1.TGRB input capture condition */
#define _08_MTU_I2BE_ENABLE             (0x08U) /* Includes MTIOC2B pin in MTU1.TGRB input capture condition */

/*
    Timer A/D Converter Start Request Control Register (TADCR)
*/
/* TCIV4 Interrupt Skipping Link Enable (ITBnVE) (n=4,7) */
#define _0000_MTU_ITBVE_DISABLE         (0x0000U) /* TCInV interrupt skipping is not linked */
#define _0001_MTU_ITBVE_ENABLE          (0x0001U) /* TCInV interrupt skipping is linked */
/* TGIA3 Interrupt Skipping Link Enable (ITBnAE) (n=3,6) */
#define _0000_MTU_ITBAE_DISABLE         (0x0000U) /* TGInA interrupt skipping is not linked */
#define _0002_MTU_ITBAE_ENABLE          (0x0002U) /* TGInA interrupt skipping is linked */
/* TCIV4 Interrupt Skipping Link Enable (ITAnVE) (n=4,7) */
#define _0000_MTU_ITAVE_DISABLE         (0x0000U) /* TCInV interrupt skipping is not linked */
#define _0004_MTU_ITAVE_ENABLE          (0x0004U) /* TCInV interrupt skipping is linked */
/* TGIA3 Interrupt Skipping Link Enable (ITAnAE) (n=3,6) */
#define _0000_MTU_ITAAE_DISABLE         (0x0000U) /* TGInA interrupt skipping is not linked */
#define _0008_MTU_ITAAE_ENABLE          (0x0008U) /* TGInA interrupt skipping is linked */
/* Down-Count TRGnBN Enable (DTnBE) */
#define _0000_MTU_DTBE_DISABLE          (0x0000U) /* A/D converter start request disabled during MTUn.TCNT down-count */
#define _0010_MTU_DTBE_ENABLE           (0x0010U) /* A/D converter start request enabled during MTUn.TCNT down-count */
/* Up-Count TRGnBN Enable (UTnBE) */
#define _0000_MTU_UTBE_DISABLE          (0x0000U) /* A/D converter start requests disabled during MTUn.TCNT up-count */
#define _0020_MTU_UTBE_ENABLE           (0x0020U) /* A/D converter start requests enabled during MTUn.TCNT up-count */
/* Down-Count TRGnAN Enable (DTnAE) */
#define _0000_MTU_DTAE_DISABLE          (0x0000U) /* A/D converter start request disabled during MTUn.TCNT down-count */
#define _0040_MTU_DTAE_ENABLE           (0x0040U) /* A/D converter start request enabled during MTUn.TCNT down-count */
/* Up-Count TRGnAN Enable (UTnAE) */
#define _0000_MTU_UTAE_DISABLE          (0x0000U) /* A/D converter start requests disabled during MTUn.TCNT up-count */
#define _0080_MTU_UTAE_ENABLE           (0x0080U) /* A/D converter start requests enabled during MTUn.TCNT up-count */
/* MTU4.TADCOBRA/B Transfer Timing Select (BF[1:0]) (n=4,7) */
#define _0000_MTU_BF_DISABLE            (0x0000U) /* Transfers data from buffer register is disabled */
#define _4000_MTU_BF_TOP                (0x4000U) /* Transfers data at the crest of the MTUn.TCNT count */
#define _8000_MTU_BF_LOW                (0x8000U) /* Transfers data at the trough of the MTUn.TCNT count */
#define _C000_MTU_BF_BOTH               (0xC000U) /* Transfers data at the crest and trough of the MTUn.TCNT count */

/*
    Timer Start Registers (TSTR)
*/
/* Counter Start 0 (CST0) */
#define _00_MTU_CST0_OFF                (0x00U) /* MTU0.TCNT performs count stop */
#define _01_MTU_CST0_ON                 (0x01U) /* MTU0.TCNT performs count operation */
/* Counter Start 1 (CST1) */
#define _00_MTU_CST1_OFF                (0x00U) /* MTU1.TCNT performs count stop */
#define _02_MTU_CST1_ON                 (0x02U) /* MTU1.TCNT performs count operation */
/* Counter Start 2 (CST2) */
#define _00_MTU_CST2_OFF                (0x00U) /* MTU2.TCNT performs count stop */
#define _04_MTU_CST2_ON                 (0x04U) /* MTU2.TCNT performs count operation */
/* Counter Start 8 (CST8) */
#define _00_MTU_CST8_OFF                (0x00U) /* MTU8.TCNT performs count stop */
#define _08_MTU_CST8_ON                 (0x08U) /* MTU8.TCNT performs count operation */
/* Counter Start 3 (CST3) */
#define _00_MTU_CST3_OFF                (0x00U) /* MTU3.TCNT performs count stop */
#define _40_MTU_CST3_ON                 (0x40U) /* MTU3.TCNT performs count operation */
/* Counter Start 4 (CST4) */
#define _00_MTU_CST4_OFF                (0x00U) /* MTU4.TCNT performs count stop */
#define _80_MTU_CST4_ON                 (0x80U) /* MTU4.TCNT performs count operation */
/* Counter Start 6 (CST6) */ 
#define _00_MTU_CST6_OFF                (0x00U) /* MTU6.TCNT performs count stop */
#define _40_MTU_CST6_ON                 (0x40U) /* MTU6.TCNT performs count operation */
/* Counter Start 7 (CST7) */ 
#define _00_MTU_CST7_OFF                (0x00U) /* MTU7.TCNT performs count stop */
#define _80_MTU_CST7_ON                 (0x80U) /* MTU7.TCNT performs count operation */
/* Counter Start W5 (CSTW5) */
#define _00_MTU_CSTW5_OFF               (0x00U) /* MTU5.TCNTW count operation is stopped */
#define _01_MTU_CSTW5_ON                (0x01U) /* MTU5.TCNTW performs count operation */
/* Counter Start V5 (CSTV5) */
#define _00_MTU_CSTV5_OFF               (0x00U) /* MTU5.TCNTV count operation is stopped */
#define _02_MTU_CSTV5_ON                (0x02U) /* MTU5.TCNTV performs count operation */
/* Counter Start U5 (CSTU5) */
#define _00_MTU_CSTU5_OFF               (0x00U) /* MTU5.TCNTU count operation is stopped */
#define _04_MTU_CSTU5_ON                (0x04U) /* MTU5.TCNTU performs count operation */

/*
    Timer Synchronous Clear Register (TSYCR)
*/
/* Clear Enable 2B (CE2B) */
#define _00_MTU6_CL2B_OFF               (0x00U) /* Disables counter clearing by the TGFB flag setting in MTU2.TSR */
#define _01_MTU6_CL2B_ON                (0x01U) /* Enables counter clearing by the TGFB flag setting in MTU2.TSR */
/* Clear Enable 2A (CE2A) */
#define _00_MTU6_CL2A_OFF               (0x00U) /* Disables counter clearing by the TGFA flag setting in MTU2.TSR */
#define _02_MTU6_CL2A_ON                (0x02U) /* Enables counter clearing by the TGFA flag setting in MTU2.TSR */
/* Clear Enable 1B (CE1B) */
#define _00_MTU6_CL1B_OFF               (0x00U) /* Disables counter clearing by the TGFB flag setting in MTU1.TSR */
#define _04_MTU6_CL1B_ON                (0x04U) /* Enables counter clearing by the TGFB flag setting in MTU1.TSR */
/* Clear Enable 1A (CE1A) */
#define _00_MTU6_CL1A_OFF               (0x00U) /* Disables counter clearing by the TGFA flag setting in MTU1.TSR */
#define _08_MTU6_CL1A_ON                (0x08U) /* Enables counter clearing by the TGFA flag setting in MTU1.TSR */
/* Clear Enable 0D (CE0D) */
#define _00_MTU6_CL0D_OFF               (0x00U) /* Disables counter clearing by the TGFD flag setting in MTU0.TSR */
#define _10_MTU6_CL0D_ON                (0x10U) /* Enables counter clearing by the TGFD flag setting in MTU0.TSR */
/* Clear Enable 0C (CE0C) */
#define _00_MTU6_CL0C_OFF               (0x00U) /* Disables counter clearing by the TGFC flag setting in MTU0.TSR */
#define _20_MTU6_CL0C_ON                (0x20U) /* Enables counter clearing by the TGFC flag setting in MTU0.TSR */
/* Clear Enable 0B (CE0B) */
#define _00_MTU6_CL0B_OFF               (0x00U) /* Disables counter clearing by the TGFB flag setting in MTU0.TSR */
#define _40_MTU6_CL0B_ON                (0x40U) /* Enables counter clearing by the TGFB flag setting in MTU0.TSR */
/* Clear Enable 0A (CE0A) */
#define _00_MTU6_CL0A_OFF               (0x00U) /* Disables counter clearing by the TGFA flag setting in MTU0.TSR */
#define _80_MTU6_CL0A_ON                (0x80U) /* Enables counter clearing by the TGFA flag setting in MTU0.TSR */

/*
    Timer Synchronous Registers (TSYRA, TSYRB)
*/
/* Timer Synchronous Operation 0 (SYNC0) */
#define _00_MTU_SYNC0_OFF               (0x00U) /* MTU0.TCNT operates independently */
#define _01_MTU_SYNC0_ON                (0x01U) /* MTU0.TCNT performs synchronous operation */
/* Timer Synchronous Operation 1 (SYNC1) */
#define _00_MTU_SYNC1_OFF               (0x00U) /* MTU1.TCNT operates independently */
#define _02_MTU_SYNC1_ON                (0x02U) /* MTU1.TCNT performs synchronous operation */
/* Timer Synchronous Operation 2 (SYNC2) */
#define _00_MTU_SYNC2_OFF               (0x00U) /* MTU2.TCNT operates independently */
#define _04_MTU_SYNC2_ON                (0x04U) /* MTU2.TCNT performs synchronous operation */
/* Timer Synchronous Operation 3 (SYNC3) */
#define _00_MTU_SYNC3_OFF               (0x00U) /* MTU3.TCNT operates independently */
#define _40_MTU_SYNC3_ON                (0x40U) /* MTU3.TCNT performs synchronous operation */
/* Timer Synchronous Operation 4 (SYNC4) */
#define _00_MTU_SYNC4_OFF               (0x00U) /* MTU4.TCNT operates independently */
#define _80_MTU_SYNC4_ON                (0x80U) /* MTU4.TCNT performs synchronous operation */
/* Timer Synchronous Operation 6 (SYNC6) */
#define _00_MTU_SYNC6_OFF               (0x00U) /* MTU6.TCNT operates independently */
#define _40_MTU_SYNC6_ON                (0x40U) /* MTU6.TCNT performs synchronous operation */
/* Timer Synchronous Operation 7 (SYNC7) */
#define _00_MTU_SYNC7_OFF               (0x00U) /* MTU7.TCNT operates independently */
#define _80_MTU_SYNC7_ON                (0x80U) /* MTU7.TCNT performs synchronous operation */

/*
    Timer Read/Write Enable Registers (TRWERA and TRWERB)
*/
/* Read/Write Enable (RWE) */
#define _00_MTU_RWE_DISABLE             (0x00U) /* Read/write access to the registers is disabled */
#define _01_MTU_RWE_ENABLE              (0x01U) /* Read/write access to the registers is enabled */

/*
    Timer Output Master Enable Registers (TOERA)
*/
/* Master Enable MTIOC3B (OE3B) */
#define _C0_MTU_OE3B_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C1_MTU_OE3B_ENABLE             (0xC1U) /* MTU output is enabled */
/* Master Enable MTIOC4A (OE4A) */
#define _C0_MTU_OE4A_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C2_MTU_OE4A_ENABLE             (0xC2U) /* MTU output is enabled */
/* Master Enable MTIOC4B (OE4B) */
#define _C0_MTU_OE4B_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C4_MTU_OE4B_ENABLE             (0xC4U) /* MTU output is enabled */
/* Master Enable MTIOC3D (OE3D) */
#define _C0_MTU_OE3D_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C8_MTU_OE3D_ENABLE             (0xC8U) /* MTU output is enabled */
/* Master Enable MTIOC4C (OE4C) */
#define _C0_MTU_OE4C_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _D0_MTU_OE4C_ENABLE             (0xD0U) /* MTU output is enabled */
/* Master Enable MTIOC4D (OE4D) */
#define _C0_MTU_OE4D_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _E0_MTU_OE4D_ENABLE             (0xE0U) /* MTU output is enabled */

/*
    Timer Output Master Enable Registers (TOERB)
*/
/* Master Enable MTIOC6B (OE6B) */
#define _C0_MTU_OE6B_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C1_MTU_OE6B_ENABLE             (0xC1U) /* MTU output is enabled */
/* Master Enable MTIOC7A (OE7A) */
#define _C0_MTU_OE7A_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C2_MTU_OE7A_ENABLE             (0xC2U) /* MTU output is enabled */
/* Master Enable MTIOC7B (OE7B) */
#define _C0_MTU_OE7B_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C4_MTU_OE7B_ENABLE             (0xC4U) /* MTU output is enabled */
/* Master Enable MTIOC6D (OE6D) */
#define _C0_MTU_OE6D_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _C8_MTU_OE6D_ENABLE             (0xC8U) /* MTU output is enabled */
/* Master Enable MTIOC7C (OE7C) */
#define _C0_MTU_OE7C_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _D0_MTU_OE7C_ENABLE             (0xD0U) /* MTU output is enabled */
/* Master Enable MTIOC7D (OE7D) */
#define _C0_MTU_OE7D_DISABLE            (0xC0U) /* MTU output is disabled (inactive level) */
#define _E0_MTU_OE7D_ENABLE             (0xE0U) /* MTU output is enabled */

/*
    Timer Gate Control Registers (TGCRA)
*/
/* bit7: Reserved( This bit is read as 1. The write value should be 1.) */
/* External Feedback Signal Enable (FB) */
#define _80_MTU_FB_EXIN                 (0x80U) /* Output is switched by external input */
#define _88_MTU_FB_SW                   (0x88U) /* Output is switched by software */
/* Positive-Phase Output Control (P) */
#define _80_MTU_P_LEVEL                 (0x80U) /* Level output */
#define _90_MTU_P_PWM                   (0x90U) /* Reset-synchronized PWM or complementary PWM output */
/* Negative-Phase Output Control (N) */
#define _80_MTU_N_LEVEL                 (0x80U) /* Level output */
#define _A0_MTU_N_PWM                   (0xA0U) /* Reset-synchronized PWM or complementary PWM output */
/* Brushless DC Motor (BDC) */
#define _80_MTU_BDC_OUT                 (0x80U) /* Ordinary output */
#define _C0_MTU_BDC_FUN                 (0xC0U) /* Functions of this register are made effective */

/*
    Timer Interrupt Skipping Set Registers (TITCR1A, TITCR1B)
*/
/* TCIV4 or TCIV7 Interrupt Skipping Count Setting (T4VCOR[2:0], T7VCOR[2:0]) */
#define _00_MTU_TVCOR_4_7_SKIP_COUNT_0  (0x00U) /* Does not skip TCIV4 or TCIV7 interrupts */
#define _01_MTU_TVCOR_4_7_SKIP_COUNT_1  (0x01U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 1 */
#define _02_MTU_TVCOR_4_7_SKIP_COUNT_2  (0x02U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 2 */
#define _03_MTU_TVCOR_4_7_SKIP_COUNT_3  (0x03U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 3 */
#define _04_MTU_TVCOR_4_7_SKIP_COUNT_4  (0x04U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 4 */
#define _05_MTU_TVCOR_4_7_SKIP_COUNT_5  (0x05U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 5 */
#define _06_MTU_TVCOR_4_7_SKIP_COUNT_6  (0x06U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 6 */
#define _07_MTU_TVCOR_4_7_SKIP_COUNT_7  (0x07U) /* Sets the TCIV4 or TCIV7 interrupt skipping count to 7 */
/* TGIA3 or TGIA6 Interrupt Skipping Count Setting (T3ACOR[2:0], T6ACOR[2:0]) */
#define _00_MTU_TACOR_3_6_SKIP_COUNT_0  (0x00U) /* Does not skip TGIA3 interrupts */
#define _10_MTU_TACOR_3_6_SKIP_COUNT_1  (0x10U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 1 */
#define _20_MTU_TACOR_3_6_SKIP_COUNT_2  (0x20U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 2 */
#define _30_MTU_TACOR_3_6_SKIP_COUNT_3  (0x30U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 3 */
#define _40_MTU_TACOR_3_6_SKIP_COUNT_4  (0x40U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 4 */
#define _50_MTU_TACOR_3_6_SKIP_COUNT_5  (0x50U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 5 */
#define _60_MTU_TACOR_3_6_SKIP_COUNT_6  (0x60U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 6 */
#define _70_MTU_TACOR_3_6_SKIP_COUNT_7  (0x70U) /* Sets the TGIA3 or TGIA6 interrupt skipping count to 7 */
/* T4VEN or T7VEN */
#define _00_MTU_T4_7VEN_DISABLE         (0x00U) /* TCIV4 or TCIV7 interrupt skipping disabled */
#define _08_MTU_T4_7VEN_ENABLE          (0x08U) /* TCIV4 or TCIV7 interrupt skipping enabled */
/* T3AEN or T6AEN */
#define _00_MTU_T3_6AEN_DISABLE         (0x00U) /* TGIA3 or TGIA6 interrupt skipping disabled */
#define _80_MTU_T3_6AEN_ENABLE          (0x80U) /* TGIA3 or TGIA6 interrupt skipping enabled */

/*
    Timer Interrupt Skipping Set Registers 2 (TITCR2A, TITCR2B)
*/
/* Setting of Interrupt Skipping Count by TRG4COR[2:0], TRG7COR[2:0] (n= 4,7) */
#define _00_MTU_TRGCOR_4_7_SKIP_COUNT_0 (0x00U) /* Does not skip TRGnAN and TRGnBN interrupts */
#define _01_MTU_TRGCOR_4_7_SKIP_COUNT_1 (0x01U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 1 */
#define _02_MTU_TRGCOR_4_7_SKIP_COUNT_2 (0x02U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 2 */
#define _03_MTU_TRGCOR_4_7_SKIP_COUNT_3 (0x03U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 3 */
#define _04_MTU_TRGCOR_4_7_SKIP_COUNT_4 (0x04U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 4 */
#define _05_MTU_TRGCOR_4_7_SKIP_COUNT_5 (0x05U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 5 */
#define _06_MTU_TRGCOR_4_7_SKIP_COUNT_6 (0x06U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 6 */
#define _07_MTU_TRGCOR_4_7_SKIP_COUNT_7 (0x07U) /* Sets the TRGnAN and TRGnBN interrupt skipping count to 7 */

/*
    Timer Buffer Transfer Set Registers (TBTERA, TBTERB)
*/
/* Buffer Transfer Disable and Interrupt Skipping Link Setting (BTE[1:0]) */
#define _00_MTU_TF_NO_INT               (0x00U) /* Enables transfer and does not link with 
                                                   interrupt skipping operation */
#define _01_MTU_TF_DISABLE              (0x01U) /* Disables transfer from the buffer registers to 
                                                   the temporary registers */
#define _02_MTU_TF_WITH_INT             (0x02U) /* Links transfer with interrupt skipping operation */

/*
    Timer Dead Time Enable Registers (TDERA, TDERB)
*/
/* Dead Time Enable (TDER) */
#define _00_MTU_TDER_DISABLE            (0x00U) /* No dead time is generated */
#define _01_MTU_TDER_ENABLE             (0x01U) /* Dead time is generated */

/*
    Timer Waveform Control Registers (TWCRA, TWCRB)
*/
/* Initial Output Inhibition Enable (WRE) */
#define _00_MTU_WRE_DISABLE             (0x00U) /* Initial value specified in TOCR is output */
#define _01_MTU_WRE_ENABLE              (0x01U) /* Initial output is inhibited */
/* Synchronous Clearing Control (SCC) */
#define _00_MTU_SCC_DISABLE             (0x00U) /* Clearing of MTU6.TCNT and MTU7.TCNT is disabled */
#define _02_MTU_SCC_ENABLE              (0x02U) /* Clearing of MTU6.TCNT and MTU7.TCNT is enabled */
 /* Compare Match Clear Enable (CCE) */
#define _00_MTU_CCE_DISABLE             (0x00U) /* Counters are not cleared at MTU3.TGRA compare match */
#define _80_MTU_CCE_ENABLE              (0x80U) /* Counters are cleared at MTU3.TGRA compare match */

/*
    Noise Filter Control Registers (NFCRn)
*/
/* Noise Filter U Enable Bit (NFUEN) */
#define _00_MTU_NFUEN_DISABLE           (0x00U) /* The noise filter for the MTIC5U pin is disabled */
#define _01_MTU_NFUEN_ENABLE            (0x01U) /* The noise filter for the MTIC5U pin is enabled */
/* Noise Filter V Enable Bit (NFVEN) */
#define _00_MTU_NFVEN_DISABLE           (0x00U) /* The noise filter for the MTIC5V pin is disabled */
#define _02_MTU_NFVEN_ENABLE            (0x02U) /* The noise filter for the MTIC5V pin is enabled */
/* Noise Filter W Enable Bit (NFWEN) */
#define _00_MTU_NFWEN_DISABLE           (0x00U) /* The noise filter for the MTIC5W pin is disabled */
#define _04_MTU_NFWEN_ENABLE            (0x04U) /* The noise filter for the MTIC5W pin is enabled */
/* Noise Filter A Enable Bit (NFAEN) */
#define _00_MTU_NFAEN_DISABLE           (0x00U) /* The noise filter for the MTIOCnA pin is disabled */
#define _01_MTU_NFAEN_ENABLE            (0x01U) /* The noise filter for the MTIOCnA pin is enabled */
/* Noise Filter B Enable Bit (NFBEN) */
#define _00_MTU_NFBEN_DISABLE           (0x00U) /* The noise filter for the MTIOCnB pin is disabled */
#define _02_MTU_NFBEN_ENABLE            (0x02U) /* The noise filter for the MTIOCnB pin is enabled */
/* Noise Filter C Enable Bit (NFCEN) */
#define _00_MTU_NFCEN_DISABLE           (0x00U) /* The noise filter for the MTIOCnC pin is disabled */
#define _04_MTU_NFCEN_ENABLE            (0x04U) /* The noise filter for the MTIOCnC pin is enabled */
/* Noise Filter D Enable Bit (NFDEN) */
#define _00_MTU_NFDEN_DISABLE           (0x00U) /* The noise filter for the MTIOCnD pin is disabled */
#define _08_MTU_NFDEN_ENABLE            (0x08U) /* The noise filter for the MTIOCnD pin is enabled */
/* Noise Filter Clock Select (NFCS[1:0]) */
#define _00_MTU_NFCS_PCLK_1             (0x00U) /* PCLK/1 */
#define _10_MTU_NFCS_PCLK_8             (0x10U) /* PCLK/8 */
#define _20_MTU_NFCS_PCLK_32            (0x20U) /* PCLK/32 */
#define _30_MTU_NFCS_EXCLK              (0x30U) /* The clock source for counting is the external clock */

/*
    Noise Filter Control Register (NFCRC)
*/
/* Noise Filter A Enable (NFAEN) */
#define _00_MTU_NFCRC_NFAEN_DISABLE     (0x00U) /* The noise filter for the MTCLKA pin is disabled */
#define _01_MTU_NFCRC_NFAEN_ENABLE      (0x01U) /* The noise filter for the MTCLKA pin is enabled */
/* Noise Filter B Enable (NFBEN) */
#define _00_MTU_NFCRC_NFBEN_DISABLE     (0x00U) /* The noise filter for the MTCLKB pin is disabled */
#define _02_MTU_NFCRC_NFBEN_ENABLE      (0x02U) /* The noise filter for the MTCLKB pin is enabled */
/* Noise Filter C Enable (NFCEN) */
#define _00_MTU_NFCRC_NFCEN_DISABLE     (0x00U) /* The noise filter for the MTCLKC pin is disabled */
#define _04_MTU_NFCRC_NFCEN_ENABLE      (0x04U) /* The noise filter for the MTCLKC pin is enabled */
/* Noise Filter D Enable (NFDEN) */
#define _00_MTU_NFCRC_NFDEN_DISABLE     (0x00U) /* The noise filter for the MTCLKD pin is disabled */
#define _08_MTU_NFCRC_NFDEN_ENABLE      (0x08U) /* The noise filter for the MTCLKD pin is enabled */
/* Noise Filter Clock Select (NFCSC[1:0]) */
#define _00_MTU_NFCSC_PCLK_1            (0x00U) /* PCLK/1 */
#define _10_MTU_NFCSC_PCLK_2            (0x10U) /* PCLK/2 */
#define _20_MTU_NFCSC_PCLK_8            (0x20U) /* PCLK/8 */
#define _30_MTU_NFCSC_PCLK_32           (0x30U) /* PCLK/32 */

/*
    Timer Compare Match Clear Register (TCNTCMPCLR)
*/
/* TCNT Compare Clear 5W (CMPCLR5W) */
#define _00_MTU_CMPCLR5W_DISABLE        (0x00U) /* Disables MTU5.TCNTW to be cleared */
#define _01_MTU_CMPCLR5W_ENABLE         (0x01U) /* Enables MTU5.TCNTW to be cleared */
/* TCNT Compare Clear 5V (CMPCLR5V) */
#define _00_MTU_CMPCLR5V_DISABLE        (0x00U) /* Disables MTU5.TCNTV to be cleared */
#define _02_MTU_CMPCLR5V_ENABLE         (0x02U) /* Enables MTU5.TCNTV to be cleared */
/* TCNT Compare Clear 5U (CMPCLR5U) */
#define _00_MTU_CMPCLR5U_DISABLE        (0x00U) /* Disables MTU5.TCNTU to be cleared */
#define _04_MTU_CMPCLR5U_ENABLE         (0x04U) /* Enables MTU5.TCNTU to be cleared */

/*
    Timer Output Control Registers 1 (TOCR1A, TOCR1B)
*/
/* Output Level Select P (OLSP) */
#define _00_MTU_OLSP_HL                 (0x00U) /* Initial output:H, Active level:L */
#define _01_MTU_OLSP_LH                 (0x01U) /* Initial output:L, Active level:H */
/* Output Level Select N (OLSN) */
#define _00_MTU_OLSN_HL                 (0x00U) /* Initial output:H, Active level:L */
#define _02_MTU_OLSN_LH                 (0x02U) /* Initial output:L, Active level:H */
/* TOC Select (TOCS) */
#define _00_MTU_TOCS_TOCR1              (0x00U) /* TOCR1 setting is selected */
#define _04_MTU_TOCS_TOCR2              (0x04U) /* TOCR2 setting is selected */
/* TOC Register Write Protection (TOCL) */
#define _00_MTU_TOCL_ENABLE             (0x00U) /* Write access to the TOCS, OLSN, and OLSP bits is enabled */
#define _08_MTU_TOCL_DISABLE            (0x08U) /* Write access to the TOCS, OLSN, and OLSP bits is disabled */
/* PWM Synchronous Output Enable (PSYE) */
#define _00_MTU_PSYE_DISABLE            (0x00U) /* Toggle output is disabled */
#define _40_MTU_PSYE_ENABLE             (0x40U) /* Toggle output is enabled */

/*
    Timer Output Control Registers 2 (TOCR2A, TOCR2B)
*/
/* Output Level Select 1P (OLS1P) */
#define _00_MTU_OLS1P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _01_MTU_OLS1P_LH                (0x01U) /* Initial output:L, Active level:H */
/* Output Level Select 1N (OLS1N) */
#define _00_MTU_OLS1N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _02_MTU_OLS1N_LH                (0x02U) /* Initial output:L, Active level:H */
/* Output Level Select 2P (OLS2P) */
#define _00_MTU_OLS2P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _04_MTU_OLS2P_LH                (0x04U) /* Initial output:L, Active level:H */
/* Output Level Select 2N (OLS2N) */
#define _00_MTU_OLS2N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _08_MTU_OLS2N_LH                (0x08U) /* Initial output:L, Active level:H */
/* Output Level Select 3P (OLS3P) */
#define _00_MTU_OLS3P_HL                (0x00U) /* Initial output:H, Active level:L */
#define _10_MTU_OLS3P_LH                (0x10U) /* Initial output:L, Active level:H */
/* Output Level Select 3N (OLS3N) */
#define _00_MTU_OLS3N_HL                (0x00U) /* Initial output:H, Active level:L */
#define _20_MTU_OLS3N_LH                (0x20U) /* Initial output:L, Active level:H */
/* TOLBR Buffer Transfer Timing Select (BF[1:0]) */
#define _00_MTU_TOLBR_DIS               (0x00U) /* Does not transfer data from the buffer register (TOLBR) to TOCR2 */
#define _40_MTU_TOLBR_C                 (0x40U) /* Transfers at the crest of the MTU4.TCNT count */
#define _80_MTU_TOLBR_T                 (0x80U) /* Transfers at the trough of the MTU4.TCNT count */
#define _C0_MTU_TOLBR_CT                (0xC0U) /* Transfers at crest and trough of the MTU4.TCNT count */

/*
    Timer Interrupt Enable Register (TIER)
*/
/* TGR Interrupt Enable A (TGIEA) */
#define _00_MTU_TGIEA_DISABLE           (0x00U) /* Interrupt requests TGIA disabled */
#define _01_MTU_TGIEA_ENABLE            (0x01U) /* Interrupt requests TGIA enabled */
/* TGR Interrupt Enable B (TGIEB) */
#define _00_MTU_TGIEB_DISABLE           (0x00U) /* Interrupt requests TGIB disabled */
#define _02_MTU_TGIEB_ENABLE            (0x02U) /* Interrupt requests TGIB enabled */
/* TGR Interrupt Enable C (TGIEC) */
#define _00_MTU_TGIEC_DISABLE           (0x00U) /* Interrupt requests TGIC disabled */
#define _04_MTU_TGIEC_ENABLE            (0x04U) /* Interrupt requests TGIC enabled */
/* TGR Interrupt Enable D (TGIED) */
#define _00_MTU_TGIED_DISABLE           (0x00U) /* Interrupt requests TGID disabled */
#define _08_MTU_TGIED_ENABLE            (0x08U) /* Interrupt requests TGID enabled */
/* Overflow Interrupt Enable (TCIEV) */
#define _00_MTU_TCIEV_DISABLE           (0x00U) /* Interrupt requests TCIV disabled */
#define _10_MTU_TCIEV_ENABLE            (0x10U) /* Interrupt requests TCIV enabled */
/* Underflow Interrupt Enable (TCIEU) */
#define _00_MTU_TCIEU_DISABLE           (0x00U) /* Interrupt requests TCIU disabled */
#define _20_MTU_TCIEU_ENABLE            (0x20U) /* Interrupt requests TCIU enabled */
/* A/D Converter Start Request Enable 2 (TTGE2) */
#define _00_MTU_TTGE2_DISABLE           (0x00U) /* A/D converter start request by MTU4.TCNT disabled */
#define _40_MTU_TTGE2_ENABLE            (0x40U) /* A/D converter start request by MTU4.TCNT enabled */
/* A/D Converter Start Request Enable (TTGE) */
#define _00_MTU_TTGE_DISABLE            (0x00U) /* A/D converter start request generation disabled */
#define _80_MTU_TTGE_ENABLE             (0x80U) /* A/D converter start request generation enabled */
/* TGR Interrupt Enable 5W (TGIE5W) */
#define _00_MTU_TGIE5W_DISABLE          (0x00U) /* Interrupt requests TGI5W disabled */
#define _01_MTU_TGIE5W_ENABLE           (0x01U) /* Interrupt requests TGI5W enabled */
/* TGR Interrupt Enable 5V (TGIE5V) */
#define _00_MTU_TGIE5V_DISABLE          (0x00U) /* Interrupt requests TGI5V disabled */
#define _02_MTU_TGIE5V_ENABLE           (0x02U) /* Interrupt requests TGI5V enabled */
/* TGR Interrupt Enable 5U (TGIE5U) */
#define _00_MTU_TGIE5U_DISABLE          (0x00U) /* Interrupt requests TGI5U disabled */
#define _04_MTU_TGIE5U_ENABLE           (0x04U) /* Interrupt requests TGI5U enabled */

/*
    Timer Interrupt Enable Register (TIER2)
*/
/* TGR Interrupt Enable E (TGIEE) */
#define _00_MTU_TGIEE_DISABLE           (0x00U) /* Interrupt requests TGIE disabled */
#define _01_MTU_TGIEE_ENABLE            (0x01U) /* Interrupt requests TGIE enabled */
/* TGR Interrupt Enable F (TGIEF) */
#define _00_MTU_TGIEF_DISABLE           (0x00U) /* Interrupt requests TGIF disabled */
#define _02_MTU_TGIEF_ENABLE            (0x02U) /* Interrupt requests TGIF enabled */
/* A/D Converter Start Request Enable 2 (TTGE2) */
#define _00_MTU_TTGE2_DISABLE           (0x00U) /* A/D converter start request by TGRE compare match disabled */
#define _80_MTU_TTGE2_ENABLE            (0x80U) /* A/D converter start request by TGRE compare match enabled */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_MTU_PRIORITY_LEVEL0         (0x00U) /* Level 0 (interrupt disabled) */
#define _01_MTU_PRIORITY_LEVEL1         (0x01U) /* Level 1 */
#define _02_MTU_PRIORITY_LEVEL2         (0x02U) /* Level 2 */
#define _03_MTU_PRIORITY_LEVEL3         (0x03U) /* Level 3 */
#define _04_MTU_PRIORITY_LEVEL4         (0x04U) /* Level 4 */
#define _05_MTU_PRIORITY_LEVEL5         (0x05U) /* Level 5 */
#define _06_MTU_PRIORITY_LEVEL6         (0x06U) /* Level 6 */
#define _07_MTU_PRIORITY_LEVEL7         (0x07U) /* Level 7 */
#define _08_MTU_PRIORITY_LEVEL8         (0x08U) /* Level 8 */
#define _09_MTU_PRIORITY_LEVEL9         (0x09U) /* Level 9 */
#define _0A_MTU_PRIORITY_LEVEL10        (0x0AU) /* Level 10 */
#define _0B_MTU_PRIORITY_LEVEL11        (0x0BU) /* Level 11 */
#define _0C_MTU_PRIORITY_LEVEL12        (0x0CU) /* Level 12 */
#define _0D_MTU_PRIORITY_LEVEL13        (0x0DU) /* Level 13 */
#define _0E_MTU_PRIORITY_LEVEL14        (0x0EU) /* Level 14 */
#define _0F_MTU_PRIORITY_LEVEL15        (0x0FU) /* Level 15 (highest) */

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

