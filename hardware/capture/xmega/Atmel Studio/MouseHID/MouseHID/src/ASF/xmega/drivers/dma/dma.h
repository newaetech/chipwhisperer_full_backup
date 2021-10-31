/**
 * \file
 *
 * \brief AVR XMEGA Direct Memory Access Controller driver definitions
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
#ifndef DRIVERS_DMA_DMA_H
#define DRIVERS_DMA_DMA_H

#include <compiler.h>
#include <interrupt.h>
#include <hugemem.h>
#include <pmic.h>

/**
 * \defgroup dma_group Direct Memory Access controller (DMA)
 *
 * See \ref xmega_dma_quickstart.
 *
 * This is a driver for configuration, enabling, disabling and use of the
 * on-chip XMEGA Direct Memory Access (DMA) controller.
 *
 * \section dependencies Dependencies
 *
 * The direct memory access controller depends on the following modules:
 *  - \ref pmic_group for setting interrupt level.
 *  - \ref sysclk_group for peripheral clock control.
 *  - \ref sleepmgr_group for setting available sleep mode.
 *
 * @{
 */

#if XMEGA_A || XMEGA_AU
#  define DMA_NUMBER_OF_CHANNELS 4
#else
/** Number of available DMA channels, device dependent. */
#  define DMA_NUMBER_OF_CHANNELS 2
#endif

/**
 * \private
 * \brief Helper macro to get the channel offset given the channel number
 *
 * \param num DMA channel number
 *
 * \return DMA_CH_t pointer to the \a num DMA channel register struct
 */
#define dma_get_channel_address_from_num(num) \
	((DMA_CH_t *)((uintptr_t)(&DMA.CH0) + (sizeof(DMA_CH_t) * num)))

/** \brief DMA channel number type */
typedef uint8_t dma_channel_num_t;

/** \brief DMA channel status */
enum dma_channel_status {
	/** DMA channel is idling */
	DMA_CH_FREE = 0,
	/** DMA channel has a block transfer pending */
	DMA_CH_PENDING,
	/** DMA channel is busy doing a block transfer */
	DMA_CH_BUSY,
	/** DMA channel has completed a block transfer */
	DMA_CH_TRANSFER_COMPLETED,
	/** DMA channel failed to complete a block transfer */
	DMA_CH_TRANSFER_ERROR,
};

/**
 * \brief DMA channel configuration struct
 */
struct dma_channel_config {
	/** DMA channel control register A */
	uint8_t ctrla;
	/** DMA channel control register B */
	uint8_t ctrlb;
	/** DMA channel address control register */
	uint8_t addrctrl;
	/** DMA channel trigger control register */
	uint8_t trigsrc;
	/** DMA channel block transfer count register */
	uint16_t trfcnt;
	/** DMA channel repeat counter register */
	uint8_t repcnt;
	union {
#ifdef CONFIG_HAVE_HUGEMEM
		/** DMA channel source hugemem address */
		hugemem_ptr_t srcaddr;
#endif
		/** DMA channel source 16-bit address */
		uint16_t srcaddr16;
	};
	union {
#ifdef CONFIG_HAVE_HUGEMEM
		/** DMA channel destination hugemem address */
		hugemem_ptr_t destaddr;
#endif
		/** DMA channel destaddr 16-bit address */
		uint16_t destaddr16;
	};
};

/** DMA interrupt levels */
enum dma_int_level_t {
	DMA_INT_LVL_OFF = 0x00,
	DMA_INT_LVL_LO = 0x01,
	DMA_INT_LVL_MED = 0x02,
	DMA_INT_LVL_HI = 0x03,
};

void dma_enable(void);
void dma_disable(void);

/** \name DMA Controller Management */
/* @{ */

/**
 * \brief Set DMA channel priority mode
 *
 * This functions sets the channel priority mode. Users can select between
 * fixed priority, round-robin or a mix of both.
 *
 * The modes are defined in the toolchain header files in the form of \a
 * DMA_PRIMODE_*_gc for devices with more than 2 channels, or DMA_PRIMODE_*_bm
 * for dual channel devices.
 *
 * \param primode DMA channel priority mode given by a DMA_PRIMODE_t type
 */
static inline void dma_set_priority_mode(DMA_PRIMODE_t primode)
{
	irqflags_t iflags;

#if XMEGA_A || XMEGA_AU
	Assert(!(primode & ~DMA_PRIMODE_gm));
#else
	Assert(!(primode & ~DMA_PRIMODE_bm));
#endif

	iflags = cpu_irq_save();

#if XMEGA_A || XMEGA_AU
	DMA.CTRL = (DMA.CTRL & ~DMA_PRIMODE_gm) | primode;
#else
	DMA.CTRL = (DMA.CTRL & ~DMA_PRIMODE_bm) | primode;
#endif

	cpu_irq_restore(iflags);
}

/**
 * \brief Set DMA channel double buffering mode
 *
 * This function modifies which channels should be in double buffer mode. The
 * modes are defined in the toolchain header files in the form of \a
 * DMA_DBUFMODE_*_gc for devices with more than 2 channels, or DMA_DBUFMODE_*_bm
 * for dual channel devices.
 *
 * \param dbufmode Double buffer channel configuration given by a
 *                 DMA_DBUFMODE_t type
 */
