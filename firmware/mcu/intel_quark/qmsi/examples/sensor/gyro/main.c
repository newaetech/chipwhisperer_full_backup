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
 * Sensor Subsystem (SS) Gyroscope
 *
 * This application will read the gyro data from the onboard sensor on the
 * Intel(R) Quark(TM) SE development platform and print it to the console every
 * 125 milliseconds. The app will complete once it has read 500 samples.
 */

#include "clk.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"
#include "qm_uart.h"
#include "bmx1xx/bmx1xx.h"

#define INTERVAL (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_8)) /* 125 milliseconds. */
#define NUM_SAMPLES (500)

static volatile uint32_t cb_count = 0;
static volatile bool complete = false;
static volatile bool read_gyro = false;
static volatile bool read_error = false;
static uint32_t gyro_x, gyro_y, gyro_z;

/* Gyro callback will run every time the RTC alarm triggers. */
static void gyro_callback(void *data)
{
	bmx1xx_gyro_t gyro = {0};

	if (0 == bmx1xx_read_gyro(&gyro)) {
		read_gyro = true;
		gyro_x = gyro.x;
		gyro_y = gyro.y;
		gyro_z = gyro.z;
	} else {
		read_error = true;
	}
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

	QM_PUTS("Starting: Gyro example app");

	/* Configure the RTC and request the IRQ. */
	rtc.init_val = 0;
	rtc.alarm_en = true;
	rtc.alarm_val = INTERVAL;
	rtc.callback = gyro_callback;
	rtc.callback_data = NULL;
	rtc.prescaler = CLK_RTC_DIV_1;

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	/* Enable the RTC. */
	clk_periph_enable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

	/* Initialise the sensor config and set the mode. */
	bmx1xx_init(cfg);
	bmx1xx_set_bandwidth(BMI160_BANDWIDTH_10MS);

	/* Start the RTC. */
	qm_rtc_set_config(QM_RTC_0, &rtc);

	/* Wait for the correct number of samples to be read. */
	while (!complete) {
		if (read_gyro) {
			QM_PRINTF("gx %d gy %d gz %d\n", gyro_x, gyro_y,
				  gyro_z);
			read_gyro = false;
		} else if (read_error) {
			QM_PUTS("Error: unable to read from sensor");
		}
	}
	QM_PUTS("Finished: Gyro example app");

	return 0;
}
