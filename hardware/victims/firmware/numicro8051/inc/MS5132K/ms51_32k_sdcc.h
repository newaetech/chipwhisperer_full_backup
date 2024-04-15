/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/***********************************************************************************************************/
/*  MS51_32K_SDCC.H                                                                                        */
/*                                                                                                         */
/*  Header file of Nuvoton MS51 32k Series for SDCC                                                        */
/***********************************************************************************************************/
#include "sfr_macro_ms51_32k.h"

/******************************************************************************/
/*                         Macro define header files                          */
/******************************************************************************/

/* MS51_32K SFR BYTE page 0  */
__sfr __at  (0x80)  P0        ;
__sfr __at  (0x81)  SP        ;
__sfr __at  (0x82)  DPL       ;
__sfr __at  (0x83)  DPH       ;
__sfr __at  (0x84)  RCTRIM0   ;
__sfr __at  (0x85)  RCTRIM1   ;
__sfr __at  (0x86)  RWKL      ;
__sfr __at  (0x87)  PCON      ;
                  
__sfr __at  (0x88)  TCON      ;
__sfr __at  (0x89)  TMOD      ;
__sfr __at  (0x8A)  TL0       ;
__sfr __at  (0x8B)  TL1       ;
__sfr __at  (0x8C)  TH0       ;
__sfr __at  (0x8D)  TH1       ;
__sfr __at  (0x8E)  CKCON     ;
__sfr __at  (0x8F)  WKCON     ;
                  
__sfr __at  (0x90)  P1        ;
__sfr __at  (0x91)  SFRS      ;
__sfr __at  (0x92)  CAPCON0   ;
__sfr __at  (0x93)  CAPCON1   ;
__sfr __at  (0x94)  CAPCON2   ;
__sfr __at  (0x95)  CKDIV     ;
__sfr __at  (0x96)  CKSWT     ;
__sfr __at  (0x97)  CKEN      ;
                  
__sfr __at  (0x98)  SCON      ;
__sfr __at  (0x99)  SBUF      ;
__sfr __at  (0x9A)  SBUF_1    ;
__sfr __at  (0x9B)  EIE       ;
__sfr __at  (0x9C)  EIE1      ;
__sfr __at  (0x9F)  CHPCON    ;
                  
__sfr __at  (0xA0)  P2        ;
__sfr __at  (0xA2)  AUXR1     ;
__sfr __at  (0xA3)  BODCON0   ;
__sfr __at  (0xA4)  IAPTRG    ;
__sfr __at  (0xA5)  IAPUEN    ;
__sfr __at  (0xA6)  IAPAL     ;
__sfr __at  (0xA7)  IAPAH     ;
                  
__sfr __at  (0xA8)  IE        ;
__sfr __at  (0xA9)  SADDR     ;
__sfr __at  (0xAA)  WDCON     ;
__sfr __at  (0xAB)  BODCON1   ;
__sfr __at  (0xAC)  P3M1      ;
__sfr __at  (0xAD)  P3M2      ;
__sfr __at  (0xAE)  IAPFD     ;
__sfr __at  (0xAF)  IAPCN     ;
                  
__sfr __at  (0xB0)  P3        ;
__sfr __at  (0xB1)  P0M1      ;
__sfr __at  (0xB2)  P0M2      ;
__sfr __at  (0xB3)  P1M1      ;
__sfr __at  (0xB4)  P1M2      ;
__sfr __at  (0xB5)  TOE       ;
__sfr __at  (0xB7)  IPH       ;
                  
__sfr __at  (0xB8)  IP        ;
__sfr __at  (0xB9)  SADEN     ;
__sfr __at  (0xBA)  SADEN_1   ;
__sfr __at  (0xBB)  SADDR_1   ;
__sfr __at  (0xBC)  I2DAT     ;
__sfr __at  (0xBD)  I2STAT    ;
__sfr __at  (0xBE)  I2CLK     ;
__sfr __at  (0xBF)  I2TOC     ;
                  
__sfr __at  (0xC0)  I2CON     ;
__sfr __at  (0xC1)  I2ADDR    ;
__sfr __at  (0xC2)  ADCRL     ;
__sfr __at  (0xC3)  ADCRH     ;
__sfr __at  (0xC4)  T3CON     ;
__sfr __at  (0xC5)  RL3       ;
__sfr __at  (0xC6)  RH3       ;
__sfr __at  (0xC7)  TA        ;
                  
