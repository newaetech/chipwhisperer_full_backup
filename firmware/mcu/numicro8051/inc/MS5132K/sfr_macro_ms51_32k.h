/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#if defined __C51__
#include <stdio.h>
#include <string.h>
#include <absacc.h>
#include <intrins.h>

#elif defined __ICC8051__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#elif defined __SDCC__
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

#endif

/******************************************************************************/
/*                         Peripheral header files                            */
/******************************************************************************/
#include "function_define_ms51_32k.h"

#ifdef INC_STD_DRIVER
#include "adc.h"
#include "bod.h"
#include "capture.h"
#include "common.h"
#include "delay.h"
#include "eeprom_sprom.h"
#include "eeprom.h"
#include "eeprom_sprom.h"
#include "gpio.h"
#include "i2c.h" 
#include "iap.h"
#include "iap_sprom.h"
#include "isr.h"
#include "pwm0.h"
#include "pwm123.h"
#include "spi.h"
#include "sys.h"
#include "timer.h"
#include "uart.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "wdt.h"
#include "wkt.h"
#endif

/********SFR ALL PAGES*************/
/********************************************************************/
/*  <Define rule I> set or clr _ regsiter name _ register bit name  */
/********************************************************************/
/**** P0  80H  PAGE A ****/
#define set_P0_7                         P0|=0x80
#define set_P0_6                         P0|=0x40
#define set_P0_5                         P0|=0x20
#define set_P0_4                         P0|=0x10
#define set_P0_3                         P0|=0x08
#define set_P0_2                         P0|=0x04
#define set_P0_1                         P0|=0x02
#define set_P0_0                         P0|=0x01

#define clr_P0_7                         P0&=0x7F
#define clr_P0_6                         P0&=0xBF
#define clr_P0_5                         P0&=0xDF
#define clr_P0_4                         P0&=0xEF
#define clr_P0_3                         P0&=0xF7
#define clr_P0_2                         P0&=0xFB
#define clr_P0_1                         P0&=0xFD
#define clr_P0_0                         P0&=0xFE

/****  87H  PAGE A ****/
#define set_PCON_SMOD                    PCON|=0x80
#define set_PCON_SMOD0                   PCON|=0x40
#define set_PCON_POF                     PCON|=0x10
#define set_PCON_GF1                     PCON|=0x08
#define set_PCON_GF0                     PCON|=0x04
#define set_PCON_PD                      PCON|=0x02
#define set_PCON_IDLE                    PCON|=0x01

#define clr_PCON_SMOD                    PCON&=0x7F
#define clr_PCON_SMOD0                   PCON&=0xBF
#define clr_PCON_POF                     PCON&=0xEF
#define clr_PCON_GF1                     PCON&=0xF7
#define clr_PCON_GF0                     PCON&=0xFB
#define clr_PCON_PD                      PCON&=0xFD
#define clr_PCON_IDLE                    PCON&=0xFE

/**** TCON  88H  PAGE A ****/
#define set_TCON_TF1                     TCON|=0x80
#define set_TCON_TR1                     TCON|=0x40
#define set_TCON_TF0                     TCON|=0x20
#define set_TCON_TR0                     TCON|=0x10
#define set_TCON_IE1                     TCON|=0x08
#define set_TCON_IT1                     TCON|=0x04
#define set_TCON_IE0                     TCON|=0x02
#define set_TCON_IT0                     TCON|=0x01

#define clr_TCON_TF1                     TCON&=0x7F
#define clr_TCON_TR1                     TCON&=0xBF
#define clr_TCON_TF0                     TCON&=0xDF
#define clr_TCON_TR0                     TCON&=0xEF
#define clr_TCON_IE1                     TCON&=0xF7
#define clr_TCON_IT1                     TCON&=0xFB
#define clr_TCON_IE0                     TCON&=0xFD
#define clr_TCON_IT0                     TCON&=0xFE

/**** P1  90H  PAGE A ****/
#define set_P1_7                         P1|=0x80
#define set_P1_6                         P1|=0x40
#define set_P1_5                         P1|=0x20
#define set_P1_4                         P1|=0x10
#define set_P1_3                         P1|=0x08
#define set_P1_2                         P1|=0x04
#define set_P1_1                         P1|=0x02
#define set_P1_0                         P1|=0x01

#define clr_P1_7                         P1&=0x7F
#define clr_P1_6                         P1&=0xBF
#define clr_P1_5                         P1&=0xDF
#define clr_P1_4                         P1&=0xEF
#define clr_P1_3                         P1&=0xF7
#define clr_P1_2                         P1&=0xFB
#define clr_P1_1                         P1&=0xFD
#define clr_P1_0                         P1&=0xFE

/**** SFRS  91H  PAGE A ****/
#define set_SFRS_SFRPSEL1                SFRS|=0x02
#define set_SFRS_SFRPSEL0                SFRS|=0x01

#define clr_SFRS_SFRPSEL1                SFRS&=0xFD
#define clr_SFRS_SFRPSEL0                SFRS&=0xFE

/**** SCON  98H  PAGE A ****/
#define set_SCON_SM0                     SCON|=0x80
#define set_SCON_FE                      SCON|=0x80
#define set_SCON_SM1                     SCON|=0x40
#define set_SCON_SM2                     SCON|=0x20
#define set_SCON_REN                     SCON|=0x10
#define set_SCON_TB8                     SCON|=0x08
#define set_SCON_RB8                     SCON|=0x04
#define set_SCON_TI                      SCON|=0x02
#define set_SCON_RI                      SCON|=0x01

#define clr_SCON_SM0                     SCON&=0x7F
#define clr_SCON_FE                      SCON&=0x7F
#define clr_SCON_SM1                     SCON&=0xBF
#define clr_SCON_SM2                     SCON&=0xDF
#define clr_SCON_REN                     SCON&=0xEF
#define clr_SCON_TB8                     SCON&=0xF7
#define clr_SCON_RB8                     SCON&=0xFB
#define clr_SCON_TI                      SCON&=0xFD
#define clr_SCON_RI                      SCON&=0xFE

/**** P2  A0H  PAGE A ****/
#define set_P2_7                         P2|=0x80
#define set_P2_6                         P2|=0x40
#define set_P2_5                         P2|=0x20
#define set_P2_4                         P2|=0x10
#define set_P2_3                         P2|=0x08
#define set_P2_2                         P2|=0x04
#define set_P2_1                         P2|=0x02
#define set_P2_0                         P2|=0x01

#define clr_P2_7                         P2&=0x7F
#define clr_P2_6                         P2&=0xBF
#define clr_P2_5                         P2&=0xDF
#define clr_P2_4                         P2&=0xEF
#define clr_P2_3                         P2&=0xF7
#define clr_P2_2                         P2&=0xFB
#define clr_P2_1                         P2&=0xFD
#define clr_P2_0                         P2&=0xFE

/**** IE  A8H  PAGE A ****/
#define set_IE_EA                        IE|=0x80
#define set_IE_EADC                      IE|=0x40
#define set_IE_EBOD                      IE|=0x20
#define set_IE_ES                        IE|=0x10
#define set_IE_ET1                       IE|=0x08
#define set_IE_EX1                       IE|=0x04
#define set_IE_ET0                       IE|=0x02
#define set_IE_EX0                       IE|=0x01

#define clr_IE_EA                        IE&=0x7F
#define clr_IE_EADC                      IE&=0xBF
#define clr_IE_EBOD                      IE&=0xDF
#define clr_IE_ES                        IE&=0xEF
#define clr_IE_ET1                       IE&=0xF7
#define clr_IE_EX1                       IE&=0xFB
#define clr_IE_ET0                       IE&=0xFD
#define clr_IE_EX0                       IE&=0xFE

/**** P3  B0H  PAGE A ****/
#define set_P3_7                         P3|=0x80
#define set_P3_6                         P3|=0x40
#define set_P3_5                         P3|=0x20
#define set_P3_4                         P3|=0x10
#define set_P3_3                         P3|=0x08
#define set_P3_2                         P3|=0x04
#define set_P3_1                         P3|=0x02
#define set_P3_0                         P3|=0x01

#define clr_P3_7                         P3&=0x7F
#define clr_P3_6                         P3&=0xBF
#define clr_P3_5                         P3&=0xDF
#define clr_P3_4                         P3&=0xEF
#define clr_P3_3                         P3&=0xF7
#define clr_P3_2                         P3&=0xFB
#define clr_P3_1                         P3&=0xFD
#define clr_P3_0                         P3&=0xFE

/**** IP  B8H  PAGE A ****/
#define set_IP_PADC                      IP|=0x40
#define set_IP_PBOD                      IP|=0x20
#define set_IP_PS                        IP|=0x10
#define set_IP_PT1                       IP|=0x08
#define set_IP_PX1                       IP|=0x04
#define set_IP_PT0                       IP|=0x02
#define set_IP_PX0                       IP|=0x01

#define clr_IP_PADC                      IP&=0xBF
#define clr_IP_PBOD                      IP&=0xDF
#define clr_IP_PS                        IP&=0xEF
#define clr_IP_PT1                       IP&=0xF7
#define clr_IP_PX1                       IP&=0xFB
#define clr_IP_PT0                       IP&=0xFD
#define clr_IP_PX0                       IP&=0xFE

/**** I2CON  C0H  PAGE A ****/
#define set_I2CON_I2CEN                  I2CON|=0x40
#define set_I2CON_STA                    I2CON|=0x20
#define set_I2CON_STO                    I2CON|=0x10
#define set_I2CON_SI                     I2CON|=0x08
#define set_I2CON_AA                     I2CON|=0x04
#define set_I2CON_I2CPX                  I2CON|=0x01

#define clr_I2CON_I2CEN                  I2CON&=0xBF
#define clr_I2CON_STA                    I2CON&=0xDF
#define clr_I2CON_STO                    I2CON&=0xEF
#define clr_I2CON_SI                     I2CON&=0xF7
#define clr_I2CON_AA                     I2CON&=0xFB
#define clr_I2CON_I2CPX                  I2CON&=0xFE

/**** T2CON  C8H  PAGE A ****/
#define set_T2CON_TF2                    T2CON|=0x80
#define set_T2CON_TR2                    T2CON|=0x04
#define set_T2CON_CMRL2                  T2CON|=0x01

#define clr_T2CON_TF2                    T2CON&=0x7F
#define clr_T2CON_TR2                    T2CON&=0xFB
#define clr_T2CON_CMRL2                  T2CON&=0xFE

/**** PSW  D0H  PAGE A ****/
#define set_PSW_CY                       PSW|=0x80
#define set_PSW_AC                       PSW|=0x40
#define set_PSW_F0                       PSW|=0x20
#define set_PSW_RS1                      PSW|=0x10
#define set_PSW_RS0                      PSW|=0x08
#define set_PSW_OV                       PSW|=0x04
#define set_PSW_P                        PSW|=0x01

#define clr_PSW_CY                       PSW&=0x7F
#define clr_PSW_AC                       PSW&=0xBF
#define clr_PSW_F0                       PSW&=0xDF
#define clr_PSW_RS1                      PSW&=0xEF
#define clr_PSW_RS0                      PSW&=0xF7
#define clr_PSW_OV                       PSW&=0xFB
#define clr_PSW_P                        PSW&=0xFE

/**** PWM0CON0  D8H  PAGE A ****/
#define set_PWM0CON0_PWM0RUN             PWM0CON0|=0x80
#define set_PWM0CON0_LOAD                PWM0CON0|=0x40
#define set_PWM0CON0_PWMF                PWM0CON0|=0x20
#define set_PWM0CON0_CLRPWM              PWM0CON0|=0x10
#define set_PWM0CON0_PWM3RUN             PWM0CON0|=0x08
#define set_PWM0CON0_PWM2RUN             PWM0CON0|=0x04
#define set_PWM0CON0_PWM1RUN             PWM0CON0|=0x02
#define set_PWM0CON0_P33FBINEN           PWM0CON0|=0x01

#define clr_PWM0CON0_PWM0RUN             PWM0CON0&=0x7F
#define clr_PWM0CON0_LOAD                PWM0CON0&=0xBF
#define clr_PWM0CON0_PWMF                PWM0CON0&=0xDF
#define clr_PWM0CON0_CLRPWM              PWM0CON0&=0xEF
#define clr_PWM0CON0_PWM3RUN             PWM0CON0&=0xF7
#define clr_PWM0CON0_PWM2RUN             PWM0CON0&=0xFB
#define clr_PWM0CON0_PWM1RUN             PWM0CON0&=0xFD
#define clr_PWM0CON0_P33FBINEN           PWM0CON0&=0xFE

/**** ACC  E0H  PAGE A ****/
#define set_ACC_7                        ACC|=0x80
#define set_ACC_6                        ACC|=0x40
#define set_ACC_5                        ACC|=0x20
#define set_ACC_4                        ACC|=0x10
#define set_ACC_3                        ACC|=0x08
#define set_ACC_2                        ACC|=0x04
#define set_ACC_1                        ACC|=0x02
#define set_ACC_0                        ACC|=0x01

#define clr_ACC_7                        ACC&=0x7F
#define clr_ACC_6                        ACC&=0xBF
#define clr_ACC_5                        ACC&=0xDF
#define clr_ACC_4                        ACC&=0xEF
#define clr_ACC_3                        ACC&=0xF7
#define clr_ACC_2                        ACC&=0xFB
#define clr_ACC_1                        ACC&=0xFD
#define clr_ACC_0                        ACC&=0xFE

/**** ADCCON0  E8H  PAGE A ****/
#define set_ADCCON0_ADCF                 SFRS=0;ADCCON0|=0x80
#define set_ADCCON0_ADCS                 SFRS=0;ADCCON0|=0x40
#define set_ADCCON0_ETGSEL1              SFRS=0;ADCCON0|=0x20
#define set_ADCCON0_ETGSEL0              SFRS=0;ADCCON0|=0x10
#define set_ADCCON0_ADCHS3               SFRS=0;ADCCON0|=0x08
#define set_ADCCON0_ADCHS2               SFRS=0;ADCCON0|=0x04
#define set_ADCCON0_ADCHS1               SFRS=0;ADCCON0|=0x02
#define set_ADCCON0_ADCHS0               SFRS=0;ADCCON0|=0x01

#define clr_ADCCON0_ADCF                 SFRS=0;ADCCON0&=0x7F
#define clr_ADCCON0_ADCS                 SFRS=0;ADCCON0&=0xBF
#define clr_ADCCON0_ETGSEL1              SFRS=0;ADCCON0&=0xDF
#define clr_ADCCON0_ETGSEL0              SFRS=0;ADCCON0&=0xEF
#define clr_ADCCON0_ADCHS3               SFRS=0;ADCCON0&=0xF7
#define clr_ADCCON0_ADCHS2               SFRS=0;ADCCON0&=0xFB
#define clr_ADCCON0_ADCHS1               SFRS=0;ADCCON0&=0xFD
#define clr_ADCCON0_ADCHS0               SFRS=0;ADCCON0&=0xFE

/**** B  F0H  PAGE A ****/
#define set_B_7                          B|=0x80
#define set_B_6                          B|=0x40
#define set_B_5                          B|=0x20
#define set_B_4                          B|=0x10
#define set_B_3                          B|=0x08
#define set_B_2                          B|=0x04
#define set_B_1                          B|=0x02
#define set_B_0                          B|=0x01

#define clr_B_7                          B&=0x7F
#define clr_B_6                          B&=0xBF
#define clr_B_5                          B&=0xDF
#define clr_B_4                          B&=0xEF
#define clr_B_3                          B&=0xF7
#define clr_B_2                          B&=0xFB
#define clr_B_1                          B&=0xFD
#define clr_B_0                          B&=0xFE

/**** SCON_1  F8H  PAGE A ****/
#define set_SCON_1_SM0_1                 SCON_1|=0x80
#define set_SCON_1_FE_1                  SCON_1|=0x80
#define set_SCON_1_SM1_1                 SCON_1|=0x40
#define set_SCON_1_SM2_1                 SCON_1|=0x20
#define set_SCON_1_REN_1                 SCON_1|=0x10
#define set_SCON_1_TB8_1                 SCON_1|=0x08
#define set_SCON_1_RB8_1                 SCON_1|=0x04
#define set_SCON_1_TI_1                  SCON_1|=0x02
#define set_SCON_1_RI_1                  SCON_1|=0x01

#define clr_SCON_1_SM0_1                 SCON_1&=0x7F
#define clr_SCON_1_FE_1                  SCON_1&=0x7F
#define clr_SCON_1_SM1_1                 SCON_1&=0xBF
#define clr_SCON_1_SM2_1                 SCON_1&=0xDF
#define clr_SCON_1_REN_1                 SCON_1&=0xEF
#define clr_SCON_1_TB8_1                 SCON_1&=0xF7
#define clr_SCON_1_RB8_1                 SCON_1&=0xFB
#define clr_SCON_1_TI_1                  SCON_1&=0xFD
#define clr_SCON_1_RI_1                  SCON_1&=0xFE

/********SFR PAGE 0*************/
/**** RCTRIM1  85H  PAGE 0 TA protect register ****/
#define set_RCTRIM1_HIRC24               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;RCTRIM1|=0x10;EA=BIT_TMP
#define set_RCTRIM1_HIRCTRIM_0           SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;RCTRIM1|=0x01;EA=BIT_TMP

#define clr_RCTRIM1_HIRC24               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;RCTRIM1&=0xEF;EA=BIT_TMP
#define clr_RCTRIM1_HIRCTRIM_0           SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;RCTRIM1&=0xFE;EA=BIT_TMP

/**** TMOD  89H  PAGE 0 ****/
/**** CKCON  8EH  PAGE 0 ****/

#define set_CKCON_PWMCKS                 SFRS=0;CKCON|=0x40
#define set_CKCON_T1M                    SFRS=0;CKCON|=0x10
#define set_CKCON_T0M                    SFRS=0;CKCON|=0x08
#define set_CKCON_CLOEN                  SFRS=0;CKCON|=0x02


#define clr_CKCON_PWMCKS                 SFRS=0;CKCON&=0xBF
#define clr_CKCON_T1M                    SFRS=0;CKCON&=0xEF
#define clr_CKCON_T0M                    SFRS=0;CKCON&=0xF7
#define clr_CKCON_CLOEN                  SFRS=0;CKCON&=0xFD

/**** WKCON  8FH  PAGE 0 ****/
#define set_WKCON_WKTF                   SFRS=0;WKCON|=0x10
#define set_WKCON_WKTR                   SFRS=0;WKCON|=0x08
#define set_WKCON_WKPS2                  SFRS=0;WKCON|=0x04
#define set_WKCON_WKPS1                  SFRS=0;WKCON|=0x02
#define set_WKCON_WKPS0                  SFRS=0;WKCON|=0x01

#define clr_WKCON_WKTF                   SFRS=0;WKCON&=0xEF
#define clr_WKCON_WKTR                   SFRS=0;WKCON&=0xF7
#define clr_WKCON_WKPS2                  SFRS=0;WKCON&=0xFB
#define clr_WKCON_WKPS1                  SFRS=0;WKCON&=0xFD
#define clr_WKCON_WKPS0                  SFRS=0;WKCON&=0xFE

/**** CAPCON0  92H  PAGE 0 ****/
#define set_CAPCON0_CAPEN2               SFRS=0;CAPCON0|=0x40
#define set_CAPCON0_CAPEN1               SFRS=0;CAPCON0|=0x20
#define set_CAPCON0_CAPEN0               SFRS=0;CAPCON0|=0x10
#define set_CAPCON0_CAPF2                SFRS=0;CAPCON0|=0x04
#define set_CAPCON0_CAPF1                SFRS=0;CAPCON0|=0x02
#define set_CAPCON0_CAPF0                SFRS=0;CAPCON0|=0x01

#define clr_CAPCON0_CAPEN2               SFRS=0;CAPCON0&=0xBF
#define clr_CAPCON0_CAPEN1               SFRS=0;CAPCON0&=0xDF
#define clr_CAPCON0_CAPEN0               SFRS=0;CAPCON0&=0xEF
#define clr_CAPCON0_CAPF2                SFRS=0;CAPCON0&=0xFB
#define clr_CAPCON0_CAPF1                SFRS=0;CAPCON0&=0xFD
#define clr_CAPCON0_CAPF0                SFRS=0;CAPCON0&=0xFE

