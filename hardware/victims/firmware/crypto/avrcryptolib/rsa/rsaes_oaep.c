
/* rsa_oaep.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2012 Daniel Otte (daniel.otte@rub.de)

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
#include "memxor.h"
#include "mgf1.h"
#include "bigint.h"
#include "rsa_basic.h"
#include "rsaes_oaep.h"

#include "random_dummy.h"

#include "hfal/hfal_sha1.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

mgf1_parameter_t mgf1_default_parameter = {
		&sha1_desc
};

rsa_oaep_parameter_t rsa_oaep_default_parameter = {
		mgf1,
		&sha1_desc,
		&mgf1_default_parameter
};

rsa_label_t rsa_oaep_default_label = {
		0, NULL
};

uint8_t rsa_encrypt_oaep(void* dest, uint16_t* out_length,
		              const void* src, uint16_t length_B,
		              rsa_publickey_t* key, const rsa_oaep_parameter_t *p,
		              const rsa_label_t* label, const void* seed){

	if(!p){
		p = &rsa_oaep_default_parameter;
	}
	if(!label){
		label = &rsa_oaep_default_label;
	}
	uint16_t hv_len = (hfal_hash_getHashsize(p->hf)+7)/8;
	if(length_B > bigint_length_B(&key->modulus) - 2*hv_len - 2){
		/* message too long */
		return 1;
	}
	uint16_t buffer_len = bigint_length_B(&key->modulus);
#if DEBUG
	cli_putstr("\r\n buffer_len = ");
	cli_hexdump_rev(&buffer_len, 2);
	cli_putstr("\r\n modulus_len = ");
	cli_hexdump_rev(&key->modulus.length_B, 2);
#endif
	uint8_t* buffer = (uint8_t*)dest;
	uint8_t off;
	/* the following needs some explanation:
	 * off is the offset which is used for compensating the effect of
	 * changeendian() when it operates on multi-byte words.
	 * */
	off = (sizeof(bigint_word_t) - (bigint_get_first_set_bit(&key->modulus)/8+1) % sizeof(bigint_word_t))
			% (sizeof(bigint_word_t));
	buffer += off;
    buffer_len -= off;
	uint8_t* seed_buffer = buffer + 1;
	uint16_t db_len = buffer_len - hv_len - 1;
	uint8_t* db = seed_buffer + hv_len;
	uint16_t maskbuffer_len = db_len>hv_len?db_len:hv_len;
	uint8_t maskbuffer[maskbuffer_len];
	bigint_t x;

	memset(dest, 0, seed_buffer - buffer + off);
	memset(db + hv_len, 0, db_len - hv_len - length_B -1);
	hfal_hash_mem(p->hf, db, label->label, label->length_b);
	db[db_len - length_B - 1] = 0x01;
	memcpy(db+db_len - length_B, src, length_B);
	if(seed){
		memcpy(seed_buffer, seed, hv_len);
	}else{
		/* generate random seed */
		if(!prng_get_byte){
			return 2; /* ERROR: no random generator specified */
		}
		uint16_t i;
		for(i=0; i<hv_len; ++i){
			seed_buffer[i] = prng_get_byte();
		}
	}
#if DEBUG
	cli_putstr("\r\n  msg (raw, pre-feistel):\r\n");
	cli_hexdump_block(dest, bigint_length_B(&key->modulus), 4, 16);
#endif
	p->mgf(maskbuffer, seed_buffer, hv_len, db_len, p->mgf_parameter);
	memxor(db, maskbuffer, db_len);
	p->mgf(maskbuffer, db, db_len, hv_len, p->mgf_parameter);
	memxor(seed_buffer, maskbuffer, hv_len);
#if DEBUG
	cli_putstr("\r\n  msg (raw, post-feistel):\r\n");
	cli_hexdump_block(dest, bigint_length_B(&key->modulus), 4, 16);
#endif
	x.info = 0;
	x.length_B = key->modulus.length_B;
	x.wordv = dest;
	bigint_adjust(&x);
	rsa_os2ip(&x, NULL, bigint_length_B(&key->modulus));
#if DEBUG
	cli_putstr("\r\ninput-msg (pre enc):\r\n");
	cli_hexdump_rev(&src, 2);
	cli_hexdump_block(src, length_B, 4, 16);
