/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Serial Peripheral Interface (SPI) loopback
 *
 * This example uses the SPI loopback mode to demonstrate setting up and
 * executing polled, IRQ and DMA based transfers on SPI master 0.
 */

#include "clk.h"
#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_spi.h"

/* The SPI clock divider is calculated in reference to a 32MHz system clock. */
#define SPI_CLOCK_125KHZ_DIV (256)
#define SPI_CLOCK_1MHZ_DIV (32)
#define SPI_CLOCK_DIV SPI_CLOCK_125KHZ_DIV
#define QM_SPI_BMODE QM_SPI_BMODE_0
#define BUFFERSIZE (128)

#define WAIT_4SEC (4000000) /* 4 seconds. */

static uint8_t tx_buff[BUFFERSIZE];
static uint8_t rx_buff[BUFFERSIZE];

/*
 * Request data for SPI asynchronous operation (transfer descriptor) needs to
 * be kept alive during request processing. It is safer when it is globally
 * accessible within the file - we are sure then that they will be always in the
 * scope when IRQ will be triggered.
 */
static qm_spi_async_transfer_t async_dma_xfer;
static qm_spi_async_transfer_t async_irq_xfer;
static volatile bool xfer_done = false;
static bool compare_tx_rx_buffers = false;

static void spi_buff_reset(void)
{
	int i;
	for (i = 0; i < BUFFERSIZE; i++) {
		tx_buff[i] = i;
		rx_buff[i] = 0xf0;
	}
}

static void spi_example_cb(void *data, int err, qm_spi_status_t status,
			   uint16_t len)
{
	int i;
	bool compare = *(bool *)data;
	xfer_done = true;

	if (!err && len == BUFFERSIZE) {
		QM_PUTS("SPI XFER COMPLETED");
		/* Compare RX and TX buffers. */
		for (i = 0; compare && (i < BUFFERSIZE); i++) {
			if (tx_buff[i] != rx_buff[i]) {
				QM_PUTS("ERROR: RX/TX Buffers mismatch!");
			} else if (i == BUFFERSIZE - 1) {
				QM_PUTS("SUCCESS: Buffers match!");
			}
		}
		spi_buff_reset();

	} else if (status == QM_SPI_RX_OVERFLOW) {
		QM_PUTS("SPI XFER ERROR -- RX Overflow");
	}
}

static void spi_config(const qm_spi_config_t *cfg_p)
{
	int ret __attribute__((unused));

	ret = qm_spi_set_config(QM_SPI_MST_0, cfg_p);
	QM_ASSERT(0 == ret);

	/* Set up loopback mode by RMW directly to the ctrlr0 register. */
	QM_SPI[QM_SPI_MST_0]->ctrlr0 |= BIT(11);

	ret = qm_spi_slave_select(QM_SPI_MST_0, QM_SPI_SS_0);
	QM_ASSERT(0 == ret);
}

void spi_polled_mode(void)
{
	qm_spi_config_t cfg;
	qm_spi_transfer_t polled_xfer_desc;
	qm_spi_status_t status;
	int ret __attribute__((unused));
	int i;

	QM_PUTS("Synchronous TXRX started");

	/* Reset the buffers. */
	spi_buff_reset();

	/* Initialise SPI configuration. */
	cfg.frame_size = QM_SPI_FRAME_SIZE_8_BIT;
	cfg.transfer_mode = QM_SPI_TMOD_TX_RX;
	cfg.bus_mode = QM_SPI_BMODE;
	cfg.clk_divider = SPI_CLOCK_DIV;
	spi_config(&cfg);

	qm_spi_slave_select(QM_SPI_MST_0, QM_SPI_SS_0);
	qm_spi_get_status(QM_SPI_MST_0, &status);

	QM_PRINTF("Status = 0x%x\n", status);

	/* Set up the sync transfer struct and call a polled transfer. */
	polled_xfer_desc.tx = tx_buff;
	polled_xfer_desc.rx = rx_buff;
	polled_xfer_desc.tx_len = BUFFERSIZE;
	polled_xfer_desc.rx_len = BUFFERSIZE;
	ret = qm_spi_transfer(QM_SPI_MST_0, &polled_xfer_desc, &status);
	QM_ASSERT(0 == ret);
	clk_sys_udelay(WAIT_4SEC);

	/* Compare RX and TX buffers. */
	for (i = 0; i < BUFFERSIZE; i++) {
		if (tx_buff[i] != rx_buff[i]) {
			QM_PUTS("ERROR: RX/TX Buffers mismatch!");
			break;
		} else if (i == BUFFERSIZE - 1) {
			QM_PUTS("SUCCESS: Buffers match!");
		}
	}

	QM_PUTS("Synchronous TXRX done\n");
}

