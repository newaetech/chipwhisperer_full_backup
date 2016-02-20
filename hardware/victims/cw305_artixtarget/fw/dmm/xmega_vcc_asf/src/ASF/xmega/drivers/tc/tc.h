/**
 * \file
 *
 * \brief AVR XMEGA Timer Counter (TC) driver
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef _TC_H_
#define _TC_H_

#include <compiler.h>
#include <parts.h>
#include "status_codes.h"
#include "pmic.h"
#include <sleepmgr.h>
#include <sysclk.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * \defgroup tc_group Timer Counter (TC)
 *
 * See \ref xmega_tc_quickstart
 *
 * This is a driver for the AVR XMEGA Timer Counter (TC). It provides functions
 * for enabling, disabling and configuring the TC modules.
 *
 * \section dependencies Dependencies
 * This driver depends on the following modules:
 * - \ref sysclk_group for peripheral clock control.
 * - \ref sleepmgr_group for setting allowed sleep mode.
 * - \ref interrupt_group for ISR definition and disabling interrupts during
 * critical code sections.
 * @{
 */



/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the tc_callback_t type.
 *
 */
typedef void (*tc_callback_t) (void);

//! Timer Counter Capture Compare Channel index
enum tc_cc_channel_t {
	//! Channel A
	TC_CCA = 1,
	//! Channel B
	TC_CCB = 2,
	//! Channel C
	TC_CCC = 3,
	//! Channel D
	TC_CCD = 4,
};

//! Timer Counter Capture Compare Channel index
enum tc_cc_channel_mask_enable_t {
	//! Channel A Enable mask
	TC_CCAEN = TC0_CCAEN_bm,
	//! Channel B Enable mask
	TC_CCBEN = TC0_CCBEN_bm,
	//! Channel C Enable mask
	TC_CCCEN = TC0_CCCEN_bm,
	//! Channel D Enable mask
	TC_CCDEN = TC0_CCDEN_bm,
};

//! Timer Counter Direction
enum tc_dir_t {
	//! Counting up
	TC_UP = 0,
	//! Down Counting B
	TC_DOWN = 1
};
//! Timer Counter Waveform Generator mode
enum tc_wg_mode_t {
	//! TC in normal Mode
	TC_WG_NORMAL = TC_WGMODE_NORMAL_gc,
	//! TC in Frequency Generator mode
	TC_WG_FRQ = TC_WGMODE_FRQ_gc,
	//! TC in single slope PWM mode
	TC_WG_SS = TC_WGMODE_SS_gc,
	//! TC in dual slope Top PWM mode
	TC_WG_DS_T = TC_WGMODE_DS_T_gc,
	//! TC in dual slope Top Bottom PWM mode
	TC_WG_DS_TB = TC_WGMODE_DS_TB_gc,
	//! TC in dual slope Bottom PWM mode
	TC_WG_DS_B = TC_WGMODE_DS_B_gc
};

//! TC interrupt levels
enum TC_INT_LEVEL_t {
	TC_INT_LVL_OFF = 0x00,
	TC_INT_LVL_LO = 0x01,
	TC_INT_LVL_MED = 0x02,
	TC_INT_LVL_HI = 0x03,
};

//! Macro to check if type of passed TC is TC1_t
#define tc_is_tc1(void) ((uint16_t)tc&0x40 ? true : false)
//! Macro to check if type of passed TC is TC0_t
#define tc_is_tc0(void) ((uint16_t)tc&0x40 ? false : true)

/**
 * \brief Enable TC
 *
 * Enables the TC.
 *
 * \param tc Pointer to TC module
 *
 * \note
 * unmask TC clock (sysclk), but does not configure the TC clock source.
 */
void tc_enable(volatile void *tc);

/**
 * \brief Disable TC
 *
 * Disables the TC.
 *
 * \param tc Pointer to TC module
 *
 * \note
 * mask TC clock (sysclk).
 */
void tc_disable(volatile void *tc);

/**
 * \ingroup tc_group
 * \defgroup tc_interrupt_group Timer Counter (TC) interrupt management
 * This group provides functions to configure TC module interrupts
 *
 *
 * @{
 */
/**
 * \brief Set TC overflow interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_overflow_interrupt_level() for interrupts to be
 *       generated each time the timer overflows.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_overflow_interrupt_callback(volatile void *tc,
		tc_callback_t callback);

/**
 * \brief Set TC error interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_error_interrupt_level() for interrupts to be
 *       generated each time a timer error occurs.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_error_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * \brief Set TC Capture Compare Channel A interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_cca_interrupt_level() for interrupts to be generated
 *       each time the timer channel A compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_cca_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * \brief Set TC Capture Compare Channel B interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_ccb_interrupt_level() for interrupts to be generated
 *       each time the timer channel B compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_ccb_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * \brief Set TC Capture Compare Channel C interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_ccc_interrupt_level() for interrupts to be generated
 *       each time the timer channel C compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_ccc_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * \brief Set TC Capture Compare Channel D interrupt callback function
 *
 * This function allows the caller to set and change the interrupt callback
 * function. Without setting a callback function the interrupt handler in the
 * driver will only clear the interrupt flags.
 *
 * \note Once a callback function is set, the interrupt priority must be set
 *       via \ref tc_set_ccd_interrupt_level() for interrupts to be generated
 *       each time the timer channel D compare matches the current timer count.
 *
 * \param tc Pointer to the Timer Counter (TC) base address
 * \param callback Reference to a callback function
 */
