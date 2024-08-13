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

#ifndef __FM_ENTRY_H__
#define __FM_ENTRY_H__

/**
 * Enter FM mode Macro.
 *
 * The behavior of this macro depends on the ENABLE_FIRMWARE_MANAGER build
 * option:
 *    - If ENABLE_FIRMWARE_MANAGER=uart, then the 'FM over UART' mode is
 * 	activated
 *    - If ENABLE_FIRMWARE_MANAGER=2nd-stage, the 2nd-stage bootloader is
 *      executed; the actual transport used for FM will depend on the specific
 *      2nd-stage bootloader flashed.
 */
#if ENABLE_FIRMWARE_MANAGER_2ND_STAGE
#define fm_entry(...) fm_entry_2nd_stage(__VA_ARGS__)
#endif
#if ENABLE_FIRMWARE_MANAGER_UART
#define fm_entry(...) fm_entry_uart(__VA_ARGS__)
#endif
#if ENABLE_FIRMWARE_MANAGER_USB
#define fm_entry(...) fm_entry_usb(__VA_ARGS__)
#endif
#if UNIT_TEST
/* Must be defined by unit tests. */
void fm_entry(void);
#endif

/**
 * Start Firmware Manager located in 2nd-stage bootloader.
 *
 * The transport used by FM mode will depend on the specific 2nd-stage
 * bootloader flashed.
 */
void fm_entry_2nd_stage(void);

/**
 * Start UART-based Firmware Manager.
 *
 * FM mode will use UART as transport.
 */
void fm_entry_uart(void);

/**
 * Start USB-based Firmware Manager.
 *
 * FM mode will use USB as transport.
 */
void fm_entry_usb(void);

#endif /* __FM_ENTRY_H__ */
