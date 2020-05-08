;/**************************************************************************//**
; * @file
; * @brief    CMSIS Core Device Startup File for
; *           Silicon Labs EFM32TG11B Device Series
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
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
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

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
                IF :DEF: __STACK_SIZE
Stack_Size      EQU     __STACK_SIZE
                ELSE
Stack_Size      EQU     0x00000400
                ENDIF

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
                IF :DEF: __HEAP_SIZE
Heap_Size       EQU     __HEAP_SIZE
                ELSE
Heap_Size       EQU     0x0
                ENDIF

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY, ALIGN=8
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     sl_app_properties         ; Application properties
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts

                DCD     EMU_IRQHandler            ; 0: EMU Interrupt
                DCD     WDOG0_IRQHandler          ; 1: WDOG0 Interrupt
                DCD     LDMA_IRQHandler           ; 2: LDMA Interrupt
                DCD     GPIO_EVEN_IRQHandler      ; 3: GPIO_EVEN Interrupt
                DCD     SMU_IRQHandler            ; 4: SMU Interrupt
                DCD     TIMER0_IRQHandler         ; 5: TIMER0 Interrupt
                DCD     USART0_IRQHandler         ; 6: USART0 Interrupt
                DCD     ACMP0_IRQHandler          ; 7: ACMP0 Interrupt
                DCD     ADC0_IRQHandler           ; 8: ADC0 Interrupt
                DCD     I2C0_IRQHandler           ; 9: I2C0 Interrupt
                DCD     I2C1_IRQHandler           ; 10: I2C1 Interrupt
                DCD     GPIO_ODD_IRQHandler       ; 11: GPIO_ODD Interrupt
                DCD     TIMER1_IRQHandler         ; 12: TIMER1 Interrupt
                DCD     USART1_IRQHandler         ; 13: USART1 Interrupt
                DCD     USART2_IRQHandler         ; 14: USART2 Interrupt
                DCD     UART0_IRQHandler          ; 15: UART0 Interrupt
                DCD     LEUART0_IRQHandler        ; 16: LEUART0 Interrupt
                DCD     LETIMER0_IRQHandler       ; 17: LETIMER0 Interrupt
                DCD     PCNT0_IRQHandler          ; 18: PCNT0 Interrupt
                DCD     RTCC_IRQHandler           ; 19: RTCC Interrupt
                DCD     CMU_IRQHandler            ; 20: CMU Interrupt
                DCD     MSC_IRQHandler            ; 21: MSC Interrupt
                DCD     CRYPTO0_IRQHandler        ; 22: CRYPTO0 Interrupt
                DCD     CRYOTIMER_IRQHandler      ; 23: CRYOTIMER Interrupt
                DCD     USART3_IRQHandler         ; 24: USART3 Interrupt
                DCD     WTIMER0_IRQHandler        ; 25: WTIMER0 Interrupt
                DCD     WTIMER1_IRQHandler        ; 26: WTIMER1 Interrupt
                DCD     VDAC0_IRQHandler          ; 27: VDAC0 Interrupt
                DCD     CSEN_IRQHandler           ; 28: CSEN Interrupt
                DCD     LESENSE_IRQHandler        ; 29: LESENSE Interrupt
                DCD     LCD_IRQHandler            ; 30: LCD Interrupt
                DCD     CAN0_IRQHandler           ; 31: CAN0 Interrupt

__Vectors_End
__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                EXPORT  sl_app_properties         [WEAK]
sl_app_properties     ; Provide a dummy value for the sl_app_properties symbol.
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  EMU_IRQHandler            [WEAK]
                EXPORT  WDOG0_IRQHandler          [WEAK]
                EXPORT  LDMA_IRQHandler           [WEAK]
                EXPORT  GPIO_EVEN_IRQHandler      [WEAK]
                EXPORT  SMU_IRQHandler            [WEAK]
                EXPORT  TIMER0_IRQHandler         [WEAK]
                EXPORT  USART0_IRQHandler         [WEAK]
                EXPORT  ACMP0_IRQHandler          [WEAK]
                EXPORT  ADC0_IRQHandler           [WEAK]
                EXPORT  I2C0_IRQHandler           [WEAK]
                EXPORT  I2C1_IRQHandler           [WEAK]
                EXPORT  GPIO_ODD_IRQHandler       [WEAK]
                EXPORT  TIMER1_IRQHandler         [WEAK]
                EXPORT  USART1_IRQHandler         [WEAK]
                EXPORT  USART2_IRQHandler         [WEAK]
                EXPORT  UART0_IRQHandler          [WEAK]
                EXPORT  LEUART0_IRQHandler        [WEAK]
                EXPORT  LETIMER0_IRQHandler       [WEAK]
                EXPORT  PCNT0_IRQHandler          [WEAK]
                EXPORT  RTCC_IRQHandler           [WEAK]
                EXPORT  CMU_IRQHandler            [WEAK]
                EXPORT  MSC_IRQHandler            [WEAK]
                EXPORT  CRYPTO0_IRQHandler        [WEAK]
                EXPORT  CRYOTIMER_IRQHandler      [WEAK]
                EXPORT  USART3_IRQHandler         [WEAK]
                EXPORT  WTIMER0_IRQHandler        [WEAK]
                EXPORT  WTIMER1_IRQHandler        [WEAK]
                EXPORT  VDAC0_IRQHandler          [WEAK]
                EXPORT  CSEN_IRQHandler           [WEAK]
                EXPORT  LESENSE_IRQHandler        [WEAK]
                EXPORT  LCD_IRQHandler            [WEAK]
                EXPORT  CAN0_IRQHandler           [WEAK]


EMU_IRQHandler
WDOG0_IRQHandler
LDMA_IRQHandler
GPIO_EVEN_IRQHandler
SMU_IRQHandler
TIMER0_IRQHandler
USART0_IRQHandler
ACMP0_IRQHandler
ADC0_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
GPIO_ODD_IRQHandler
TIMER1_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
UART0_IRQHandler
LEUART0_IRQHandler
LETIMER0_IRQHandler
PCNT0_IRQHandler
RTCC_IRQHandler
CMU_IRQHandler
MSC_IRQHandler
CRYPTO0_IRQHandler
CRYOTIMER_IRQHandler
USART3_IRQHandler
WTIMER0_IRQHandler
WTIMER1_IRQHandler
VDAC0_IRQHandler
CSEN_IRQHandler
LESENSE_IRQHandler
LCD_IRQHandler
CAN0_IRQHandler
                B       .
                ENDP

                ALIGN

; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF

                END
