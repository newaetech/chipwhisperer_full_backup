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
 * Quark D2000 Analog-to-Digital Converter (ADC) Continuous Conversion
 *
 * This application demonstrates how to extend QMSI's
 * Analog-to-Digital-Converter (ADC) driver to use it in continuous convert
 * mode by reading data from its FIFO into a ring buffer.
 *
 * This app requires an Intel(R) Quark(TM) D2000 development platform.
 *
 * The ADC is used in continuous conversion mode. The interrupt handler is
 * overriden with an ISR specific for continuous conversion.
 *
 * The ISR produces samples by reading sample data off the FIFO and populating
 * the ring buffer while the application polls the buffer and consumes the data.
 * The application stops with a failure whenever the ring buffer overflows or
 * the ADC meets an error.
 *
 * Implementation of the ring buffer is lockless.
 *
 * Two channels are used in the example:
 *  - channel 8 is located on J4 Pin "7"
 *  - channel 9 is located on J3 Pin "8"
 */

#include "clk.h"
#include "qm_adc.h"
#include "qm_interrupt.h"
#include "qm_pinmux.h"
#include "ring_buffer.h"

#define QM_ADC_CMD_STOP_CONT (5)
#define NUM_CHANNELS (2)
#define NUM_SAMPLES (NUM_CHANNELS * 5)
#define ADC_BUFFER_SIZE (128)
#define MAX_SAMPLES (4096)

qm_adc_xfer_t xfer;
qm_adc_channel_t channels[] = {QM_ADC_CH_8, QM_ADC_CH_9};
uint16_t samples[NUM_SAMPLES] = {0};
ring_buffer_t rb;
uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile bool acq_error = false, acq_complete = false, data_available = false;

/* ISR for ADC 0 continuous conversion. */
QM_ISR_DECLARE(adc_0_continuous_isr)
{
	uint32_t int_status, i, samples_to_read;

	int_status = QM_ADC[QM_ADC_0].adc_intr_status;

	/* FIFO overrun interrupt. */
	if (int_status & QM_ADC_INTR_STATUS_FO) {
		/* Stop the transfer. */
		QM_ADC[QM_ADC_0].adc_cmd = QM_ADC_CMD_STOP_CONT;

		/* Disable all interrupts. */
		QM_ADC[QM_ADC_0].adc_intr_enable = 0;

		/* Call the user callback. */
		xfer.callback(xfer.callback_data, -EIO, QM_ADC_OVERFLOW,
			      QM_ADC_TRANSFER);
	}

	/* Continuous mode command complete interrupt. */
	if (int_status & QM_ADC_INTR_STATUS_CONT_CC) {
		/* Clear the interrupt. */
		QM_ADC[QM_ADC_0].adc_intr_status &= QM_ADC_INTR_STATUS_CONT_CC;

		/* Calculate the number of samples to read. */
		samples_to_read = QM_ADC[QM_ADC_0].adc_fifo_count;

		/* Copy data out of FIFO. */
		for (i = 0; i < samples_to_read; i++) {
			rb_add(&rb, QM_ADC[QM_ADC_0].adc_sample);
		}
	}

	/* Clear interrupt. */
	QM_ISR_EOI(QM_IRQ_ADC_0_CAL_INT_VECTOR);
}

/* Transfer callback. */
void transfer_cb(void *data, int error, qm_adc_status_t status,
		 qm_adc_cb_source_t source)
{
	if (error) {
		acq_error = true;
	}
}

int main(void)
{
	uint16_t val;
	uint32_t count = 0;
	qm_adc_config_t cfg;

	QM_PUTS("Starting: ADC continuous conversion");

	/* Setup ring buffer. */
	setup_rb(&rb, adc_buffer, ADC_BUFFER_SIZE);

	/* Enable the ADC and set the clock divisor. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_ADC |
			  CLK_PERIPH_ADC_REGISTER);
	clk_adc_set_div(100); /* ADC clock is 320KHz @ 32MHz. */

	/* Set up pinmux. */
	qm_pmux_select(QM_PIN_ID_8, QM_PMUX_FN_1);
	qm_pmux_select(QM_PIN_ID_9, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_8, true);
	qm_pmux_input_en(QM_PIN_ID_9, true);

	/* Set the mode and calibrate. */
	qm_adc_set_mode(QM_ADC_0, QM_ADC_MODE_NORM_CAL);
	qm_adc_calibrate(QM_ADC_0);

	/* Set up config. */
	cfg.window = 20; /* Clock cycles between the start of each sample. */
	cfg.resolution = QM_ADC_RES_12_BITS;
	qm_adc_set_config(QM_ADC_0, &cfg);

	qm_irq_request(QM_IRQ_ADC_0_CAL_INT, adc_0_continuous_isr);

	/* Set up transfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples;
	xfer.samples_len = NUM_SAMPLES;
	xfer.callback = transfer_cb;

	if (qm_adc_irq_convert(QM_ADC_0, &xfer)) {
		QM_PUTS("Error: qm_adc_irq_convert failed.");
		return 1;
	}

	while ((!acq_error) && (count < MAX_SAMPLES)) {
		if (rb.overflow) {
			QM_PUTS("Error : Ring Buffer Overflow.");
			acq_error = true;
		}
		/* Print one data from the ring buffer. */
		if (!rb_is_empty(&rb)) {
			rb_get(&rb, &val);

			/*
			 * Print only the first 4 data in the ring buffer to not
			 * overflow the ring buffer as PRINTF is very slow.
			 */
			if (rb_get_tail_pos(&rb) < 4) {
				QM_PRINTF("%d(%d): %x\n", count,
					  rb_get_tail_pos(&rb), val);
			}
			count++;
		}
	}

	if (acq_error & !rb.overflow) {
		QM_PUTS("Error: ADC conversion failed.");
	}

	/* Stop the transfer. */
	QM_ADC[0].adc_cmd = QM_ADC_CMD_STOP_CONT;

	/* Disable all ADC interrupts. */
	QM_ADC[0].adc_intr_enable = 0;

	/* Flush the FIFO. */
	QM_ADC[0].adc_sample = QM_ADC_FIFO_CLEAR;

	/* Clear all pending interrupts. */
	QM_ADC[0].adc_intr_status = QM_ADC_INTR_STATUS_CC |
				    QM_ADC_INTR_STATUS_FO |
				    QM_ADC_INTR_STATUS_CONT_CC;

	QM_PUTS("Finished: ADC continuous conversion");

	return 0;
}
