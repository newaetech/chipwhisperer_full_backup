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
#include "qm_init.h"

#include "qda_packets.h"
#include "../dfu.h"
#include "../dfu_config.h"
#include "../core/dfu_core.h"
#include "xmodem.h"
#include "xmodem_io_uart.h"

#define DFU_BLOCK_SIZE (QM_FLASH_PAGE_SIZE_BYTES)
/* Additional XMODEM_BLOCK_SIZE bytes needed because of QDA overhead */
#define QDA_BUF_SIZE (DFU_BLOCK_SIZE + XMODEM_BLOCK_SIZE)

/*--------------------------------------------------------------------------*/
/*                              MACROS                                      */
/*--------------------------------------------------------------------------*/

#define STALL_AND_BREAK_ON_ERR(func)                                           \
	if ((func) != 0) {                                                     \
		qda_stall();                                                   \
		break;                                                         \
	}

/*--------------------------------------------------------------------------*/
/*                    GLOBAL VARIABLES                                      */
/*--------------------------------------------------------------------------*/

/** The buffer for incoming and outgoing QDA packets. */
/*
 * NOTE: optimization: for outgoing packets, consider using pre-compiled
 * packets instead of preparing them dynamically using qda_buf
 */
static uint8_t qda_buf[QDA_BUF_SIZE];

/*--------------------------------------------------------------------------*/
/*                           FORWARD DECLARATIONS                           */
/*--------------------------------------------------------------------------*/
static void qda_process_pkt(uint8_t *data, size_t len);
static void qda_ack();
static void qda_stall();
static void handle_upload_req(qda_upl_req_payload_t *req);
static void qda_dfu_get_status_rsp(dfu_dev_state_t state,
				   dfu_dev_status_t status,
				   uint32_t poll_timeout);
static void qda_dfu_get_state_rsp(dfu_dev_state_t state);
static void qda_dev_dsc_rsp();
static void qda_dfu_dsc_rsp();

/*--------------------------------------------------------------------------*/
/*                            GLOBAL FUNCTIONS                              */
/*--------------------------------------------------------------------------*/
/**
 * Initialize QDA module.
 *
 * Initialize the Quark DFU Adaptation (QDA) module.
 *
 * @param[in] cfg	The DFU configuration.
 */
/* NOTE: we can remove this init function and use global variables/functions for
 * optimization purposes */
void qda_init()
{
	xmodem_io_uart_init();
	dfu_init();
}

/*
 * Receive and process QDA packets.
 *
 * Receive and process QDA packets, until the communication becomes idle (i.e.,
 * no data is received for a certain amount of time).
 */
void qda_receive_loop()
{
	int len;

	do {
		/*
		 * Receive a new packet using XMODEM.
		 *
		 * xmodem_receive() is blocking: the function returns when the
		 * XMODEM transfer is completed or an unrecoverable reception
		 * error occurs (e.g., a transmission starts, but then
		 * timeouts or the maximum number of retries is exceeded). The
		 * function returns the length of the received data on success,
		 * a negative error code otherwise.
		 */
		len = xmodem_receive_package(qda_buf, sizeof(qda_buf));
		if (len > 0) {
			qda_process_pkt(qda_buf, len);
		}
		/*
		 * NOTE: for this function to work properly, XMODEM must be
		 * changed to return a special value when the failure is due to
		 * a timeout and not an error.
		 *
		 * For now we do not distinguish between a timeout and an
		 * unrecoverable error.
		 */
	} while (len > 0);
}

/*--------------------------------------------------------------------------*/
/*                    STATIC FUNCTION DEFINITION                            */
/*--------------------------------------------------------------------------*/

/**
 * Process QDA packet.
 *
 * Parse, process, and reply to an incoming QDA packet.
 *
 * @param[in] data The buffer containing the packet.
 * @param[in] len  The length of packet or its upper bound (since XMODEM may
 * 		   add some padding bytes).
 */
