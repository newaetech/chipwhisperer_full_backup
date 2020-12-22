                                                                          
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                           
                                                                          
/************************************************************************/
/*    File Version : V0.5B                                              */
/*    History  : 0.5B  (2015-11-20)  [Hardware Manual Revision : 0.5B]  */
/*    Date Modified: 26/04/2016                                         */
/************************************************************************/

#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

// Exception(Supervisor Instruction)
#pragma interrupt (Excep_SuperVisorInst)
void INT_Excep_SuperVisorInst(void) __attribute__ ((interrupt));

// Exception(Access Instruction)
#pragma interrupt (Excep_AccessInst)
void INT_Excep_AccessInst(void) __attribute__ ((interrupt));

// Exception(Undefined Instruction)
#pragma interrupt (Excep_UndefinedInst)
void INT_Excep_UndefinedInst(void) __attribute__ ((interrupt));

// Exception(Floating Point)
#pragma interrupt (Excep_FloatingPoint)
void INT_Excep_FloatingPoint(void) __attribute__ ((interrupt));

// NMI
#pragma interrupt (NonMaskableInterrupt)
void INT_NonMaskableInterrupt(void) __attribute__ ((interrupt));

// Dummy
#pragma interrupt (Dummy)
void Dummy(void) __attribute__ ((interrupt));

// BRK
#pragma interrupt (Excep_BRK(vect=0))
void INT_Excep_BRK(void) __attribute__ ((interrupt));

// vector  1 reserved
// vector  2 reserved
// vector  3 reserved
// vector  4 reserved
// vector  5 reserved
// vector  6 reserved
// vector  7 reserved
// vector  8 reserved
// vector  9 reserved
// vector 10 reserved
// vector 11 reserved
// vector 12 reserved
// vector 13 reserved
// vector 14 reserved
// vector 15 reserved

// BSC BUSERR
#pragma interrupt (Excep_BSC_BUSERR(vect=16))
void INT_Excep_BSC_BUSERR(void) __attribute__ ((interrupt));

// vector 17 reserved

// RAM RAMERR
#pragma interrupt (Excep_RAM_RAMERR(vect=18))
void INT_Excep_RAM_RAMERR(void) __attribute__ ((interrupt));

// vector 19 reserved
// vector 20 reserved

// FCU FIFERR
#pragma interrupt (Excep_FCU_FIFERR(vect=21))
void INT_Excep_FCU_FIFERR(void) __attribute__ ((interrupt));

// vector 22 reserved

// FCU FRDYI
#pragma interrupt (Excep_FCU_FRDYI(vect=23))
void INT_Excep_FCU_FRDYI(void) __attribute__ ((interrupt));

// vector 24 reserved
// vector 25 reserved

// ICU SWINT2
#pragma interrupt (Excep_ICU_SWINT2(vect=26))
void INT_Excep_ICU_SWINT2(void) __attribute__ ((interrupt));

// ICU SWINT
#pragma interrupt (Excep_ICU_SWINT(vect=27))
void INT_Excep_ICU_SWINT(void) __attribute__ ((interrupt));

// CMT0 CMI0
#pragma interrupt (Excep_CMT0_CMI0(vect=28))
void INT_Excep_CMT0_CMI0(void) __attribute__ ((interrupt));

// CMT1 CMI1
#pragma interrupt (Excep_CMT1_CMI1(vect=29))
void INT_Excep_CMT1_CMI1(void) __attribute__ ((interrupt));

// CMTW0 CMWI0
#pragma interrupt (Excep_CMTW0_CMWI0(vect=30))
void INT_Excep_CMTW0_CMWI0(void) __attribute__ ((interrupt));

// CMTW1 CMWI1
#pragma interrupt (Excep_CMTW1_CMWI1(vect=31))
void INT_Excep_CMTW1_CMWI1(void) __attribute__ ((interrupt));

// vector 32 reserved
// vector 33 reserved

// USB0 D0FIFO0
#pragma interrupt (Excep_USB0_D0FIFO0(vect=34))
void INT_Excep_USB0_D0FIFO0(void) __attribute__ ((interrupt));

// USB0 D1FIFO0
#pragma interrupt (Excep_USB0_D1FIFO0(vect=35))
void INT_Excep_USB0_D1FIFO0(void) __attribute__ ((interrupt));

// vector 36 reserved
// vector 37 reserved

// RSPI0 SPRI0
#pragma interrupt (Excep_RSPI0_SPRI0(vect=38))
void INT_Excep_RSPI0_SPRI0(void) __attribute__ ((interrupt));

// RSPI0 SPTI0
#pragma interrupt (Excep_RSPI0_SPTI0(vect=39))
void INT_Excep_RSPI0_SPTI0(void) __attribute__ ((interrupt));

// RSPI1 SPRI1
#pragma interrupt (Excep_RSPI1_SPRI1(vect=40))
void INT_Excep_RSPI1_SPRI1(void) __attribute__ ((interrupt));

// RSPI1 SPTI1
#pragma interrupt (Excep_RSPI1_SPTI1(vect=41))
void INT_Excep_RSPI1_SPTI1(void) __attribute__ ((interrupt));

// QSPI SPRI
#pragma interrupt (Excep_QSPI_SPRI(vect=42))
void INT_Excep_QSPI_SPRI(void) __attribute__ ((interrupt));