/**** CAPCON1  93H  PAGE 0 ****/
#define set_CAPCON1_CAP2LS1              SFRS=0;CAPCON1|=0x20
#define set_CAPCON1_CAP2LS0              SFRS=0;CAPCON1|=0x10
#define set_CAPCON1_CAP1LS1              SFRS=0;CAPCON1|=0x08
#define set_CAPCON1_CAP1LS0              SFRS=0;CAPCON1|=0x04
#define set_CAPCON1_CAP0LS1              SFRS=0;CAPCON1|=0x02
#define set_CAPCON1_CAP0LS0              SFRS=0;CAPCON1|=0x01

#define clr_CAPCON1_CAP2LS1              SFRS=0;CAPCON1&=0xDF
#define clr_CAPCON1_CAP2LS0              SFRS=0;CAPCON1&=0xEF
#define clr_CAPCON1_CAP1LS1              SFRS=0;CAPCON1&=0xF7
#define clr_CAPCON1_CAP1LS0              SFRS=0;CAPCON1&=0xFB
#define clr_CAPCON1_CAP0LS1              SFRS=0;CAPCON1&=0xFD
#define clr_CAPCON1_CAP0LS0              SFRS=0;CAPCON1&=0xFE

/**** CAPCON2  94H  PAGE 0 ****/
#define set_CAPCON2_ENF2                 SFRS=0;CAPCON2|=0x40
#define set_CAPCON2_ENF1                 SFRS=0;CAPCON2|=0x20
#define set_CAPCON2_ENF0                 SFRS=0;CAPCON2|=0x10

#define clr_CAPCON2_ENF2                 SFRS=0;CAPCON2&=0xBF
#define clr_CAPCON2_ENF1                 SFRS=0;CAPCON2&=0xDF
#define clr_CAPCON2_ENF0                 SFRS=0;CAPCON2&=0xEF

/**** CKSWT  96H  PAGE 0 TA protect register ****/
#define set_CKSWT_HXTST                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x80;EA=BIT_TMP
#define set_CKSWT_ECKP00ST               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x40;EA=BIT_TMP
#define set_CKSWT_HIRCST                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x20;EA=BIT_TMP
#define set_CKSWT_ECKP30ST               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x08;EA=BIT_TMP
#define set_CKSWT_OSC1                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x04;EA=BIT_TMP
#define set_CKSWT_OSC0                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT|=0x02;EA=BIT_TMP

#define clr_CKSWT_HXTST                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0x7F;EA=BIT_TMP
#define clr_CKSWT_ECKP00ST               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0xBF;EA=BIT_TMP
#define clr_CKSWT_HIRCST                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0xDF;EA=BIT_TMP
#define clr_CKSWT_ECKP30ST               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0xF7;EA=BIT_TMP
#define clr_CKSWT_OSC1                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0xFB;EA=BIT_TMP
#define clr_CKSWT_OSC0                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKSWT&=0xFD;EA=BIT_TMP

/**** CKEN  97H  PAGE 0 TA protect register ****/
#define set_CKEN_EXTEN1                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN|=0x80;EA=BIT_TMP
#define set_CKEN_EXTEN0                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN|=0x40;EA=BIT_TMP
#define set_CKEN_HIRCEN                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN|=0x20;EA=BIT_TMP
#define set_CKEN_LIRCEN                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN|=0x10;EA=BIT_TMP
#define set_CKEN_CKSWTF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN|=0x01;EA=BIT_TMP

#define clr_CKEN_EXTEN1                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN&=0x7F;EA=BIT_TMP
#define clr_CKEN_EXTEN0                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN&=0xBF;EA=BIT_TMP
#define clr_CKEN_HIRCEN                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN&=0xDF;EA=BIT_TMP
#define clr_CKEN_LIRCEN                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN&=0xEF;EA=BIT_TMP
#define clr_CKEN_CKSWTF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CKEN&=0xFE;EA=BIT_TMP

/**** EIE  9BH  PAGE 0 ****/
#define set_EIE_ET2                      SFRS=0;EIE|=0x80
#define set_EIE_ESPI                     SFRS=0;EIE|=0x40
#define set_EIE_EFB                      SFRS=0;EIE|=0x20
#define set_EIE_EWDT                     SFRS=0;EIE|=0x10
#define set_EIE_EPWM0                    SFRS=0;EIE|=0x08
#define set_EIE_ECAP                     SFRS=0;EIE|=0x04
#define set_EIE_EPI                      SFRS=0;EIE|=0x02
#define set_EIE_EI2C                     SFRS=0;EIE|=0x01

#define clr_EIE_ET2                      SFRS=0;EIE&=0x7F
#define clr_EIE_ESPI                     SFRS=0;EIE&=0xBF
#define clr_EIE_EFB                      SFRS=0;EIE&=0xDF
#define clr_EIE_EWDT                     SFRS=0;EIE&=0xEF
#define clr_EIE_EPWM0                    SFRS=0;EIE&=0xF7
#define clr_EIE_ECAP                     SFRS=0;EIE&=0xFB
#define clr_EIE_EPI                      SFRS=0;EIE&=0xFD
#define clr_EIE_EI2C                     SFRS=0;EIE&=0xFE

/**** EIE1  9CH  PAGE 0 ****/
#define set_EIE1_EPWM3                   SFRS=0;EIE1|=0x20
#define set_EIE1_EPWM2                   SFRS=0;EIE1|=0x10
#define set_EIE1_EPWM1                   SFRS=0;EIE1|=0x08
#define set_EIE1_EWKT                    SFRS=0;EIE1|=0x04
#define set_EIE1_ET3                     SFRS=0;EIE1|=0x02
#define set_EIE1_ES_1                    SFRS=0;EIE1|=0x01

#define clr_EIE1_EPWM3                   SFRS=0;EIE1&=0xDF
#define clr_EIE1_EPWM2                   SFRS=0;EIE1&=0xEF
#define clr_EIE1_EPWM1                   SFRS=0;EIE1&=0xF7
#define clr_EIE1_EWKT                    SFRS=0;EIE1&=0xFB
#define clr_EIE1_ET3                     SFRS=0;EIE1&=0xFD
#define clr_EIE1_ES_1                    SFRS=0;EIE1&=0xFE

/**** CHPCON  9FH  PAGE 0 TA protect register ****/
#define set_CHPCON_SWRST                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON|=0x80;EA=BIT_TMP
#define set_CHPCON_IAPFF                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON|=0x40;EA=BIT_TMP
#define set_CHPCON_BS                    SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON|=0x02;EA=BIT_TMP
#define set_CHPCON_IAPEN                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON|=0x01;EA=BIT_TMP

#define clr_CHPCON_SWRST                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON&=0x7F;EA=BIT_TMP
#define clr_CHPCON_IAPFF                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON&=0xBF;EA=BIT_TMP
#define clr_CHPCON_BS                    SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON&=0xFD;EA=BIT_TMP
#define clr_CHPCON_IAPEN                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON&=0xFE;EA=BIT_TMP

/**** AUXR1  A2H  PAGE 0 ****/
#define set_AUXR1_SWRF                   SFRS=0;AUXR1|=0x80
#define set_AUXR1_RSTPINF                SFRS=0;AUXR1|=0x40
#define set_AUXR1_HardF                  SFRS=0;AUXR1|=0x20
#define set_AUXR1_GF2                    SFRS=0;AUXR1|=0x08
#define set_AUXR1_UART0PX                SFRS=0;AUXR1|=0x04
#define set_AUXR1_0                      SFRS=0;AUXR1|=0x02
#define set_AUXR1_DPS                    SFRS=0;AUXR1|=0x01

#define clr_AUXR1_SWRF                   SFRS=0;AUXR1&=0x7F
#define clr_AUXR1_RSTPINF                SFRS=0;AUXR1&=0xBF
#define clr_AUXR1_HardF                  SFRS=0;AUXR1&=0xDF
#define clr_AUXR1_GF2                    SFRS=0;AUXR1&=0xF7
#define clr_AUXR1_UART0PX                SFRS=0;AUXR1&=0xFB
#define clr_AUXR1_0                      SFRS=0;AUXR1&=0xFD
#define clr_AUXR1_DPS                    SFRS=0;AUXR1&=0xFE

/**** BODCON0  A3H  PAGE 0 TA protect register ****/
#define set_BODCON0_BODEN                SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x80;EA=BIT_TMP
#define set_BODCON0_BOV1                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x20;EA=BIT_TMP
#define set_BODCON0_BOV0                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x10;EA=BIT_TMP
#define set_BODCON0_BOF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x08;EA=BIT_TMP
#define set_BODCON0_BORST                SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x04;EA=BIT_TMP
#define set_BODCON0_BORF                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x02;EA=BIT_TMP
#define set_BODCON0_BOS                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0|=0x01;EA=BIT_TMP

#define clr_BODCON0_BODEN                SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0x7F;EA=BIT_TMP
#define clr_BODCON0_BOV1                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xDF;EA=BIT_TMP
#define clr_BODCON0_BOV0                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xEF;EA=BIT_TMP
#define clr_BODCON0_BOF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xF7;EA=BIT_TMP
#define clr_BODCON0_BORST                SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xFB;EA=BIT_TMP
#define clr_BODCON0_BORF                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xFD;EA=BIT_TMP
#define clr_BODCON0_BOS                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON0&=0xFE;EA=BIT_TMP

/**** IAPTRG  A4H  PAGE 0 TA protect register ****/
#define set_IAPTRG_IAPGO                 SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPTRG|=0x01;EA=BIT_TMP
#define set_IAPTRG_IAPGO_WDCLR           SFRS=0;BIT_TMP=EA;EA=0;set_WDCON_WDCLR;TA=0xAA;TA=0x55;IAPTRG|=0x01;EA=BIT_TMP

/**** IAPUEN  A5H  PAGE 0 TA protect register ****/
#define set_IAPUEN_SPMEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x10;EA=BIT_TMP
#define set_IAPUEN_SPUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x08;EA=BIT_TMP
#define set_IAPUEN_CFUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x04;EA=BIT_TMP
#define set_IAPUEN_LDUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x02;EA=BIT_TMP
#define set_IAPUEN_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x01;EA=BIT_TMP
 
#define clr_IAPUEN_SPMEM   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=0xEF;EA=BIT_TMP
#define clr_IAPUEN_SPUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=0xF7;EA=BIT_TMP
#define clr_IAPUEN_CFUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=0xFB;EA=BIT_TMP
#define clr_IAPUEN_LDUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=0xFD;EA=BIT_TMP
#define clr_IAPUEN_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=0xFE;EA=BIT_TMP

/**** WDCON  AAH  PAGE 0 TA protect register ****/
#define set_WDCON_WDTR                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x80;EA=BIT_TMP
#define set_WDCON_WDCLR                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x40;EA=BIT_TMP
#define set_WDCON_WDTF                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x20;EA=BIT_TMP
#define set_WDCON_WIDPD                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x10;EA=BIT_TMP
#define set_WDCON_WDTRF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x08;EA=BIT_TMP
#define set_WDCON_WDPS2                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x04;EA=BIT_TMP
#define set_WDCON_WDPS1                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x02;EA=BIT_TMP
#define set_WDCON_WDPS0                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON|=0x01;EA=BIT_TMP

#define clr_WDCON_WDTR                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0x7F;EA=BIT_TMP
#define clr_WDCON_WDCLR                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xBF;EA=BIT_TMP
#define clr_WDCON_WDTF                   SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xDF;EA=BIT_TMP
#define clr_WDCON_WIDPD                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xEF;EA=BIT_TMP
#define clr_WDCON_WDTRF                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xF7;EA=BIT_TMP
#define clr_WDCON_WDPS2                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xFB;EA=BIT_TMP
#define clr_WDCON_WDPS1                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xFD;EA=BIT_TMP
#define clr_WDCON_WDPS0                  SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;WDCON&=0xFE;EA=BIT_TMP

/**** BODCON1  ABH  PAGE 0 TA protect register ****/
#define set_BODCON1_LPBOD1               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1|=0x04;EA=BIT_TMP
#define set_BODCON1_LPBOD0               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1|=0x02;EA=BIT_TMP
#define set_BODCON1_BODFLT               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1|=0x01;EA=BIT_TMP

#define clr_BODCON1_LPBOD1               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1&=0xFB;EA=BIT_TMP
#define clr_BODCON1_LPBOD0               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1&=0xFD;EA=BIT_TMP
#define clr_BODCON1_BODFLT               SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;BODCON1&=0xFE;EA=BIT_TMP

/**** P3M1  ACH  PAGE 0 ****/
#define set_P3M1_7                       SFRS=0;P3M1|=0x80
#define set_P3M1_6                       SFRS=0;P3M1|=0x40
#define set_P3M1_5                       SFRS=0;P3M1|=0x20
#define set_P3M1_4                       SFRS=0;P3M1|=0x10
#define set_P3M1_3                       SFRS=0;P3M1|=0x08
#define set_P3M1_2                       SFRS=0;P3M1|=0x04
#define set_P3M1_1                       SFRS=0;P3M1|=0x02
#define set_P3M1_0                       SFRS=0;P3M1|=0x01

#define clr_P3M1_7                       SFRS=0;P3M1&=0x7F
#define clr_P3M1_6                       SFRS=0;P3M1&=0xBF
#define clr_P3M1_5                       SFRS=0;P3M1&=0xDF
#define clr_P3M1_4                       SFRS=0;P3M1&=0xEF
#define clr_P3M1_3                       SFRS=0;P3M1&=0xF7
#define clr_P3M1_2                       SFRS=0;P3M1&=0xFB
#define clr_P3M1_1                       SFRS=0;P3M1&=0xFD
#define clr_P3M1_0                       SFRS=0;P3M1&=0xFE

/**** P3M2  ADH  PAGE 0 ****/
#define set_P3M2_7                       SFRS=0;P3M2|=0x80
#define set_P3M2_6                       SFRS=0;P3M2|=0x40
#define set_P3M2_5                       SFRS=0;P3M2|=0x20
#define set_P3M2_4                       SFRS=0;P3M2|=0x10
#define set_P3M2_3                       SFRS=0;P3M2|=0x08
#define set_P3M2_2                       SFRS=0;P3M2|=0x04
#define set_P3M2_1                       SFRS=0;P3M2|=0x02
#define set_P3M2_0                       SFRS=0;P3M2|=0x01

#define clr_P3M2_7                       SFRS=0;P3M2&=0x7F
#define clr_P3M2_6                       SFRS=0;P3M2&=0xBF
#define clr_P3M2_5                       SFRS=0;P3M2&=0xDF
#define clr_P3M2_4                       SFRS=0;P3M2&=0xEF
#define clr_P3M2_3                       SFRS=0;P3M2&=0xF7
#define clr_P3M2_2                       SFRS=0;P3M2&=0xFB
#define clr_P3M2_1                       SFRS=0;P3M2&=0xFD
#define clr_P3M2_0                       SFRS=0;P3M2&=0xFE

/**** IAPCN  AFH  PAGE 0 ****/
#define set_IAPCN_IAPA17                 SFRS=0;IAPCN|=0x80
#define set_IAPCN_IAPA16                 SFRS=0;IAPCN|=0x40
#define set_IAPCN_FOEN                   SFRS=0;IAPCN|=0x20
#define set_IAPCN_FCEN                   SFRS=0;IAPCN|=0x10
#define set_IAPCN_FCTRL3                 SFRS=0;IAPCN|=0x08
#define set_IAPCN_FCTRL2                 SFRS=0;IAPCN|=0x04
#define set_IAPCN_FCTRL1                 SFRS=0;IAPCN|=0x02
#define set_IAPCN_FCTRL0                 SFRS=0;IAPCN|=0x01

#define clr_IAPCN_IAPA17                 SFRS=0;IAPCN&=0x7F
#define clr_IAPCN_IAPA16                 SFRS=0;IAPCN&=0xBF
#define clr_IAPCN_FOEN                   SFRS=0;IAPCN&=0xDF
#define clr_IAPCN_FCEN                   SFRS=0;IAPCN&=0xEF
#define clr_IAPCN_FCTRL3                 SFRS=0;IAPCN&=0xF7
#define clr_IAPCN_FCTRL2                 SFRS=0;IAPCN&=0xFB
#define clr_IAPCN_FCTRL1                 SFRS=0;IAPCN&=0xFD
#define clr_IAPCN_FCTRL0                 SFRS=0;IAPCN&=0xFE

/**** P0M1  B1H  PAGE 0 ****/
#define set_P0M1_7                       SFRS=0;P0M1|=0x80
#define set_P0M1_6                       SFRS=0;P0M1|=0x40
#define set_P0M1_5                       SFRS=0;P0M1|=0x20
#define set_P0M1_4                       SFRS=0;P0M1|=0x10
#define set_P0M1_3                       SFRS=0;P0M1|=0x08
#define set_P0M1_2                       SFRS=0;P0M1|=0x04
#define set_P0M1_1                       SFRS=0;P0M1|=0x02
#define set_P0M1_0                       SFRS=0;P0M1|=0x01

#define clr_P0M1_7                       SFRS=0;P0M1&=0x7F
#define clr_P0M1_6                       SFRS=0;P0M1&=0xBF
#define clr_P0M1_5                       SFRS=0;P0M1&=0xDF
#define clr_P0M1_4                       SFRS=0;P0M1&=0xEF
#define clr_P0M1_3                       SFRS=0;P0M1&=0xF7
#define clr_P0M1_2                       SFRS=0;P0M1&=0xFB
#define clr_P0M1_1                       SFRS=0;P0M1&=0xFD
#define clr_P0M1_0                       SFRS=0;P0M1&=0xFE

/**** P0M2  B2H  PAGE 0 ****/
#define set_P0M2_7                       SFRS=0;P0M2|=0x80
#define set_P0M2_6                       SFRS=0;P0M2|=0x40
#define set_P0M2_5                       SFRS=0;P0M2|=0x20
#define set_P0M2_4                       SFRS=0;P0M2|=0x10
#define set_P0M2_3                       SFRS=0;P0M2|=0x08
#define set_P0M2_2                       SFRS=0;P0M2|=0x04
#define set_P0M2_1                       SFRS=0;P0M2|=0x02
#define set_P0M2_0                       SFRS=0;P0M2|=0x01

#define clr_P0M2_7                       SFRS=0;P0M2&=0x7F
#define clr_P0M2_6                       SFRS=0;P0M2&=0xBF
#define clr_P0M2_5                       SFRS=0;P0M2&=0xDF
#define clr_P0M2_4                       SFRS=0;P0M2&=0xEF
#define clr_P0M2_3                       SFRS=0;P0M2&=0xF7
#define clr_P0M2_2                       SFRS=0;P0M2&=0xFB
#define clr_P0M2_1                       SFRS=0;P0M2&=0xFD
#define clr_P0M2_0                       SFRS=0;P0M2&=0xFE

/**** P1M1  B3H  PAGE 0 ****/
#define set_P1M1_7                       SFRS=0;P1M1|=0x80
#define set_P1M1_6                       SFRS=0;P1M1|=0x40
#define set_P1M1_5                       SFRS=0;P1M1|=0x20
#define set_P1M1_4                       SFRS=0;P1M1|=0x10
#define set_P1M1_3                       SFRS=0;P1M1|=0x08
#define set_P1M1_2                       SFRS=0;P1M1|=0x04
#define set_P1M1_1                       SFRS=0;P1M1|=0x02
#define set_P1M1_0                       SFRS=0;P1M1|=0x01

#define clr_P1M1_7                       SFRS=0;P1M1&=0x7F
#define clr_P1M1_6                       SFRS=0;P1M1&=0xBF
#define clr_P1M1_5                       SFRS=0;P1M1&=0xDF
#define clr_P1M1_4                       SFRS=0;P1M1&=0xEF
#define clr_P1M1_3                       SFRS=0;P1M1&=0xF7
#define clr_P1M1_2                       SFRS=0;P1M1&=0xFB
#define clr_P1M1_1                       SFRS=0;P1M1&=0xFD
#define clr_P1M1_0                       SFRS=0;P1M1&=0xFE

/**** P1M2  B4H  PAGE 0 ****/
#define set_P1M2_7                       SFRS=0;P1M2|=0x80
#define set_P1M2_6                       SFRS=0;P1M2|=0x40
#define set_P1M2_5                       SFRS=0;P1M2|=0x20
#define set_P1M2_4                       SFRS=0;P1M2|=0x10
#define set_P1M2_3                       SFRS=0;P1M2|=0x08
#define set_P1M2_2                       SFRS=0;P1M2|=0x04
#define set_P1M2_1                       SFRS=0;P1M2|=0x02
#define set_P1M2_0                       SFRS=0;P1M2|=0x01

