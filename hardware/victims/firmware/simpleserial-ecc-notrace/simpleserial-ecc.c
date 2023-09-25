/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2023 NewAE Technology Inc.

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

#include "uECC.c"
#include "uECC_vli.h"
#include "types.h"

#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>



// Use globals for pmul input (P) and output (Q) points because
// they're too big to transmit all in one simpleserial transfer:
uECC_word_t P[uECC_MAX_WORDS * 2];
uECC_word_t Q[uECC_MAX_WORDS * 2];

void print(const char *ptr)
{
   while (*ptr != (char)0) {
      putch(*ptr);
      ptr++;
   }
}


uint8_t set_px(uint8_t* x, uint8_t len)
{
   int i, j;
   for (i = 0; i < 8; i++) {
      P[7-i] = 0;
      for (j = 0; j < 4; j++) {
         P[7-i] |= x[i*4+j] << ((3-j)*8);
      }
   }
   return 0x00;
}


uint8_t set_py(uint8_t* y, uint8_t len)
{
   // TODO: this conversion is used in multiple places, move it to a function:
   int i, j;
   for (i = 0; i < 8; i++) {
      P[15-i] = 0;
      for (j = 0; j < 4; j++) {
         P[15-i] |= y[i*4+j] << ((3-j)*8);
      }
   }
   return 0x00;
}


uint8_t get_qx(uint8_t* x, uint8_t len)
{
    int i, j;
    for (i = 0; i < 8; i++) {
       for (j = 0; j < 4; j++) {
          x[i*4+j] = (int)((Q[7-i] >> (3-j)*8) & 255);
       }
    }
    simpleserial_put('r', 32, x);
    return 0x00;
}


uint8_t get_qy(uint8_t* y, uint8_t len)
{
    int i, j;
    for (i = 0; i < 8; i++) {
       for (j = 0; j < 4; j++) {
          y[i*4+j] = (int)((Q[15-i] >> (3-j)*8) & 255);
       }
    }
    simpleserial_put('r', 32, y);
    return 0x00;
}



uint8_t run_pmul(uint8_t* k, uint8_t len)
{
    const struct uECC_Curve_t * curve;
    uECC_word_t kwords[uECC_MAX_WORDS];
    curve = uECC_secp256r1();

    int i, j;
    for (i = 0; i < 8; i++) {
       kwords[7-i] = 0;
       for (j = 0; j < 4; j++) {
          kwords[7-i] |= k[i*4+j] << ((3-j)*8);
       }
    }

    //trigger_high();
    uECC_point_mult(Q, P, kwords, curve);
    trigger_low();
    return 0x00;
}


uint8_t run_pmul_fixed(uint8_t* k, uint8_t len)
{
    const struct uECC_Curve_t * curve;
    uECC_word_t kwords[uECC_MAX_WORDS];
    curve = uECC_secp256r1();

    int i, j;
    for (i = 0; i < 8; i++) {
       kwords[7-i] = 0;
       for (j = 0; j < 4; j++) {
          kwords[7-i] |= k[i*4+j] << ((3-j)*8);
       }
    }

    //trigger_high();
    uECC_point_mult(Q, curve->G, kwords, curve);
    trigger_low();
    return 0x00;
}




uint8_t reset(uint8_t* x, uint8_t len)
{
    // Reset key here if needed
	return 0x00;
}


uint8_t info(uint8_t* x, uint8_t len)
{
        print("ChipWhisperer simpleserial-ecc-notrace, compiled ");
        print(__DATE__);
        print(", ");
        print(__TIME__);
#ifdef FW_TRIGGER
        print(" with FW trigger");
#else
        print(" without FW trigger");
#endif
        print("\n");
	return 0x00;
}


int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    simpleserial_init();
    simpleserial_addcmd('k', 32, run_pmul);
    simpleserial_addcmd('f', 32, run_pmul_fixed);
    simpleserial_addcmd('a', 32, set_px);
    simpleserial_addcmd('b', 32, set_py);
    simpleserial_addcmd('p', 32, get_qx);
    simpleserial_addcmd('q', 32, get_qy);
    simpleserial_addcmd('x',  0, reset);
    simpleserial_addcmd('i',  0, info);

    while(1)
        simpleserial_get();
}