void tc_set_ccd_interrupt_callback(volatile void *tc, tc_callback_t callback);

/**
 * \brief Configures TC overflow Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level Overflow interrupt level
 * \note  Configures OVFINTLVL in INTCTRLA
 */
static inline void tc_set_overflow_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLA = ((TC0_t *)tc)->INTCTRLA & ~TC0_OVFINTLVL_gm;
	((TC0_t *)tc)->INTCTRLA =
			((TC0_t *)tc)->INTCTRLA | (level << TC0_OVFINTLVL_gp);
}

/**
 * \brief Configures TC error Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level Error interrupt level
 * \note  Configures ERRINTLVL in INTCTRLA
 */
static inline void tc_set_error_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLA = ((TC0_t *)tc)->INTCTRLA & ~TC0_ERRINTLVL_gm;
	((TC0_t *)tc)->INTCTRLA =
			((TC0_t *)tc)->INTCTRLA | (level << TC0_ERRINTLVL_gp);
}

/**
 * \brief Configures TC Capture Compare A Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level CCA interrupt level
 * \note Configures CCAINTLVL in INTCTRLB
 */
static inline void tc_set_cca_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLB = ((TC0_t *)tc)->INTCTRLB & ~TC0_CCAINTLVL_gm;
	((TC0_t *)tc)->INTCTRLB =
			((TC0_t *)tc)->INTCTRLB | (level << TC0_CCAINTLVL_gp);
}

/**
 * \brief Configures TC Capture Compare B Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level CCB interrupt level
 * \note Configures CCBINTLVL in INTCTRLB
 */
static inline void tc_set_ccb_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLB = ((TC0_t *)tc)->INTCTRLB & ~TC0_CCBINTLVL_gm;
	((TC0_t *)tc)->INTCTRLB =
			((TC0_t *)tc)->INTCTRLB | (level << TC0_CCBINTLVL_gp);
}

/**
 * \brief Configures TC Capture Compare C Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level CCC interrupt level
 * \note Configures CCCINTLVL in INTCTRLB
 */
static inline void tc_set_ccc_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLB = ((TC0_t *)tc)->INTCTRLB & ~TC0_CCCINTLVL_gm;
	((TC0_t *)tc)->INTCTRLB =
			((TC0_t *)tc)->INTCTRLB | (level << TC0_CCCINTLVL_gp);
}

  /**
 * \brief Configures TC Capture Compare D Interrupt level
 *
 * \param tc Pointer to TC module.
 * \param level CCD interrupt level
 * \note Configures CCDINTLVL in INTCTRLB
 */
static inline void tc_set_ccd_interrupt_level(volatile void *tc,
		enum TC_INT_LEVEL_t level)
{
	((TC0_t *)tc)->INTCTRLB = ((TC0_t *)tc)->INTCTRLB & ~TC0_CCDINTLVL_gm;
	((TC0_t *)tc)->INTCTRLB =
			((TC0_t *)tc)->INTCTRLB | (level << TC0_CCDINTLVL_gp);
}

//@}

/**
 * \brief Configure Timer Clock Source
 *
 * \param tc Pointer to TC module.
 * \param TC_CLKSEL_enum Clock source selection
 * \note Configuring the clock also starts the timer
 */
static inline void tc_write_clock_source(volatile void *tc,
		TC_CLKSEL_t TC_CLKSEL_enum)
{
	((TC0_t *)tc)->CTRLA =
			(((TC0_t *)tc)->CTRLA & ~TC0_CLKSEL_gm) |
			TC_CLKSEL_enum;
}

/**
 * \brief Read Timer Clock Source
 *
 * \param tc Pointer to TC module.
 * \return TC_CLKSEL_enum Clock source selection
 */
static inline TC_CLKSEL_t tc_read_clock_source(volatile void *tc)
{
	return (TC_CLKSEL_t)(((TC0_t *)tc)->CTRLA & TC0_CLKSEL_gm);
}

/**
 * \brief Select clock for a specified TC and resolution.
 *
 * This function configures the clock selection, as prescaled CLKper, for a
 * specified TC that gives a resolution at least as high as the one specified.
 * The resolution of a TC is synonymous with its clock frequency.
 *
 * \note It is also possible to clock TCs with event channels. This is not
 * handled by this implementation.
 *
 * \param tc   ID of TC to get clock selection for.
 * \param resolution Desired resolution for the TC in Hz.
 */
static inline void tc_set_resolution(volatile void *tc, uint32_t resolution)
{
	uint32_t tc_clk_rate = sysclk_get_per_hz();

	if (resolution <= (tc_clk_rate / 1024)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV1024_gc);
	} else if (resolution <= (tc_clk_rate / 256)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV256_gc);
	} else if (resolution <= (tc_clk_rate / 64)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV64_gc);
	} else if (resolution <= (tc_clk_rate / 8)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV8_gc);
	} else if (resolution <= (tc_clk_rate / 4)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV4_gc);
	} else if (resolution <= (tc_clk_rate / 2)) {
		tc_write_clock_source(tc, TC_CLKSEL_DIV2_gc);
	} else {
		tc_write_clock_source(tc, TC_CLKSEL_DIV1_gc);
	}
}

/**
 * \brief Get real resolution for a specified TC.
 *
 * This function returns the resolution which the specified clock selection
 * of TC will result in. The resolution of a TC is synonymous with its clock
 * frequency.
 *
 * \note This function does not handle event channel clock selections.
 *
 * \param tc Pointer of TC module to get resolution for.
 *
 * \return The resolution of \a tc.
 */
