/* nessie_bc_test.h */
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
#ifndef NESSIE_BC_TEST_H_
#define NESSIE_BC_TEST_H_

#include <stdint.h>

typedef void (*nessie_bc_gen_fpt)(const uint8_t* key, uint16_t keysize_b, void* ctx);
typedef void (*nessie_bc_free_fpt)(void* ctx);
typedef void (*nessie_bc_enc_fpt)(void* buffer, const void* ctx);
typedef void (*nessie_bc_dec_fpt)(void* buffer, const void* ctx);

typedef struct nessie_bc_ctx_st{
	uint16_t keysize_b;
	uint16_t blocksize_B;
	uint16_t ctx_size_B;
	char* name; 
	nessie_bc_gen_fpt  cipher_genctx;
	nessie_bc_free_fpt cipher_free;
	nessie_bc_enc_fpt  cipher_enc;
	nessie_bc_dec_fpt  cipher_dec;
} nessie_bc_ctx_t; 


extern nessie_bc_ctx_t nessie_bc_ctx;

void nessie_bc_run(void);
void nessie_bc_init(void);


#endif /*NESSIE_BC_TEST_H_*/
