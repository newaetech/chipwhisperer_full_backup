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

//PLATFORM Define Types
#define CW301_AVR 1
#define CW301_XMEGA 2
#define CW303 3
#define CW304 4
#define CW308_MEGARF  8

//HAL_TYPE Define Types
#define HAL_avr 1
#define HAL_xmega 2

#if HAL_TYPE == HAL_avr
    #include <avr/io.h>
    #include <util/delay.h>
    #include "avr/avr_hal.h"
#elif HAL_TYPE == HAL_xmega
    #include <avr/io.h>
    #include <util/delay.h>
    #include "xmega/xmega_hal.h"
    #include "xmega/avr_compiler.h"
#else
    #error "Unsupported HAL Type"
#endif

/*
#if PLATFORM == CW301_XMEGA
    #define HW_CRYPTO 1
#else
    #define HW_CRYPTO 0
#endif
*/

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
                                //Wait for done \
                                while ((AES_STATUS & (1<<AES_DONE)) == 0){ \
                                    ; \
                                } \
                                  \
                                for (uint8_t i = 0; i < 16; i++){ \
                                    *(pt+i) = AES_STATE; \
                                }
    
#endif


#ifndef led_error
#define led_error(a)
#endif

#ifndef led_ok
#define led_ok(a)
#endif

#endif //HAL_H_