void spi_irq_mode(void)
{
	qm_spi_config_t cfg;
	qm_spi_status_t status;
	int ret __attribute__((unused));
	compare_tx_rx_buffers = false;

	QM_PUTS("IRQ-based TXRX started");

	/* Register driver IRQ. */
	qm_irq_request(QM_IRQ_SPI_MASTER_0_INT, qm_spi_master_0_isr);

	/* Reset the buffers. */
	spi_buff_reset();

	/*  Initialise SPI configuration. */
	cfg.frame_size = QM_SPI_FRAME_SIZE_8_BIT;
	cfg.transfer_mode = QM_SPI_TMOD_TX_RX;
	cfg.bus_mode = QM_SPI_BMODE;
	cfg.clk_divider = SPI_CLOCK_DIV;
	spi_config(&cfg);

	qm_spi_slave_select(QM_SPI_MST_0, QM_SPI_SS_0);
	qm_spi_get_status(QM_SPI_MST_0, &status);

	QM_PRINTF("Status = 0x%x\n", status);

	xfer_done = false;
	compare_tx_rx_buffers = true;
	/* Set up the async transfer struct. */
	async_irq_xfer.tx = tx_buff;
	async_irq_xfer.rx = rx_buff;
	async_irq_xfer.tx_len = BUFFERSIZE;
	async_irq_xfer.rx_len = BUFFERSIZE;
	async_irq_xfer.callback = spi_example_cb;
	async_irq_xfer.callback_data = &compare_tx_rx_buffers;

	ret = qm_spi_irq_transfer(QM_SPI_MST_0, &async_irq_xfer);
	QM_ASSERT(0 == ret);

	while (false == xfer_done) {
	}

	QM_PUTS("IRQ-based TXRX done");
}

void spi_dma_mode(void)
{
	qm_spi_config_t cfg;
	qm_spi_status_t status;
	int ret __attribute__((unused));
	compare_tx_rx_buffers = false;

	/* Register the DMA interrupts. */
	qm_irq_request(QM_IRQ_DMA_0_INT_0, qm_dma_0_isr_0);
	qm_irq_request(QM_IRQ_DMA_0_INT_1, qm_dma_0_isr_1);
	qm_irq_request(QM_IRQ_DMA_0_ERROR_INT, qm_dma_0_error_isr);

	/*  Initialise SPI configuration. */
	cfg.frame_size = QM_SPI_FRAME_SIZE_8_BIT;
	cfg.transfer_mode = QM_SPI_TMOD_TX;
	cfg.bus_mode = QM_SPI_BMODE;
	cfg.clk_divider = SPI_CLOCK_DIV;
	spi_config(&cfg);

	ret = qm_dma_init(QM_DMA_0);
	QM_ASSERT(0 == ret);

	/* SPI DMA channels configuration. */
	ret =
	    qm_spi_dma_channel_config(QM_SPI_MST_0, QM_DMA_0, QM_DMA_CHANNEL_0,
				      QM_DMA_MEMORY_TO_PERIPHERAL);
	QM_ASSERT(0 == ret);
	ret =
	    qm_spi_dma_channel_config(QM_SPI_MST_0, QM_DMA_0, QM_DMA_CHANNEL_1,
				      QM_DMA_PERIPHERAL_TO_MEMORY);
	QM_ASSERT(0 == ret);

	/* DMA based TX. */
	QM_PUTS("DMA-based TX started");

	/* Reset the buffers. */
	spi_buff_reset();

	qm_spi_slave_select(QM_SPI_MST_0, QM_SPI_SS_0);
	qm_spi_get_status(QM_SPI_MST_0, &status);

	QM_PRINTF("Status = 0x%x\n", status);

	xfer_done = false;
	compare_tx_rx_buffers = false;
	/* Set up the async transfer struct. */
	async_dma_xfer.tx = tx_buff;
	async_dma_xfer.tx_len = BUFFERSIZE;
	async_dma_xfer.rx = NULL;
	async_dma_xfer.rx_len = 0;
	async_dma_xfer.callback = spi_example_cb;
	async_dma_xfer.callback_data = &compare_tx_rx_buffers;

	ret = qm_spi_dma_transfer(QM_SPI_MST_0, &async_dma_xfer);
	QM_ASSERT(0 == ret);

	while (false == xfer_done) {
	}

	QM_PUTS("DMA-based TX done");

	/* DMA based RX. */
	QM_PUTS("DMA-based RX started");

	/* Reset the buffers. */
	spi_buff_reset();

	cfg.transfer_mode = QM_SPI_TMOD_RX;
	spi_config(&cfg);

	xfer_done = false;
	compare_tx_rx_buffers = false;
	/* Set up the async transfer struct. */
	async_dma_xfer.tx = NULL;
	async_dma_xfer.tx_len = 0;
	async_dma_xfer.rx = rx_buff;
	async_dma_xfer.rx_len = BUFFERSIZE;
	async_dma_xfer.callback = spi_example_cb;
	async_dma_xfer.callback_data = &compare_tx_rx_buffers;

	ret = qm_spi_dma_transfer(QM_SPI_MST_0, &async_dma_xfer);
	QM_ASSERT(0 == ret);

	while (false == xfer_done) {
	}

	QM_PUTS("DMA-based RX done");

	/* DMA based TXRX. */
	QM_PUTS("DMA-based TXRX started");

	/* Reset the buffers. */
	spi_buff_reset();

	cfg.transfer_mode = QM_SPI_TMOD_TX_RX;
	spi_config(&cfg);

	xfer_done = false;
	compare_tx_rx_buffers = true;
	/* Set up the async transfer struct. */
	async_dma_xfer.tx = tx_buff;
	async_dma_xfer.tx_len = BUFFERSIZE;
	async_dma_xfer.rx = rx_buff;
	async_dma_xfer.rx_len = BUFFERSIZE;
	async_dma_xfer.callback = spi_example_cb;
	async_dma_xfer.callback_data = &compare_tx_rx_buffers;
	ret = qm_spi_dma_transfer(QM_SPI_MST_0, &async_dma_xfer);
	QM_ASSERT(0 == ret);

	while (false == xfer_done) {
	}

	QM_PUTS("DMA-based TXRX done");
}

