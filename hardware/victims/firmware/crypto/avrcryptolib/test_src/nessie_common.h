/* nessie_common.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * common function for nessie-tests
 * 
 * */

#ifndef NESSIE_COMMON_H_
#define NESSIE_COMMON_H_

#define NESSIE_ALIVE_CHAR 0x06
#define NESSIE_ALIVE
#define NESSIE_USE_CLI

#include <stdint.h>

#ifdef NESSIE_ALIVE
#define NESSIE_SEND_ALIVE nessie_send_alive()
void nessie_send_alive(void);
#define NESSIE_SEND_ALIVE_A(i) nessie_send_alive_a(i)
void nessie_send_alive_a(uint16_t i);
#else
#define NESSIE_SEND_ALIVE 
#define NESSIE_SEND_ALIVE_A(i)  
#endif


#ifdef NESSIE_USE_CLI
#include "cli.h"
#define NESSIE_PUTC cli_putc
#define NESSIE_PUTSTR cli_putstr
#define NESSIE_PUTSTR_P cli_putstr_P
#else
# error "direct uart output removed for nessie"
#endif

void nessie_print_block(uint8_t* block, uint16_t blocksize_bit);
void nessie_print_item(char* name, uint8_t* buffer, uint16_t size_B);
void nessie_print_set_vector(uint8_t set, uint16_t vector);
void nessie_print_setheader(uint8_t set);
void nessie_print_header(char* name,
                         uint16_t keysize_b, 
                         uint16_t blocksize_b,
                         uint16_t hashsize_b, 
                         uint16_t macsize_b,
                         uint16_t ivsize_b );
void nessie_print_footer(void);

#endif /*NESSIE_COMMON_H_*/
