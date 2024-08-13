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

#ifndef __QDA_PACKETS_H__
#define __QDA_PACKETS_H__

#include <stdint.h>

#define __QM_ATTR_PACKED__ __attribute__((__packed__))

/**
 * QDA packet types
 */
typedef enum {
	/* Host requests */
	QDA_PKT_RESET = 0x4D550000,
	QDA_PKT_DEV_DESC_REQ = 0x4D550005,
	QDA_PKT_DFU_DESC_REQ = 0x4D5501FF,
	QDA_PKT_DFU_SET_ALT_SETTING = 0x4D5501FE,
	QDA_PKT_DFU_DETACH = 0x4D550100,
	QDA_PKT_DFU_DNLOAD_REQ = 0x4D550101,
	QDA_PKT_DFU_UPLOAD_REQ = 0x4D550102,
	QDA_PKT_DFU_GETSTATUS_REQ = 0x4D550103,
	QDA_PKT_DFU_CLRSTATUS = 0x4D550104,
	QDA_PKT_DFU_GETSTATE_REQ = 0x4D550105,
	QDA_PKT_DFU_ABORT = 0x4D550106,
	/* Device responses */
	QDA_PKT_ATTACH = 0x4D558001,
	QDA_PKT_DETACH = 0x4D558002,
	QDA_PKT_ACK = 0x4D558003,
	QDA_PKT_STALL = 0x4D558004,
	QDA_PKT_DEV_DESC_RSP = 0x4D558005,
	QDA_PKT_DFU_DESC_RSP = 0x4D5581FF,
	QDA_PKT_DFU_UPLOAD_RSP = 0x4D558102,
	QDA_PKT_DFU_GETSTATUS_RSP = 0x4D558103,
	QDA_PKT_DFU_GETSTATE_RSP = 0x4D558105,

} qda_pkt_type_t;

/**
 * Generic QDA Packet structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint32_t type;
	uint8_t payload[];
} qda_pkt_t;

/**
 * QDA_DNLOAD_REQ payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint16_t data_len;
	uint16_t block_num;
	uint8_t data[];
} qda_dnl_req_payload_t;

/**
 * QDA_UPLOAD_REQ payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint16_t max_data_len;
	uint16_t block_num;
} qda_upl_req_payload_t;

/**
 * QDA_USB_SET_ALT_SETTING payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint8_t alt_setting;
} qda_set_alt_setting_payload_t;

/**
 * QDA_UPLOAD_RSP payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint16_t data_len;
	uint8_t data[];
} qda_upl_rsp_payload_t;

/**
 * QDA_USB_DEV_DSC_RSP payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint16_t id_vendor;
	uint16_t id_product;
	uint16_t bcd_device;
} qda_dev_dsc_rsp_payload_t;

/**
 * QDA_DFU_DSC_RSP payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint8_t num_alt_settings;
	uint8_t bm_attributes;
	uint16_t detach_timeout;
	uint16_t transfer_size;
	uint16_t bcd_dfu_ver;
} qda_dfu_dsc_rsp_payload_t;

/**
 * QDA_GET_STATUS_RSP payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	/*
	 * As per the DFU spec, poll_timeout is 3-byte integer, but QDA stores
	 * it in a 4-byte field for simplicity
	 */
	uint32_t poll_timeout;
	uint8_t status;
	uint8_t state;
} qda_get_status_rsp_payload_t;

/**
 * QDA_GET_STATE_RSP payload structure
 */
typedef struct __QM_ATTR_PACKED__ {
	uint8_t state;
} qda_get_state_rsp_payload_t;

#endif /* __QDA_PACKETS_H__ */