__sfr __at  (0xC8)  T2CON     ;
__sfr __at  (0xC9)  T2MOD     ;
__sfr __at  (0xCA)  RCMP2L    ;
__sfr __at  (0xCB)  RCMP2H    ;
__sfr __at  (0xCC)  TL2       ;
__sfr __at  (0xCD)  TH2       ;
__sfr __at  (0xCE)  ADCMPL    ;
__sfr __at  (0xCF)  ADCMPH    ;
                  
__sfr __at  (0xD0)  PSW       ;
__sfr __at  (0xD1)  PWM0PH    ;
__sfr __at  (0xD2)  PWM0C0H   ;
__sfr __at  (0xD3)  PWM0C1H   ;
__sfr __at  (0xD4)  PWM0C2H   ;
__sfr __at  (0xD5)  PWM0C3H   ;
__sfr __at  (0xD6)  PNP       ;
__sfr __at  (0xD7)  PWM0FBD   ;
                  
__sfr __at  (0xD8)  PWM0CON0  ;
__sfr __at  (0xD9)  PWM0PL    ;
__sfr __at  (0xDA)  PWM0C0L   ;
__sfr __at  (0xDB)  PWM0C1L   ;
__sfr __at  (0xDC)  PWM0C2L   ;
__sfr __at  (0xDD)  PWM0C3L   ;
__sfr __at  (0xDE)  PIOCON0   ;
__sfr __at  (0xDF)  PWM0CON1  ;
                  
__sfr __at  (0xE0)  ACC       ;
__sfr __at  (0xE1)  ADCCON1   ;
__sfr __at  (0xE2)  ADCCON2   ;
__sfr __at  (0xE3)  ADCDLY    ;
__sfr __at  (0xE4)  C0L       ;
__sfr __at  (0xE5)  C0H       ;
__sfr __at  (0xE6)  C1L       ;
__sfr __at  (0xE7)  C1H       ;
                  
__sfr __at  (0xE8)  ADCCON0   ;
__sfr __at  (0xE9)  PICON     ;
__sfr __at  (0xEA)  PINEN     ;
__sfr __at  (0xEB)  PIPEN     ;
__sfr __at  (0xEC)  PIF       ;
__sfr __at  (0xED)  C2L       ;
__sfr __at  (0xEE)  C2H       ;
__sfr __at  (0xEF)  EIP       ;
                  
__sfr __at  (0xF0)  B         ;
__sfr __at  (0xF1)  CAPCON3   ;
__sfr __at  (0xF2)  CAPCON4   ;
__sfr __at  (0xF3)  SPCR      ;
__sfr __at  (0xF4)  SPSR      ;
__sfr __at  (0xF5)  SPDR      ;
__sfr __at  (0xF6)  AINDIDS0  ;
__sfr __at  (0xF7)  EIPH      ;
                  
__sfr __at  (0xF8)  SCON_1    ;
__sfr __at  (0xF9)  PWM0DTEN  ;
__sfr __at  (0xFA)  PWM0DTCNT ;
__sfr __at  (0xFB)  PWM0MEN   ;
__sfr __at  (0xFC)  PWM0MD    ;
__sfr __at  (0xFD)  PORDIS    ;
__sfr __at  (0xFE)  EIP1      ;
__sfr __at  (0xFF)  EIPH1     ;
                  
/* MS51_32K       SFR BYTE page 1  */
__sfr __at  (0xAC)  P3S       ;
__sfr __at  (0xAD)  P3SR      ;
                  
                  
__sfr __at  (0xB1)  P0S       ;
__sfr __at  (0xB2)  P0SR      ;
__sfr __at  (0xB3)  P1S       ;
__sfr __at  (0xB4)  P1SR      ;
__sfr __at  (0xB7)  PWM0INTC  ;
                  
__sfr __at  (0xC4)  PWM0C4H   ;
__sfr __at  (0xC5)  PWM0C5H   ;
__sfr __at  (0xC6)  PIOCON1   ;
                  
__sfr __at  (0xCC)  PWM0C4L   ;
__sfr __at  (0xCD)  PWM0C5L   ;
                  
__sfr __at  (0xF3)  SPCR2     ;
                  
__sfr __at  (0xFF)  LVRDIS    ;
                  
/* MS51_32K       SFR BYTE page 2  */
__sfr __at  (0x84)  ADCBAL    ;
__sfr __at  (0x85)  ADCBAH    ;
__sfr __at  (0x86)  ADCCON3   ;
                  