static inline uint32_t tc_get_resolution(volatile void *tc)
{
	uint32_t tc_clk_rate = sysclk_get_per_hz();
	switch (tc_read_clock_source(tc)) {
	case TC_CLKSEL_OFF_gc:
		tc_clk_rate = 0;
		break;

	case TC_CLKSEL_DIV1024_gc:
		tc_clk_rate /= 1024;
		break;

	case TC_CLKSEL_DIV256_gc:
		tc_clk_rate /= 256;
		break;

	case TC_CLKSEL_DIV64_gc:
		tc_clk_rate /= 64;
		break;

	case TC_CLKSEL_DIV8_gc:
		tc_clk_rate /= 8;
		break;

	case TC_CLKSEL_DIV4_gc:
		tc_clk_rate /= 4;
		break;

	case TC_CLKSEL_DIV2_gc:
		tc_clk_rate /= 2;
		break;

	case TC_CLKSEL_DIV1_gc:
		break;

	default:
		tc_clk_rate = 0;
		break;
	}
	return (tc_clk_rate);
}

/**
 * \brief Configure Timer Direction
 *
 * \param tc Pointer to TC module.
 * \param dir Timer direction :
 */
static inline void tc_set_direction(volatile void *tc, enum tc_dir_t dir)
{
	if (dir == TC_UP) {
		((TC0_t *)tc)->CTRLFCLR |= ~TC0_DIR_bm;
	} else {
		((TC0_t *)tc)->CTRLFSET |= TC0_DIR_bm;
	}
}

/**
 * \brief Write the Counter value of the Timer
 *
 * \param tc Pointer to TC module.
 * \param cnt_value Counter value :
 */
static inline void tc_write_count(volatile void *tc, uint16_t cnt_value)
{
	((TC0_t *)tc)->CNT = cnt_value;
}

/**
 * \brief Reads the Counter value of the Timer
 *
 * \param tc Pointer to TC module.
 * \note Output the Counter value CNT
 */
static inline uint16_t tc_read_count(volatile void *tc)
{
	return (((TC0_t *)tc)->CNT);
}

/**
 * \brief Writes the Period value of the Timer
 *
 * \param tc Pointer to TC module.
 * \param per_value Period value : PER
 */
static inline void tc_write_period(volatile void *tc, uint16_t per_value)
{
	((TC0_t *)tc)->PER = per_value;
}

/**
 * \brief Reads the Period value of the Timer
 *
 * \param tc Pointer to TC module.
 * \return Period value : PER
 */
static inline uint16_t tc_read_period(volatile void *tc)
{
	return (((TC0_t *)tc)->PER);
}

/**
 * \brief Writes the Period Buffer value of the Timer
 *
 * \param tc Pointer to TC module.
 * \param per_buf Period Buffer value : PERH/PERL
 */
static inline void tc_write_period_buffer(volatile void *tc, uint16_t per_buf)
{
	((TC0_t *)tc)->PERBUF = per_buf;
}

/**
 * \brief Reads the Period Buffer value of the Timer
 *
 * \param tc Pointer to TC module.
 * \return Period Buffer value : PERH/PERL
 */
static inline uint16_t tc_read_period_buffer(volatile void *tc)
{
	return (((TC0_t *)tc)->PERBUF);
}

/**
 * \brief Tests if the Period Buffer is valid
 *
 * \param tc Pointer to TC module.
 * \return  period Buffer is valid or not:PERBV
 */
static inline bool tc_period_buffer_is_valid(volatile void *tc)
{
	return (((TC0_t *)tc)->CTRLGCLR & TC0_PERBV_bm);
}

/**
 * \brief Enables delay (used for 32bit timer mode)
 *
 * \param tc Pointer to TC module.
 * \note  enables Delay mode
 */
static inline void tc_enable_delay(volatile void *tc)
{
	((TC0_t *)tc)->CTRLD = (((TC0_t *)tc)->CTRLD &
			~TC0_EVDLY_bm) | (1 << TC0_EVDLY_bp);
}

/**
 * \brief Disables delay
 *
 * \param tc Pointer to TC module.
 * \note  disables Delay mode
 */
static inline void tc_disable_delay(volatile void *tc)
{
	((TC0_t *)tc)->CTRLD = ((TC0_t *)tc)->CTRLD & ~TC0_EVDLY_bm;
}

/**
 * \brief Tests if the Overflow flag is set
 *
 * \param tc Pointer to TC module.
 * \return  overflow has occurred or not : OVFIF
 */
static inline bool tc_is_overflow(volatile void *tc)
{
	return (((TC0_t *)tc)->INTFLAGS & TC0_OVFIF_bm);
}

/**
 * \brief Clears the Overflow flag
 *
 * \param tc Pointer to TC module.
 * \note  OVFIF is cleared
 */
static inline void tc_clear_overflow(volatile void *tc)
{
	((TC0_t *)tc)->INTFLAGS |= TC0_OVFIF_bm;
}

/**
 * \brief Tests if the Error flag is set
 *
 * \param tc Pointer to TC module.
 * \return  Error has occurred or not : ERRIF
 */
static inline bool tc_read_error(volatile void *tc)
{
	return (((TC0_t *)tc)->INTFLAGS & TC0_ERRIF_bm);
}

/**
 * \brief Clears the Error flag
 *
 * \param tc Pointer to TC module.
 * \note  ERRIF is cleared
 */
static inline void tc_clear_error(volatile void *tc)
{
	((TC0_t *)tc)->INTFLAGS |= TC0_ERRIF_bm;
}

