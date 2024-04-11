/**
 * \file
 *
 * \brief Chip-specific PLL management functions
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef XMEGA_PLL_H_INCLUDED
#define XMEGA_PLL_H_INCLUDED

#include <compiler.h>

/**
 * \weakgroup pll_group
 * @{
 */

#define NR_PLLS         1
#define PLL_MIN_HZ      10000000UL
#define PLL_MAX_HZ      200000000UL
#define PLL_NR_OPTIONS  0

enum pll_source {
	//! 2 MHz Internal RC Oscillator
	PLL_SRC_RC2MHZ     = OSC_PLLSRC_RC2M_gc,
	//! 32 MHz Internal RC Oscillator
	PLL_SRC_RC32MHZ    = OSC_PLLSRC_RC32M_gc,
	//! External Clock Source
	PLL_SRC_XOSC       = OSC_PLLSRC_XOSC_gc,
};

#define pll_get_default_rate(pll_id)                              \
	pll_get_default_rate_priv(CONFIG_PLL##pll_id##_SOURCE,    \
			CONFIG_PLL##pll_id##_MUL,                 \
			CONFIG_PLL##pll_id##_DIV)

/**
 * \internal
 * \brief Return clock rate for specified PLL settings.
 *
 * \note Due to the hardware implementation of the PLL, \a div must be 4 if the
 * 32 MHz RC oscillator is used as reference and 1 otherwise. The reference must
 * be above 440 kHz, and the output between 10 and 200 MHz.
 *
 * \param src ID of the PLL's reference source oscillator.
 * \param mul Multiplier for the PLL.
 * \param div Divisor for the PLL.
 *
 * \retval Output clock rate from PLL.
 */
static inline uint32_t pll_get_default_rate_priv(enum pll_source src,
		unsigned int mul, unsigned int div)
{
	uint32_t rate;

	switch (src) {
	case PLL_SRC_RC2MHZ:
		rate = 2000000UL;
		Assert(div == 1);
		break;

	case PLL_SRC_RC32MHZ:
#ifdef CONFIG_OSC_RC32_CAL //32MHz oscillator is calibrated to another frequency
		rate = CONFIG_OSC_RC32_CAL / 4;
#else
		rate = 8000000UL;
 #endif
		Assert(div == 4);
		break;

	case PLL_SRC_XOSC:
		rate = osc_get_rate(OSC_ID_XOSC);
		Assert(div == 1);
		break;

	default:
		break;
	}

	Assert(rate >= 440000UL);

	rate *= mul;

	Assert(rate >= PLL_MIN_HZ);
	Assert(rate <= PLL_MAX_HZ);

	return rate;
}

struct pll_config {
	uint8_t ctrl;
};

/**
 * \note The XMEGA PLL hardware uses hard-wired input dividers, so the
 * user must ensure that \a div is set as follows:
 *   - If \a src is PLL_SRC_32MHZ, \a div must be set to 4.
 *   - Otherwise, \a div must be set to 1.
 */
static inline void pll_config_init(struct pll_config *cfg, enum pll_source src,
		unsigned int div, unsigned int mul)
{
	Assert(mul >= 1 && mul <= 31);

	if (src == PLL_SRC_RC32MHZ) {
		Assert(div == 4);
	} else {
		Assert(div == 1);
	}

	/* Initialize the configuration */
	cfg->ctrl = src | (mul << OSC_PLLFAC_gp);
}

#define pll_config_defaults(cfg, pll_id)                                \
	pll_config_init(cfg,                                            \
			CONFIG_PLL##pll_id##_SOURCE,                    \
			CONFIG_PLL##pll_id##_DIV,                       \
			CONFIG_PLL##pll_id##_MUL)

static inline void pll_config_read(struct pll_config *cfg, unsigned int pll_id)
{
	Assert(pll_id < NR_PLLS);

	cfg->ctrl = OSC.PLLCTRL;
}

static inline void pll_config_write(const struct pll_config *cfg,
		unsigned int pll_id)
{
	Assert(pll_id < NR_PLLS);

	OSC.PLLCTRL = cfg->ctrl;
}

/**
 * \note If a different PLL reference oscillator than those enabled by
 * \ref sysclk_init() is used, the user must ensure that the desired reference
 * is enabled prior to calling this function.
 */
static inline void pll_enable(const struct pll_config *cfg,
		unsigned int pll_id)
{
	irqflags_t flags;

	Assert(pll_id < NR_PLLS);

	flags = cpu_irq_save();
	pll_config_write(cfg, pll_id);
	OSC.CTRL |= OSC_PLLEN_bm;
	cpu_irq_restore(flags);
}

/*! \note This will not automatically disable the reference oscillator that is
 * configured for the PLL.
 */
static inline void pll_disable(unsigned int pll_id)
{
	irqflags_t flags;

	Assert(pll_id < NR_PLLS);

	flags = cpu_irq_save();
	OSC.CTRL &= ~OSC_PLLEN_bm;
	cpu_irq_restore(flags);
}

static inline bool pll_is_locked(unsigned int pll_id)
{
	Assert(pll_id < NR_PLLS);

	return OSC.STATUS & OSC_PLLRDY_bm;
}

static inline void pll_enable_source(enum pll_source src)
{
	switch (src) {
	case PLL_SRC_RC2MHZ:
		break;

	case PLL_SRC_RC32MHZ:
		if (!osc_is_ready(OSC_ID_RC32MHZ)) {
			osc_enable(OSC_ID_RC32MHZ);
			osc_wait_ready(OSC_ID_RC32MHZ);
		}
		break;

	case PLL_SRC_XOSC:
		if (!osc_is_ready(OSC_ID_XOSC)) {
			osc_enable(OSC_ID_XOSC);
			osc_wait_ready(OSC_ID_XOSC);
		}
		break;
	default:
		Assert(false);
		break;
	}
}

static inline void pll_enable_config_defaults(unsigned int pll_id)
{
	struct pll_config pllcfg;

	if (pll_is_locked(pll_id)) {
		return; // Pll already running
	}
	switch (pll_id) {
#ifdef CONFIG_PLL0_SOURCE
	case 0:
		pll_enable_source(CONFIG_PLL0_SOURCE);
		pll_config_init(&pllcfg,
				CONFIG_PLL0_SOURCE,
				CONFIG_PLL0_DIV,
				CONFIG_PLL0_MUL);
		break;
#endif
	default:
		Assert(false);
		break;
	}
	pll_enable(&pllcfg, pll_id);
	while (!pll_is_locked(pll_id));
}

//! @}

#endif /* XMEGA_PLL_H_INCLUDED */
