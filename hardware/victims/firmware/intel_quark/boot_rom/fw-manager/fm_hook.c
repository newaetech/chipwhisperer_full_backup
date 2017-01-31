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

#include "clk.h"

#include "qm_soc_regs.h"
#include "fw-manager.h"
#include "fm_entry.h"
#include "bl_data.h"
#include "qm_gpio.h"
#include "qm_pinmux.h"

/* Flash configuration defines, valid when we are running at 32MHz */
#define FLASH_US_COUNT (0x20)
#define FLASH_WAIT_STATES (0x01)

/** Flash configuration for writing to bl-data and QFU images. */
static const qm_flash_config_t cfg_wr = {
    .us_count = SYS_TICKS_PER_US_32MHZ / BIT(CLK_SYS_DIV_1),
    .wait_states = FLASH_WAIT_STATES,
    .write_disable = QM_FLASH_WRITE_ENABLE,
};

void fm_hook()
{
	/*
	 * Initialize flash for writing, since this may be needed by
	 * bl_data_sanitize() and the FM mode (in entered).
	 *
	 * TODO: consider moving flash init and bl-data sanitizing back to rom
	 * startup code and combine it with the trim-code setup code (which is
	 * also initializing the flash controller).
	 */
	qm_flash_set_config(QM_FLASH_0, &cfg_wr);
#if (QUARK_SE)
	qm_flash_set_config(QM_FLASH_1, &cfg_wr);
#endif
	bl_data_sanitize();

	/*
	 * Get FM pin status.
	 *
	 * We support both regular GPIO and always-on GPIO. However, they
	 * must be handled differently:
	 * - For AON-GPIO we cannot assume a default configuration since in case
	 *   of warm resets the configuration is not re-initialized
	 *   automatically;
	 * - For regular GPIO, we can rely on their default configuration, but
	 *   we have to handle the pin mixing.
	 */
	qm_gpio_state_t state;

	clk_periph_enable(CLK_PERIPH_REGISTER | CLK_PERIPH_CLK |
			  CLK_PERIPH_GPIO_REGISTER);
#if (FM_CONFIG_USE_AON_GPIO_PORT)
	qm_gpio_reg_t *const gpio_ctrl = QM_GPIO[QM_AON_GPIO_0];
	gpio_ctrl->gpio_inten &= ~BIT(FM_CONFIG_GPIO_PIN);
	gpio_ctrl->gpio_swporta_ddr &= ~BIT(FM_CONFIG_GPIO_PIN);
	qm_gpio_read_pin(QM_AON_GPIO_0, FM_CONFIG_GPIO_PIN, &state);
#else /* FM GPIO is a regular GPIO */
	qm_pmux_select(FM_CONFIG_GPIO_PIN, QM_PMUX_FN_0);
	qm_pmux_pullup_en(FM_CONFIG_GPIO_PIN, true);
	qm_pmux_input_en(FM_CONFIG_GPIO_PIN, true);
	/* No need to configure the GPIO, default configuration is okay. */
	qm_gpio_read_pin(QM_GPIO_0, FM_CONFIG_GPIO_PIN, &state);
	qm_pmux_pullup_en(FM_CONFIG_GPIO_PIN, false);
#endif
	/* Enter FM mode if FM sticky bit is set or FM_CONFIG_GPIO_PIN is low */
	if (FM_STICKY_BITCHK() || (state == QM_GPIO_LOW)) {
		FM_STICKY_BITCLR();
		/* run the firmware management code; fm_main() never returns */
		fm_entry();
	}
}