/**
 * \brief Restart the Timer
 *
 * \param tc Pointer to TC module.
 * \note  CMD[3] in CTRLFSET is set to 1 and CMD[2] in CTRLFCLR is set
 */
static inline void tc_restart(volatile void *tc)
{
	((TC0_t *)tc)->CTRLFSET = TC_CMD_RESTART_gc;
}

/**
 * \brief Reset the Timer
 *
 * \param tc Pointer to TC module.
 * \note  CMD[3:2] in CTRLFSET are set to 1
 */
static inline void tc_reset(volatile void *tc)
{
	((TC0_t *)tc)->CTRLFSET = TC_CMD_RESET_gc;
}

/**
 * \brief Update the Timer
 *
 * \param tc Pointer to TC module.
 * \note  CMD[2] in CTRLFSET is set to 1 and CMD[3] in CTRLFCLR is set
 */
static inline void tc_update(volatile void *tc)
{
	((TC0_t *)tc)->CTRLFSET = TC_CMD_UPDATE_gc;
}

/**
 * \brief Configures the Timer in Byte mode
 *
 * \param tc Pointer to TC module.
 * \note Configures  BYTEM in CTRLE
 */
static inline void tc_set_8bits_mode(volatile void *tc)
{
#ifdef TC0_BYTEM0_bm
	((TC0_t *)tc)->CTRLE |= TC0_BYTEM0_bm;
#else
	((TC0_t *)tc)->CTRLE |= TC0_BYTEM_bm;
#endif
}

/**
 * \brief Locks the Update of the Buffered registers
 *
 * \param tc Pointer to TC module.
 *
 *  */
static inline void tc_lock_update_buffers(volatile void *tc)
{
	((TC0_t *)tc)->CTRLFSET |= TC0_LUPD_bm;
}

/**
 * \brief Unlocks the Update of the Buffered registers
 *
 * \param tc Pointer to TC module.
 * \note  Configures LUPD in CTRLFCLR
 */
static inline void tc_unlock_update_buffers(volatile void *tc)
{
	((TC0_t *)tc)->CTRLFCLR |= TC0_LUPD_bm;
}

/**
 * \brief Enables Compare/Capture channel
 *
 * \param tc Pointer to TC module.
 * \param enablemask CC channel
 */
static inline void tc_enable_cc_channels(volatile void *tc,
		enum tc_cc_channel_mask_enable_t enablemask)
{
	if (tc_is_tc0(void *tc)) {
		((TC0_t *)tc)->CTRLB |= enablemask;
	} else if (tc_is_tc1(void *tc)) {
		((TC1_t *)tc)->CTRLB |=
				enablemask & (TC1_CCAEN_bm | TC1_CCBEN_bm);
	}
}

/**
 * \brief Disables Compare/Capture channel
 *
 * \param tc Pointer to TC module.
 * \param disablemask CC channel
 */
static inline void tc_disable_cc_channels(volatile void *tc,
		enum tc_cc_channel_mask_enable_t disablemask)
{
	if (tc_is_tc0(void *tc)) {
		((TC0_t *)tc)->CTRLB &= ~disablemask;
	} else if (tc_is_tc1(void *tc)) {
		((TC1_t *)tc)->CTRLB &=
				~(disablemask & TC0_CCAEN_bm & TC0_CCBEN_bm);
	}
}

/**
 * \brief Enables Input capture mode
 *
 * \param tc Pointer to TC module.
 * \param eventsource Source for the capture
 * \param eventaction Event action capture type
 */
static inline void tc_set_input_capture(volatile void *tc,
		TC_EVSEL_t eventsource, TC_EVACT_t eventaction)
{
	((TC0_t *)tc)->CTRLD &= ~(TC0_EVSEL_gm | TC0_EVACT_gm);
	((TC0_t *)tc)->CTRLD |= ((uint8_t)eventsource | (uint8_t)eventaction);
}

/**
 * \brief Reads the Capture value
 *
 * \param tc Pointer to TC module.
 * \param channel_index Channel x
 * \return  Read value of CCx
 */
static inline uint16_t tc_read_cc(volatile void *tc,
		enum tc_cc_channel_t channel_index)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			return (((TC0_t *)tc)->CCA);
		case TC_CCB:
			return (((TC0_t *)tc)->CCB);
		case TC_CCC:
			return (((TC0_t *)tc)->CCC);
		case TC_CCD:
			return (((TC0_t *)tc)->CCD);
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				return (((TC1_t *)tc)->CCA);
			case TC_CCB:
				return (((TC1_t *)tc)->CCB);
			default:
				return (0);
			}
		}
	return (0);
}

/**
 * \brief Writes the CC value
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 * \param value Counter value
 */
static inline void tc_write_cc(volatile void *tc,
		enum tc_cc_channel_t channel_index, uint16_t value)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			((TC0_t *)tc)->CCA = value;
			break;
		case TC_CCB:
			((TC0_t *)tc)->CCB = value;
			break;
		case TC_CCC:
			((TC0_t *)tc)->CCC = value;
			break;
		case TC_CCD:
			((TC0_t *)tc)->CCD = value;
			break;
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				((TC1_t *)tc)->CCA = value;
				break;
			case TC_CCB:
				((TC1_t *)tc)->CCB = value;
				break;
			default:
				return ;
			}
		}
}

/**
 * \brief Writes the Capture/Compare Buffer value
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 * \param buffer_value Counter Buffer value
 */