#define clr_P1M2_7                       SFRS=0;P1M2&=0x7F
#define clr_P1M2_6                       SFRS=0;P1M2&=0xBF
#define clr_P1M2_5                       SFRS=0;P1M2&=0xDF
#define clr_P1M2_4                       SFRS=0;P1M2&=0xEF
#define clr_P1M2_3                       SFRS=0;P1M2&=0xF7
#define clr_P1M2_2                       SFRS=0;P1M2&=0xFB
#define clr_P1M2_1                       SFRS=0;P1M2&=0xFD
#define clr_P1M2_0                       SFRS=0;P1M2&=0xFE

/**** TOE  B5H  PAGE 0 ****/
#define set_TOE_T1OE                     SFRS=0;TOE|=0x08
#define set_TOE_T0OE                     SFRS=0;TOE|=0x04

#define clr_TOE_T1OE                     SFRS=0;TOE&=0xF7
#define clr_TOE_T0OE                     SFRS=0;TOE&=0xFB

/**** IPH  B7H  PAGE 0 ****/
#define set_IPH_PADCH                    SFRS=0;IPH|=0x40
#define set_IPH_PBODH                    SFRS=0;IPH|=0x20
#define set_IPH_PSH                      SFRS=0;IPH|=0x10
#define set_IPH_PT1H                     SFRS=0;IPH|=0x08
#define set_IPH_PX1H                     SFRS=0;IPH|=0x04
#define set_IPH_PT0H                     SFRS=0;IPH|=0x02
#define set_IPH_PX0H                     SFRS=0;IPH|=0x01

#define clr_IPH_PADCH                    SFRS=0;IPH&=0xBF
#define clr_IPH_PBODH                    SFRS=0;IPH&=0xDF
#define clr_IPH_PSH                      SFRS=0;IPH&=0xEF
#define clr_IPH_PT1H                     SFRS=0;IPH&=0xF7
#define clr_IPH_PX1H                     SFRS=0;IPH&=0xFB
#define clr_IPH_PT0H                     SFRS=0;IPH&=0xFD
#define clr_IPH_PX0H                     SFRS=0;IPH&=0xFE

/**** I2STAT  BDH  PAGE 0 ****/
#define set_I2STAT_I2STAT7               SFRS=0;I2STAT|=0x80
#define set_I2STAT_I2STAT6               SFRS=0;I2STAT|=0x40
#define set_I2STAT_I2STAT5               SFRS=0;I2STAT|=0x20
#define set_I2STAT_I2STAT4               SFRS=0;I2STAT|=0x10
#define set_I2STAT_I2STAT3               SFRS=0;I2STAT|=0x08


#define clr_I2STAT_I2STAT7               SFRS=0;I2STAT&=0x7F
#define clr_I2STAT_I2STAT6               SFRS=0;I2STAT&=0xBF
#define clr_I2STAT_I2STAT5               SFRS=0;I2STAT&=0xDF
#define clr_I2STAT_I2STAT4               SFRS=0;I2STAT&=0xEF
#define clr_I2STAT_I2STAT3               SFRS=0;I2STAT&=0xF7


/**** I2TOC  BFH  PAGE 0 ****/
#define set_I2TOC_I2TOCEN                SFRS=0;I2TOC|=0x04
#define set_I2TOC_DIV                    SFRS=0;I2TOC|=0x02
#define set_I2TOC_I2TOF                  SFRS=0;I2TOC|=0x01

#define clr_I2TOC_I2TOCEN                SFRS=0;I2TOC&=0xFB
#define clr_I2TOC_DIV                    SFRS=0;I2TOC&=0xFD
#define clr_I2TOC_I2TOF                  SFRS=0;I2TOC&=0xFE

/**** I2ADDR  C1H  PAGE 0 ****/
#define set_I2ADDR_I2ADDR7               SFRS=0;I2ADDR|=0x80
#define set_I2ADDR_I2ADDR6               SFRS=0;I2ADDR|=0x40
#define set_I2ADDR_I2ADDR5               SFRS=0;I2ADDR|=0x20
#define set_I2ADDR_I2ADDR4               SFRS=0;I2ADDR|=0x10
#define set_I2ADDR_I2ADDR3               SFRS=0;I2ADDR|=0x08
#define set_I2ADDR_I2ADDR2               SFRS=0;I2ADDR|=0x04
#define set_I2ADDR_I2ADDR1               SFRS=0;I2ADDR|=0x02
#define set_I2ADDR_GC                    SFRS=0;I2ADDR|=0x01

#define clr_I2ADDR_I2ADDR7               SFRS=0;I2ADDR&=0x7F
#define clr_I2ADDR_I2ADDR6               SFRS=0;I2ADDR&=0xBF
#define clr_I2ADDR_I2ADDR5               SFRS=0;I2ADDR&=0xDF
#define clr_I2ADDR_I2ADDR4               SFRS=0;I2ADDR&=0xEF
#define clr_I2ADDR_I2ADDR3               SFRS=0;I2ADDR&=0xF7
#define clr_I2ADDR_I2ADDR2               SFRS=0;I2ADDR&=0xFB
#define clr_I2ADDR_I2ADDR1               SFRS=0;I2ADDR&=0xFD
#define clr_I2ADDR_GC                    SFRS=0;I2ADDR&=0xFE

/**** ADCRL  C2H  PAGE 0 ****/
#define set_ADCRL_ADCR3                  SFRS=0;ADCRL|=0x08
#define set_ADCRL_ADCR2                  SFRS=0;ADCRL|=0x04
#define set_ADCRL_ADCR1                  SFRS=0;ADCRL|=0x02
#define set_ADCRL_ADCR0                  SFRS=0;ADCRL|=0x01

#define clr_ADCRL_ADCR3                  SFRS=0;ADCRL&=0xF7
#define clr_ADCRL_ADCR2                  SFRS=0;ADCRL&=0xFB
#define clr_ADCRL_ADCR1                  SFRS=0;ADCRL&=0xFD
#define clr_ADCRL_ADCR0                  SFRS=0;ADCRL&=0xFE

/**** T3CON  C4H  PAGE 0 ****/
#define set_T3CON_SMOD_1                 SFRS=0;T3CON|=0x80
#define set_T3CON_SMOD0_1                SFRS=0;T3CON|=0x40
#define set_T3CON_BRCK                   SFRS=0;T3CON|=0x20
#define set_T3CON_TF3                    SFRS=0;T3CON|=0x10
#define set_T3CON_TR3                    SFRS=0;T3CON|=0x08
#define set_T3CON_T3PS2                  SFRS=0;T3CON|=0x04
#define set_T3CON_T3PS1                  SFRS=0;T3CON|=0x02
#define set_T3CON_T3PS0                  SFRS=0;T3CON|=0x01

#define clr_T3CON_SMOD_1                 SFRS=0;T3CON&=0x7F
#define clr_T3CON_SMOD0_1                SFRS=0;T3CON&=0xBF
#define clr_T3CON_BRCK                   SFRS=0;T3CON&=0xDF
#define clr_T3CON_TF3                    SFRS=0;T3CON&=0xEF
#define clr_T3CON_TR3                    SFRS=0;T3CON&=0xF7
#define clr_T3CON_T3PS2                  SFRS=0;T3CON&=0xFB
#define clr_T3CON_T3PS1                  SFRS=0;T3CON&=0xFD
#define clr_T3CON_T3PS0                  SFRS=0;T3CON&=0xFE

/**** T2MOD  C9H  PAGE 0 ****/
#define set_T2MOD_LDEN                   SFRS=0;T2MOD|=0x80
#define set_T2MOD_T2DIV2                 SFRS=0;T2MOD|=0x40
#define set_T2MOD_T2DIV1                 SFRS=0;T2MOD|=0x20
#define set_T2MOD_T2DIV0                 SFRS=0;T2MOD|=0x10
#define set_T2MOD_CAPCR                  SFRS=0;T2MOD|=0x08
#define set_T2MOD_CMPCR                  SFRS=0;T2MOD|=0x04
#define set_T2MOD_LDTS1                  SFRS=0;T2MOD|=0x02
#define set_T2MOD_LDTS0                  SFRS=0;T2MOD|=0x01

#define clr_T2MOD_LDEN                   SFRS=0;T2MOD&=0x7F
#define clr_T2MOD_T2DIV2                 SFRS=0;T2MOD&=0xBF
#define clr_T2MOD_T2DIV1                 SFRS=0;T2MOD&=0xDF
#define clr_T2MOD_T2DIV0                 SFRS=0;T2MOD&=0xEF
#define clr_T2MOD_CAPCR                  SFRS=0;T2MOD&=0xF7
#define clr_T2MOD_CMPCR                  SFRS=0;T2MOD&=0xFB
#define clr_T2MOD_LDTS1                  SFRS=0;T2MOD&=0xFD
#define clr_T2MOD_LDTS0                  SFRS=0;T2MOD&=0xFE

/**** ADCMPL  CEH  PAGE 0 ****/
#define set_ADCMPL_ADCMP3                SFRS=0;ADCMPL|=0x08
#define set_ADCMPL_ADCMP2                SFRS=0;ADCMPL|=0x04
#define set_ADCMPL_ADCMP1                SFRS=0;ADCMPL|=0x02
#define set_ADCMPL_ADCMP0                SFRS=0;ADCMPL|=0x01

#define clr_ADCMPL_ADCMP3                SFRS=0;ADCMPL&=0xF7
#define clr_ADCMPL_ADCMP2                SFRS=0;ADCMPL&=0xFB
#define clr_ADCMPL_ADCMP1                SFRS=0;ADCMPL&=0xFD
#define clr_ADCMPL_ADCMP0                SFRS=0;ADCMPL&=0xFE

/**** PNP  D6H  PAGE 0 ****/
#define set_PNP_PNP5                     SFRS=0;PNP|=0x20
#define set_PNP_PNP4                     SFRS=0;PNP|=0x10
#define set_PNP_PNP3                     SFRS=0;PNP|=0x08
#define set_PNP_PNP2                     SFRS=0;PNP|=0x04
#define set_PNP_PNP1                     SFRS=0;PNP|=0x02
#define set_PNP_PNP0                     SFRS=0;PNP|=0x01

#define clr_PNP_PNP5                     SFRS=0;PNP&=0xDF
#define clr_PNP_PNP4                     SFRS=0;PNP&=0xEF
#define clr_PNP_PNP3                     SFRS=0;PNP&=0xF7
#define clr_PNP_PNP2                     SFRS=0;PNP&=0xFB
#define clr_PNP_PNP1                     SFRS=0;PNP&=0xFD
#define clr_PNP_PNP0                     SFRS=0;PNP&=0xFE

/**** PWM0FBD  D7H  PAGE 0 ****/
#define set_PWM0FBD_FBF                  SFRS=0;PWM0FBD|=0x80
#define set_PWM0FBD_FBINLS               SFRS=0;PWM0FBD|=0x40
#define set_PWM0FBD_FBD5                 SFRS=0;PWM0FBD|=0x20
#define set_PWM0FBD_FBD4                 SFRS=0;PWM0FBD|=0x10
#define set_PWM0FBD_FBD3                 SFRS=0;PWM0FBD|=0x08
#define set_PWM0FBD_FBD2                 SFRS=0;PWM0FBD|=0x04
#define set_PWM0FBD_FBD1                 SFRS=0;PWM0FBD|=0x02
#define set_PWM0FBD_R                    SFRS=0;PWM0FBD|=0x01
#define set_PWM0FBD_W                    SFRS=0;PWM0FBD|=0x01

#define clr_PWM0FBD_FBF                  SFRS=0;PWM0FBD&=0x7F
#define clr_PWM0FBD_FBINLS               SFRS=0;PWM0FBD&=0xBF
#define clr_PWM0FBD_FBD5                 SFRS=0;PWM0FBD&=0xDF
#define clr_PWM0FBD_FBD4                 SFRS=0;PWM0FBD&=0xEF
#define clr_PWM0FBD_FBD3                 SFRS=0;PWM0FBD&=0xF7
#define clr_PWM0FBD_FBD2                 SFRS=0;PWM0FBD&=0xFB
#define clr_PWM0FBD_FBD1                 SFRS=0;PWM0FBD&=0xFD
#define clr_PWM0FBD_R                    SFRS=0;PWM0FBD&=0xFE
#define clr_PWM0FBD_W                    SFRS=0;PWM0FBD&=0xFE

/**** PIOCON0  DEH  PAGE 0 ****/
#define set_PIOCON0_PIO03           SFRS=0;PIOCON0|=0x20
#define set_PIOCON0_PIO01           SFRS=0;PIOCON0|=0x10
#define set_PIOCON0_PIO00           SFRS=0;PIOCON0|=0x08
#define set_PIOCON0_PIO10           SFRS=0;PIOCON0|=0x04
#define set_PIOCON0_PIO11           SFRS=0;PIOCON0|=0x02
#define set_PIOCON0_PIO12           SFRS=0;PIOCON0|=0x01

#define clr_PIOCON0_PIO03           SFRS=0;PIOCON0&=0xDF
#define clr_PIOCON0_PIO01           SFRS=0;PIOCON0&=0xEF
#define clr_PIOCON0_PIO00           SFRS=0;PIOCON0&=0xF7
#define clr_PIOCON0_PIO10           SFRS=0;PIOCON0&=0xFB
#define clr_PIOCON0_PIO11           SFRS=0;PIOCON0&=0xFD
#define clr_PIOCON0_PIO12           SFRS=0;PIOCON0&=0xFE

/**** PWM0CON1  DFH  PAGE 0 ****/
#define set_PWM0CON1_PWMMOD1             SFRS=0;PWM0CON1|=0x80
#define set_PWM0CON1_PWMMOD0             SFRS=0;PWM0CON1|=0x40
#define set_PWM0CON1_GP                  SFRS=0;PWM0CON1|=0x20
#define set_PWM0CON1_PWMTYP              SFRS=0;PWM0CON1|=0x10
#define set_PWM0CON1_FBINEN              SFRS=0;PWM0CON1|=0x08
#define set_PWM0CON1_PWMDIV2             SFRS=0;PWM0CON1|=0x04
#define set_PWM0CON1_PWMDIV1             SFRS=0;PWM0CON1|=0x02
#define set_PWM0CON1_PWMDIV0             SFRS=0;PWM0CON1|=0x01

#define clr_PWM0CON1_PWMMOD1             SFRS=0;PWM0CON1&=0x7F
#define clr_PWM0CON1_PWMMOD0             SFRS=0;PWM0CON1&=0xBF
#define clr_PWM0CON1_GP                  SFRS=0;PWM0CON1&=0xDF
#define clr_PWM0CON1_PWMTYP              SFRS=0;PWM0CON1&=0xEF
#define clr_PWM0CON1_FBINEN              SFRS=0;PWM0CON1&=0xF7
#define clr_PWM0CON1_PWMDIV2             SFRS=0;PWM0CON1&=0xFB
#define clr_PWM0CON1_PWMDIV1             SFRS=0;PWM0CON1&=0xFD
#define clr_PWM0CON1_PWMDIV0             SFRS=0;PWM0CON1&=0xFE

/**** ADCCON1  E1H  PAGE 0 ****/
#define set_ADCCON1_OCEN                 SFRS=0;ADCCON1|=0x80
#define set_ADCCON1_STADCPX              SFRS=0;ADCCON1|=0x40
#define set_ADCCON1_ADCDIV1              SFRS=0;ADCCON1|=0x20
#define set_ADCCON1_ADCDIV0              SFRS=0;ADCCON1|=0x10
#define set_ADCCON1_ETGTYP1              SFRS=0;ADCCON1|=0x08
#define set_ADCCON1_ETGTYP0              SFRS=0;ADCCON1|=0x04
#define set_ADCCON1_ADCEX                SFRS=0;ADCCON1|=0x02
#define set_ADCCON1_ADCEN                SFRS=0;ADCCON1|=0x01

#define clr_ADCCON1_OCEN                 SFRS=0;ADCCON1&=0x7F
#define clr_ADCCON1_STADCPX              SFRS=0;ADCCON1&=0xBF
#define clr_ADCCON1_ADCDIV1              SFRS=0;ADCCON1&=0xDF
#define clr_ADCCON1_ADCDIV0              SFRS=0;ADCCON1&=0xEF
#define clr_ADCCON1_ETGTYP1              SFRS=0;ADCCON1&=0xF7
#define clr_ADCCON1_ETGTYP0              SFRS=0;ADCCON1&=0xFB
#define clr_ADCCON1_ADCEX                SFRS=0;ADCCON1&=0xFD
#define clr_ADCCON1_ADCEN                SFRS=0;ADCCON1&=0xFE

/**** ADCCON2  E2H  PAGE 0 ****/
#define set_ADCCON2_ADFBEN               SFRS=0;ADCCON2|=0x80
#define set_ADCCON2_ADCMPOP              SFRS=0;ADCCON2|=0x40
#define set_ADCCON2_ADCMPEN              SFRS=0;ADCCON2|=0x20
#define set_ADCCON2_ADCMPO               SFRS=0;ADCCON2|=0x10
#define set_ADCCON2_ADCAQT0_2            SFRS=0;ADCCON2|=0x08
#define set_ADCCON2_ADCAQT0_1            SFRS=0;ADCCON2|=0x04
#define set_ADCCON2_ADCAQT0_0            SFRS=0;ADCCON2|=0x02
#define set_ADCCON2_ADCDLY_8             SFRS=0;ADCCON2|=0x01

#define clr_ADCCON2_ADFBEN               SFRS=0;ADCCON2&=0x7F
#define clr_ADCCON2_ADCMPOP              SFRS=0;ADCCON2&=0xBF
#define clr_ADCCON2_ADCMPEN              SFRS=0;ADCCON2&=0xDF
#define clr_ADCCON2_ADCMPO               SFRS=0;ADCCON2&=0xEF
#define clr_ADCCON2_ADCAQT0_2            SFRS=0;ADCCON2&=0xF7
#define clr_ADCCON2_ADCAQT0_1            SFRS=0;ADCCON2&=0xFB
#define clr_ADCCON2_ADCAQT0_0            SFRS=0;ADCCON2&=0xFD
#define clr_ADCCON2_ADCDLY_8             SFRS=0;ADCCON2&=0xFE

/**** PICON  E9H  PAGE 0 ****/
#define set_PICON_PIT7                   SFRS=0;PICON|=0x80
#define set_PICON_PIT6                   SFRS=0;PICON|=0x40
#define set_PICON_PIT5                   SFRS=0;PICON|=0x20
#define set_PICON_PIT4                   SFRS=0;PICON|=0x10
#define set_PICON_PIT3                   SFRS=0;PICON|=0x08
#define set_PICON_PIT2                   SFRS=0;PICON|=0x04
#define set_PICON_PIT1                   SFRS=0;PICON|=0x02
#define set_PICON_PIT0                   SFRS=0;PICON|=0x01

#define clr_PICON_PIT7                   SFRS=0;PICON&=0x7F
#define clr_PICON_PIT6                   SFRS=0;PICON&=0xBF
#define clr_PICON_PIT5                   SFRS=0;PICON&=0xDF
#define clr_PICON_PIT4                   SFRS=0;PICON&=0xEF
#define clr_PICON_PIT3                   SFRS=0;PICON&=0xF7
#define clr_PICON_PIT2                   SFRS=0;PICON&=0xFB
#define clr_PICON_PIT1                   SFRS=0;PICON&=0xFD
#define clr_PICON_PIT0                   SFRS=0;PICON&=0xFE

/**** PINEN  EAH  PAGE 0 ****/
#define set_PINEN_PINEN7                 SFRS=0;PINEN|=0x80
#define set_PINEN_PINEN6                 SFRS=0;PINEN|=0x40
#define set_PINEN_PINEN5                 SFRS=0;PINEN|=0x20
#define set_PINEN_PINEN4                 SFRS=0;PINEN|=0x10
#define set_PINEN_PINEN3                 SFRS=0;PINEN|=0x08
#define set_PINEN_PINEN2                 SFRS=0;PINEN|=0x04
#define set_PINEN_PINEN1                 SFRS=0;PINEN|=0x02
#define set_PINEN_PINEN0                 SFRS=0;PINEN|=0x01