__sfr __at  (0x89)  P2M1      ;
__sfr __at  (0x8A)  P2M2      ;
__sfr __at  (0x8B)  P2SR      ;
__sfr __at  (0x8C)  P2S       ;
__sfr __at  (0x8D)  ADCSN     ;
__sfr __at  (0x8E)  ADCCN     ;
__sfr __at  (0x8F)  ADCSR     ;
                  
__sfr __at  (0x92)  P0UP      ;
__sfr __at  (0x93)  P1UP      ;
__sfr __at  (0x94)  P2UP      ;
__sfr __at  (0x95)  P3UP      ;
__sfr __at  (0x97)  RWKH      ;
                  
__sfr __at  (0x99)  AINDIDS1  ;
__sfr __at  (0x9A)  P0DW      ;
__sfr __at  (0x9B)  P1DW      ;
__sfr __at  (0x9C)  P2DW      ;
__sfr __at  (0x9D)  P3DW      ;
                  
__sfr __at  (0xA1)  AUXR2     ;
__sfr __at  (0xA2)  AUXR3     ;
__sfr __at  (0xA3)  AUXR4     ;
__sfr __at  (0xA4)  AUXR5     ;
__sfr __at  (0xA5)  AUXR6     ;
__sfr __at  (0xA6)  AUXR7     ;
__sfr __at  (0xA7)  AUXR8     ;
                  
__sfr __at  (0xA9)  PWM1PH    ;
__sfr __at  (0xAA)  PWM1C0H   ;
__sfr __at  (0xAB)  PWM1C1H   ;
__sfr __at  (0xAC)  PWM1MD    ;
__sfr __at  (0xAD)  PWM1MEN   ;
                  
__sfr __at  (0xB1)  PWM1PL    ;
__sfr __at  (0xB2)  PWM1C0L   ;
__sfr __at  (0xB3)  PWM1C1L   ;
__sfr __at  (0xB4)  PWM1CON0  ;
__sfr __at  (0xB5)  PWM1CON1  ;
__sfr __at  (0xB6)  PWM1INTC  ;
__sfr __at  (0xB7)  PIOCON2   ;
                  
__sfr __at  (0xB9)  PWM2PH    ;
__sfr __at  (0xBA)  PWM2C0H   ;
__sfr __at  (0xBB)  PWM2C1H   ;
__sfr __at  (0xBC)  PWM2MD    ;
__sfr __at  (0xBD)  PWM2MEN   ;
                  
__sfr __at  (0xC1)  PWM2PL    ;
__sfr __at  (0xC2)  PWM2C0L   ;
__sfr __at  (0xC3)  PWM2C1L   ;
__sfr __at  (0xC4)  PWM2CON0  ;
__sfr __at  (0xC5)  PWM2CON1  ;
__sfr __at  (0xC6)  PWM2INTC  ;
                  
__sfr __at  (0xC9)  PWM3PH    ;
__sfr __at  (0xCA)  PWM3C0H   ;
__sfr __at  (0xCB)  PWM3C1H   ;
__sfr __at  (0xCC)  PWM3MD    ;
__sfr __at  (0xCD)  PWM3MEN   ;
__sfr __at  (0xCE)  EIP2      ;
__sfr __at  (0xCF)  EIPH2     ;
                  
__sfr __at  (0xD1)  PWM3PL    ;
__sfr __at  (0xD2)  PWM3C0L   ;
__sfr __at  (0xD3)  PWM3C1L   ;
__sfr __at  (0xD4)  PWM3CON0  ;
__sfr __at  (0xD5)  PWM3CON1  ;
__sfr __at  (0xD6)  PWM3INTC  ;
__sfr __at  (0xD7)  XTLCON    ;
                  
__sfr __at  (0xD9)  SC0DR     ;
__sfr __at  (0xDA)  SC0EGT    ;
__sfr __at  (0xDB)  SC0ETURD0 ;
__sfr __at  (0xDC)  SC0ETURD1 ;
__sfr __at  (0xDD)  SC0IE     ;
__sfr __at  (0xDE)  SC0IS     ;
__sfr __at  (0xDF)  SC0TSR    ;
                  
__sfr __at  (0xE1)  SC1DR     ;
__sfr __at  (0xE2)  SC1EGT    ;
__sfr __at  (0xE3)  SC1ETURD0 ;
__sfr __at  (0xE4)  SC1ETURD1 ;
__sfr __at  (0xE5)  SC1IE     ;
__sfr __at  (0xE6)  SC1IS     ;
__sfr __at  (0xE7)  SC1TSR    ;
                  
