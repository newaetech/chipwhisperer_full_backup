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
 * CLib Functions
 *
 * Very simple app demonstrating the usage of
 * QM_PUTS, QM_PRINTF, QM_ASSERT,
 * pico_printf, printf, malloc and free.
 */

#include <stdlib.h>
#include "qm_common.h"

#define SIZE (100)

int main(void)
{
	int i, *p;

	QM_PUTS("Starting: clib");

	p = malloc(SIZE * sizeof(int));

	if (NULL == p) {
		QM_PUTS("Error: malloc unable to allocate memory");
		return -ENOMEM;
	}

	for (i = 0; i < SIZE; i++) {
		p[i] = i;
	}

	QM_PUTS("Demonstrating QM_PUTS/QM_PRINTF functionality");

	for (i = 0; i < 10; i++) {
		QM_PRINTF("%d\n", p[i]);
	}

	free(p);

	/* pico_printf only supports %d, %u, %x, %X and %s. */
	pico_printf("pico_printf demo: %d\n", 5);

	/* Full printf has a bigger footprint than pico_printf. */
	printf("printf demo: %.3f\n", 3.14159);

	QM_PUTS("Demonstrating QM_ASSERT functionality: \"Assert 1 == 1\"");

	QM_ASSERT(1 == 1);

	QM_PUTS("Finished: clib");

	return 0;
}