#define clr_PINEN_PINEN7                 SFRS=0;PINEN&=0x7F
#define clr_PINEN_PINEN6                 SFRS=0;PINEN&=0xBF
#define clr_PINEN_PINEN5                 SFRS=0;PINEN&=0xDF
#define clr_PINEN_PINEN4                 SFRS=0;PINEN&=0xEF
#define clr_PINEN_PINEN3                 SFRS=0;PINEN&=0xF7
#define clr_PINEN_PINEN2                 SFRS=0;PINEN&=0xFB
#define clr_PINEN_PINEN1                 SFRS=0;PINEN&=0xFD
#define clr_PINEN_PINEN0                 SFRS=0;PINEN&=0xFE

/**** PIPEN  EBH  PAGE 0 ****/
#define set_PIPEN_PIPEN7                 SFRS=0;PIPEN|=0x80
#define set_PIPEN_PIPEN6                 SFRS=0;PIPEN|=0x40
#define set_PIPEN_PIPEN5                 SFRS=0;PIPEN|=0x20
#define set_PIPEN_PIPEN4                 SFRS=0;PIPEN|=0x10
#define set_PIPEN_PIPEN3                 SFRS=0;PIPEN|=0x08
#define set_PIPEN_PIPEN2                 SFRS=0;PIPEN|=0x04
#define set_PIPEN_PIPEN1                 SFRS=0;PIPEN|=0x02
#define set_PIPEN_PIPEN0                 SFRS=0;PIPEN|=0x01

#define clr_PIPEN_PIPEN7                 SFRS=0;PIPEN&=0x7F
#define clr_PIPEN_PIPEN6                 SFRS=0;PIPEN&=0xBF
#define clr_PIPEN_PIPEN5                 SFRS=0;PIPEN&=0xDF
#define clr_PIPEN_PIPEN4                 SFRS=0;PIPEN&=0xEF
#define clr_PIPEN_PIPEN3                 SFRS=0;PIPEN&=0xF7
#define clr_PIPEN_PIPEN2                 SFRS=0;PIPEN&=0xFB
#define clr_PIPEN_PIPEN1                 SFRS=0;PIPEN&=0xFD
#define clr_PIPEN_PIPEN0                 SFRS=0;PIPEN&=0xFE

/**** PIF  ECH  PAGE 0 ****/
#define set_PIF_PIF7                     SFRS=0;PIF|=0x80
#define set_PIF_PIF6                     SFRS=0;PIF|=0x40
#define set_PIF_PIF5                     SFRS=0;PIF|=0x20
#define set_PIF_PIF4                     SFRS=0;PIF|=0x10
#define set_PIF_PIF3                     SFRS=0;PIF|=0x08
#define set_PIF_PIF2                     SFRS=0;PIF|=0x04
#define set_PIF_PIF1                     SFRS=0;PIF|=0x02
#define set_PIF_PIF0                     SFRS=0;PIF|=0x01

#define clr_PIF_PIF7                     SFRS=0;PIF&=0x7F
#define clr_PIF_PIF6                     SFRS=0;PIF&=0xBF
#define clr_PIF_PIF5                     SFRS=0;PIF&=0xDF
#define clr_PIF_PIF4                     SFRS=0;PIF&=0xEF
#define clr_PIF_PIF3                     SFRS=0;PIF&=0xF7
#define clr_PIF_PIF2                     SFRS=0;PIF&=0xFB
#define clr_PIF_PIF1                     SFRS=0;PIF&=0xFD
#define clr_PIF_PIF0                     SFRS=0;PIF&=0xFE

/**** EIP  EFH  PAGE 0 ****/
#define set_EIP_PT2                      SFRS=0;EIP|=0x80
#define set_EIP_PSPI                     SFRS=0;EIP|=0x40
#define set_EIP_PFB                      SFRS=0;EIP|=0x20
#define set_EIP_PWDT                     SFRS=0;EIP|=0x10
#define set_EIP_PPWM                     SFRS=0;EIP|=0x08
#define set_EIP_PCAP                     SFRS=0;EIP|=0x04
#define set_EIP_PPI                      SFRS=0;EIP|=0x02
#define set_EIP_PI2C                     SFRS=0;EIP|=0x01

#define clr_EIP_PT2                      SFRS=0;EIP&=0x7F
#define clr_EIP_PSPI                     SFRS=0;EIP&=0xBF
#define clr_EIP_PFB                      SFRS=0;EIP&=0xDF
#define clr_EIP_PWDT                     SFRS=0;EIP&=0xEF
#define clr_EIP_PPWM                     SFRS=0;EIP&=0xF7
#define clr_EIP_PCAP                     SFRS=0;EIP&=0xFB
#define clr_EIP_PPI                      SFRS=0;EIP&=0xFD
#define clr_EIP_PI2C                     SFRS=0;EIP&=0xFE

/**** CAPCON3  F1H  PAGE 0 ****/
#define set_CAPCON3_CAP13                SFRS=0;CAPCON3|=0x80
#define set_CAPCON3_CAP12                SFRS=0;CAPCON3|=0x40
#define set_CAPCON3_CAP11                SFRS=0;CAPCON3|=0x20
#define set_CAPCON3_CAP10                SFRS=0;CAPCON3|=0x10
#define set_CAPCON3_CAP03                SFRS=0;CAPCON3|=0x08
#define set_CAPCON3_CAP02                SFRS=0;CAPCON3|=0x04
#define set_CAPCON3_CAP01                SFRS=0;CAPCON3|=0x02
#define set_CAPCON3_CAP00                SFRS=0;CAPCON3|=0x01

#define clr_CAPCON3_CAP13                SFRS=0;CAPCON3&=0x7F
#define clr_CAPCON3_CAP12                SFRS=0;CAPCON3&=0xBF
#define clr_CAPCON3_CAP11                SFRS=0;CAPCON3&=0xDF
#define clr_CAPCON3_CAP10                SFRS=0;CAPCON3&=0xEF
#define clr_CAPCON3_CAP03                SFRS=0;CAPCON3&=0xF7
#define clr_CAPCON3_CAP02                SFRS=0;CAPCON3&=0xFB
#define clr_CAPCON3_CAP01                SFRS=0;CAPCON3&=0xFD
#define clr_CAPCON3_CAP00                SFRS=0;CAPCON3&=0xFE

/**** CAPCON4  F2H  PAGE 0 ****/
#define set_CAPCON4_CAP23                SFRS=0;CAPCON4|=0x08
#define set_CAPCON4_CAP22                SFRS=0;CAPCON4|=0x04
#define set_CAPCON4_CAP21                SFRS=0;CAPCON4|=0x02
#define set_CAPCON4_CAP20                SFRS=0;CAPCON4|=0x01

#define clr_CAPCON4_CAP23                SFRS=0;CAPCON4&=0xF7
#define clr_CAPCON4_CAP22                SFRS=0;CAPCON4&=0xFB
#define clr_CAPCON4_CAP21                SFRS=0;CAPCON4&=0xFD
#define clr_CAPCON4_CAP20                SFRS=0;CAPCON4&=0xFE

/**** SPCR  F3H  PAGE 0 ****/
#define set_SPCR_SSOE                    SFRS=0;SPCR|=0x80
#define set_SPCR_SPIEN                   SFRS=0;SPCR|=0x40
#define set_SPCR_LSBFE                   SFRS=0;SPCR|=0x20
#define set_SPCR_MSTR                    SFRS=0;SPCR|=0x10
#define set_SPCR_CPOL                    SFRS=0;SPCR|=0x08
#define set_SPCR_CPHA                    SFRS=0;SPCR|=0x04
#define set_SPCR_SPR1                    SFRS=0;SPCR|=0x02
#define set_SPCR_SPR0                    SFRS=0;SPCR|=0x01

#define clr_SPCR_SSOE                    SFRS=0;SPCR&=0x7F
#define clr_SPCR_SPIEN                   SFRS=0;SPCR&=0xBF
#define clr_SPCR_LSBFE                   SFRS=0;SPCR&=0xDF
#define clr_SPCR_MSTR                    SFRS=0;SPCR&=0xEF
#define clr_SPCR_CPOL                    SFRS=0;SPCR&=0xF7
#define clr_SPCR_CPHA                    SFRS=0;SPCR&=0xFB
#define clr_SPCR_SPR1                    SFRS=0;SPCR&=0xFD
#define clr_SPCR_SPR0                    SFRS=0;SPCR&=0xFE

/**** SPSR  F4H  PAGE 0 ****/
#define set_SPSR_SPIF                    SFRS=0;SPSR|=0x80
#define set_SPSR_WCOL                    SFRS=0;SPSR|=0x40
#define set_SPSR_SPIOVF                  SFRS=0;SPSR|=0x20
#define set_SPSR_MODF                    SFRS=0;SPSR|=0x10
#define set_SPSR_DISMODF                 SFRS=0;SPSR|=0x08
#define set_SPSR_TXBUF                   SFRS=0;SPSR|=0x04

#define clr_SPSR_SPIF                    SFRS=0;SPSR&=0x7F
#define clr_SPSR_WCOL                    SFRS=0;SPSR&=0xBF
#define clr_SPSR_SPIOVF                  SFRS=0;SPSR&=0xDF
#define clr_SPSR_MODF                    SFRS=0;SPSR&=0xEF
#define clr_SPSR_DISMODF                 SFRS=0;SPSR&=0xF7
#define clr_SPSR_TXBUF                   SFRS=0;SPSR&=0xFB

/**** AINDIDS0  F6H  PAGE 0 ****/
#define set_AINDIDS0_P11DIDS             SFRS=0;AINDIDS0|=0x80
#define set_AINDIDS0_P03DIDS             SFRS=0;AINDIDS0|=0x40
#define set_AINDIDS0_P04DIDS             SFRS=0;AINDIDS0|=0x20
#define set_AINDIDS0_P05DIDS             SFRS=0;AINDIDS0|=0x10
#define set_AINDIDS0_P06DIDS             SFRS=0;AINDIDS0|=0x08
#define set_AINDIDS0_P07DIDS             SFRS=0;AINDIDS0|=0x04
#define set_AINDIDS0_P30DIDS             SFRS=0;AINDIDS0|=0x02
#define set_AINDIDS0_P17DIDS             SFRS=0;AINDIDS0|=0x01

#define clr_AINDIDS0_P11DIDS             SFRS=0;AINDIDS0&=0x7F
#define clr_AINDIDS0_P03DIDS             SFRS=0;AINDIDS0&=0xBF
#define clr_AINDIDS0_P04DIDS             SFRS=0;AINDIDS0&=0xDF
#define clr_AINDIDS0_P05DIDS             SFRS=0;AINDIDS0&=0xEF
#define clr_AINDIDS0_P06DIDS             SFRS=0;AINDIDS0&=0xF7
#define clr_AINDIDS0_P07DIDS             SFRS=0;AINDIDS0&=0xFB
#define clr_AINDIDS0_P30DIDS             SFRS=0;AINDIDS0&=0xFD
#define clr_AINDIDS0_P17DIDS             SFRS=0;AINDIDS0&=0xFE

/**** EIPH  F7H  PAGE 0 ****/
#define set_EIPH_PT2H                    SFRS=0;EIPH|=0x80
#define set_EIPH_PSPIH                   SFRS=0;EIPH|=0x40
#define set_EIPH_PFBH                    SFRS=0;EIPH|=0x20
#define set_EIPH_PWDTH                   SFRS=0;EIPH|=0x10
#define set_EIPH_PPWMH                   SFRS=0;EIPH|=0x08
#define set_EIPH_PCAPH                   SFRS=0;EIPH|=0x04
#define set_EIPH_PPIH                    SFRS=0;EIPH|=0x02
#define set_EIPH_PI2CH                   SFRS=0;EIPH|=0x01

#define clr_EIPH_PT2H                    SFRS=0;EIPH&=0x7F
#define clr_EIPH_PSPIH                   SFRS=0;EIPH&=0xBF
#define clr_EIPH_PFBH                    SFRS=0;EIPH&=0xDF
#define clr_EIPH_PWDTH                   SFRS=0;EIPH&=0xEF
#define clr_EIPH_PPWMH                   SFRS=0;EIPH&=0xF7
#define clr_EIPH_PCAPH                   SFRS=0;EIPH&=0xFB
#define clr_EIPH_PPIH                    SFRS=0;EIPH&=0xFD
#define clr_EIPH_PI2CH                   SFRS=0;EIPH&=0xFE

/**** PWM0DTEN  F9H  PAGE 0 TA protect register ****/
#define set_PWM0DTEN_PWM0DTCNT_8         SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN|=0x10;EA=BIT_TMP
#define set_PWM0DTEN_PDT45EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN|=0x04;EA=BIT_TMP
#define set_PWM0DTEN_PDT23EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN|=0x02;EA=BIT_TMP
#define set_PWM0DTEN_PDT01EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN|=0x01;EA=BIT_TMP

#define clr_PWM0DTEN_PWM0DTCNT_8         SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN&=0xEF;EA=BIT_TMP
#define clr_PWM0DTEN_PDT45EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN&=0xFB;EA=BIT_TMP
#define clr_PWM0DTEN_PDT23EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN&=0xFD;EA=BIT_TMP
#define clr_PWM0DTEN_PDT01EN             SFRS=0;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PWM0DTEN&=0xFE;EA=BIT_TMP

/**** PWM0MEN  FBH  PAGE 0 ****/
#define set_PWM0MEN_PMEN5                SFRS=0;PWM0MEN|=0x20
#define set_PWM0MEN_PMEN4                SFRS=0;PWM0MEN|=0x10
#define set_PWM0MEN_PMEN3                SFRS=0;PWM0MEN|=0x08
#define set_PWM0MEN_PMEN2                SFRS=0;PWM0MEN|=0x04
#define set_PWM0MEN_PMEN1                SFRS=0;PWM0MEN|=0x02
#define set_PWM0MEN_PMEN0                SFRS=0;PWM0MEN|=0x01

#define clr_PWM0MEN_PMEN5                SFRS=0;PWM0MEN&=0xDF
#define clr_PWM0MEN_PMEN4                SFRS=0;PWM0MEN&=0xEF
#define clr_PWM0MEN_PMEN3                SFRS=0;PWM0MEN&=0xF7
#define clr_PWM0MEN_PMEN2                SFRS=0;PWM0MEN&=0xFB
#define clr_PWM0MEN_PMEN1                SFRS=0;PWM0MEN&=0xFD
#define clr_PWM0MEN_PMEN0                SFRS=0;PWM0MEN&=0xFE

/**** PWM0MD  FCH  PAGE 0 ****/
#define set_PWM0MD_PMD5                  SFRS=0;PWM0MD|=0x20
#define set_PWM0MD_PMD4                  SFRS=0;PWM0MD|=0x10
#define set_PWM0MD_PMD3                  SFRS=0;PWM0MD|=0x08
#define set_PWM0MD_PMD2                  SFRS=0;PWM0MD|=0x04
#define set_PWM0MD_PMD1                  SFRS=0;PWM0MD|=0x02
#define set_PWM0MD_PMD0                  SFRS=0;PWM0MD|=0x01

#define clr_PWM0MD_PMD5                  SFRS=0;PWM0MD&=0xDF
#define clr_PWM0MD_PMD4                  SFRS=0;PWM0MD&=0xEF
#define clr_PWM0MD_PMD3                  SFRS=0;PWM0MD&=0xF7
#define clr_PWM0MD_PMD2                  SFRS=0;PWM0MD&=0xFB
#define clr_PWM0MD_PMD1                  SFRS=0;PWM0MD&=0xFD
#define clr_PWM0MD_PMD0                  SFRS=0;PWM0MD&=0xFE

/**** EIPH1  FFH  PAGE 0 ****/
#define set_EIPH1_PPWM3H                 SFRS=0;EIPH1|=0x20
#define set_EIPH1_PPWM2H                 SFRS=0;EIPH1|=0x10
#define set_EIPH1_PPWM1H                 SFRS=0;EIPH1|=0x08
#define set_EIPH1_PWKTH                  SFRS=0;EIPH1|=0x04
#define set_EIPH1_PT3H                   SFRS=0;EIPH1|=0x02
#define set_EIPH1_PSH_1                  SFRS=0;EIPH1|=0x01

#define clr_EIPH1_PPWM3H                 SFRS=0;EIPH1&=0xDF
#define clr_EIPH1_PPWM2H                 SFRS=0;EIPH1&=0xEF
#define clr_EIPH1_PPWM1H                 SFRS=0;EIPH1&=0xF7
#define clr_EIPH1_PWKTH                  SFRS=0;EIPH1&=0xFB
#define clr_EIPH1_PT3H                   SFRS=0;EIPH1&=0xFD
#define clr_EIPH1_PSH_1                  SFRS=0;EIPH1&=0xFE

/********SFR PAGE 1*************/
/**** P3S  ACH  PAGE 1 ****/
#define set_P3S_7                        SFRS=1;P3S|=0x80
#define set_P3S_6                        SFRS=1;P3S|=0x40
#define set_P3S_5                        SFRS=1;P3S|=0x20
#define set_P3S_4                        SFRS=1;P3S|=0x10
#define set_P3S_3                        SFRS=1;P3S|=0x08
#define set_P3S_2                        SFRS=1;P3S|=0x04
#define set_P3S_1                        SFRS=1;P3S|=0x02
#define set_P3S_0                        SFRS=1;P3S|=0x01

#define clr_P3S_7                        SFRS=1;P3S&=0x7F
#define clr_P3S_6                        SFRS=1;P3S&=0xBF
#define clr_P3S_5                        SFRS=1;P3S&=0xDF
#define clr_P3S_4                        SFRS=1;P3S&=0xEF
#define clr_P3S_3                        SFRS=1;P3S&=0xF7
#define clr_P3S_2                        SFRS=1;P3S&=0xFB
#define clr_P3S_1                        SFRS=1;P3S&=0xFD
#define clr_P3S_0                        SFRS=1;P3S&=0xFE

/**** P3SR  ADH  PAGE 1 ****/
#define set_P3SR_7                       SFRS=1;P3SR|=0x80
#define set_P3SR_6                       SFRS=1;P3SR|=0x40
#define set_P3SR_5                       SFRS=1;P3SR|=0x20
#define set_P3SR_4                       SFRS=1;P3SR|=0x10
#define set_P3SR_3                       SFRS=1;P3SR|=0x08
#define set_P3SR_2                       SFRS=1;P3SR|=0x04
#define set_P3SR_1                       SFRS=1;P3SR|=0x02
#define set_P3SR_0                       SFRS=1;P3SR|=0x01

#define clr_P3SR_7                       SFRS=1;P3SR&=0x7F
#define clr_P3SR_6                       SFRS=1;P3SR&=0xBF
#define clr_P3SR_5                       SFRS=1;P3SR&=0xDF
#define clr_P3SR_4                       SFRS=1;P3SR&=0xEF
#define clr_P3SR_3                       SFRS=1;P3SR&=0xF7
#define clr_P3SR_2                       SFRS=1;P3SR&=0xFB
#define clr_P3SR_1                       SFRS=1;P3SR&=0xFD
#define clr_P3SR_0                       SFRS=1;P3SR&=0xFE

/**** PWM0INTC  AFH  PAGE 1 ****/
#define set_PWM0INTC_INTTYP1             SFRS=1;PWM0INTC|=0x20
#define set_PWM0INTC_INTTYP0             SFRS=1;PWM0INTC|=0x10
#define set_PWM0INTC_INTSEL2             SFRS=1;PWM0INTC|=0x04
#define set_PWM0INTC_INTSEL1             SFRS=1;PWM0INTC|=0x02
#define set_PWM0INTC_INTSEL0             SFRS=1;PWM0INTC|=0x01

#define clr_PWM0INTC_INTTYP1             SFRS=1;PWM0INTC&=0xDF
#define clr_PWM0INTC_INTTYP0             SFRS=1;PWM0INTC&=0xEF
#define clr_PWM0INTC_INTSEL2             SFRS=1;PWM0INTC&=0xFB
#define clr_PWM0INTC_INTSEL1             SFRS=1;PWM0INTC&=0xFD
#define clr_PWM0INTC_INTSEL0             SFRS=1;PWM0INTC&=0xFE

/**** P0S  B1H  PAGE 1 ****/
#define set_P0S_7                        SFRS=1;P0S|=0x80
#define set_P0S_6                        SFRS=1;P0S|=0x40
#define set_P0S_5                        SFRS=1;P0S|=0x20
#define set_P0S_4                        SFRS=1;P0S|=0x10
#define set_P0S_3                        SFRS=1;P0S|=0x08
#define set_P0S_2                        SFRS=1;P0S|=0x04
#define set_P0S_1                        SFRS=1;P0S|=0x02
#define set_P0S_0                        SFRS=1;P0S|=0x01

