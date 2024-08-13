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
 * Grove Shield Electricity Sensor
 *
 * This application works with shield and Grove electricity sensor TA12-200.
 * The example assumes that the sensor is connected to A0 on Grove shield.
 *
 * More details can be found at:
 * http://www.seeedstudio.com/wiki/Grove_-_Electricity_Sensor
 *
 * This application makes use of a ring buffer to store data from the ADC in
 * continuous conversion mode. The interrupt handler is overriden with an ISR
 * specific for continuous conversion. The application stops with a failure
 * whenever the ring buffer overflows or the ADC meets an error. This example
 * uses single channel conversion.
 *
 * On the Intel(R) Quark(TM) SE this app uses the sensor subsystem.
 */

#include "qm_interrupt.h"
#include "qm_pinmux.h"
#include "ring_buffer.h"

#if (QUARK_SE)
#include "qm_common.h"
#include "qm_isr.h"
#include "qm_ss_adc.h"
#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_uart.h"
#include "ss_clk.h"

#elif(QUARK_D2000)
#include "clk.h"
#include "qm_adc.h"
#endif

#define QM_ADC_CMD_STOP_CONT (5)
#define NUM_CHANNELS (1)
#define SAMPLE_SIZE (NUM_CHANNELS * 5)
#define ADC_BUFFER_SIZE (128)
#define NUM_SAMPLES (5000)
#define ADC_SAMPLE_SHIFT (11)
#define NUM_RUN (1000)
#define FRACTION_PRECISION (100)

/* Following defines are used for computing the ampere. */
#define VCC_VALUE (3.3)
#define DIVISOR_FOR_12_BIT_RESOLUTION (4096)
#define SAMPLING_RESISTANCE (800)
#define MAXIMUM_AC_VALUE (1.414)
#define TRANSFORMATION_RATIO (2000)

#if (QUARK_SE)
#define WAIT_CYCLES_BEFORE_START_SAMPLING (50)

#elif(QUARK_D2000)
#define WAIT_CYCLES_BEFORE_START_SAMPLING (20)
#endif

static ring_buffer_t rb;
static uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile bool acq_error = false;
static uint16_t samples[SAMPLE_SIZE];

#if (QUARK_SE)
static qm_ss_adc_xfer_t xfer;
static qm_ss_adc_channel_t channels[] = {QM_SS_ADC_CH_10};

#elif(QUARK_D2000)
static qm_adc_xfer_t xfer;
static qm_adc_channel_t channels[] = {QM_ADC_CH_3};
#endif

#if (QUARK_SE)
/* ISR for ADC 0 continuous conversion. */
QM_ISR_DECLARE(adc_0_continuous_isr)
{
	uint32_t i, controller = QM_SS_ADC_BASE;

	/* Read the samples into the array. */
	for (i = 0; i < SAMPLE_SIZE; ++i) {
		/* Pop one sample into the sample register. */
		QM_SS_REG_AUX_OR(controller + QM_SS_ADC_SET,
				 QM_SS_ADC_SET_POP_RX);
		/* Add the sample to the ring buffer. */
		rb_add(&rb, __builtin_arc_lr(controller + QM_SS_ADC_SAMPLE) >>
				(ADC_SAMPLE_SHIFT - QM_SS_ADC_RES_12_BITS));
	}

	/* Clear the data available status register. */
	QM_SS_REG_AUX_OR(controller + QM_SS_ADC_CTRL,
			 QM_SS_ADC_CTRL_CLR_DATA_A);
}

#elif(QUARK_D2000)
/*
 * This is the ISR handler for continuous conversion. It gets called
 * periodically whenever FIFO is full with samples.  This routine checks if the
 * FIFO status and flags error or copies the samples to the ring buffer. The
 * ring buffer is then processed in main() whenever FIFO is getting filled by
 * the ADC driver.
 */