// QSPI SPTI
#pragma interrupt (Excep_QSPI_SPTI(vect=43))
void INT_Excep_QSPI_SPTI(void) __attribute__ ((interrupt));

// SDHI SBFAI
#pragma interrupt (Excep_SDHI_SBFAI(vect=44))
void INT_Excep_SDHI_SBFAI(void) __attribute__ ((interrupt));

// MMCIF MBFAI
#pragma interrupt (Excep_MMCIF_MBFAI(vect=45))
void INT_Excep_MMCIF_MBFAI(void) __attribute__ ((interrupt));

// vector 46 reserved
// vector 47 reserved
// vector 48 reserved
// vector 49 reserved
// vector 50 reserved
// vector 51 reserved

// RIIC0 RXI0
#pragma interrupt (Excep_RIIC0_RXI0(vect=52))
void INT_Excep_RIIC0_RXI0(void) __attribute__ ((interrupt));

// RIIC0 TXI0
#pragma interrupt (Excep_RIIC0_TXI0(vect=53))
void INT_Excep_RIIC0_TXI0(void) __attribute__ ((interrupt));

// RIIC2 RXI2
#pragma interrupt (Excep_RIIC2_RXI2(vect=54))
void INT_Excep_RIIC2_RXI2(void) __attribute__ ((interrupt));

// RIIC2 TXI2
#pragma interrupt (Excep_RIIC2_TXI2(vect=55))
void INT_Excep_RIIC2_TXI2(void) __attribute__ ((interrupt));

// vector 56 reserved
// vector 57 reserved

// SCI0 RXI0
#pragma interrupt (Excep_SCI0_RXI0(vect=58))
void INT_Excep_SCI0_RXI0(void) __attribute__ ((interrupt));

// SCI0 TXI0
#pragma interrupt (Excep_SCI0_TXI0(vect=59))
void INT_Excep_SCI0_TXI0(void) __attribute__ ((interrupt));

// SCI1 RXI1
#pragma interrupt (Excep_SCI1_RXI1(vect=60))
void INT_Excep_SCI1_RXI1(void) __attribute__ ((interrupt));

// SCI1 TXI1
#pragma interrupt (Excep_SCI1_TXI1(vect=61))
void INT_Excep_SCI1_TXI1(void) __attribute__ ((interrupt));

// SCI2 RXI2
#pragma interrupt (Excep_SCI2_RXI2(vect=62))
void INT_Excep_SCI2_RXI2(void) __attribute__ ((interrupt));

// SCI2 TXI2
#pragma interrupt (Excep_SCI2_TXI2(vect=63))
void INT_Excep_SCI2_TXI2(void) __attribute__ ((interrupt));

// ICU IRQ0
#pragma interrupt (Excep_ICU_IRQ0(vect=64))
void INT_Excep_ICU_IRQ0(void) __attribute__ ((interrupt));

// ICU IRQ1
#pragma interrupt (Excep_ICU_IRQ1(vect=65))
void INT_Excep_ICU_IRQ1(void) __attribute__ ((interrupt));

// ICU IRQ2
#pragma interrupt (Excep_ICU_IRQ2(vect=66))
void INT_Excep_ICU_IRQ2(void) __attribute__ ((interrupt));

// ICU IRQ3
#pragma interrupt (Excep_ICU_IRQ3(vect=67))
void INT_Excep_ICU_IRQ3(void) __attribute__ ((interrupt));

// ICU IRQ4
#pragma interrupt (Excep_ICU_IRQ4(vect=68))
void INT_Excep_ICU_IRQ4(void) __attribute__ ((interrupt));

// ICU IRQ5
#pragma interrupt (Excep_ICU_IRQ5(vect=69))
void INT_Excep_ICU_IRQ5(void) __attribute__ ((interrupt));

// ICU IRQ6
#pragma interrupt (Excep_ICU_IRQ6(vect=70))
void INT_Excep_ICU_IRQ6(void) __attribute__ ((interrupt));

// ICU IRQ7
#pragma interrupt (Excep_ICU_IRQ7(vect=71))
void INT_Excep_ICU_IRQ7(void) __attribute__ ((interrupt));

// ICU IRQ8
#pragma interrupt (Excep_ICU_IRQ8(vect=72))
void INT_Excep_ICU_IRQ8(void) __attribute__ ((interrupt));

// ICU IRQ9
#pragma interrupt (Excep_ICU_IRQ9(vect=73))
void INT_Excep_ICU_IRQ9(void) __attribute__ ((interrupt));

// ICU IRQ10
#pragma interrupt (Excep_ICU_IRQ10(vect=74))
void INT_Excep_ICU_IRQ10(void) __attribute__ ((interrupt));

// ICU IRQ11
#pragma interrupt (Excep_ICU_IRQ11(vect=75))
void INT_Excep_ICU_IRQ11(void) __attribute__ ((interrupt));

// ICU IRQ12
#pragma interrupt (Excep_ICU_IRQ12(vect=76))
void INT_Excep_ICU_IRQ12(void) __attribute__ ((interrupt));

// ICU IRQ13
#pragma interrupt (Excep_ICU_IRQ13(vect=77))
void INT_Excep_ICU_IRQ13(void) __attribute__ ((interrupt));

// ICU IRQ14
#pragma interrupt (Excep_ICU_IRQ14(vect=78))
void INT_Excep_ICU_IRQ14(void) __attribute__ ((interrupt));

