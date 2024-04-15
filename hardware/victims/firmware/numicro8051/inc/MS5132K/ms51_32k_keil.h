/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2024 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "sfr_macro_ms51_32k.h"

/*--------------------------------------------------------------------------
ms51_32k.h
Nuvoton MG51 64k Series Header file for Keil 
--------------------------------------------------------------------------*/
/* MS51_32K SFR BYTE page 0  */
sfr P0                         =0x80;
sfr SP                         =0x81;
sfr DPL                        =0x82;
sfr DPH                        =0x83;
sfr RCTRIM0                    =0x84;
sfr RCTRIM1                    =0x85;
sfr RWKL                       =0x86;
sfr PCON                       =0x87;

sfr TCON                       =0x88;
sfr TMOD                       =0x89;
sfr TL0                        =0x8A;
sfr TL1                        =0x8B;
sfr TH0                        =0x8C;
sfr TH1                        =0x8D;
sfr CKCON                      =0x8E;
sfr WKCON                      =0x8F;

sfr P1                         =0x90;
sfr SFRS                       =0x91;
sfr CAPCON0                    =0x92;
sfr CAPCON1                    =0x93;
sfr CAPCON2                    =0x94;
sfr CKDIV                      =0x95;
sfr CKSWT                      =0x96;
sfr CKEN                       =0x97;

sfr SCON                       =0x98;
sfr SBUF                       =0x99;
sfr SBUF_1                     =0x9A;
sfr EIE                        =0x9B;
sfr EIE1                       =0x9C;
sfr CHPCON                     =0x9F;

sfr P2                        =0xA0;
sfr AUXR1                      =0xA2;
sfr BODCON0                    =0xA3;
sfr IAPTRG                     =0xA4;
sfr IAPUEN                     =0xA5;
sfr IAPAL                      =0xA6;
sfr IAPAH                      =0xA7;

sfr IE                         =0xA8;
sfr SADDR                      =0xA9;
sfr WDCON                      =0xAA;
sfr BODCON1                    =0xAB;
sfr P3M1                       =0xAC;
sfr P3M2                       =0xAD;
sfr IAPFD                      =0xAE;
sfr IAPCN                      =0xAF;

sfr P3                         =0xB0;
sfr P0M1                       =0xB1;
sfr P0M2                       =0xB2;
sfr P1M1                       =0xB3;
sfr P1M2                       =0xB4;
sfr TOE                        =0xB5;
sfr IPH                        =0xB7;

sfr IP                         =0xB8;
sfr SADEN                      =0xB9;
sfr SADEN_1                    =0xBA;
sfr SADDR_1                    =0xBB;
sfr I2DAT                      =0xBC;
sfr I2STAT                     =0xBD;
sfr I2CLK                      =0xBE;
sfr I2TOC                      =0xBF;

sfr I2CON                      =0xC0;
sfr I2ADDR                     =0xC1;
sfr ADCRL                      =0xC2;
sfr ADCRH                      =0xC3;
sfr T3CON                      =0xC4;
sfr RL3                        =0xC5;
sfr RH3                        =0xC6;
sfr TA                         =0xC7;

sfr T2CON                      =0xC8;
sfr T2MOD                      =0xC9;
sfr RCMP2L                     =0xCA;
sfr RCMP2H                     =0xCB;
sfr TL2                        =0xCC;
sfr TH2                        =0xCD;
sfr ADCMPL                     =0xCE;
sfr ADCMPH                     =0xCF;

sfr PSW                        =0xD0;
sfr PWM0PH                     =0xD1;
sfr PWM0C0H                    =0xD2;
sfr PWM0C1H                    =0xD3;
sfr PWM0C2H                    =0xD4;
sfr PWM0C3H                    =0xD5;
sfr PNP                        =0xD6;
sfr PWM0FBD                    =0xD7;

sfr PWM0CON0                   =0xD8;
sfr PWM0PL                     =0xD9;
sfr PWM0C0L                    =0xDA;
sfr PWM0C1L                    =0xDB;
sfr PWM0C2L                    =0xDC;
sfr PWM0C3L                    =0xDD;
sfr PIOCON0                    =0xDE;
sfr PWM0CON1                   =0xDF;

sfr ACC                        =0xE0;
sfr ADCCON1                    =0xE1;
sfr ADCCON2                    =0xE2;
sfr ADCDLY                     =0xE3;
sfr C0L                        =0xE4;
sfr C0H                        =0xE5;
sfr C1L                        =0xE6;
sfr C1H                        =0xE7;

sfr ADCCON0                    =0xE8;
sfr PICON                      =0xE9;
sfr PINEN                      =0xEA;
sfr PIPEN                      =0xEB;
sfr PIF                        =0xEC;
sfr C2L                        =0xED;
sfr C2H                        =0xEE;
sfr EIP                        =0xEF;

sfr B                          =0xF0;
sfr CAPCON3                    =0xF1;
sfr CAPCON4                    =0xF2;
sfr SPCR                       =0xF3;
sfr SPSR                       =0xF4;
sfr SPDR                       =0xF5;
sfr AINDIDS0                    =0xF6;
sfr EIPH                       =0xF7;