static void adc_continuous_isr_handler(const qm_adc_t adc)
{
	uint32_t int_status, i, samples_to_read;

	int_status = QM_ADC[adc].adc_intr_status;

	/* FIFO overrun interrupt. */
	if (int_status & QM_ADC_INTR_STATUS_FO) {
		/* Stop the transfer. */
		QM_ADC[adc].adc_cmd = QM_ADC_CMD_STOP_CONT;
		/* Disable all interrupts. */
		QM_ADC[adc].adc_intr_enable = 0;
		/* Call the user callback. */
		xfer.callback(xfer.callback_data, -EIO, QM_ADC_OVERFLOW,
			      QM_ADC_TRANSFER);
	}

	/* Continuous mode command complete interrupt. */
	if (int_status & QM_ADC_INTR_STATUS_CONT_CC) {
		/* Clear the interrupt. */
		QM_ADC[adc].adc_intr_status &= QM_ADC_INTR_STATUS_CONT_CC;

		/* Calculate the number of samples to read. */
		samples_to_read = QM_ADC[adc].adc_fifo_count;

		/* Copy data from FIFO to ring buffer. */
		for (i = 0; i < samples_to_read; i++) {
			rb_add(&rb, QM_ADC[adc].adc_sample);
		}
	}
}

/* Register the ISR handler for ADC 0 continuous conversion. */
QM_ISR_DECLARE(adc_0_continuous_isr)
{
	adc_continuous_isr_handler(QM_ADC_0);
	QM_ISR_EOI(QM_IRQ_ADC_0_CAL_INT_VECTOR);
}
#endif /* QUARK D2000. */

/*
 * The following function is a callback that gets called when ADC error occurs.
 * The function sets the acq_error to TRUE.  This in turn would break the while
 * loop in main().
 */
#if (QUARK_SE)
static void error_callback(void *data, int error, qm_ss_adc_status_t status,
			   qm_ss_adc_cb_source_t source)

#elif(QUARK_D2000)
static void error_callback(void *data, int error, qm_adc_status_t status,
			   qm_adc_cb_source_t source)
#endif
{
	if (error) {
		QM_PUTS("Error: ADC conversion failed");
		QM_PRINTF("Status: 0x%x, source: 0x%x\n", status, source);
		acq_error = true;
	}
}

static int start_irq_conversion(void)
{
#if (QUARK_D2000)
	/* Register the ISR routine. */
	qm_irq_request(QM_IRQ_ADC_0_CAL_INT, adc_0_continuous_isr);
#endif

	/* Set up xfer. */
	xfer.ch = channels;
	xfer.ch_len = NUM_CHANNELS;
	xfer.samples = samples;
	xfer.samples_len = SAMPLE_SIZE;
	xfer.callback = error_callback;

#if (QUARK_SE)
	return qm_ss_adc_irq_convert(QM_SS_ADC_0, &xfer);
#elif(QUARK_D2000)
	return qm_adc_irq_convert(QM_ADC_0, &xfer);
#endif
}

static int setup_adc(void)
{
#if (QUARK_SE)
	qm_ss_adc_config_t cfg;

	/* Enable the ADC and set the clock divisor. */
	ss_clk_adc_enable();
	ss_clk_adc_set_div(100);

	/* Set up pinmux. */
	qm_pmux_select(QM_PIN_ID_10, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_10, true);

	/* Request the necessary IRQs. */
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_INT, adc_0_continuous_isr);
	qm_ss_irq_request(QM_SS_IRQ_ADC_0_ERROR_INT, qm_ss_adc_0_error_isr);

	/* Set the mode and calibrate. */
	qm_ss_adc_set_mode(QM_SS_ADC_0, QM_SS_ADC_MODE_NORM_CAL);
	qm_ss_adc_calibrate(QM_SS_ADC_0);

	/* Set up config. */
	cfg.window = WAIT_CYCLES_BEFORE_START_SAMPLING;
	cfg.resolution = QM_SS_ADC_RES_12_BITS;

	return qm_ss_adc_set_config(QM_SS_ADC_0, &cfg);

#elif(QUARK_D2000)
	qm_adc_config_t cfg;

	/* Enable the ADC and set the clock divisor. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_ADC |
			  CLK_PERIPH_ADC_REGISTER);
	clk_adc_set_div(100); /* ADC clock is 320KHz @ 32MHz. */

	/* Set up pinmux. */
	qm_pmux_select(QM_PIN_ID_3, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_3, true);

	/* Set the mode and calibrate. */
	qm_adc_set_mode(QM_ADC_0, QM_ADC_MODE_NORM_CAL);
	qm_adc_calibrate(QM_ADC_0);

	/* Set up config. */
	cfg.window = WAIT_CYCLES_BEFORE_START_SAMPLING;
	cfg.resolution = QM_ADC_RES_12_BITS;

	return qm_adc_set_config(QM_ADC_0, &cfg);

#endif /* QUARK D2000. */
}

