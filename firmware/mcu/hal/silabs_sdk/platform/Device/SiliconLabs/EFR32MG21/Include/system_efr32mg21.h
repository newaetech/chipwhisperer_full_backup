/**************************************************************************//**
 * @file
 * @brief CMSIS system header file for EFR32MG21
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

#ifndef SYSTEM_EFR32MG21_H
#define SYSTEM_EFR32MG21_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFR32MG21 EFR32MG21
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   TYPEDEFS   ***********************************
 ******************************************************************************/

/* Interrupt vectortable entry */
typedef union {
  void (*pFunc)(void);
  void *topOfStack;
} tVectorEntry;

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

#if !defined(SYSTEM_NO_STATIC_MEMORY)
extern uint32_t SystemCoreClock;     /**< System Clock Frequency (Core Clock) */
extern uint32_t SystemHfrcoFreq;     /**< System HFRCO frequency */
#endif

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
#if defined(__ICCARM__)    /* IAR requires the __vector_table symbol */
#define __Vectors    __vector_table
#endif
extern const tVectorEntry __Vectors[];
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void Reset_Handler(void);           /**< Reset Handler */
void NMI_Handler(void);             /**< NMI Handler */
void HardFault_Handler(void);       /**< Hard Fault Handler */
void MemManage_Handler(void);       /**< MPU Fault Handler */
void BusFault_Handler(void);        /**< Bus Fault Handler */
void UsageFault_Handler(void);      /**< Usage Fault Handler */
void SVC_Handler(void);             /**< SVCall Handler */
void DebugMon_Handler(void);        /**< Debug Monitor Handler */
void PendSV_Handler(void);          /**< PendSV Handler */
void SysTick_Handler(void);         /**< SysTick Handler */

