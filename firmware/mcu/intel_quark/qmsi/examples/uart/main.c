/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

/*
 * Universal Asynchronous Receiver/Transmitter (UART)
 *
 * This app demonstrates the UART functionality on Intel(R) Quark(TM)
 * development platforms.
 */

#include "clk.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_uart.h"

#define BANNER_STR ("\nQMSI echo application, Pol'd message.\n")

#define BANNER_IRQ_ID (1)
#define BANNER_IRQ ("\nQMSI echo application, IRQ'd message.\n")
#define BANNER_IRQ_COMPLETE ("IRQ Transfer completed.\n\n")

#define BANNER_DMA_ID (2)
#define BANNER_DMA ("\n\nQMSI echo application, DMA message.\n")
#define BANNER_DMA_COMPLETE ("DMA Transfer completed.\n\n")

#define RX_STR ("Data received: ")
#define ERR_STR ("Error: Transmission incomplete.\n")

/* Read callback status polling period (us). */
#define WAIT_READ_CB_PERIOD_1MS (1000)
#define TIMEOUT_10SEC (10 * 1000000 / WAIT_READ_CB_PERIOD_1MS)
/* Wait time (us). */
#define WAIT_1MSEC (1000)
#define WAIT_1SEC (1000000)
#define WAIT_5SEC (5000000)

#define DATA_LEN (50)

/* Get UART name from STDOUT_UART as a string. */
#define STR_VALUE(arg) #arg
#define ARG_NAME(name) STR_VALUE(name)
#define UART_NAME ARG_NAME(STDOUT_UART)

static uint8_t rx_buffer[DATA_LEN];
static volatile bool rx_callback_invoked = false, tx_callback_invoked = false;
static qm_uart_status_t uart_status __attribute__((unused)) = 0;
static uint32_t write_len = 0;

/*
 * Request data for UART asynchronous operation (transfer descriptor) needs to
 * be kept alive during request processing. It is safer when it is globally
 * accessible within the file - we are sure then that they will be always in the
 * scope when IRQ will be triggered.
 */
static qm_uart_transfer_t async_xfer_desc = {0};

static void wait_rx_callback_timeout(uint32_t timeout)
{
	while (!rx_callback_invoked && timeout) {
		clk_sys_udelay(WAIT_READ_CB_PERIOD_1MS);
		--timeout;
	};
}

static void uart_example_tx_callback(void *data, int error,
				     qm_uart_status_t status, uint32_t len)
{
	uint32_t id = *(uint32_t *)data;
	write_len = len;

	switch (id) {
	case BANNER_IRQ_ID:
	case BANNER_DMA_ID:
		tx_callback_invoked = true;
		break;

	default:
		break;
	}
}

static void uart_example_rx_callback(void *data, int error,
				     qm_uart_status_t status, uint32_t len)
{
	uint32_t id = *(uint32_t *)data;

	write_len = len;
	if (!error || error == -ECANCELED) {
		/* Transfer successful or RX terminated. */
		switch (id) {
		case BANNER_IRQ_ID:
		case BANNER_DMA_ID:
			rx_callback_invoked = true;
			break;
		default:
			break;
		}
	}
}

static void pin_mux_setup()
{
/* Mux out STDOUT_UART TX/RX pins and enable input for RX. */
#if (QUARK_SE)
	if (STDOUT_UART == QM_UART_0) {
		qm_pmux_select(QM_PIN_ID_18, QM_PMUX_FN_0);
		qm_pmux_select(QM_PIN_ID_19, QM_PMUX_FN_0);
		qm_pmux_input_en(QM_PIN_ID_18, true);
	} else {
		qm_pmux_select(QM_PIN_ID_16, QM_PMUX_FN_2);
		qm_pmux_select(QM_PIN_ID_17, QM_PMUX_FN_2);
		qm_pmux_input_en(QM_PIN_ID_17, true);
	}

#elif(QUARK_D2000)
	if (STDOUT_UART == QM_UART_0) {
		qm_pmux_select(QM_PIN_ID_12, QM_PMUX_FN_2);
		qm_pmux_select(QM_PIN_ID_13, QM_PMUX_FN_2);
		qm_pmux_input_en(QM_PIN_ID_13, true);
	} else {
		qm_pmux_select(QM_PIN_ID_20, QM_PMUX_FN_2);
		qm_pmux_select(QM_PIN_ID_21, QM_PMUX_FN_2);
		qm_pmux_input_en(QM_PIN_ID_21, true);
	}

#else
#error("Unsupported / unspecified processor detected.")
#endif
}