static int init_tasks(void)
{
	int status_ret;

	/* Setup ring buffer. */
	setup_rb(&rb, adc_buffer, ADC_BUFFER_SIZE);

	/* Initialize and calibrate ADC, pinmux settings, register IRQs. */
	status_ret = setup_adc();
	if (status_ret != 0) {
		QM_PRINTF("Error: setup_adc failed. Err Code = 0x%x\n",
			  status_ret);
		return status_ret;
	}

	/* Begin an IRQ conversion. */
	status_ret = start_irq_conversion();
	if (status_ret) {
		QM_PRINTF("Error: start_irq_conversion. Err Code = 0x%x\n",
			  status_ret);
	}
	return status_ret;
}

/* Function computes the decimal and fractional part and prints. */
static void print_float(float input)
{
	uint32_t decimal_part;
	float fraction_part;

	decimal_part = (uint16_t)input;
	fraction_part = input - (float)decimal_part;

	QM_PRINTF("Effective Current (Amp.): %d.%d\n", decimal_part,
		  (uint16_t)(fraction_part * FRACTION_PRECISION));
}

static void run_tasks(void)
{
	uint16_t val;
	uint32_t num_of_runs = 0, sample_max_val, sample_threshold;
	float amplitude_current, effective_value;

	/*
	 * Continue until error occurs or it reaches pre-defined MAX_RUN value.
	 */
	while ((!acq_error) && (num_of_runs < NUM_RUN)) {
		/*
		 * Initialize the values to ensure no miscalculations and actual
		 * results.
		 */
		sample_max_val = 0;
		sample_threshold = 0;

		/*
		 * Continue until predefined samples are captured for more
		 * accurate result.
		 */
		while (sample_threshold < NUM_SAMPLES) {
			if (rb.overflow) {
				QM_PUTS("Error: Ring Buffer Overflow!");
				acq_error = true;
				break;
			}
			/*
			 * Get the maximum sample value from ring buffer (which
			 * is filled in the ISR). The conversion formula for
			 * Grove Electricity sensor v1.0 is applied to find the
			 * amplitude and effective current.
			 */
			while (!rb_is_empty(&rb)) {
				rb_get(&rb, &val);
				if (sample_max_val < val) {
					sample_max_val = val;
				}
			}
			++sample_threshold;
		}

		if (!acq_error) {
			amplitude_current = 0;
			effective_value = 0;

			/*
			 * Apply the formula on the max sample value found in
			 * the above while loop.
			 */
			amplitude_current = (float)sample_max_val /
					    DIVISOR_FOR_12_BIT_RESOLUTION *
					    VCC_VALUE / SAMPLING_RESISTANCE *
					    TRANSFORMATION_RATIO;
			effective_value = amplitude_current / MAXIMUM_AC_VALUE;

			print_float(effective_value);

			num_of_runs++;
		}
	}
}

static void cleanup_tasks(void)
{
#if (QUARK_SE)
	/* Stop the sequencer. */
	QM_SS_REG_AUX_NAND(QM_SS_ADC_BASE + QM_SS_ADC_CTRL,
			   QM_SS_ADC_CTRL_SEQ_START);

	/* Mask all interrupts. */
	QM_SS_REG_AUX_OR(QM_SS_ADC_BASE + QM_SS_ADC_CTRL,
			 QM_SS_ADC_CTRL_MSK_ALL_INT);

	/* Shut the ADC down. */
	qm_ss_adc_set_mode(QM_SS_ADC_0, QM_SS_ADC_MODE_PWR_DOWN);

#elif(QUARK_D2000)
	/* Stop the transfer. */
	QM_ADC[0].adc_cmd = QM_ADC_CMD_STOP_CONT;

	/* Disable all interrupts. */
	QM_ADC[0].adc_intr_enable = 0;

	/* Flush the FIFO. */
	QM_ADC[0].adc_sample = QM_ADC_FIFO_CLEAR;

	/* Clear all pending interrupts. */
	QM_ADC[0].adc_intr_status = QM_ADC_INTR_STATUS_CC |
				    QM_ADC_INTR_STATUS_FO |
				    QM_ADC_INTR_STATUS_CONT_CC;
#endif
}

int main(void)
{
	QM_PUTS("Starting: Electricity sensor");

	acq_error = init_tasks();

	if (!acq_error) {
		run_tasks();
	}

	cleanup_tasks();

	QM_PUTS("Finished: Electricity sensor");

	return 0;
}