/* Part Specific Interrupts */
void SETAMPERHOST_IRQHandler(void);   /**< SETAMPERHOST IRQ Handler */
void SEMBRX_IRQHandler(void);         /**< SEMBRX IRQ Handler */
void SEMBTX_IRQHandler(void);         /**< SEMBTX IRQ Handler */
void SMU_SECURE_IRQHandler(void);     /**< SMU_SECURE IRQ Handler */
void SMU_PRIVILEGED_IRQHandler(void); /**< SMU_PRIVILEGED IRQ Handler */
void EMU_IRQHandler(void);            /**< EMU IRQ Handler */
void TIMER0_IRQHandler(void);         /**< TIMER0 IRQ Handler */
void TIMER1_IRQHandler(void);         /**< TIMER1 IRQ Handler */
void TIMER2_IRQHandler(void);         /**< TIMER2 IRQ Handler */
void TIMER3_IRQHandler(void);         /**< TIMER3 IRQ Handler */
void RTCC_IRQHandler(void);           /**< RTCC IRQ Handler */
void USART0_RX_IRQHandler(void);      /**< USART0_RX IRQ Handler */
void USART0_TX_IRQHandler(void);      /**< USART0_TX IRQ Handler */
void USART1_RX_IRQHandler(void);      /**< USART1_RX IRQ Handler */
void USART1_TX_IRQHandler(void);      /**< USART1_TX IRQ Handler */
void USART2_RX_IRQHandler(void);      /**< USART2_RX IRQ Handler */
void USART2_TX_IRQHandler(void);      /**< USART2_TX IRQ Handler */
void ICACHE0_IRQHandler(void);        /**< ICACHE0 IRQ Handler */
void BURTC_IRQHandler(void);          /**< BURTC IRQ Handler */
void LETIMER0_IRQHandler(void);       /**< LETIMER0 IRQ Handler */
void SYSCFG_IRQHandler(void);         /**< SYSCFG IRQ Handler */
void LDMA_IRQHandler(void);           /**< LDMA IRQ Handler */
void LFXO_IRQHandler(void);           /**< LFXO IRQ Handler */
void LFRCO_IRQHandler(void);          /**< LFRCO IRQ Handler */
void ULFRCO_IRQHandler(void);         /**< ULFRCO IRQ Handler */
void GPIO_ODD_IRQHandler(void);       /**< GPIO_ODD IRQ Handler */
void GPIO_EVEN_IRQHandler(void);      /**< GPIO_EVEN IRQ Handler */
void I2C0_IRQHandler(void);           /**< I2C0 IRQ Handler */
void I2C1_IRQHandler(void);           /**< I2C1 IRQ Handler */
void EMUDG_IRQHandler(void);          /**< EMUDG IRQ Handler */
void EMUSE_IRQHandler(void);          /**< EMUSE IRQ Handler */
void AGC_IRQHandler(void);            /**< AGC IRQ Handler */
void BUFC_IRQHandler(void);           /**< BUFC IRQ Handler */
void FRC_PRI_IRQHandler(void);        /**< FRC_PRI IRQ Handler */
void FRC_IRQHandler(void);            /**< FRC IRQ Handler */
void MODEM_IRQHandler(void);          /**< MODEM IRQ Handler */
void PROTIMER_IRQHandler(void);       /**< PROTIMER IRQ Handler */
void RAC_RSM_IRQHandler(void);        /**< RAC_RSM IRQ Handler */
void RAC_SEQ_IRQHandler(void);        /**< RAC_SEQ IRQ Handler */
void PRORTC_IRQHandler(void);         /**< PRORTC IRQ Handler */
void SYNTH_IRQHandler(void);          /**< SYNTH IRQ Handler */
void ACMP0_IRQHandler(void);          /**< ACMP0 IRQ Handler */
void ACMP1_IRQHandler(void);          /**< ACMP1 IRQ Handler */
void WDOG0_IRQHandler(void);          /**< WDOG0 IRQ Handler */
void WDOG1_IRQHandler(void);          /**< WDOG1 IRQ Handler */
void HFXO00_IRQHandler(void);         /**< HFXO00 IRQ Handler */
void HFRCO0_IRQHandler(void);         /**< HFRCO0 IRQ Handler */
void HFRCOEM23_IRQHandler(void);      /**< HFRCOEM23 IRQ Handler */
void CMU_IRQHandler(void);            /**< CMU IRQ Handler */
void AES_IRQHandler(void);            /**< AES IRQ Handler */
void IADC_IRQHandler(void);           /**< IADC IRQ Handler */
void MSC_IRQHandler(void);            /**< MSC IRQ Handler */
void DPLL0_IRQHandler(void);          /**< DPLL0 IRQ Handler */
void SW0_IRQHandler(void);            /**< SW0 IRQ Handler */
void SW1_IRQHandler(void);            /**< SW1 IRQ Handler */
void SW2_IRQHandler(void);            /**< SW2 IRQ Handler */
void SW3_IRQHandler(void);            /**< SW3 IRQ Handler */
void KERNEL0_IRQHandler(void);        /**< KERNEL0 IRQ Handler */
void KERNEL1_IRQHandler(void);        /**< KERNEL1 IRQ Handler */
void M33CTI0_IRQHandler(void);        /**< M33CTI0 IRQ Handler */
void M33CTI1_IRQHandler(void);        /**< M33CTI1 IRQ Handler */

#if (__FPU_PRESENT == 1)
void FPUEH_IRQHandler(void);        /**< FPU IRQ Handler */
#endif

uint32_t SystemHCLKGet(void);

/**************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EMLIB
 *   CMU API, this variable will be kept updated. This function is only
 *   provided for CMSIS compliance and if a user modifies the the core clock
 *   outside the EMLIB CMU API.
 *****************************************************************************/
static __INLINE uint32_t SystemCoreClockGet(void)
{
  return SystemHCLKGet();
}

/**************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EMLIB
 *   CMU API, this variable will be kept updated. This function is only
 *   provided for CMSIS compliance and if a user modifies the the core clock
 *   outside the EMLIB CMU API.
 *****************************************************************************/
static __INLINE void SystemCoreClockUpdate(void)
{
  SystemHCLKGet();
}

void     SystemInit(void);
uint32_t SystemSYSCLKGet(void);
uint32_t SystemMaxCoreClockGet(void);
uint32_t SystemFSRCOClockGet(void);
uint32_t SystemHFXOClockGet(void);
void     SystemHFXOClockSet(uint32_t freq);
uint32_t SystemHFRCODPLLClockGet(void);
void     SystemHFRCODPLLClockSet(uint32_t freq);
uint32_t SystemCLKIN0Get(void);
uint32_t SystemHFRCOEM23ClockGet(void);
uint32_t SystemLFXOClockGet(void);
void     SystemLFXOClockSet(uint32_t freq);
uint32_t SystemLFRCOClockGet(void);
uint32_t SystemULFRCOClockGet(void);

/** @} End of group */
/** @} End of group Parts */

#ifdef __cplusplus
}
#endif
#endif /* SYSTEM_EFR32MG21_H */