#define clr_P0S_7                        SFRS=1;P0S&=0x7F
#define clr_P0S_6                        SFRS=1;P0S&=0xBF
#define clr_P0S_5                        SFRS=1;P0S&=0xDF
#define clr_P0S_4                        SFRS=1;P0S&=0xEF
#define clr_P0S_3                        SFRS=1;P0S&=0xF7
#define clr_P0S_2                        SFRS=1;P0S&=0xFB
#define clr_P0S_1                        SFRS=1;P0S&=0xFD
#define clr_P0S_0                        SFRS=1;P0S&=0xFE

/**** P0SR  B2H  PAGE 1 ****/
#define set_P0SR_7                       SFRS=1;P0SR|=0x80
#define set_P0SR_6                       SFRS=1;P0SR|=0x40
#define set_P0SR_5                       SFRS=1;P0SR|=0x20
#define set_P0SR_4                       SFRS=1;P0SR|=0x10
#define set_P0SR_3                       SFRS=1;P0SR|=0x08
#define set_P0SR_2                       SFRS=1;P0SR|=0x04
#define set_P0SR_1                       SFRS=1;P0SR|=0x02
#define set_P0SR_0                       SFRS=1;P0SR|=0x01

#define clr_P0SR_7                       SFRS=1;P0SR&=0x7F
#define clr_P0SR_6                       SFRS=1;P0SR&=0xBF
#define clr_P0SR_5                       SFRS=1;P0SR&=0xDF
#define clr_P0SR_4                       SFRS=1;P0SR&=0xEF
#define clr_P0SR_3                       SFRS=1;P0SR&=0xF7
#define clr_P0SR_2                       SFRS=1;P0SR&=0xFB
#define clr_P0SR_1                       SFRS=1;P0SR&=0xFD
#define clr_P0SR_0                       SFRS=1;P0SR&=0xFE

/**** P1S  B3H  PAGE 1 ****/
#define set_P1S_7                        SFRS=1;P1S|=0x80
#define set_P1S_6                        SFRS=1;P1S|=0x40
#define set_P1S_5                        SFRS=1;P1S|=0x20
#define set_P1S_4                        SFRS=1;P1S|=0x10
#define set_P1S_3                        SFRS=1;P1S|=0x08
#define set_P1S_2                        SFRS=1;P1S|=0x04
#define set_P1S_1                        SFRS=1;P1S|=0x02
#define set_P1S_0                        SFRS=1;P1S|=0x01

#define clr_P1S_7                        SFRS=1;P1S&=0x7F
#define clr_P1S_6                        SFRS=1;P1S&=0xBF
#define clr_P1S_5                        SFRS=1;P1S&=0xDF
#define clr_P1S_4                        SFRS=1;P1S&=0xEF
#define clr_P1S_3                        SFRS=1;P1S&=0xF7
#define clr_P1S_2                        SFRS=1;P1S&=0xFB
#define clr_P1S_1                        SFRS=1;P1S&=0xFD
#define clr_P1S_0                        SFRS=1;P1S&=0xFE

/**** P1SR  B4H  PAGE 1 ****/
#define set_P1SR_7                       SFRS=1;P1SR|=0x80
#define set_P1SR_6                       SFRS=1;P1SR|=0x40
#define set_P1SR_5                       SFRS=1;P1SR|=0x20
#define set_P1SR_4                       SFRS=1;P1SR|=0x10
#define set_P1SR_3                       SFRS=1;P1SR|=0x08
#define set_P1SR_2                       SFRS=1;P1SR|=0x04
#define set_P1SR_1                       SFRS=1;P1SR|=0x02
#define set_P1SR_0                       SFRS=1;P1SR|=0x01

#define clr_P1SR_7                       SFRS=1;P1SR&=0x7F
#define clr_P1SR_6                       SFRS=1;P1SR&=0xBF
#define clr_P1SR_5                       SFRS=1;P1SR&=0xDF
#define clr_P1SR_4                       SFRS=1;P1SR&=0xEF
#define clr_P1SR_3                       SFRS=1;P1SR&=0xF7
#define clr_P1SR_2                       SFRS=1;P1SR&=0xFB
#define clr_P1SR_1                       SFRS=1;P1SR&=0xFD
#define clr_P1SR_0                       SFRS=1;P1SR&=0xFE

/**** PIOCON1  C6H  PAGE1 ****/ 
#define set_PIOCON1_PIO17                SFRS=1;PIOCON1|=0x80   
#define set_PIOCON1_PIO15                SFRS=1;PIOCON1|=0x20
#define set_PIOCON1_PIO04                SFRS=1;PIOCON1|=0x08
#define set_PIOCON1_PIO05                SFRS=1;PIOCON1|=0x04
#define set_PIOCON1_PIO14                SFRS=1;PIOCON1|=0x02
                                         
#define clr_PIOCON1_PIO17                SFRS=1;PIOCON1&=0x7F
#define clr_PIOCON1_PIO15                SFRS=1;PIOCON1&=0xDF
#define clr_PIOCON1_PIO04                SFRS=1;PIOCON1&=0xF7
#define clr_PIOCON1_PIO05                SFRS=1;PIOCON1&=0xFB
#define clr_PIOCON1_PIO14                SFRS=1;PIOCON1&=0xFD

/**** SPCR2  F3H  PAGE 1 ****/
#define set_SPCR2_SPIS1                  SFRS=1;SPCR2|=0x02
#define set_SPCR2_SPIS0                  SFRS=1;SPCR2|=0x01


#define clr_SPCR2_SPIS1                  SFRS=1;SPCR2&=0xFD
#define clr_SPCR2_SPIS0                  SFRS=1;SPCR2&=0xFE

/**** EIP1  FEH  PAGE 1 ****/
#define set_EIP1_PPWM3                   SFRS=1;EIP1|=0x20
#define set_EIP1_PPWM2                   SFRS=1;EIP1|=0x10
#define set_EIP1_PPWM1                   SFRS=1;EIP1|=0x08
#define set_EIP1_PWKT                    SFRS=1;EIP1|=0x04
#define set_EIP1_PT3                     SFRS=1;EIP1|=0x02
#define set_EIP1_PS_1                    SFRS=1;EIP1|=0x01

#define clr_EIP1_PPWM3                   SFRS=1;EIP1&=0xDF
#define clr_EIP1_PPWM2                   SFRS=1;EIP1&=0xEF
#define clr_EIP1_PPWM1                   SFRS=1;EIP1&=0xF7
#define clr_EIP1_PWKT                    SFRS=1;EIP1&=0xFB
#define clr_EIP1_PT3                     SFRS=1;EIP1&=0xFD
#define clr_EIP1_PS_1                    SFRS=1;EIP1&=0xFE

/********SFR PAGE 2*************/
/**** ADCBAH  85H  PAGE 2 ****/
#define set_ADCBAH_ADCBA3                SFRS=2;ADCBAH|=0x08
#define set_ADCBAH_ADCBA2                SFRS=2;ADCBAH|=0x04
#define set_ADCBAH_ADCBA1                SFRS=2;ADCBAH|=0x02
#define set_ADCBAH_ADCBA0                SFRS=2;ADCBAH|=0x01

#define clr_ADCBAH_ADCBA3                SFRS=2;ADCBAH&=0xF7
#define clr_ADCBAH_ADCBA2                SFRS=2;ADCBAH&=0xFB
#define clr_ADCBAH_ADCBA1                SFRS=2;ADCBAH&=0xFD
#define clr_ADCBAH_ADCBA0                SFRS=2;ADCBAH&=0xFE

/**** ADCCON3  86H  PAGE 2 ****/
#define set_ADCCON3_HIE                  SFRS=2;ADCCON3|=0x20
#define set_ADCCON3_CONT                 SFRS=2;ADCCON3|=0x10
#define set_ADCCON3_ADCAQT1_2            SFRS=2;ADCCON3|=0x08
#define set_ADCCON3_ADCAQT1_1            SFRS=2;ADCCON3|=0x04
#define set_ADCCON3_ADCAQT1_0            SFRS=2;ADCCON3|=0x02
#define set_ADCCON3_SLOW                 SFRS=2;ADCCON3|=0x01

#define clr_ADCCON3_HIE                  SFRS=2;ADCCON3&=0xDF
#define clr_ADCCON3_CONT                 SFRS=2;ADCCON3&=0xEF
#define clr_ADCCON3_ADCAQT1_2            SFRS=2;ADCCON3&=0xF7
#define clr_ADCCON3_ADCAQT1_1            SFRS=2;ADCCON3&=0xFB
#define clr_ADCCON3_ADCAQT1_0            SFRS=2;ADCCON3&=0xFD
#define clr_ADCCON3_SLOW                 SFRS=2;ADCCON3&=0xFE

/**** P2M1  89H  PAGE 2 ****/
#define set_P2M1_7                       SFRS=2;P2M1|=0x80
#define set_P2M1_6                       SFRS=2;P2M1|=0x40
#define set_P2M1_5                       SFRS=2;P2M1|=0x20
#define set_P2M1_4                       SFRS=2;P2M1|=0x10
#define set_P2M1_3                       SFRS=2;P2M1|=0x08
#define set_P2M1_2                       SFRS=2;P2M1|=0x04
#define set_P2M1_1                       SFRS=2;P2M1|=0x02
#define set_P2M1_0                       SFRS=2;P2M1|=0x01

#define clr_P2M1_7                       SFRS=2;P2M1&=0x7F
#define clr_P2M1_6                       SFRS=2;P2M1&=0xBF
#define clr_P2M1_5                       SFRS=2;P2M1&=0xDF
#define clr_P2M1_4                       SFRS=2;P2M1&=0xEF
#define clr_P2M1_3                       SFRS=2;P2M1&=0xF7
#define clr_P2M1_2                       SFRS=2;P2M1&=0xFB
#define clr_P2M1_1                       SFRS=2;P2M1&=0xFD
#define clr_P2M1_0                       SFRS=2;P2M1&=0xFE

/**** P2M2  8AH  PAGE 2 ****/
#define set_P2M2_7                       SFRS=2;P2M2|=0x80
#define set_P2M2_6                       SFRS=2;P2M2|=0x40
#define set_P2M2_5                       SFRS=2;P2M2|=0x20
#define set_P2M2_4                       SFRS=2;P2M2|=0x10
#define set_P2M2_3                       SFRS=2;P2M2|=0x08
#define set_P2M2_2                       SFRS=2;P2M2|=0x04
#define set_P2M2_1                       SFRS=2;P2M2|=0x02
#define set_P2M2_0                       SFRS=2;P2M2|=0x01

#define clr_P2M2_7                       SFRS=2;P2M2&=0x7F
#define clr_P2M2_6                       SFRS=2;P2M2&=0xBF
#define clr_P2M2_5                       SFRS=2;P2M2&=0xDF
#define clr_P2M2_4                       SFRS=2;P2M2&=0xEF
#define clr_P2M2_3                       SFRS=2;P2M2&=0xF7
#define clr_P2M2_2                       SFRS=2;P2M2&=0xFB
#define clr_P2M2_1                       SFRS=2;P2M2&=0xFD
#define clr_P2M2_0                       SFRS=2;P2M2&=0xFE

/**** P2SR  8BH  PAGE 2 ****/
#define set_P2SR_7                       SFRS=2;P2SR|=0x80
#define set_P2SR_6                       SFRS=2;P2SR|=0x40
#define set_P2SR_5                       SFRS=2;P2SR|=0x20
#define set_P2SR_4                       SFRS=2;P2SR|=0x10
#define set_P2SR_3                       SFRS=2;P2SR|=0x08
#define set_P2SR_2                       SFRS=2;P2SR|=0x04
#define set_P2SR_1                       SFRS=2;P2SR|=0x02
#define set_P2SR_0                       SFRS=2;P2SR|=0x01

#define clr_P2SR_7                       SFRS=2;P2SR&=0x7F
#define clr_P2SR_6                       SFRS=2;P2SR&=0xBF
#define clr_P2SR_5                       SFRS=2;P2SR&=0xDF
#define clr_P2SR_4                       SFRS=2;P2SR&=0xEF
#define clr_P2SR_3                       SFRS=2;P2SR&=0xF7
#define clr_P2SR_2                       SFRS=2;P2SR&=0xFB
#define clr_P2SR_1                       SFRS=2;P2SR&=0xFD
#define clr_P2SR_0                       SFRS=2;P2SR&=0xFE

/**** P2S  8CH  PAGE 2 ****/
#define set_P2S_7                        SFRS=2;P2S|=0x80
#define set_P2S_6                        SFRS=2;P2S|=0x40
#define set_P2S_5                        SFRS=2;P2S|=0x20
#define set_P2S_4                        SFRS=2;P2S|=0x10
#define set_P2S_3                        SFRS=2;P2S|=0x08
#define set_P2S_2                        SFRS=2;P2S|=0x04
#define set_P2S_1                        SFRS=2;P2S|=0x02
#define set_P2S_0                        SFRS=2;P2S|=0x01

#define clr_P2S_7                        SFRS=2;P2S&=0x7F
#define clr_P2S_6                        SFRS=2;P2S&=0xBF
#define clr_P2S_5                        SFRS=2;P2S&=0xDF
#define clr_P2S_4                        SFRS=2;P2S&=0xEF
#define clr_P2S_3                        SFRS=2;P2S&=0xF7
#define clr_P2S_2                        SFRS=2;P2S&=0xFB
#define clr_P2S_1                        SFRS=2;P2S&=0xFD
#define clr_P2S_0                        SFRS=2;P2S&=0xFE

/**** ADCSR  8FH  PAGE 2 ****/
#define set_ADCSR_CMPHIT                 SFRS=2;ADCSR|=0x04
#define set_ADCSR_HDONE                  SFRS=2;ADCSR|=0x02
#define set_ADCSR_FDONE                  SFRS=2;ADCSR|=0x01

#define clr_ADCSR_CMPHIT                 SFRS=2;ADCSR&=0xFB
#define clr_ADCSR_HDONE                  SFRS=2;ADCSR&=0xFD
#define clr_ADCSR_FDONE                  SFRS=2;ADCSR&=0xFE

/**** P0UP  92H  PAGE 2 ****/
#define set_P0UP_7                       SFRS=2;P0UP|=0x80
#define set_P0UP_6                       SFRS=2;P0UP|=0x40
#define set_P0UP_5                       SFRS=2;P0UP|=0x20
#define set_P0UP_4                       SFRS=2;P0UP|=0x10
#define set_P0UP_3                       SFRS=2;P0UP|=0x08
#define set_P0UP_2                       SFRS=2;P0UP|=0x04
#define set_P0UP_1                       SFRS=2;P0UP|=0x02
#define set_P0UP_0                       SFRS=2;P0UP|=0x01

#define clr_P0UP_7                       SFRS=2;P0UP&=0x7F
#define clr_P0UP_6                       SFRS=2;P0UP&=0xBF
#define clr_P0UP_5                       SFRS=2;P0UP&=0xDF
#define clr_P0UP_4                       SFRS=2;P0UP&=0xEF
#define clr_P0UP_3                       SFRS=2;P0UP&=0xF7
#define clr_P0UP_2                       SFRS=2;P0UP&=0xFB
#define clr_P0UP_1                       SFRS=2;P0UP&=0xFD
#define clr_P0UP_0                       SFRS=2;P0UP&=0xFE

/**** P1UP  93H  PAGE 2 ****/
#define set_P1UP_7                       SFRS=2;P1UP|=0x80
#define set_P1UP_6                       SFRS=2;P1UP|=0x40
#define set_P1UP_5                       SFRS=2;P1UP|=0x20
#define set_P1UP_4                       SFRS=2;P1UP|=0x10
#define set_P1UP_3                       SFRS=2;P1UP|=0x08
#define set_P1UP_2                       SFRS=2;P1UP|=0x04
#define set_P1UP_1                       SFRS=2;P1UP|=0x02
#define set_P1UP_0                       SFRS=2;P1UP|=0x01

#define clr_P1UP_7                       SFRS=2;P1UP&=0x7F
#define clr_P1UP_6                       SFRS=2;P1UP&=0xBF
#define clr_P1UP_5                       SFRS=2;P1UP&=0xDF
#define clr_P1UP_4                       SFRS=2;P1UP&=0xEF
#define clr_P1UP_3                       SFRS=2;P1UP&=0xF7
#define clr_P1UP_2                       SFRS=2;P1UP&=0xFB
#define clr_P1UP_1                       SFRS=2;P1UP&=0xFD
#define clr_P1UP_0                       SFRS=2;P1UP&=0xFE

/**** P2UP  94H  PAGE 2 ****/
#define set_P2UP_5                       SFRS=2;P2UP|=0x20
#define set_P2UP_4                       SFRS=2;P2UP|=0x10
#define set_P2UP_3                       SFRS=2;P2UP|=0x08
#define set_P2UP_2                       SFRS=2;P2UP|=0x04
#define set_P2UP_1                       SFRS=2;P2UP|=0x02
#define set_P2UP_0                       SFRS=2;P2UP|=0x01

#define clr_P2UP_5                       SFRS=2;P2UP&=0xDF
#define clr_P2UP_4                       SFRS=2;P2UP&=0xEF
#define clr_P2UP_3                       SFRS=2;P2UP&=0xF7
#define clr_P2UP_2                       SFRS=2;P2UP&=0xFB
#define clr_P2UP_1                       SFRS=2;P2UP&=0xFD
#define clr_P2UP_0                       SFRS=2;P2UP&=0xFE

/**** P3UP  95H  PAGE 2 ****/
#define set_P3UP_7                       SFRS=2;P3UP|=0x80
#define set_P3UP_6                       SFRS=2;P3UP|=0x40
#define set_P3UP_5                       SFRS=2;P3UP|=0x20
#define set_P3UP_4                       SFRS=2;P3UP|=0x10
#define set_P3UP_3                       SFRS=2;P3UP|=0x08
#define set_P3UP_2                       SFRS=2;P3UP|=0x04
#define set_P3UP_1                       SFRS=2;P3UP|=0x02
#define set_P3UP_0                       SFRS=2;P3UP|=0x01

#define clr_P3UP_7                       SFRS=2;P3UP&=0x7F
#define clr_P3UP_6                       SFRS=2;P3UP&=0xBF
#define clr_P3UP_5                       SFRS=2;P3UP&=0xDF
#define clr_P3UP_4                       SFRS=2;P3UP&=0xEF
#define clr_P3UP_3                       SFRS=2;P3UP&=0xF7
#define clr_P3UP_2                       SFRS=2;P3UP&=0xFB
#define clr_P3UP_1                       SFRS=2;P3UP&=0xFD
#define clr_P3UP_0                       SFRS=2;P3UP&=0xFE

/**** AINDIDS1  99H  PAGE 2 ****/
#define set_AINDIDS1_P25DIDS             SFRS=2;AINDIDS1|=0x80
#define set_AINDIDS1_P14DIDS             SFRS=2;AINDIDS1|=0x40
#define set_AINDIDS1_P13DIDS             SFRS=2;AINDIDS1|=0x20
#define set_AINDIDS1_P24DIDS             SFRS=2;AINDIDS1|=0x10
#define set_AINDIDS1_P23DIDS             SFRS=2;AINDIDS1|=0x08
#define set_AINDIDS1_P22DIDS             SFRS=2;AINDIDS1|=0x04
#define set_AINDIDS1_P21DIDS             SFRS=2;AINDIDS1|=0x02

#define clr_AINDIDS1_P25DIDS             SFRS=2;AINDIDS1&=0x7F
#define clr_AINDIDS1_P14DIDS             SFRS=2;AINDIDS1&=0xBF
#define clr_AINDIDS1_P13DIDS             SFRS=2;AINDIDS1&=0xDF
#define clr_AINDIDS1_P24DIDS             SFRS=2;AINDIDS1&=0xEF
#define clr_AINDIDS1_P23DIDS             SFRS=2;AINDIDS1&=0xF7
#define clr_AINDIDS1_P22DIDS             SFRS=2;AINDIDS1&=0xFB
#define clr_AINDIDS1_P21DIDS             SFRS=2;AINDIDS1&=0xFD

/**** P0DW  9AH  PAGE 2 ****/
#define set_P0DW_7                       SFRS=2;P0DW|=0x80
#define set_P0DW_6                       SFRS=2;P0DW|=0x40
#define set_P0DW_5                       SFRS=2;P0DW|=0x20
#define set_P0DW_4                       SFRS=2;P0DW|=0x10
#define set_P0DW_3                       SFRS=2;P0DW|=0x08
#define set_P0DW_2                       SFRS=2;P0DW|=0x04
#define set_P0DW_1                       SFRS=2;P0DW|=0x02
#define set_P0DW_0                       SFRS=2;P0DW|=0x01