// ICU IRQ15
#pragma interrupt (Excep_ICU_IRQ15(vect=79))
void INT_Excep_ICU_IRQ15(void) __attribute__ ((interrupt));

// SCI3 RXI3
#pragma interrupt (Excep_SCI3_RXI3(vect=80))
void INT_Excep_SCI3_RXI3(void) __attribute__ ((interrupt));

// SCI3 TXI3
#pragma interrupt (Excep_SCI3_TXI3(vect=81))
void INT_Excep_SCI3_TXI3(void) __attribute__ ((interrupt));

// SCI4 RXI4
#pragma interrupt (Excep_SCI4_RXI4(vect=82))
void INT_Excep_SCI4_RXI4(void) __attribute__ ((interrupt));

// SCI4 TXI4
#pragma interrupt (Excep_SCI4_TXI4(vect=83))
void INT_Excep_SCI4_TXI4(void) __attribute__ ((interrupt));

// SCI5 RXI5
#pragma interrupt (Excep_SCI5_RXI5(vect=84))
void INT_Excep_SCI5_RXI5(void) __attribute__ ((interrupt));

// SCI5 TXI5
#pragma interrupt (Excep_SCI5_TXI5(vect=85))
void INT_Excep_SCI5_TXI5(void) __attribute__ ((interrupt));

// SCI6 RXI6
#pragma interrupt (Excep_SCI6_RXI6(vect=86))
void INT_Excep_SCI6_RXI6(void) __attribute__ ((interrupt));

// SCI6 TXI6
#pragma interrupt (Excep_SCI6_TXI6(vect=87))
void INT_Excep_SCI6_TXI6(void) __attribute__ ((interrupt));

// LVD1 LVD1
#pragma interrupt (Excep_LVD1_LVD1(vect=88))
void INT_Excep_LVD1_LVD1(void) __attribute__ ((interrupt));

// LVD2 LVD2
#pragma interrupt (Excep_LVD2_LVD2(vect=89))
void INT_Excep_LVD2_LVD2(void) __attribute__ ((interrupt));

// USB0 USBR0
#pragma interrupt (Excep_USB0_USBR0(vect=90))
void INT_Excep_USB0_USBR0(void) __attribute__ ((interrupt));

// RTC ALM
#pragma interrupt (Excep_RTC_ALM(vect=92))
void INT_Excep_RTC_ALM(void) __attribute__ ((interrupt));

// RTC PRD
#pragma interrupt (Excep_RTC_PRD(vect=93))
void INT_Excep_RTC_PRD(void) __attribute__ ((interrupt));

// USBA USBAR
#pragma interrupt (Excep_USBA_USBAR(vect=94))
void INT_Excep_USBA_USBAR(void) __attribute__ ((interrupt));

// IWDT IWUNI
#pragma interrupt (Excep_IWDT_IWUNI(vect=95))
void INT_Excep_IWDT_IWUNI(void) __attribute__ ((interrupt));

// WDT WUNI
#pragma interrupt (Excep_WDT_WUNI(vect=96))
void INT_Excep_WDT_WUNI(void) __attribute__ ((interrupt));

// PDC PCDFI
#pragma interrupt (Excep_PDC_PCDFI(vect=97))
void INT_Excep_PDC_PCDFI(void) __attribute__ ((interrupt));

// SCI7 RXI7
#pragma interrupt (Excep_SCI7_RXI7(vect=98))
void INT_Excep_SCI7_RXI7(void) __attribute__ ((interrupt));

// SCI7 TXI7
#pragma interrupt (Excep_SCI7_TXI7(vect=99))
void INT_Excep_SCI7_TXI7(void) __attribute__ ((interrupt));

// SCI8 RXI8
#pragma interrupt (Excep_SCI8_RXI8(vect=100))
void INT_Excep_SCI8_RXI8(void) __attribute__ ((interrupt));

// SCI8 TXI8
#pragma interrupt (Excep_SCI8_TXI8(vect=101))
void INT_Excep_SCI8_TXI8(void) __attribute__ ((interrupt));

// SCI9 RXI9
#pragma interrupt (Excep_SCI9_RXI9(vect=102))
void INT_Excep_SCI9_RXI9(void) __attribute__ ((interrupt));

// SCI9 TXI9
#pragma interrupt (Excep_SCI9_TXI9(vect=103))
void INT_Excep_SCI9_TXI9(void) __attribute__ ((interrupt));

// SCI10 RXI10
#pragma interrupt (Excep_SCI10_RXI10(vect=104))
void INT_Excep_SCI10_RXI10(void) __attribute__ ((interrupt));

// SCI10 TXI10
#pragma interrupt (Excep_SCI10_TXI10(vect=105))
void INT_Excep_SCI10_TXI10(void) __attribute__ ((interrupt));

// ICU GROUPBE0
#pragma interrupt (Excep_ICU_GROUPBE0(vect=106))
void INT_Excep_ICU_GROUPBE0(void) __attribute__ ((interrupt));

// ICU GROUPBL2
#pragma interrupt (Excep_ICU_GROUPBL2(vect=107))
void INT_Excep_ICU_GROUPBL2(void) __attribute__ ((interrupt));

// RSPI2 SPRI2
#pragma interrupt (Excep_RSPI2_SPRI2(vect=108))
void INT_Excep_RSPI2_SPRI2(void) __attribute__ ((interrupt));

// RSPI2 SPTI2
#pragma interrupt (Excep_RSPI2_SPTI2(vect=109))
void INT_Excep_RSPI2_SPTI2(void) __attribute__ ((interrupt));

