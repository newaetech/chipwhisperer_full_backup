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

#ifndef __XMODEM_IO_H__
#define __XMODEM_IO_H__

#include <stdint.h>

/**
 * XMODEM I/O Layer Interface.
 *
 * This file describes the functions that an XMODEM I/O layer must implement.
 *
 * @defgroup groupXMODEM_IO XMODEM I/O
 * @{
 */

/*
 * Get a character from the XMODEM I/O layer.
 *
 * This function must be blocking, but is also expected to timeout after a
 * certain amount of time. Moreover, in case of error, the function must not
 * set the output parameter (i.e., the pointed variable must remain unchanged).
 *
 * @param[out] ch A pointer to the variable where to store the read character.
 * 		  In case of error, the current value of the pointed variable
 * 		  is not modified.
 *
 * @return 0 on success, negative error code otherwise.
 * @retval -ETIME in case of timeout.
 * @retval -EIO   in case of I/O error.
 */
int xmodem_io_getc(uint8_t *ch);

/*
 * Pass a character to the XMODEM I/O layer.
 *
 * This function must be blocking.
 *
 * @param[in] ch A pointer to the character to be written.
 *
 * @return 0 on success, negative error code otherwise.
 */
int xmodem_io_putc(const uint8_t *ch);

/** @} */

#endif /* __XMODEM_IO_H__ */