static int irq_tx()
{
	/* IRQ based TX. */
	async_xfer_desc.data = (uint8_t *)BANNER_IRQ;
	async_xfer_desc.data_len = sizeof(BANNER_IRQ);
	async_xfer_desc.callback = uart_example_tx_callback;
	if (qm_uart_irq_write(STDOUT_UART, &async_xfer_desc)) {
		QM_PUTS("IRQ write failed");
		return 1;
	}
	while (!tx_callback_invoked)
		;
	qm_uart_write_buffer(STDOUT_UART, (uint8_t *)BANNER_IRQ_COMPLETE,
			     sizeof(BANNER_IRQ_COMPLETE));
	tx_callback_invoked = false;
	return 0;
}

static int irq_rx()
{
	/* IRQ based RX. */
	rx_callback_invoked = false;
	write_len = 0;

	async_xfer_desc.data = rx_buffer;
	async_xfer_desc.data_len = DATA_LEN;
	async_xfer_desc.callback = uart_example_rx_callback;
	if (qm_uart_irq_read(STDOUT_UART, &async_xfer_desc)) {
		QM_PUTS("IRQ read failed");
		return 1;
	}

	QM_PRINTF("Waiting for data on %s (IRQ mode). Type %d "
		  "characters...\n",
		  UART_NAME, DATA_LEN);

	wait_rx_callback_timeout(TIMEOUT_10SEC);

	if (!rx_callback_invoked) {
		qm_uart_write_buffer(STDOUT_UART, (uint8_t *)ERR_STR,
				     sizeof(ERR_STR));
		/*
		 * RX complete callback was not invoked, we need to terminate
		 * the transfer in order to grab whatever is available in the RX
		 * buffer.
		 */
		if (qm_uart_irq_read_terminate(STDOUT_UART)) {
			QM_PUTS("IRQ transfer termination failed");
			return 1;
		}
	} else {
		qm_uart_write_buffer(STDOUT_UART, (uint8_t *)RX_STR,
				     sizeof(RX_STR));
		qm_uart_write_buffer(STDOUT_UART, rx_buffer, write_len);
		/*
		 * RX complete callback was invoked and RX buffer was read, we
		 * wait in case the user does not stop typing after entering the
		 * exact amount of data that fits the RX buffer, i.e. there may
		 * be additional bytes in the RX FIFO that need to be read
		 * before continuing.
		 */
		clk_sys_udelay(WAIT_5SEC);

		qm_uart_get_status(STDOUT_UART, &uart_status);
		if (QM_UART_RX_BUSY & uart_status) {
			/* Fetch data in RX FIFO. */
			if (qm_uart_irq_read(STDOUT_UART, &async_xfer_desc)) {
				QM_PUTS("IRQ read failed");
				return 1;
			}

			if (qm_uart_irq_read_terminate(STDOUT_UART)) {
				QM_PUTS("IRQ transfer termination failed");
				return 1;
			}
		}
	}
	return 0;
}

static int dma_rx()
{
	/* DMA based RX. */
	rx_callback_invoked = false;
	write_len = 0;

	if (qm_uart_dma_channel_config(STDOUT_UART, QM_DMA_0, QM_DMA_CHANNEL_0,
				       QM_DMA_PERIPHERAL_TO_MEMORY)) {
		QM_PUTS("DMA channel config failed");
		return 1;
	}

	QM_PRINTF("Waiting for data on %s (DMA mode). Type %d "
		  "characters...\n",
		  UART_NAME, DATA_LEN);

	async_xfer_desc.data = (uint8_t *)rx_buffer;
	async_xfer_desc.data_len = DATA_LEN;
	async_xfer_desc.callback = uart_example_rx_callback;
	if (qm_uart_dma_read(STDOUT_UART, &async_xfer_desc)) {
		QM_PUTS("DMA read failed.");
		return 1;
	}

	wait_rx_callback_timeout(TIMEOUT_10SEC);

	if (!rx_callback_invoked) {
		qm_uart_write_buffer(STDOUT_UART, (uint8_t *)ERR_STR,
				     sizeof(ERR_STR));
		/*
		 * RX complete callback was not invoked, we need to terminate
		 * the transfer in order to grab whatever was written in the RX
		 * buffer.
		 */
		if (qm_uart_dma_read_terminate(STDOUT_UART)) {
			QM_PUTS("DMA transfer termination failed");
			return 1;
		}
	} else {

		qm_uart_write_buffer(STDOUT_UART, (uint8_t *)RX_STR,
				     sizeof(RX_STR));
		qm_uart_write_buffer(STDOUT_UART, rx_buffer, write_len);
	}
	return 0;
}

