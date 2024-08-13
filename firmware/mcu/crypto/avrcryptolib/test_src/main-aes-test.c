/* main-aes-test.c */
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
/*
 * AES test-suit
 *
*/
#include "main-test-common.h"
#include "aes.h"

#include "performance_test.h"
#include "dump.h"

#include "bcal_aes128.h"
#include "bcal_aes192.h"
#include "bcal_aes256.h"
#include "bcal-cbc.h"
#include "bcal-cfb_byte.h"
#include "bcal-cfb_bit.h"
#include "bcal-ofb.h"
#include "bcal-ctr.h"
#include "bcal-cmac.h"
#include "bcal-eax.h"
#include "cmacvs.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"

const char* algo_name = "AES";

const bcdesc_t* const const algolist[] PROGMEM = {
	(bcdesc_t*)&aes128_desc,
	(bcdesc_t*)&aes192_desc,
	(bcdesc_t*)&aes256_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_aes(void){
	bcal_nessie_multiple(algolist);
}

void testrun_test_aes(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c };
	uint8_t data[16] = { 0x32, 0x43, 0xf6, 0xa8,
	                     0x88, 0x5a, 0x30, 0x8d,
	                     0x31, 0x31, 0x98, 0xa2,
	                     0xe0, 0x37, 0x07, 0x34 };
	aes128_ctx_t ctx;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n cipher test (FIPS 197):\r\n key:        "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	aes128_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n ciphertext: "));
	cli_hexdump(data, 16);
	aes128_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	cli_putstr(PSTR("\r\n testing bcal:"));
	bcgen_ctx_t bcal_ctx;
	uint8_t r;
	r = bcal_cipher_init(&aes128_desc, key, 128, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n init = 0x"));
	cli_hexdump(&r, 1);

	bcal_cipher_enc(data, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n ciphertext: "));
	cli_hexdump(data, 16);
	bcal_cipher_dec(data, &bcal_ctx);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	bcal_cipher_free(&bcal_ctx);
}

void testrun_testkey_aes128(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c};
	aes128_ctx_t ctx;
	uint8_t i;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 16);
	for(i=0; i<11; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes192(void){
	uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7,
	                    0xda, 0x0e, 0x64, 0x52,
	                    0xc8, 0x10, 0xf3, 0x2b,
	                    0x80, 0x90, 0x79, 0xe5,
	                    0x62, 0xf8, 0xea, 0xd2,
	                    0x52, 0x2c, 0x6b, 0x7b};
	aes192_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes192_ctx_t));
	aes192_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 24);
	for(i=0; i<13; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}


void testrun_testkey_aes256(void){
	uint8_t key[32] = { 0x60, 0x3d, 0xeb, 0x10,
	                    0x15, 0xca, 0x71, 0xbe,
	                    0x2b, 0x73, 0xae, 0xf0,
	                    0x85, 0x7d, 0x77, 0x81,
	                    0x1f, 0x35, 0x2c, 0x07,
	                    0x3b, 0x61, 0x08, 0xd7,
	                    0x2d, 0x98, 0x10, 0xa3,
	                    0x09, 0x14, 0xdf, 0xf4};
	aes256_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes256_ctx_t));
	aes256_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 32);
	for(i=0; i<15; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes(void){
	testrun_testkey_aes128();
	testrun_testkey_aes192();
	testrun_testkey_aes256();
}

const uint8_t modes_key[]   PROGMEM = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	};
const uint8_t modes_iv[]    PROGMEM = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

const uint8_t modes_ctriv[] PROGMEM = {
		0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
		0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
	};

const uint8_t modes_plain[] PROGMEM = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		/* --- */
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		/* --- */
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
		0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		/* --- */
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
		0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
    };