#define clr_P0DW_7                       SFRS=2;P0DW&=0x7F
#define clr_P0DW_6                       SFRS=2;P0DW&=0xBF
#define clr_P0DW_5                       SFRS=2;P0DW&=0xDF
#define clr_P0DW_4                       SFRS=2;P0DW&=0xEF
#define clr_P0DW_3                       SFRS=2;P0DW&=0xF7
#define clr_P0DW_2                       SFRS=2;P0DW&=0xFB
#define clr_P0DW_1                       SFRS=2;P0DW&=0xFD
#define clr_P0DW_0                       SFRS=2;P0DW&=0xFE

/**** P1DW  9BH  PAGE 2 ****/
#define set_P1DW_7                       SFRS=2;P1DW|=0x80
#define set_P1DW_6                       SFRS=2;P1DW|=0x40
#define set_P1DW_5                       SFRS=2;P1DW|=0x20
#define set_P1DW_4                       SFRS=2;P1DW|=0x10
#define set_P1DW_3                       SFRS=2;P1DW|=0x08
#define set_P1DW_2                       SFRS=2;P1DW|=0x04
#define set_P1DW_1                       SFRS=2;P1DW|=0x02
#define set_P1DW_0                       SFRS=2;P1DW|=0x01

#define clr_P1DW_7                       SFRS=2;P1DW&=0x7F
#define clr_P1DW_6                       SFRS=2;P1DW&=0xBF
#define clr_P1DW_5                       SFRS=2;P1DW&=0xDF
#define clr_P1DW_4                       SFRS=2;P1DW&=0xEF
#define clr_P1DW_3                       SFRS=2;P1DW&=0xF7
#define clr_P1DW_2                       SFRS=2;P1DW&=0xFB
#define clr_P1DW_1                       SFRS=2;P1DW&=0xFD
#define clr_P1DW_0                       SFRS=2;P1DW&=0xFE

/**** P2DW  9CH  PAGE 2 ****/
#define set_P2DW_5                       SFRS=2;P2DW|=0x20
#define set_P2DW_4                       SFRS=2;P2DW|=0x10
#define set_P2DW_3                       SFRS=2;P2DW|=0x08
#define set_P2DW_2                       SFRS=2;P2DW|=0x04
#define set_P2DW_1                       SFRS=2;P2DW|=0x02
#define set_P2DW_0                       SFRS=2;P2DW|=0x01

#define clr_P2DW_5                       SFRS=2;P2DW&=0xDF
#define clr_P2DW_4                       SFRS=2;P2DW&=0xEF
#define clr_P2DW_3                       SFRS=2;P2DW&=0xF7
#define clr_P2DW_2                       SFRS=2;P2DW&=0xFB
#define clr_P2DW_1                       SFRS=2;P2DW&=0xFD
#define clr_P2DW_0                       SFRS=2;P2DW&=0xFE

/**** P3DW  9DH  PAGE 2 ****/
#define set_P3DW_7                       SFRS=2;P3DW|=0x80
#define set_P3DW_6                       SFRS=2;P3DW|=0x40
#define set_P3DW_5                       SFRS=2;P3DW|=0x20
#define set_P3DW_4                       SFRS=2;P3DW|=0x10
#define set_P3DW_3                       SFRS=2;P3DW|=0x08
#define set_P3DW_2                       SFRS=2;P3DW|=0x04
#define set_P3DW_1                       SFRS=2;P3DW|=0x02
#define set_P3DW_0                       SFRS=2;P3DW|=0x01

#define clr_P3DW_7                       SFRS=2;P3DW&=0x7F
#define clr_P3DW_6                       SFRS=2;P3DW&=0xBF
#define clr_P3DW_5                       SFRS=2;P3DW&=0xDF
#define clr_P3DW_4                       SFRS=2;P3DW&=0xEF
#define clr_P3DW_3                       SFRS=2;P3DW&=0xF7
#define clr_P3DW_2                       SFRS=2;P3DW&=0xFB
#define clr_P3DW_1                       SFRS=2;P3DW&=0xFD
#define clr_P3DW_0                       SFRS=2;P3DW&=0xFE

/**** AUXR2  A1H  PAGE 2 ****/
#define set_AUXR2_UART2TXP1              SFRS=2;AUXR2|=0x80
#define set_AUXR2_UART2TXP0              SFRS=2;AUXR2|=0x40
#define set_AUXR2_UART2RXP1              SFRS=2;AUXR2|=0x20
#define set_AUXR2_UART2RXP0              SFRS=2;AUXR2|=0x10
#define set_AUXR2_UART1TXP1              SFRS=2;AUXR2|=0x08
#define set_AUXR2_UART1TXP0              SFRS=2;AUXR2|=0x04
#define set_AUXR2_UART1RXP1              SFRS=2;AUXR2|=0x02
#define set_AUXR2_UART1RXP0              SFRS=2;AUXR2|=0x01

#define clr_AUXR2_UART2TXP1              SFRS=2;AUXR2&=0x7F
#define clr_AUXR2_UART2TXP0              SFRS=2;AUXR2&=0xBF
#define clr_AUXR2_UART2RXP1              SFRS=2;AUXR2&=0xDF
#define clr_AUXR2_UART2RXP0              SFRS=2;AUXR2&=0xEF
#define clr_AUXR2_UART1TXP1              SFRS=2;AUXR2&=0xF7
#define clr_AUXR2_UART1TXP0              SFRS=2;AUXR2&=0xFB
#define clr_AUXR2_UART1RXP1              SFRS=2;AUXR2&=0xFD
#define clr_AUXR2_UART1RXP0              SFRS=2;AUXR2&=0xFE

/**** AUXR3  A2H  PAGE 2 ****/
#define set_AUXR3_UART4TXP1              SFRS=2;AUXR3|=0x80
#define set_AUXR3_UART4TXP0              SFRS=2;AUXR3|=0x40
#define set_AUXR3_UART4RXP1              SFRS=2;AUXR3|=0x20
#define set_AUXR3_UART4RXP0              SFRS=2;AUXR3|=0x10
#define set_AUXR3_UART3TXP1              SFRS=2;AUXR3|=0x08
#define set_AUXR3_UART3TXP0              SFRS=2;AUXR3|=0x04
#define set_AUXR3_UART3RXP1              SFRS=2;AUXR3|=0x02
#define set_AUXR3_UART3RXP0              SFRS=2;AUXR3|=0x01

#define clr_AUXR3_UART4TXP1              SFRS=2;AUXR3&=0x7F
#define clr_AUXR3_UART4TXP0              SFRS=2;AUXR3&=0xBF
#define clr_AUXR3_UART4RXP1              SFRS=2;AUXR3&=0xDF
#define clr_AUXR3_UART4RXP0              SFRS=2;AUXR3&=0xEF
#define clr_AUXR3_UART3TXP1              SFRS=2;AUXR3&=0xF7
#define clr_AUXR3_UART3TXP0              SFRS=2;AUXR3&=0xFB
#define clr_AUXR3_UART3RXP1              SFRS=2;AUXR3&=0xFD
#define clr_AUXR3_UART3RXP0              SFRS=2;AUXR3&=0xFE

/**** AUXR4  A3H  PAGE 2 ****/
#define set_AUXR4_PWM2C1P1               SFRS=2;AUXR4|=0x80
#define set_AUXR4_PWM2C1P0               SFRS=2;AUXR4|=0x40
#define set_AUXR4_PWM2C0P1               SFRS=2;AUXR4|=0x20
#define set_AUXR4_PWM2C0P0               SFRS=2;AUXR4|=0x10
#define set_AUXR4_PWM1C1P1               SFRS=2;AUXR4|=0x08
#define set_AUXR4_PWM1C1P0               SFRS=2;AUXR4|=0x04
#define set_AUXR4_PWM1C0P1               SFRS=2;AUXR4|=0x02
#define set_AUXR4_PWM1C0P0               SFRS=2;AUXR4|=0x01

#define clr_AUXR4_PWM2C1P1               SFRS=2;AUXR4&=0x7F
#define clr_AUXR4_PWM2C1P0               SFRS=2;AUXR4&=0xBF
#define clr_AUXR4_PWM2C0P1               SFRS=2;AUXR4&=0xDF
#define clr_AUXR4_PWM2C0P0               SFRS=2;AUXR4&=0xEF
#define clr_AUXR4_PWM1C1P1               SFRS=2;AUXR4&=0xF7
#define clr_AUXR4_PWM1C1P0               SFRS=2;AUXR4&=0xFB
#define clr_AUXR4_PWM1C0P1               SFRS=2;AUXR4&=0xFD
#define clr_AUXR4_PWM1C0P0               SFRS=2;AUXR4&=0xFE

/**** AUXR5  A4H  PAGE 2 ****/
#define set_AUXR5_CLOP                   SFRS=2;AUXR5|=0x80
#define set_AUXR5_T0P                    SFRS=2;AUXR5|=0x40
#define set_AUXR5_PWM3C1P1               SFRS=2;AUXR5|=0x08
#define set_AUXR5_PWM3C1P0               SFRS=2;AUXR5|=0x04
#define set_AUXR5_PWM3C0P1               SFRS=2;AUXR5|=0x02
#define set_AUXR5_PWM3C0P0               SFRS=2;AUXR5|=0x01

#define clr_AUXR5_CLOP                   SFRS=2;AUXR5&=0x7F
#define clr_AUXR5_T0P                    SFRS=2;AUXR5&=0xBF
#define clr_AUXR5_PWM3C1P1               SFRS=2;AUXR5&=0xF7
#define clr_AUXR5_PWM3C1P0               SFRS=2;AUXR5&=0xFB
#define clr_AUXR5_PWM3C0P1               SFRS=2;AUXR5&=0xFD
#define clr_AUXR5_PWM3C0P0               SFRS=2;AUXR5&=0xFE

/**** AUXR6  A5H  PAGE 2 ****/
#define set_AUXR6_UART4DG                SFRS=2;AUXR6|=0x10
#define set_AUXR6_UART3DG                SFRS=2;AUXR6|=0x08
#define set_AUXR6_UART2DG                SFRS=2;AUXR6|=0x04
#define set_AUXR6_UART1DG                SFRS=2;AUXR6|=0x02
#define set_AUXR6_UART0DG                SFRS=2;AUXR6|=0x01

#define clr_AUXR6_UART4DG                SFRS=2;AUXR6&=0xEF
#define clr_AUXR6_UART3DG                SFRS=2;AUXR6&=0xF7
#define clr_AUXR6_UART2DG                SFRS=2;AUXR6&=0xFB
#define clr_AUXR6_UART1DG                SFRS=2;AUXR6&=0xFD
#define clr_AUXR6_UART0DG                SFRS=2;AUXR6&=0xFE

/**** AUXR7  A6H  PAGE 2 ****/
#define set_AUXR7_SPI0NSSP1              SFRS=2;AUXR7|=0x10
#define set_AUXR7_SPI0NSSP0              SFRS=2;AUXR7|=0x08
#define set_AUXR7_SPI0MOSIP              SFRS=2;AUXR7|=0x04
#define set_AUXR7_SPI0MISOP              SFRS=2;AUXR7|=0x02
#define set_AUXR7_SPI0CKP                SFRS=2;AUXR7|=0x01

#define clr_AUXR7_SPI0NSSP1              SFRS=2;AUXR7&=0xEF
#define clr_AUXR7_SPI0NSSP0              SFRS=2;AUXR7&=0xF7
#define clr_AUXR7_SPI0MOSIP              SFRS=2;AUXR7&=0xFB
#define clr_AUXR7_SPI0MISOP              SFRS=2;AUXR7&=0xFD
#define clr_AUXR7_SPI0CKP                SFRS=2;AUXR7&=0xFE

/**** AUXR8  A7H  PAGE 2 TA protect register ****/
#define set_AUXR8_CLODIV3                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x80;EA=BIT_TMP
#define set_AUXR8_CLODIV2                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x40;EA=BIT_TMP
#define set_AUXR8_CLODIV1                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x20;EA=BIT_TMP
#define set_AUXR8_CLODIV0                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x10;EA=BIT_TMP
#define set_AUXR8_CKTESTOEN3             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x08;EA=BIT_TMP
#define set_AUXR8_CKTESTOEN2             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x04;EA=BIT_TMP
#define set_AUXR8_CKTESTOEN1             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x02;EA=BIT_TMP
#define set_AUXR8_CKTESTOEN0             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8|=0x01;EA=BIT_TMP

#define clr_AUXR8_CLODIV3                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0x7F;EA=BIT_TMP
#define clr_AUXR8_CLODIV2                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xBF;EA=BIT_TMP
#define clr_AUXR8_CLODIV1                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xDF;EA=BIT_TMP
#define clr_AUXR8_CLODIV0                SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xEF;EA=BIT_TMP
#define clr_AUXR8_CKTESTOEN3             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xF7;EA=BIT_TMP
#define clr_AUXR8_CKTESTOEN2             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xFB;EA=BIT_TMP
#define clr_AUXR8_CKTESTOEN1             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xFD;EA=BIT_TMP
#define clr_AUXR8_CKTESTOEN0             SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;AUXR8&=0xFE;EA=BIT_TMP

/**** PWM1MD  ADH  PAGE 2 ****/
#define set_PWM1MD_PMD1                  SFRS=2;PWM1MD|=0x02
#define set_PWM1MD_PMD0                  SFRS=2;PWM1MD|=0x01

#define clr_PWM1MD_PMD1                  SFRS=2;PWM1MD&=0xFD
#define clr_PWM1MD_PMD0                  SFRS=2;PWM1MD&=0xFE

/**** PWM1MEN  AEH  PAGE 2 ****/
#define set_PWM1MEN_PMEN1                SFRS=2;PWM1MEN|=0x02
#define set_PWM1MEN_PMEN0                SFRS=2;PWM1MEN|=0x01

#define clr_PWM1MEN_PMEN1                SFRS=2;PWM1MEN&=0xFD
#define clr_PWM1MEN_PMEN0                SFRS=2;PWM1MEN&=0xFE

/**** PWM1CON0  B4H  PAGE 2 ****/
#define set_PWM1CON0_PWMRUN              SFRS=2;PWM1CON0|=0x80
#define set_PWM1CON0_LOAD                SFRS=2;PWM1CON0|=0x40
#define set_PWM1CON0_PWMF                SFRS=2;PWM1CON0|=0x20
#define set_PWM1CON0_CLRPWM              SFRS=2;PWM1CON0|=0x10

#define clr_PWM1CON0_PWMRUN              SFRS=2;PWM1CON0&=0x7F
#define clr_PWM1CON0_LOAD                SFRS=2;PWM1CON0&=0xBF
#define clr_PWM1CON0_PWMF                SFRS=2;PWM1CON0&=0xDF
#define clr_PWM1CON0_CLRPWM              SFRS=2;PWM1CON0&=0xEF

/**** PWM1CON1  B5H  PAGE 2 ****/
#define set_PWM1CON1_PWMMOD1             SFRS=2;PWM1CON1|=0x80
#define set_PWM1CON1_PWMMOD0             SFRS=2;PWM1CON1|=0x40
#define set_PWM1CON1_PWMTYP              SFRS=2;PWM1CON1|=0x10
#define set_PWM1CON1_PWMDIV2             SFRS=2;PWM1CON1|=0x04
#define set_PWM1CON1_PWMDIV1             SFRS=2;PWM1CON1|=0x02
#define set_PWM1CON1_PWMDIV0             SFRS=2;PWM1CON1|=0x01

#define clr_PWM1CON1_PWMMOD1             SFRS=2;PWM1CON1&=0x7F
#define clr_PWM1CON1_PWMMOD0             SFRS=2;PWM1CON1&=0xBF
#define clr_PWM1CON1_PWMTYP              SFRS=2;PWM1CON1&=0xEF
#define clr_PWM1CON1_PWMDIV2             SFRS=2;PWM1CON1&=0xFB
#define clr_PWM1CON1_PWMDIV1             SFRS=2;PWM1CON1&=0xFD
#define clr_PWM1CON1_PWMDIV0             SFRS=2;PWM1CON1&=0xFE

/**** PWM1INTC  B6H  PAGE 2 ****/
#define set_PWM1INTC_INTTYP1             SFRS=2;PWM1INTC|=0x20
#define set_PWM1INTC_INTTYP0             SFRS=2;PWM1INTC|=0x10
#define set_PWM1INTC_INTSEL2             SFRS=2;PWM1INTC|=0x04
#define set_PWM1INTC_INTSEL1             SFRS=2;PWM1INTC|=0x02
#define set_PWM1INTC_INTSEL0             SFRS=2;PWM1INTC|=0x01

#define clr_PWM1INTC_INTTYP1             SFRS=2;PWM1INTC&=0xDF
#define clr_PWM1INTC_INTTYP0             SFRS=2;PWM1INTC&=0xEF
#define clr_PWM1INTC_INTSEL2             SFRS=2;PWM1INTC&=0xFB
#define clr_PWM1INTC_INTSEL1             SFRS=2;PWM1INTC&=0xFD
#define clr_PWM1INTC_INTSEL0             SFRS=2;PWM1INTC&=0xFE

/**** PIOCON2  B7H  PAGE 2 ****/
#define set_PIOCON2_PIO34                SFRS=2;PIOCON2|=0x80
#define set_PIOCON2_PIO33                SFRS=2;PIOCON2|=0x40
#define set_PIOCON2_PIO32                SFRS=2;PIOCON2|=0x20
#define set_PIOCON2_PIO31                SFRS=2;PIOCON2|=0x10
#define set_PIOCON2_PIO30                SFRS=2;PIOCON2|=0x08
#define set_PIOCON2_PIO23                SFRS=2;PIOCON2|=0x04
#define set_PIOCON2_PIO22                SFRS=2;PIOCON2|=0x02
#define set_PIOCON2_PIO21                SFRS=2;PIOCON2|=0x01

#define clr_PIOCON2_PIO34                SFRS=2;PIOCON2&=0x7F
#define clr_PIOCON2_PIO33                SFRS=2;PIOCON2&=0xBF
#define clr_PIOCON2_PIO32                SFRS=2;PIOCON2&=0xDF
#define clr_PIOCON2_PIO31                SFRS=2;PIOCON2&=0xEF
#define clr_PIOCON2_PIO30                SFRS=2;PIOCON2&=0xF7
#define clr_PIOCON2_PIO23                SFRS=2;PIOCON2&=0xFB
#define clr_PIOCON2_PIO22                SFRS=2;PIOCON2&=0xFD
#define clr_PIOCON2_PIO21                SFRS=2;PIOCON2&=0xFE

/**** PWM2MD  BEH  PAGE 2 ****/
#define set_PWM2MD_PMD1                  SFRS=2;PWM2MD|=0x02
#define set_PWM2MD_PMD0                  SFRS=2;PWM2MD|=0x01

#define clr_PWM2MD_PMD1                  SFRS=2;PWM2MD&=0xFD
#define clr_PWM2MD_PMD0                  SFRS=2;PWM2MD&=0xFE

/**** PWM2MEN  BFH  PAGE 2 ****/
#define set_PWM2MEN_PMEN1                SFRS=2;PWM2MEN|=0x02
#define set_PWM2MEN_PMEN0                SFRS=2;PWM2MEN|=0x01

#define clr_PWM2MEN_PMEN1                SFRS=2;PWM2MEN&=0xFD
#define clr_PWM2MEN_PMEN0                SFRS=2;PWM2MEN&=0xFE

/**** PWM2CON0  C4H  PAGE 2 ****/
#define set_PWM2CON0_PWMRUN              SFRS=2;PWM2CON0|=0x80
#define set_PWM2CON0_LOAD                SFRS=2;PWM2CON0|=0x40
#define set_PWM2CON0_PWMF                SFRS=2;PWM2CON0|=0x20
#define set_PWM2CON0_CLRPWM              SFRS=2;PWM2CON0|=0x10

#define clr_PWM2CON0_PWMRUN              SFRS=2;PWM2CON0&=0x7F
#define clr_PWM2CON0_LOAD                SFRS=2;PWM2CON0&=0xBF
#define clr_PWM2CON0_PWMF                SFRS=2;PWM2CON0&=0xDF
#define clr_PWM2CON0_CLRPWM              SFRS=2;PWM2CON0&=0xEF

