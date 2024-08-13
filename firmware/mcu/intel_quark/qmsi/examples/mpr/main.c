/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
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

/*
 * Memory Protection Region (MPR)
 *
 * This example uses an MPR to protect a 1kB page of SRAM and then attempts to
 * write to that region which will trigger an MPR violation interrupt.
 */

#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_mpr.h"

#define MPR_PAGE_SIZE (0x400)
#if (QUARK_D2000)
#define SRAM_BASE (0x00280000)
#elif(QUARK_SE)
#define SRAM_BASE (0xA8000000)
#endif

extern char __heap; /* Address of the start of the heap. */
static volatile bool callback_invoked = false;

/* MPR example callback. */
static void mpr_example_callback(void *data)
{
	callback_invoked = true;
}

int main(void)
{
	qm_mpr_config_t cfg;
	uint8_t lower_bound;
	uint32_t heap_offset, mpr_base;

	QM_PUTS("Starting: MPR");

	/*
	 * The MPR is located in the heap in order to ensure it doesn't clash
	 * with anything so it is necessary to calculate the page number that
	 * corresponds to the start of the heap.
	 */
	heap_offset = (uint32_t)&__heap;
	lower_bound = (uint8_t)((heap_offset - SRAM_BASE) / MPR_PAGE_SIZE) + 1;

	/* Calculate the physical address of the start of the MPR. */
	mpr_base = SRAM_BASE + (lower_bound * MPR_PAGE_SIZE);

	/* Request the IRQ. */
	qm_irq_request(QM_IRQ_SRAM_MPR_0_INT, qm_sram_mpr_0_isr);

	/* Set the violation policy to trigger an interrupt. */
	qm_mpr_set_violation_policy(MPR_VIOL_MODE_INTERRUPT,
				    mpr_example_callback, NULL);

	/* Configure MPR to allow R/W from DMA agent only. */
	cfg.en_lock_mask = QM_SRAM_MPR_EN_MASK_ENABLE;
	cfg.agent_read_en_mask = QM_SRAM_MPR_AGENT_MASK_DMA;
	cfg.agent_write_en_mask = QM_SRAM_MPR_AGENT_MASK_DMA;
	cfg.up_bound = lower_bound;
	cfg.low_bound = lower_bound;

	qm_mpr_set_config(QM_MPR_0, &cfg);

	/* Trigger a violation event by attempting a write inside the MPR. */
	REG_VAL(mpr_base + 1) = 0xff;

	/* Wait for the callback to be invoked. */
	while (false == callback_invoked)
		;

	QM_PUTS("MPR Violation!");

	QM_PUTS("Finished: MPR");

	return 0;
}