__sfr __at  (0xE9)  SC2DR     ;
__sfr __at  (0xEA)  SC2EGT    ;
__sfr __at  (0xEB)  SC2ETURD0 ;
__sfr __at  (0xEC)  SC2ETURD1 ;
__sfr __at  (0xED)  SC2IE     ;
__sfr __at  (0xEE)  SC2IS     ;
__sfr __at  (0xEF)  SC2TSR    ;
                  
__sfr __at  (0xF1)  SC0CR0    ;
__sfr __at  (0xF2)  SC0CR1    ;
__sfr __at  (0xF3)  SC1CR0    ;
__sfr __at  (0xF4)  SC1CR1    ;
__sfr __at  (0xF5)  SC2CR0    ;
__sfr __at  (0xF6)  SC2CR1    ;
__sfr __at  (0xF7)  PIPS7     ;
                  
__sfr __at  (0xF9)  PIPS0     ;
__sfr __at  (0xFA)  PIPS1     ;
__sfr __at  (0xFB)  PIPS2     ;
__sfr __at  (0xFC)  PIPS3     ;
__sfr __at  (0xFD)  PIPS4     ;
__sfr __at  (0xFE)  PIPS5     ;
__sfr __at  (0xFF)  PIPS6     ;

/*  BIT Registers  */
/*  SCON_1  */
__sbit  __at (0xF8+7) SM0_1   ; //SCON_1^7;
__sbit  __at (0xF8+7) FE_1    ; //SCON_1^7
__sbit  __at (0xF8+6) SM1_1   ; //SCON_1^6
__sbit  __at (0xF8+5) SM2_1   ; //SCON_1^5
__sbit  __at (0xF8+4) REN_1   ; //SCON_1^4
__sbit  __at (0xF8+3) TB8_1   ; //SCON_1^3
__sbit  __at (0xF8+2) RB8_1   ; //SCON_1^2
__sbit  __at (0xF8+1) TI_1    ; //SCON_1^1
__sbit  __at (0xF8+0) RI_1    ; //SCON_1^0

/*  ADCCON0  */
__sbit  __at (0xE8+7) ADCF    ; //ADCCON0^7
__sbit  __at (0xE8+6) ADCS    ; //ADCCON0^6
__sbit  __at (0xE8+5) ETGSEL1 ; //ADCCON0^5
__sbit  __at (0xE8+4) ETGSEL0 ; //ADCCON0^4
__sbit  __at (0xE8+3) ADCHS3  ; //ADCCON0^3
__sbit  __at (0xE8+2) ADCHS2  ; //ADCCON0^2
__sbit  __at (0xE8+1) ADCHS1  ; //ADCCON0^1
__sbit  __at (0xE8+0) ADCHS0  ; //ADCCON0^0

/*  PWMCON0  */
__sbit  __at (0xD8+7) PWMRUN  ; //PWMCON0^7
__sbit  __at (0xD8+6) LOAD    ; //PWMCON0^6
__sbit  __at (0xD8+5) PWMF    ; //PWMCON0^5
__sbit  __at (0xD8+4) CLRPWM  ; //PWMCON0^4


/*  PSW */
__sbit  __at (0xD0+7) CY       ; //PSW^7
__sbit  __at (0xD0+6) AC       ; //PSW^6
__sbit  __at (0xD0+5) F0       ; //PSW^5
__sbit  __at (0xD0+4) RS1      ; //PSW^4
__sbit  __at (0xD0+3) RS0      ; //PSW^3
__sbit  __at (0xD0+2) OV       ; //PSW^2
__sbit  __at (0xD0+0) P        ; //PSW^0

/*  T2CON  */
__sbit  __at (0xC8+7) TF2      ; //T2CON^7
__sbit  __at (0xC8+2) TR2      ; //T2CON^2
__sbit  __at (0xC8+0) CM_RL2   ; //T2CON^0
 
/*  I2CON  */
__sbit  __at (0xC0+6) I2CEN    ; //I2CON^6
__sbit  __at (0xC0+5) STA      ; //I2CON^5
__sbit  __at (0xC0+4) STO      ; //I2CON^4
__sbit  __at (0xC0+3) SI       ; //I2CON^3
__sbit  __at (0xC0+2) AA       ; //I2CON^2
__sbit  __at (0xC0+0) I2CPX    ; //I2CON^0

