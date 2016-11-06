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

/* HMAC SHA256 digest calculation example, part of Tinycrypt crypto example. */

#include <string.h>
#include <x86intrin.h>
#include "clk.h"
#include "qm_common.h"
#include "tinycrypt/hmac.h"

/* Data for which digest is computed. */
static const uint8_t hmac_sha256_data[28] = {
    0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31};

/* Result of digest computation. */
static uint8_t hmac_sha256_digest[32];

/* Pre-computed digest to be compared to digest computed. */
static const uint8_t hmac_sha256_expected[32] = {
    0xd6, 0xa3, 0x86, 0x9b, 0x03, 0x57, 0x83, 0xca, 0x68, 0x91, 0x06,
    0xfc, 0xf2, 0x66, 0x83, 0xba, 0xe0, 0x0a, 0x92, 0xa3, 0xb0, 0x3b,
    0x5e, 0xc3, 0xdc, 0x8a, 0xfd, 0xc8, 0x5c, 0x40, 0x3d, 0x6d};

/* HMAC SHA256 key. */
static const uint8_t hmac_sha256_key[32] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31};

/* Example of HMAC digest calculation. */
int do_hmac_sha256(void)
{
	uint64_t t_init, t_end, t_op_usec;
	struct tc_hmac_state_struct h;

	QM_PUTS("Computing HMAC-SHA256 digest...");

	/* Initialize HMAC data structure and set the key. */
	bzero(&h, sizeof(h));
	tc_hmac_set_key(&h, hmac_sha256_key, sizeof(hmac_sha256_key));

	/* Get system tick before calculation. */
	t_init = _rdtsc();

	/* Everything is ready, do the computation of the digest. */
	tc_hmac_init(&h);
	tc_hmac_update(&h, hmac_sha256_data, sizeof(hmac_sha256_data));
	tc_hmac_final(hmac_sha256_digest, TC_SHA256_DIGEST_SIZE, &h);

	/* Get system tick after calculation and compute calculation time. */
	t_end = _rdtsc();
	t_op_usec = (t_end - t_init) / SYS_TICKS_PER_US_32MHZ;

	/* Compare pre-computed digest with the one computed with tinycrypt. */
	if (memcmp(hmac_sha256_expected, hmac_sha256_digest,
		   sizeof(hmac_sha256_digest)) == 0) {
		QM_PRINTF("HMAC-SHA256 digest value OK (%lu usecs)\n",
			  (unsigned long)t_op_usec);
	} else {
		QM_PUTS("Error: HMAC-SHA256 mismatch in digest value");
		return 1;
	}

	return 0;
}