// ICU GROUPBL0
#pragma interrupt (Excep_ICU_GROUPBL0(vect=110))
void INT_Excep_ICU_GROUPBL0(void) __attribute__ ((interrupt));

// ICU GROUPBL1
#pragma interrupt (Excep_ICU_GROUPBL1(vect=111))
void INT_Excep_ICU_GROUPBL1(void) __attribute__ ((interrupt));

// ICU GROUPAL0
#pragma interrupt (Excep_ICU_GROUPAL0(vect=112))
void INT_Excep_ICU_GROUPAL0(void) __attribute__ ((interrupt));

// ICU GROUPAL1
#pragma interrupt (Excep_ICU_GROUPAL1(vect=113))
void INT_Excep_ICU_GROUPAL1(void) __attribute__ ((interrupt));

// SCI11 RXI11
#pragma interrupt (Excep_SCI11_RXI11(vect=114))
void INT_Excep_SCI11_RXI11(void) __attribute__ ((interrupt));

// SCI11 TXI11
#pragma interrupt (Excep_SCI11_TXI11(vect=115))
void INT_Excep_SCI11_TXI11(void) __attribute__ ((interrupt));

// SCI12 RXI12
#pragma interrupt (Excep_SCI12_RXI12(vect=116))
void INT_Excep_SCI12_RXI12(void) __attribute__ ((interrupt));

// SCI12 TXI12
#pragma interrupt (Excep_SCI12_TXI12(vect=117))
void INT_Excep_SCI12_TXI12(void) __attribute__ ((interrupt));

// vector 118 reserved
// vector 119 reserved

// DMAC DMAC0I
#pragma interrupt (Excep_DMAC_DMAC0I(vect=120))
void INT_Excep_DMAC_DMAC0I(void) __attribute__ ((interrupt));

// DMAC DMAC1I
#pragma interrupt (Excep_DMAC_DMAC1I(vect=121))
void INT_Excep_DMAC_DMAC1I(void) __attribute__ ((interrupt));

// DMAC DMAC2I
#pragma interrupt (Excep_DMAC_DMAC2I(vect=122))
void INT_Excep_DMAC_DMAC2I(void) __attribute__ ((interrupt));

// DMAC DMAC3I
#pragma interrupt (Excep_DMAC_DMAC3I(vect=123))
void INT_Excep_DMAC_DMAC3I(void) __attribute__ ((interrupt));

// DMAC DMAC74I
#pragma interrupt (Excep_DMAC_DMAC74I(vect=124))
void INT_Excep_DMAC_DMAC74I(void) __attribute__ ((interrupt));

// OST OSTDI
#pragma interrupt (Excep_OST_OSTDI(vect=125))
void INT_Excep_OST_OSTDI(void) __attribute__ ((interrupt));

// EXDMAC EXDMAC0I
#pragma interrupt (Excep_EXDMAC_EXDMAC0I(vect=126))
void INT_Excep_EXDMAC_EXDMAC0I(void) __attribute__ ((interrupt));

// EXDMAC EXDMAC1I
#pragma interrupt (Excep_EXDMAC_EXDMAC1I(vect=127))
void INT_Excep_EXDMAC_EXDMAC1I(void) __attribute__ ((interrupt));

// PERIB INTB128
#pragma interrupt (Excep_PERIB_INTB128(vect=128))
void INT_Excep_PERIB_INTB128(void) __attribute__ ((interrupt));

// PERIB INTB129
#pragma interrupt (Excep_PERIB_INTB129(vect=129))
void INT_Excep_PERIB_INTB129(void) __attribute__ ((interrupt));

// PERIB INTB130
#pragma interrupt (Excep_PERIB_INTB130(vect=130))
void INT_Excep_PERIB_INTB130(void) __attribute__ ((interrupt));

// PERIB INTB131
#pragma interrupt (Excep_PERIB_INTB131(vect=131))
void INT_Excep_PERIB_INTB131(void) __attribute__ ((interrupt));

// PERIB INTB132
#pragma interrupt (Excep_PERIB_INTB132(vect=132))
void INT_Excep_PERIB_INTB132(void) __attribute__ ((interrupt));

// PERIB INTB133
#pragma interrupt (Excep_PERIB_INTB133(vect=133))
void INT_Excep_PERIB_INTB133(void) __attribute__ ((interrupt));

// PERIB INTB134
#pragma interrupt (Excep_PERIB_INTB134(vect=134))
void INT_Excep_PERIB_INTB134(void) __attribute__ ((interrupt));

// PERIB INTB135
#pragma interrupt (Excep_PERIB_INTB135(vect=135))
void INT_Excep_PERIB_INTB135(void) __attribute__ ((interrupt));

// PERIB INTB136
#pragma interrupt (Excep_PERIB_INTB136(vect=136))
void INT_Excep_PERIB_INTB136(void) __attribute__ ((interrupt));

// PERIB INTB137
#pragma interrupt (Excep_PERIB_INTB137(vect=137))
void INT_Excep_PERIB_INTB137(void) __attribute__ ((interrupt));

// PERIB INTB138
#pragma interrupt (Excep_PERIB_INTB138(vect=138))
void INT_Excep_PERIB_INTB138(void) __attribute__ ((interrupt));

