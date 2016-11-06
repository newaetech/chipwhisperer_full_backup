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
 * Accelerometer
 *
 * This app will read the accelerometer data from the onboard BMC150/160 sensor
 * and print it to the console every 125 milliseconds. The app will complete
 * once it has read 500 samples.
 *
 * If the app is compiled with the Intel(R) Integrated Performance Primitives
 * (IPP) library enabled, it will also print the Root Mean Square (RMS),
 * variance and mean of the last 15 samples each time.
 */

#include <unistd.h>
#if (__IPP_ENABLED__)
#include <dsp.h>
#endif
#include "clk.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"
#include "qm_uart.h"
#include "bmx1xx/bmx1xx.h"

#define INTERVAL (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_8)) /* 125 milliseconds. */
#define NUM_SAMPLES (500)
#if (__IPP_ENABLED__)
/* Number of samples to use to generate the statistics from. */
#define SAMPLES_SIZE (15)
#endif /* __IPP_ENABLED__ */

static volatile uint32_t cb_count = 0;
static volatile bool complete = false;
static volatile bool read_accel = false;
static volatile bool print_axis = false;
static volatile bool read_error = false;
static uint32_t accel_x, accel_y, accel_z;

#if (__IPP_ENABLED__)
static float32_t mean, var, rms;
static float32_t samples[SAMPLES_SIZE];

static void print_axis_stats(int16_t value)
{
	static uint32_t index = 0;
	static uint32_t count = 0;

	/* Overwrite the oldest sample in the array. */
	samples[index] = value;
	/* Move the index on the next position, wrap around if necessary. */
	index = (index + 1) % SAMPLES_SIZE;

	/* Store number of samples until it reaches SAMPLES_SIZE. */
	count = count == SAMPLES_SIZE ? SAMPLES_SIZE : count + 1;

	/* Get the root mean square (RMS), variance and mean. */
	ippsq_rms_f32(samples, count, &rms);
	ippsq_var_f32(samples, count, &var);
	ippsq_mean_f32(samples, count, &mean);
}
#endif /* __IPP_ENABLE__ */

/* Accel callback will run every time the RTC alarm triggers. */
static void accel_callback(void *data)
{
	bmx1xx_accel_t accel = {0};

	if (0 == bmx1xx_read_accel(&accel)) {
		read_accel = true;
		accel_x = accel.x;
		accel_y = accel.y;
		accel_z = accel.z;
	} else {
		read_error = true;
	}
#if (__IPP_ENABLED__)
	print_axis_stats(accel.z);
	print_axis = true;
#endif /* __IPP_ENABLE__ */

	/* Reset the RTC alarm to fire again if necessary. */
	if (cb_count < NUM_SAMPLES) {
		qm_rtc_set_alarm(QM_RTC_0,
				 (QM_RTC[QM_RTC_0].rtc_ccvr + INTERVAL));
		cb_count++;
	} else {
		complete = true;
	}
}

int main(void)
{
	qm_rtc_config_t rtc;
	bmx1xx_setup_config_t cfg;

	QM_PUTS("Starting: Accelerometer example app");

	/* Configure the RTC and request the IRQ. */
	rtc.init_val = 0;
	rtc.alarm_en = true;
	rtc.alarm_val = INTERVAL;
	rtc.callback = accel_callback;
	rtc.callback_data = NULL;
	rtc.prescaler = CLK_RTC_DIV_1;

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	/* Enable the RTC. */
	clk_periph_enable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

#if (QUARK_D2000)
	cfg.pos = BMC150_J14_POS_0;
#endif /* QUARK_D2000 */

	/* Initialise the sensor config and set the mode. */
	bmx1xx_init(cfg);
	bmx1xx_accel_set_mode(BMX1XX_MODE_2G);

#if (BMC150_SENSOR)
	bmx1xx_set_bandwidth(BMC150_BANDWIDTH_64MS); /* Set the bandwidth. */
#elif(BMI160_SENSOR)
	bmx1xx_set_bandwidth(BMI160_BANDWIDTH_10MS); /* Set the bandwidth. */
#endif /* BMC150_SENSOR */

	/* Start the RTC. */
	qm_rtc_set_config(QM_RTC_0, &rtc);

	/* Wait for the correct number of samples to be read. */
	while (!complete) {

		if (read_accel) {
			QM_PRINTF("x %d y %d z %d\n", accel_x, accel_y,
				  accel_z);
			read_accel = false;
		} else if (read_error) {
			QM_PUTS("Error: unable to read from sensor");
		}
#if (__IPP_ENABLED__)
		if (print_axis) {
			QM_PRINTF("rms %d var %d mean %d\n", (int)rms, (int)var,
				  (int)mean);
			print_axis = false;
		}
#endif
	}

	QM_PUTS("Finished: Accelerometer example app");

	return 0;
}
