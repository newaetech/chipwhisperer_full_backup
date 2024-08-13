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

#ifndef __FW_MANAGER_CONFIG_H__
#define __FW_MANAGER_CONFIG_H__

/** Dual-bank configuration flag. */
/* NOTE: consider moving this to a bl_config.h file or rename this file */
#define BL_CONFIG_DUAL_BANK (false)

/*
 * FM UART comm parameters.
 */

#if (QUARK_SE)
#define FM_CONFIG_UART (1)
#elif(QUARK_D2000)
#define FM_CONFIG_UART (0)
#endif
#define FM_CONFIG_UART_BAUD_DIV (BOOTROM_UART_115200)

/* GPIO pin for FM requests. */
#if (QUARK_SE)
/*
 * Intel(R) Quark(TM) Microcontroller SE Development Platform Button 0 / Pin
 * J14.43 (AON GPIO 4).
 */
#define FM_CONFIG_USE_AON_GPIO_PORT (1)
#define FM_CONFIG_GPIO_PIN (4)
#elif(QUARK_D2000)
/*
 * Intel(R) Quark(TM) Microcontroller D2000 Development Platform Button 0 / Pin
 * J4.5 (GPIO 2).
 */
#define FM_CONFIG_GPIO_PIN (2)
#endif

#endif /* __FW_MANAGER_CONFIG_H__ */
