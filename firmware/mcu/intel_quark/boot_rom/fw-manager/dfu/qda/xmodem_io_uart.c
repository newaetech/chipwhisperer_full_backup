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

#include <errno.h>

#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_interrupt.h"
#include "qm_pic_timer.h"
#include "qm_uart.h"
#include "clk.h"

#include "xmodem_io.h"
#include "../../fw-manager_comm.h"

#define PIC_TIMER_ALARM_SECOND (0x2000000)
#define XMODEM_UART_TIMEOUT_S (2)

/*-------------------------------------------------------------------------*/
/*                          FORWARD DECLARATIONS                           */
/*-------------------------------------------------------------------------*/
static void uart_callback(void *data, int error, qm_uart_status_t status,
			  uint32_t len);
static void pic_timer_callback(void *data);

/*-------------------------------------------------------------------------*/
/*                            GLOBAL VARIABLES                             */
/*-------------------------------------------------------------------------*/

/** The variable where UART isr stores the read byte (see uart_transfer var). */
static uint8_t in_byte;

/** The XMODEM UART configuration. */
static const qm_uart_config_t uart_config = {
    .baud_divisor = FM_CONFIG_UART_BAUD_DIV,
    .line_control = QM_UART_LC_8N1,
    .hw_fc = false,
};

/** The XMODEM UART transfer configuration. */
static const qm_uart_transfer_t uart_transfer = {
    .data = &in_byte,
    .data_len = 1,
    .callback = uart_callback,
    .callback_data = NULL,
};

/** The XMODEM PIC Timer configuration. */
static const qm_pic_timer_config_t pic_timer_cfg = {
    .mode = QM_PIC_TIMER_MODE_ONE_SHOT,
    .int_en = true,
    .callback = pic_timer_callback,
    .callback_data = NULL,
};

/** The XMODEM RX state enum. */
static volatile enum rx_state {
	STATE_UART_ERROR = -EIO,
	STATE_TIMEOUT = -ETIME,
	STATE_UART_RX_DONE = 1,
	STATE_WAITING = 2,
} xmodem_io_rx_state;

/*-------------------------------------------------------------------------*/
/*                             CALLBACKS                                   */
/*-------------------------------------------------------------------------*/

static void uart_callback(void *data, int error, qm_uart_status_t status,
			  uint32_t len)
{
	(void)data;
	(void)status;
	(void)len;

	if (error < 0) {
		xmodem_io_rx_state = STATE_UART_ERROR;
	} else {
		xmodem_io_rx_state = STATE_UART_RX_DONE;
	}
}

static void pic_timer_callback(void *data)
{
	(void)data;

	xmodem_io_rx_state = STATE_TIMEOUT;
}

/*-------------------------------------------------------------------------*/
/*                         XMODEM I/O FUNCTIONS                            */
/*-------------------------------------------------------------------------*/
/* Send one byte */
int xmodem_io_putc(const uint8_t *ch)
{
	return qm_uart_write(FM_CONFIG_UART, *ch);
}

/* Receive one byte */
int xmodem_io_getc(uint8_t *ch)
{
	enum rx_state retv;

	/* Set up timeout timer */
	qm_pic_timer_set_config(&pic_timer_cfg);
	qm_pic_timer_set(PIC_TIMER_ALARM_SECOND * XMODEM_UART_TIMEOUT_S);

	/* Resetting the state and read byte */
	xmodem_io_rx_state = STATE_WAITING;
	qm_uart_irq_read(FM_CONFIG_UART, &uart_transfer);

	/* Wait for UART interruption or PIC Timer timeout callback */
	while (retv = xmodem_io_rx_state, retv == STATE_WAITING) {
		; /* Busy wait until one of the callbacks is called */
	}
	switch (retv) {
	case STATE_TIMEOUT:
		qm_uart_irq_read_terminate(FM_CONFIG_UART);
		break;
	case STATE_UART_RX_DONE:
		/* Got byte */
		*ch = in_byte;
		break;
	default:
		/**
		 * Handle STATE_UART_ERROR case which can happen on UART read
		 * error, and STATE_WAITING which is an impossible case
		 */
		break;
	}

	return retv;
}

/*-------------------------------------------------------------------------*/
/*                           GLOBAL FUNCTIONS                              */
/*-------------------------------------------------------------------------*/
void xmodem_io_uart_init()
{
	/* Pinmux for UART_x */
	qm_pmux_select(FM_COMM_UART_PIN_TX_ID, FM_COMM_UART_PIN_TX_FN);
	qm_pmux_select(FM_COMM_UART_PIN_RX_ID, FM_COMM_UART_PIN_RX_FN);
	qm_pmux_input_en(FM_COMM_UART_PIN_RX_ID, true);

	/* Enable UART clocks */
	clk_periph_enable(FM_COMM_UART_CLK | CLK_PERIPH_CLK);

	/* Setup UART */
	qm_uart_set_config(FM_CONFIG_UART, &uart_config);

	/* Request IRQ for UART */
	fm_comm_irq_request();

#if (HAS_APIC)
	/* Request interrupts for PIC Timer */
	qm_int_vector_request(QM_X86_PIC_TIMER_INT_VECTOR, qm_pic_timer_0_isr);
#elif(HAS_MVIC)
	qm_irq_request(QM_IRQ_PIC_TIMER, qm_pic_timer_0_isr);
#endif
}