void testrun_aes128_cbc(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cbc_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CBC-TEST **"));
	r = bcal_cbc_init(&aes128_desc, key, 128, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	if(r)
		return;
	bcal_cbc_encMsg(iv, plain, 4, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_cbc_decMsg(iv, plain, 4, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_cbc_free(&ctx);
}

void testrun_aes128_cfb8(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cfb_B_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CFB8-TEST **"));
	r = bcal_cfb_B_init(&aes128_desc, key, 128, 8, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	if(r)
		return;
	bcal_cfb_B_encMsg(iv, plain, 64, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 64, 4, 16);

	bcal_cfb_B_decMsg(iv, plain, 64, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 64, 4, 16);

	bcal_cfb_B_free(&ctx);

}

void testrun_aes128_cfb1(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_cfb_b_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CFB1-TEST **"));
	r = bcal_cfb_b_init(&aes128_desc, key, 128, 1, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 2, 4, 16);
	if(r)
		return;
	uint8_t i, bit_offset, byte_offset;
	bcal_cfb_b_loadIV(iv, &ctx);
	for(i=0; i<16; ++i){
		byte_offset = i/8;
		bit_offset = i&7;
		cli_putstr_P(PSTR("\r\n  plain bit:   "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
		bcal_cfb_b_encNext(plain+byte_offset, bit_offset, &ctx);
		cli_putstr_P(PSTR("\r\n  cipher bit:  "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
	}
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 2, 4, 16);

	bcal_cfb_b_loadIV(iv, &ctx);
	for(i=0; i<16; ++i){
		byte_offset = i/8;
		bit_offset = i&7;
		cli_putstr_P(PSTR("\r\n  plain bit:   "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
		bcal_cfb_b_decNext(plain+byte_offset, bit_offset, &ctx);
		cli_putstr_P(PSTR("\r\n  cipher bit:  "));
		cli_putc((plain[byte_offset]&(1<<(7-bit_offset)))?'1':'0');
	}
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 2, 4, 16);


	bcal_cfb_b_encMsg(iv, plain, 0, 64*8, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 64, 4, 16);

	bcal_cfb_b_decMsg(iv, plain, 0, 64*8, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 64, 4, 16);

	bcal_cfb_b_free(&ctx);
}

void testrun_aes128_ofb(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_ofb_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_iv,    16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-OFB-TEST **"));
	r = bcal_ofb_init(&aes128_desc, key, 128, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	if(r)
		return;
	bcal_ofb_encMsg(iv, plain, 4*128, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_ofb_decMsg(iv, plain, 4*128, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_ofb_free(&ctx);
}

void testrun_aes128_ctr(void){
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t plain[64];

	bcal_ctr_ctx_t ctx;
	uint8_t r;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(iv,    modes_ctriv, 16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CTR-TEST **"));
	r = bcal_ctr_init(&aes128_desc, key, 128, NULL, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	cli_putstr_P(PSTR("\r\n  IV:    "));
	cli_hexdump(iv, 128/8);
	cli_putstr_P(PSTR("\r\n  plaintext:"));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	if(r)
		return;
	bcal_ctr_encMsg(iv, plain, 4*128, &ctx);
	cli_putstr_P(PSTR("\r\n  ciphertext:  "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_ctr_decMsg(iv, plain, 4*128, &ctx);
	cli_putstr_P(PSTR("\r\n  plaintext:   "));
	cli_hexdump_block(plain, 4*128/8, 4, 16);
	bcal_ctr_free(&ctx);
}

void testrun_aes128_cmac(void){
	uint8_t key[16];
	uint8_t tag[16];
	uint8_t plain[64];
	uint16_t length[] = { 0, 128, 320, 512 };
	bcal_cmac_ctx_t ctx;
	uint8_t r,i;

	memcpy_P(key,   modes_key,   16);
	memcpy_P(plain, modes_plain, 64);

	cli_putstr_P(PSTR("\r\n** AES128-CMAC-TEST **"));

	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	for(i=0; i<4; ++i){
		r = bcal_cmac_init(&aes128_desc, key, 128, &ctx);
		cli_putstr_P(PSTR("\r\n  init = 0x"));
		cli_hexdump(&r, 1);
		cli_putstr_P(PSTR("\r\n  message: "));
		cli_hexdump_block(plain, length[i]/8, 4, 16);
		if(r)
			return;
		bcal_cmac(tag, 128, plain, length[i], &ctx);
		cli_putstr_P(PSTR("\r\n  tag:     "));
		cli_hexdump_block(tag, 128/8, 4, 16);
		bcal_cmac_free(&ctx);
	}
}
/*
Klen = 16
Mlen = 18
Tlen = 2
Key = 3250974e306b4b678f914b514d1e90f6
Msg = cf132fd4ebc25fd3866f1a95a6193a1a9cdf
*/
void testrun_aes128_cmac72(void){
	uint8_t key[16]= {
			0x32, 0x50, 0x97, 0x4e, 0x30, 0x6b, 0x4b, 0x67,
			0x8f, 0x91, 0x4b, 0x51, 0x4d, 0x1e, 0x90, 0xf6
	};
	uint8_t tag[2];
	uint8_t plain[18] = {
			0xcf, 0x13, 0x2f, 0xd4, 0xeb, 0xc2, 0x5f, 0xd3,
			0x86, 0x6f, 0x1a, 0x95, 0xa6, 0x19, 0x3a, 0x1a,
			0x9c, 0xdf,
	};
	bcal_cmac_ctx_t ctx;
	uint8_t r;


	cli_putstr_P(PSTR("\r\n** AES128-CMAC-72-TEST **"));

	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 128/8);
	r = bcal_cmac_init(&aes128_desc, key, 128, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	cli_putstr_P(PSTR("\r\n  message: "));
	cli_hexdump_block(plain, 18, 4, 16);
	if(r)
		return;
	bcal_cmac(tag, 16, plain, 18*8, &ctx);
	cli_putstr_P(PSTR("\r\n  tag:     "));
	cli_hexdump_block(tag, 2, 4, 16);
	bcal_cmac_free(&ctx);
}
/*
Count = 0
Klen = 24
Mlen = 0
Tlen = 2
Key = 2b2aaa666be161ed16648e862ac9bd1e317f71bc69e268b5
Msg = 00
*/
void testrun_aes192_cmac0(void){
	uint8_t key[24]= {
			0x2b, 0x2a, 0xaa, 0x66, 0x6b, 0xe1, 0x61, 0xed,
			0x16, 0x64, 0x8e, 0x86, 0x2a, 0xc9, 0xbd, 0x1e,
			0x31, 0x7f, 0x71, 0xbc, 0x69, 0xe2, 0x68, 0xb5
	};
	uint8_t tag[2];
	uint8_t plain[1] = {
			0x00
	};
	bcal_cmac_ctx_t ctx;
	uint8_t r;


	cli_putstr_P(PSTR("\r\n** AES192-CMAC-0-TEST **"));

	cli_putstr_P(PSTR("\r\n  key:   "));
	cli_hexdump(key, 192/8);
	r = bcal_cmac_init(&aes192_desc, key, 192, &ctx);
	cli_putstr_P(PSTR("\r\n  init = 0x"));
	cli_hexdump(&r, 1);
	if(r)
		return;
	bcal_cmac(tag, 16, plain, 0*8, &ctx);
	cli_putstr_P(PSTR("\r\n  tag:     "));
	cli_hexdump_block(tag, 2, 4, 16);
	bcal_cmac_free(&ctx);
}

const uint8_t eax_msg[] PROGMEM = {
    0xF7, 0xFB,
    0x1A, 0x47, 0xCB, 0x49, 0x33,
    0x48, 0x1C, 0x9E, 0x39, 0xB1,
    0x40, 0xD0, 0xC0, 0x7D, 0xA5, 0xE4,
    0x4D, 0xE3, 0xB3, 0x5C, 0x3F, 0xC0, 0x39, 0x24, 0x5B, 0xD1, 0xFB, 0x7D,
    0x8B, 0x0A, 0x79, 0x30, 0x6C, 0x9C, 0xE7, 0xED, 0x99, 0xDA, 0xE4, 0xF8, 0x7F, 0x8D, 0xD6, 0x16, 0x36,
    0x1B, 0xDA, 0x12, 0x2B, 0xCE, 0x8A, 0x8D, 0xBA, 0xF1, 0x87, 0x7D, 0x96, 0x2B, 0x85, 0x92, 0xDD, 0x2D, 0x56,
    0x6C, 0xF3, 0x67, 0x20, 0x87, 0x2B, 0x85, 0x13, 0xF6, 0xEA, 0xB1, 0xA8, 0xA4, 0x44, 0x38, 0xD5, 0xEF, 0x11,
    0xCA, 0x40, 0xD7, 0x44, 0x6E, 0x54, 0x5F, 0xFA, 0xED, 0x3B, 0xD1, 0x2A, 0x74, 0x0A, 0x65, 0x9F, 0xFB, 0xBB, 0x3C, 0xEA, 0xB7
};

const uint8_t eax_msg_len[] PROGMEM =  {0, 2, 5, 5, 6, 12, 17, 18, 18, 21};

const uint8_t eax_key[] PROGMEM = {
	0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F, 0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78,
    0x91, 0x94, 0x5D, 0x3F, 0x4D, 0xCB, 0xEE, 0x0B, 0xF4, 0x5E, 0xF5, 0x22, 0x55, 0xF0, 0x95, 0xA4,
    0x01, 0xF7, 0x4A, 0xD6, 0x40, 0x77, 0xF2, 0xE7, 0x04, 0xC0, 0xF6, 0x0A, 0xDA, 0x3D, 0xD5, 0x23,
    0xD0, 0x7C, 0xF6, 0xCB, 0xB7, 0xF3, 0x13, 0xBD, 0xDE, 0x66, 0xB7, 0x27, 0xAF, 0xD3, 0xC5, 0xE8,
    0x35, 0xB6, 0xD0, 0x58, 0x00, 0x05, 0xBB, 0xC1, 0x2B, 0x05, 0x87, 0x12, 0x45, 0x57, 0xD2, 0xC2,
    0xBD, 0x8E, 0x6E, 0x11, 0x47, 0x5E, 0x60, 0xB2, 0x68, 0x78, 0x4C, 0x38, 0xC6, 0x2F, 0xEB, 0x22,
    0x7C, 0x77, 0xD6, 0xE8, 0x13, 0xBE, 0xD5, 0xAC, 0x98, 0xBA, 0xA4, 0x17, 0x47, 0x7A, 0x2E, 0x7D,
    0x5F, 0xFF, 0x20, 0xCA, 0xFA, 0xB1, 0x19, 0xCA, 0x2F, 0xC7, 0x35, 0x49, 0xE2, 0x0F, 0x5B, 0x0D,
    0xA4, 0xA4, 0x78, 0x2B, 0xCF, 0xFD, 0x3E, 0xC5, 0xE7, 0xEF, 0x6D, 0x8C, 0x34, 0xA5, 0x61, 0x23,
    0x83, 0x95, 0xFC, 0xF1, 0xE9, 0x5B, 0xEB, 0xD6, 0x97, 0xBD, 0x01, 0x0B, 0xC7, 0x66, 0xAA, 0xC3
};

const uint8_t eax_nonce[] PROGMEM = {
    0x62, 0xEC, 0x67, 0xF9, 0xC3, 0xA4, 0xA4, 0x07, 0xFC, 0xB2, 0xA8, 0xC4, 0x90, 0x31, 0xA8, 0xB3,
    0xBE, 0xCA, 0xF0, 0x43, 0xB0, 0xA2, 0x3D, 0x84, 0x31, 0x94, 0xBA, 0x97, 0x2C, 0x66, 0xDE, 0xBD,
    0x70, 0xC3, 0xDB, 0x4F, 0x0D, 0x26, 0x36, 0x84, 0x00, 0xA1, 0x0E, 0xD0, 0x5D, 0x2B, 0xFF, 0x5E,
    0x84, 0x08, 0xDF, 0xFF, 0x3C, 0x1A, 0x2B, 0x12, 0x92, 0xDC, 0x19, 0x9E, 0x46, 0xB7, 0xD6, 0x17,
    0xFD, 0xB6, 0xB0, 0x66, 0x76, 0xEE, 0xDC, 0x5C, 0x61, 0xD7, 0x42, 0x76, 0xE1, 0xF8, 0xE8, 0x16,
    0x6E, 0xAC, 0x5C, 0x93, 0x07, 0x2D, 0x8E, 0x85, 0x13, 0xF7, 0x50, 0x93, 0x5E, 0x46, 0xDA, 0x1B,
    0x1A, 0x8C, 0x98, 0xDC, 0xD7, 0x3D, 0x38, 0x39, 0x3B, 0x2B, 0xF1, 0x56, 0x9D, 0xEE, 0xFC, 0x19,
    0xDD, 0xE5, 0x9B, 0x97, 0xD7, 0x22, 0x15, 0x6D, 0x4D, 0x9A, 0xFF, 0x2B, 0xC7, 0x55, 0x98, 0x26,
    0xB7, 0x81, 0xFC, 0xF2, 0xF7, 0x5F, 0xA5, 0xA8, 0xDE, 0x97, 0xA9, 0xCA, 0x48, 0xE5, 0x22, 0xEC,
    0x22, 0xE7, 0xAD, 0xD9, 0x3C, 0xFC, 0x63, 0x93, 0xC5, 0x7E, 0xC0, 0xB3, 0xC1, 0x7D, 0x6B, 0x44
};

const uint8_t eax_header[] PROGMEM = {
    0x6B, 0xFB, 0x91, 0x4F, 0xD0, 0x7E, 0xAE, 0x6B,
    0xFA, 0x3B, 0xFD, 0x48, 0x06, 0xEB, 0x53, 0xFA,
    0x23, 0x4A, 0x34, 0x63, 0xC1, 0x26, 0x4A, 0xC6,
    0x33, 0xCC, 0xE2, 0xEA, 0xBF, 0xF5, 0xA7, 0x9D,
    0xAE, 0xB9, 0x6E, 0xAE, 0xBE, 0x29, 0x70, 0xE9,
    0xD4, 0x48, 0x2D, 0x1C, 0xA7, 0x8D, 0xCE, 0x0F,
    0x65, 0xD2, 0x01, 0x79, 0x90, 0xD6, 0x25, 0x28,
    0x54, 0xB9, 0xF0, 0x4E, 0x6A, 0x09, 0x18, 0x9A,
    0x89, 0x9A, 0x17, 0x58, 0x97, 0x56, 0x1D, 0x7E,
    0x12, 0x67, 0x35, 0xFC, 0xC3, 0x20, 0xD2, 0x5A
};

const uint8_t eax_cipher[] PROGMEM = {
    0xE0, 0x37, 0x83, 0x0E, 0x83, 0x89, 0xF2, 0x7B, 0x02, 0x5A, 0x2D, 0x65, 0x27, 0xE7, 0x9D, 0x01,
    0x19, 0xDD, 0x5C, 0x4C, 0x93, 0x31, 0x04, 0x9D, 0x0B, 0xDA, 0xB0, 0x27, 0x74, 0x08, 0xF6, 0x79, 0x67, 0xE5,
    0xD8, 0x51, 0xD5, 0xBA, 0xE0, 0x3A, 0x59, 0xF2, 0x38, 0xA2, 0x3E, 0x39, 0x19, 0x9D, 0xC9, 0x26, 0x66, 0x26, 0xC4, 0x0F, 0x80,
    0x63, 0x2A, 0x9D, 0x13, 0x1A, 0xD4, 0xC1, 0x68, 0xA4, 0x22, 0x5D, 0x8E, 0x1F, 0xF7, 0x55, 0x93, 0x99, 0x74, 0xA7, 0xBE, 0xDE,
    0x07, 0x1D, 0xFE, 0x16, 0xC6, 0x75, 0xCB, 0x06, 0x77, 0xE5, 0x36, 0xF7, 0x3A, 0xFE, 0x6A, 0x14, 0xB7, 0x4E, 0xE4, 0x98, 0x44, 0xDD,
    0x83, 0x5B, 0xB4, 0xF1, 0x5D, 0x74, 0x3E, 0x35, 0x0E, 0x72, 0x84, 0x14, 0xAB, 0xB8, 0x64, 0x4F, 0xD6, 0xCC, 0xB8, 0x69, 0x47, 0xC5, 0xE1, 0x05, 0x90, 0x21, 0x0A, 0x4F,
    0x02, 0x08, 0x3E, 0x39, 0x79, 0xDA, 0x01, 0x48, 0x12, 0xF5, 0x9F, 0x11, 0xD5, 0x26, 0x30, 0xDA, 0x30, 0x13, 0x73, 0x27, 0xD1, 0x06, 0x49, 0xB0, 0xAA, 0x6E, 0x1C, 0x18, 0x1D, 0xB6, 0x17, 0xD7, 0xF2,
    0x2E, 0xC4, 0x7B, 0x2C, 0x49, 0x54, 0xA4, 0x89, 0xAF, 0xC7, 0xBA, 0x48, 0x97, 0xED, 0xCD, 0xAE, 0x8C, 0xC3, 0x3B, 0x60, 0x45, 0x05, 0x99, 0xBD, 0x02, 0xC9, 0x63, 0x82, 0x90, 0x2A, 0xEF, 0x7F, 0x83, 0x2A,
    0x0D, 0xE1, 0x8F, 0xD0, 0xFD, 0xD9, 0x1E, 0x7A, 0xF1, 0x9F, 0x1D, 0x8E, 0xE8, 0x73, 0x39, 0x38, 0xB1, 0xE8, 0xE7, 0xF6, 0xD2, 0x23, 0x16, 0x18, 0x10, 0x2F, 0xDB, 0x7F, 0xE5, 0x5F, 0xF1, 0x99, 0x17, 0x00,
    0xCB, 0x89, 0x20, 0xF8, 0x7A, 0x6C, 0x75, 0xCF, 0xF3, 0x96, 0x27, 0xB5, 0x6E, 0x3E, 0xD1, 0x97, 0xC5, 0x52, 0xD2, 0x95, 0xA7, 0xCF, 0xC4, 0x6A, 0xFC, 0x25, 0x3B, 0x46, 0x52, 0xB1, 0xAF, 0x37, 0x95, 0xB1, 0x24, 0xAB, 0x6E
};

void testrun_aes128_eax(void){
	uint8_t key[16];
	uint8_t nonce[16];
	uint8_t header[8];
	uint8_t tag[16];
	uint8_t msg[21];
	uint8_t msg_len;
	PGM_VOID_P msg_p;
	PGM_VOID_P cipher_p;
	uint8_t i, r;

	bcal_eax_ctx_t ctx;

	msg_p = eax_msg;
	cipher_p = eax_cipher;
	for(i=0; i<10; ++i){
		cli_putstr_P(PSTR("\r\n\r\n** AES128-EAX-TEST #"));
		cli_putc('0'+i);
		cli_putstr_P(PSTR(" **"));

		msg_len = pgm_read_byte(eax_msg_len+i);
		memcpy_P(key, eax_key+16*i, 16);
		memcpy_P(nonce, eax_nonce+16*i, 16);
		memcpy_P(header, eax_header+8*i, 8);
		memcpy_P(msg, msg_p, msg_len);
		msg_p = (uint8_t*)msg_p+msg_len;

		cli_putstr_P(PSTR("\r\n  key:     "));
		cli_hexdump(key, 16);
		cli_putstr_P(PSTR("\r\n  msg:     "));
		if(msg_len){
			cli_hexdump(msg, msg_len);
		}
		cli_putstr_P(PSTR("\r\n  nonce:   "));
		cli_hexdump(nonce, 16);
		cli_putstr_P(PSTR("\r\n  header:  "));
		cli_hexdump(header, 8);

		r = bcal_eax_init(&aes128_desc, key, 128, &ctx);
		cli_putstr_P(PSTR("\r\n  init = 0x"));
		cli_hexdump(&r, 1);
		if(r)
			return;
		bcal_eax_loadNonce(nonce, 16*8, &ctx);
		bcal_eax_addLastHeader(header, 8*8, &ctx);
		bcal_eax_encLastBlock(msg, msg_len*8, &ctx);
		bcal_eax_ctx2tag(tag, 128, &ctx);

		cli_putstr_P(PSTR("\r\n  cipher:  "));
		cli_hexdump_block(msg, msg_len, 4, 16);

		cli_putstr_P(PSTR("\r\n  tag:     "));
		cli_hexdump_block(tag, 16, 4, 16);

		if(memcmp_P(msg, cipher_p, msg_len)){
			cli_putstr_P(PSTR("\r\n cipher:  [fail]\r\n  should: "));
			memcpy_P(msg, cipher_p, msg_len);
			cli_hexdump_block(msg, msg_len, 4, 16);
		}else{
			cli_putstr_P(PSTR("\r\n cipher:  [pass]"));
		}
		cipher_p = ((uint8_t*)cipher_p)+msg_len;
		// *
		if(memcmp_P(tag, cipher_p, 16)){
			cli_putstr_P(PSTR("\r\n tag:     [fail]"));
		}else{
			cli_putstr_P(PSTR("\r\n tag:     [pass]"));
		}

		cipher_p = ((uint8_t*)cipher_p)+16;
		bcal_eax_free(&ctx);
	}
}

/*****************************************************************************/

void testrun_performance_aes(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]       PROGMEM = "nessie";
const char test_str[]         PROGMEM = "test";
const char testkey_str[]      PROGMEM = "testkey";
const char testcbc_str[]      PROGMEM = "testcbc";
const char testcfb8_str[]     PROGMEM = "testcfb8";
const char testcfb1_str[]     PROGMEM = "testcfb1";
const char testofb_str[]      PROGMEM = "testofb";
const char testctr_str[]      PROGMEM = "testctr";
const char testcmac_str[]     PROGMEM = "testcmac";
const char testcmac72_str[]   PROGMEM = "testcmac72";
const char testcmac0_str[]    PROGMEM = "testcmac0";
const char testeax_str[]      PROGMEM = "testeax";
const char cmacvs_list_str[]  PROGMEM = "cmacvs_list";
const char cmacvs_set_str[]   PROGMEM = "cmacvs_set";
const char cmacvs_test1_str[] PROGMEM = "cmacvs_test1";
const char cmacvs_test2_str[] PROGMEM = "cmacvs_test2";
const char performance_str[]  PROGMEM = "performance";
const char dump_str[]         PROGMEM = "dump";
const char echo_str[]         PROGMEM = "echo";

const const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_aes              },
	{ test_str,            NULL, testrun_test_aes                },
	{ testkey_str,         NULL, testrun_testkey_aes             },
	{ testcbc_str,         NULL, testrun_aes128_cbc              },
	{ testcfb8_str,        NULL, testrun_aes128_cfb8             },
	{ testcfb1_str,        NULL, testrun_aes128_cfb1             },
	{ testofb_str,         NULL, testrun_aes128_ofb              },
	{ testctr_str,         NULL, testrun_aes128_ctr              },
	{ testcmac_str,        NULL, testrun_aes128_cmac             },
	{ testcmac72_str,      NULL, testrun_aes128_cmac72           },
	{ testcmac0_str,       NULL, testrun_aes192_cmac0            },
	{ testeax_str,         NULL, testrun_aes128_eax              },
	{ cmacvs_list_str,     NULL, cmacvs_listalgos                },
	{ cmacvs_set_str,  (void*)1, (void_fpt)cmacvs_setalgo        },
	{ cmacvs_test1_str,    NULL, cmacvs_test1                    },
	{ cmacvs_test2_str,    NULL, cmacvs_test2                    },
	{ performance_str,     NULL, testrun_performance_aes         },
	{ dump_str,        (void*)1, (void_fpt)dump                  },
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl             },
	{ NULL,                NULL, NULL                            }
};


int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	cmacvs_algolist=(bcdesc_t**)algolist;
	cmacvs_algo=(bcdesc_t*)&aes128_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
int main(void) {
	main_setup();

	cmacvs_algolist=(bcdesc_t**)algolist;
	cmacvs_algo=(bcdesc_t*)&aes128_desc;

	for(;;){
		welcome_msg(algo_name);
		cmd_interface(cmdlist);
    }

}