static inline void dma_set_double_buffer_mode(DMA_DBUFMODE_t dbufmode)
{
	irqflags_t iflags;

#if XMEGA_A || XMEGA_AU
	Assert(!(dbufmode & ~DMA_DBUFMODE_gm));
#else
	Assert(!(dbufmode & ~DMA_DBUFMODE_bm));
#endif

	iflags = cpu_irq_save();

#if XMEGA_A || XMEGA_AU
	DMA.CTRL = (DMA.CTRL & ~DMA_DBUFMODE_gm) | dbufmode;
#else
	DMA.CTRL = (DMA.CTRL & ~DMA_DBUFMODE_bm) | dbufmode;
#endif

	cpu_irq_restore(iflags);
}

/** @} */

/** \name DMA Channel Management */
/** @{ */

void dma_channel_write_config(dma_channel_num_t num,
		struct dma_channel_config *config);
void dma_channel_read_config(dma_channel_num_t num,
		struct dma_channel_config *config);

/**
 * \brief Enable a DMA channel
 *
 * This function enables a DMA channel, depending on the configuration the DMA
 * channel will start the block transfer upon a
 * \ref dma_channel_trigger_block_transfer() or when other hardware modules trigger
 * the DMA hardware.
 *
 * \pre The DMA channel configuration must be written to the channel before
 * enabling the channel.
 *
 * \param num DMA channel to enable
 */
static inline void dma_channel_enable(dma_channel_num_t num)
{
	irqflags_t iflags = cpu_irq_save();
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);

#if XMEGA_A || XMEGA_AU
	channel->CTRLA |= DMA_CH_ENABLE_bm;
#else
	channel->CTRLA |= DMA_CH_CHEN_bm;
#endif

	cpu_irq_restore(iflags);
}

/**
 * \brief Disable a DMA channel
 *
 * This function disables a DMA channel. If the DMA channel was already enabled
 * the DMA channel is not disabled before the internal transfer buffer is empty
 * and the DMA transfer is aborted.
 *
 * \param num DMA channel to disable
 */
static inline void dma_channel_disable(dma_channel_num_t num)
{
	irqflags_t iflags = cpu_irq_save();
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);

#if XMEGA_A || XMEGA_AU
	channel->CTRLA &= ~DMA_CH_ENABLE_bm;
#else
	channel->CTRLA &= ~DMA_CH_CHEN_bm;
#endif

	cpu_irq_restore(iflags);
}

/**
 * \brief Check if DMA channel is enabled
 *
 * This function checks if a DMA channel is enabled.
 *
 * \param num DMA channel number to query
 *
 * \retval true DMA channel is enabled
 * \retval false DMA channel is disabled
 */
static inline bool dma_channel_is_enabled(dma_channel_num_t num)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	bool channel_enabled;

#if XMEGA_A || XMEGA_AU
	channel_enabled = (channel->CTRLA & DMA_CH_ENABLE_bm);
#else
	channel_enabled = (channel->CTRLA & DMA_CH_CHEN_bm);
#endif

	return channel_enabled;
}

/**
 * \brief Check if DMA channel is busy
 *
 * This function checks if a DMA channel is busy doing or going to do (pending)
 * a block transfer.
 *
 * \param num DMA channel number to query
 *
 * \retval true DMA channel is busy or have a block transfer pending
 * \retval false DMA channel is not busy or have a block transfer pending
 */
static inline bool dma_channel_is_busy(dma_channel_num_t num)
{
	uint8_t busy_pending = DMA.STATUS;

	busy_pending &= (1 << num) | (1 << (num + 4));
	if (busy_pending) {
		return true;
	}

	return false;
}

/**
 * \brief Get channel status
 *
 * This function returns the current channel status.
 *
 * \param num DMA channel number
 *
 * \return Channel status given by a \ref dma_channel_status
 */
static inline enum dma_channel_status dma_get_channel_status(
		dma_channel_num_t num)
{
	uint8_t busy_pending    = DMA.STATUS;
	uint8_t error_completed = DMA.INTFLAGS;

	/*
	 * Check lower and upper nibble of INTFLAGS register to find possible
	 * error or transfer completed status.
	 */
	error_completed &= (1 << num) | (1 << (num + 4));
	if (error_completed & (1 << (num + 4))) {
		return DMA_CH_TRANSFER_ERROR;
	} else if (error_completed & (1 << num)) {
		return DMA_CH_TRANSFER_COMPLETED;
	}

	/*
	 * Check lower and upper nibble of STATUS register to find possible
	 * busy or pending completed status.
	 */
	busy_pending &= (1 << num) | (1 << (num + 4));
	if (busy_pending & (1 << (num + 4))) {
		return DMA_CH_BUSY;
	} else if (busy_pending & (1 << num)) {
		return DMA_CH_PENDING;
	}

	return DMA_CH_FREE;
}

/**
 * \brief DMA channel trigger block transfer
 *
 * This function triggers a start of a block transfer on a given DMA channel.
 *
 * \pre The DMA channel must be configured and enabled for this to have any
 * affect.
 *
 * \param num DMA channel number to trigger block transfer for
 */
static inline void dma_channel_trigger_block_transfer(dma_channel_num_t num)
{
	irqflags_t iflags = cpu_irq_save();
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);

	channel->CTRLA  |= DMA_CH_TRFREQ_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Reset a DMA channel
 *
 * This function resets a given DMA channel.
 *
 * \param num DMA channel number to trigger block transfer for
 */
static inline void dma_channel_reset(dma_channel_num_t num)
{
	irqflags_t iflags = cpu_irq_save();
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);

#if XMEGA_A || XMEGA_AU
	channel->CTRLA  |= DMA_CH_RESET_bm;
#else
	channel->CTRLA  |= DMA_CH_CHRST_bm;
