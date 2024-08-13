/* rsa_pkcs1v15.c */
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
#include "rsa_basic.h"
#include "rsaes_pkcs1v15.h"

#define DEBUG 0

#if DEBUG
#include "bigint_io.h"
#include "cli.h"
#endif

#include "random_dummy.h"

uint16_t rsa_pkcs1v15_compute_padlength_B(bigint_t* modulus, uint16_t msg_length_B){
	return bigint_get_first_set_bit(modulus) / 8 + 1 - msg_length_B - 3;
}

uint8_t rsa_encrypt_pkcs1v15(void* dest, uint16_t* out_length, const void* src,
	uint16_t length_B, rsa_publickey_t* key, const void* pad){
	int16_t pad_length;
	bigint_t x;
	pad_length = rsa_pkcs1v15_compute_padlength_B(&key->modulus, length_B);
	if(pad_length<8){
#if DEBUG
		cli_putstr_P(PSTR("\r\nERROR: pad_length<8; pad_length: "));
		cli_hexdump_rev(&pad_length, 2);
#endif
		return 2; /* message to long */
	}
	if(!pad){
#if DEBUG
		cli_putstr_P(PSTR("\r\nauto-generating pad ..."));
#endif
		uint16_t i;
		uint8_t c;
		for(i=0; i<pad_length; ++i){
			do{
				c = prng_get_byte();
			}while(c==0);
			((uint8_t*)dest)[i+2] = c;
		}
	}else{
#if DEBUG
		cli_putstr_P(PSTR("\r\nsupplied pad: "));
		cli_hexdump_block(pad, pad_length, 4, 16);
#endif
		memcpy((uint8_t*)dest + 2, pad, pad_length);
	}
	((uint8_t*)dest)[0] = 0x00;
	((uint8_t*)dest)[1] = 0x02;
	((uint8_t*)dest)[2+pad_length] = 0x00;
	memcpy((uint8_t*)dest+3+pad_length, src, length_B);
	x.wordv = dest;
	x.length_B = (length_B+pad_length+3+sizeof(bigint_word_t)-1)/sizeof(bigint_word_t);
#if DEBUG
	cli_putstr_P(PSTR("\r\nx-data: "));
	cli_hexdump_block(x.wordv, x.length_B * sizeof(bigint_word_t), 4, 16);
#endif
	bigint_adjust(&x);
	rsa_os2ip(&x, NULL, length_B+pad_length+3);
	rsa_enc(&x, key);
	rsa_i2osp(NULL, &x, out_length);
	return 0;
}

uint8_t rsa_decrypt_pkcs1v15(void* dest, uint16_t* out_length, const void* src,
	uint16_t length_B, rsa_privatekey_t* key, void* pad){
	bigint_t x;
	uint16_t m_length, pad_length=0, idx=0;
	x.wordv = dest;
	rsa_os2ip(&x, src, length_B);
#if DEBUG
	cli_putstr_P(PSTR("\r\ncalling rsa_dec() with bigint:"));
	bigint_print_hex(&x);
#endif
	rsa_dec(&x, key);
#if DEBUG
	cli_putstr_P(PSTR("\r\nfinished rsa_dec() bigint:"));
	bigint_print_hex(&x);
#endif
	rsa_i2osp(NULL, &x, &m_length);
#if DEBUG
	cli_putstr_P(PSTR("\r\ndecoded block:"));
	cli_hexdump_block(x.wordv, m_length, 4, 16);
#endif
	while(((uint8_t*)x.wordv)[idx]==0 && idx<m_length){
		++idx;
	}
	if(((uint8_t*)x.wordv)[idx]!=2 || idx>=m_length){
		return 1;
	}
	++idx;
	while(((uint8_t*)x.wordv)[idx+pad_length]!=0  && (idx+pad_length)<m_length){
		++pad_length;
	}
	if(pad_length<8 || (idx+pad_length)>=m_length){
		return 2;
	}
	*out_length = m_length - idx - pad_length - 1;
	if(pad){
#if DEBUG
		cli_putstr_P(PSTR("\r\npadding block:"));
		cli_hexdump_block(((uint8_t*)x.wordv)+idx, pad_length, 4, 16);
		cli_putstr_P(PSTR("\r\npad @ 0x"));
		cli_hexdump_rev(&pad, 2);
		cli_putstr_P(PSTR("\r\ndst @ 0x"));
		cli_hexdump_rev(&dest, 2);
#endif
		memcpy(pad, ((uint8_t*)x.wordv)+idx, pad_length);
	}
	memmove(dest, ((uint8_t*)x.wordv) + idx + pad_length + 1, m_length - idx - pad_length - 1);

	return 0;
}