/**** PWM2CON1  C5H  PAGE 2 ****/
#define set_PWM2CON1_PWMMOD1             SFRS=2;PWM2CON1|=0x80
#define set_PWM2CON1_PWMMOD0             SFRS=2;PWM2CON1|=0x40
#define set_PWM2CON1_PWMTYP              SFRS=2;PWM2CON1|=0x20
#define set_PWM2CON1_PWMDIV2             SFRS=2;PWM2CON1|=0x04
#define set_PWM2CON1_PWMDIV1             SFRS=2;PWM2CON1|=0x02
#define set_PWM2CON1_PWMDIV0             SFRS=2;PWM2CON1|=0x01

#define clr_PWM2CON1_PWMMOD1             SFRS=2;PWM2CON1&=0x7F
#define clr_PWM2CON1_PWMMOD0             SFRS=2;PWM2CON1&=0xBF
#define clr_PWM2CON1_PWMTYP              SFRS=2;PWM2CON1&=0xDF
#define clr_PWM2CON1_PWMDIV2             SFRS=2;PWM2CON1&=0xFB
#define clr_PWM2CON1_PWMDIV1             SFRS=2;PWM2CON1&=0xFD
#define clr_PWM2CON1_PWMDIV0             SFRS=2;PWM2CON1&=0xFE

/**** PWM2INTC  C6H  PAGE 2 ****/
#define set_PWM2INTC_INTTYP1             SFRS=2;PWM2INTC|=0x20
#define set_PWM2INTC_INTTYP0             SFRS=2;PWM2INTC|=0x10
#define set_PWM2INTC_INTSEL2             SFRS=2;PWM2INTC|=0x04
#define set_PWM2INTC_INTSEL1             SFRS=2;PWM2INTC|=0x02
#define set_PWM2INTC_INTSEL0             SFRS=2;PWM2INTC|=0x01

#define clr_PWM2INTC_INTTYP1             SFRS=2;PWM2INTC&=0xDF
#define clr_PWM2INTC_INTTYP0             SFRS=2;PWM2INTC&=0xEF
#define clr_PWM2INTC_INTSEL2             SFRS=2;PWM2INTC&=0xFB
#define clr_PWM2INTC_INTSEL1             SFRS=2;PWM2INTC&=0xFD
#define clr_PWM2INTC_INTSEL0             SFRS=2;PWM2INTC&=0xFE

/**** PWM3MD  CCH  PAGE 2 ****/
#define set_PWM3MD_PMD1                  SFRS=2;PWM3MD|=0x02
#define set_PWM3MD_PMD0                  SFRS=2;PWM3MD|=0x01

#define clr_PWM3MD_PMD1                  SFRS=2;PWM3MD&=0xFD
#define clr_PWM3MD_PMD0                  SFRS=2;PWM3MD&=0xFE

/**** PWM3MEN  CDH  PAGE 2 ****/
#define set_PWM3MEN_PMEN1                SFRS=2;PWM3MEN|=0x02
#define set_PWM3MEN_PMEN0                SFRS=2;PWM3MEN|=0x01

#define clr_PWM3MEN_PMEN1                SFRS=2;PWM3MEN&=0xFD
#define clr_PWM3MEN_PMEN0                SFRS=2;PWM3MEN&=0xFE

/**** EIP2  CEH  PAGE 2 ****/
#define set_EIP2_PSC2                    SFRS=2;EIP2|=0x04
#define set_EIP2_PSC1                    SFRS=2;EIP2|=0x02
#define set_EIP2_PSC0                    SFRS=2;EIP2|=0x01

#define clr_EIP2_PSC2                    SFRS=2;EIP2&=0xFB
#define clr_EIP2_PSC1                    SFRS=2;EIP2&=0xFD
#define clr_EIP2_PSC0                    SFRS=2;EIP2&=0xFE

/**** EIPH2  CFH  PAGE 2 ****/
#define set_EIPH2_PSC2H                    SFRS=2;EIPH2|=0x04
#define set_EIPH2_PSC1H                    SFRS=2;EIPH2|=0x02
#define set_EIPH2_PSC0H                    SFRS=2;EIPH2|=0x01

#define clr_EIPH2_PSC2H                    SFRS=2;EIPH2&=0xFB
#define clr_EIPH2_PSC1H                    SFRS=2;EIPH2&=0xFD
#define clr_EIPH2_PSC0H                    SFRS=2;EIPH2&=0xFE

/**** PWM3CON0  D4H  PAGE 2 ****/
#define set_PWM3CON0_PWMRUN              SFRS=2;PWM3CON0|=0x80
#define set_PWM3CON0_LOAD                SFRS=2;PWM3CON0|=0x40
#define set_PWM3CON0_PWMF                SFRS=2;PWM3CON0|=0x20
#define set_PWM3CON0_CLRPWM              SFRS=2;PWM3CON0|=0x10

#define clr_PWM3CON0_PWMRUN              SFRS=2;PWM3CON0&=0x7F
#define clr_PWM3CON0_LOAD                SFRS=2;PWM3CON0&=0xBF
#define clr_PWM3CON0_PWMF                SFRS=2;PWM3CON0&=0xDF
#define clr_PWM3CON0_CLRPWM              SFRS=2;PWM3CON0&=0xEF

/**** PWM3CON1  D5H  PAGE 2 ****/
#define set_PWM3CON1_PWMMOD1             SFRS=2;PWM3CON1|=0x80
#define set_PWM3CON1_PWMMOD0             SFRS=2;PWM3CON1|=0x40
#define set_PWM3CON1_PWMTYP              SFRS=2;PWM3CON1|=0x20
#define set_PWM3CON1_PWMDIV2             SFRS=2;PWM3CON1|=0x04
#define set_PWM3CON1_PWMDIV1             SFRS=2;PWM3CON1|=0x02
#define set_PWM3CON1_PWMDIV0             SFRS=2;PWM3CON1|=0x01

#define clr_PWM3CON1_PWMMOD1             SFRS=2;PWM3CON1&=0x7F
#define clr_PWM3CON1_PWMMOD0             SFRS=2;PWM3CON1&=0xBF
#define clr_PWM3CON1_PWMTYP              SFRS=2;PWM3CON1&=0xDF
#define clr_PWM3CON1_PWMDIV2             SFRS=2;PWM3CON1&=0xFB
#define clr_PWM3CON1_PWMDIV1             SFRS=2;PWM3CON1&=0xFD
#define clr_PWM3CON1_PWMDIV0             SFRS=2;PWM3CON1&=0xFE

/**** PWM3INTC  D6H  PAGE 2 ****/
#define set_PWM3INTC_INTTYP1             SFRS=2;PWM3INTC|=0x20
#define set_PWM3INTC_INTTYP0             SFRS=2;PWM3INTC|=0x10
#define set_PWM3INTC_INTSEL2             SFRS=2;PWM3INTC|=0x04
#define set_PWM3INTC_INTSEL1             SFRS=2;PWM3INTC|=0x02
#define set_PWM3INTC_INTSEL0             SFRS=2;PWM3INTC|=0x01

#define clr_PWM3INTC_INTTYP1             SFRS=2;PWM3INTC&=0xDF
#define clr_PWM3INTC_INTTYP0             SFRS=2;PWM3INTC&=0xEF
#define clr_PWM3INTC_INTSEL2             SFRS=2;PWM3INTC&=0xFB
#define clr_PWM3INTC_INTSEL1             SFRS=2;PWM3INTC&=0xFD
#define clr_PWM3INTC_INTSEL0             SFRS=2;PWM3INTC&=0xFE

/**** XTLCON  D7H  PAGE 2 TA protect register ****/
#define set_XTLCON_HXSG2                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON|=0x40;EA=BIT_TMP
#define set_XTLCON_HXSG1                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON|=0x20;EA=BIT_TMP
#define set_XTLCON_HXSG0                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON|=0x10;EA=BIT_TMP

#define clr_XTLCON_HXSG2                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON&=0xBF;EA=BIT_TMP
#define clr_XTLCON_HXSG1                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON&=0xDF;EA=BIT_TMP
#define clr_XTLCON_HXSG0                 SFRS=2;BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;XTLCON&=0xEF;EA=BIT_TMP

/**** SC0ETURD1  DCH  PAGE 2 ****/
#define set_SC0ETURD1_SCDIV2             SFRS=2;SC0ETURD1|=0x40
#define set_SC0ETURD1_SCDIV1             SFRS=2;SC0ETURD1|=0x20
#define set_SC0ETURD1_SCDIV0             SFRS=2;SC0ETURD1|=0x10
#define set_SC0ETURD1_ETURDIV11          SFRS=2;SC0ETURD1|=0x08
#define set_SC0ETURD1_ETURDIV10          SFRS=2;SC0ETURD1|=0x04
#define set_SC0ETURD1_ETURDIV9           SFRS=2;SC0ETURD1|=0x02
#define set_SC0ETURD1_ETURDIV8           SFRS=2;SC0ETURD1|=0x01

#define clr_SC0ETURD1_SCDIV2             SFRS=2;SC0ETURD1&=0xBF
#define clr_SC0ETURD1_SCDIV1             SFRS=2;SC0ETURD1&=0xDF
#define clr_SC0ETURD1_SCDIV0             SFRS=2;SC0ETURD1&=0xEF
#define clr_SC0ETURD1_ETURDIV11          SFRS=2;SC0ETURD1&=0xF7
#define clr_SC0ETURD1_ETURDIV10          SFRS=2;SC0ETURD1&=0xFB
#define clr_SC0ETURD1_ETURDIV9           SFRS=2;SC0ETURD1&=0xFD
#define clr_SC0ETURD1_ETURDIV8           SFRS=2;SC0ETURD1&=0xFE

/**** SC0IE  DDH  PAGE 2 ****/
#define set_SC0IE_ACERRIEN               SFRS=2;SC0IE|=0x10
#define set_SC0IE_BGTIEN                 SFRS=2;SC0IE|=0x08
#define set_SC0IE_TERRIEN                SFRS=2;SC0IE|=0x04
#define set_SC0IE_TBEIEN                 SFRS=2;SC0IE|=0x02
#define set_SC0IE_RDAIEN                 SFRS=2;SC0IE|=0x01

#define clr_SC0IE_ACERRIEN               SFRS=2;SC0IE&=0xEF
#define clr_SC0IE_BGTIEN                 SFRS=2;SC0IE&=0xF7
#define clr_SC0IE_TERRIEN                SFRS=2;SC0IE&=0xFB
#define clr_SC0IE_TBEIEN                 SFRS=2;SC0IE&=0xFD
#define clr_SC0IE_RDAIEN                 SFRS=2;SC0IE&=0xFE

/**** SC0IS  DEH  PAGE 2 ****/
#define set_SC0IS_ACERRIF                SFRS=2;SC0IS|=0x10
#define set_SC0IS_BGTIF                  SFRS=2;SC0IS|=0x08
#define set_SC0IS_TERRIF                 SFRS=2;SC0IS|=0x04
#define set_SC0IS_TBEIF                  SFRS=2;SC0IS|=0x02
#define set_SC0IS_RDAIF                  SFRS=2;SC0IS|=0x01

#define clr_SC0IS_ACERRIF                SFRS=2;SC0IS&=0xEF
#define clr_SC0IS_BGTIF                  SFRS=2;SC0IS&=0xF7
#define clr_SC0IS_TERRIF                 SFRS=2;SC0IS&=0xFB
#define clr_SC0IS_TBEIF                  SFRS=2;SC0IS&=0xFD
#define clr_SC0IS_RDAIF                  SFRS=2;SC0IS&=0xFE

/**** SC0TSR  DFH  PAGE 2 ****/
#define set_SC0TSR_ACT                   SFRS=2;SC0TSR|=0x80
#define set_SC0TSR_BEF                   SFRS=2;SC0TSR|=0x40
#define set_SC0TSR_FEF                   SFRS=2;SC0TSR|=0x20
#define set_SC0TSR_PEF                   SFRS=2;SC0TSR|=0x10
#define set_SC0TSR_TXEMPTY               SFRS=2;SC0TSR|=0x08
#define set_SC0TSR_TXOV                  SFRS=2;SC0TSR|=0x04
#define set_SC0TSR_RXEMPTY               SFRS=2;SC0TSR|=0x02
#define set_SC0TSR_RXOV                  SFRS=2;SC0TSR|=0x01

#define clr_SC0TSR_ACT                   SFRS=2;SC0TSR&=0x7F
#define clr_SC0TSR_BEF                   SFRS=2;SC0TSR&=0xBF
#define clr_SC0TSR_FEF                   SFRS=2;SC0TSR&=0xDF
#define clr_SC0TSR_PEF                   SFRS=2;SC0TSR&=0xEF
#define clr_SC0TSR_TXEMPTY               SFRS=2;SC0TSR&=0xF7
#define clr_SC0TSR_TXOV                  SFRS=2;SC0TSR&=0xFB
#define clr_SC0TSR_RXEMPTY               SFRS=2;SC0TSR&=0xFD
#define clr_SC0TSR_RXOV                  SFRS=2;SC0TSR&=0xFE

/**** SC1ETURD1  E4H  PAGE 2 ****/
#define set_SC1ETURD1_SCDIV2             SFRS=2;SC1ETURD1|=0x40
#define set_SC1ETURD1_SCDIV1             SFRS=2;SC1ETURD1|=0x20
#define set_SC1ETURD1_SCDIV0             SFRS=2;SC1ETURD1|=0x10
#define set_SC1ETURD1_ETURDIV11          SFRS=2;SC1ETURD1|=0x08
#define set_SC1ETURD1_ETURDIV10          SFRS=2;SC1ETURD1|=0x04
#define set_SC1ETURD1_ETURDIV9           SFRS=2;SC1ETURD1|=0x02
#define set_SC1ETURD1_ETURDIV8           SFRS=2;SC1ETURD1|=0x01

#define clr_SC1ETURD1_SCDIV2             SFRS=2;SC1ETURD1&=0xBF
#define clr_SC1ETURD1_SCDIV1             SFRS=2;SC1ETURD1&=0xDF
#define clr_SC1ETURD1_SCDIV0             SFRS=2;SC1ETURD1&=0xEF
#define clr_SC1ETURD1_ETURDIV11          SFRS=2;SC1ETURD1&=0xF7
#define clr_SC1ETURD1_ETURDIV10          SFRS=2;SC1ETURD1&=0xFB
#define clr_SC1ETURD1_ETURDIV9           SFRS=2;SC1ETURD1&=0xFD
#define clr_SC1ETURD1_ETURDIV8           SFRS=2;SC1ETURD1&=0xFE

/**** SC1IE  E5H  PAGE 2 ****/
#define set_SC1IE_ACERRIEN               SFRS=2;SC1IE|=0x10
#define set_SC1IE_BGTIEN                 SFRS=2;SC1IE|=0x08
#define set_SC1IE_TERRIEN                SFRS=2;SC1IE|=0x04
#define set_SC1IE_TBEIEN                 SFRS=2;SC1IE|=0x02
#define set_SC1IE_RDAIEN                 SFRS=2;SC1IE|=0x01

#define clr_SC1IE_ACERRIEN               SFRS=2;SC1IE&=0xEF
#define clr_SC1IE_BGTIEN                 SFRS=2;SC1IE&=0xF7
#define clr_SC1IE_TERRIEN                SFRS=2;SC1IE&=0xFB
#define clr_SC1IE_TBEIEN                 SFRS=2;SC1IE&=0xFD
#define clr_SC1IE_RDAIEN                 SFRS=2;SC1IE&=0xFE

/**** SC1IS  E6H  PAGE 2 ****/
#define set_SC1IS_ACERRIF                SFRS=2;SC1IS|=0x10
#define set_SC1IS_BGTIF                  SFRS=2;SC1IS|=0x08
#define set_SC1IS_TERRIF                 SFRS=2;SC1IS|=0x04
#define set_SC1IS_TBEIF                  SFRS=2;SC1IS|=0x02
#define set_SC1IS_RDAIF                  SFRS=2;SC1IS|=0x01

#define clr_SC1IS_ACERRIF                SFRS=2;SC1IS&=0xEF
#define clr_SC1IS_BGTIF                  SFRS=2;SC1IS&=0xF7
#define clr_SC1IS_TERRIF                 SFRS=2;SC1IS&=0xFB
#define clr_SC1IS_TBEIF                  SFRS=2;SC1IS&=0xFD
#define clr_SC1IS_RDAIF                  SFRS=2;SC1IS&=0xFE

/**** SC1TSR  E7H  PAGE 2 ****/
#define set_SC1TSR_ACT                   SFRS=2;SC1TSR|=0x80
#define set_SC1TSR_BEF                   SFRS=2;SC1TSR|=0x40
#define set_SC1TSR_FEF                   SFRS=2;SC1TSR|=0x20
#define set_SC1TSR_PEF                   SFRS=2;SC1TSR|=0x10
#define set_SC1TSR_TXEMPTY               SFRS=2;SC1TSR|=0x08
#define set_SC1TSR_TXOV                  SFRS=2;SC1TSR|=0x04
#define set_SC1TSR_RXEMPTY               SFRS=2;SC1TSR|=0x02
#define set_SC1TSR_RXOV                  SFRS=2;SC1TSR|=0x01

#define clr_SC1TSR_ACT                   SFRS=2;SC1TSR&=0x7F
#define clr_SC1TSR_BEF                   SFRS=2;SC1TSR&=0xBF
#define clr_SC1TSR_FEF                   SFRS=2;SC1TSR&=0xDF
#define clr_SC1TSR_PEF                   SFRS=2;SC1TSR&=0xEF
#define clr_SC1TSR_TXEMPTY               SFRS=2;SC1TSR&=0xF7
#define clr_SC1TSR_TXOV                  SFRS=2;SC1TSR&=0xFB
#define clr_SC1TSR_RXEMPTY               SFRS=2;SC1TSR&=0xFD
#define clr_SC1TSR_RXOV                  SFRS=2;SC1TSR&=0xFE

/**** SC2ETURD1  ECH  PAGE 2 ****/
#define set_SC2ETURD1_SCDIV2             SFRS=2;SC2ETURD1|=0x40
#define set_SC2ETURD1_SCDIV1             SFRS=2;SC2ETURD1|=0x20
#define set_SC2ETURD1_SCDIV0             SFRS=2;SC2ETURD1|=0x10
#define set_SC2ETURD1_ETURDIV11          SFRS=2;SC2ETURD1|=0x08
#define set_SC2ETURD1_ETURDIV10          SFRS=2;SC2ETURD1|=0x04
#define set_SC2ETURD1_ETURDIV9           SFRS=2;SC2ETURD1|=0x02
#define set_SC2ETURD1_ETURDIV8           SFRS=2;SC2ETURD1|=0x01

#define clr_SC2ETURD1_SCDIV2             SFRS=2;SC2ETURD1&=0xBF
#define clr_SC2ETURD1_SCDIV1             SFRS=2;SC2ETURD1&=0xDF
#define clr_SC2ETURD1_SCDIV0             SFRS=2;SC2ETURD1&=0xEF
#define clr_SC2ETURD1_ETURDIV11          SFRS=2;SC2ETURD1&=0xF7
#define clr_SC2ETURD1_ETURDIV10          SFRS=2;SC2ETURD1&=0xFB
#define clr_SC2ETURD1_ETURDIV9           SFRS=2;SC2ETURD1&=0xFD
#define clr_SC2ETURD1_ETURDIV8           SFRS=2;SC2ETURD1&=0xFE

/**** SC2IE  EDH  PAGE 2 ****/
#define set_SC2IE_ACERRIEN               SFRS=2;SC2IE|=0x10
#define set_SC2IE_BGTIEN                 SFRS=2;SC2IE|=0x08
#define set_SC2IE_TERRIEN                SFRS=2;SC2IE|=0x04
#define set_SC2IE_TBEIEN                 SFRS=2;SC2IE|=0x02
#define set_SC2IE_RDAIEN                 SFRS=2;SC2IE|=0x01

#define clr_SC2IE_ACERRIEN               SFRS=2;SC2IE&=0xEF
#define clr_SC2IE_BGTIEN                 SFRS=2;SC2IE&=0xF7
#define clr_SC2IE_TERRIEN                SFRS=2;SC2IE&=0xFB
#define clr_SC2IE_TBEIEN                 SFRS=2;SC2IE&=0xFD
#define clr_SC2IE_RDAIEN                 SFRS=2;SC2IE&=0xFE

