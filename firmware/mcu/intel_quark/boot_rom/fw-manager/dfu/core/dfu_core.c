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

#include "qm_common.h"

#include "../dfu.h"
#include "../dfu_config.h"
#include "dfu_core.h"
#include "../../qfu/qfu.h"
#include "../../qfm/qfm.h"

/*
 * NOTE: this implementation of the DFU state machine does not handle the
 * following DFU state:
 * - appIDLE
 * - appDETACH
 * - dfuMANIFEST-WAIT-RESET
 *
 * It does not because we never end up there when using QDA.
 */

/** The current DFU state */
static dfu_dev_state_t dfu_state;
/** The current DFU status */
static dfu_dev_status_t dfu_status;
/**
 * The current DFU request handler (i.e., the DFU handler associated with the
 * active alternate setting).
 */
static const dfu_request_handler_t *dfu_rh;
/**
 * Block counter. Used to number the DNLOAD/UPLOAD blocks of a DNLOAD/UPLOAD
 * transfer, always starting from zero. The number is passed to the DFU request
 * handler.
 */
static unsigned int block_cnt;
/**
 * The expected block number of the next DNLOAD/UPLOAD block. This is different
 * from block_cnt since the first block sent by the host is not necessary
 * number zero.
 */
static uint16_t next_block_num;

/*-------------------------------------------------------------------------*/
/*                        STATIC FUNCTIONS                                 */
/*-------------------------------------------------------------------------*/

/**
 * Transition to error state and set error status.
 *
 * @param err_status The error status to set.
 */
static void set_err(dfu_dev_status_t err_status)
{
	dfu_state = DFU_STATE_DFU_ERROR;
	dfu_status = err_status;
}

/**
 * Reset the state machine.
 *
 * Restart from the initial state (DFU_IDLE) and clear the error status (by
 * setting it to DFU_STATUS_OK).
 *
 */
static void reset_status()
{
	dfu_state = DFU_STATE_DFU_IDLE;
	dfu_status = DFU_STATUS_OK;
}

/*-------------------------------------------------------------------------*/
/*                        GLOBAL FUNCTIONS                                 */
/*-------------------------------------------------------------------------*/
/**
 * Initialize the DFU Core module.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_init()
{
	/* Activate the default alt setting, i.e., alt setting 0 */
	/*
	 * NOTE: to be fixed: start in DFU_ERROR state if firmware is missing
	 * or corrupted (dfu_set_alt_setting() makes the state machine start in
	 * DFU_IDLE state and DFU_OK status).
	 */
	return dfu_set_alt_setting(0);
}

/**
 * Handle a USB Reset event.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_usb_reset()
{
	/* NOTE: yet to be implemented; we return error for now */
	return -EIO;
}

/**
 * Handle a USB Set Alternate Setting request for the DFU interface.
 *
 * @param alt_setting The alt_setting number to be activate.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_set_alt_setting(uint8_t alt_setting)
{
	if (alt_setting >= DFU_CFG_NUM_ALT_SETTINGS) {
		return -EIO;
	}
	reset_status();
	/*
	 * Activate the proper handler depending on the selected alt setting:
	 * - QFM if alt_setting == 0
	 * - QFU if alt_setting  > 0
	 */
	dfu_rh = (alt_setting == 0) ? &qfm_dfu_rh : &qfu_dfu_rh;
	dfu_rh->init(alt_setting);

	return 0;
}

/**
 * Handle a DFU_DETACH request.
 *
 * @param[in] timeout_ms The time, in ms, within which the device must detach.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_detach(uint16_t timeout_ms)
{
	/* NOTE: to be implemented for USB; not used for now */
	(void)timeout_ms;

	return -EIO;
}

/**
 * Handle a DFU_DNLOAD request.
 *
 * @param[in] bock_num The block sequence number.
 * @param[in] data     The buffer containing the block data.
 * @param[in] len      The size of the block, i.e., the amount of data in the
 * 		       buffer.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_process_dnload(uint16_t block_num, const uint8_t *data, uint16_t len)
{
	switch (dfu_state) {
	case DFU_STATE_DFU_IDLE:
		if (len == 0) {
			set_err(DFU_STATUS_ERR_STALLEDPKT);
			return -EIO;
		}
		block_cnt = 0;
		break;
	case DFU_STATE_DFU_DNLOAD_IDLE:
		/* If the block is out of order */
		if (block_num != next_block_num) {
			/*
			 * NOTE: this check is not mentioned in the DFU spec;
			 * we keep it for now, but we may consider removing it
			 * for code-size optimization purposes.
			 */
			set_err(DFU_STATUS_ERR_VENDOR);
			return -EIO;
		}
		/* If the block is empty, it signals the end of the download */
		if (len == 0) {
			/* check if finalization is allowed */
			if (dfu_rh->fin_dnload_xfer() == 0) {
				dfu_state = DFU_STATE_DFU_MANIFEST_SYNC;
				return 0;
			} else {
				set_err(DFU_STATUS_ERR_NOTDONE);
				return -EIO;
			}
		}
		break;
	default:
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	}
	/* we end up here if a DNLOAD transfer just started or is continuing */
	next_block_num = block_num + 1;
	dfu_rh->proc_dnload_blk(block_cnt, data, len);
	block_cnt++;
	dfu_state = DFU_STATE_DFU_DNLOAD_SYNC;

	return 0;
}

