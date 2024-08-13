/**************************************************************************//**
 * @file
 * @brief CMSIS Compatible EFR32MG22 startup file in C for IAR EWARM
 ******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/

#include <stdbool.h>
#include "em_device.h"        /* The correct device header file. */

#pragma language=extended
#pragma segment="CSTACK"

/* IAR start function */
extern void __iar_program_start(void);
/* CMSIS init function */
extern void SystemInit(void);

/* Auto defined by linker */
extern unsigned char CSTACK$$Limit;

__weak void Reset_Handler(void)
{
  SystemInit();
  __iar_program_start();
}

/* Provide a dummy value for the sl_app_properties symbol. */
void sl_app_properties(void);   /* Prototype to please MISRA checkers. */
__weak void sl_app_properties(void)
{
}

__weak void NMI_Handler(void)
{
  while (true) {
  }
}

__weak void HardFault_Handler(void)
{
  while (true) {
  }
}

__weak void MemManage_Handler(void)
{
  while (true) {
  }
}

__weak void BusFault_Handler(void)
{
  while (true) {
  }
}

__weak void UsageFault_Handler(void)
{
  while (true) {
  }
}

__weak void SVC_Handler(void)
{
  while (true) {
  }
}

__weak void DebugMon_Handler(void)
{
  while (true) {
  }
}

__weak void PendSV_Handler(void)
{
  while (true) {
  }
}

__weak void SysTick_Handler(void)
{
  while (true) {
  }
}
__weak void CRYPTOACC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TRNG_IRQHandler(void)
{
  while (true) {
  }
}
__weak void PKE_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SMU_SECURE_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SMU_PRIVILEGED_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SMU_NS_PRIVILEGED_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EMU_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TIMER0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TIMER1_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TIMER2_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TIMER3_IRQHandler(void)
{
  while (true) {
  }
}
__weak void TIMER4_IRQHandler(void)
{
  while (true) {
  }
}
__weak void RTCC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void USART0_RX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void USART0_TX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void USART1_RX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void USART1_TX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void ICACHE0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void BURTC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void LETIMER0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SYSCFG_IRQHandler(void)
{
  while (true) {
  }
}
__weak void LDMA_IRQHandler(void)
{
  while (true) {
  }
}
__weak void LFXO_IRQHandler(void)
{
  while (true) {
  }
}
__weak void LFRCO_IRQHandler(void)
{
  while (true) {
  }
}
__weak void ULFRCO_IRQHandler(void)
{
  while (true) {
  }
}
__weak void GPIO_ODD_IRQHandler(void)
{
  while (true) {
  }
}
__weak void GPIO_EVEN_IRQHandler(void)
{
  while (true) {
  }
}
__weak void I2C0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void I2C1_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EMUDG_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EMUSE_IRQHandler(void)
{
  while (true) {
  }
}
__weak void AGC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void BUFC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void FRC_PRI_IRQHandler(void)
{
  while (true) {
  }
}
__weak void FRC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void MODEM_IRQHandler(void)
{
  while (true) {
  }
}
__weak void PROTIMER_IRQHandler(void)
{
  while (true) {
  }
}
__weak void RAC_RSM_IRQHandler(void)
{
  while (true) {
  }
}
__weak void RAC_SEQ_IRQHandler(void)
{
  while (true) {
  }
}
__weak void RDMAILBOX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void RFSENSE_IRQHandler(void)
{
  while (true) {
  }
}
__weak void PRORTC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SYNTH_IRQHandler(void)
{
  while (true) {
  }
}
__weak void WDOG0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void HFXO0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void HFRCO0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void CMU_IRQHandler(void)
{
  while (true) {
  }
}
__weak void AES_IRQHandler(void)
{
  while (true) {
  }
}
__weak void IADC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void MSC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void DPLL0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void PDM_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SW0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SW1_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SW2_IRQHandler(void)
{
  while (true) {
  }
}
__weak void SW3_IRQHandler(void)
{
  while (true) {
  }
}
__weak void KERNEL0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void KERNEL1_IRQHandler(void)
{
  while (true) {
  }
}
__weak void M33CTI0_IRQHandler(void)
{
  while (true) {
  }
}
__weak void M33CTI1_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EMUEFP_IRQHandler(void)
{
  while (true) {
  }
}
__weak void DCDC_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EUART0_RX_IRQHandler(void)
{
  while (true) {
  }
}
__weak void EUART0_TX_IRQHandler(void)
{
  while (true) {
  }
}

