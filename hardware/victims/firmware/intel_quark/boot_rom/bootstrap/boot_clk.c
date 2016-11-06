/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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

#include <string.h>
#include "boot_clk.h"
#include "qm_flash.h"
#include "flash_layout.h"
#include <x86intrin.h>

/* 64bit Timestamp counter */
#define get_ticks() _rdtsc()

/*
 * Oscillator trim data calculation.
 * Ensure RTC clock and AON counter are enabled and hybrid oscillator
 * is in silicon mode before calling this function.
 * Calibration period (using RTC): ~3ms. (100 * (1 / 32768) = 0.00305176)
 */
#define OSC_TRIM_PERIOD_RTC_TICKS (100)
#define OSC_TRIM_MSB (9)

uint32_t flash_page_buffer[QM_FLASH_PAGE_SIZE_DWORDS];

/* Desired timestamp in sysclk ticks.  Relative to RTC period and sysclk
 * divider.
 */
#define SYSCLK_32M_FREQ ((32000000 / 2 / 32768) * OSC_TRIM_PERIOD_RTC_TICKS)
#define SYSCLK_16M_FREQ ((16000000 / 2 / 32768) * OSC_TRIM_PERIOD_RTC_TICKS)
#define SYSCLK_8M_FREQ ((8000000 / 2 / 32768) * OSC_TRIM_PERIOD_RTC_TICKS)
#define SYSCLK_4M_FREQ ((4000000 / 2 / 32768) * OSC_TRIM_PERIOD_RTC_TICKS)

#define AONC_CFG_AONC_CNT_EN BIT(0)

#if (HAS_RTC_XTAL)
/**
 * Compute the silicon oscillator trim code.
 *
 * Compute and apply the silicon oscillator trim code for the current frequency.
 * The system clock must be set to hybrid oscillator in silicon mode.
 *
 * Note the trim code computation algorithm makes use of an AON counter and it
 * therefore requires that both the RTC clock and the AON counter be enabled.
 *
 * @param[in] mode System clock source operating mode.
 *
 * @return Resulting status code.
 * @retval 0 if successful.
 */
static int boot_clk_trim_compute(clk_sys_mode_t mode, uint16_t *const trim)
{
	uint64_t ts_start, ts_stop, ts_elapsed;
	int i;
	int rc = 0;
	uint32_t trim_code = 0;
	volatile uint32_t aonc_start;
	const uint32_t ts_desired[] = {
	    SYSCLK_32M_FREQ, SYSCLK_16M_FREQ, SYSCLK_8M_FREQ, SYSCLK_4M_FREQ,
	};

	/* Enable AON counter */
	QM_AONC[QM_AONC_0].aonc_cfg |= AONC_CFG_AONC_CNT_EN;

	rc = boot_clk_hyb_set_mode(mode, CLK_SYS_DIV_2);
	QM_CHECK(rc == 0, rc);

	/* Enable trim mode */
	QM_SCSS_CCU->osc0_cfg0 |= BIT(1);

	/* Trim code calculation algorithm */
	/* First iteration, select "midpoint" */
	for (i = OSC_TRIM_MSB; i >= 0; i--) {
		trim_code |= BIT(i);

		/* Apply trim code */
		QM_SCSS_CCU->osc0_cfg1 &= ~OSC0_CFG1_FTRIMOTP_MASK;
		QM_SCSS_CCU->osc0_cfg1 |=
		    (trim_code << OSC0_CFG1_FTRIMOTP_OFFS) &
		    OSC0_CFG1_FTRIMOTP_MASK;

		/* Wait one RTC tick so as to eliminate any time
		 * inconsistencies between clock domains */
		aonc_start = QM_AONC->aonc_cnt;
		while (QM_AONC->aonc_cnt == aonc_start) {
		}

		/* Start calibration period */
		aonc_start = QM_AONC->aonc_cnt;
		ts_start = get_ticks();

		while (QM_AONC->aonc_cnt - aonc_start <
		       OSC_TRIM_PERIOD_RTC_TICKS) {
		}
		ts_stop = get_ticks();

		/* calculate the number of elapsed timestamp ticks */
		ts_elapsed = ts_stop - ts_start;

		if (ts_elapsed > ts_desired[mode]) {
			/* Clock is too fast */
			trim_code &= ~BIT(i);
		}
	}

	*trim = trim_code;

	/* Disable trim mode */
	QM_SCSS_CCU->osc0_cfg0 &= ~BIT(1);

	return rc;
}
#endif /* HAS_RTC_XTAL */

/**
 * Store trim code in flash.
 *
 * Write trim codes to the defined flash shadow region.
 * Each trim code is stored as a uint16_t.
 *
 * @param[in] trim_codes Trim codes to store.
 *
 * @return Resulting status code.
 * @retval 0 if successful.
 */