static void qda_process_pkt(uint8_t *data, size_t len)
{
	qda_pkt_t *pkt;
	qda_dnl_req_payload_t *dnload_req;
	qda_upl_req_payload_t *upload_req;
	qda_set_alt_setting_payload_t *altset_req;
	size_t expected_len;

	dfu_dev_state_t state;
	dfu_dev_status_t status;
	uint32_t poll_timeout;

	pkt = (qda_pkt_t *)data;
	expected_len = sizeof(*pkt);

	switch (pkt->type) {
	case QDA_PKT_DEV_DESC_REQ:
		qda_dev_dsc_rsp();
		break;
	case QDA_PKT_DFU_DESC_REQ:
		qda_dfu_dsc_rsp();
		break;
	case QDA_PKT_DFU_SET_ALT_SETTING:
		altset_req = (qda_set_alt_setting_payload_t *)pkt->payload;
		STALL_AND_BREAK_ON_ERR(
		    dfu_set_alt_setting(altset_req->alt_setting));
		qda_ack();
		break;
	case QDA_PKT_DFU_DNLOAD_REQ:
		dnload_req = (qda_dnl_req_payload_t *)pkt->payload;
		expected_len += sizeof(*dnload_req) + dnload_req->data_len;
		if ((len < expected_len) ||
		    dfu_process_dnload(dnload_req->block_num, dnload_req->data,
				       dnload_req->data_len)) {
			qda_stall();
			break;
		}
		qda_ack();
		break;
	case QDA_PKT_DFU_UPLOAD_REQ:
		upload_req = (qda_upl_req_payload_t *)pkt->payload;
		/* UPLOAD requests are handled differently from the others in
		 * order to reuse qda_buf */
		handle_upload_req(upload_req);
		break;
	case QDA_PKT_DFU_GETSTATUS_REQ:
		STALL_AND_BREAK_ON_ERR(
		    dfu_get_status(&status, &state, &poll_timeout));
		qda_dfu_get_status_rsp(state, status, poll_timeout);
		break;
	case QDA_PKT_DFU_CLRSTATUS:
		STALL_AND_BREAK_ON_ERR(dfu_clr_status());
		qda_ack();
		break;
	case QDA_PKT_DFU_GETSTATE_REQ:
		STALL_AND_BREAK_ON_ERR(dfu_get_state(&state));
		qda_dfu_get_state_rsp(state);
		break;
	case QDA_PKT_DFU_ABORT:
		STALL_AND_BREAK_ON_ERR(dfu_abort());
		qda_ack();
		break;
	case QDA_PKT_RESET:
		qda_ack();
		qm_soc_reset(QM_COLD_RESET);
		break;
	/* QDA_PKT_DFU_DETACH should not be received */
	default:
		/* NOTE: how to handle bad QDA requests? Send a QDA_STALL
		 * message for now */
		qda_stall();
		break;
	}
}

/*
 * USB Ack response
 *
 * -------------
 * |4B|TYPE    |
 * -------------
 */
static void qda_ack()
{
	qda_pkt_t *pkt;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_ACK;

	xmodem_transmit_package(qda_buf, sizeof(*pkt));
}

/*
 * USB Stall response
 *
 * -------------
 * |4B|TYPE    |
 * -------------
 */
static void qda_stall()
{
	qda_pkt_t *pkt;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_STALL;

	xmodem_transmit_package(qda_buf, sizeof(*pkt));
}

/*
 * DFU_UPLOAD response
 *
 * -------------
 * |4B|TYPE    |
 * ------------|
 * |2B|DATA_LEN|
 * ------------|
 * |xB|DATA    |
 * -------------
 *
 */
static void handle_upload_req(qda_upl_req_payload_t *req)
{
	uint16_t max_len;
	uint16_t block_num;
	qda_pkt_t *pkt;
	qda_upl_rsp_payload_t *rsp;
	int retv;

	/* store request parameters in temporary variables */
	block_num = req->block_num;
	max_len = req->max_data_len;

	/* prepare upload response packet */
	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_DFU_UPLOAD_RSP;
	rsp = (qda_upl_rsp_payload_t *)pkt->payload;
	retv =
	    dfu_process_upload(block_num, max_len, rsp->data, &rsp->data_len);
	if (retv == 0) {
		xmodem_transmit_package(qda_buf, sizeof(*pkt) + sizeof(*rsp) +
						     rsp->data_len);
	} else {
		qda_stall();
	}
}