sfr SCON_1                     =0xF8;
sfr PWM0DTEN                   =0xF9;
sfr PWM0DTCNT                  =0xFA;
sfr PWM0MEN                    =0xFB;
sfr PWM0MD                     =0xFC;
sfr PORDIS                     =0xFD;
sfr EIP1                       =0xFE;
sfr EIPH1                      =0xFF;

/* MS51_32K SFR BYTE page 1  */
sfr P3S                        =0xAC;
sfr P3SR                       =0xAD;


sfr P0S                        =0xB1;
sfr P0SR                       =0xB2;
sfr P1S                        =0xB3;
sfr P1SR                       =0xB4;
sfr PWM0INTC                   =0xB7;

sfr PWM0C4H                    =0xC4;
sfr PWM0C5H                    =0xC5;
sfr PIOCON1                    =0xC6;

sfr PWM0C4L                    =0xCC;
sfr PWM0C5L                    =0xCD;

sfr SPCR2                      =0xF3;

sfr LVRDIS                     =0xFF;

/* MS51_32K SFR BYTE page 2  */
sfr ADCBAL                     =0x84;
sfr ADCBAH                     =0x85;
sfr ADCCON3                    =0x86;

sfr P2M1                       =0x89;
sfr P2M2                       =0x8A;
sfr P2SR                       =0x8B;
sfr P2S                        =0x8C;
sfr ADCSN                      =0x8D;
sfr ADCCN                      =0x8E;
sfr ADCSR                      =0x8F;

sfr P0UP                       =0x92;
sfr P1UP                       =0x93;
sfr P2UP                       =0x94;
sfr P3UP                       =0x95;
sfr RWKH                       =0x97;

sfr AINDIDS1                   =0x99;
sfr P0DW                       =0x9A;
sfr P1DW                       =0x9B;
sfr P2DW                       =0x9C;
sfr P3DW                       =0x9D;

sfr AUXR2                      =0xA1;
sfr AUXR3                      =0xA2;
sfr AUXR4                      =0xA3;
sfr AUXR5                      =0xA4;
sfr AUXR6                      =0xA5;
sfr AUXR7                      =0xA6;
sfr AUXR8                      =0xA7;

sfr PWM1PH                     =0xA9;
sfr PWM1C0H                    =0xAA;
sfr PWM1C1H                    =0xAB;
sfr PWM1MD                     =0xAC;
sfr PWM1MEN                    =0xAD;

sfr PWM1PL                     =0xB1;
sfr PWM1C0L                    =0xB2;
sfr PWM1C1L                    =0xB3;
sfr PWM1CON0                   =0xB4;
sfr PWM1CON1                   =0xB5;
sfr PWM1INTC                   =0xB6;
sfr PIOCON2                    =0xB7;

sfr PWM2PH                     =0xB9;
sfr PWM2C0H                    =0xBA;
sfr PWM2C1H                    =0xBB;
sfr PWM2MD                     =0xBC;
sfr PWM2MEN                    =0xBD;

sfr PWM2PL                     =0xC1;
sfr PWM2C0L                    =0xC2;
sfr PWM2C1L                    =0xC3;
sfr PWM2CON0                   =0xC4;
sfr PWM2CON1                   =0xC5;
sfr PWM2INTC                   =0xC6;

sfr PWM3PH                     =0xC9;
sfr PWM3C0H                    =0xCA;
sfr PWM3C1H                    =0xCB;
sfr PWM3MD                     =0xCC;
sfr PWM3MEN                    =0xCD;
sfr EIP2                       =0xCE;
sfr EIPH2                      =0xCF;

sfr PWM3PL                     =0xD1;
sfr PWM3C0L                    =0xD2;
sfr PWM3C1L                    =0xD3;
sfr PWM3CON0                   =0xD4;
sfr PWM3CON1                   =0xD5;
sfr PWM3INTC                   =0xD6;
sfr XTLCON                     =0xD7;

sfr SC0DR                      =0xD9;
sfr SC0EGT                     =0xDA;
sfr SC0ETURD0                  =0xDB;
sfr SC0ETURD1                  =0xDC;
sfr SC0IE                      =0xDD;
sfr SC0IS                      =0xDE;
sfr SC0TSR                     =0xDF;

sfr SC1DR                      =0xE1;
sfr SC1EGT                     =0xE2;
sfr SC1ETURD0                  =0xE3;
sfr SC1ETURD1                  =0xE4;
sfr SC1IE                      =0xE5;
sfr SC1IS                      =0xE6;
sfr SC1TSR                     =0xE7;

sfr SC2DR                      =0xE9;
sfr SC2EGT                     =0xEA;
sfr SC2ETURD0                  =0xEB;
sfr SC2ETURD1                  =0xEC;
sfr SC2IE                      =0xED;
sfr SC2IS                      =0xEE;
sfr SC2TSR                     =0xEF;