/**
 * Handle a DFU_UPLOAD request.
 *
 * @param[in]  block_num The block sequence number.
 * @param[in]  req_len   The size of the block, i.e., the amount of data the
 * 			 host is requesting.
 * @param[out] data	 The buffer where to write the requested data.
 * @param[out] data_len  The actual amount of data provided by the host (the
 * 			 device shall never provide more data then req_len,
 * 			 while it can provide less data to notify the host
 * 			 that it has no more data to send).
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_process_upload(uint16_t block_num, uint16_t req_len, uint8_t *data,
		       uint16_t *data_len)
{
	switch (dfu_state) {
	case DFU_STATE_DFU_IDLE:
		block_cnt = 0;
		next_block_num = block_num;
		break;
	case DFU_STATE_DFU_UPLOAD_IDLE:
		break;
	default:
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	}
	if (block_num != next_block_num) {
		/*
		 * NOTE: this check is not mentioned in the DFU spec;
		 * should we keep it?
		 */
		set_err(DFU_STATUS_ERR_VENDOR);
		return -EIO;
	}
	dfu_rh->fill_upload_blk(block_cnt, data, req_len, data_len);
	next_block_num = block_num + 1;
	block_cnt++;
	/* if the device write less bytes than required then upload is over */
	if (*data_len < req_len) {
		dfu_state = DFU_STATE_DFU_IDLE;
	} else {
		dfu_state = DFU_STATE_DFU_UPLOAD_IDLE;
	}

	return 0;
}

/**
 * Handle a DFU_GETSTATUS request
 *
 * @param[out] status	       A pointer to the variable where to store the DFU
 * 			       status.
 * @param[out] state	       A pointer to the variable where to store the DFU
 * 			       state.
 * @param[out] poll_timeout_ms A pointer to the variable where to store the poll
 * 			       timeout (i.e., the time the host has to wait
 * 			       before issuing another GET_STATUS request). The
 * 			       value is in ms.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_get_status(dfu_dev_status_t *status, dfu_dev_state_t *state,
		   uint32_t *poll_timeout_ms)
{
	switch (dfu_state) {
	case DFU_STATE_DFU_DNBUSY:
	case DFU_STATE_DFU_MANIFEST:
		/* The host has to wait for the poll timeout! */
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	case DFU_STATE_DFU_DNLOAD_SYNC:
	case DFU_STATE_DFU_MANIFEST_SYNC:
		/* update the internal dfu_status and get poll_timeout value */
		dfu_rh->get_proc_status(&dfu_status, poll_timeout_ms);
		if (dfu_status != DFU_STATUS_OK) {
			dfu_state = DFU_STATE_DFU_ERROR;
			break;
		}
		if (*poll_timeout_ms == 0) {
			dfu_state = (dfu_state == DFU_STATE_DFU_DNLOAD_SYNC)
					? DFU_STATE_DFU_DNLOAD_IDLE
					: DFU_STATE_DFU_IDLE;
		}
		/*
		 * NOTE: if poll_timeout != 0 we should set a timer and
		 * transition to DFU_STATE_DFU_DNBUSY / DFU_STATE_DFU_MANIFEST.
		 * However, for now, we just remain in the current state,
		 * to avoid using timers.
		 * (This is not a big issue: we are just not enforcing the
		 * waiting time between two consecutive GET_STATUS requests; a
		 * well-design host will respect it anyway).
		 *
		 */
		break;
	default:
		*poll_timeout_ms = 0;
	}
	*state = dfu_state;
	*status = dfu_status;

	return 0;
}

/**
 * Handle a DFU_CLRSTATUS request.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_clr_status()
{
	/* we can receive a CLR_STATUS request only if an error has occurred */
	if (dfu_state != DFU_STATE_DFU_ERROR) {
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	}
	dfu_rh->clr_status();
	reset_status();
	return 0;
}

/**
 * Handle a DFU_GETSTATE request.
 *
 * @param[out] state A pointer to the variable where to store the DFU state.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_get_state(dfu_dev_state_t *state)
{
	switch (dfu_state) {
	case DFU_STATE_DFU_DNBUSY:
	case DFU_STATE_DFU_MANIFEST:
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	default:
		*state = dfu_state;
		return 0;
	}
}

/**
 * Handle a DFU_ABORT request.
 *
 * @return  0 if no error has occurred, an error code otherwise.
 */
int dfu_abort()
{
	switch (dfu_state) {
	case DFU_STATE_DFU_DNLOAD_IDLE:
		dfu_rh->abort_dnload_xfer();
		break;
	case DFU_STATE_DFU_UPLOAD_IDLE:
		break;
	default:
		set_err(DFU_STATUS_ERR_STALLEDPKT);
		return -EIO;
	}
	dfu_state = DFU_STATE_DFU_IDLE;
	return 0;
}
