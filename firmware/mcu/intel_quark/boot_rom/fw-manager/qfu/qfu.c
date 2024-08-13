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

#include "qm_common.h"
#include "qm_flash.h"
#include "clk.h"

#include "qfu_format.h"
#include "bl_data.h"
#include "../dfu/dfu.h"

/*
 * If !ENABLE_FIRMWARE_MANAGER, we are compiling this code for an example or
 * debugging app, so we enable the debugging printf and replace calls to
 * qm_flash with calls to the debugging printf.
 */
#if (!ENABLE_FIRMWARE_MANAGER)
#define DBG_PRINTF(...) QM_PRINTF(__VA_ARGS__)
#define qm_flash_page_write(ctrl, reg, pg, data, len)                          \
	do {                                                                   \
		DBG_PRINTF("[SUPPRESSED] qm_flash_page_write()\n");            \
		(void) pg;                                                     \
	} while (0);
#else
#define DBG_PRINTF(...)
#endif

/*
 * NOTE: the DFU block size is defined in multiple places and in different ways;
 * a patch should be made to have just one definition.
 */
#define DFU_BLOCK_SIZE_MAX (QM_FLASH_PAGE_SIZE_BYTES)

/*
 * The block number of the first data block; to be changed to a variable once
 * authentication is implemented (it will be 1 in case of no authentication, 2
 * or more in case of authentication).
 */
#define FIRST_DATA_BLK_NUM (1)

/*-----------------------------------------------------------------------*/
/* FORWARD DECLARATIONS                                                  */
/*-----------------------------------------------------------------------*/
static void qfu_init(uint8_t alt_setting);
static void qfu_get_status(dfu_dev_status_t *status, uint32_t *poll_timeout_ms);
static void qfu_clear_status();
static void qfu_dnl_process_block(uint32_t block_num, const uint8_t *data,
				  uint16_t len);
static int qfu_dnl_finalize_transfer();
static void qfu_upl_fill_block(uint32_t block_num, uint8_t *data,
			       uint16_t max_len, uint16_t *len);
static void qfu_abort_transfer();

/*-----------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                      */
/*-----------------------------------------------------------------------*/
/**
 * QFU request handler variable (used by DFU core when an alternate setting
 * different from 0 is selected).
 */
const dfu_request_handler_t qfu_dfu_rh = {
    &qfu_init, &qfu_get_status, &qfu_clear_status, &qfu_dnl_process_block,
    &qfu_dnl_finalize_transfer, &qfu_upl_fill_block, &qfu_abort_transfer,
};

/** The DFU (error) status of this DFU request handler. */
static dfu_dev_status_t qfu_err_status;

/** The partition associated with the current alternate setting. */
static bl_flash_partition_t *part;
/** The current alternate setting; needed to verify the QFU header. */
static uint8_t active_alt_setting;

/** The header of the QFU image being processed. */
static qfu_hdr_t img_hdr;

/**
 * Prepare BL-Data Section to firmware update.
 *
 * In case of single-bank configuration, the active partition for the
 * partition's target is set to invalid and the updated BL-Data is stored in
 * the Backup copy; then BL-Data Main is erased. In case of dual-bank
 * configuration nothing is done.
 */
static void prepare_bl_data()
{
	/* Flag partition as invalid */
	part->is_consistent = false;
	/* Write back bl-data to flash */
	bl_data_shadow_writeback();
}

/**
 * Handle a block expected to contain a QFU header.
 *
 * @param[in] data The block to be processed.
 * @param[in] len  The len of the block.
 */
static dfu_dev_status_t qfu_handle_hdr(const uint8_t *data, uint32_t len)
{
	DBG_PRINTF("handle_qfu_hdr(): len = %u\n", len);
	qfu_hdr_t *hdr;
	uint32_t extra_blocks;

	hdr = (qfu_hdr_t *)data;
	if (len < sizeof(qfu_hdr_t) || hdr->partition != active_alt_setting) {
		return DFU_STATUS_ERR_ADDRESS;
	}
	/* NOTE: todo: verify the other header fields (vid, pid, etc.) */
	/* NOTE: fix me: for now we force block size to be equal to page size */
	if (hdr->block_sz != QM_FLASH_PAGE_SIZE_BYTES) {
		DBG_PRINTF("Block size error: %d\n", hdr->block_sz);
		return DFU_STATUS_ERR_FILE;
	}
	/* If no auth, then we just have 1 header block (this one) */
	extra_blocks = (hdr->cipher == QFU_AUTH_NONE) ? 1 : 2;
	/* Image size cannot be bigger than the partition size */
	if ((hdr->n_blocks) > (part->num_pages + extra_blocks)) {
		return DFU_STATUS_ERR_ADDRESS;
	}
	/* Store header in RAM */
	memcpy(&img_hdr, hdr, sizeof(img_hdr));

	return DFU_STATUS_OK;
}

/**
 * Handle a block expected to contain a QFU data block to be written to flash.
 *
 * @param[in] data The block to be processed.
 * @param[in] len  The len of the block.
 */
