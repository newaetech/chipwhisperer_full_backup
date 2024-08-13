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
 * Flash Protection Region (FPR)
 *
 * This example sets up the FPR violation policy with a callback. It then uses
 * an FPR to lock read on the flash and then try to read which should trigger
 * the violation.
 */

#include "qm_common.h"
#include "qm_fpr.h"
#include "qm_interrupt.h"
#include "qm_isr.h"

#define FPR_SIZE (0x400)
#if (QUARK_D2000)
#define FLASH_END (0x00200000)
#elif(QUARK_SE)
#define FLASH_END (0x40060000)
#endif /* QUARK_D2000 */

static volatile bool callback_invoked = false;

/*
 * Resolve the extent of the application in flash, so as to find the first
 * free flash page to safely allocate an FPR against. This is needed in order
 * to avoid setting up an FPR on a region of flash were the application
 * performs instruction or data fetches. Note the __data_lma and __data_size
 * are resolved at linking time.
 */
extern uint32_t __data_lma[];
extern uint32_t __data_size[];

static void fpr_example_cb(void *data)
{
	callback_invoked = true;
}

int main(void)
{
	uint32_t flash_base, fpr_flash, app_end, address;
	uint8_t low_bound;
	qm_fpr_config_t cfg = {0};

	QM_PUTS("Starting: FPR");

#if (QUARK_D2000)
	flash_base = QM_FLASH_REGION_SYS_0_BASE;
	fpr_flash = QM_FLASH_0;
#elif(QUARK_SE)
	flash_base = QM_FLASH_REGION_SYS_1_BASE;
	fpr_flash = QM_FLASH_1;
#endif /* QUARK_D2000 */

	/* Calculate size of the app in flash so the FPR does not overlap it. */
	app_end = (uint32_t)__data_lma + (uint32_t)__data_size;

	if ((app_end + FPR_SIZE + 1) > FLASH_END) {
		QM_PUTS("Error: No free pages.");
		return 1;
	}

	/* Calculate 1k-aligned physical flash address for FPR start. */
	low_bound = ((app_end - flash_base) / FPR_SIZE) + 1;

	/* Calculate MMIO address of a location inside the FPR. */
	address = (flash_base + (FPR_SIZE * low_bound)) + 4;

/* Set the violation policy to trigger an interrupt. */
#if (QUARK_D2000)
	qm_irq_request(QM_IRQ_FLASH_MPR_0_INT, qm_flash_mpr_0_isr);
#elif(QUARK_SE)
	qm_irq_request(QM_IRQ_FLASH_MPR_1_INT, qm_flash_mpr_1_isr);
#endif /* QUARK_D2000 */

	qm_fpr_set_violation_policy(FPR_VIOL_MODE_INTERRUPT, fpr_flash,
				    fpr_example_cb, NULL);

	/* Configure MPR to allow R/W from DMA agent only. */
	cfg.en_mask = QM_FPR_ENABLE;
	cfg.allow_agents = QM_FPR_DMA;
	cfg.up_bound = low_bound + 1;
	cfg.low_bound = low_bound;

	qm_fpr_set_config(fpr_flash, QM_FPR_0, &cfg, QM_MAIN_FLASH_SYSTEM);

	/* Trigger a violation event by attempting to read in the flash. */
	REG_VAL(address);

	/* Wait for the callback to be invoked. */
	while (false == callback_invoked)
		;

	QM_PUTS("FPR Violation!");

	QM_PUTS("Finished: FPR");

	return 0;
}