/**** SC2IS  EEH  PAGE 2 ****/
#define set_SC2IS_ACERRIF                SFRS=2;SC2IS|=0x10
#define set_SC2IS_BGTIF                  SFRS=2;SC2IS|=0x08
#define set_SC2IS_TERRIF                 SFRS=2;SC2IS|=0x04
#define set_SC2IS_TBEIF                  SFRS=2;SC2IS|=0x02
#define set_SC2IS_RDAIF                  SFRS=2;SC2IS|=0x01

#define clr_SC2IS_ACERRIF                SFRS=2;SC2IS&=0xEF
#define clr_SC2IS_BGTIF                  SFRS=2;SC2IS&=0xF7
#define clr_SC2IS_TERRIF                 SFRS=2;SC2IS&=0xFB
#define clr_SC2IS_TBEIF                  SFRS=2;SC2IS&=0xFD
#define clr_SC2IS_RDAIF                  SFRS=2;SC2IS&=0xFE

/**** SC2TSR  EFH  PAGE 2 ****/
#define set_SC2TSR_ACT                   SFRS=2;SC2TSR|=0x80
#define set_SC2TSR_BEF                   SFRS=2;SC2TSR|=0x40
#define set_SC2TSR_FEF                   SFRS=2;SC2TSR|=0x20
#define set_SC2TSR_PEF                   SFRS=2;SC2TSR|=0x10
#define set_SC2TSR_TXEMPTY               SFRS=2;SC2TSR|=0x08
#define set_SC2TSR_TXOV                  SFRS=2;SC2TSR|=0x04
#define set_SC2TSR_RXEMPTY               SFRS=2;SC2TSR|=0x02
#define set_SC2TSR_RXOV                  SFRS=2;SC2TSR|=0x01

#define clr_SC2TSR_ACT                   SFRS=2;SC2TSR&=0x7F
#define clr_SC2TSR_BEF                   SFRS=2;SC2TSR&=0xBF
#define clr_SC2TSR_FEF                   SFRS=2;SC2TSR&=0xDF
#define clr_SC2TSR_PEF                   SFRS=2;SC2TSR&=0xEF
#define clr_SC2TSR_TXEMPTY               SFRS=2;SC2TSR&=0xF7
#define clr_SC2TSR_TXOV                  SFRS=2;SC2TSR&=0xFB
#define clr_SC2TSR_RXEMPTY               SFRS=2;SC2TSR&=0xFD
#define clr_SC2TSR_RXOV                  SFRS=2;SC2TSR&=0xFE

/**** SC0CR0  F1H  PAGE 2 ****/
#define set_SC0CR0_NSB                   SFRS=2;SC0CR0|=0x80
#define set_SC0CR0_T                     SFRS=2;SC0CR0|=0x40
#define set_SC0CR0_RXBGTEN               SFRS=2;SC0CR0|=0x20
#define set_SC0CR0_CONSEL                SFRS=2;SC0CR0|=0x10
#define set_SC0CR0_AUTOCEN               SFRS=2;SC0CR0|=0x08
#define set_SC0CR0_TXOFF                 SFRS=2;SC0CR0|=0x04
#define set_SC0CR0_RXOFF                 SFRS=2;SC0CR0|=0x02
#define set_SC0CR0_SCEN                  SFRS=2;SC0CR0|=0x01

#define clr_SC0CR0_NSB                   SFRS=2;SC0CR0&=0x7F
#define clr_SC0CR0_T                     SFRS=2;SC0CR0&=0xBF
#define clr_SC0CR0_RXBGTEN               SFRS=2;SC0CR0&=0xDF
#define clr_SC0CR0_CONSEL                SFRS=2;SC0CR0&=0xEF
#define clr_SC0CR0_AUTOCEN               SFRS=2;SC0CR0&=0xF7
#define clr_SC0CR0_TXOFF                 SFRS=2;SC0CR0&=0xFB
#define clr_SC0CR0_RXOFF                 SFRS=2;SC0CR0&=0xFD
#define clr_SC0CR0_SCEN                  SFRS=2;SC0CR0&=0xFE

/**** SC0CR1  F2H  PAGE 2 ****/
#define set_SC0CR1_OPE                   SFRS=2;SC0CR1|=0x80
#define set_SC0CR1_PBOFF                 SFRS=2;SC0CR1|=0x40
#define set_SC0CR1_WLS1                  SFRS=2;SC0CR1|=0x20
#define set_SC0CR1_WLS0                  SFRS=2;SC0CR1|=0x10
#define set_SC0CR1_TXDMAEN               SFRS=2;SC0CR1|=0x08
#define set_SC0CR1_RXDMAEN               SFRS=2;SC0CR1|=0x04
#define set_SC0CR1_CLKKEEP               SFRS=2;SC0CR1|=0x02
#define set_SC0CR1_UARTEN                SFRS=2;SC0CR1|=0x01

#define clr_SC0CR1_OPE                   SFRS=2;SC0CR1&=0x7F
#define clr_SC0CR1_PBOFF                 SFRS=2;SC0CR1&=0xBF
#define clr_SC0CR1_WLS1                  SFRS=2;SC0CR1&=0xDF
#define clr_SC0CR1_WLS0                  SFRS=2;SC0CR1&=0xEF
#define clr_SC0CR1_TXDMAEN               SFRS=2;SC0CR1&=0xF7
#define clr_SC0CR1_RXDMAEN               SFRS=2;SC0CR1&=0xFB
#define clr_SC0CR1_CLKKEEP               SFRS=2;SC0CR1&=0xFD
#define clr_SC0CR1_UARTEN                SFRS=2;SC0CR1&=0xFE

/**** SC1CR0  F3H  PAGE 2 ****/
#define set_SC1CR0_NSB                   SFRS=2;SC1CR0|=0x80
#define set_SC1CR0_T                     SFRS=2;SC1CR0|=0x40
#define set_SC1CR0_RXBGTEN               SFRS=2;SC1CR0|=0x20
#define set_SC1CR0_CONSEL                SFRS=2;SC1CR0|=0x10
#define set_SC1CR0_AUTOCEN               SFRS=2;SC1CR0|=0x08
#define set_SC1CR0_TXOFF                 SFRS=2;SC1CR0|=0x04
#define set_SC1CR0_RXOFF                 SFRS=2;SC1CR0|=0x02
#define set_SC1CR0_SCEN                  SFRS=2;SC1CR0|=0x01

#define clr_SC1CR0_NSB                   SFRS=2;SC1CR0&=0x7F
#define clr_SC1CR0_T                     SFRS=2;SC1CR0&=0xBF
#define clr_SC1CR0_RXBGTEN               SFRS=2;SC1CR0&=0xDF
#define clr_SC1CR0_CONSEL                SFRS=2;SC1CR0&=0xEF
#define clr_SC1CR0_AUTOCEN               SFRS=2;SC1CR0&=0xF7
#define clr_SC1CR0_TXOFF                 SFRS=2;SC1CR0&=0xFB
#define clr_SC1CR0_RXOFF                 SFRS=2;SC1CR0&=0xFD
#define clr_SC1CR0_SCEN                  SFRS=2;SC1CR0&=0xFE

/**** SC1CR1  F4H  PAGE 2 ****/
#define set_SC1CR1_OPE                   SFRS=2;SC1CR1|=0x80
#define set_SC1CR1_PBOFF                 SFRS=2;SC1CR1|=0x40
#define set_SC1CR1_WLS1                  SFRS=2;SC1CR1|=0x20
#define set_SC1CR1_WLS0                  SFRS=2;SC1CR1|=0x10
#define set_SC1CR1_TXDMAEN               SFRS=2;SC1CR1|=0x08
#define set_SC1CR1_RXDMAEN               SFRS=2;SC1CR1|=0x04
#define set_SC1CR1_CLKKEEP               SFRS=2;SC1CR1|=0x02
#define set_SC1CR1_UARTEN                SFRS=2;SC1CR1|=0x01

#define clr_SC1CR1_OPE                   SFRS=2;SC1CR1&=0x7F
#define clr_SC1CR1_PBOFF                 SFRS=2;SC1CR1&=0xBF
#define clr_SC1CR1_WLS1                  SFRS=2;SC1CR1&=0xDF
#define clr_SC1CR1_WLS0                  SFRS=2;SC1CR1&=0xEF
#define clr_SC1CR1_TXDMAEN               SFRS=2;SC1CR1&=0xF7
#define clr_SC1CR1_RXDMAEN               SFRS=2;SC1CR1&=0xFB
#define clr_SC1CR1_CLKKEEP               SFRS=2;SC1CR1&=0xFD
#define clr_SC1CR1_UARTEN                SFRS=2;SC1CR1&=0xFE

/**** SC2CR0  F5H  PAGE 2 ****/
#define set_SC2CR0_NSB                   SFRS=2;SC2CR0|=0x80
#define set_SC2CR0_T                     SFRS=2;SC2CR0|=0x40
#define set_SC2CR0_RXBGTEN               SFRS=2;SC2CR0|=0x20
#define set_SC2CR0_CONSEL                SFRS=2;SC2CR0|=0x10
#define set_SC2CR0_AUTOCEN               SFRS=2;SC2CR0|=0x08
#define set_SC2CR0_TXOFF                 SFRS=2;SC2CR0|=0x04
#define set_SC2CR0_RXOFF                 SFRS=2;SC2CR0|=0x02
#define set_SC2CR0_SCEN                  SFRS=2;SC2CR0|=0x01

#define clr_SC2CR0_NSB                   SFRS=2;SC2CR0&=0x7F
#define clr_SC2CR0_T                     SFRS=2;SC2CR0&=0xBF
#define clr_SC2CR0_RXBGTEN               SFRS=2;SC2CR0&=0xDF
#define clr_SC2CR0_CONSEL                SFRS=2;SC2CR0&=0xEF
#define clr_SC2CR0_AUTOCEN               SFRS=2;SC2CR0&=0xF7
#define clr_SC2CR0_TXOFF                 SFRS=2;SC2CR0&=0xFB
#define clr_SC2CR0_RXOFF                 SFRS=2;SC2CR0&=0xFD
#define clr_SC2CR0_SCEN                  SFRS=2;SC2CR0&=0xFE

/**** SC2CR1  F6H  PAGE 2 ****/
#define set_SC2CR1_OPE                   SFRS=2;SC2CR1|=0x80
#define set_SC2CR1_PBOFF                 SFRS=2;SC2CR1|=0x40
#define set_SC2CR1_WLS1                  SFRS=2;SC2CR1|=0x20
#define set_SC2CR1_WLS0                  SFRS=2;SC2CR1|=0x10
#define set_SC2CR1_TXDMAEN               SFRS=2;SC2CR1|=0x08
#define set_SC2CR1_RXDMAEN               SFRS=2;SC2CR1|=0x04
#define set_SC2CR1_CLKKEEP               SFRS=2;SC2CR1|=0x02
#define set_SC2CR1_UARTEN                SFRS=2;SC2CR1|=0x01

#define clr_SC2CR1_OPE                   SFRS=2;SC2CR1&=0x7F
#define clr_SC2CR1_PBOFF                 SFRS=2;SC2CR1&=0xBF
#define clr_SC2CR1_WLS1                  SFRS=2;SC2CR1&=0xDF
#define clr_SC2CR1_WLS0                  SFRS=2;SC2CR1&=0xEF
#define clr_SC2CR1_TXDMAEN               SFRS=2;SC2CR1&=0xF7
#define clr_SC2CR1_RXDMAEN               SFRS=2;SC2CR1&=0xFB
#define clr_SC2CR1_CLKKEEP               SFRS=2;SC2CR1&=0xFD
#define clr_SC2CR1_UARTEN                SFRS=2;SC2CR1&=0xFE

/**** PIPS7  F7H  PAGE 2 ****/
#define set_PIPS7_PSEL2                  SFRS=2;PIPS7|=0x40
#define set_PIPS7_PSEL1                  SFRS=2;PIPS7|=0x20
#define set_PIPS7_PSEL0                  SFRS=2;PIPS7|=0x10
#define set_PIPS7_BSEL2                  SFRS=2;PIPS7|=0x04
#define set_PIPS7_BSEL1                  SFRS=2;PIPS7|=0x02
#define set_PIPS7_BSEL0                  SFRS=2;PIPS7|=0x01

#define clr_PIPS7_PSEL2                  SFRS=2;PIPS7&=0xBF
#define clr_PIPS7_PSEL1                  SFRS=2;PIPS7&=0xDF
#define clr_PIPS7_PSEL0                  SFRS=2;PIPS7&=0xEF
#define clr_PIPS7_BSEL2                  SFRS=2;PIPS7&=0xFB
#define clr_PIPS7_BSEL1                  SFRS=2;PIPS7&=0xFD
#define clr_PIPS7_BSEL0                  SFRS=2;PIPS7&=0xFE

/**** PIPS0  F9H  PAGE 2 ****/
#define set_PIPS0_PSEL2                  SFRS=2;PIPS0|=0x40
#define set_PIPS0_PSEL1                  SFRS=2;PIPS0|=0x20
#define set_PIPS0_PSEL0                  SFRS=2;PIPS0|=0x10
#define set_PIPS0_BSEL2                  SFRS=2;PIPS0|=0x04
#define set_PIPS0_BSEL1                  SFRS=2;PIPS0|=0x02
#define set_PIPS0_BSEL0                  SFRS=2;PIPS0|=0x01

#define clr_PIPS0_PSEL2                  SFRS=2;PIPS0&=0xBF
#define clr_PIPS0_PSEL1                  SFRS=2;PIPS0&=0xDF
#define clr_PIPS0_PSEL0                  SFRS=2;PIPS0&=0xEF
#define clr_PIPS0_BSEL2                  SFRS=2;PIPS0&=0xFB
#define clr_PIPS0_BSEL1                  SFRS=2;PIPS0&=0xFD
#define clr_PIPS0_BSEL0                  SFRS=2;PIPS0&=0xFE

/**** PIPS1  FAH  PAGE 2 ****/
#define set_PIPS1_PSEL2                  SFRS=2;PIPS1|=0x40
#define set_PIPS1_PSEL1                  SFRS=2;PIPS1|=0x20
#define set_PIPS1_PSEL0                  SFRS=2;PIPS1|=0x10
#define set_PIPS1_BSEL2                  SFRS=2;PIPS1|=0x04
#define set_PIPS1_BSEL1                  SFRS=2;PIPS1|=0x02
#define set_PIPS1_BSEL0                  SFRS=2;PIPS1|=0x01

#define clr_PIPS1_PSEL2                  SFRS=2;PIPS1&=0xBF
#define clr_PIPS1_PSEL1                  SFRS=2;PIPS1&=0xDF
#define clr_PIPS1_PSEL0                  SFRS=2;PIPS1&=0xEF
#define clr_PIPS1_BSEL2                  SFRS=2;PIPS1&=0xFB
#define clr_PIPS1_BSEL1                  SFRS=2;PIPS1&=0xFD
#define clr_PIPS1_BSEL0                  SFRS=2;PIPS1&=0xFE

/**** PIPS2  FBH  PAGE 2 ****/
#define set_PIPS2_PSEL2                  SFRS=2;PIPS2|=0x40
#define set_PIPS2_PSEL1                  SFRS=2;PIPS2|=0x20
#define set_PIPS2_PSEL0                  SFRS=2;PIPS2|=0x10
#define set_PIPS2_BSEL2                  SFRS=2;PIPS2|=0x04
#define set_PIPS2_BSEL1                  SFRS=2;PIPS2|=0x02
#define set_PIPS2_BSEL0                  SFRS=2;PIPS2|=0x01

#define clr_PIPS2_PSEL2                  SFRS=2;PIPS2&=0xBF
#define clr_PIPS2_PSEL1                  SFRS=2;PIPS2&=0xDF
#define clr_PIPS2_PSEL0                  SFRS=2;PIPS2&=0xEF
#define clr_PIPS2_BSEL2                  SFRS=2;PIPS2&=0xFB
#define clr_PIPS2_BSEL1                  SFRS=2;PIPS2&=0xFD
#define clr_PIPS2_BSEL0                  SFRS=2;PIPS2&=0xFE

/**** PIPS3  FCH  PAGE 2 ****/
#define set_PIPS3_PSEL2                  SFRS=2;PIPS3|=0x40
#define set_PIPS3_PSEL1                  SFRS=2;PIPS3|=0x20
#define set_PIPS3_PSEL0                  SFRS=2;PIPS3|=0x10
#define set_PIPS3_BSEL2                  SFRS=2;PIPS3|=0x04
#define set_PIPS3_BSEL1                  SFRS=2;PIPS3|=0x02
#define set_PIPS3_BSEL0                  SFRS=2;PIPS3|=0x01

#define clr_PIPS3_PSEL2                  SFRS=2;PIPS3&=0xBF
#define clr_PIPS3_PSEL1                  SFRS=2;PIPS3&=0xDF
#define clr_PIPS3_PSEL0                  SFRS=2;PIPS3&=0xEF
#define clr_PIPS3_BSEL2                  SFRS=2;PIPS3&=0xFB
#define clr_PIPS3_BSEL1                  SFRS=2;PIPS3&=0xFD
#define clr_PIPS3_BSEL0                  SFRS=2;PIPS3&=0xFE

/**** PIPS4  FDH  PAGE 2 ****/
#define set_PIPS4_PSEL2                  SFRS=2;PIPS4|=0x40
#define set_PIPS4_PSEL1                  SFRS=2;PIPS4|=0x20
#define set_PIPS4_PSEL0                  SFRS=2;PIPS4|=0x10
#define set_PIPS4_BSEL2                  SFRS=2;PIPS4|=0x04
#define set_PIPS4_BSEL1                  SFRS=2;PIPS4|=0x02
#define set_PIPS4_BSEL0                  SFRS=2;PIPS4|=0x01

#define clr_PIPS4_PSEL2                  SFRS=2;PIPS4&=0xBF
#define clr_PIPS4_PSEL1                  SFRS=2;PIPS4&=0xDF
#define clr_PIPS4_PSEL0                  SFRS=2;PIPS4&=0xEF
#define clr_PIPS4_BSEL2                  SFRS=2;PIPS4&=0xFB
#define clr_PIPS4_BSEL1                  SFRS=2;PIPS4&=0xFD
#define clr_PIPS4_BSEL0                  SFRS=2;PIPS4&=0xFE

/**** PIPS5  FEH  PAGE 2 ****/
#define set_PIPS5_PSEL2                  SFRS=2;PIPS5|=0x40
#define set_PIPS5_PSEL1                  SFRS=2;PIPS5|=0x20
#define set_PIPS5_PSEL0                  SFRS=2;PIPS5|=0x10
#define set_PIPS5_BSEL2                  SFRS=2;PIPS5|=0x04
#define set_PIPS5_BSEL1                  SFRS=2;PIPS5|=0x02
#define set_PIPS5_BSEL0                  SFRS=2;PIPS5|=0x01

#define clr_PIPS5_PSEL2                  SFRS=2;PIPS5&=0xBF
#define clr_PIPS5_PSEL1                  SFRS=2;PIPS5&=0xDF
#define clr_PIPS5_PSEL0                  SFRS=2;PIPS5&=0xEF
#define clr_PIPS5_BSEL2                  SFRS=2;PIPS5&=0xFB
#define clr_PIPS5_BSEL1                  SFRS=2;PIPS5&=0xFD
#define clr_PIPS5_BSEL0                  SFRS=2;PIPS5&=0xFE

/**** PIPS6  FFH  PAGE 2 ****/
#define set_PIPS6_PSEL2                  SFRS=2;PIPS6|=0x40
#define set_PIPS6_PSEL1                  SFRS=2;PIPS6|=0x20
#define set_PIPS6_PSEL0                  SFRS=2;PIPS6|=0x10
#define set_PIPS6_BSEL2                  SFRS=2;PIPS6|=0x04
#define set_PIPS6_BSEL1                  SFRS=2;PIPS6|=0x02
#define set_PIPS6_BSEL0                  SFRS=2;PIPS6|=0x01

#define clr_PIPS6_PSEL2                  SFRS=2;PIPS6&=0xBF
#define clr_PIPS6_PSEL1                  SFRS=2;PIPS6&=0xDF
#define clr_PIPS6_PSEL0                  SFRS=2;PIPS6&=0xEF
#define clr_PIPS6_BSEL2                  SFRS=2;PIPS6&=0xFB
#define clr_PIPS6_BSEL1                  SFRS=2;PIPS6&=0xFD
#define clr_PIPS6_BSEL0                  SFRS=2;PIPS6&=0xFE