#endif

	cpu_irq_restore(iflags);
}

/**
 * \brief Callback definition for DMA channel interrupts
 *
 * \param status Status of DMA channel block transfer
 */
typedef void (*dma_callback_t)(enum dma_channel_status status);

void dma_set_callback(dma_channel_num_t num, dma_callback_t callback);

/** @} */

/**
 * \name DMA Channel Direct Configuration Functions
 *
 * These functions allows direct configuration on the DMA channels, not going
 * through a \ref dma_channel_config struct. This allows update of the most
 * commonly changed DMA channel properties.
 *
 * @{
 */

/**
 * \brief Write DMA channel burst length to hardware
 *
 * This function writes the DMA channel burst length directly to hardware. The
 * burst lengths are defined in the toolchain header files in the form of \a
 * DMA_CH_BURSTLEN_*_gc, where * represents the various available burst
 * lengths.
 *
 * \param num DMA channel number to write burst length for
 * \param burst_length DMA channel burst length given by a DMA_CH_BURSTLEN_t
 *                     type
 */
static inline void dma_channel_write_burst_length(dma_channel_num_t num,
		DMA_CH_BURSTLEN_t burst_length)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->CTRLA &= ~DMA_CH_BURSTLEN_gm;
	channel->CTRLA |= burst_length;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write DMA channel transfer count to hardware
 *
 * This function writes the DMA channel number of bytes in each block transfer
 * to hardware.
 *
 * \param num DMA channel number to write transfer count for
 * \param count Number of bytes in each block transfer
 */
static inline void dma_channel_write_transfer_count(dma_channel_num_t num,
		uint16_t count)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->TRFCNT = count;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write DMA channel number of transfer repeats to hardware
 *
 * This function writes the DMA channel number of block transfer repeats to
 * hardware. It will also enable the repeat feature.
 *
 * \param num DMA channel number to write transfer repeats for
 * \param repeats Number of block transfer repeats
 */
static inline void dma_channel_write_repeats(dma_channel_num_t num,
		uint8_t repeats)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->REPCNT = repeats;
	channel->CTRLA |= DMA_CH_REPEAT_bm;

	cpu_irq_restore(iflags);
}

/**
 * \brief Write DMA channel 16-bit destination address to hardware
 *
 * This function writes a DMA channel destination 16-bit LSB address to
 *hardware.
 *
 * \param num DMA channel number to write 16-bit destination address for
 * \param destination 16-bit LSB destination address
 */
static inline void dma_channel_write_destination(dma_channel_num_t num,
		uint16_t destination)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->DESTADDR0 = destination;
	channel->DESTADDR1 = destination >> 8;

#if XMEGA_A || XMEGA_AU
	/*
	 * Make sure the third byte of the destination address is zero to avoid
	 * a runaway DMA transfer.
	 */
	channel->DESTADDR2 = 0;
#endif

	cpu_irq_restore(iflags);
}

#ifdef CONFIG_HAVE_HUGEMEM

/**
 * \brief Write DMA channel hugemem destination address to hardware
 *
 * This function writes a DMA channel destination hugemem address to hardware.
 *
 * \param num DMA channel number to write hugemem destination address for
 * \param destination hugemem destination address
 */
static inline void dma_channel_write_destination_hugemem(dma_channel_num_t num,
		hugemem_ptr_t destination)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->DESTADDR0 = (uint32_t)destination;
	channel->DESTADDR1 = (uint32_t)destination >> 8;
	channel->DESTADDR2 = (uint32_t)destination  >> 16;

	cpu_irq_restore(iflags);
}

#endif

/**
 * \brief Write DMA channel 16-bit source address to hardware
 *
 * This function writes a DMA channel source 16-bit LSB address to hardware.
 *
 * \param num DMA channel number to write 16-bit source address for
 * \param source 16-bit LSB source address
 */
static inline void dma_channel_write_source(dma_channel_num_t num,
		uint16_t source)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->SRCADDR0 = source;
	channel->SRCADDR1 = source >> 8;

#if XMEGA_A || XMEGA_AU
	/*
	 * Make sure the third byte of the source address is zero to avoid a
	 * runaway DMA transfer.
	 */
	channel->SRCADDR2 = 0;
#endif

	cpu_irq_restore(iflags);
}

#ifdef CONFIG_HAVE_HUGEMEM

/**
 * \brief Write DMA channel hugemem source address to hardware
 *
 * This function writes a DMA channel source hugemem address to hardware.
 *
 * \param num DMA channel number to write hugemem source address for
 * \param source hugemem source address
 */
static inline void dma_channel_write_source_hugemem(dma_channel_num_t num,
		hugemem_ptr_t source)
{
	DMA_CH_t *channel = dma_get_channel_address_from_num(num);
	irqflags_t iflags = cpu_irq_save();

	channel->SRCADDR0 = (uint32_t)source;
	channel->SRCADDR1 = (uint32_t)source >> 8;
	channel->SRCADDR2 = (uint32_t)source >> 16;

	cpu_irq_restore(iflags);
}

#endif

/** @} */

/** \name DMA Channel Configuration Helper Functions */
/** @{ */

/**
 * \brief Set DMA channel burst length
 *
 * This function helps the caller setting the DMA channel burst length. The
 * burst lengths are defined in the toolchain header files in the form of \a
 * DMA_CH_BURSTLEN_*_gc, where * represents the various available burst
 * lengths.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param burst_length DMA channel burst length given by a DMA_CH_BURSTLEN_t
 *                     type
 */
