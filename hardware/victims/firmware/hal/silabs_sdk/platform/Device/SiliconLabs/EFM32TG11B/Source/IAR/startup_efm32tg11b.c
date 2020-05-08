/***************************************************************************//**
 * @file
 * @brief CMSIS Compatible EFM32TG11B startup file in C for IAR EWARM
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
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
 ******************************************************************************/

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

__weak void SVC_Handler(void)
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

__weak void EMU_IRQHandler(void)
{
  while (true) {
  }
}

__weak void WDOG0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LDMA_IRQHandler(void)
{
  while (true) {
  }
}

__weak void GPIO_EVEN_IRQHandler(void)
{
  while (true) {
  }
}

__weak void SMU_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TIMER0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void ACMP0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void ADC0_IRQHandler(void)
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

__weak void GPIO_ODD_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TIMER1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART2_IRQHandler(void)
{
  while (true) {
  }
}

__weak void UART0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LEUART0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LETIMER0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void PCNT0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void RTCC_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CMU_IRQHandler(void)
{
  while (true) {
  }
}

__weak void MSC_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CRYPTO0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CRYOTIMER_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART3_IRQHandler(void)
{
  while (true) {
  }
}

__weak void WTIMER0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void WTIMER1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void VDAC0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CSEN_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LESENSE_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LCD_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CAN0_IRQHandler(void)
{
  while (true) {
  }
}

#pragma data_alignment=256
#pragma location = ".intvec"
const tVectorEntry __vector_table[] = {
  { .topOfStack = &CSTACK$$Limit },  /* With IAR, the CSTACK is defined via */
                                     /* project options settings */

  { Reset_Handler             },
  { NMI_Handler               },
  { HardFault_Handler         },
  { 0                         },
  { 0                         },
  { 0                         },
  { 0                         },
  { 0                         },
  { 0                         },
  { 0                         },
  { SVC_Handler               },
  { 0                         },
  { sl_app_properties         },
  { PendSV_Handler            },
  { SysTick_Handler           },
  { EMU_IRQHandler            },              /* 0 */
  { WDOG0_IRQHandler          },              /* 1 */
  { LDMA_IRQHandler           },              /* 2 */
  { GPIO_EVEN_IRQHandler      },              /* 3 */
  { SMU_IRQHandler            },              /* 4 */
  { TIMER0_IRQHandler         },              /* 5 */
  { USART0_IRQHandler         },              /* 6 */
  { ACMP0_IRQHandler          },              /* 7 */
  { ADC0_IRQHandler           },              /* 8 */
  { I2C0_IRQHandler           },              /* 9 */
  { I2C1_IRQHandler           },              /* 10 */
  { GPIO_ODD_IRQHandler       },              /* 11 */
  { TIMER1_IRQHandler         },              /* 12 */
  { USART1_IRQHandler         },              /* 13 */
  { USART2_IRQHandler         },              /* 14 */
  { UART0_IRQHandler          },              /* 15 */
  { LEUART0_IRQHandler        },              /* 16 */
  { LETIMER0_IRQHandler       },              /* 17 */
  { PCNT0_IRQHandler          },              /* 18 */
  { RTCC_IRQHandler           },              /* 19 */
  { CMU_IRQHandler            },              /* 20 */
  { MSC_IRQHandler            },              /* 21 */
  { CRYPTO0_IRQHandler        },              /* 22 */
  { CRYOTIMER_IRQHandler      },              /* 23 */
  { USART3_IRQHandler         },              /* 24 */
  { WTIMER0_IRQHandler        },              /* 25 */
  { WTIMER1_IRQHandler        },              /* 26 */
  { VDAC0_IRQHandler          },              /* 27 */
  { CSEN_IRQHandler           },              /* 28 */
  { LESENSE_IRQHandler        },              /* 29 */
  { LCD_IRQHandler            },              /* 30 */
  { CAN0_IRQHandler           },              /* 31 */
};
