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
 * Power SoC Restore app example.
 *
 * This app requires a comparator to wake the SoC from deep sleep. RTC will be
 * explicitly disabled in deep sleep and WDT is disabled in C2. Disabling the
 * RTC is not required but this is done to showcase comparator wake up when in
 * deep sleep mode.
 *
 * On the Intel(R) Quark(TM) SE development platform the comparator pin is
 * located on header J14 pin 20.
 *
 * Wake up from SoC sleep and SoC deep sleep states result in a processor core
 * reset. The execution context of the processor is restored by the bootloader.
 * Peripherals used by the application are saved and restored on demand.
 * This application uses UART and IRQs which are the two only components
 * saved and restored.
 * rtc and comparator being only used as a wake source, these are not saved.
 *
 * States executed in this example are:
 * SLEEP: Core voltage rail off, peripherals off except Always On.
 * DEEP_SLEEP: Core voltage rail off,
 *             1P8V voltage regulator in linear mode,
 *             3P3V voltage regulator in linear mode,
 *             peripherals off except Always On.
 */

#if (ENABLE_RESTORE_CONTEXT)
#include "power_soc_common.h"
#include "power_states.h"
#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_uart.h"

#if (QM_SENSOR)
#include "ss_power_states.h"
#endif

#define QM_AC_COMPARATORS_MASK (0x7FFFF)

static void app_sleep()
{
	qm_uart_context_t uart_ctx;
	qm_irq_context_t irq_ctx;

	setup_rtc_alarm();

	QM_PUTS("Go to sleep.");

	qm_uart_save_context(STDOUT_UART, &uart_ctx);
	qm_irq_save_context(&irq_ctx);

#if (QM_SENSOR)
	/* Go to sleep, RTC will wake me up. */
	ss_power_soc_sleep_restore();
#else
	/* Go to sleep, RTC will wake me up. */
	power_soc_sleep_restore();
#endif

	/*
	 * Interrupts are not cleared when waking up from sleep.
	 * The application needs to check which wake event triggered
	 * and react to it appropriately.
	 * In our case, we simply clear the interrupt.
	 */
	QM_RTC[QM_RTC_0].rtc_eoi;
	QM_RTC[QM_RTC_0].rtc_ccr &= ~QM_RTC_CCR_INTERRUPT_ENABLE;

	qm_irq_restore_context(&irq_ctx);
	qm_uart_restore_context(STDOUT_UART, &uart_ctx);
}

static void app_deep_sleep()
{
	qm_uart_context_t uart_ctx;
	qm_irq_context_t irq_ctx;

	setup_aon_comparator();

	QM_PUTS("Go to deep sleep. Trigger comparator to wake up.");

	qm_uart_save_context(STDOUT_UART, &uart_ctx);
	qm_irq_save_context(&irq_ctx);

#if (QM_SENSOR)
	/* Go to deep sleep, Comparator will wake me up. */
	ss_power_soc_deep_sleep_restore();
#else
	/* Go to deep sleep, Comparator will wake me up. */
	power_soc_deep_sleep_restore();
#endif

	/*
	 * Interrupts are not cleared when waking up from sleep.
	 * The application needs to check which wake event triggered
	 * and react to it appropriately.
	 * In our case, we simply clear the interrupt.
	 */
	QM_SCSS_CMP->cmp_pwr = 0;
	QM_SCSS_CMP->cmp_stat_clr = QM_AC_COMPARATORS_MASK;

	qm_irq_restore_context(&irq_ctx);
	qm_uart_restore_context(STDOUT_UART, &uart_ctx);
}

void power_soc_restore(void)
{
	QM_PUTS("Starting: Power SoC");

	QM_PUTS("Press PB0 to go to sleep.");
	wait_user_ready();
	app_sleep();
	QM_PUTS("SoC states example back from sleep.");

	QM_PUTS("Set comparator pin to Ground and press PB0 when ready.");
	wait_user_ready();
	app_deep_sleep();
	QM_PUTS("SoC states example back from deep sleep.");

	QM_PUTS("Finished: Power SoC");
}
#endif /* ENABLE_RESTORE_CONTEXT */
