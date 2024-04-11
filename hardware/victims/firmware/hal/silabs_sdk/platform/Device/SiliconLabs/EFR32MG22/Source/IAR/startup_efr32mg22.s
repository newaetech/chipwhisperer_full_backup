;/**************************************************************************//**
; * @file
; * @brief CMSIS Core Device Startup File for IAR
; *        Silicon Labs EFR32MG22 Device Series
; ******************************************************************************
; * # License
; *
; * The licensor of this software is Silicon Laboratories Inc. Your use of this
; * software is governed by the terms of Silicon Labs Master Software License
; * Agreement (MSLA) available at
; * www.silabs.com/about-us/legal/master-software-license-agreement. This
; * software is Third Party Software licensed by Silicon Labs from a third party
; * and is governed by the sections of the MSLA applicable to Third Party
; * Software and the additional terms set forth below.
; *
; *****************************************************************************/
;/*
; * <b>Copyright 2009-2020 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
;
; When debugging in RAM, it can be located in RAM with at least a 128 byte
; alignment, 256 byte alignment is requied if all interrupt vectors are in use.
;
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;
        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(8)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     MemManage_Handler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     DebugMon_Handler
        DCD     sl_app_properties
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts
        DCD     CRYPTOACC_IRQHandler    ; 0: CRYPTOACC Interrupt
        DCD     TRNG_IRQHandler         ; 1: TRNG Interrupt
        DCD     PKE_IRQHandler          ; 2: PKE Interrupt
        DCD     SMU_SECURE_IRQHandler   ; 3: SMU_SECURE Interrupt
        DCD     SMU_PRIVILEGED_IRQHandler; 4: SMU_PRIVILEGED Interrupt
        DCD     SMU_NS_PRIVILEGED_IRQHandler; 5: SMU_NS_PRIVILEGED Interrupt
        DCD     EMU_IRQHandler          ; 6: EMU Interrupt
        DCD     TIMER0_IRQHandler       ; 7: TIMER0 Interrupt
        DCD     TIMER1_IRQHandler       ; 8: TIMER1 Interrupt
        DCD     TIMER2_IRQHandler       ; 9: TIMER2 Interrupt
        DCD     TIMER3_IRQHandler       ; 10: TIMER3 Interrupt
        DCD     TIMER4_IRQHandler       ; 11: TIMER4 Interrupt
        DCD     RTCC_IRQHandler         ; 12: RTCC Interrupt
        DCD     USART0_RX_IRQHandler    ; 13: USART0_RX Interrupt
        DCD     USART0_TX_IRQHandler    ; 14: USART0_TX Interrupt
        DCD     USART1_RX_IRQHandler    ; 15: USART1_RX Interrupt
        DCD     USART1_TX_IRQHandler    ; 16: USART1_TX Interrupt
        DCD     ICACHE0_IRQHandler      ; 17: ICACHE0 Interrupt
        DCD     BURTC_IRQHandler        ; 18: BURTC Interrupt
        DCD     LETIMER0_IRQHandler     ; 19: LETIMER0 Interrupt
        DCD     SYSCFG_IRQHandler       ; 20: SYSCFG Interrupt
        DCD     LDMA_IRQHandler         ; 21: LDMA Interrupt
        DCD     LFXO_IRQHandler         ; 22: LFXO Interrupt
        DCD     LFRCO_IRQHandler        ; 23: LFRCO Interrupt
        DCD     ULFRCO_IRQHandler       ; 24: ULFRCO Interrupt
        DCD     GPIO_ODD_IRQHandler     ; 25: GPIO_ODD Interrupt
        DCD     GPIO_EVEN_IRQHandler    ; 26: GPIO_EVEN Interrupt
        DCD     I2C0_IRQHandler         ; 27: I2C0 Interrupt
        DCD     I2C1_IRQHandler         ; 28: I2C1 Interrupt
        DCD     EMUDG_IRQHandler        ; 29: EMUDG Interrupt
        DCD     EMUSE_IRQHandler        ; 30: EMUSE Interrupt
        DCD     AGC_IRQHandler          ; 31: AGC Interrupt
        DCD     BUFC_IRQHandler         ; 32: BUFC Interrupt
        DCD     FRC_PRI_IRQHandler      ; 33: FRC_PRI Interrupt
        DCD     FRC_IRQHandler          ; 34: FRC Interrupt
        DCD     MODEM_IRQHandler        ; 35: MODEM Interrupt
        DCD     PROTIMER_IRQHandler     ; 36: PROTIMER Interrupt
        DCD     RAC_RSM_IRQHandler      ; 37: RAC_RSM Interrupt
        DCD     RAC_SEQ_IRQHandler      ; 38: RAC_SEQ Interrupt
        DCD     RDMAILBOX_IRQHandler    ; 39: RDMAILBOX Interrupt
        DCD     RFSENSE_IRQHandler      ; 40: RFSENSE Interrupt
        DCD     PRORTC_IRQHandler       ; 41: PRORTC Interrupt
        DCD     SYNTH_IRQHandler        ; 42: SYNTH Interrupt
        DCD     WDOG0_IRQHandler        ; 43: WDOG0 Interrupt
        DCD     HFXO0_IRQHandler        ; 44: HFXO0 Interrupt
        DCD     HFRCO0_IRQHandler       ; 45: HFRCO0 Interrupt
        DCD     CMU_IRQHandler          ; 46: CMU Interrupt
        DCD     AES_IRQHandler          ; 47: AES Interrupt
        DCD     IADC_IRQHandler         ; 48: IADC Interrupt
        DCD     MSC_IRQHandler          ; 49: MSC Interrupt
        DCD     DPLL0_IRQHandler        ; 50: DPLL0 Interrupt
        DCD     PDM_IRQHandler          ; 51: PDM Interrupt
        DCD     SW0_IRQHandler          ; 52: SW0 Interrupt
        DCD     SW1_IRQHandler          ; 53: SW1 Interrupt
        DCD     SW2_IRQHandler          ; 54: SW2 Interrupt
        DCD     SW3_IRQHandler          ; 55: SW3 Interrupt
        DCD     KERNEL0_IRQHandler      ; 56: KERNEL0 Interrupt
        DCD     KERNEL1_IRQHandler      ; 57: KERNEL1 Interrupt
        DCD     M33CTI0_IRQHandler      ; 58: M33CTI0 Interrupt
        DCD     M33CTI1_IRQHandler      ; 59: M33CTI1 Interrupt
        DCD     EMUEFP_IRQHandler       ; 60: EMUEFP Interrupt
        DCD     DCDC_IRQHandler         ; 61: DCDC Interrupt
        DCD     EUART0_RX_IRQHandler    ; 62: EUART0_RX Interrupt
        DCD     EUART0_TX_IRQHandler    ; 63: EUART0_TX Interrupt

__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        PUBWEAK sl_app_properties
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
sl_app_properties     ; Provide a dummy value for the sl_app_properties symbol.
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        ; Device specific interrupt handlers
        PUBWEAK CRYPTOACC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CRYPTOACC_IRQHandler
        B CRYPTOACC_IRQHandler

        PUBWEAK TRNG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TRNG_IRQHandler
        B TRNG_IRQHandler

        PUBWEAK PKE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PKE_IRQHandler
        B PKE_IRQHandler

        PUBWEAK SMU_SECURE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SMU_SECURE_IRQHandler
        B SMU_SECURE_IRQHandler

        PUBWEAK SMU_PRIVILEGED_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SMU_PRIVILEGED_IRQHandler
        B SMU_PRIVILEGED_IRQHandler

        PUBWEAK SMU_NS_PRIVILEGED_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SMU_NS_PRIVILEGED_IRQHandler
        B SMU_NS_PRIVILEGED_IRQHandler

        PUBWEAK EMU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EMU_IRQHandler
        B EMU_IRQHandler

        PUBWEAK TIMER0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER0_IRQHandler
        B TIMER0_IRQHandler

        PUBWEAK TIMER1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER1_IRQHandler
        B TIMER1_IRQHandler

        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER2_IRQHandler
        B TIMER2_IRQHandler

        PUBWEAK TIMER3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER3_IRQHandler
        B TIMER3_IRQHandler

        PUBWEAK TIMER4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER4_IRQHandler
        B TIMER4_IRQHandler

        PUBWEAK RTCC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTCC_IRQHandler
        B RTCC_IRQHandler

        PUBWEAK USART0_RX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART0_RX_IRQHandler
        B USART0_RX_IRQHandler

        PUBWEAK USART0_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART0_TX_IRQHandler
        B USART0_TX_IRQHandler

        PUBWEAK USART1_RX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_RX_IRQHandler
        B USART1_RX_IRQHandler

        PUBWEAK USART1_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART1_TX_IRQHandler
        B USART1_TX_IRQHandler

        PUBWEAK ICACHE0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ICACHE0_IRQHandler
        B ICACHE0_IRQHandler

        PUBWEAK BURTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BURTC_IRQHandler
        B BURTC_IRQHandler

        PUBWEAK LETIMER0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LETIMER0_IRQHandler
        B LETIMER0_IRQHandler

        PUBWEAK SYSCFG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SYSCFG_IRQHandler
        B SYSCFG_IRQHandler

        PUBWEAK LDMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LDMA_IRQHandler
        B LDMA_IRQHandler

        PUBWEAK LFXO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LFXO_IRQHandler
        B LFXO_IRQHandler

        PUBWEAK LFRCO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LFRCO_IRQHandler
        B LFRCO_IRQHandler

        PUBWEAK ULFRCO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ULFRCO_IRQHandler
        B ULFRCO_IRQHandler

        PUBWEAK GPIO_ODD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO_ODD_IRQHandler
        B GPIO_ODD_IRQHandler

        PUBWEAK GPIO_EVEN_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO_EVEN_IRQHandler
        B GPIO_EVEN_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK EMUDG_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EMUDG_IRQHandler
        B EMUDG_IRQHandler

        PUBWEAK EMUSE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EMUSE_IRQHandler
        B EMUSE_IRQHandler

        PUBWEAK AGC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
AGC_IRQHandler
        B AGC_IRQHandler

        PUBWEAK BUFC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BUFC_IRQHandler
        B BUFC_IRQHandler

        PUBWEAK FRC_PRI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FRC_PRI_IRQHandler
        B FRC_PRI_IRQHandler

        PUBWEAK FRC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FRC_IRQHandler
        B FRC_IRQHandler

        PUBWEAK MODEM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MODEM_IRQHandler
        B MODEM_IRQHandler

        PUBWEAK PROTIMER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PROTIMER_IRQHandler
        B PROTIMER_IRQHandler

        PUBWEAK RAC_RSM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RAC_RSM_IRQHandler
        B RAC_RSM_IRQHandler

        PUBWEAK RAC_SEQ_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RAC_SEQ_IRQHandler
        B RAC_SEQ_IRQHandler

        PUBWEAK RDMAILBOX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RDMAILBOX_IRQHandler
        B RDMAILBOX_IRQHandler

        PUBWEAK RFSENSE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RFSENSE_IRQHandler
        B RFSENSE_IRQHandler

        PUBWEAK PRORTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PRORTC_IRQHandler
        B PRORTC_IRQHandler

        PUBWEAK SYNTH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SYNTH_IRQHandler
        B SYNTH_IRQHandler

        PUBWEAK WDOG0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WDOG0_IRQHandler
        B WDOG0_IRQHandler

        PUBWEAK HFXO0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
HFXO0_IRQHandler
        B HFXO0_IRQHandler

        PUBWEAK HFRCO0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
HFRCO0_IRQHandler
        B HFRCO0_IRQHandler

        PUBWEAK CMU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CMU_IRQHandler
        B CMU_IRQHandler

        PUBWEAK AES_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
AES_IRQHandler
        B AES_IRQHandler

        PUBWEAK IADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
IADC_IRQHandler
        B IADC_IRQHandler

        PUBWEAK MSC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MSC_IRQHandler
        B MSC_IRQHandler

        PUBWEAK DPLL0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DPLL0_IRQHandler
        B DPLL0_IRQHandler

        PUBWEAK PDM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PDM_IRQHandler
        B PDM_IRQHandler

        PUBWEAK SW0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SW0_IRQHandler
        B SW0_IRQHandler

        PUBWEAK SW1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SW1_IRQHandler
        B SW1_IRQHandler

        PUBWEAK SW2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SW2_IRQHandler
        B SW2_IRQHandler

        PUBWEAK SW3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SW3_IRQHandler
        B SW3_IRQHandler

        PUBWEAK KERNEL0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
KERNEL0_IRQHandler
        B KERNEL0_IRQHandler

        PUBWEAK KERNEL1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
KERNEL1_IRQHandler
        B KERNEL1_IRQHandler

        PUBWEAK M33CTI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
M33CTI0_IRQHandler
        B M33CTI0_IRQHandler

        PUBWEAK M33CTI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
M33CTI1_IRQHandler
        B M33CTI1_IRQHandler

        PUBWEAK EMUEFP_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EMUEFP_IRQHandler
        B EMUEFP_IRQHandler

        PUBWEAK DCDC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DCDC_IRQHandler
        B DCDC_IRQHandler

        PUBWEAK EUART0_RX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EUART0_RX_IRQHandler
        B EUART0_RX_IRQHandler

        PUBWEAK EUART0_TX_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EUART0_TX_IRQHandler
        B EUART0_TX_IRQHandler


        END