/* rsa_basic.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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
#include <stdlib.h>
#include <string.h>
#include "bigint.h"
#include "bigint_io.h"
#include "rsa_basic.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

void rsa_enc(bigint_t* data, const rsa_publickey_t* key){
/*
	cli_putstr_P(PSTR("\r\n -->rsa_enc()\r\n m = "));
	bigint_print_hex(data);
	cli_putstr_P(PSTR("\r\n e = "));
	bigint_print_hex(key->exponent);
	cli_putstr_P(PSTR("\r\n n = "));
	bigint_print_hex(key->modulus);
*/
	bigint_expmod_u(data, data, &key->exponent, &key->modulus);
}

/*
(p,q,dp,dq,qinv)
m1 = c**dp % p
m2 = c**dq % q
h = (m1 - m2) * qinv % p
m = m2 + q * h
*/

uint8_t rsa_dec_crt_mono(bigint_t* data, const rsa_privatekey_t* key){
	bigint_t m1, m2;
	m1.wordv = malloc((key->components[0].length_B /* + 1 */) * sizeof(bigint_word_t));
	m2.wordv = malloc((key->components[1].length_B /* + 1 */) * sizeof(bigint_word_t));
	if(!m1.wordv || !m2.wordv){
#if DEBUG
		cli_putstr_P(PSTR("\r\nERROR: OOM!"));
#endif
		free(m1.wordv);
		free(m2.wordv);
		return 1;
	}
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: expmod m1 ..."));
	cli_putstr_P(PSTR("\r\nexpmod("));
	bigint_print_hex(data);
	cli_putc(',');
	bigint_print_hex(&(key->components[2]));
	cli_putc(',');
	bigint_print_hex(&(key->components[0]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_expmod_u(&m1, data, &(key->components[2]), &(key->components[0]));
#if DEBUG
	bigint_print_hex(&m1);
	cli_putstr_P(PSTR("expmod m2 ..."));
	cli_putstr_P(PSTR("\r\nexpmod("));
	bigint_print_hex(data);
	cli_putc(',');
	bigint_print_hex(&(key->components[3]));
	cli_putc(',');
	bigint_print_hex(&(key->components[1]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_expmod_u(&m2, data, &(key->components[3]), &(key->components[1]));
#if DEBUG
	bigint_print_hex(&m2);
	cli_putstr_P(PSTR("\r\nDBG: sub ..."));
	cli_putstr_P(PSTR("\r\nsub("));
	bigint_print_hex(&m1);
	cli_putc(',');
	bigint_print_hex(&m2);
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_sub_s(&m1, &m1, &m2);
#if DEBUG
	bigint_print_hex(&m1);
#endif
	while(BIGINT_NEG_MASK & m1.info){
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: adding "));
	bigint_print_hex(key->components[0]);
	cli_putstr_P(PSTR("\r\nDBG: to "));
	bigint_print_hex(&m1);
#endif
		bigint_add_s(&m1, &m1, &(key->components[0]));
	}
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: reduce-mul ..."));
	cli_putstr_P(PSTR("\r\nreduce("));
	bigint_print_hex(&m1);
	cli_putc(',');
	bigint_print_hex(&(key->components[0]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_reduce(&m1, &(key->components[0]));
#if DEBUG
	bigint_print_hex(&m1);
	cli_putstr_P(PSTR("\r\nmul("));
	bigint_print_hex(&m1);
	cli_putc(',');
	bigint_print_hex(&(key->components[4]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_mul_u(data, &m1, &(key->components[4]));
#if DEBUG
	bigint_print_hex(data);
	cli_putstr_P(PSTR("\r\nreduce("));
	bigint_print_hex(data);
	cli_putc(',');
	bigint_print_hex(&(key->components[0]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_reduce(data, &(key->components[0]));
#if DEBUG
	bigint_print_hex(data);
	cli_putstr_P(PSTR("\r\nmul("));
	bigint_print_hex(data);
	cli_putc(',');
	bigint_print_hex(&(key->components[1]));
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_mul_u(data, data, &(key->components[1]));
#if DEBUG
	bigint_print_hex(data);
	cli_putstr_P(PSTR("\r\nadd("));
	bigint_print_hex(data);
	cli_putc(',');
	bigint_print_hex(&m2);
	cli_putstr_P(PSTR(") = "));
#endif
	bigint_add_u(data, data, &m2);
#if DEBUG
	bigint_print_hex(data);
#endif
	free(m2.wordv);
	free(m1.wordv);
	return 0;
}

uint8_t rsa_dec(bigint_t* data, const rsa_privatekey_t* key){
	if(key->n == 1){
		bigint_expmod_u(data, data, &(key->components[0]), &key->modulus);
		return 0;
	}
	if(key->n == 5){
		if (rsa_dec_crt_mono(data, key)){
			return 3;
		}
		return 0;
	}
	if(key->n<8 || (key->n-5)%3 != 0){
		return 1;
	}
	//rsa_dec_crt_multi(data, key, (key->n-5)/3);
	return 2;
}

void rsa_os2ip(bigint_t* dest, const void* data, uint32_t length_B){
#if BIGINT_WORD_SIZE == 8
	if(data){
		memcpy(dest->wordv, data, length_B);
	}
	dest->length_B = length_B;
#else
	uint8_t off;
	off = (sizeof(bigint_word_t) - length_B % sizeof(bigint_word_t)) % sizeof(bigint_word_t);
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: off = 0x"));
	cli_hexdump_byte(off);
#endif
	if(!data){
		if(off){
			dest->wordv = realloc(dest->wordv, length_B + sizeof(bigint_word_t) - off);
			memmove((uint8_t*)dest->wordv+off, dest->wordv, length_B);
			memset(dest->wordv, 0, off);
		}
	}else{
		memcpy((uint8_t*)dest->wordv + off, data, length_B);
		if(off){
			memset(dest->wordv, 0, off);
		}
	}
	dest->length_B = (length_B + off) / sizeof(bigint_word_t);
#if DEBUG
	cli_putstr_P(PSTR("\r\nDBG: dest->length_B = 0x"));
	cli_hexdump_rev(&(dest->length_B), 2);
#endif
#endif
	dest->info = 0;
	bigint_changeendianess(dest);
	bigint_adjust(dest);
}

void rsa_i2osp(void* dest, bigint_t* src, uint16_t* out_length_B){
#if BIGINT_WORD_SIZE == 8
	if(dest){
		uint8_t *e = src->wordv + src->length_B;
		uint16_t i;
		for(i=src->length_B; i>0; --i){
			*((uint8_t*)dest) = *--e;
			dest = (uint8_t*)dest + 1;
		}
	}else{
		bigint_changeendianess(src);
	}

	*out_length_B = src->length_B;
#else
	*out_length_B = bigint_get_first_set_bit(src) / 8 + 1;
	if(dest){
		uint16_t i;
		for(i=*out_length_B; i>0; --i){
			*((uint8_t*)dest) = ((uint8_t*)src->wordv)[i-1];
			dest = (uint8_t*)dest + 1;
		}
	}else{
		uint8_t off;
		bigint_changeendianess(src);
		bigint_adjust(src);

		off = bigint_get_last_set_bit(src)/8;
		if(off){
			memmove(src->wordv, (uint8_t*)src->wordv+off, *out_length_B);
		}
	}
#endif
}

