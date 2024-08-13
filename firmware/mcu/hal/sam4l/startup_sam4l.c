/**
 * \file
 *
 * \brief gcc starttup file for SAM4L
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "sam4l.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M4 core handlers */
void NMI_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemManage_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMon_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void HFLASHC_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_0_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_1_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_2_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_3_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_4_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_5_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_6_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_7_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_8_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_9_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_10_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_11_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_12_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_13_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_14_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDCA_15_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void CRCCU_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_USBC
void USBC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
void PEVC_TR_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PEVC_OV_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_AESA
void AESA_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
void PM_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SCIF_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FREQM_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_0_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_1_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_2_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_3_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_4_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_5_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_6_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_7_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_8_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_9_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_10_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void GPIO_11_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BPM_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BSCIF_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AST_ALARM_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AST_PER_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AST_OVF_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AST_READY_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AST_CLKREADY_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_3_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_4_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_5_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_6_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_7_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_8_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void IISC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SPI_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC00_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC01_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC02_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC10_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC11_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC12_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIM0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIS0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIM1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIS1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART0_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART1_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART2_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USART3_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADCIFE_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DACC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ACIFC_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ABDACB_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TRNG_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PARC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void CATB_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIM2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TWIM3_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_LCDCA
void LCDCA_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        (void*) (&_estack),

        (void*) Reset_Handler,
        (void*) NMI_Handler,
        (void*) HardFault_Handler,
        (void*) MemManage_Handler,
        (void*) BusFault_Handler,
        (void*) UsageFault_Handler,
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) (0UL), /* Reserved */
        (void*) SVC_Handler,
        (void*) DebugMon_Handler,
        (void*) (0UL), /* Reserved */
        (void*) PendSV_Handler,
        (void*) SysTick_Handler,

        /* Configurable interrupts */
        (void*) HFLASHC_Handler,        /*  0 Flash Controller */
        (void*) PDCA_0_Handler,         /*  1 Peripheral DMA Controller */
        (void*) PDCA_1_Handler,         /*  2 Peripheral DMA Controller */
        (void*) PDCA_2_Handler,         /*  3 Peripheral DMA Controller */
        (void*) PDCA_3_Handler,         /*  4 Peripheral DMA Controller */
        (void*) PDCA_4_Handler,         /*  5 Peripheral DMA Controller */
        (void*) PDCA_5_Handler,         /*  6 Peripheral DMA Controller */
        (void*) PDCA_6_Handler,         /*  7 Peripheral DMA Controller */
        (void*) PDCA_7_Handler,         /*  8 Peripheral DMA Controller */
        (void*) PDCA_8_Handler,         /*  9 Peripheral DMA Controller */
        (void*) PDCA_9_Handler,         /* 10 Peripheral DMA Controller */
        (void*) PDCA_10_Handler,        /* 11 Peripheral DMA Controller */
        (void*) PDCA_11_Handler,        /* 12 Peripheral DMA Controller */
        (void*) PDCA_12_Handler,        /* 13 Peripheral DMA Controller */
        (void*) PDCA_13_Handler,        /* 14 Peripheral DMA Controller */
        (void*) PDCA_14_Handler,        /* 15 Peripheral DMA Controller */
        (void*) PDCA_15_Handler,        /* 16 Peripheral DMA Controller */
        (void*) CRCCU_Handler,          /* 17 CRC Calculation Unit */
#ifdef ID_USBC
        (void*) USBC_Handler,           /* 18 USB 2.0 Interface */
#else
        (void*) (0UL), /* Reserved */
#endif
        (void*) PEVC_TR_Handler,        /* 19 Peripheral Event Controller */
        (void*) PEVC_OV_Handler,        /* 20 Peripheral Event Controller */
#ifdef ID_AESA
        (void*) AESA_Handler,           /* 21 Advanced Encryption Standard */
#else
        (void*) (0UL), /* Reserved */