sfr SC0CR0                     =0xF1;
sfr SC0CR1                     =0xF2;
sfr SC1CR0                     =0xF3;
sfr SC1CR1                     =0xF4;
sfr SC2CR0                     =0xF5;
sfr SC2CR1                     =0xF6;
sfr PIPS7                      =0xF7;

sfr PIPS0                      =0xF9;
sfr PIPS1                      =0xFA;
sfr PIPS2                      =0xFB;
sfr PIPS3                      =0xFC;
sfr PIPS4                      =0xFD;
sfr PIPS5                      =0xFE;
sfr PIPS6                      =0xFF;

/*  BIT Registers  */
/*  SCON_1  */
sbit SM0_1      = SCON_1^7;
sbit FE_1       = SCON_1^7; 
sbit SM1_1      = SCON_1^6; 
sbit SM2_1      = SCON_1^5; 
sbit REN_1      = SCON_1^4; 
sbit TB8_1      = SCON_1^3; 
sbit RB8_1      = SCON_1^2; 
sbit TI_1       = SCON_1^1; 
sbit RI_1       = SCON_1^0; 

/*  ADCCON0  */
sbit ADCF       = ADCCON0^7;
sbit ADCS       = ADCCON0^6;
sbit ETGSEL1    = ADCCON0^5;
sbit ETGSEL0    = ADCCON0^4;
sbit ADCHS3     = ADCCON0^3;
sbit ADCHS2     = ADCCON0^2;
sbit ADCHS1     = ADCCON0^1;
sbit ADCHS0     = ADCCON0^0;

/*  PWMCON0  */
sbit PWMRUN     = PWM0CON0^7;
sbit LOAD       = PWM0CON0^6;
sbit PWMF       = PWM0CON0^5;
sbit CLRPWM     = PWM0CON0^4;


/*  PSW */
sbit CY         = PSW^7;
sbit AC         = PSW^6;
sbit F0         = PSW^5;
sbit RS1        = PSW^4;
sbit RS0        = PSW^3;
sbit OV         = PSW^2;
sbit P          = PSW^0;

/*  T2CON  */
sbit TF2        = T2CON^7;
sbit TR2        = T2CON^2;
sbit CM_RL2     = T2CON^0;
 
/*  I2CON  */
sbit I2CEN      = I2CON^6;
sbit STA        = I2CON^5;
sbit STO        = I2CON^4;
sbit SI         = I2CON^3;
sbit AA         = I2CON^2;
sbit I2CPX  = I2CON^0;

/*  IP  */  
sbit PADC       = IP^6;
sbit PBOD       = IP^5;
sbit PS         = IP^4;
sbit PT1        = IP^3;
sbit PX1        = IP^2;
sbit PT0        = IP^1;
sbit PX0        = IP^0;

/*  P3  */  
sbit P37  = P3^7;
sbit P36  = P3^6;
sbit P35  = P3^5;
sbit P34  = P3^4;
sbit P33  = P3^3;
sbit P32  = P3^2; 
sbit P31  = P3^1;
sbit P30  = P3^0;

/*  IE  */
sbit EA         = IE^7;
sbit EADC       = IE^6;
sbit EBOD       = IE^5;
sbit ES         = IE^4;
sbit ET1        = IE^3;
sbit EX1        = IE^2;
sbit ET0        = IE^1;
sbit EX0        = IE^0;

/*  P2  */  
sbit P27  = P2^7;
sbit P26  = P2^6;
sbit P25  = P2^5;
sbit P24  = P2^4;
sbit P23  = P2^3;
sbit P22  = P2^2; 
sbit P21  = P2^1;
sbit P20  = P2^0;

/*  SCON  */
sbit SM0        = SCON^7;
sbit FE         = SCON^7; 
sbit SM1        = SCON^6; 
sbit SM2        = SCON^5; 
sbit REN        = SCON^4; 
sbit TB8        = SCON^3; 
sbit RB8        = SCON^2; 
sbit TI         = SCON^1; 
sbit RI         = SCON^0; 

/*  P1  */     
sbit P17  = P1^7;
sbit P16  = P1^6;
sbit TXD_1  = P1^6; 
sbit P15  = P1^5;
sbit P14  = P1^4;
sbit SDA  = P1^4;    
sbit P13  = P1^3;
sbit SCL  = P1^3;  
sbit P12        = P1^2; 
sbit P11        = P1^1;
sbit P10        = P1^0;

/*  TCON  */
sbit TF1        = TCON^7;
sbit TR1        = TCON^6;
sbit TF0        = TCON^5;
sbit TR0        = TCON^4;
sbit IE1        = TCON^3;
sbit IT1        = TCON^2;
sbit IE0        = TCON^1;
sbit IT0        = TCON^0;

/*  P0  */  

sbit P07        = P0^7;
sbit RXD        = P0^7;
sbit P06        = P0^6;
sbit TXD        = P0^6;
sbit P05        = P0^5;
sbit P04        = P0^4;
sbit STADC      = P0^4;
sbit P03        = P0^3;
sbit P02        = P0^2;
sbit RXD_1      = P0^2;
sbit P01        = P0^1;
sbit MISO       = P0^1;
sbit P00        = P0^0;
sbit MOSI       = P0^0;