static inline void tc_write_cc_buffer(volatile void *tc,
		enum tc_cc_channel_t channel_index, uint16_t buffer_value)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			((TC0_t *)tc)->CCABUF = buffer_value;
			break;
		case TC_CCB:
			((TC0_t *)tc)->CCBBUF = buffer_value;
			break;
		case TC_CCC:
			((TC0_t *)tc)->CCCBUF = buffer_value;
			break;
		case TC_CCD:
			((TC0_t *)tc)->CCDBUF = buffer_value;
			break;
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				((TC1_t *)tc)->CCABUF = buffer_value;
				break;
			case TC_CCB:
				((TC1_t *)tc)->CCBBUF = buffer_value;
				break;
			default:
				return;
			}
		}
}

/**
 * \brief Reads the Capture/Compare Buffer value
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 * \return  CCx Buffer value
 */
static inline uint16_t tc_read_cc_buffer(volatile void *tc,
		enum tc_cc_channel_t channel_index)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			return (((TC0_t *)tc)->CCABUF);
		case TC_CCB:
			return (((TC0_t *)tc)->CCBBUF);
		case TC_CCC:
			return (((TC0_t *)tc)->CCCBUF);
		case TC_CCD:
			return (((TC0_t *)tc)->CCDBUF);
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				return (((TC1_t *)tc)->CCABUF);
			case TC_CCB:
				return (((TC1_t *)tc)->CCBBUF);
			default:
				return (0);
			}
		}
	return (0);
}

/**
 * \brief Reports is Capture/Compare Buffer is valid
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 * \return  CCx Buffer is valid or not
 */
static inline bool tc_cc_buffer_is_valid(volatile void *tc,
		enum tc_cc_channel_t channel_index)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			return ((TC0_t *)tc)->CTRLGCLR & TC0_CCABV_bm;
		case TC_CCB:
			return ((TC0_t *)tc)->CTRLGCLR & TC0_CCBBV_bm;
		case TC_CCC:
			return ((TC0_t *)tc)->CTRLGCLR & TC0_CCCBV_bm;
		case TC_CCD:
			return ((TC0_t *)tc)->CTRLGCLR & TC0_CCDBV_bm;
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				return (((TC1_t *)tc)->CTRLGCLR &
						TC1_CCABV_bm);
			case TC_CCB:
				return (((TC1_t *)tc)->CTRLGCLR &
						TC1_CCBBV_bm);
			default:
				return (0);
			}
		}
	return (0);
}

/**
 * \brief Reports if Capture/Compare interrupt has occurred
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 * \return  CCx Interrupt or not
 */
static inline bool tc_is_cc_interrupt(volatile void *tc,
		enum tc_cc_channel_t channel_index)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			return (((TC0_t *)tc)->INTFLAGS & TC0_CCAIF_bm);
		case TC_CCB:
			return (((TC0_t *)tc)->INTFLAGS & TC0_CCBIF_bm);
		case TC_CCC:
			return (((TC0_t *)tc)->INTFLAGS & TC0_CCCIF_bm);
		case TC_CCD:
			return (((TC0_t *)tc)->INTFLAGS & TC0_CCDIF_bm);
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				return (((TC1_t *)tc)->INTFLAGS &
						TC1_CCAIF_bm);
			case TC_CCB:
				return (((TC1_t *)tc)->INTFLAGS &
						TC1_CCBIF_bm);
			default:
				return (0);
			}
		}
	return (0);
}

/**
 * \brief Clears Capture/Compare interrupt
 *
 * \param tc Pointer to TC module.
 * \param channel_index CC Channel
 */
static inline void tc_clear_cc_interrupt(volatile void *tc,
		enum tc_cc_channel_t channel_index)
{
	if (tc_is_tc0(void *tc)) {
		switch (channel_index) {
		case TC_CCA:
			((TC0_t *)tc)->INTFLAGS = TC0_CCAIF_bm;
			break;
		case TC_CCB:
			((TC0_t *)tc)->INTFLAGS = TC0_CCBIF_bm;
			break;
		case TC_CCC:
			((TC0_t *)tc)->INTFLAGS = TC0_CCCIF_bm;
			break;
		case TC_CCD:
			((TC0_t *)tc)->INTFLAGS = TC0_CCDIF_bm;
			break;
		}
	} else if (tc_is_tc1(void *tc)) {
			switch (channel_index) {
			case TC_CCA:
				((TC1_t *)tc)->INTFLAGS = TC1_CCAIF_bm;
				break;
			case TC_CCB:
				((TC1_t *)tc)->INTFLAGS = TC1_CCBIF_bm;
				break;
			default:
				return;
			}
		}
}

/**
 * \brief Configures TC in the specified Waveform generator mode
 *
 * \param tc Pointer to TC module.
 * \param wgm : waveform generator
 */
static inline void tc_set_wgm(volatile void *tc, enum tc_wg_mode_t wgm)
{
	((TC0_t *)tc)->CTRLB = (((TC0_t *)tc)->CTRLB & ~TC0_WGMODE_gm) | wgm;
}

/**
 * \ingroup tc_group
 * \defgroup tc_awex_group AWeX extension driver
 * This group provides low level drivers to configure AWeX extension
 * @{
 */

