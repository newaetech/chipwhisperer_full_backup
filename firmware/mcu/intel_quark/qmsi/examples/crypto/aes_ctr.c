/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * AES CTR ciphering and deciphering example.
 * Part of Tinycrypt crypto example.
 *
 * This example shows how to cipher and decipher data using Tinycrypt AES CTR
 * functions.
 */

#include <string.h>
#include <x86intrin.h>
#include "clk.h"
#include "qm_common.h"
#include "tinycrypt/ctr_mode.h"

/* Pre-computed cipher text. */
static const uint8_t aes_ctr_ciphertext[80] = {
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
    0xec, 0xed, 0xee, 0xef, 0x43, 0xa6, 0x54, 0xcd, 0xfb, 0x0f, 0xdc, 0x9b,
    0x9c, 0x48, 0x42, 0x88, 0x31, 0x85, 0x19, 0xbc, 0x02, 0xc0, 0xaa, 0xc2,
    0x3c, 0x78, 0x7b, 0x17, 0x5b, 0x9e, 0xc0, 0xa2, 0x74, 0x87, 0xe8, 0xb1,
    0x45, 0xee, 0xf8, 0x76, 0xfe, 0xa7, 0x2e, 0xdb, 0xcf, 0x7d, 0x83, 0xc2,
    0x58, 0x7b, 0x37, 0xe1, 0x17, 0x5f, 0x12, 0x54, 0xe1, 0x87, 0xfa, 0xe0,
    0x94, 0x61, 0x28, 0xea, 0x0e, 0xf0, 0xec, 0x1d};

/* AES counter first value. */
static uint8_t aes_ctr_ctr[16] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
				  0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
				  0xec, 0xed, 0xee, 0xef};

/* Result of decryption. */
static uint8_t aes_ctr_decrypted[64];

/* AES key. */
static const uint8_t aes_ctr_key[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
					0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
					0x32, 0x33, 0x34, 0x35};

/* Result of encryption. */
static uint8_t aes_ctr_out[80];

/* Text to cipher. */
static const uint8_t aes_ctr_plaintext[64] = {
    0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30, 0x30, 0x30};

/* Example of AES-CTR encryption and decryption. */
int do_aes_ctr(void)
{
	uint64_t t_init, t_end;
	uint64_t t_op_usec;
	struct tc_aes_key_sched_struct sched;

	QM_PUTS("AES-CTR Encrypting plain text...");

	/* Initialize AES data structure and set the key. */
	bzero(&sched, sizeof(sched));
	tc_aes128_set_encrypt_key(&sched, aes_ctr_key);

	/* Copy the first counter value before the ciphered text. */
	memcpy(aes_ctr_out, aes_ctr_ctr, sizeof(aes_ctr_ctr));

	/* Get system tick before calculation. */
	t_init = _rdtsc();

	/* Everything is ready, cipher the text. */
	tc_ctr_mode(&aes_ctr_out[TC_AES_BLOCK_SIZE], sizeof(aes_ctr_plaintext),
		    aes_ctr_plaintext, sizeof(aes_ctr_plaintext), aes_ctr_ctr,
		    &sched);

	/* Get system tick after calculation and compute calculation time. */
	t_end = _rdtsc();
	t_op_usec = (t_end - t_init) / SYS_TICKS_PER_US_32MHZ;

	/* Compare ciphered text to pre-computed cipher text. */
	if (memcmp(aes_ctr_ciphertext, aes_ctr_out, sizeof(aes_ctr_out)) == 0) {
		QM_PRINTF("AES-CTR encryption success (%lu usecs)\n",
			  (unsigned long)t_op_usec);
	} else {
		QM_PUTS("Error: AES-CTR mismatch in data encrypted");
		return 1;
	}

	QM_PUTS("AES-CTR Decrypting cipher text...");

	/* Copy back the first counter value in the counter variable. */
	memcpy(aes_ctr_ctr, aes_ctr_out, sizeof(aes_ctr_ctr));

	/* Get system tick before calculation. */
	t_init = _rdtsc();

	/* Decipher the encrypted text. */
	tc_ctr_mode(aes_ctr_decrypted, sizeof(aes_ctr_decrypted),
		    &aes_ctr_out[TC_AES_BLOCK_SIZE], sizeof(aes_ctr_decrypted),
		    aes_ctr_ctr, &sched);

	/* Get system tick after calculation and compute calculation time. */
	t_end = _rdtsc();
	t_op_usec = (t_end - t_init) / SYS_TICKS_PER_US_32MHZ;

	/* Compare deciphered text to original text. */
	if (memcmp(aes_ctr_plaintext, aes_ctr_decrypted,
		   sizeof(aes_ctr_plaintext)) == 0) {
		QM_PRINTF("AES-CTR decryption success (%lu usecs)\n",
			  (unsigned long)t_op_usec);
	} else {
		QM_PUTS("Error: AES-CTR mismatch in data decrypted");
		return 1;
	}

	return 0;
}