// PERIB INTB139
#pragma interrupt (Excep_PERIB_INTB139(vect=139))
void INT_Excep_PERIB_INTB139(void) __attribute__ ((interrupt));

// PERIB INTB140
#pragma interrupt (Excep_PERIB_INTB140(vect=140))
void INT_Excep_PERIB_INTB140(void) __attribute__ ((interrupt));

// PERIB INTB141
#pragma interrupt (Excep_PERIB_INTB141(vect=141))
void INT_Excep_PERIB_INTB141(void) __attribute__ ((interrupt));

// PERIB INTB142
#pragma interrupt (Excep_PERIB_INTB142(vect=142))
void INT_Excep_PERIB_INTB142(void) __attribute__ ((interrupt));

// PERIB INTB143
#pragma interrupt (Excep_PERIB_INTB143(vect=143))
void INT_Excep_PERIB_INTB143(void) __attribute__ ((interrupt));

// PERIB INTB144
#pragma interrupt (Excep_PERIB_INTB144(vect=144))
void INT_Excep_PERIB_INTB144(void) __attribute__ ((interrupt));

// PERIB INTB145
#pragma interrupt (Excep_PERIB_INTB145(vect=145))
void INT_Excep_PERIB_INTB145(void) __attribute__ ((interrupt));

// PERIB INTB146
#pragma interrupt (Excep_PERIB_INTB146(vect=146))
void INT_Excep_PERIB_INTB146(void) __attribute__ ((interrupt));

// PERIB INTB147
#pragma interrupt (Excep_PERIB_INTB147(vect=147))
void INT_Excep_PERIB_INTB147(void) __attribute__ ((interrupt));

// PERIB INTB148
#pragma interrupt (Excep_PERIB_INTB148(vect=148))
void INT_Excep_PERIB_INTB148(void) __attribute__ ((interrupt));

// PERIB INTB149
#pragma interrupt (Excep_PERIB_INTB149(vect=149))
void INT_Excep_PERIB_INTB149(void) __attribute__ ((interrupt));

// PERIB INTB150
#pragma interrupt (Excep_PERIB_INTB150(vect=150))
void INT_Excep_PERIB_INTB150(void) __attribute__ ((interrupt));

// PERIB INTB151
#pragma interrupt (Excep_PERIB_INTB151(vect=151))
void INT_Excep_PERIB_INTB151(void) __attribute__ ((interrupt));

// PERIB INTB152
#pragma interrupt (Excep_PERIB_INTB152(vect=152))
void INT_Excep_PERIB_INTB152(void) __attribute__ ((interrupt));

// PERIB INTB153
#pragma interrupt (Excep_PERIB_INTB153(vect=153))
void INT_Excep_PERIB_INTB153(void) __attribute__ ((interrupt));

// PERIB INTB154
#pragma interrupt (Excep_PERIB_INTB154(vect=154))
void INT_Excep_PERIB_INTB154(void) __attribute__ ((interrupt));

// PERIB INTB155
#pragma interrupt (Excep_PERIB_INTB155(vect=155))
void INT_Excep_PERIB_INTB155(void) __attribute__ ((interrupt));

// PERIB INTB156
#pragma interrupt (Excep_PERIB_INTB156(vect=156))
void INT_Excep_PERIB_INTB156(void) __attribute__ ((interrupt));

// PERIB INTB157
#pragma interrupt (Excep_PERIB_INTB157(vect=157))
void INT_Excep_PERIB_INTB157(void) __attribute__ ((interrupt));

// PERIB INTB158
#pragma interrupt (Excep_PERIB_INTB158(vect=158))
void INT_Excep_PERIB_INTB158(void) __attribute__ ((interrupt));

// PERIB INTB159
#pragma interrupt (Excep_PERIB_INTB159(vect=159))
void INT_Excep_PERIB_INTB159(void) __attribute__ ((interrupt));

// PERIB INTB160
#pragma interrupt (Excep_PERIB_INTB160(vect=160))
void INT_Excep_PERIB_INTB160(void) __attribute__ ((interrupt));

// PERIB INTB161
#pragma interrupt (Excep_PERIB_INTB161(vect=161))
void INT_Excep_PERIB_INTB161(void) __attribute__ ((interrupt));

// PERIB INTB162
#pragma interrupt (Excep_PERIB_INTB162(vect=162))
void INT_Excep_PERIB_INTB162(void) __attribute__ ((interrupt));

// PERIB INTB163
#pragma interrupt (Excep_PERIB_INTB163(vect=163))
void INT_Excep_PERIB_INTB163(void) __attribute__ ((interrupt));

// PERIB INTB164
#pragma interrupt (Excep_PERIB_INTB164(vect=164))
void INT_Excep_PERIB_INTB164(void) __attribute__ ((interrupt));

// PERIB INTB165
#pragma interrupt (Excep_PERIB_INTB165(vect=165))
void INT_Excep_PERIB_INTB165(void) __attribute__ ((interrupt));

// PERIB INTB166
#pragma interrupt (Excep_PERIB_INTB166(vect=166))
void INT_Excep_PERIB_INTB166(void) __attribute__ ((interrupt));

// PERIB INTB167
#pragma interrupt (Excep_PERIB_INTB167(vect=167))
void INT_Excep_PERIB_INTB167(void) __attribute__ ((interrupt));

// PERIB INTB168
#pragma interrupt (Excep_PERIB_INTB168(vect=168))
void INT_Excep_PERIB_INTB168(void) __attribute__ ((interrupt));