/**
 * \brief AWeX extension enable
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_cwcm(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_CWCM_bm;
}

/**
 * \brief AWeX extension disable Common waveform mode
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_cwcm(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_CWCM_bm;
}

/**
 * \brief AWeX extension enable pattern generator mode
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_pgm(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_PGM_bm;
}

/**
 * \brief AWeX extension disable pattern generator mode
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_pgm(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_PGM_bm;
}

/**
 * \brief AWeX extension : enable Deadtime insertion on ccA
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_cca_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_DTICCAEN_bm;
}

/**
 * \brief AWeX extension : disable Deadtime insertion on ccA
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_cca_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_DTICCAEN_bm;
}

/**
 * \brief AWeX extension : enable Deadtime insertion on ccB
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_ccb_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_DTICCBEN_bm;
}

/**
 * \brief AWeX extension : disable Deadtime insertion on ccB
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_ccb_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_DTICCBEN_bm;
}

/**
 * \brief AWeX extension : enable Deadtime insertion on ccC
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_ccc_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_DTICCCEN_bm;
}

/**
 * \brief AWeX extension : disable Deadtime insertion on ccD
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_ccc_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_DTICCCEN_bm;
}

/**
 * \brief AWeX extension : enable Deadtime insertion on ccD
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_ccd_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL |= AWEX_DTICCDEN_bm;
}

/**
 * \brief AWeX extension : disable Deadtime insertion on ccD
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_ccd_deadtime(AWEX_t *awex)
{
	((AWEX_t *)awex)->CTRL &= ~AWEX_DTICCDEN_bm;
}
/**
 * \brief AWeX extension : configures high side deadtime
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param value : deadtime value
 */
static inline void tc_awex_set_dti_high(AWEX_t *awex, int16_t value)
{
	((AWEX_t *)awex)->DTHS = value;
}
/**
 * \brief AWeX extension : configures low side deadtime
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param value : deadtime value
 */
static inline void tc_awex_set_dti_low(AWEX_t *awex, int16_t value)
{
	((AWEX_t *)awex)->DTLS = value;
}
/**
 * \brief AWeX extension : configures symmetrical deadtime
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param value : deadtime value
 */
static inline void tc_awex_set_dti_both(AWEX_t *awex, int16_t value)
{
	((AWEX_t *)awex)->DTBOTH = value;
}

/**
 * \brief AWeX extension : configures symmetrical deadtime buffer
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param value : deadtime buffer value
 */
static inline void tc_awex_set_dti_both_buffer(AWEX_t *awex,
		int16_t value)
{
	((AWEX_t *)awex)->DTBOTHBUF = value;
}

/**
 * \brief AWeX extension : returns the deadtime buffer high nibble
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \return Dead Time High value
 */
static inline int8_t tc_awex_get_dti_high_buffer(AWEX_t *awex)
{
	return (((AWEX_t *)awex)->DTHSBUF);
}

/**
 * \brief AWeX extension : returns the deadtime buffer low nibble
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \return Dead Time High value
 */
static inline int8_t tc_awex_get_dti_low_buffer(AWEX_t *awex)
{
	return (((AWEX_t *)awex)->DTLSBUF);
}

/**
 * \brief AWeX extension : returns if DTI high buffer is valid
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \return  Dead Time High Buffer valid or not
 */
static inline bool tc_awex_is_dti_high_buffer_valid(AWEX_t *awex)
{
	return (((AWEX_t *)awex)->STATUS & AWEX_DTHSBUFV_bm);
}

/**
 * \brief AWeX extension : returns if DTI low buffer is valid
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \return Dead Time Low Buffer is valid or not
 */
static inline bool tc_awex_is_dti_low_buffer_valid(AWEX_t *awex)
{
	return (((AWEX_t *)awex)->STATUS & AWEX_DTLSBUFV_bm);
}

/**
 * \brief AWeX extension : configures the Fault restart in latched mode
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_fdmode_restart_latched(AWEX_t *awex)
{
	((AWEX_t *)awex)->FDCTRL &= ~AWEX_FDMODE_bm;
}

/**
 * \brief AWeX extension : configures the Fault restart in cycle to cycle mode
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_fdmode_restart_cycle(AWEX_t *awex)
{
	((AWEX_t *)awex)->FDCTRL |= AWEX_FDMODE_bm;
}

/**
 * \brief AWeX extension : returns if fault is detected
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline bool tc_awex_fault_is_detected(AWEX_t *awex)
{
	return (((AWEX_t *)awex)->STATUS & AWEX_FDF_bm);
}

/**
 * \brief AWeX extension : clears the Fault detection
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_clear_fault(AWEX_t *awex)
{
	((AWEX_t *)awex)->STATUS = AWEX_FDF_bm;
}

/**
 * \brief AWeX extension : configures fault action
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param fd_act Fault action
 */
static inline void tc_awex_set_fault_detection_action(AWEX_t *
		awex, AWEX_FDACT_t fd_act)
{
	((AWEX_t *)awex)->FDCTRL = (((AWEX_t *)awex)->FDCTRL & ~AWEX_FDACT_gm) |
		(fd_act & AWEX_FDACT_gm);

}

/**
 * \brief AWeX extension : configures fault detection event
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param eventmask Fault detection event
 */
static inline void tc_awex_set_fault_detection_event(AWEX_t *awex,
		int8_t eventmask)
{
	((AWEX_t *)awex)->FDEMASK = eventmask;
}

/**
 * \brief AWeX extension : configures the port overdrive
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 * \param value Output override configuration
 */
static inline void tc_awex_set_output_override(AWEX_t * awex,
		int8_t value)
{
	((AWEX_t *)awex)->OUTOVEN = value;
}

/**
 * \brief AWeX extension : enable fault detection on debug break detection
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_enable_fault_debug_break(AWEX_t *awex)
{
	((AWEX_t *)awex)->FDCTRL &= ~AWEX_FDDBD_bm;
}

/**
 * \brief AWeX extension : disable fault detection on debug break detection
 *
 * \param awex Pointer to AWeX module (AWEXC or AWEXE)
 */
