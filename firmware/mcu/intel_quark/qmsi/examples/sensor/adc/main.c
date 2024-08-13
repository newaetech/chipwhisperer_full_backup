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
 * Sensor Subsystem (SS) Analog-to-Digital Converter (ADC)
 *
 * This application demonstrates the basic functionality of the ADC driver
 * for the Sensor Subsystem using Polling and Interrupt based methods.
 *
 * This app requires an Intel(R) Quark(TM) SE board to be set up with jumper
 * cables connecting ADC pin 10 and 11 to 3.3v and GND. The sequence table
 * will read the value of these pins alternatively.
 *
 * The ADC pins 10 and 11 are located on header J14 at pins 14 and 16
 * respectively. GND and 3.3v are all located on the same header at
 * pins 12 and 2 respectively.
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

#define NUM_CHANNELS (2)
#define NUM_SAMPLES_POLLED (10)
#define NUM_SAMPLES_INTERRUPT (50)

static volatile bool complete = false;
static volatile bool callback_error = true;
static volatile bool overflow_error = false;
static volatile bool underflow_error = false;
static volatile bool seq_error = false;

static void callback(void *data, int error, qm_ss_adc_status_t status,
		     qm_ss_adc_cb_source_t source)
{
	if (!error) {
		callback_error = false;
		complete = true;
	}

	if (status & QM_SS_ADC_OVERFLOW) {
		overflow_error = true;
		complete = true;
	}

	if (status & QM_SS_ADC_UNDERFLOW) {
		underflow_error = true;
		complete = true;
	}

	if (status & QM_SS_ADC_SEQERROR) {
		seq_error = true;
		complete = true;
	}
	return;
}

int main(void)
{
	int i;
	qm_ss_adc_config_t cfg;
	qm_ss_adc_xfer_t xfer;
	qm_ss_adc_channel_t channels[] = {QM_SS_ADC_CH_10, QM_SS_ADC_CH_11};
	uint16_t samples_polled[NUM_SAMPLES_POLLED] = {0};
	uint16_t samples_interrupt[NUM_SAMPLES_INTERRUPT] = {0};

	QM_PUTS("Starting: SS ADC");

	/* Enable the ADC and set the clock divisor. */
	ss_clk_adc_enable();
	ss_clk_adc_set_div(100);

	/* Set up pinmux. */
	qm_pmux_select(QM_PIN_ID_10, QM_PMUX_FN_1);
	qm_pmux_select(QM_PIN_ID_11, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_10, true);
	qm_pmux_input_en(QM_PIN_ID_11, true);

	/* Set the mode and calibrate. */
	qm_ss_adc_set_mode(QM_SS_ADC_0, QM_SS_ADC_MODE_NORM_CAL);
	qm_ss_adc_calibrate(QM_SS_ADC_0);

	/* Set up config. */
	cfg.window = 50; /* Clock cycles between the start of each sample. */
	cfg.resolution = QM_SS_ADC_RES_12_BITS;
	qm_ss_adc_set_config(QM_SS_ADC_0, &cfg);

	/* ADC polled mode example. */
	QM_PUTS("ADC polled mode");

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples_polled;
	xfer.samples_len = NUM_SAMPLES_POLLED;
	xfer.callback = NULL;
	xfer.callback_data = NULL;

	/* Run the conversion. */
	if (qm_ss_adc_convert(QM_SS_ADC_0, &xfer, NULL)) {
		QM_PUTS("Error: qm_ss_adc_convert failed");
		return 1;
	}

	/* Print the values of the samples. */
	for (i = 0; i < NUM_SAMPLES_POLLED; i++) {
		QM_PRINTF("%d:%x ", i, (unsigned int)samples_polled[i]);
	}

	/* ADC interrupt mode example. */
	QM_PUTS("\nADC interrupt mode");

	/* Request the necessary IRQs. */
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_INT, qm_ss_adc_0_isr);
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_ERROR_INT, qm_ss_adc_0_error_isr);

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples_interrupt;
	xfer.samples_len = NUM_SAMPLES_INTERRUPT;
	xfer.callback = callback;
	xfer.callback_data = NULL;

	if (qm_ss_adc_irq_convert(QM_SS_ADC_0, &xfer)) {
		QM_PUTS("Error: qm_adc_irq_convert failed");
		return 1;
	}

	/* Wait for the callback. */
	while (false == complete)
		;

	if (!callback_error) {
		QM_PUTS("---COMPLETE CALLBACK---");
	} else if (overflow_error) {
		QM_PUTS("Error: ADC FIFO overflow");
		return 1;
	} else if (underflow_error) {
		QM_PUTS("Error: ADC FIFO underflow");
		return 1;
	} else if (seq_error) {
		QM_PUTS("Error: ADC sequencer error");
		return 1;
	}
	/* Print the values of the samples. */
	for (i = 0; i < NUM_SAMPLES_INTERRUPT; i++) {
		QM_PRINTF("%d:%x ", i, (unsigned int)samples_interrupt[i]);
	}

	QM_PUTS("\nFinished: SS ADC");

	return 0;
}
