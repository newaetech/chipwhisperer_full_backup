/* dsa_verify.c */
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

#include <stdint.h>
#include "bigint.h"
#include "dsa.h"
#include "hfal-basic.h"

uint8_t dsa_verify_bigint(const dsa_signature_t* s, const bigint_t* m,
		                  const dsa_ctx_t* ctx){
	if(s->r.length_B==0 || s->s.length_B==0){
		return DSA_SIGNATURE_FAIL;
	}
	if(bigint_cmp_u(&(s->r), &(ctx->domain.q))>=0 || bigint_cmp_u(&(s->s), &(ctx->domain.q))>=0){
		return DSA_SIGNATURE_FAIL;
	}
	bigint_t w, u1, u2, v1, v2;
	uint8_t w_b[ctx->domain.q.length_B], u1_b[ctx->domain.q.length_B*2], u2_b[ctx->domain.q.length_B*2];
	uint8_t v1_b[ctx->domain.p.length_B*2], v2_b[ctx->domain.p.length_B];
	w.wordv = w_b;
	u1.wordv = u1_b;
	u2.wordv = u2_b;
	v1.wordv = v1_b;
	v2.wordv = v2_b;
	bigint_inverse(&w, &(s->s), &(ctx->domain.q));
	bigint_mul_u(&u1, &w, m);
	bigint_reduce(&u1, &(ctx->domain.q));
	bigint_mul_u(&u2, &w, &(s->r));
	bigint_reduce(&u2, &(ctx->domain.q));
	bigint_expmod_u(&v1, &(ctx->domain.g), &u1, &(ctx->domain.p));
	bigint_expmod_u(&v2, &(ctx->pub), &u2, &(ctx->domain.p));
	bigint_mul_u(&v1, &v1, &v2);
	bigint_reduce(&v1, &(ctx->domain.p));
	bigint_reduce(&v1, &(ctx->domain.q));
	if(bigint_cmp_u(&v1, &(s->r))==0){
		return DSA_SIGNATURE_OK;
	}
	return DSA_SIGNATURE_FAIL;
}

uint8_t dsa_verify_message(const dsa_signature_t* s, const void* m, uint16_t m_len_b,
						  const hfdesc_t* hash_desc, const dsa_ctx_t* ctx){
	bigint_t z;
	uint8_t n_B = ctx->domain.q.length_B;
	uint8_t hash_value[(hfal_hash_getHashsize(hash_desc)+7)/8];
	hfal_hash_mem(hash_desc, hash_value, m, m_len_b);
	z.wordv=hash_value;
	z.length_B=n_B;
	bigint_changeendianess(&z);
	bigint_adjust(&z);
	return dsa_verify_bigint(s, &z, ctx);
}







