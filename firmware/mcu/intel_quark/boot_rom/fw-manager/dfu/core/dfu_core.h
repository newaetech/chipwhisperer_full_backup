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

#ifndef __DFU_CORE_H__
#define __DFU_CORE_H__

#include <stdint.h>

#include "qm_common.h"
#include "../dfu.h"

/**
 * Device Firmware Upgrade (DFU) Core Logic.
 *
 * Implementation of the DFU state machine.
 *
 * @defgroup groupDFU DFU Core
 * @{
 */

/**
 * Initialize the DFU Core module.
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_init();

/**
 * Handle a USB Reset event
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_usb_reset();

/**
 * Handle a USB Set Alternate Setting request for the DFU interface
 *
 * @param alt_setting The alt_setting number to be activate
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_set_alt_setting(uint8_t alt_setting);

/**
 * Handle a DFU_DETACH request
 *
 * @param[in] timeout_ms The time (in ms) within which the device must detach
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_detach(uint16_t timeout_ms);

/**
 * Handle a DFU_DNLOAD request
 *
 * @param[in] bock_num The block sequence number
 * @param[in] data     The buffer containing the block data
 * @param[in] len      The size of the block, i.e., the amount of data in the
 * 		       buffer
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_process_dnload(uint16_t block_num, const uint8_t *data, uint16_t len);

/**
 * Handle a DFU_UPLOAD request
 *
 *
 * @param[in]  block_num The block sequence number
 * @param[in]  req_len   The size of the block, i.e., the amount of data the
 * 			 host is requesting
 * @param[out] data	 The buffer where to write the requested data
 * @param[out] data_len  The actual amount of data provided by the host (the
 * 			 device shall never provide more data then req_len,
 * 			 while it can provide less data to notify the host
 * 			 that it has no more data to send)
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_process_upload(uint16_t block_num, uint16_t max_len, uint8_t *data,
		       uint16_t *data_len);

/**
 * Handle a DFU_GETSTATUS request
 *
 * @param[out] status	       A pointer to the variable where to store the DFU
 * 			       status
 * @param[out] state	       A pointer to the variable where to store the DFU
 * 			       state
 * @param[out] poll_timeout_ms A pointer to the variable where to store the poll
 * 			       timeout (i.e., the time the host has to wait
 * 			       before issuing another GET_STATUS request). The
 * 			       value is in ms.
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_get_status(dfu_dev_status_t *status, dfu_dev_state_t *state,
		   uint32_t *poll_timeout_ms);

/**
 * Handle a DFU_CLRSTATUS request
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_clr_status();

/**
 * Handle a DFU_GETSTATE request
 *
 * @param[out] state A pointer to the variable where to store the DFU state
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_get_state(dfu_dev_state_t *state);

/**
 * Handle a DFU_ABORT request
 *
 * @return  0 if no error has occurred, an error code otherwise
 */
int dfu_abort();

/**
 * @}
 */

#endif /* __DFU_CORE_H__ */
