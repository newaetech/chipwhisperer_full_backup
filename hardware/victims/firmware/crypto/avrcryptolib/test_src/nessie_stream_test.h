/* nessie_stream_test.h */
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
#ifndef NESSIE_STREAM_TEST_H_
#define NESSIE_STREAM_TEST_H_

#include <stdint.h>

typedef void (*nessie_stream_genctx_fpt)(uint8_t* key, uint16_t keylength_b, void* ctx);
typedef uint8_t (*nessie_stream_genenc_fpt)(void* ctx);

typedef struct nessie_stream_ctx_st{
	uint16_t keysize_b;
	uint16_t ivsize_b;
	uint16_t outsize_b;
	uint16_t ctx_size_B;
	char* name; 
	nessie_stream_genctx_fpt cipher_genctx;
	nessie_stream_genenc_fpt cipher_enc;
} nessie_stream_ctx_t; 


extern nessie_stream_ctx_t nessie_stream_ctx;

void nessie_stream_run(void);

#endif /*NESSIE_STREAM_TEST_H_*/