static inline void tc_awex_disable_fault_debug_break(AWEX_t *awex)
{
	((AWEX_t *)awex)->FDCTRL |= AWEX_FDDBD_bm;
}
//@}
/**
 * \ingroup tc_group
 * \defgroup tc_hires_group Hi-Res extension driver
 * This group provides low level drivers to configure Hi-Res extension
 * @{
 */

/**
 * \brief Hi-Res Extension : configures the Hi-Res
 *
 * \param hires Pointer to AWeX module (AWEXC or AWEXE)
 * \param hi_res_mode HIRES configuration
 */
static inline void tc_hires_set_mode(HIRES_t * hires, HIRES_HREN_t hi_res_mode)
{
	((HIRES_t *)hires)->CTRLA = hi_res_mode;
}
//@}

/** @} */


#ifdef __cplusplus
}
#endif

/**
 * \page xmega_tc_quickstart Quick Start Guide for the XMEGA TC Driver
 *
 * This is the quick start guide for the \ref tc_group , with step-by-step
 * instructions on how to configure and use the driver for a specific use case.
 * The code examples can be copied into e.g the main application loop or any
 * other function that will need to control the timer/counters.
 *
 *
 * \section xmega_tc_qs_use_cases Use cases
 * - \ref xmega_tc_qs_ovf
 * - \ref xmega_tc_qs_cc
 * - \ref xmega_tc_qs_pwm
 *
 *
 * \section xmega_tc_qs_ovf Timer/counter overflow (interrupt based)
 *
 * This use case will prepare a timer to trigger an interrupt when the timer
 * overflows. The interrupt is handled by a cutomisable callback function.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode (incrementing timer)
 * - Use the system clock as clock source
 * - No prescaling (clock divider set to 1)
 * - Overflow interrupt after 1000 counts. This will be done by setting the top
 *   value to 1000.
 *
 *
 * \section xmega_tc_qs_ovf_setup Setup steps
 *
 * \subsection xmega_tc_qs_ovf_usage_prereq Prequisites
 *
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc_qs_ovf_setup_code Example code
 *
 * Add a callback function that will be executed when the overflow interrupt
 * trigger.
 * \code
	static void my_callback(void)
	{
	    // User code to execute when the overflow occurs here
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	pmic_init();
	sysclk_init();
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0, my_callback);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 1000);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	cpu_irq_enable();
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
\endcode
 *
 * \subsection xmega_tc_qs_ovf_setup_code_workflow Workflow
 *
 * -# Enable the interrupt controller:
 *  - \code pmic_init(); \endcode
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable timer/counter TCC0
 *  - \code tc_enable(&TCC0); \endcode
 *    \note This will enable the clock system for the module
 * -# Set the callback function for overflow interrupt
 *  - \code tc_set_overflow_interrupt_callback(&TCC0, my_callback); \endcode
 *    \warning This function requires that the my_callback function is defined
 * -# Set the desired waveform mode
 *  - \code tc_set_wgm(&TCC0, TC_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
            count value until the TOP value is reached. The timer then reset
            its count value to 0.
 * -# Set the period
 *  - \code tc_write_period(&TCC0, 1000); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set the overflow interrupt level
 *   - \code tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO); \endcode
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 * -# Set the clock source
 *  - \code tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc_qs_ovf_usage Usage steps
 *
 * - None. The timer will run in the background, and the code written in the
 *   call back function will execute each time the timer overflows.
 *
 *
 * \section xmega_tc_qs_cc Timer/counter compare match (interrupt based)
 *
 * This use case will prepare a timer to trigger two independent interrupts
 * when it reaches two different compare values. The period of the timer
 * is customizable and the two compare matches will be handled by two separate
 * interrupts implemented in call back functions.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode - incrementing timer
 * - Use the system clock as clock source
 * - No prescaling (divider set to 1)
 * - Period of timer 10000 counts
 * - Compare match A interrupt trigger after 100 counts
 * - Compare match B interrupt trigger after 1000 counts
 * - If compare A and compare B match occurs simultaneously, compare B
 *   should have higher priority
 *
 *
 * \section xmega_tc_qs_cc_setup Setup steps
 *
 * \subsection xmega_tc_qs_cc_usage_prereq Prequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref interrupt_group "Global Interrupt Management"
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc_qs_cc_setup_code Example code
 *
 * Add two callback functions that will be executed when compare match A and
 * compare match B occurs
 * \code
	static void my_cca_callback(void)
	{
	   // User code here to execute when a channel A compare match occurs
	}
	static void my_ccb_callback(void)
	{
	   // User code here to execute when a channel B compare match occurs
	}
\endcode
 * Add to, e.g., the main loop in the application C-file:
 * \code
	pmic_init();
	sysclk_init();
	cpu_irq_enable();
	tc_enable(&TCC0);
	tc_set_cca_interrupt_callback(&TCC0, my_cca_callback);
	tc_set_ccb_interrupt_callback(&TCC0, my_ccb_callback);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 10000);
	tc_write_cc(&TCC0, TC_CCA, 100);
	tc_write_cc(&TCC0, TC_CCB, 1000);
	tc_enable_cc_channels(&TCC0,(TC_CCAEN | TC_CCBEN));
	tc_set_cca_interrupt_level(&TCC0, TC_INT_LVL_LO);
	tc_set_ccb_interrupt_level(&TCC0, TC_INT_LVL_MED);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
\endcode
 *
 * \subsection xmega_tc_qs_cc_setup_code_workflow Workflow
 *
 * -# Enable the interrupt controller:
 *  - \code pmic_init(); \endcode
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -# Enable interrupts:
 *  - \code cpu_irq_enable(); \endcode
 * -#  Enable timer/counter TCC0
 *  - \code tc_enable(&TCC0); \endcode
 *    \note This will enable the clock system for the module
 * -# Set call back function for CCA interrupt
 *  - \code tc_set_cca_interrupt_callback(&TCC0, my_cca_callback); \endcode
 *    \warning This function requires that the call back function is defined
 * -# Set call back function for CCB interrupt
 *  - \code tc_set_ccb_interrupt_callback(&TCC0, my_ccb_callback); \endcode
 *    \warning This function requires that the call back function is defined
 * -# Set the desired waveform mode
 *  - \code tc_set_wgm(&TCC0, TC_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
            count value until the TOP value is reached. The timer then reset
            its count value to 0.
 * -# Set the period
 *  - \code tc_write_period(&TCC0, 10000); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set compare match value on CCA
 *   - \code tc_write_cc(&TCC0, TC_CCA, 100); \endcode
 * -# Set compare match value on CCB
 *   - \code tc_write_cc(&TCC0, TC_CCB, 1000); \endcode
 * -# Enable compare channel A and compare channel B
 *  -\code tc_enable_cc_channels(&TCC0, (TC_CCAEN | TC_CCBEN)); \endcode
 * -# Set interrupt level on channel A (low priority, see \ref TC_INT_LEVEL_t)
 *   - \code tc_set_cca_interrupt_level(&TCC0, TC_INT_LVL_LO); \endcode
 * -# Set interrupt level on channel B (medium priority \ref TC_INT_LEVEL_t)
 *   - \code tc_set_ccb_interrupt_level(&TCC0, TC_INT_LVL_MED); \endcode
 * -# Set the clock source
 *  - \code tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc_qs_cc_usage Usage steps
 *
 * - None. The timer will run in the background, and the code written in the
 *   call back functions will execute each time a compare match occur.
 *
 *
 * \section xmega_tc_qs_pwm Timer/counter PWM
 *
 * This use case will setup a timer in PWM mode. For more details you can
 * also look at the XMEGA PWM service.
 *
 * We will setup the timer in this mode:
 * - Normal WGM mode - incrementing timer
 * - Use the 2MHz oscillator as clock source (default)
 * - 1Hz PWM frequency (2MHz clock, 1024x prescale, TOP value 1950)
 * - 10% duty cycle (1:10 ratio between PER and CC register)
 * - Output the PWM signal to a I/O port
 *
 * \section xmega_tc_qs_pwm_setup Setup steps
 *
 * \subsection xmega_tc_qs_pwm_usage_prereq Prequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * - \ref clk_group "Clock Management"
 *
 * \subsection xmega_tc_qs_pwm_setup_code Example code
 *
 * Add to, e.g., the main loop in the application C-file:
 * \code
	board_init();
	sysclk_init();
	tc_enable(&TCE0);
	tc_set_wgm(&TCE0, TC_WG_SS);
	tc_write_period(&TCE0, 1950);
	tc_write_cc(&TCE0, TC_CCA, 195);
	tc_enable_cc_channels(&TCE0,TC_CCAEN);
	tc_write_clock_source(&TCE0, TC_CLKSEL_DIV1024_gc);
\endcode
 *
 * \subsection xmega_tc_qs_pwm_setup_code_workflow Workflow
 *
 * -# Ensure that PWM I/O pin is configured as output
 *  - \code board_init(); \endcode
 *    \note The board_init(); function configures the I/O pins. If this function
 *          is not executed, the I/O pin must be configured as output manually
 * -# Enable the clock system:
 *  - \code sysclk_init(); \endcode
 * -#  Enable timer/counter TCE0
 *  - \code tc_enable(&TCE0); \endcode
 *    \note This will enable the clock system for the module
 * -# Set the desired waveform mode
 *  - \code tc_set_wgm(&TCE0, TC_WG_NORMAL); \endcode
 *    \note In this case, we use normal mode where the timer increments it
 *          count value until the TOP value is reached. The timer then reset
  *         its count value to 0.
 * -# Set the period
 *  - \code tc_write_period(&TCE0, 1950); \endcode
 *    \note This will specify the TOP value of the counter. The timer will
 *          overflow and reset when this value is reached.
 * -# Set compare match value on CCA
 *   - \code tc_write_cc(&TCC0, TC_CCA, 195); \endcode
 *     \note The PWM duty cycle will be the ratio between PER and CCA, which
 *           is set by the tc_write_period() and tc_write_cc() functions. Use
 *           tc_write_cc() to change duty cycle run time (e.g to dim a LED).
 *           When CCA = 0, the duty cycle will be 0%. When CCA = PER (top value)
 *           the duty cycle will be 100%.
 * -# Enable compare channel A
 *  -\code tc_enable_cc_channels(&TCE0,TC_CCAEN); \endcode
 * -# Set the clock source
 *  - \code tc_write_clock_source(&TCE0, TC_CLKSEL_DIV1024_gc); \endcode
 *    \warning When the clock source is set, the timer will start counting
 *
 * \section xmega_tc_qs_pwm_usage Usage steps
 *  - Use tc_write_cc() to change the duty cycle of the PWM signal
 *  - Use tc_write_period() to change the PWM frequency
 */

#endif /* _TC_H_ */