static inline void dma_channel_set_burst_length(
		struct dma_channel_config *config,
		DMA_CH_BURSTLEN_t burst_length)
{
	config->ctrla &= ~DMA_CH_BURSTLEN_gm;
	config->ctrla |= burst_length;
}

/**
 * \brief Set DMA channel in single shot transfer mode
 *
 * This function helps the caller setting the DMA channel in single shot
 * transfer mode.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 */
static inline void dma_channel_set_single_shot(struct dma_channel_config *config)
{
	config->ctrla |= DMA_CH_SINGLE_bm;
}

/**
 * \brief Unset DMA channel from single shot transfer mode
 *
 * This function helps the caller clear the DMA channel single shot transfer
 * mode feature.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 */
static inline void dma_channel_unset_single_shot(
		struct dma_channel_config *config)
{
	config->ctrla &= ~DMA_CH_SINGLE_bm;
}

/**
 * \brief Set DMA channel interrupt level
 *
 * This function helps the caller setting the DMA channel interrupt level for
 * transaction complete and channel error.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param level Interrupt level given by a \ref dma_int_level_t type
 */
static inline void dma_channel_set_interrupt_level(struct dma_channel_config
		*config, enum dma_int_level_t level)
{
	config->ctrlb &= ~(DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm);
	config->ctrlb |= (level << DMA_CH_ERRINTLVL_gp)
			| (level << DMA_CH_TRNINTLVL_gp);
}

/**
 * \brief Set DMA channel source address reload mode
 *
 * This function helps the caller setting the DMA channel source address reload
 * mode. The reload modes are defined in the toolchain header files in the form
 * of \a DMA_CH_SRCRELOAD_*_gc, where * is NONE, BLOCK, BURST, TRANSACTION.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param mode DMA channel source address reload mode given by an
 *             DMA_CH_SRCRELOAD_t type
 */
static inline void dma_channel_set_src_reload_mode(
		struct dma_channel_config *config,
		DMA_CH_SRCRELOAD_t mode)
{
	config->addrctrl &= ~DMA_CH_SRCRELOAD_gm;
	config->addrctrl |= mode;
}

/**
 * \brief Set DMA channel destination address reload mode
 *
 * This function helps the caller setting the DMA channel destination address
 * reload mode. The reload modes are defined in the toolchain header files in
 * the form of \a DMA_CH_DESTRELOAD_*_gc, where * is NONE, BLOCK, BURST,
 * TRANSACTION.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param mode DMA channel destination address reload mode given by an
 *             DMA_CH_DESTRELOAD_t type
 */
static inline void dma_channel_set_dest_reload_mode(
		struct dma_channel_config *config,
		DMA_CH_DESTRELOAD_t mode)
{
	config->addrctrl &= ~DMA_CH_DESTRELOAD_gm;
	config->addrctrl |= mode;
}

/**
 * \brief Set DMA channel source addressing mode
 *
 * This function helps the caller setting the DMA channel source addressing
 * mode. The addressing modes are defined in the toolchain header files in the
 * form of \a DMA_CH_SRCDIR_*_gc, where * is FIXED, INC, DEC.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param mode DMA channel source addressing mode given by an
 *             DMA_CH_SRCDIR_t type
 */
static inline void dma_channel_set_src_dir_mode(
		struct dma_channel_config *config,
		DMA_CH_SRCDIR_t mode)
{
	config->addrctrl &= ~DMA_CH_SRCDIR_gm;
	config->addrctrl |= mode;
}

/**
 * \brief Set DMA channel destination addressing mode
 *
 * This function helps the caller setting the DMA channel destination
 * addressing mode. The reload modes are defined in the toolchain header files
 * in the form of \a DMA_CH_DESTDIR_*_gc, where * is FIXED, INC, DEC.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param mode DMA channel destination addressing mode given by an
 *             DMA_CH_DESTDIR_t type
 */
static inline void dma_channel_set_dest_dir_mode(
		struct dma_channel_config *config,
		DMA_CH_DESTDIR_t mode)
{
	config->addrctrl &= ~DMA_CH_DESTDIR_gm;
	config->addrctrl |= mode;
}

/**
 * \brief Set DMA channel trigger source
 *
 * This function helps the caller setting the DMA channel trigger source. The
 * trigger sources are defined in the toolchain header files in the form of \a
 * DMA_CH_TRIGSRC_*_gc, where * represents the various sources.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param source DMA channel trigger source given by a DMA_CH_TRIGSRC_t type
 */
static inline void dma_channel_set_trigger_source(
		struct dma_channel_config *config,
		DMA_CH_TRIGSRC_t source)
{
	config->trigsrc = source;
}

/**
 * \brief Set DMA channel transfer count
 *
 * This function helps the caller setting the DMA channel number of bytes in
 * each block transfer.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param count Number of bytes in each block transfer
 */
static inline void dma_channel_set_transfer_count(
		struct dma_channel_config *config,
		uint16_t count)
{
	config->trfcnt = count;
}

/**
 * \brief Set DMA channel number of transfer repeats
 *
 * This function helps the caller setting the DMA channel number of block
 * transfer repeats. It will also enable the repeat feature.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param repeats Number of block transfer repeats
 */
static inline void dma_channel_set_repeats(struct dma_channel_config *config,
		uint8_t repeats)
{
	config->ctrla |= DMA_CH_REPEAT_bm;
	config->repcnt = repeats;
}

