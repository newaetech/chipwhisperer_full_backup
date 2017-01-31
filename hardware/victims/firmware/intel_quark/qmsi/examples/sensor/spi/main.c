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
 * Sensor Subsystem (SS) Serial Peripheral Interface (SPI)
 *
 * This example uses the sensor subsystem SPI block 0 to read the CHIPID
 * of the on-board BMI160 by using polled mode and interrupts.
 */

#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_ss_spi.h"
#include "qm_uart.h"
#include "ss_clk.h"

/* Buffer size for reading CHIPID. */
#define BUFFER_SIZE (2)

volatile bool xfer_active;
volatile int err_code;
volatile uint16_t transfer_len;

qm_ss_spi_t spi = QM_SS_SPI_0;
qm_ss_spi_slave_select_t select = QM_SS_SPI_SS_0;
qm_ss_spi_config_t conf;

uint8_t tx_buffer[BUFFER_SIZE];
uint8_t rx_buffer[BUFFER_SIZE];

static void spi_cb(void *data, int error, qm_ss_spi_status_t spi_status,
		   uint16_t len)
{
	err_code = error;
	xfer_active = false;
	transfer_len = len;
}

static void spi_transfer_polled(void)
{
	QM_PUTS("Reading CHIPID in polled mode.");

	tx_buffer[0] = 0x80; /* Read chip ID. */

	/* Set up transfer values. */
	qm_ss_spi_transfer_t trans;
	trans.rx = rx_buffer;
	trans.tx = tx_buffer;
	trans.rx_len = BUFFER_SIZE;
	trans.tx_len = BUFFER_SIZE;

	err_code = 0;

	/* Set SPI configuration. */
	err_code = qm_ss_spi_set_config(spi, &conf);

	/* Enable clock for SPI 0. */
	ss_clk_spi_enable(QM_SS_SPI_0);

	/* Select slave and do the actual SPI transfer. */
	err_code |= qm_ss_spi_slave_select(spi, select);
	err_code |= qm_ss_spi_transfer(spi, &trans, NULL);

	/* Disable clock for SPI 0. */
	ss_clk_spi_disable(QM_SS_SPI_0);

	if (err_code != 0) {
		QM_PUTS("Error: SPI transfer failed.");
	} else if (rx_buffer[1] == 0xd1) {
		QM_PUTS("CHIPID = 0x1d");
	} else {
		QM_PRINTF("Error: CHIPID doesn't match 0x%x != 0xd1.\n",
			  rx_buffer[1]);
	}
}

static void spi_transfer_irq(void)
{
	QM_PUTS("Reading CHIPID in IRQ mode.");

	tx_buffer[0] = 0x80; /* Read chip ID. */

	/* Set up transfer values. */
	qm_ss_spi_async_transfer_t irq_trans;
	irq_trans.rx = rx_buffer;
	irq_trans.tx = tx_buffer;
	irq_trans.rx_len = BUFFER_SIZE;
	irq_trans.tx_len = BUFFER_SIZE;
	irq_trans.callback_data = NULL;
	irq_trans.callback = spi_cb;

	/* Register interrupts. */
	qm_ss_irq_request(QM_SS_IRQ_SPI_0_ERROR_INT, qm_ss_spi_0_error_isr);
	qm_ss_irq_request(QM_SS_IRQ_SPI_0_RX_AVAIL_INT,
			  qm_ss_spi_0_rx_avail_isr);
	qm_ss_irq_request(QM_SS_IRQ_SPI_0_TX_REQ_INT, qm_ss_spi_0_tx_req_isr);

	tx_buffer[0] = 0x80; /* Read chip ID. */

	err_code = 0;
	xfer_active = true;

	/* Set SPI configuration. */
	qm_ss_spi_set_config(spi, &conf);

	/* Enable clock for SPI 0. */
	ss_clk_spi_enable(QM_SS_SPI_0);

	/* Select slave and do the actual SPI transfer. */
	qm_ss_spi_slave_select(spi, select);
	qm_ss_spi_irq_transfer(spi, &irq_trans);
	while (xfer_active)
		;

	/* Disable clock for SPI 0. */
	ss_clk_spi_disable(QM_SS_SPI_0);

	if (err_code != 0) {
		QM_PRINTF(
		    "Error: SPI transfer failed. (%d frames transmitted)\n",
		    transfer_len);
	} else if (rx_buffer[1] == 0xd1) {
		QM_PUTS("CHIPID = 0x1d");
	} else {
		QM_PRINTF("Error: CHIPID doesn't match 0x%x != 0xd1.\n",
			  rx_buffer[1]);
	}
}

int main(void)
{
	QM_PUTS("Starting: SS SPI");

	/* Set up SPI configuration. */
	conf.frame_size = QM_SS_SPI_FRAME_SIZE_8_BIT;
	conf.transfer_mode = QM_SS_SPI_TMOD_TX_RX;
	conf.clk_divider = 32; /* 1MHz. */
	conf.bus_mode = QM_SS_SPI_BMODE_0;

	spi_transfer_polled();
	spi_transfer_irq();

	QM_PUTS("Finished: SS SPI");

	return 0;
}
