/**
 * \file
 *
 * \brief Universal Asynchronous Receiver Transceiver (UART) driver for SAM.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include "compiler.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/* UART internal div factor for sampling */
#define UART_MCK_DIV             16
/* Div factor to get the maximum baud rate */
#define UART_MCK_DIV_MIN_FACTOR  1
/* Div factor to get the minimum baud rate */
#define UART_MCK_DIV_MAX_FACTOR  65535

/*! \brief Option list for UART peripheral initialization */
typedef struct sam_uart_opt {
	/** MCK for UART */
	uint32_t ul_mck;
	/** Expected baud rate */
	uint32_t ul_baudrate;
	/** Initialize value for UART mode register */
	uint32_t ul_mode;
} sam_uart_opt_t;

uint32_t uart_init(Uart *p_uart, const sam_uart_opt_t *p_uart_opt);
void uart_enable_tx(Uart *p_uart);
void uart_disable_tx(Uart *p_uart);
void uart_reset_tx(Uart *p_uart);
void uart_enable_rx(Uart *p_uart);
void uart_disable_rx(Uart *p_uart);
void uart_reset_rx(Uart *p_uart);
void uart_enable(Uart *p_uart);
void uart_disable(Uart *p_uart);
void uart_reset(Uart *p_uart);
void uart_enable_interrupt(Uart *p_uart, uint32_t ul_sources);
void uart_disable_interrupt(Uart *p_uart, uint32_t ul_sources);
uint32_t uart_get_interrupt_mask(Uart *p_uart);
uint32_t uart_get_status(Uart *p_uart);
void uart_reset_status(Uart *p_uart);
uint32_t uart_is_tx_ready(Uart *p_uart);
uint32_t uart_is_tx_empty(Uart *p_uart);
uint32_t uart_is_rx_ready(Uart *p_uart);
uint32_t uart_is_tx_buf_empty(Uart *p_uart);
void uart_set_clock_divisor(Uart *p_uart, uint16_t us_divisor);
uint32_t uart_write(Uart *p_uart, const uint8_t uc_data);
uint32_t uart_read(Uart *p_uart, uint8_t *puc_data);
#if (!SAMV71 && !SAMV70 && !SAME70 && !SAMS70)
uint32_t uart_is_rx_buf_end(Uart *p_uart);
uint32_t uart_is_tx_buf_end(Uart *p_uart);
uint32_t uart_is_rx_buf_full(Uart *p_uart);
Pdc *uart_get_pdc_base(Uart *p_uart);
#endif
#if (SAMG53 || SAMG54 || SAMV71 || SAMV70 || SAME70 || SAMS70)
void uart_set_sleepwalking(Uart *p_uart, uint8_t ul_low_value,
		bool cmpmode, bool cmppar, uint8_t ul_high_value);
void uart_set_write_protection(Uart *p_uart, bool flag);
#endif

#if (SAM4C || SAM4CP || SAM4CM)
enum uart_optical_duty_cycle {
	UART_MOD_CLK_DUTY_50_00 = UART_MR_OPT_DUTY_DUTY_50,
	UART_MOD_CLK_DUTY_43_75 = UART_MR_OPT_DUTY_DUTY_43P75,
	UART_MOD_CLK_DUTY_37_50 = UART_MR_OPT_DUTY_DUTY_37P5,
	UART_MOD_CLK_DUTY_31_25 = UART_MR_OPT_DUTY_DUTY_31P25,
	UART_MOD_CLK_DUTY_25_00 = UART_MR_OPT_DUTY_DUTY_25,
	UART_MOD_CLK_DUTY_18_75 = UART_MR_OPT_DUTY_DUTY_18P75,
	UART_MOD_CLK_DUTY_12_50 = UART_MR_OPT_DUTY_DUTY_12P5,
	UART_MOD_CLK_DUTY_06_25 = UART_MR_OPT_DUTY_DUTY_6P25,
};

enum uart_optical_cmp_threshold {
	UART_RX_CMP_THRESHOLD_VDDIO_DIV_10_0 = UART_MR_OPT_CMPTH_VDDIO_DIV10,
	UART_RX_CMP_THRESHOLD_VDDIO_DIV_5_0 = UART_MR_OPT_CMPTH_VDDIO_DIV5,
	UART_RX_CMP_THRESHOLD_VDDIO_DIV_3_3 = UART_MR_OPT_CMPTH_VDDIO_DIV3P3,
	UART_RX_CMP_THRESHOLD_VDDIO_DIV_2_5 = UART_MR_OPT_CMPTH_VDDIO_DIV2P5,
	UART_RX_CMP_THRESHOLD_VDDIO_DIV_2_0 = UART_MR_OPT_CMPTH_VDDIO_DIV2,
};

struct uart_config_optical {
	/* UART Receive Data Inverted */
	bool rx_inverted;
	/* UART Modulated Data Inverted */
	bool tx_inverted;
	/* UART Receiver Digital Filter */
	bool rx_filter;
	/* Optical Link Clock Divider */
	uint8_t clk_div;
	/* Optical Link Modulation Clock Duty Cycle */
	enum uart_optical_duty_cycle duty;
	/* Receive Path Comparator Threshold */
	enum uart_optical_cmp_threshold threshold;
};

void uart_enable_optical_interface(Uart *p_uart);
void uart_disable_optical_interface(Uart *p_uart);
void uart_config_optical_interface(Uart *p_uart,
		struct uart_config_optical *cfg);
#endif

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* UART_H_INCLUDED */
