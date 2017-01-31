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
 * Analog-to-Digital Converter (ADC)
 *
 * This app demonstrates the basic functionality of the QMSI ADC API. It shows
 * the two main modes of operation; polled mode (blocking, up to 32 samples per
 * conversion) and IRQ mode (non-blocking, any number of samples).
 *
 * This app requires an Intel(R) Quark(TM) D2000 development platform to be set
 * up with jumper cables connecting ADC pin 8 (header J4, "7") and 9 (header J3
 * "8") to 3.3v (header J21) and GND respectively. The sequence table will
 * alternate reading values between the two pins defined.
 */

#include "clk.h"
#include "qm_adc.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"

#define NUM_CHANNELS (2)	   /* Number of channels to sample from. */
#define NUM_SAMPLES_POLLED (10)    /* Number of samples in polled mode. */
#define NUM_SAMPLES_INTERRUPT (50) /* Number of samples in interrupt mode. */

static volatile bool complete = false;
static volatile bool convert_error = false;

static void callback(void *data, int error, qm_adc_status_t status,
		     qm_adc_cb_source_t source)
{
	complete = true;

	if (error) {
		convert_error = true;
	}
}

int main(void)
{
	int i;
	qm_adc_config_t cfg;
	qm_adc_xfer_t xfer;
	qm_adc_channel_t channels[] = {QM_ADC_CH_8, QM_ADC_CH_9};
	uint16_t samples_polled[NUM_SAMPLES_POLLED] = {0};
	uint16_t samples_interrupt[NUM_SAMPLES_INTERRUPT] = {0};

	QM_PUTS("Starting: ADC");

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

	/* Configure the ADC. */
	cfg.window = 20; /* Clock cycles between the start of each sample. */
	cfg.resolution = QM_ADC_RES_12_BITS;
	qm_adc_set_config(QM_ADC_0, &cfg);

	/* ADC polled mode example. */
	QM_PUTS("ADC polled mode");

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples_polled;
	xfer.samples_len = NUM_SAMPLES_POLLED;

	/* Run the conversion and wait for it to complete. */
	if (qm_adc_convert(QM_ADC_0, &xfer, NULL)) {
		QM_PUTS("Error: qm_adc_convert failed");
		return 1;
	}

	/* Print the values of the samples. */
	for (i = 0; i < NUM_SAMPLES_POLLED; i++) {
		QM_PRINTF("%d:%x ", i, (unsigned int)samples_polled[i]);
	}

	/* ADC interrupt mode example. */
	QM_PUTS("\nADC interrupt mode");

	qm_irq_request(QM_IRQ_ADC_0_CAL_INT, qm_adc_0_cal_isr);

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples_interrupt;
	xfer.samples_len = NUM_SAMPLES_INTERRUPT;
	xfer.callback = callback;
	xfer.callback_data = NULL;

	/* Run the conversion. */
	if (qm_adc_irq_convert(QM_ADC_0, &xfer)) {
		QM_PUTS("Error: qm_adc_irq_convert failed");
		return 1;
	}

	/* Wait for the callback to be invoked on completion. */
	while (false == complete)
		;

	/* Check if there was an error during the conversion. */
	if (convert_error) {
		QM_PUTS("Error: ADC conversion failed");
		return 1;
	} else {
		QM_PUTS("---COMPLETE CALLBACK---");
	}

	/* Print all of the sample values. */
	for (i = 0; i < NUM_SAMPLES_INTERRUPT; i++) {
		QM_PRINTF("%d:%x ", i, (unsigned int)samples_interrupt[i]);
	}

	QM_PUTS("\nFinished: ADC");

	return 0;
}