/**
 * \brief Set DMA channel 16-bit destination address
 *
 * This function helps the caller setting the DMA channel destination 16-bit
 * LSB address.
 *
 * \note This function assumes the \a config was initialized with zeros (static
 * or intentionally zeroed). If not zero filled, there might be garbage left in
 * the upper byte of the destination address.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param destination 16-bit LSB destination address
 */
static inline void dma_channel_set_destination_address(
		struct dma_channel_config *config,
		uint16_t destination)
{
	config->destaddr16 = destination;
}

#ifdef CONFIG_HAVE_HUGEMEM

/**
 * \brief Set DMA channel hugemem destination address
 *
 * This function helps the caller setting the DMA channel destination address.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param destination Destination address provided by a \ref hugemem_ptr_t type
 */
static inline void dma_channel_set_destination_hugemem(
		struct dma_channel_config *config,
		hugemem_ptr_t destination)
{
	config->destaddr = destination;
}

#endif

/**
 * \brief Set DMA channel 16-bit source address
 *
 * This function helps the caller setting the DMA channel source 16-bit LSB
 * address.
 *
 * \note This function assumes the \a config was initialized with zeros (static
 * or intentionally zeroed). If not zero filled, there might be garbage left in
 * the upper byte of the source address.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param source 16-bit LSB source address
 */
static inline void dma_channel_set_source_address(
		struct dma_channel_config *config,
		uint16_t source)
{
	config->srcaddr16 = source;
}

#ifdef CONFIG_HAVE_HUGEMEM

/**
 * \brief Set DMA channel hugemem source address
 *
 * This function helps the caller setting the DMA channel source address.
 *
 * \param config Pointer to a \ref dma_channel_config variable
 * \param source Source address provided by a \ref hugemem_ptr_t type
 */
static inline void dma_channel_set_source_hugemem(
		struct dma_channel_config *config,
		hugemem_ptr_t source)
{
	config->srcaddr = source;
}

#endif
/** @} */

/** @} */

/**
 * \page xmega_dma_quickstart Quick start guide for the XMEGA DMA driver
 *
 * This is the quick start guide for the \ref dma_group "DMA driver", with
 * step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section dma_use_cases Advanced use cases
 * For more advanced use of the DMA driver, see the following use cases:
 * - \subpage dma_use_case_1
 * - \subpage dma_use_case_2
 *
 * \section dma_basic_use_case Basic use case
 * In this basic use case, the DMA is configured for:
 * - Burst length: 1 byte
 * - Transfer count: 1024
 * - Source: Buffer located in RAM
 * - Destination: Buffer located in RAM
 * - Source and destination address reload mode: End of transaction
 * - Source and destination address direction mode: Increment
 *
 * In this use case data is copied from the source buffer to the destination
 * buffer in 1-byte bursts, until all data in the block is transferred. This
 * example is analogus to a <code>memcpy(destination, source, sizeof(source))</code>
 * operation performed in hardware asynchronously to the CPU.
 *
 * \section dma_basic_use_case_setup Setup steps
 *
 * \subsection dma_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 *
 * \subsection dma_basic_use_case_setup_setup_code Example code
 * Add to application C-file:
 * \code
 *    #define DMA_CHANNEL     0
 *    #define DMA_BUFFER_SIZE 1024
 *
 *    static uint8_t source[DMA_BUFFER_SIZE];
 *    static uint8_t destination[DMA_BUFFER_SIZE];
 *
 *    static void dma_init(void)
 *    {
 *        struct dma_channel_config dmach_conf;
 *        memset(&dmach_conf, 0, sizeof(dmach_conf));
 *
 *        dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_1BYTE_gc);
 *        dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *
 *        dma_channel_set_src_reload_mode(&dmach_conf,
 *               DMA_CH_SRCRELOAD_TRANSACTION_gc);
 *        dma_channel_set_dest_reload_mode(&dmach_conf,
 *               DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *
 *        dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 *        dma_channel_set_source_address(&dmach_conf,
 *                (uint16_t)(uintptr_t)source);
 *
 *        dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *        dma_channel_set_destination_address(&dmach_conf,
 *                (uint16_t)(uintptr_t)destination);
 *
 *        dma_enable();
 *
 *        dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 *        dma_channel_enable(DMA_CHANNEL);
 *    }
 * \endcode
 *
 * Add to \c main():
 * \code
 * sysclk_init();
 * dma_init();
 * \endcode
 *
 * \subsection dma_basic_use_case_setup_flow Workflow
 * -# Define the DMA channel that will be used for the transfer for convenience:
 *   - \code #define DMA_CHANNEL     0 \endcode
 * -# Define the array length that will be the used for the source and
 *    destination buffers located in RAM:
 *   - \code #define DMA_BUFFER_SIZE 1024 \endcode
 * -# Create a pair of global arrays that will hold the source and destination
 *    data copied by the DMA controller channel when it is triggered:
 *   - \code
 * static uint8_t source[DMA_BUFFER_SIZE];
 * static uint8_t destination[DMA_BUFFER_SIZE];
 *     \endcode
 * -# Create a function \c dma_init() to intialize the DMA:
 *     - \code
 * static void dma_init(void)
 * {
 *     // ...
 * }
 *       \endcode
 * -# Create config struct for DMA channel:
 *   - \code struct dma_channel_config       dmach_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&dmach_conf, 0, sizeof(dmach_conf)); \endcode
 * -# Configure the DMA channel for single byte bursts, with a transfer length
 *    equal to the size of the source and destination buffers:
 *   - \code
 * dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_1BYTE_gc);
 * dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *     \endcode
 * -# Configure the DMA channel to reset the source and destination addresses at
 *    the end of the complete transaction (i.e. after \c DMA_BUFFER_SIZE bytes
 *    copied):
 *   - \code
 * dma_channel_set_src_reload_mode(&dmach_conf, DMA_CH_SRCRELOAD_TRANSACTION_gc);
 * dma_channel_set_dest_reload_mode(&dmach_conf, DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *     \endcode
 * -# Configure the DMA channel to increment the source and destination
 *    addresses after each byte transferred:
 *   - \code
 * dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 * dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *     \endcode
 * -# Configure the DMA channel source and destination addresses:
 *   - \code
 * dma_channel_set_source_address(&dmach_conf, (uint16_t)(uintptr_t)source);
 * dma_channel_set_destination_address(&dmach_conf, (uint16_t)(uintptr_t)destination);
 *     \endcode
 * -# Enable the DMA module so that channels can be configured in it:
 *      - \code dma_enable(); \endcode
 *  \attention Calling dma_enable() will result in a soft-reset of the entire
 *             DMA module, clearing all channel configurations. If more than one
 *             DMA channel is to be configured, this function should be called
 *             only once in the application initialization procedure only.
 * -# Write the DMA channel configuration to the DMA and enable it so that
 *    it can be triggered to start the transfer:
 *   - \code
 * dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 * dma_channel_enable(DMA_CHANNEL);
 *     \endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 * -# Call our DMA init function:
 *      - \code dma_init(); \endcode
 *
 * \section dma_basic_use_case_usage Usage steps
 *
 * \subsection dma_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
 *    dma_channel_trigger_block_transfer(DMA_CHANNEL);
 *    do {} while (dma_get_channel_status(DMA_CHANNEL) != DMA_CH_TRANSFER_COMPLETED);
 * \endcode
 *
 * \subsection dma_basic_use_case_usage_flow Workflow
 * -# Start the DMA transfer:
 *   - \code dma_channel_trigger_block_transfer(DMA_CHANNEL); \endcode
 * -# Wait for the transfer to complete:
 *   - \code do {} while (dma_get_channel_status(DMA_CHANNEL) != DMA_CH_TRANSFER_COMPLETED);
 *     \endcode
 */

