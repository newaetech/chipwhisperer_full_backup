/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2016-2017 NewAE Technology Inc.

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

#include "hal.h"
#include "simpleserial.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define IDLE 0
#define KEY 1
#define PLAIN 2

void rsa_init(void);
uint8_t real_dec(uint8_t * pt, uint8_t len);
uint8_t get_pt(uint8_t * pt, uint8_t len);

#if defined(__arm__) || defined(__riscv__) || defined(__riscv)

uint8_t sig_chunk_1(uint8_t *pt, uint8_t len);
uint8_t sig_chunk_2(uint8_t *pt, uint8_t len);
#endif

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    /* Load all the keys etc */
    rsa_init();

    simpleserial_init();
    simpleserial_addcmd('t', 0,  real_dec);
    #if (SS_VER != SS_VER_2_1) && (defined(__arm__) || defined(__riscv__) || defined(__riscv))
    simpleserial_addcmd('1', 0,  sig_chunk_1);
    simpleserial_addcmd('2', 0,  sig_chunk_2);
    #endif

    //Perform encryption -  must set key via plaintext
    simpleserial_addcmd('p', 16, get_pt);
    //simpleserial_addcmd('k', 16, set_key);
    while(1)
        simpleserial_get();
}

