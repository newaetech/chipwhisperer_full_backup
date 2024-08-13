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
 * Reset
 *
 * This app demonstrates the usage of qm_soc_reset().
 *
 * This app uses the gps3 general purpose sticky register. Power on reset (POR)
 * value is 0. The value of this register is cleared on POR or COLD reset. The
 * value is preserved across a WARM reset.
 */

#include "qm_common.h"
#include "qm_init.h"

int main(void)
{
	QM_SCSS_GP->gps3++;

	switch (QM_SCSS_GP->gps3) {
	case 1:
		QM_PUTS("Starting: Reset");

		QM_PUTS("Last reset type: Power on reset or COLD reset.");

		qm_soc_reset(QM_WARM_RESET);

	case 2:
		QM_PUTS("Last reset type: Warm reset.");
		/*
		 * Cold reset clears the sticky registers, so can not track if
		 * the reset was a cold reset or a power-on reset. Uncomment
		 * the following line to demonstrate a cold reset.
		 */
		/*qm_soc_reset(QM_COLD_RESET);*/

		/* We reached the end of the example.  Because general purpose
		 * registers are sticky, reset it to 0. */
		QM_SCSS_GP->gps3 = 0;
		break;

	default:
		QM_ASSERT(0);
		break;
	}

	QM_PUTS("Finished: Reset");

	return 0;
}
