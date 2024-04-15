/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

void platform_init(void);

//PLATFORM Define Types
#define CW301_AVR      1
#define CW301_XMEGA    2
#define CW303          3
#define CW304          4
#define CW308_MEGARF   8
#define CW308_PIC24FJ  10
#define CW308_SAM4L    11
#define CW308_SI4010   12
#define CW308_MPC5748G 13
#define CW308_STM32F0  14
#define CW308_STM32F1  15
#define CW308_STM32F2  16
#define CW308_STM32F3  17
#define CW308_STM32F4  18
#define CW308_CC2538   19
#define CW308_K24F     20
#define CW308_NRF52840 21
#define CW308_AURIX    22
#define CW308_SAML11   23
#define CW308_EFM32TG11B 24
#define CW308_K82F     25
#define CW308_LPC55S6X 26
#define CW308_PSOC62   27
#define CW308_IMXRT1062 28
#define CW308_FE310    29
#define CW308_EFR32MG21A  30
#define CW308_EFM32GG11  31
#define CW308_STM32L5  32
#define CW308_STM32L4  33
#define CW308_RX65N  34
#define CW308_MPC5676R 35
#define CW308_NEORV32  36
#define CW305_IBEX  37

//HAL_TYPE Define Types
#define HAL_avr      1
#define HAL_xmega    2
#define HAL_pic24f   3
#define HAL_sam4l    4
#define HAL_stm32f0  5
#define HAL_stm32f1  6
#define HAL_stm32f2  7
#define HAL_stm32f3  8
#define HAL_stm32f4  9
#define HAL_cc2538   10
#define HAL_k24f     11
#define HAL_nrf52840 12
#define HAL_stm32f0_nano 13
#define HAL_aurix    14
#define HAL_saml11   15
#define HAL_efm32tg11b 16
#define HAL_k82f     17
#define HAL_lpc55s6x 18
#define HAL_psoc62   19
#define HAL_imxrt1062 20
#define HAL_fe310    21
#define HAL_efr32mg21a 22
#define HAL_efm32gg11 23
#define HAL_stm32l5 24
#define HAL_stm32l4 25
#define HAL_rx65n 26
#define HAL_mpc5676r 27
#define HAL_neorv32  28
#define HAL_sam4s  29
#define HAL_ibex  30

#if HAL_TYPE == HAL_avr
    #include <avr/io.h>
    #include <util/delay.h>
    #include "avr/avr_hal.h"
#elif HAL_TYPE == HAL_xmega
    #include <avr/io.h>
    #include <util/delay.h>
    #include "xmega/xmega_hal.h"
    #include "xmega/avr_compiler.h"
#elif HAL_TYPE == HAL_pic24f
    #include <xc.h>
    #include "pic24f/pic24f_hal.h"
    #include "pic24f/uart.h"
#elif HAL_TYPE == HAL_sam4l
    #include "sam4l/sam4l_hal.h"
#elif HAL_TYPE == HAL_stm32f0
	#include "stm32f0/stm32f0_hal.h"
#elif HAL_TYPE == HAL_stm32f1
	#include "stm32f1/stm32f1_hal.h"
#elif HAL_TYPE == HAL_stm32f2
	#include "stm32f2/stm32f2_hal.h"
#elif HAL_TYPE == HAL_stm32f3
	#include "stm32f3/stm32f3_hal.h"
	#ifdef SECCAN
		#include "stm32f3/stm32f3_hal_seccan.h"
	#endif
#elif HAL_TYPE == HAL_stm32f4
	#include "stm32f4/stm32f4_hal.h"
#elif HAL_TYPE == HAL_cc2538
	#include "cc2538/cc2538_hal.h"
#elif HAL_TYPE == HAL_k24f
    #include "k24f/k24f_hal.h"
#elif HAL_TYPE == HAL_k82f
#include "k82f/k82f_hal.h"
#elif HAL_TYPE == HAL_nrf52840
    #include "nrf52840/nrf52840_hal.h"   
#elif HAL_TYPE == HAL_stm32f0_nano
    #include "stm32f0/stm32f0_hal.h" 
    void led_error(unsigned int status);
    void led_ok(unsigned int status);
#elif HAL_TYPE == HAL_aurix
    #include "aurix/aurix_hal.h"
#elif HAL_TYPE == HAL_saml11
     #include "saml11/saml11_hal.h"
#elif HAL_TYPE == HAL_efm32tg11b
     #include "efm32tg11b/efm32tg11b_hal.h"
#elif HAL_TYPE == HAL_lpc55s6x
     #include "lpc55s6x/lpc55s6x_hal.h"
#elif HAL_TYPE == HAL_psoc62
    #include "psoc62/psoc62_hal.h"
#elif HAL_TYPE == HAL_imxrt1062
    #include "imxrt1062/imxrt1062_hal.h"
#elif HAL_TYPE == HAL_fe310
    #include "fe310/fe310_hal.h"
#elif HAL_TYPE == HAL_efr32mg21a
    #include "efr32mg21a/efr32mg21a_hal.h"
#elif HAL_TYPE == HAL_efm32gg11
    #include "efm32gg11/efm32gg11_hal.h"
#elif HAL_TYPE == HAL_stm32l5
    #include "stm32l5/stm32l5_hal.h"
#elif HAL_TYPE == HAL_stm32l4
    #include "stm32l4/stm32l4_hal.h"
#elif HAL_TYPE == HAL_rx65n
    #include "rx65n/rx65n_hal.h"
#elif HAL_TYPE == HAL_mpc5676r
    #include "mpc5676r/MPC5676R_hal.h"
#elif HAL_TYPE == HAL_neorv32
    #include "neorv32/neorv32_hal.h"
#elif HAL_TYPE == HAL_sam4s
    #include "sam4s/sam4s_hal.h"
#elif HAL_TYPE == HAL_ibex
    #include "ibex/ibex_hal.h"
#else
    #error "Unsupported HAL Type"
#endif

#if PLATFORM == CW308_MEGARF
    #undef trigger_setup
    #undef trigger_high
    #undef trigger_low
    #define trigger_setup() DDRD |= 0x02
    #define trigger_high()  PORTD |= 0x02
    #define trigger_low()   PORTD &= ~(0x02)
    
    #define HW_AES128_Init();      AES_CTRL = 0x00;

    #define HW_AES128_LoadKey(key);  for (uint8_t i = 0; i < 16; i++){ \
                                     AES_KEY = *(key+i); \
                                  }

    #define HW_AES128_Enc(pt);  for (uint8_t i = 0; i < 16; i++){ \
                                    AES_STATE = *(pt+i); \
                                } \
                                  \
                                AES_CTRL |= 1<<AES_REQUEST; \
                                  \
                                /*Wait for done  */ \
                                while ((AES_STATUS & (1<<AES_DONE)) == 0){ \
                                    ; \
                                } \
                                  \
                                for (uint8_t i = 0; i < 16; i++){ \
                                    *(pt+i) = AES_STATE; \
                                }
    
#endif

__attribute__((weak)) void led_ok(unsigned int status)
{
}

__attribute__((weak)) void led_error(unsigned int status)
{
}

#endif //HAL_H_