static int dma_tx()
{
	/* DMA based TX. */
	if (qm_uart_dma_channel_config(STDOUT_UART, QM_DMA_0, QM_DMA_CHANNEL_0,
				       QM_DMA_MEMORY_TO_PERIPHERAL)) {
		QM_PUTS("DMA channel config failed");
		return 1;
	}

	async_xfer_desc.data = (uint8_t *)BANNER_DMA;
	async_xfer_desc.data_len = sizeof(BANNER_DMA);
	async_xfer_desc.callback = uart_example_tx_callback;
	if (qm_uart_dma_write(STDOUT_UART, &async_xfer_desc)) {
		QM_PUTS("DMA write failed");
		return 1;
	}
	while (!tx_callback_invoked)
		;

	qm_uart_write_buffer(STDOUT_UART, (uint8_t *)BANNER_DMA_COMPLETE,
			     sizeof(BANNER_DMA_COMPLETE));
	tx_callback_invoked = false;
	return 0;
}

int main(void)
{
	qm_uart_config_t cfg = {0};
	const uint32_t xfer_irq_data = BANNER_IRQ_ID;
	const uint32_t xfer_dma_data = BANNER_DMA_ID;
	uint8_t dummy_data;

	/*
	 * Wait as the UART has just been configured by the app_entry
	 * before reconfiguring it.
	 */
	clk_sys_udelay(WAIT_1MSEC);

	/*
	 * Set divisors to yield 115200bps baud rate. Sysclk is set by boot ROM
	 * to hybrid osc in silicon mode (32MHz), peripheral clock divisor set
	 * to 1.
	 */
	cfg.baud_divisor = QM_UART_CFG_BAUD_DL_PACK(0, 17, 6);
	cfg.line_control = QM_UART_LC_8N1;
	cfg.hw_fc = false;

	pin_mux_setup();

	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_UARTA_REGISTER);
	qm_uart_set_config(STDOUT_UART, &cfg);

	/*
	 * Changing the pin muxing as the UART is configured creates
	 * a garbage character in the FIFO. Discard it.
	 */
	qm_uart_read_non_block(STDOUT_UART, &dummy_data);

	QM_PUTS("Starting: UART");

	/* Synchronous TX. */
	if (qm_uart_write_buffer(STDOUT_UART, (uint8_t *)BANNER_STR,
				 sizeof(BANNER_STR))) {
		QM_PUTS("UART buffer write failed.");
		return 1;
	}

/* Register the UART interrupts. */
#if (STDOUT_UART_0)
	qm_irq_request(QM_IRQ_UART_0_INT, qm_uart_0_isr);
#elif(STDOUT_UART_1)
	qm_irq_request(QM_IRQ_UART_1_INT, qm_uart_1_isr);
#endif

	/* Used on both TX and RX. */
	async_xfer_desc.callback_data = (void *)&xfer_irq_data;

	/* IRQ based TX. */
	if (irq_tx()) {
		return 1;
	}
	clk_sys_udelay(WAIT_1SEC);

	/* IRQ based RX. */
	if (irq_rx()) {
		return 1;
	}

	/* Register the DMA interrupts. */
	qm_irq_request(QM_IRQ_DMA_0_INT_0, qm_dma_0_isr_0);
	qm_irq_request(QM_IRQ_DMA_0_ERROR_INT, qm_dma_0_error_isr);

	/* DMA controller initialization. */
	if (qm_dma_init(QM_DMA_0)) {
		QM_PUTS("DMA init failed");
		return 1;
	}

	/* Used on both TX and RX. */
	async_xfer_desc.callback_data = (void *)&xfer_dma_data;

	/* DMA based TX. */
	if (dma_tx()) {
		return 1;
	}
	clk_sys_udelay(WAIT_1SEC);

	/* DMA based RX. */
	if (dma_rx()) {
		return 1;
	}
	QM_PUTS("\n\nFinished: UART");

	return 0;
}