// PERIB INTB169
#pragma interrupt (Excep_PERIB_INTB169(vect=169))
void INT_Excep_PERIB_INTB169(void) __attribute__ ((interrupt));

// PERIB INTB170
#pragma interrupt (Excep_PERIB_INTB170(vect=170))
void INT_Excep_PERIB_INTB170(void) __attribute__ ((interrupt));

// PERIB INTB171
#pragma interrupt (Excep_PERIB_INTB171(vect=171))
void INT_Excep_PERIB_INTB171(void) __attribute__ ((interrupt));

// PERIB INTB172
#pragma interrupt (Excep_PERIB_INTB172(vect=172))
void INT_Excep_PERIB_INTB172(void) __attribute__ ((interrupt));

// PERIB INTB173
#pragma interrupt (Excep_PERIB_INTB173(vect=173))
void INT_Excep_PERIB_INTB173(void) __attribute__ ((interrupt));

// PERIB INTB174
#pragma interrupt (Excep_PERIB_INTB174(vect=174))
void INT_Excep_PERIB_INTB174(void) __attribute__ ((interrupt));

// PERIB INTB175
#pragma interrupt (Excep_PERIB_INTB175(vect=175))
void INT_Excep_PERIB_INTB175(void) __attribute__ ((interrupt));

// PERIB INTB176
#pragma interrupt (Excep_PERIB_INTB176(vect=176))
void INT_Excep_PERIB_INTB176(void) __attribute__ ((interrupt));

// PERIB INTB177
#pragma interrupt (Excep_PERIB_INTB177(vect=177))
void INT_Excep_PERIB_INTB177(void) __attribute__ ((interrupt));

// PERIB INTB178
#pragma interrupt (Excep_PERIB_INTB178(vect=178))
void INT_Excep_PERIB_INTB178(void) __attribute__ ((interrupt));

// PERIB INTB179
#pragma interrupt (Excep_PERIB_INTB179(vect=179))
void INT_Excep_PERIB_INTB179(void) __attribute__ ((interrupt));

// PERIB INTB180
#pragma interrupt (Excep_PERIB_INTB180(vect=180))
void INT_Excep_PERIB_INTB180(void) __attribute__ ((interrupt));

// PERIB INTB181
#pragma interrupt (Excep_PERIB_INTB181(vect=181))
void INT_Excep_PERIB_INTB181(void) __attribute__ ((interrupt));

// PERIB INTB182
#pragma interrupt (Excep_PERIB_INTB182(vect=182))
void INT_Excep_PERIB_INTB182(void) __attribute__ ((interrupt));

// PERIB INTB183
#pragma interrupt (Excep_PERIB_INTB183(vect=183))
void INT_Excep_PERIB_INTB183(void) __attribute__ ((interrupt));

// PERIB INTB184
#pragma interrupt (Excep_PERIB_INTB184(vect=184))
void INT_Excep_PERIB_INTB184(void) __attribute__ ((interrupt));

// PERIB INTB185
#pragma interrupt (Excep_PERIB_INTB185(vect=185))
void INT_Excep_PERIB_INTB185(void) __attribute__ ((interrupt));

// PERIB INTB186
#pragma interrupt (Excep_PERIB_INTB186(vect=186))
void INT_Excep_PERIB_INTB186(void) __attribute__ ((interrupt));

// PERIB INTB187
#pragma interrupt (Excep_PERIB_INTB187(vect=187))
void INT_Excep_PERIB_INTB187(void) __attribute__ ((interrupt));

// PERIB INTB188
#pragma interrupt (Excep_PERIB_INTB188(vect=188))
void INT_Excep_PERIB_INTB188(void) __attribute__ ((interrupt));

// PERIB INTB189
#pragma interrupt (Excep_PERIB_INTB189(vect=189))
void INT_Excep_PERIB_INTB189(void) __attribute__ ((interrupt));

// PERIB INTB190
#pragma interrupt (Excep_PERIB_INTB190(vect=190))
void INT_Excep_PERIB_INTB190(void) __attribute__ ((interrupt));

// PERIB INTB191
#pragma interrupt (Excep_PERIB_INTB191(vect=191))
void INT_Excep_PERIB_INTB191(void) __attribute__ ((interrupt));

// PERIB INTB192
#pragma interrupt (Excep_PERIB_INTB192(vect=192))
void INT_Excep_PERIB_INTB192(void) __attribute__ ((interrupt));

// PERIB INTB193
#pragma interrupt (Excep_PERIB_INTB193(vect=193))
void INT_Excep_PERIB_INTB193(void) __attribute__ ((interrupt));

// PERIB INTB194
#pragma interrupt (Excep_PERIB_INTB194(vect=194))
void INT_Excep_PERIB_INTB194(void) __attribute__ ((interrupt));

// PERIB INTB195
#pragma interrupt (Excep_PERIB_INTB195(vect=195))
void INT_Excep_PERIB_INTB195(void) __attribute__ ((interrupt));

// PERIB INTB196
#pragma interrupt (Excep_PERIB_INTB196(vect=196))
void INT_Excep_PERIB_INTB196(void) __attribute__ ((interrupt));

// PERIB INTB197
#pragma interrupt (Excep_PERIB_INTB197(vect=197))
void INT_Excep_PERIB_INTB197(void) __attribute__ ((interrupt));

