/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "qm_soc_regs.h"

#include "qm_interrupt.h"
#include "qm_isr.h"

#include "clk.h"

#include "dfu/usb-dfu/usb_dfu.h"

#if UNIT_TEST
/* Loop-control variable used by unit tests. */
int test_fm_entry_usb_loop;
#define FOREVER() (test_fm_entry_usb_loop--)
#else
#define FOREVER() (1)
#endif

int fm_entry_usb(void)
{
	/*
	 * NOTE: this delay is needed to make Windows properly detect the device
	 * after a DFU Detach from the application.
	 */
	clk_sys_udelay(50000);
	usb_dfu_start();

	qm_irq_request(QM_IRQ_USB_0_INT, qm_usb_0_isr);

	/*
	 * NOTE: consider making this loop smarter by moving the timeout logic
	 * here.
	 */
	while (FOREVER()) {
		/* do nothing */
	}

	return 0;
}
