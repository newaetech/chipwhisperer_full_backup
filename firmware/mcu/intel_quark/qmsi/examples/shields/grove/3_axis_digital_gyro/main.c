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
 * Grove Shield 3 Axis Digital Gyro
 *
 * This sample application measures the angular velocity using
 * Grove digital gyro sensor v1.3 ITG-3200 on the Intel(R) Quark(TM) SE
 * and Quark(TM) D2000 development platforms.
 *
 * The device is connected to I2C port on the Grove shield. To use on
 * Intel(R) Quark(TM) SE development platform pins 22 and 23 on header J15 are
 * multiplexed and to use on Quark(TM) D2000 development platform pins 6 and 7
 * are multiplexed.
 *
 * More information about the sensor can be found using the following link:
 * http://www.seeedstudio.com/wiki/Grove_-_3-Axis_Digital_Gyro
 *
 * The application configures the I2C and RTC. The RTC callback reads
 * the sensor data registers on a time-quantum and calculates the
 * angular velocity for 3 axes.
 */

#include "clk.h"
#include "itg3200_gyro.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"

#define ALARM (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1) >> 3) /* 0.125 seconds. */
#define NUM_SAMPLES (500) /* Number of samples to be read. */

/* Sampling count grows until NUM_SAMPLES to limit interrupt triggers. */
static volatile uint16_t sampling_count;
static volatile bool print_float = false;
static float ax, ay, az;
static int16_t axis[ITG3200_MAX_AXIS] = {0};

/*
 * RTC callback function prints the angular velocity for x, y and z axis and
 * sets the RTC alarm for next trigger.
 */
static void rtc_callback(void *data)
{
	int rc;

	/* Calculate angular velocity for x, y, z axis. */
	rc = itg3200_gyro_get_angular_vel(&ax, &ay, &az, axis);
	if (rc) {
		sampling_count = NUM_SAMPLES;
		return;
	}

	print_float = true;

	/* Set a new RTC alarm. */
	++sampling_count;
	qm_rtc_set_alarm(QM_RTC_0, (QM_RTC[QM_RTC_0].rtc_ccvr + ALARM));
}

/*
 * Set I2C configuration. This enables clock lines for I2C, sets up pin
 * multiplexing for I2C and sets the configuration for I2C.
 */
static int i2c_cfg_init(void)
{

	qm_i2c_config_t config = {
	    .address_mode = QM_I2C_7_BIT,
	    .mode = QM_I2C_MASTER,
	    .speed = QM_I2C_SPEED_FAST,
	};

	/* Enable I2C. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_MX_REGISTER);

	/* Setup pin mux. */
	qm_pmux_select(QM_PIN_ID_XX, QM_PMUX_FN_X);
	qm_pmux_select(QM_PIN_ID_YY, QM_PMUX_FN_X);

	/* Set I2C configuration. */
	return qm_i2c_set_config(QM_I2C_X, &config);
}

/*
 * Set RTC configuration. This function registers RTC0 interrupt handler,
 * enables clock, initializes and sets the configuration for RTC.
 */
static void rtc_config(void)
{
	qm_rtc_config_t rtc_cfg;

	/* Register RTC ISR for periodical interrupt. */
	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	clk_periph_enable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

	/* Initialize RTC module configuration. */
	rtc_cfg.alarm_en = true;
	rtc_cfg.init_val = 0;
	rtc_cfg.alarm_val = ALARM;
	rtc_cfg.callback = rtc_callback;
	rtc_cfg.callback_data = NULL;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;

	/* Set RTC configuration. */
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);
}

int main(void)
{
	int status;

	QM_PUTS("Starting: Grove 3 axis digital gyro");

	/* Initialize and register as I2C device. */
	status = i2c_cfg_init();
	if (status) {
		QM_PRINTF("Error: I2C configuration failed! with 0x%x\n",
			  status);
	}

	/* Initialize device specific configuration. */
	status = itg3200_gyro_cfg_init();
	if (status) {
		QM_PRINTF("Error: sensor configuration failed! with 0x%x\n",
			  status);
	}

	if (!status) {
		/* Initialize and set RTC configuration. */
		rtc_config();

		/* Wait for RTC to fire NUM_SAMPLES times and then finish. */
		while (sampling_count < NUM_SAMPLES) {
			if (print_float) {
				/* Print the angular velocity as float. */
				itg3200_gyro_print_float(ax, ay, az, axis);
				print_float = false;
			}
		};
	}

	QM_PUTS("Finished: Grove 3 axis digital gyro");

	clk_periph_disable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

	return 0;
}
