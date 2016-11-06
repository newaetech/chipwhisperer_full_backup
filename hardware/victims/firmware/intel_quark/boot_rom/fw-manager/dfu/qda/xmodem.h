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

#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <stdint.h>

/** XMODEM block size */
#define XMODEM_BLOCK_SIZE (128)

/**
 * @defgroup groupXMODEM XMODEM
 * @{
 */

/**
 * Switch XMODEM to receive mode.
 *
 * XMODEM starts to send 'C' (NAK-CRC) messages to the sender and waits for
 * incoming transmissions. Received data is copied into the provided buffer.
 *
 * This function is blocking and it does not timeout.
 *
 * @param[out] buf      Buffer where to store the received data.
 * @param[in]  buf_size The size of the buffer.
 *
 * @return Number of received bytes or negative error code. Note that XMODEM
 *         may add up to 127 padding bytes at the end of the real data.
 * @retval >0 Number of received bytes (including padding).
 * @retval -1 Error (either the reception failed for an unrecoverable protocol
 * 	      error or the provided buffer is too small)
 */
int xmodem_receive_package(uint8_t *buf, size_t buf_size);

/**
 * Switch XMODEM to transmit mode.
 *
 * XMODEM waits for 'C' (NAK-CRC) messages until the transmission begins.  The
 * package content is sent in 128 bytes frames. Extra (padding) data is added
 * to the last frame if the data size is not multiple of 128 bytes.
 *
 * This function is blocking, but may timeout.
 *
 * @param[in] data The data to send.
 * @param[in] len  The length of the data.
 *
 * @return 0 on success, negative error code otherwise.
 * @retval 0 Success.
 * @retval -1 Error (timeout or number of retries exceeded).
 */
int xmodem_transmit_package(uint8_t *data, size_t len);

/**
 * @}
 */

#endif