// PERIB INTB198
#pragma interrupt (Excep_PERIB_INTB198(vect=198))
void INT_Excep_PERIB_INTB198(void) __attribute__ ((interrupt));

// PERIB INTB199
#pragma interrupt (Excep_PERIB_INTB199(vect=199))
void INT_Excep_PERIB_INTB199(void) __attribute__ ((interrupt));

// PERIB INTB200
#pragma interrupt (Excep_PERIB_INTB200(vect=200))
void INT_Excep_PERIB_INTB200(void) __attribute__ ((interrupt));

// PERIB INTB201
#pragma interrupt (Excep_PERIB_INTB201(vect=201))
void INT_Excep_PERIB_INTB201(void) __attribute__ ((interrupt));

// PERIB INTB202
#pragma interrupt (Excep_PERIB_INTB202(vect=202))
void INT_Excep_PERIB_INTB202(void) __attribute__ ((interrupt));

// PERIB INTB203
#pragma interrupt (Excep_PERIB_INTB203(vect=203))
void INT_Excep_PERIB_INTB203(void) __attribute__ ((interrupt));

// PERIB INTB204
#pragma interrupt (Excep_PERIB_INTB204(vect=204))
void INT_Excep_PERIB_INTB204(void) __attribute__ ((interrupt));

// PERIB INTB205
#pragma interrupt (Excep_PERIB_INTB205(vect=205))
void INT_Excep_PERIB_INTB205(void) __attribute__ ((interrupt));

// PERIB INTB206
#pragma interrupt (Excep_PERIB_INTB206(vect=206))
void INT_Excep_PERIB_INTB206(void) __attribute__ ((interrupt));

// PERIB INTB207
#pragma interrupt (Excep_PERIB_INTB207(vect=207))
void INT_Excep_PERIB_INTB207(void) __attribute__ ((interrupt));

// PERIA INTA208
#pragma interrupt (Excep_PERIA_INTA208(vect=208))
void INT_Excep_PERIA_INTA208(void) __attribute__ ((interrupt));

// PERIA INTA209
#pragma interrupt (Excep_PERIA_INTA209(vect=209))
void INT_Excep_PERIA_INTA209(void) __attribute__ ((interrupt));

// PERIA INTA210
#pragma interrupt (Excep_PERIA_INTA210(vect=210))
void INT_Excep_PERIA_INTA210(void) __attribute__ ((interrupt));

// PERIA INTA211
#pragma interrupt (Excep_PERIA_INTA211(vect=211))
void INT_Excep_PERIA_INTA211(void) __attribute__ ((interrupt));

// PERIA INTA212
#pragma interrupt (Excep_PERIA_INTA212(vect=212))
void INT_Excep_PERIA_INTA212(void) __attribute__ ((interrupt));

// PERIA INTA213
#pragma interrupt (Excep_PERIA_INTA213(vect=213))
void INT_Excep_PERIA_INTA213(void) __attribute__ ((interrupt));

// PERIA INTA214
#pragma interrupt (Excep_PERIA_INTA214(vect=214))
void INT_Excep_PERIA_INTA214(void) __attribute__ ((interrupt));

// PERIA INTA215
#pragma interrupt (Excep_PERIA_INTA215(vect=215))
void INT_Excep_PERIA_INTA215(void) __attribute__ ((interrupt));

// PERIA INTA216
#pragma interrupt (Excep_PERIA_INTA216(vect=216))
void INT_Excep_PERIA_INTA216(void) __attribute__ ((interrupt));

// PERIA INTA217
#pragma interrupt (Excep_PERIA_INTA217(vect=217))
void INT_Excep_PERIA_INTA217(void) __attribute__ ((interrupt));

// PERIA INTA218
#pragma interrupt (Excep_PERIA_INTA218(vect=218))
void INT_Excep_PERIA_INTA218(void) __attribute__ ((interrupt));

// PERIA INTA219
#pragma interrupt (Excep_PERIA_INTA219(vect=219))
void INT_Excep_PERIA_INTA219(void) __attribute__ ((interrupt));

// PERIA INTA220
#pragma interrupt (Excep_PERIA_INTA220(vect=220))
void INT_Excep_PERIA_INTA220(void) __attribute__ ((interrupt));

// PERIA INTA221
#pragma interrupt (Excep_PERIA_INTA221(vect=221))
void INT_Excep_PERIA_INTA221(void) __attribute__ ((interrupt));

// PERIA INTA222
#pragma interrupt (Excep_PERIA_INTA222(vect=222))
void INT_Excep_PERIA_INTA222(void) __attribute__ ((interrupt));

// PERIA INTA223
#pragma interrupt (Excep_PERIA_INTA223(vect=223))
void INT_Excep_PERIA_INTA223(void) __attribute__ ((interrupt));

// PERIA INTA224
#pragma interrupt (Excep_PERIA_INTA224(vect=224))
void INT_Excep_PERIA_INTA224(void) __attribute__ ((interrupt));

// PERIA INTA225
#pragma interrupt (Excep_PERIA_INTA225(vect=225))
void INT_Excep_PERIA_INTA225(void) __attribute__ ((interrupt));

// PERIA INTA226
#pragma interrupt (Excep_PERIA_INTA226(vect=226))
void INT_Excep_PERIA_INTA226(void) __attribute__ ((interrupt));

// PERIA INTA227
#pragma interrupt (Excep_PERIA_INTA227(vect=227))
void INT_Excep_PERIA_INTA227(void) __attribute__ ((interrupt));