/**
 * \page dma_use_case_1 Use case #1 - Interrupt for a completed DMA transfer.
 *
 * This use case shows how to set up an interrupt for when a DMA transfer is
 * completed.
 *
 * In this use case, the DMA is configured for:
 * - Interrupt level: Low
 * - Burst length: 1 byte
 * - Transfer count: 1024
 * - Source: Buffer located in RAM
 * - Destination: Buffer located in RAM
 * - Source and destination address reload mode: End of transaction
 * - Source and destination address direction mode: Increment
 *
 * In this use case data is copied from the source buffer to the destination
 * buffer in 1-byte bursts, until all data in the block is transferred. This
 * example is analogus to a <code>memcpy(destination, source, sizeof(source))</code>
 * operation performed in hardware asynchronously to the CPU.
 *
 * Each time the DMA transfer completes, a user-specified callback function is
 * run to notify the user application.
 *
 * \section dma_use_case_1_setup Setup steps
 *
 * \subsection dma_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 * -# \ref sleepmgr_group "Sleep Manager Service"
 * -# \ref pmic_group "PMIC Driver"
 *
 * A \ref dma_callback_t "callback" function, called \c dma_transfer_done, must
 * also be provided by the user application:
 * \code
 *   static void dma_transfer_done(enum dma_channel_status status)
 *   {
 *       // ...
 *   }
 * \endcode
 *
 * \subsection dma_use_case_1_setup_code Example code
 * Add to application C-file:
 * \code
 *    #define DMA_CHANNEL     0
 *    #define DMA_BUFFER_SIZE 1024
 *
 *    static uint8_t source[DMA_BUFFER_SIZE];
 *    static uint8_t destination[DMA_BUFFER_SIZE];
 *
 *    static void dma_init(void)
 *    {
 *        struct dma_channel_config dmach_conf;
 *        memset(&dmach_conf, 0, sizeof(dmach_conf));
 *
 *        dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_1BYTE_gc);
 *        dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *
 *        dma_channel_set_src_reload_mode(&dmach_conf,
 *               DMA_CH_SRCRELOAD_TRANSACTION_gc);
 *        dma_channel_set_dest_reload_mode(&dmach_conf,
 *               DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *
 *        dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 *        dma_channel_set_source_address(&dmach_conf,
 *                (uint16_t)(uintptr_t)source);
 *
 *        dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *        dma_channel_set_destination_address(&dmach_conf,
 *                (uint16_t)(uintptr_t)destination);
 *
 *        dma_enable();
 *
 *        dma_set_callback(DMA_CHANNEL, dma_transfer_done);
 *        dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);
 *
 *        dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 *        dma_channel_enable(DMA_CHANNEL);
 *    }
 * \endcode
 *
 * Add to \c main():
 * \code
 * sysclk_init();
 * pmic_init();
 * sleepmgr_init();
 * dma_init();
 * cpu_irq_enable();
 * \endcode
 *
 * \subsection dma_use_case_1_setup_flow Workflow
 * -# Define the DMA channel that will be used for the transfer for convenience:
 *   - \code #define DMA_CHANNEL     0 \endcode
 * -# Define the array length that will be the used for the source and
 *    destination buffers located in RAM:
 *   - \code #define DMA_BUFFER_SIZE 1024 \endcode
 * -# Create a pair of global arrays that will hold the source and destination
 *    data copied by the DMA controller channel when it is triggered:
 *   - \code
 * static uint8_t source[DMA_BUFFER_SIZE];
 * static uint8_t destination[DMA_BUFFER_SIZE];
 *     \endcode
 * -# Create a function \c dma_init() to intialize the DMA:
 *     - \code
 * static void dma_init(void)
 * {
 *     // ...
 * }
 *       \endcode
 * -# Create config struct for DMA channel:
 *   - \code struct dma_channel_config       dmach_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&dmach_conf, 0, sizeof(dmach_conf)); \endcode
 * -# Configure the DMA channel for single byte bursts, with a transfer length
 *    equal to the size of the source and destination buffers:
 *   - \code
 * dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_1BYTE_gc);
 * dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *     \endcode
 * -# Configure the DMA channel to reset the source and destination addresses at
 *    the end of the complete transaction (i.e. after \c DMA_BUFFER_SIZE bytes
 *    copied):
 *   - \code
 * dma_channel_set_src_reload_mode(&dmach_conf, DMA_CH_SRCRELOAD_TRANSACTION_gc);
 * dma_channel_set_dest_reload_mode(&dmach_conf, DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *     \endcode
 * -# Configure the DMA channel to increment the source and destination
 *    addresses after each byte transferred:
 *   - \code
 * dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 * dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *     \endcode
 * -# Configure the DMA channel source and destination addresses:
 *   - \code
 * dma_channel_set_source_address(&dmach_conf, (uint16_t)(uintptr_t)source);
 * dma_channel_set_destination_address(&dmach_conf,
 *         (uint16_t)(uintptr_t)destination);
 *     \endcode
 * -# Enable the DMA module so that channels can be configured in it:
 *      - \code dma_enable(); \endcode
 *  \attention Calling dma_enable() will result in a soft-reset of the entire
 *             DMA module, clearing all channel configurations. If more than one
 *             DMA channel is to be configured, this function should be called
 *             only once in the application initialization procedure only.
 * -# Set up the DMA channel interrupt to run at low interrupt priority, and
 *    link it to the user created \c dma_transfer_done() function:
 *      - \code
 * dma_set_callback(DMA_CHANNEL, dma_transfer_done);
 * dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);
 *        \endcode
 * -# Write the DMA channel configuration to the DMA and enable it so that
 *    it can be triggered to start the transfer:
 *   - \code
 * dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 * dma_channel_enable(DMA_CHANNEL);
 *     \endcode
 * -# Initialize the clock system, PMIC driver and Sleep Manager service:
 *      - \code
 * sysclk_init();
 * pmic_init();
 * sleepmgr_init();
 *        \endcode
 * -# Call our DMA init function:
 *      - \code dma_init(); \endcode
 * -# Enable global processing of interrupts:
 *      - \code cpu_irq_enable(); \endcode
 *
 * \section dma_use_case_1_usage Usage steps
 *
 * \subsection dma_use_case_1_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
 *    dma_channel_trigger_block_transfer(DMA_CHANNEL);
 *    sleepmgr_enter_sleep();
 * \endcode
 *
 * \subsection dma_use_case_1_usage_flow Workflow
 * -# Start the DMA transfer:
 *   - \code dma_channel_trigger_block_transfer(DMA_CHANNEL); \endcode
 * -# Sleep while waiting for the transfer to complete:
 *   - \code sleepmgr_enter_sleep(); \endcode
 */