/*  IP  */  
__sbit  __at (0xB8+6) PADC     ; //IP^6
__sbit  __at (0xB8+5) PBOD     ; //IP^5
__sbit  __at (0xB8+4) PS       ; //IP^4
__sbit  __at (0xB8+3) PT1      ; //IP^3
__sbit  __at (0xB8+2) PX1      ; //IP^2
__sbit  __at (0xB8+1) PT0      ; //IP^1
__sbit  __at (0xB8+0) PX0      ; //IP^0

/*  P3  */  
__sbit  __at (0xB0+7) P37      ; // P3^7
__sbit  __at (0xB0+6) P36      ; // P3^6
__sbit  __at (0xB0+5) P35      ; // P3^5
__sbit  __at (0xB0+4) P34      ; // P3^4
__sbit  __at (0xB0+3) P33      ; // P3^3
__sbit  __at (0xB0+2) P32      ; // P3^2 
__sbit  __at (0xB0+1) P31      ; // P3^1
__sbit  __at (0xB0+0) P30      ; // P3^0

/*  IE  */
__sbit  __at (0xA8+7) EA       ; //IE^7
__sbit  __at (0xA8+6) EADC     ; //IE^6
__sbit  __at (0xA8+5) EBOD     ; //IE^5
__sbit  __at (0xA8+4) ES       ; //IE^4
__sbit  __at (0xA8+3) ET1      ; //IE^3
__sbit  __at (0xA8+2) EX1      ; //IE^2
__sbit  __at (0xA8+1) ET0      ; //IE^1
__sbit  __at (0xA8+0) EX0      ; //IE^0

/*  P2  */ 
__sbit  __at (0xA0+0) P20      ; //P2^0

/*  SCON  */
__sbit  __at (0x98+7) SM0      ; //SCON^7
__sbit  __at (0x98+7) FE       ; //SCON^7
__sbit  __at (0x98+6) SM1      ; //SCON^6
__sbit  __at (0x98+5) SM2      ; //SCON^5
__sbit  __at (0x98+4) REN      ; //SCON^4
__sbit  __at (0x98+3) TB8      ; //SCON^3
__sbit  __at (0x98+2) RB8      ; //SCON^2
__sbit  __at (0x98+1) TI       ; //SCON^1
__sbit  __at (0x98+0) RI       ; //SCON^0

/*  P1  */     
__sbit  __at (0x90+7) P17      ; //P1^7
__sbit  __at (0x90+6) P16      ; //P1^6
__sbit  __at (0x90+6) TXD_1    ; //P1^6
__sbit  __at (0x90+5) P15      ; //P1^5
__sbit  __at (0x90+4) P14      ; //P1^4
__sbit  __at (0x90+4) SDA      ; //P1^4
__sbit  __at (0x90+3) P13      ; //P1^3
__sbit  __at (0x90+3) SCL      ; //P1^3
__sbit  __at (0x90+2) P12      ; //P1^2
__sbit  __at (0x90+1) P11      ; //P1^1
__sbit  __at (0x90+0) P10      ; //P1^0

/*  TCON  */
__sbit  __at (0x88+7) TF1      ; //TCON^7
__sbit  __at (0x88+6) TR1      ; //TCON^6
__sbit  __at (0x88+5) TF0      ; //TCON^5
__sbit  __at (0x88+4) TR0      ; //TCON^4
__sbit  __at (0x88+3) IE1      ; //TCON^3
__sbit  __at (0x88+2) IT1      ; //TCON^2
__sbit  __at (0x88+1) IE0      ; //TCON^1
__sbit  __at (0x88+0) IT0      ; //TCON^0

/*  P0  */  
__sbit  __at (0x80+7) P07      ; //P0^7
__sbit  __at (0x80+7) RXD      ; //P0^7
__sbit  __at (0x80+6) P06      ; //P0^6
__sbit  __at (0x80+6) TXD      ; //P0^6
__sbit  __at (0x80+5) P05      ; //P0^5
__sbit  __at (0x80+4) P04      ; //P0^4
__sbit  __at (0x80+4) STADC    ; //P0^4
__sbit  __at (0x80+3) P03      ; //P0^3
__sbit  __at (0x80+2) P02      ; //P0^2
__sbit  __at (0x80+2) RXD_1    ; //P0^2
__sbit  __at (0x80+1) P01      ; //P0^1
__sbit  __at (0x80+1) MISO     ; //P0^1
__sbit  __at (0x80+0) P00      ; //P0^0
__sbit  __at (0x80+0) MOSI     ; //P0^0