// PERIA INTA228
#pragma interrupt (Excep_PERIA_INTA228(vect=228))
void INT_Excep_PERIA_INTA228(void) __attribute__ ((interrupt));

// PERIA INTA229
#pragma interrupt (Excep_PERIA_INTA229(vect=229))
void INT_Excep_PERIA_INTA229(void) __attribute__ ((interrupt));

// PERIA INTA230
#pragma interrupt (Excep_PERIA_INTA230(vect=230))
void INT_Excep_PERIA_INTA230(void) __attribute__ ((interrupt));

// PERIA INTA231
#pragma interrupt (Excep_PERIA_INTA231(vect=231))
void INT_Excep_PERIA_INTA231(void) __attribute__ ((interrupt));

// PERIA INTA232
#pragma interrupt (Excep_PERIA_INTA232(vect=232))
void INT_Excep_PERIA_INTA232(void) __attribute__ ((interrupt));

// PERIA INTA233
#pragma interrupt (Excep_PERIA_INTA233(vect=233))
void INT_Excep_PERIA_INTA233(void) __attribute__ ((interrupt));

// PERIA INTA234
#pragma interrupt (Excep_PERIA_INTA234(vect=234))
void INT_Excep_PERIA_INTA234(void) __attribute__ ((interrupt));

// PERIA INTA235
#pragma interrupt (Excep_PERIA_INTA235(vect=235))
void INT_Excep_PERIA_INTA235(void) __attribute__ ((interrupt));

// PERIA INTA236
#pragma interrupt (Excep_PERIA_INTA236(vect=236))
void INT_Excep_PERIA_INTA236(void) __attribute__ ((interrupt));

// PERIA INTA237
#pragma interrupt (Excep_PERIA_INTA237(vect=237))
void INT_Excep_PERIA_INTA237(void) __attribute__ ((interrupt));

// PERIA INTA238
#pragma interrupt (Excep_PERIA_INTA238(vect=238))
void INT_Excep_PERIA_INTA238(void) __attribute__ ((interrupt));

// PERIA INTA239
#pragma interrupt (Excep_PERIA_INTA239(vect=239))
void INT_Excep_PERIA_INTA239(void) __attribute__ ((interrupt));

// PERIA INTA240
#pragma interrupt (Excep_PERIA_INTA240(vect=240))
void INT_Excep_PERIA_INTA240(void) __attribute__ ((interrupt));

// PERIA INTA241
#pragma interrupt (Excep_PERIA_INTA241(vect=241))
void INT_Excep_PERIA_INTA241(void) __attribute__ ((interrupt));

// PERIA INTA242
#pragma interrupt (Excep_PERIA_INTA242(vect=242))
void INT_Excep_PERIA_INTA242(void) __attribute__ ((interrupt));

// PERIA INTA243
#pragma interrupt (Excep_PERIA_INTA243(vect=243))
void INT_Excep_PERIA_INTA243(void) __attribute__ ((interrupt));

// PERIA INTA244
#pragma interrupt (Excep_PERIA_INTA244(vect=244))
void INT_Excep_PERIA_INTA244(void) __attribute__ ((interrupt));

// PERIA INTA245
#pragma interrupt (Excep_PERIA_INTA245(vect=245))
void INT_Excep_PERIA_INTA245(void) __attribute__ ((interrupt));

// PERIA INTA246
#pragma interrupt (Excep_PERIA_INTA246(vect=246))
void INT_Excep_PERIA_INTA246(void) __attribute__ ((interrupt));

// PERIA INTA247
#pragma interrupt (Excep_PERIA_INTA247(vect=247))
void INT_Excep_PERIA_INTA247(void) __attribute__ ((interrupt));

// PERIA INTA248
#pragma interrupt (Excep_PERIA_INTA248(vect=248))
void INT_Excep_PERIA_INTA248(void) __attribute__ ((interrupt));

// PERIA INTA249
#pragma interrupt (Excep_PERIA_INTA249(vect=249))
void INT_Excep_PERIA_INTA249(void) __attribute__ ((interrupt));

// PERIA INTA250
#pragma interrupt (Excep_PERIA_INTA250(vect=250))
void INT_Excep_PERIA_INTA250(void) __attribute__ ((interrupt));

// PERIA INTA251
#pragma interrupt (Excep_PERIA_INTA251(vect=251))
void INT_Excep_PERIA_INTA251(void) __attribute__ ((interrupt));

// PERIA INTA252
#pragma interrupt (Excep_PERIA_INTA252(vect=252))
void INT_Excep_PERIA_INTA252(void) __attribute__ ((interrupt));

// PERIA INTA253
#pragma interrupt (Excep_PERIA_INTA253(vect=253))
void INT_Excep_PERIA_INTA253(void) __attribute__ ((interrupt));

// PERIA INTA254
#pragma interrupt (Excep_PERIA_INTA254(vect=254))
void INT_Excep_PERIA_INTA254(void) __attribute__ ((interrupt));

// PERIA INTA255
#pragma interrupt (Excep_PERIA_INTA255(vect=255))
void INT_Excep_PERIA_INTA255(void) __attribute__ ((interrupt));

//;<<VECTOR DATA START (POWER ON RESET)>>
//;Power On Reset PC
extern void PowerON_Reset(void) __attribute__ ((interrupt));                                                                                                                
//;<<VECTOR DATA END (POWER ON RESET)>>

#endif