/**
 * \page dma_use_case_2 Use case #2 - Event driven DMA transfer from a peripheral.
 *
 * This use case shows how to set up a burst DMA transfer between a peripheral
 * register set and the main system memory.
 *
 * In this use case, the DMA is configured for:
 * - Burst length: 2 bytes (one 16-bit result from the ADC peripheral)
 * - Transfer count: 1024 (512 16-bit samples)
 * - Source: ADC channel 0 result register
 * - Destination: Buffer located in RAM
 * - Source address reload mode: End of burst
 * - Destination address reload mode: End of transaction
 * - Source and destination address direction mode: Increment
 *
 * In this use case data is copied from the 16-bit ADC channel 0 result register
 * to a buffer in RAM, each time the ADC indicates that a new conversion has
 * completed.
 *
 * \section dma_use_case_2_setup Setup steps
 *
 * \subsection dma_basic_use_case_setup_prereq Prerequisites
 * For the setup code of this use case to work, the following must
 * be added to the project:
 * -# \ref sysclk_group "System Clock Manager Service (sysclk)"
 *
 * The ADC must be configured according to the XMEGA ADC driver Quick Start
 * \ref adc_basic_use_case "basic use case".
 *
 * \subsection dma_use_case_2_setup_code Example code
 * Add to application C-file:
 * \code
 *    #define DMA_CHANNEL     0
 *    #define DMA_BUFFER_SIZE 1024
 *
 *    static uint16_t adc_samples[DMA_BUFFER_SIZE / 2];
 *
 *    static void dma_init(void)
 *    {
 *        struct dma_channel_config dmach_conf;
 *        memset(&dmach_conf, 0, sizeof(dmach_conf));
 *
 *        dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_2BYTE_gc);
 *        dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *
 *        dma_channel_set_src_reload_mode(&dmach_conf,
 *              DMA_CH_SRCRELOAD_BURST_gc);
 *        dma_channel_set_dest_reload_mode(&dmach_conf,
 *              DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *
 *        dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 *        dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *
 *        dma_channel_set_source_address(&dmach_conf,
 *              (uint16_t)(uintptr_t)&ADCA.CH0RES);
 *        dma_channel_set_destination_address(&dmach_conf,
 *              (uint16_t)(uintptr_t)adc_samples);
 *
 *        dma_channel_set_trigger_source(&dmach_conf, DMA_CH_TRIGSRC_ADCA_CH0_gc);
 *        dma_channel_set_single_shot(&dmach_conf);
 *
 *        dma_enable();
 *
 *        dma_set_callback(DMA_CHANNEL, dma_transfer_done);
 *        dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);
 *
 *        dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 *        dma_channel_enable(DMA_CHANNEL);
 *    }
 * \endcode
 *
 * Add to \c main():
 * \code
 * sysclk_init();
 * dma_init();
 * \endcode
 *
 * \subsection dma_use_case_2_setup_flow Workflow
 * -# Define the DMA channel that will be used for the transfer for convenience:
 *   - \code #define DMA_CHANNEL     0 \endcode
 * -# Define the array length that will be the used for the source and
 *    destination buffers located in RAM:
 *   - \code #define DMA_BUFFER_SIZE 1024 \endcode
 * -# Create a global array that will hold the ADC sample result data copied by
 *    the DMA controller channel when it is triggered (the buffer size is
 *    halved as each sample is two bytes long):
 *   - \code static uint16_t adc_samples[DMA_BUFFER_SIZE / 2]; \endcode
 * -# Create a function \c dma_init() to intialize the DMA:
 *     - \code
 * static void dma_init(void)
 * {
 *     // ...
 * }
 *       \endcode
 * -# Create config struct for DMA channel:
 *   - \code struct dma_channel_config       dmach_conf; \endcode
 * -# Make sure the configuration structure is zeroed out to ensure that all
 *    values are reset to their defaults before writing new values:
 *   - \code memset(&dmach_conf, 0, sizeof(dmach_conf)); \endcode
 * -# Configure the DMA channel for two byte bursts (the size of a single ADC
 *    conversion) with a transfer length equal to the size of the destination
 *    buffer:
 *   - \code
 * dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_2BYTE_gc);
 * dma_channel_set_transfer_count(&dmach_conf, DMA_BUFFER_SIZE);
 *     \endcode
 * -# Configure the DMA channel to reset the source address at the end of each
 *    burst transfer, and the destination addresses at the end of the complete
 *    transaction (i.e. after \c DMA_BUFFER_SIZE bytes copied):
 *   - \code
 * dma_channel_set_src_reload_mode(&dmach_conf, DMA_CH_SRCRELOAD_BURST_gc);
 * dma_channel_set_dest_reload_mode(&dmach_conf, DMA_CH_DESTRELOAD_TRANSACTION_gc);
 *     \endcode
 * -# Configure the DMA channel to increment the source and destination
 *    addresses after each byte transferred:
 *   - \code
 * dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
 * dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);
 *     \endcode
 * -# Configure the DMA channel source and destination addresses to the ADC
 *    module channel 0 result registers and RAM buffer respectively:
 *   - \code
 * dma_channel_set_source_address(&dmach_conf,
 *         (uint16_t)(uintptr_t)&ADCA.CH0RES);
 * dma_channel_set_destination_address(&dmach_conf,
 *         (uint16_t)(uintptr_t)adc_samples);
 *     \endcode
 * -# Set the DMA channel trigger source to the ADC module channel 0 complete
 *    event:
 *   - \code dma_channel_set_trigger_source(&dmach_conf, DMA_CH_TRIGSRC_ADCA_CH0_gc); \endcode
 * -# Configure the DMA channel in single shot mode, so that each time it is
 *    triggered it will perform one bust transfer only:
 *   - \code dma_channel_set_single_shot(&dmach_conf); \endcode
 * -# Enable the DMA module so that channels can be configured in it:
 *      - \code dma_enable(); \endcode
 *  \attention Calling dma_enable() will result in a soft-reset of the entire
 *             DMA module, clearing all channel configurations. If more than one
 *             DMA channel is to be configured, this function should be called
 *             only once in the application initialization procedure only.
 * -# Set up the DMA channel interrupt to run at low interrupt priority, and
 *    link it to the user created \c dma_transfer_done() function:
 *      - \code
 * dma_set_callback(DMA_CHANNEL, dma_transfer_done);
 * dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);
 *        \endcode
 * -# Write the DMA channel configuration to the DMA and enable it so that
 *    it can be triggered to start the transfer:
 *   - \code
 * dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
 * dma_channel_enable(DMA_CHANNEL);
 *     \endcode
 * -# Initialize the clock system:
 *      - \code sysclk_init(); \endcode
 * -# Call our DMA init function:
 *      - \code dma_init(); \endcode
 *
 * \section dma_use_case_2_usage Usage steps
 *
 * \subsection dma_use_case_2_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 *     \code
 * adc_start_conversion(&ADCA, ADC_CH0);
 * adc_wait_for_interrupt_flag(&ADCA, ADC_CH0);
 *     \endcode
 *
 * \subsection dma_use_case_2_usage_flow Workflow
 * -# Start an ADC conversion, result will be automatically copied to the
 *    global buffer when complete:
 *   - \code adc_start_conversion(&ADCA, ADC_CH0); \endcode
 * -# Wait for the ADC to complete before triggering the next conversion by
 *    polling the ADC channel 0 complete interrupt flag:
 *   - \code adc_wait_for_interrupt_flag(&ADCA, ADC_CH0); \endcode
 */

#endif /* DRIVERS_DMA_DMA_H */
