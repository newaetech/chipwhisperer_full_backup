/* dsa.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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

#ifndef DSA_H_
#define DSA_H_

#include <stdint.h>
#include "hfal-basic.h"
#include "bigint.h"

typedef struct{
	bigint_t p;
	bigint_t q;
	bigint_t g;
} dsa_domainparameters_t;

typedef bigint_t dsa_pubkey_t;
typedef bigint_t dsa_privkey_t;
typedef struct{
	bigint_t r;
	bigint_t s;
} dsa_signature_t;

typedef struct{
	dsa_privkey_t priv;
	dsa_pubkey_t  pub;
	dsa_domainparameters_t domain;
} dsa_ctx_t;

typedef uint8_t(*rand_func_ptr_t)(void);

#define DSA_SIGNATURE_OK 1
#define DSA_SIGNATURE_FAIL 0

uint8_t dsa_sign_bigint(dsa_signature_t* s, const bigint_t* m,
		                const dsa_ctx_t* ctx, const bigint_t* k);
uint8_t dsa_sign_message(dsa_signature_t* s, const void* m, uint16_t m_len_b,
		                const hfdesc_t* hash_desc, const dsa_ctx_t* ctx,
		                const rand_func_ptr_t rand_in);
uint8_t dsa_verify_bigint(const dsa_signature_t* s, const bigint_t* m,
		                  const dsa_ctx_t* ctx);
uint8_t dsa_verify_message(const dsa_signature_t* s, const void* m, uint16_t m_len_b,
						  const hfdesc_t* hash_desc, const dsa_ctx_t* ctx);

#endif /* DSA_H_ */
