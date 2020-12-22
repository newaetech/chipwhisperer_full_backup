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
* File Name    : r_cg_vector_table.c
* Version      : 1.2.3
* Device(s)    : R5F565NEDxFP
* Description  : This file implements interrupt vector table.
* Creation Date: 2019-03-02
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
void * const Reserved_Vector[] __attribute((section(".rvectors"))) =
{
    /* 0x0000 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0004 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0008 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x000C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0010 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0014 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0018 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x001C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0020 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0024 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0028 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x002C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0030 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0034 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0038 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x003C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0040 BSC BUSERR */
    bus_error_isr,
    /* 0x0044 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0048 RAM RAMERR */
    undefined_interrupt_source_isr,
    /* 0x004C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0050 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0054 FCU FIFERR */
    undefined_interrupt_source_isr,
    /* 0x0058 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x005C FCU FRDYI */
    undefined_interrupt_source_isr,
    /* 0x0060 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0064 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0068 ICU SWINT2 */
    undefined_interrupt_source_isr,
    /* 0x006C ICU SWINT */
    undefined_interrupt_source_isr,
    /* 0x0070 CMT0 CMI0 */
    undefined_interrupt_source_isr,
    /* 0x0074 CMT1 CMI1 */
    undefined_interrupt_source_isr,
    /* 0x0078 CMTW0 CMWI0 */
    undefined_interrupt_source_isr,
    /* 0x007C CMTW1 CMWI1 */
    undefined_interrupt_source_isr,
    /* 0x0080 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0084 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0088 USB0 D0FIFO0 */
    undefined_interrupt_source_isr,
    /* 0x008C USB0 D1FIFO0 */
    undefined_interrupt_source_isr,
    /* 0x0090 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0094 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0098 RSPI0 SPRI0 */
    undefined_interrupt_source_isr,
    /* 0x009C RSPI0 SPTI0 */
    undefined_interrupt_source_isr,
    /* 0x00A0 RSPI1 SPRI1 */
    undefined_interrupt_source_isr,
    /* 0x00A4 RSPI1 SPTI1 */
    undefined_interrupt_source_isr,
    /* 0x00A8 QSPI SPRI */
    undefined_interrupt_source_isr,
    /* 0x00AC QSPI SPTI */
    undefined_interrupt_source_isr,
    /* 0x00B0 SDHI SBFAI */
    undefined_interrupt_source_isr,
    /* 0x00B4 MMCIF MBFAI */
    undefined_interrupt_source_isr,
    /* 0x00B8 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00BC Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00C0 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00C4 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00C8 RIIC1 RXI1 */
    undefined_interrupt_source_isr,
    /* 0x00CC RIIC1 TXI1 */
    undefined_interrupt_source_isr,
    /* 0x00D0 RIIC0 RXI0 */
    undefined_interrupt_source_isr,
    /* 0x00D4 RIIC0 TXI0 */
    undefined_interrupt_source_isr,
    /* 0x00D8 RIIC2 RXI2 */
    undefined_interrupt_source_isr,
    /* 0x00DC RIIC2 TXI2 */
    undefined_interrupt_source_isr,
    /* 0x00E0 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00E4 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x00E8 SCI0 RXI0 */
    undefined_interrupt_source_isr,
    /* 0x00EC SCI0 TXI0 */
    undefined_interrupt_source_isr,
    /* 0x00F0 SCI1 RXI1 */
    undefined_interrupt_source_isr,
    /* 0x00F4 SCI1 TXI1 */
    undefined_interrupt_source_isr,
    /* 0x00F8 SCI2 RXI2 */
    undefined_interrupt_source_isr,
    /* 0x00FC SCI2 TXI2 */
    undefined_interrupt_source_isr,
    /* 0x0100 ICU IRQ0 */
    undefined_interrupt_source_isr,
    /* 0x0104 ICU IRQ1 */
    undefined_interrupt_source_isr,
    /* 0x0108 ICU IRQ2 */
    undefined_interrupt_source_isr,
    /* 0x010C ICU IRQ3 */
    undefined_interrupt_source_isr,
    /* 0x0110 ICU IRQ4 */
    undefined_interrupt_source_isr,
    /* 0x0114 ICU IRQ5 */
    undefined_interrupt_source_isr,
    /* 0x0118 ICU IRQ6 */
    undefined_interrupt_source_isr,
    /* 0x011C ICU IRQ7 */
    undefined_interrupt_source_isr,
    /* 0x0120 ICU IRQ8 */
    undefined_interrupt_source_isr,
    /* 0x0124 ICU IRQ9 */
    undefined_interrupt_source_isr,
    /* 0x0128 ICU IRQ10 */
    undefined_interrupt_source_isr,
    /* 0x012C ICU IRQ11 */
    undefined_interrupt_source_isr,
    /* 0x0130 ICU IRQ12 */
    undefined_interrupt_source_isr,
    /* 0x0134 ICU IRQ13 */
    undefined_interrupt_source_isr,
    /* 0x0138 ICU IRQ14 */
    undefined_interrupt_source_isr,
    /* 0x013C ICU IRQ15 */
    undefined_interrupt_source_isr,
    /* 0x0140 SCI3 RXI3 */
    undefined_interrupt_source_isr,
    /* 0x0144 SCI3 TXI3 */
    undefined_interrupt_source_isr,
    /* 0x0148 SCI4 RXI4 */
    undefined_interrupt_source_isr,
    /* 0x014C SCI4 TXI4 */
    undefined_interrupt_source_isr,
    /* 0x0150 SCI5 RXI5 */
    undefined_interrupt_source_isr,
    /* 0x0154 SCI5 TXI5 */
    undefined_interrupt_source_isr,
    /* 0x0158 SCI6 RXI6 */
    undefined_interrupt_source_isr,
    /* 0x015C SCI6 TXI6 */
    undefined_interrupt_source_isr,
    /* 0x0160 LVD1 LVD1 */
    undefined_interrupt_source_isr,
    /* 0x0164 LVD2 LVD2 */
    undefined_interrupt_source_isr,
    /* 0x0168 USB0 USBR0 */
    undefined_interrupt_source_isr,
    /* 0x016C Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x0170 RTC ALM */
    undefined_interrupt_source_isr,
    /* 0x0174 RTC PRD */
    undefined_interrupt_source_isr,
    /* 0x0178 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x017C IWDT IWUNI */
    undefined_interrupt_source_isr,
    /* 0x0180 WDT WUNI */
    undefined_interrupt_source_isr,
    /* 0x0184 PDC PCDFI */
    undefined_interrupt_source_isr,
    /* 0x0188 SCI7 RXI7 */
    undefined_interrupt_source_isr,
    /* 0x018C SCI7 TXI7 */
    undefined_interrupt_source_isr,
    /* 0x0190 SCI8 RXI8 */
    undefined_interrupt_source_isr,
    /* 0x0194 SCI8 TXI8 */
    undefined_interrupt_source_isr,
    /* 0x0198 SCI9 RXI9 */
    undefined_interrupt_source_isr,
    /* 0x019C SCI9 TXI9 */
    undefined_interrupt_source_isr,
    /* 0x01A0 SCI10 RXI10 */
    undefined_interrupt_source_isr,
    /* 0x01A4 SCI10 TXI10 */
    undefined_interrupt_source_isr,
    /* 0x01A8 ICU GROUPBE0 */
    undefined_interrupt_source_isr,
    /* 0x01AC ICU GROUPBL2 */
    group_bl2_handler_isr,
    /* 0x01B0 RSPI2 SPRI2 */
    undefined_interrupt_source_isr,
    /* 0x01B4 RSPI2 SPTI2 */
    undefined_interrupt_source_isr,
    /* 0x01B8 ICU GROUPBL0 */
    group_bl0_handler_isr,
    /* 0x01BC ICU GROUPBL1 */
    group_bl1_handler_isr,
    /* 0x01C0 ICU GROUPAL0 */
    group_al0_handler_isr,
    /* 0x01C4 ICU GROUPAL1 */
    group_al1_handler_isr,
    /* 0x01C8 SCI11 RXI11 */
    undefined_interrupt_source_isr,
    /* 0x01CC SCI11 TXI11 */
    undefined_interrupt_source_isr,
    /* 0x01D0 SCI12 RXI12 */
    undefined_interrupt_source_isr,
    /* 0x01D4 SCI12 TXI12 */
    undefined_interrupt_source_isr,
    /* 0x01D8 Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x01DC Reserved */
    (void (*)(void))0xFFFFFFFF,
    /* 0x01E0 DMAC DMAC0I */
    undefined_interrupt_source_isr,
    /* 0x01E4 DMAC DMAC1I */
    undefined_interrupt_source_isr,
    /* 0x01E8 DMAC DMAC2I */
    undefined_interrupt_source_isr,
    /* 0x01EC DMAC DMAC3I */
    undefined_interrupt_source_isr,
    /* 0x01F0 DMAC DMAC74I */
    undefined_interrupt_source_isr,
    /* 0x01F4 OST OSTDI */
    undefined_interrupt_source_isr,
    /* 0x01F8 EXDMAC EXDMAC0I */
    undefined_interrupt_source_isr,
    /* 0x01FC EXDMAC EXDMAC1I */
    undefined_interrupt_source_isr,
    /* 0x0200 PERIB INTB128 */
    undefined_interrupt_source_isr,
    /* 0x0204 PERIB INTB129 */
    undefined_interrupt_source_isr,
    /* 0x0208 PERIB INTB130 */
    undefined_interrupt_source_isr,
    /* 0x020C PERIB INTB131 */
    undefined_interrupt_source_isr,
    /* 0x0210 PERIB INTB132 */
    undefined_interrupt_source_isr,
    /* 0x0214 PERIB INTB133 */
    undefined_interrupt_source_isr,
    /* 0x0218 PERIB INTB134 */
    undefined_interrupt_source_isr,
    /* 0x021C PERIB INTB135 */
    undefined_interrupt_source_isr,
    /* 0x0220 PERIB INTB136 */
    undefined_interrupt_source_isr,
    /* 0x0224 PERIB INTB137 */
    undefined_interrupt_source_isr,
    /* 0x0228 PERIB INTB138 */
    undefined_interrupt_source_isr,
    /* 0x022C PERIB INTB139 */
    undefined_interrupt_source_isr,
    /* 0x0230 PERIB INTB140 */
    undefined_interrupt_source_isr,
    /* 0x0234 PERIB INTB141 */
    undefined_interrupt_source_isr,
    /* 0x0238 PERIB INTB142 */
    undefined_interrupt_source_isr,
    /* 0x023C PERIB INTB143 */
    undefined_interrupt_source_isr,
    /* 0x0240 PERIB INTB144 */
    undefined_interrupt_source_isr,
    /* 0x0244 PERIB INTB145 */
    undefined_interrupt_source_isr,
    /* 0x0248 PERIB INTB146 */
    undefined_interrupt_source_isr,
    /* 0x024C PERIB INTB147 */
    undefined_interrupt_source_isr,
    /* 0x0250 PERIB INTB148 */
    undefined_interrupt_source_isr,
    /* 0x0254 PERIB INTB149 */
    undefined_interrupt_source_isr,
    /* 0x0258 PERIB INTB150 */
    undefined_interrupt_source_isr,
    /* 0x025C PERIB INTB151 */
    undefined_interrupt_source_isr,
    /* 0x0260 PERIB INTB152 */
    undefined_interrupt_source_isr,
    /* 0x0264 PERIB INTB153 */
    undefined_interrupt_source_isr,
    /* 0x0268 PERIB INTB154 */
    undefined_interrupt_source_isr,
    /* 0x026C PERIB INTB155 */
    undefined_interrupt_source_isr,
    /* 0x0270 PERIB INTB156 */
    undefined_interrupt_source_isr,
    /* 0x0274 PERIB INTB157 */
    undefined_interrupt_source_isr,
    /* 0x0278 PERIB INTB158 */
    undefined_interrupt_source_isr,
    /* 0x027C PERIB INTB159 */
    undefined_interrupt_source_isr,
    /* 0x0280 PERIB INTB160 */
    undefined_interrupt_source_isr,
    /* 0x0284 PERIB INTB161 */
    undefined_interrupt_source_isr,
    /* 0x0288 PERIB INTB162 */
    undefined_interrupt_source_isr,
    /* 0x028C PERIB INTB163 */
    undefined_interrupt_source_isr,
    /* 0x0290 PERIB INTB164 */
    undefined_interrupt_source_isr,
    /* 0x0294 PERIB INTB165 */
    undefined_interrupt_source_isr,
    /* 0x0298 PERIB INTB166 */
    undefined_interrupt_source_isr,
    /* 0x029C PERIB INTB167 */
    undefined_interrupt_source_isr,
    /* 0x02A0 PERIB INTB168 */
    undefined_interrupt_source_isr,
    /* 0x02A4 PERIB INTB169 */
    undefined_interrupt_source_isr,
    /* 0x02A8 PERIB INTB170 */
    undefined_interrupt_source_isr,
    /* 0x02AC PERIB INTB171 */
    undefined_interrupt_source_isr,
    /* 0x02B0 PERIB INTB172 */
    undefined_interrupt_source_isr,
    /* 0x02B4 PERIB INTB173 */
    undefined_interrupt_source_isr,
    /* 0x02B8 PERIB INTB174 */
    undefined_interrupt_source_isr,
    /* 0x02BC PERIB INTB175 */
    undefined_interrupt_source_isr,
    /* 0x02C0 PERIB INTB176 */
    undefined_interrupt_source_isr,
    /* 0x02C4 PERIB INTB177 */
    undefined_interrupt_source_isr,
    /* 0x02C8 PERIB INTB178 */
    undefined_interrupt_source_isr,
    /* 0x02CC PERIB INTB179 */
    undefined_interrupt_source_isr,
    /* 0x02D0 PERIB INTB180 */
    undefined_interrupt_source_isr,
    /* 0x02D4 PERIB INTB181 */
    undefined_interrupt_source_isr,
    /* 0x02D8 PERIB INTB182 */
    undefined_interrupt_source_isr,
    /* 0x02DC PERIB INTB183 */
    undefined_interrupt_source_isr,
    /* 0x02E0 PERIB INTB184 */
    undefined_interrupt_source_isr,
    /* 0x02E4 PERIB INTB185 */
    undefined_interrupt_source_isr,
    /* 0x02E8 PERIB INTB186 */
    undefined_interrupt_source_isr,
    /* 0x02EC PERIB INTB187 */
    undefined_interrupt_source_isr,
    /* 0x02F0 PERIB INTB188 */
    undefined_interrupt_source_isr,
    /* 0x02F4 PERIB INTB189 */
    undefined_interrupt_source_isr,
    /* 0x02F8 PERIB INTB190 */
    undefined_interrupt_source_isr,
    /* 0x02FC PERIB INTB191 */
    undefined_interrupt_source_isr,
    /* 0x0300 PERIB INTB192 */
    undefined_interrupt_source_isr,
    /* 0x0304 PERIB INTB193 */
    undefined_interrupt_source_isr,
    /* 0x0308 PERIB INTB194 */
    undefined_interrupt_source_isr,
    /* 0x030C PERIB INTB195 */
    undefined_interrupt_source_isr,
    /* 0x0310 PERIB INTB196 */
    undefined_interrupt_source_isr,
    /* 0x0314 PERIB INTB197 */
    undefined_interrupt_source_isr,
    /* 0x0318 PERIB INTB198 */
    undefined_interrupt_source_isr,
    /* 0x031C PERIB INTB199 */
    undefined_interrupt_source_isr,
    /* 0x0320 PERIB INTB200 */
    undefined_interrupt_source_isr,
    /* 0x0324 PERIB INTB201 */
    undefined_interrupt_source_isr,
    /* 0x0328 PERIB INTB202 */
    undefined_interrupt_source_isr,
    /* 0x032C PERIB INTB203 */
    undefined_interrupt_source_isr,
    /* 0x0330 PERIB INTB204 */
    undefined_interrupt_source_isr,
    /* 0x0334 PERIB INTB205 */
    undefined_interrupt_source_isr,
    /* 0x0338 PERIB INTB206 */
    undefined_interrupt_source_isr,
    /* 0x033C PERIB INTB207 */
    undefined_interrupt_source_isr,
    /* 0x0340 PERIA INTA208 */
    undefined_interrupt_source_isr,
    /* 0x0344 PERIA INTA209 */
    undefined_interrupt_source_isr,
    /* 0x0348 PERIA INTA210 */
    undefined_interrupt_source_isr,
    /* 0x034C PERIA INTA211 */
    undefined_interrupt_source_isr,
    /* 0x0350 PERIA INTA212 */
    undefined_interrupt_source_isr,
    /* 0x0354 PERIA INTA213 */
    undefined_interrupt_source_isr,
    /* 0x0358 PERIA INTA214 */
    undefined_interrupt_source_isr,
    /* 0x035C PERIA INTA215 */
    undefined_interrupt_source_isr,
    /* 0x0360 PERIA INTA216 */
    undefined_interrupt_source_isr,
    /* 0x0364 PERIA INTA217 */
    undefined_interrupt_source_isr,
    /* 0x0368 PERIA INTA218 */
    undefined_interrupt_source_isr,
    /* 0x036C PERIA INTA219 */
    undefined_interrupt_source_isr,
    /* 0x0370 PERIA INTA220 */
    undefined_interrupt_source_isr,
    /* 0x0374 PERIA INTA221 */
    undefined_interrupt_source_isr,
    /* 0x0378 PERIA INTA222 */
    undefined_interrupt_source_isr,
    /* 0x037C PERIA INTA223 */
    undefined_interrupt_source_isr,
    /* 0x0380 PERIA INTA224 */
    undefined_interrupt_source_isr,
    /* 0x0384 PERIA INTA225 */
    undefined_interrupt_source_isr,
    /* 0x0388 PERIA INTA226 */
    undefined_interrupt_source_isr,
    /* 0x038C PERIA INTA227 */
    undefined_interrupt_source_isr,
    /* 0x0390 PERIA INTA228 */
    undefined_interrupt_source_isr,
    /* 0x0394 PERIA INTA229 */
    undefined_interrupt_source_isr,
    /* 0x0398 PERIA INTA230 */
    undefined_interrupt_source_isr,
    /* 0x039C PERIA INTA231 */
    undefined_interrupt_source_isr,
    /* 0x03A0 PERIA INTA232 */
    undefined_interrupt_source_isr,
    /* 0x03A4 PERIA INTA233 */
    undefined_interrupt_source_isr,
    /* 0x03A8 PERIA INTA234 */
    undefined_interrupt_source_isr,
    /* 0x03AC PERIA INTA235 */
    undefined_interrupt_source_isr,
    /* 0x03B0 PERIA INTA236 */
    undefined_interrupt_source_isr,
    /* 0x03B4 PERIA INTA237 */
    undefined_interrupt_source_isr,
    /* 0x03B8 PERIA INTA238 */
    undefined_interrupt_source_isr,
    /* 0x03BC PERIA INTA239 */
    undefined_interrupt_source_isr,
    /* 0x03C0 PERIA INTA240 */
    undefined_interrupt_source_isr,
    /* 0x03C4 PERIA INTA241 */
    undefined_interrupt_source_isr,
    /* 0x03C8 PERIA INTA242 */
    undefined_interrupt_source_isr,
    /* 0x03CC PERIA INTA243 */
    undefined_interrupt_source_isr,
    /* 0x03D0 PERIA INTA244 */
    undefined_interrupt_source_isr,
    /* 0x03D4 PERIA INTA245 */
    undefined_interrupt_source_isr,
    /* 0x03D8 PERIA INTA246 */
    undefined_interrupt_source_isr,
    /* 0x03DC PERIA INTA247 */
    undefined_interrupt_source_isr,
    /* 0x03E0 PERIA INTA248 */
    undefined_interrupt_source_isr,
    /* 0x03E4 PERIA INTA249 */
    undefined_interrupt_source_isr,
    /* 0x03E8 PERIA INTA250 */
    undefined_interrupt_source_isr,
    /* 0x03EC PERIA INTA251 */
    undefined_interrupt_source_isr,
    /* 0x03F0 PERIA INTA252 */
    undefined_interrupt_source_isr,
    /* 0x03F4 PERIA INTA253 */
    undefined_interrupt_source_isr,
    /* 0x03F8 PERIA INTA254 */
    undefined_interrupt_source_isr,
    /* 0x03FC PERIA INTA255 */
    undefined_interrupt_source_isr
};
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
