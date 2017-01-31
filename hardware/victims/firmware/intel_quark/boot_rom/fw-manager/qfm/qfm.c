/**
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

#include "qfm_packets.h"
#include "bl_data.h"
#include "../dfu/dfu.h"
/* qfu_format.h included because of authentication enum (qfm_auth_type_t) */
#include "../qfu/qfu_format.h"

/**
 * The size of the buffer used by the QFM module. It is equal to the size of
 * the biggest QFM packet, which is QFM_SYS_INFO_RSP.
 */
#define QFM_BUF_SIZE (sizeof(qfm_sys_info_rsp_t))

#if (QUARK_SE)
#define QFM_SYS_INFO_INIT_SOC_TYPE (QFM_SOC_TYPE_QUARK_SE)
#define QFM_SYS_INFO_INIT_TARGET_LIST                                          \
	{                                                                      \
		{                                                              \
			.target_type = QFM_TARGET_TYPE_X86                     \
		}                                                              \
		,                                                              \
		{                                                              \
			.target_type = QFM_TARGET_TYPE_SENSOR                  \
		}                                                              \
	}
#elif(QUARK_D2000)
#define QFM_SYS_INFO_INIT_SOC_TYPE (QFM_SOC_TYPE_QUARK_D2000)
#define QFM_SYS_INFO_INIT_TARGET_LIST                                          \
	{                                                                      \
		{                                                              \
			.target_type = QFM_TARGET_TYPE_X86                     \
		}                                                              \
	}
#endif

/*-----------------------------------------------------------------------*/
/* FORWARD DECLARATIONS                                                  */
/*-----------------------------------------------------------------------*/
static void qfm_init(uint8_t alt_setting);
static void qfm_get_processing_status(dfu_dev_status_t *status,
				      uint32_t *poll_timeout_ms);
static void qfm_clear_status();
static void qfm_dnl_process_block(uint32_t block_num, const uint8_t *data,
				  uint16_t len);
static int qfm_dnl_finalize_transfer();
static void qfm_upl_fill_block(uint32_t block_num, uint8_t *data,
			       uint16_t max_len, uint16_t *len);
static void qfm_abort_transfer();

/*-----------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                      */
/*-----------------------------------------------------------------------*/
/**
 * QFM request handler variable (used by DFU core when alternate setting zero
 * is selected).
 */
const dfu_request_handler_t qfm_dfu_rh = {
    &qfm_init, &qfm_get_processing_status, &qfm_clear_status,
    &qfm_dnl_process_block, &qfm_dnl_finalize_transfer, &qfm_upl_fill_block,
    &qfm_abort_transfer,
};

static qfm_sys_info_rsp_t sys_info_rsp = {
    .qfm_pkt_type = QFM_SYS_INFO_RSP,
    .sysupd_version = 0,
    .soc_type = QFM_SYS_INFO_INIT_SOC_TYPE,
    .auth_type = QFU_AUTH_NONE,
    .target_count = BL_BOOT_TARGETS_NUM,
    .partition_count = BL_FLASH_PARTITIONS_NUM,
    .targets = QFM_SYS_INFO_INIT_TARGET_LIST,
};

/**
 * The buffer where we store incoming/outgoing QFM packets.
 */
static struct qfm_buf {
	unsigned int len;	   /**< The length of the data in the buffer */
	uint8_t data[QFM_BUF_SIZE]; /**< The actual buffer */
} qfm_buf;

/**
 * The DFU status of this DFU request handler.
 */
static dfu_dev_status_t dfu_status;

/*-----------------------------------------------------------------------*/
/* STATIC FUNCTIONS (QFM functions)                                      */
/*-----------------------------------------------------------------------*/
/**
 * Prepare a QFM System Information response (QFM_SYS_INFO_RSP) packet.
 *
 * This function is called when a QFM System Information request
 * (QFM_SYS_INFO_REQ) is received.
 */
static void prepare_sys_info_rsp()
{
	int i;
	bl_flash_partition_t *part;

	for (i = 0; i < BL_FLASH_PARTITIONS_NUM; i++) {
		part = &bl_data->partitions[i];
		sys_info_rsp.partitions[i].app_present =
		    (*part->start_addr != 0xFFFFFFFF);
		sys_info_rsp.partitions[i].app_version = part->app_version;
	}
	for (i = 0; i < BL_BOOT_TARGETS_NUM; i++) {
		sys_info_rsp.targets[i].active_partition_idx =
		    bl_data->targets[i].active_partition_idx;
	}

	qfm_buf.len = sizeof(sys_info_rsp);
}

/**
 * Application Erase.
 *
 * Erase all application code from flash.
 */
static void app_erase()
{
	int i;
	uint32_t page;
	bl_flash_partition_t *part;

	/*
	 * First update bl-data by marking every partition as inconsistent and
	 * setting the app version of each partition to undefined.
	 */
	for (i = 0; i < BL_FLASH_PARTITIONS_NUM; i++) {
		part = &bl_data->partitions[i];
		part->is_consistent = false;
	}
	bl_data_shadow_writeback();
	/*
	 * Then actually erase the partitions.
	 */
	for (i = 0; i < BL_FLASH_PARTITIONS_NUM; i++) {
		part = &bl_data->partitions[i];
		for (page = part->first_page;
		     page < (part->first_page + part->num_pages); page++) {
			qm_flash_page_erase(part->controller,
					    QM_FLASH_REGION_SYS, page);
		}
	}
	/* Finally mark back partitions as consistent */
	/*
	 * Note: updating bl-data twice is required to correctly handle failures
	 * (e.g., power cuts) during the erase process.
	 */
	for (i = 0; i < BL_FLASH_PARTITIONS_NUM; i++) {
		part = &bl_data->partitions[i];
		part->is_consistent = true;
	}
	bl_data_shadow_writeback();
}

