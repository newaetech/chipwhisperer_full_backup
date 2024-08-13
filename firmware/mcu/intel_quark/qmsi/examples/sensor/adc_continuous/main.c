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
 * Sensor Subsystem (SS) Analog-to-Digital Converter (ADC) Continuous Conversion
 *
 * This application demonstrates how to extend QMSI's ADC driver to use it
 * in continuous convert mode for the Sensor Subsystem. The application
 * makes use of a Ring Buffer to store data from the ADC.
 *
 * The Sensor subsystem ADC is used in continuous conversion mode.
 * The interrupt handler is overriden with an ISR specific for continuous
 * conversion.
 * This app requires an Intel(R) Quark(TM) SE development platform.
 *
 * The ISR produces samples by reading sample data off the FIFO and populating
 * the ring buffer. The application polls the buffer and consumes the data.
 * Implementation of the ring buffer is lockless.
 *
 * The application stops with a failure whenever the ring buffer overflows or
 * the adc meets an error.
 *
 * Two channels are used in the example:
 *  - channel 10 is located on J14 Pin 14
 *  - channel 11 is located on J14 Pin 16
 */

#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_ss_adc.h"
#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_uart.h"
#include "ss_clk.h"
#include "ring_buffer.h"

#define QM_ADC_CMD_STOP_CONT (5)
#define NUM_CHANNELS (2)
#define NUM_SAMPLES (NUM_CHANNELS * 5)
#define ADC_BUFFER_SIZE (128)
#define MAX_SAMPLES (4096)
#define ADC_SAMPLE_SHIFT (11)

qm_ss_adc_channel_t channels[] = {QM_SS_ADC_CH_10, QM_SS_ADC_CH_11};
uint16_t samples[NUM_SAMPLES] = {0};
ring_buffer_t rb;
uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile bool acq_error = false;

/* ISR for ADC 0 continuous conversion. */
QM_ISR_DECLARE(adc_0_continuous_isr)
{
	uint32_t i, controller = QM_SS_ADC_BASE;

	/* Read the samples into the array. */
	for (i = 0; i < NUM_SAMPLES; ++i) {
		/* Pop one sample into the sample register. */
		QM_SS_REG_AUX_OR(controller + QM_SS_ADC_SET,
				 QM_SS_ADC_SET_POP_RX);
		/* Read the sample in the array. */
		rb_add(&rb, __builtin_arc_lr(controller + QM_SS_ADC_SAMPLE) >>
				(ADC_SAMPLE_SHIFT - QM_SS_ADC_RES_12_BITS));
	}

	/* Clear the data available status register. */
	QM_SS_REG_AUX_OR(controller + QM_SS_ADC_CTRL,
			 QM_SS_ADC_CTRL_CLR_DATA_A);
}

/* Transfer callback. */
void transfer_cb(void *data, int error, qm_ss_adc_status_t status,
		 qm_ss_adc_cb_source_t source)
{
	if (error) {
		acq_error = true;
	}
}

void wait_for_samples(void)
{
	uint16_t val;
	uint32_t count = 0;

	/* Loop until an error occurs or MAX_SAMPLES conversions done. */
	while ((!acq_error) && (count < MAX_SAMPLES)) {
		if (rb.overflow) {
			QM_PUTS("Error : Ring Buffer Overflow");
			acq_error = true;
		}
		/* Print one data from the ring buffer. */
		if (!rb_is_empty(&rb)) {
			rb_get(&rb, &val);
			/*
			 * Print only the first 4 data in the ring buffer to not
			 * overflow the ring buffer as PRINTF is very slow in
			 * comparison.
			 */
			if (rb_get_tail_pos(&rb) < 4) {
				QM_PRINTF("%d(%d): %x\n", count,
					  rb_get_tail_pos(&rb), val);
			}
			++count;
			/*
			 * Make sure the ADC is enabled as the driver disables
			 * it after a successful conversion.
			 */
			QM_SS_REG_AUX_OR(QM_SS_ADC_BASE + QM_SS_ADC_CTRL,
					 QM_SS_ADC_CTRL_ADC_ENA);
		}
	}

	/* Transfer error. */
	if (acq_error && !rb.overflow) {
		QM_PUTS("Error: ADC conversion failed.");
	}
}

int start_irq_conversion(void)
{
	qm_ss_adc_xfer_t xfer;

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples;
	xfer.samples_len = NUM_SAMPLES;
	xfer.callback = transfer_cb;

	if (qm_ss_adc_irq_convert(QM_SS_ADC_0, &xfer)) {
		QM_PUTS("Error: qm_ss_adc_irq_convert failed");
		return 1;
	}
	return 0;
}

void setup_adc(void)
{
	qm_ss_adc_config_t cfg;
	/* Enable the adc and set the clock divisor. */
	ss_clk_adc_enable();
	ss_clk_adc_set_div(100);

	/* Set up pinmux. */
	qm_pmux_select(QM_PIN_ID_10, QM_PMUX_FN_1);
	qm_pmux_select(QM_PIN_ID_11, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_10, true);
	qm_pmux_input_en(QM_PIN_ID_11, true);

	/* Request the necessary IRQs. */
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_INT, adc_0_continuous_isr);
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_ERROR_INT, qm_ss_adc_0_error_isr);

	/* Set the mode and calibrate. */
	qm_ss_adc_set_mode(QM_SS_ADC_0, QM_SS_ADC_MODE_NORM_CAL);
	qm_ss_adc_calibrate(QM_SS_ADC_0);

	/* Set up config. */
	cfg.window = 50; /* Clock cycles between the start of each sample. */
	cfg.resolution = QM_SS_ADC_RES_12_BITS;
	qm_ss_adc_set_config(QM_SS_ADC_0, &cfg);
}

int main(void)
{
	QM_PUTS("Starting: ADC continuous conversion");

	/* Setup ring buffer. */
	setup_rb(&rb, adc_buffer, ADC_BUFFER_SIZE);

	/* Initialise and calibrate ADC, pinmux settings, register IRQs. */
	setup_adc();

	/* Begin an IRQ conversion. */
	if (start_irq_conversion()) {
		return 1;
	}

	/* Wait for MAX_SAMPLES to be converted and print some of them out. */
	wait_for_samples();

	/* Stop the sequencer. */
	QM_SS_REG_AUX_NAND(QM_SS_ADC_BASE + QM_SS_ADC_CTRL,
			   QM_SS_ADC_CTRL_SEQ_START);

	/* Mask all ADC interrupts. */
	QM_SS_REG_AUX_OR(QM_SS_ADC_BASE + QM_SS_ADC_CTRL,
			 QM_SS_ADC_CTRL_MSK_ALL_INT);
	/* Shut the ADC down. */
	qm_ss_adc_set_mode(QM_SS_ADC_0, QM_SS_ADC_MODE_PWR_DOWN);

	QM_PUTS("Finished: ADC continuous conversion");

	return 0;
}