int main(void)
{
	QM_PUTS("Starting: SPI");

/* Mux out SPI tx/rx pins and enable input for rx. */
#if (QUARK_SE)
	qm_pmux_select(QM_PIN_ID_55, QM_PMUX_FN_1); /* SPI0_M SCK */
	qm_pmux_select(QM_PIN_ID_56, QM_PMUX_FN_1); /* SPI0_M MISO */
	qm_pmux_select(QM_PIN_ID_57, QM_PMUX_FN_1); /* SPI0_M MOSI */
	qm_pmux_select(QM_PIN_ID_58, QM_PMUX_FN_1); /* SPI0_M SS0 */
	qm_pmux_select(QM_PIN_ID_59, QM_PMUX_FN_1); /* SPI0_M SS1 */
	qm_pmux_select(QM_PIN_ID_60, QM_PMUX_FN_1); /* SPI0_M SS2 */
	qm_pmux_select(QM_PIN_ID_61, QM_PMUX_FN_1); /* SPI0_M SS3 */
	qm_pmux_input_en(QM_PIN_ID_56, true);

#elif(QUARK_D2000)
	qm_pmux_select(QM_PIN_ID_0, QM_PMUX_FN_2);  /* SS0 */
	qm_pmux_select(QM_PIN_ID_1, QM_PMUX_FN_2);  /* SS1 */
	qm_pmux_select(QM_PIN_ID_2, QM_PMUX_FN_2);  /* SS2 */
	qm_pmux_select(QM_PIN_ID_3, QM_PMUX_FN_2);  /* SS3 */
	qm_pmux_select(QM_PIN_ID_16, QM_PMUX_FN_2); /* SCK */
	qm_pmux_select(QM_PIN_ID_17, QM_PMUX_FN_2); /* TXD */
	qm_pmux_select(QM_PIN_ID_18, QM_PMUX_FN_2); /* RXD */
	qm_pmux_input_en(QM_PIN_ID_18, true);       /* RXD input */
#endif

	spi_polled_mode();
	spi_irq_mode();
	spi_dma_mode();

	QM_PUTS("Finished: SPI");

	return 0;
}
