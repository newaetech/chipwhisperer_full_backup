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
 * Interrupt Request (IRQ) Lock / Unlock
 *
 * This app prints a message, then runs a few tests with conditional
 * IRQ locking / unlocking mechanism and return. A failure or success
 * message will be printed in the end accordingly.
 */

#include "qm_interrupt.h"

/*
 * x86 CPU FLAGS.IF register field (Interrupt enable Flag, bit 9), indicating
 * whether or not CPU interrupts are enabled.
 */
#define X86_FLAGS_IF BIT(9)

int main(void)
{
	QM_PUTS("Starting: IRQ lock / unlock");

	int key = 0;

	/* Disable interrupts unconditionally. */
	qm_irq_disable();

	/*
	 * Lock interrupts and check if the Interrupt Flag bit from EFLAGS
	 * was cleared.
	 */
	key = qm_irq_lock();
	if (key & X86_FLAGS_IF) {
		QM_PUTS("Error: IF was set");
		return -EIO;
	}

	/* Try to unlock, lock again, but check that IF was still cleared. */
	qm_irq_unlock(key);
	key = 0;
	key = qm_irq_lock();
	if (key & X86_FLAGS_IF) {
		QM_PUTS("Error: IF was set");
		return -EIO;
	}

	/* Enable interrupts unconditionally. */
	qm_irq_enable();

	/* Lock them and check if IF was set. */
	key = qm_irq_lock();
	if (!(key & X86_FLAGS_IF)) {
		QM_PUTS("Error: IF was cleared");
		return -EIO;
	}

	/* Try to unlock, lock again, and check that IF is still set. */
	qm_irq_unlock(key);
	key = 0;
	key = qm_irq_lock();
	if (!(key & X86_FLAGS_IF)) {
		QM_PUTS("Error: IF was cleared");
		return -EIO;
	}

	QM_PUTS("Finished: IRQ lock / unlock");
	return 0;
}