#endif
	rsa_enc(&x, key);
#if DEBUG
	cli_putstr("\r\ninput-msg (post enc):\r\n");
	cli_hexdump_rev(&src, 2);
	cli_hexdump_block(src, length_B, 4, 16);
#endif
	rsa_i2osp(NULL, &x, out_length);
	return 0;
}

uint8_t rsa_decrypt_oaep(void* dest, uint16_t* out_length,
		              const void* src, uint16_t length_B,
		              rsa_privatekey_t* key, const rsa_oaep_parameter_t *p,
		              const rsa_label_t* label, void* seed){

//	cli_putstr("\r\n -->rsa_decrypt_oaep()"); uart_flush(0);
	if(!label){
		label = &rsa_oaep_default_label;
	}
	if(!p){
		p = &rsa_oaep_default_parameter;
	}
	uint16_t x_len, data_len;
	bigint_t x;
	uint16_t hv_len = hfal_hash_getHashsize(p->hf)/8;
	uint8_t label_hv[hv_len];
	uint16_t msg_len = bigint_get_first_set_bit(&key->modulus) / 8 + 1;
	uint16_t db_len = msg_len - hv_len - 1;
	uint8_t maskbuffer[db_len>hv_len?db_len:hv_len];

	uint8_t *seed_buffer = dest;
	uint8_t *db_buffer = seed_buffer + hv_len;

	x_len = bigint_get_first_set_bit(&key->modulus)/8;
	memset(dest, 0, bigint_length_B(&key->modulus) - length_B);
	memcpy((uint8_t*)dest + bigint_length_B(&key->modulus) - length_B, src, length_B);

//	cli_putc('a'); uart_flush(0);

	x.wordv = dest;
	x.length_B = key->modulus.length_B;
	x.info = 0;
	bigint_adjust(&x);


//	cli_putc('b'); uart_flush(0);
	rsa_os2ip(&x, NULL, bigint_length_B(&key->modulus));
#if DEBUG
	cli_putstr_P(PSTR("\r\n rsa decrypting ..."));
#endif
	rsa_dec(&x, key);
#if DEBUG
	cli_putstr_P(PSTR(" [done]"));
#endif
	rsa_i2osp(NULL, &x, &data_len);

//	cli_putstr("\r\n  msg (raw, pre-move):\r\n");
//	cli_hexdump_block(dest, bigint_length_B(key->modulus), 4, 16);

	if(data_len > x_len){
		return 7;
	}
/*
	cli_putstr("\r\n moving some bytes; x_len = ");
	cli_hexdump_rev(&x_len, 2);
	cli_putstr("  data_len = ");
	cli_hexdump_rev(&data_len, 2);
	uart_flush(0);
*/
	if(x_len != data_len){
		memmove((uint8_t*)dest + x_len - data_len, dest, data_len);
//		cli_putstr("  (oh, not dead yet?!)");
//		uart_flush(0);
		memset(dest, 0, x_len - data_len);
	}

	hfal_hash_mem(p->hf, label_hv, label->label, label->length_b);
/*
	cli_putstr("\r\n  msg (raw, pre-feistel):\r\n");
	cli_hexdump_block(seed_buffer, bigint_length_B(key->modulus), 4, 16);
	uart_flush(0);
*/
	p->mgf(maskbuffer, db_buffer, db_len, hv_len, p->mgf_parameter);
	memxor(seed_buffer, maskbuffer, hv_len);
	p->mgf(maskbuffer, seed_buffer, hv_len, db_len, p->mgf_parameter);
	memxor(db_buffer, maskbuffer, db_len);

	if(memcmp(label_hv, db_buffer, hv_len)){
//		cli_putstr("\r\nDBG: DB:\r\n");
//		cli_hexdump_block(db_buffer, db_len, 4, 16);
		return 2;
	}

	uint16_t ps_len=0;
	while(db_buffer[hv_len + ps_len++] == 0)
		;

	--ps_len;
	if(db_buffer[hv_len + ps_len] != 1){
		return 3;
	}

	if(seed){
		memcpy(seed, seed_buffer, hv_len);
	}

	msg_len = db_len - hv_len - 1 - ps_len;
	memmove(dest, db_buffer + hv_len + ps_len + 1, msg_len);

	*out_length = msg_len;

	return 0;
}