/**
 * Parse and process the incoming QFM request.
 *
 * This function is called every time a DFU_DNLOAD transfer is finalized (since
 * in QFM mode DFU_DNLOAD transfers carry QFM requests).
 * This function has no parameter since the request is expected to be in the
 * QFM buffer (qfm_buf global variable).
 */
static void process_qfm_req()
{
	qfm_generic_pkt_t *pkt;

	pkt = (qfm_generic_pkt_t *)qfm_buf.data;
	switch (pkt->type) {
	case QFM_SYS_INFO_REQ:
		prepare_sys_info_rsp();
		break;
	case QFM_APP_ERASE:
		/*
		 * App erase takes just a few ms so we can safely perform it
		 * here, instead of replying to the DFU_DNLOAD request first.
		 */
		app_erase();
		/* No QFM response for this request; set len to zero */
		qfm_buf.len = 0;
		break;
	case QFM_UPDATE_KEY:
	/* Not yet supported, handle it as an invalid message */
	default:
		dfu_status = DFU_STATUS_ERR_VENDOR;
		/* No QFM response in this case; set len to zero */
		qfm_buf.len = 0;
	}
}

/*-----------------------------------------------------------------------*/
/* STATIC FUNCTIONS (DFU Request Handler implementation)                 */
/*-----------------------------------------------------------------------*/

/**
 * Initialize the QFM DFU Request Handler.
 *
 * This function is called by the DFU logic when the QFM alternate setting is
 * selected (i.e., alternate setting 0).
 *
 * @param[in] alt_setting The alternate setting triggering this handler (always
 * 			  0 in the case of this handler).
 */
static void qfm_init(uint8_t alt_setting)
{
	/* alt_setting is not needed by the QFM DFU request handler */
	(void)alt_setting;
	dfu_status = DFU_STATUS_OK;
}

/**
 * Get the status and state of the handler.
 *
 * @param[out] status The status of the processing (OK or error code)
 * @param[out] poll_timeout The poll_timeout to be returned to the host.
 * 			     Must be zero if the processing is over; otherwise
 * 			     it	should be the expected remaining time.
 */
static void qfm_get_processing_status(dfu_dev_status_t *status,
				      uint32_t *poll_timeout_ms)
{
	*status = dfu_status;
	*poll_timeout_ms = 0;
}

/**
 * Clear the status and state of the handler.
 *
 * This function is used to reset the handler state machine after an error. It
 * is called by DFU core when a DFU_CLRSTATUS request is recevied.
 */
static void qfm_clear_status()
{
	dfu_status = DFU_STATUS_OK;
	qfm_buf.len = 0;
}

/**
 * Process a DFU_DNLOAD block.
 *
 * @param[in] block_num The block number.
 * @param[in] data      The buffer containing the block data.
 * @param[in] len       The length of the block.
 */
static void qfm_dnl_process_block(uint32_t block_num, const uint8_t *data,
				  uint16_t len)
{
	if (block_num == 0) {
		qfm_buf.len = 0;
	}
	if (len > (sizeof(qfm_buf.data) - qfm_buf.len)) {
		dfu_status = DFU_STATUS_ERR_VENDOR;
		return;
	}
	memcpy(&qfm_buf.data[qfm_buf.len], data, len);
	qfm_buf.len += len;
}

/**
 * Finalize the current DFU_DNLOAD transfer.
 *
 * This function is called by DFU Core when an empty DFU_DNLOAD request
 * (signaling the end of the current DFU_DNLOAD transfer) is received.
 *
 * @return QMSI return code.
 * @retval 0    if handler agrees with the end of the transfer.
 * @retval -EIO if handler was actually expecting more data.
 */
static int qfm_dnl_finalize_transfer()
{
	process_qfm_req();

	return 0;
}

/**
 * Fill up a DFU_UPLOAD block.
 *
 * This function is called by the DFU logic when a request for an
 * UPLOAD block is received. The handler is in charge of filling the
 * payload of the block.
 *
 * When QFM mode (i.e., alternate setting 0) is active, the host sends a
 * DFU_UPLOAD request to retrieve the response to the QFM Request previously
 * sent in DFU_DNLOAD transfer. Note, however, that not every QFM request
 * produces a QFM response (for instance, the QFM Application Erase request).
 *
 * @param[in]  blk_num The block number (first block is always block 0).
 * @param[out] data    The buffer where the payload will be put.
 * @param[in]  req_len The amount of data requested by the host.
 * @param[out] len     A pointer to the variable where to store the actual
 * 		       amount of data provided by the handler (len < req_len
 * 		       means that there is no more data to send, i.e., this
 * 		       is the last block).
 */
static void qfm_upl_fill_block(uint32_t blk_num, uint8_t *data,
			       uint16_t req_len, uint16_t *len)
{
	(void)blk_num;
	/*
	 * For the sake of code-size minimization, we require the host to use a
	 * block size (i.e., req_len) greater than the response length.  In
	 * other words, the response must fit in a single UPLOAD block.  This
	 * is not a huge limitation, since there is no reason for the host to
	 * use a block size smaller than the device's maximum block size
	 * (typically a few kB).
	 */
	if ((qfm_buf.len > 0) && (req_len >= qfm_buf.len)) {
		memcpy(data, &sys_info_rsp, qfm_buf.len);
		*len = qfm_buf.len;
	} else {
		/* We upload nothing */
		*len = 0;
	}
}

/**
 * Abort current DNLOAD/UPLOAD transfer and go back to handler's initial state.
 *
 * This function is called by DFU core when a DFU_ABORT request is received.
 */
static void qfm_abort_transfer()
{
	qfm_buf.len = 0;
}
