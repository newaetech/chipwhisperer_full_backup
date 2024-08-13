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

#ifndef __FW_MANAGER_COMM_H__
#define __FW_MANAGER_COMM_H__

#include "qm_soc_regs.h"
#include "fw-manager_config.h"

/*
 * SoC-specific UART comm parameters.
 */

#if (QUARK_SE)
#if (FM_CONFIG_UART == 0)
#define FM_COMM_UART_PIN_TX_ID (QM_PIN_ID_18)
#define FM_COMM_UART_PIN_TX_FN (QM_PMUX_FN_0)
#define FM_COMM_UART_PIN_RX_ID (QM_PIN_ID_19)
#define FM_COMM_UART_PIN_RX_FN (QM_PMUX_FN_0)
#define FM_COMM_UART_CLK (CLK_PERIPH_UARTA_REGISTER)
#define fm_comm_irq_request() qm_irq_request(QM_IRQ_UART_0_INT, qm_uart_0_isr)
#elif(FM_CONFIG_UART == 1)
#define FM_COMM_UART_PIN_TX_ID (QM_PIN_ID_16)
#define FM_COMM_UART_PIN_TX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_PIN_RX_ID (QM_PIN_ID_17)
#define FM_COMM_UART_PIN_RX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_IRQ (QM_IRQ_UART_1_INT)
#define FM_COMM_UART_CLK (CLK_PERIPH_UARTB_REGISTER)
#define fm_comm_irq_request() qm_irq_request(QM_IRQ_UART_1_INT, qm_uart_1_isr)
#else
#error "Invalid UART ID for FM comm"
#endif /* FM_CONFIG_UART */
#elif(QUARK_D2000)
#if (FM_CONFIG_UART == 0)
#define FM_COMM_UART_PIN_TX_ID (QM_PIN_ID_12)
#define FM_COMM_UART_PIN_TX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_PIN_RX_ID (QM_PIN_ID_13)
#define FM_COMM_UART_PIN_RX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_CLK (CLK_PERIPH_UARTA_REGISTER)
#define fm_comm_irq_request() qm_irq_request(QM_IRQ_UART_0_INT, qm_uart_0_isr)
#elif(FM_CONFIG_UART == 1)
#define FM_COMM_UART_PIN_TX_ID (QM_PIN_ID_20)
#define FM_COMM_UART_PIN_TX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_PIN_RX_ID (QM_PIN_ID_21)
#define FM_COMM_UART_PIN_RX_FN (QM_PMUX_FN_2)
#define FM_COMM_UART_IRQ (QM_IRQ_UART_1_INT)
#define FM_COMM_UART_CLK (CLK_PERIPH_UARTB_REGISTER)
#define fm_comm_irq_request() qm_irq_request(QM_IRQ_UART_1_INT, qm_uart_1_isr)
#else
#error "Invalid UART ID for FM comm"
#endif /* FM_CONFIG_UART */
#else
#error "SOC not supported"
#endif

#endif /* __FW_MANAGER_COMM_H__ */