static dfu_dev_status_t qfu_handle_blk(uint32_t blk_num, const uint8_t *data,
				       uint32_t len)
{
	DBG_PRINTF("handle_qfu_blk(): blk_num = %u; len = %u\n", blk_num, len);
	uint32_t page_num;

	/*
	 * Verify block validity:
	 * - block_num must be < number of blocks declared in header
	 * - len must be equal to declared block size, with the exception of
	 *   the last, which can be smaller (but not greater!)
	 */
	if (blk_num >= img_hdr.n_blocks || len > img_hdr.block_sz ||
	    (blk_num + 1 < img_hdr.n_blocks && len != img_hdr.block_sz)) {
		return DFU_STATUS_ERR_ADDRESS;
	}
	/* If first data block, perform anti-brownout check */
	if (blk_num == FIRST_DATA_BLK_NUM) {
		/* NOTE: to do: perform anti-brownout check */
		prepare_bl_data();
	}
	/*
	 * Express len as a multiple of the word size, rounding it up if needed.
	 *
	 * The round up is needed since the size of the last block may not be a
	 * multiple of the word size. The extra bytes we write in such a case
	 * is just garbage from the previous block.
	 */
	len = (len + sizeof(uint32_t) - 1) / sizeof(uint32_t);
	page_num = part->first_page + (blk_num - FIRST_DATA_BLK_NUM);
	qm_flash_page_write(part->controller, QM_FLASH_REGION_SYS, page_num,
			    (uint32_t *)data, len);

	return DFU_STATUS_OK;
}

/*-----------------------------------------------------------------------*/
/* STATIC FUNCTIONS (DFU Request Handler implementation)                 */
/*-----------------------------------------------------------------------*/

/**
 * Initialize the QFU DFU Request Handler.
 *
 * This function is called when a QFU alt setting is selected (i.e., every
 * alternate setting > 0).
 *
 * @param[in] alt_setting The specific alternate setting activating this
 * 			  handler.
 */
static void qfu_init(uint8_t alt_setting)
{
	active_alt_setting = alt_setting;
	/* Decrement alt setting since first QFU alt setting is 1 and not 0 */
	part = &bl_data->partitions[alt_setting - 1];
	qfu_err_status = DFU_STATUS_OK;
}

/**
 * Get the status and state of the handler.
 *
 * @param[out] status The status of the processing (OK or error code)
 * @param[out] poll_timeout_ms The poll_timeout to be returned to the host.
 * 			       Must be zero if the processing is over; otherwise
 * 			       it should be the expected remaining time.
 */
static void qfu_get_status(dfu_dev_status_t *status, uint32_t *poll_timeout_ms)
{
	*status = qfu_err_status;
	/*
	 * NOTE: poll_timeout is always set to zero because the flash is
	 * updated in qfu_dnl_process_block() (i.e., as soon as the block is
	 * received). This is fine for QDA but may need to be changed for USB.
	 */
	*poll_timeout_ms = 0;
}

/**
 * Clear the status and state of the handler.
 *
 * This function is used to reset the handler state machine after an error. It
 * is called by DFU core when a DFU_CLRSTATUS request is received.
 */
static void qfu_clear_status()
{
	qfu_err_status = DFU_STATUS_OK;
}

/**
 * Process a DFU_DNLOAD block.
 *
 * @param[in] block_num The block number.
 * @param[in] data      The buffer containing the block data.
 * @param[in] len       The length of the block.
 */
static void qfu_dnl_process_block(uint32_t block_num, const uint8_t *data,
				  uint16_t len)
{
	if (block_num == 0) {
		/* Header block */
		qfu_err_status = qfu_handle_hdr(data, len);
		return;
	}
	if (block_num == 1 && img_hdr.cipher != QFU_AUTH_NONE) {
		/* Authentication block */
		/* NOTE: not supported for now, set error status */
		qfu_err_status = DFU_STATUS_ERR_FILE;
		return;
	}
	/* Data block */
	qfu_err_status = qfu_handle_blk(block_num, data, len);
}

/**
 * Finalize the current DFU_DNLOAD transfer.
 *
 * This function is called by DFU Core when an empty DFU_DNLOAD request
 * (signaling the end of the current DFU_DNLOAD transfer) is received.
 *
 * NOTE: in case of the QFU handler, this is where bootloader data (e.g.,
 * application version, SVN, image selector, etc.) get updated with information
 * about the new application firmware.
 *
 * @return O on success, negative errno otherwise.
 */
static int qfu_dnl_finalize_transfer()
{
	DBG_PRINTF("Finalize update\n");
	int t_idx;
	/*
	 * NOTE: to do: add a block_cnt parameter and check that
	 * block_cnt ==  qfu_buf.block_num
	 */

	part->is_consistent = true;
	part->app_version = img_hdr.version;
	t_idx = part->target_idx;
	bl_data->targets[t_idx].active_partition_idx = active_alt_setting - 1;
	bl_data_shadow_writeback();

	return 0;
}

/**
 * Fill up a DFU_UPLOAD block.
 *
 * This function is called by the DFU logic when a request for an
 * UPLOAD block is received. The handler is in charge of filling the
 * payload of the block.
 *
 * NOTE: when the QFU handler is active (i.e., the selected alternate setting
 * is different from 0), DFU_UPLOAD requests are not allowed and therefore an
 * empty payload is always returned.
 *
 * @param[in]  blk_num The block number (first block is always block 0).
 * @param[out] data    The buffer where the payload will be put.
 * @param[in]  req_len The amount of data requested by the host.
 * @param[out] len     A pointer to the variable where to store the actual
 * 		       amount of data provided by the handler (len < req_len
 * 		       means that there is no more data to send, i.e., this
 * 		       is the last block).
 */
static void qfu_upl_fill_block(uint32_t blk_num, uint8_t *data,
			       uint16_t req_len, uint16_t *len)
{
	/* Firmware extraction is not allowed: upload nothing */
	(void)blk_num;
	(void)data;
	(void)req_len;
	*len = 0;
}

/**
 * Abort current DNLOAD/UPLOAD transfer and go back to handler's initial state.
 *
 * This function is called by DFU core when a DFU_ABORT request is received.
 */
static void qfu_abort_transfer()
{
	/* calling bl_data_sanitize() will copy backup over main if needed */
	bl_data_sanitize();
}
