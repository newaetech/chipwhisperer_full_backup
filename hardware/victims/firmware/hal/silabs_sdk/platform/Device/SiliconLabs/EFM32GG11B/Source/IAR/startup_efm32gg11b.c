/***************************************************************************//**
 * @file
 * @brief CMSIS Compatible EFM32GG11B startup file in C for IAR EWARM
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

__weak void IDAC0_IRQHandler(void)
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

__weak void USART2_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART2_TX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void UART0_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void UART0_TX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void UART1_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void UART1_TX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LEUART0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LEUART1_IRQHandler(void)
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

__weak void PCNT1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void PCNT2_IRQHandler(void)
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

__weak void FPUEH_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART3_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART3_TX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART4_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART4_TX_IRQHandler(void)
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

__weak void WTIMER2_IRQHandler(void)
{
  while (true) {
  }
}

__weak void WTIMER3_IRQHandler(void)
{
  while (true) {
  }
}

__weak void I2C2_IRQHandler(void)
{
  while (true) {
  }
}

__weak void VDAC0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TIMER4_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TIMER5_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TIMER6_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART5_RX_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USART5_TX_IRQHandler(void)
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

__weak void EBI_IRQHandler(void)
{
  while (true) {
  }
}

__weak void ACMP2_IRQHandler(void)
{
  while (true) {
  }
}

__weak void ADC1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LCD_IRQHandler(void)
{
  while (true) {
  }
}

__weak void SDIO_IRQHandler(void)
{
  while (true) {
  }
}

__weak void ETH_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CAN0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void CAN1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void USB_IRQHandler(void)
{
  while (true) {
  }
}

__weak void RTC_IRQHandler(void)
{
  while (true) {
  }
}

__weak void WDOG1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void LETIMER1_IRQHandler(void)
{
  while (true) {
  }
}

__weak void TRNG0_IRQHandler(void)
{
  while (true) {
  }
}

__weak void QSPI0_IRQHandler(void)
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
  { MemManage_Handler         },
  { BusFault_Handler          },
  { UsageFault_Handler        },
  { 0                         },
  { 0                         },
  { 0                         },
  { 0                         },
  { SVC_Handler               },
  { DebugMon_Handler          },
  { sl_app_properties         },
  { PendSV_Handler            },
  { SysTick_Handler           },
  { EMU_IRQHandler            },              /* 0 */
  { WDOG0_IRQHandler          },              /* 1 */
  { LDMA_IRQHandler           },              /* 2 */
  { GPIO_EVEN_IRQHandler      },              /* 3 */
  { SMU_IRQHandler            },              /* 4 */
  { TIMER0_IRQHandler         },              /* 5 */
  { USART0_RX_IRQHandler      },              /* 6 */
  { USART0_TX_IRQHandler      },              /* 7 */
  { ACMP0_IRQHandler          },              /* 8 */
  { ADC0_IRQHandler           },              /* 9 */
  { IDAC0_IRQHandler          },              /* 10 */
  { I2C0_IRQHandler           },              /* 11 */
  { I2C1_IRQHandler           },              /* 12 */
  { GPIO_ODD_IRQHandler       },              /* 13 */
  { TIMER1_IRQHandler         },              /* 14 */
  { TIMER2_IRQHandler         },              /* 15 */
  { TIMER3_IRQHandler         },              /* 16 */
  { USART1_RX_IRQHandler      },              /* 17 */
  { USART1_TX_IRQHandler      },              /* 18 */
  { USART2_RX_IRQHandler      },              /* 19 */
  { USART2_TX_IRQHandler      },              /* 20 */
  { UART0_RX_IRQHandler       },              /* 21 */
  { UART0_TX_IRQHandler       },              /* 22 */
  { UART1_RX_IRQHandler       },              /* 23 */
  { UART1_TX_IRQHandler       },              /* 24 */
  { LEUART0_IRQHandler        },              /* 25 */
  { LEUART1_IRQHandler        },              /* 26 */
  { LETIMER0_IRQHandler       },              /* 27 */
  { PCNT0_IRQHandler          },              /* 28 */
  { PCNT1_IRQHandler          },              /* 29 */
  { PCNT2_IRQHandler          },              /* 30 */
  { RTCC_IRQHandler           },              /* 31 */
  { CMU_IRQHandler            },              /* 32 */
  { MSC_IRQHandler            },              /* 33 */
  { CRYPTO0_IRQHandler        },              /* 34 */
  { CRYOTIMER_IRQHandler      },              /* 35 */
  { FPUEH_IRQHandler          },              /* 36 */
  { USART3_RX_IRQHandler      },              /* 37 */
  { USART3_TX_IRQHandler      },              /* 38 */
  { USART4_RX_IRQHandler      },              /* 39 */
  { USART4_TX_IRQHandler      },              /* 40 */
  { WTIMER0_IRQHandler        },              /* 41 */
  { WTIMER1_IRQHandler        },              /* 42 */
  { WTIMER2_IRQHandler        },              /* 43 */
  { WTIMER3_IRQHandler        },              /* 44 */
  { I2C2_IRQHandler           },              /* 45 */
  { VDAC0_IRQHandler          },              /* 46 */
  { TIMER4_IRQHandler         },              /* 47 */
  { TIMER5_IRQHandler         },              /* 48 */
  { TIMER6_IRQHandler         },              /* 49 */
  { USART5_RX_IRQHandler      },              /* 50 */
  { USART5_TX_IRQHandler      },              /* 51 */
  { CSEN_IRQHandler           },              /* 52 */
  { LESENSE_IRQHandler        },              /* 53 */
  { EBI_IRQHandler            },              /* 54 */
  { ACMP2_IRQHandler          },              /* 55 */
  { ADC1_IRQHandler           },              /* 56 */
  { LCD_IRQHandler            },              /* 57 */
  { SDIO_IRQHandler           },              /* 58 */
  { ETH_IRQHandler            },              /* 59 */
  { CAN0_IRQHandler           },              /* 60 */
  { CAN1_IRQHandler           },              /* 61 */
  { USB_IRQHandler            },              /* 62 */
  { RTC_IRQHandler            },              /* 63 */
  { WDOG1_IRQHandler          },              /* 64 */
  { LETIMER1_IRQHandler       },              /* 65 */
  { TRNG0_IRQHandler          },              /* 66 */
  { QSPI0_IRQHandler          },              /* 67 */
};