/*
 * DFU_GETSTATUS response
 *
 * -----------------
 * |4B|TYPE        |
 * ----------------|
 * |1B|STATUS      |
 * ----------------|
 * |3B|POLL_TIMEOUT|
 * -----------------
 * |1B|STATE       |
 * -----------------
 *
 */
static void qda_dfu_get_status_rsp(dfu_dev_state_t state,
				   dfu_dev_status_t status,
				   uint32_t poll_timeout)
{
	qda_pkt_t *pkt;
	qda_get_status_rsp_payload_t *rsp;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_DFU_GETSTATUS_RSP;
	rsp = (qda_get_status_rsp_payload_t *)pkt->payload;
	rsp->status = status;
	rsp->poll_timeout = poll_timeout;
	rsp->state = state;

	xmodem_transmit_package(qda_buf, sizeof(*pkt) + sizeof(*rsp));
}

/*
 * DFU_GETSTATE response
 *
 * -----------------
 * |4B|TYPE        |
 * -----------------
 * |1B|STATE       |
 * -----------------
 */
static void qda_dfu_get_state_rsp(dfu_dev_state_t state)
{
	qda_pkt_t *pkt;
	qda_get_state_rsp_payload_t *rsp;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_DFU_GETSTATE_RSP;
	rsp = (qda_get_state_rsp_payload_t *)pkt->payload;
	rsp->state = state;

	xmodem_transmit_package(qda_buf, sizeof(*pkt) + sizeof(*rsp));
}

/*
 * Reply with a USB Device Descriptor response
 * -----------------
 * |4B|TYPE        |
 * -----------------
 * |2B|VENDOR_ID   |
 * -----------------
 * |2B|PRODUCT_ID  |
 * -----------------
 * |2B|DEVICE_BCD  |
 * -----------------
 */
static void qda_dev_dsc_rsp()
{
	qda_pkt_t *pkt;
	qda_dev_dsc_rsp_payload_t *rsp;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_DEV_DESC_RSP;
	rsp = (qda_dev_dsc_rsp_payload_t *)pkt->payload;
	rsp->id_vendor = DFU_CFG_VID;
	rsp->id_product = DFU_CFG_PID_DFU;
	rsp->bcd_device = DFU_CFG_DEV_BCD;

	xmodem_transmit_package(qda_buf, sizeof(*pkt) + sizeof(*rsp));
}

/*
 * Reply with a DFU Descriptors response
 *
 * ----------------------
 * |4B|TYPE             |
 * ----------------------
 * |1B|NUM_ALT_SETTINGS |
 * ----------------------
 * |1B|DFU_ATTRIBUTES   |
 * ----------------------
 * |2B|DETACH_TIMEOUT   |
 * ----------------------
 * |2B|MAX BLOCK SIZE   |
 * ----------------------
 * |2B|DFU_VERSION      |
 * ----------------------
 */
static void qda_dfu_dsc_rsp()
{
	qda_pkt_t *pkt;
	qda_dfu_dsc_rsp_payload_t *rsp;

	pkt = (qda_pkt_t *)qda_buf;
	pkt->type = QDA_PKT_DFU_DESC_RSP;
	rsp = (qda_dfu_dsc_rsp_payload_t *)pkt->payload;
	rsp->num_alt_settings = DFU_CFG_NUM_ALT_SETTINGS;
	rsp->bm_attributes = DFU_CFG_DFU_ATTRIBUTES;
	rsp->detach_timeout = DFU_CFG_DETACH_TIMEOUT;
	rsp->transfer_size = DFU_CFG_MAX_BLOCK_SIZE;
	rsp->bcd_dfu_ver = DFU_CFG_DFU_VERSION_BCD;

	xmodem_transmit_package(qda_buf, sizeof(*pkt) + sizeof(*rsp));
}
