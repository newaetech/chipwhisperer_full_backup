/* nessie_hash_test.h */
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
#ifndef NESSIE_HASH_TEST_H_
#define NESSIE_HASH_TEST_H_

#include <stdint.h>

typedef void (*nessie_hash_init_fpt)(void* ctx);
typedef void (*nessie_hash_next_fpt)(void* ctx, const void* buffer);
typedef void (*nessie_hash_last_fpt)(void* ctx, const void* buffer, uint16_t size_b);
typedef void (*nessie_hash_conv_fpt)(void* buffer, void* ctx);


typedef struct nessie_hash_ctx_st{
	uint16_t hashsize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_hash_init_fpt hash_init;
	nessie_hash_next_fpt hash_next;
	nessie_hash_last_fpt hash_last;
	nessie_hash_conv_fpt hash_conv;
} nessie_hash_ctx_t; 


extern nessie_hash_ctx_t nessie_hash_ctx;
extern uint8_t nessie_hash_quick;
void nessie_hash_run(void);

#endif /*NESSIE_HASH_TEST_H_*/
