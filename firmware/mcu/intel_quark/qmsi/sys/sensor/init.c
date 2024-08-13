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

#include "qm_common.h"
#include "qm_sensor_regs.h"

/*
 * Initialize interrupt vectors to the meaningful address.
 */
static QM_ISR_DECLARE(irq_dummy_routine)
{
}

typedef void (*fp)(void);

extern fp __ivt_vect_table[];

static void sensor_vectors_init(void)
{
	int i;
	uint32_t reg;

	/* Ensure global interrupts disabled before we mess with the setup. */
	__builtin_arc_clri();

	/* Start from i=1, skip reset vector */
	for (i = 1; i < QM_SS_INT_VECTOR_NUM; i++) {
		__builtin_arc_sr(i, QM_SS_AUX_IRQ_SELECT);
		__builtin_arc_sr(QM_SS_INT_PRIORITY_1, QM_SS_AUX_IRQ_PRIORITY);
		__builtin_arc_sr(QM_SS_INT_DISABLE, QM_SS_AUX_IRQ_ENABLE);

		__ivt_vect_table[i] = (fp)irq_dummy_routine;
	}

	/* Setting an interrupt priority threshold */
	reg = __builtin_arc_lr(QM_SS_AUX_STATUS32);
	reg = (reg & ~(0xf << 1)) | (15 << 1);

	/* This one has to be a kernel operation */
	__builtin_arc_kflag(reg);

	/* Enable instruction cache */
	reg = __builtin_arc_lr(QM_SS_AUX_IC_CTRL);
	reg &= ~BIT(0);
	__builtin_arc_sr(reg, QM_SS_AUX_IC_CTRL);

	/*
	 * Interrupt Context Saving Control Register, AUX_IRQ_CTRL
	 * NR [4:0] Indicates number of general-purpose register pairs saved.
	 * B[9]     Indicates whether to save and restore BLINK
	 * L[10]    Indicates whether to save and restore loop registers
	 *          (LP_COUNT, LP_START,LP_END )
	 * U[11]    Indicates if user context is saved to user stack
	 * LP[13]   Indicates whether to save and restore code-density registers
	 *          (EI_BASE, JLI_BASE, LDI_BASE)
	 */
	__builtin_arc_sr(0x1f | BIT(9) | BIT(10) | BIT(13), QM_SS_AUX_IRQ_CTRL);

	/* Enable global interrupts. */
	__builtin_arc_seti(0);
}

/*
 * Setup Sensor Subsystem TimeStamp Counter
 * ARC EM 32 bit processors do not provide a native timestamp counter,
 * so here we repurpose the core timer1 to monotonically count up.
 */
static void sensor_tsc_init(void)
{
	__builtin_arc_sr(0, QM_SS_TSC_BASE + QM_SS_TIMER_CONTROL);
	__builtin_arc_sr(0, QM_SS_TSC_BASE + QM_SS_TIMER_COUNT);
	__builtin_arc_sr(0xffffffff, QM_SS_TSC_BASE + QM_SS_TIMER_LIMIT);
}

void sensor_init(void)
{
	/* Initialize vectors to something meaningful  */
	sensor_vectors_init();
	/* Start timerstamp counter */
	sensor_tsc_init();
}