#pragma data_alignment=256
#pragma location = ".intvec"
const tVectorEntry __vector_table[] = {
  { .topOfStack = &CSTACK$$Limit },    /* Initial Stack Pointer.
                                        * With IAR, the CSTACK is defined
                                        * via project options settings.   */
  { Reset_Handler },
  { NMI_Handler },
  { HardFault_Handler },
  { MemManage_Handler },
  { BusFault_Handler },
  { UsageFault_Handler },
  { 0 },
  { 0 },
  { 0 },
  { 0 },
  { SVC_Handler },
  { DebugMon_Handler },
  { sl_app_properties },
  { PendSV_Handler },
  { SysTick_Handler },
  { CRYPTOACC_IRQHandler },         /* -16 - CRYPTOACC */
  { TRNG_IRQHandler },              /* -15 - TRNG */
  { PKE_IRQHandler },               /* -14 - PKE */
  { SMU_SECURE_IRQHandler },        /* -13 - SMU_SECURE */
  { SMU_PRIVILEGED_IRQHandler },    /* -12 - SMU_PRIVILEGED */
  { SMU_NS_PRIVILEGED_IRQHandler }, /* -11 - SMU_NS_PRIVILEGED */
  { EMU_IRQHandler },               /* -10 - EMU */
  { TIMER0_IRQHandler },            /* -9 - TIMER0 */
  { TIMER1_IRQHandler },            /* -8 - TIMER1 */
  { TIMER2_IRQHandler },            /* -7 - TIMER2 */
  { TIMER3_IRQHandler },            /* -6 - TIMER3 */
  { TIMER4_IRQHandler },            /* -5 - TIMER4 */
  { RTCC_IRQHandler },              /* -4 - RTCC */
  { USART0_RX_IRQHandler },         /* -3 - USART0_RX */
  { USART0_TX_IRQHandler },         /* -2 - USART0_TX */
  { USART1_RX_IRQHandler },         /* -1 - USART1_RX */
  { USART1_TX_IRQHandler },         /* 00 - USART1_TX */
  { ICACHE0_IRQHandler },           /* 01 - ICACHE0 */
  { BURTC_IRQHandler },             /* 02 - BURTC */
  { LETIMER0_IRQHandler },          /* 03 - LETIMER0 */
  { SYSCFG_IRQHandler },            /* 04 - SYSCFG */
  { LDMA_IRQHandler },              /* 05 - LDMA */
  { LFXO_IRQHandler },              /* 06 - LFXO */
  { LFRCO_IRQHandler },             /* 07 - LFRCO */
  { ULFRCO_IRQHandler },            /* 08 - ULFRCO */
  { GPIO_ODD_IRQHandler },          /* 09 - GPIO_ODD */
  { GPIO_EVEN_IRQHandler },         /* 10 - GPIO_EVEN */
  { I2C0_IRQHandler },              /* 11 - I2C0 */
  { I2C1_IRQHandler },              /* 12 - I2C1 */
  { EMUDG_IRQHandler },             /* 13 - EMUDG */
  { EMUSE_IRQHandler },             /* 14 - EMUSE */
  { AGC_IRQHandler },               /* 15 - AGC */
  { BUFC_IRQHandler },              /* 16 - BUFC */
  { FRC_PRI_IRQHandler },           /* 17 - FRC_PRI */
  { FRC_IRQHandler },               /* 18 - FRC */
  { MODEM_IRQHandler },             /* 19 - MODEM */
  { PROTIMER_IRQHandler },          /* 20 - PROTIMER */
  { RAC_RSM_IRQHandler },           /* 21 - RAC_RSM */
  { RAC_SEQ_IRQHandler },           /* 22 - RAC_SEQ */
  { RDMAILBOX_IRQHandler },         /* 23 - RDMAILBOX */
  { RFSENSE_IRQHandler },           /* 24 - RFSENSE */
  { PRORTC_IRQHandler },            /* 25 - PRORTC */
  { SYNTH_IRQHandler },             /* 26 - SYNTH */
  { WDOG0_IRQHandler },             /* 27 - WDOG0 */
  { HFXO0_IRQHandler },             /* 28 - HFXO0 */
  { HFRCO0_IRQHandler },            /* 29 - HFRCO0 */
  { CMU_IRQHandler },               /* 30 - CMU */
  { AES_IRQHandler },               /* 31 - AES */
  { IADC_IRQHandler },              /* 32 - IADC */
  { MSC_IRQHandler },               /* 33 - MSC */
  { DPLL0_IRQHandler },             /* 34 - DPLL0 */
  { PDM_IRQHandler },               /* 35 - PDM */
  { SW0_IRQHandler },               /* 36 - SW0 */
  { SW1_IRQHandler },               /* 37 - SW1 */
  { SW2_IRQHandler },               /* 38 - SW2 */
  { SW3_IRQHandler },               /* 39 - SW3 */
  { KERNEL0_IRQHandler },           /* 40 - KERNEL0 */
  { KERNEL1_IRQHandler },           /* 41 - KERNEL1 */
  { M33CTI0_IRQHandler },           /* 42 - M33CTI0 */
  { M33CTI1_IRQHandler },           /* 43 - M33CTI1 */
  { EMUEFP_IRQHandler },            /* 44 - EMUEFP */
  { DCDC_IRQHandler },              /* 45 - DCDC */
  { EUART0_RX_IRQHandler },         /* 46 - EUART0_RX */
  { EUART0_TX_IRQHandler },         /* 47 - EUART0_TX */
};