#endif
        (void*) PM_Handler,             /* 22 Power Manager */
        (void*) SCIF_Handler,           /* 23 System Control Interface */
        (void*) FREQM_Handler,          /* 24 Frequency Meter */
        (void*) GPIO_0_Handler,         /* 25 General-Purpose Input/Output Controller */
        (void*) GPIO_1_Handler,         /* 26 General-Purpose Input/Output Controller */
        (void*) GPIO_2_Handler,         /* 27 General-Purpose Input/Output Controller */
        (void*) GPIO_3_Handler,         /* 28 General-Purpose Input/Output Controller */
        (void*) GPIO_4_Handler,         /* 29 General-Purpose Input/Output Controller */
        (void*) GPIO_5_Handler,         /* 30 General-Purpose Input/Output Controller */
        (void*) GPIO_6_Handler,         /* 31 General-Purpose Input/Output Controller */
        (void*) GPIO_7_Handler,         /* 32 General-Purpose Input/Output Controller */
        (void*) GPIO_8_Handler,         /* 33 General-Purpose Input/Output Controller */
        (void*) GPIO_9_Handler,         /* 34 General-Purpose Input/Output Controller */
        (void*) GPIO_10_Handler,        /* 35 General-Purpose Input/Output Controller */
        (void*) GPIO_11_Handler,        /* 36 General-Purpose Input/Output Controller */
        (void*) BPM_Handler,            /* 37 Backup Power Manager */
        (void*) BSCIF_Handler,          /* 38 Backup System Control Interface */
        (void*) AST_ALARM_Handler,      /* 39 Asynchronous Timer */
        (void*) AST_PER_Handler,        /* 40 Asynchronous Timer */
        (void*) AST_OVF_Handler,        /* 41 Asynchronous Timer */
        (void*) AST_READY_Handler,      /* 42 Asynchronous Timer */
        (void*) AST_CLKREADY_Handler,   /* 43 Asynchronous Timer */
        (void*) WDT_Handler,            /* 44 Watchdog Timer */
        (void*) EIC_1_Handler,          /* 45 External Interrupt Controller */
        (void*) EIC_2_Handler,          /* 46 External Interrupt Controller */
        (void*) EIC_3_Handler,          /* 47 External Interrupt Controller */
        (void*) EIC_4_Handler,          /* 48 External Interrupt Controller */
        (void*) EIC_5_Handler,          /* 49 External Interrupt Controller */
        (void*) EIC_6_Handler,          /* 50 External Interrupt Controller */
        (void*) EIC_7_Handler,          /* 51 External Interrupt Controller */
        (void*) EIC_8_Handler,          /* 52 External Interrupt Controller */
        (void*) IISC_Handler,           /* 53 Inter-IC Sound (I2S) Controller */
        (void*) SPI_Handler,            /* 54 Serial Peripheral Interface */
        (void*) TC00_Handler,           /* 55 Timer/Counter 0 */
        (void*) TC01_Handler,           /* 56 Timer/Counter 0 */
        (void*) TC02_Handler,           /* 57 Timer/Counter 0 */
        (void*) TC10_Handler,           /* 58 Timer/Counter 1 */
        (void*) TC11_Handler,           /* 59 Timer/Counter 1 */
        (void*) TC12_Handler,           /* 60 Timer/Counter 1 */
        (void*) TWIM0_Handler,          /* 61 Two-wire Master Interface 0 */
        (void*) TWIS0_Handler,          /* 62 Two-wire Slave Interface 0 */
        (void*) TWIM1_Handler,          /* 63 Two-wire Master Interface 1 */
        (void*) TWIS1_Handler,          /* 64 Two-wire Slave Interface 1 */
        (void*) USART0_Handler,         /* 65 Universal Synchronous Asynchronous Receiver Transmitter 0 */
        (void*) USART1_Handler,         /* 66 Universal Synchronous Asynchronous Receiver Transmitter 1 */
        (void*) USART2_Handler,         /* 67 Universal Synchronous Asynchronous Receiver Transmitter 2 */
        (void*) USART3_Handler,         /* 68 Universal Synchronous Asynchronous Receiver Transmitter 3 */
        (void*) ADCIFE_Handler,         /* 69 ADC controller interface */
        (void*) DACC_Handler,           /* 70 DAC Controller */
        (void*) ACIFC_Handler,          /* 71 Analog Comparator Interface */
        (void*) ABDACB_Handler,         /* 72 Audio Bitstream DAC */
        (void*) TRNG_Handler,           /* 73 True Random Number Generator */
        (void*) PARC_Handler,           /* 74 Parallel Capture */
        (void*) CATB_Handler,           /* 75 Capacitive Touch Module B */
        (void*) PTC_Handler,            /* 76  */
        (void*) TWIM2_Handler,          /* 77 Two-wire Master Interface 2 */
        (void*) TWIM3_Handler,          /* 78 Two-wire Master Interface 3 */
#ifdef ID_LCDCA
        (void*) LCDCA_Handler           /* 79 LCD Controller */
#else
        (void*) (0UL)  /* Reserved */
#endif
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Initialize the C library */
        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