static int boot_clk_trim_code_store(qm_flash_data_trim_t *trim_codes)
{
	int rc = 0;
	static const qm_flash_config_t cfg = {
	    .us_count = SYS_TICKS_PER_US_32MHZ / BIT(CLK_SYS_DIV_1),
	    .wait_states = 0x1,
	    .write_disable = QM_FLASH_WRITE_ENABLE};

	/* Set flash configuration.
	 * Keep default wait_states.
	 * Set the number of clocks in a micro second
	 * for the current frequency.
	 * Current frequency is 32MHZ as defined by boot_clk_trim_code_setup().
	 */
	rc = qm_flash_set_config(QM_FLASH_0, &cfg);
	QM_CHECK(rc == 0, rc);

	rc = qm_flash_page_update(QM_FLASH_0, QM_FLASH_DATA_TRIM_REGION,
				  QM_FLASH_DATA_TRIM_OFFSET, flash_page_buffer,
				  (uint32_t *)trim_codes->osc_trim_u32,
				  sizeof(trim_codes->osc_trim_u32) /
				      sizeof(trim_codes->osc_trim_u32[0]));

	return rc;
}

/*
 * Get trim code to store in flash.
 *
 * Trim codes are copied from the manufacturing data region
 * if provisioned.
 * If a trim code is not provisioned, it is computed.
 * The trim code to be shadowed is returned.
 *
 * @param[in] mode System clock source operating mode.
 * @param[out] trim Returned trim code value.
 *
 * @return Resulting status code.
 * @retval 0 if successful.
 */
static int boot_clk_trim_code_get(const clk_sys_mode_t mode, uint16_t *trim)
{
	int rc = 0;

	/*
	 * Check first if the trim code is available in the OTP.
	 * The OTP trim code is used if valid.
	 * The presence field need to be set for a trim code to be valid.
	 * For Quark SE, the magic code is checked as well.
	 * If not provisioned in the OTP, the trim code is computed.
	 */
	if ((QM_FLASH_OTP_TRIM_MAGIC != QM_FLASH_OTP_SOC_DATA_VALID) ||
	    ((*trim & QM_FLASH_TRIM_PRESENT_MASK) != QM_FLASH_TRIM_PRESENT)) {
#if (HAS_RTC_XTAL)
		rc = boot_clk_trim_compute(mode, trim);
		QM_CHECK(rc == 0, rc);
#else
		(void)mode;
		*trim &= ~QM_FLASH_TRIM_PRESENT_MASK;
		return -EINVAL;
#endif /* HAS_RTC_XTAL */
	}

	*trim &= ~QM_FLASH_TRIM_PRESENT_MASK;
	*trim |= QM_FLASH_TRIM_PRESENT;

	return rc;
}

int boot_clk_hyb_set_mode(const clk_sys_mode_t mode, const clk_sys_div_t div)
{
	QM_CHECK(div < CLK_SYS_DIV_NUM, -EINVAL);
	QM_CHECK(mode <= CLK_SYS_HYB_OSC_4MHZ, -EINVAL);

	/*
	 * Get current settings, clear the clock divisor bits, and clock divider
	 * enable bit.
	 */
	uint32_t ccu_sys_clk_ctl =
	    QM_SCSS_CCU->ccu_sys_clk_ctl & CLK_SYS_CLK_DIV_DEF_MASK;

	/* Select the silicon oscillator frequency */
	QM_SCSS_CCU->osc0_cfg1 &= ~OSC0_CFG1_SI_FREQ_SEL_MASK;
	QM_SCSS_CCU->osc0_cfg1 |= (mode << OSC0_CFG1_SI_FREQ_SEL_OFFS);
	/* Enable the silicon oscillator */
	QM_SCSS_CCU->osc0_cfg1 |= QM_OSC0_EN_SI_OSC;
	/* Wait for the oscillator to lock */
	while (!(QM_SCSS_CCU->osc0_stat1 & QM_OSC0_LOCK_SI)) {
	};
	/* Switch to silicon oscillator mode */
	QM_SCSS_CCU->osc0_cfg1 &= ~QM_OSC0_MODE_SEL;
	/* Set the system clock divider */
	QM_SCSS_CCU->ccu_sys_clk_ctl = ccu_sys_clk_ctl | QM_CCU_SYS_CLK_SEL |
				       (div << QM_CCU_SYS_CLK_DIV_OFFSET);

	QM_SCSS_CCU->ccu_sys_clk_ctl |= QM_CCU_SYS_CLK_DIV_EN;

	return 0;
}

int boot_clk_trim_code_setup(void)
{
	int rc = 0;
	clk_sys_mode_t mode;
	qm_flash_data_trim_t trim_codes;

	(trim_codes.fields).osc_trim_4mhz =
	    QM_FLASH_OTP_TRIM_CODE->osc_trim_4mhz;

	(trim_codes.fields).osc_trim_8mhz =
	    QM_FLASH_OTP_TRIM_CODE->osc_trim_8mhz;

	(trim_codes.fields).osc_trim_16mhz =
	    QM_FLASH_OTP_TRIM_CODE->osc_trim_16mhz;

	(trim_codes.fields).osc_trim_32mhz =
	    QM_FLASH_OTP_TRIM_CODE->osc_trim_32mhz;

	for (mode = CLK_SYS_HYB_OSC_32MHZ; mode < CLK_SYS_RTC_OSC; mode++) {
		rc = boot_clk_trim_code_get(
		    mode, (uint16_t *)&trim_codes.osc_trim_u16[mode]);
	}
	boot_clk_hyb_set_mode(CLK_SYS_HYB_OSC_32MHZ, CLK_SYS_DIV_1);
	rc = boot_clk_trim_code_store(&trim_codes);

	return rc;
}
