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
 * Grove Shield Temperature/Humidity Sensor
 *
 * This example requires the BASE SHIELD(v2) by Seeed Studio and the
 * HDC1000 sensor from TI. The BASE SHIELD(v2) is Arduino compatible,
 * and so can be attached to the Quark Microcontroller D2000 and
 * SE Development Platforms.
 *
 * The HDC1000 sensor is an I2C device which has the capability
 * to provide real time temperature and humidity data.
 *
 * The temperature value read from the sensor is converted to
 * degrees Celsius, with the same applying to humidity data for
 * values in relative humidity.
 *
 */

#include "clk.h"
#include "qm_common.h"
#include "qm_i2c.h"
#include "qm_identification.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_version.h"
#include "utils.h"
#include "hdc1000_bus.h"
#include "hdc1000_device.h"

#define SENSOR_READ_INTERVAL (3000000) /* 3 seconds. */
#define NUM_SENSOR_READ (10)	   /* Number of sensor reads. */

/* Get a external reference for I2C instance. */
extern qm_i2c_t g_i2c_controller_instance;

static int device_verify(void)
{
	int status;

	status = dev_hdc1000_read_id(HDC1000_ID_MANUFACTURER);
	if (status) {
		QM_PRINTF("Error: Manufacturer Id verify failed %d\n", status);
		return status;
	}

	status = dev_hdc1000_read_id(HDC1000_ID_DEVICE);
	if (status) {
		QM_PRINTF("Error: Device Id verify failed %d\n", status);
		return status;
	}

	status = dev_hdc1000_read_id(HDC1000_ID_SERIAL);
	if (status) {
		QM_PRINTF("Error: Device Serial Id verify failed %d\n", status);
		return status;
	}

	return status;
}

static int bus_i2c_init(void)
{
	qm_i2c_config_t cfg;

	QM_CHECK((true == IS_SUPPORTED_SoC()), -EINVAL);

#if (QUARK_SE)
	qm_irq_request(QM_IRQ_I2C_1_INT, qm_i2c_1_isr);

	/* Enable I2C 1. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_M1_REGISTER);

	/* Setup pin mux. */
	qm_pmux_select(QM_PIN_ID_22, QM_PMUX_FN_0);
	qm_pmux_select(QM_PIN_ID_23, QM_PMUX_FN_0);
#elif(QUARK_D2000)
	qm_irq_request(QM_IRQ_I2C_0_INT, qm_i2c_0_isr);

	/* Enable I2C 0. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_M0_REGISTER);

	/* Setup pin mux. */
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_2);
	qm_pmux_select(QM_PIN_ID_7, QM_PMUX_FN_2);
#endif

	/* Configure I2C. */
	cfg.address_mode = QM_I2C_7_BIT;
	cfg.mode = QM_I2C_MASTER;
	cfg.speed = QM_I2C_SPEED_STD;

	qm_i2c_set_config(g_i2c_controller_instance, &cfg);

	return 0;
}

static int sensor_init(hdc1000_measurement_mode_t measure_mode,
		       hdc1000_resolution_t resolution,
		       hdc1000_battery_status_t battery,
		       hdc1000_soft_reset_t reset)
{
	int status;

	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);
	QM_CHECK((resolution <= HDC1000_RESOLUTION_14BIT), -EINVAL);
	QM_CHECK((battery <= HDC1000_BATTERY_STATUS_LOW_INDICATION_DISABLE),
		 -EINVAL);
	QM_CHECK((reset <= HDC1000_DO_SOFT_RESET), -EINVAL);

	status = device_verify();
	if (status) {
		QM_PRINTF("Error: Device verify failed %d\n", status);
		return status;
	}

	status =
	    dev_hdc1000_configure(measure_mode, resolution, battery, reset);
	if (status) {
		QM_PRINTF("Error: Config failed %d\n", status);
		return status;
	}

	return status;
}

static int sensor_read(hdc1000_sensor_data_t *sensor_info_ptr,
		       hdc1000_measurement_mode_t measure_mode, int loop_count)
{
	int status;

	QM_CHECK((NULL != sensor_info_ptr), -EINVAL);
	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);

	do {
		status =
		    dev_hdc1000_get_sensor_data(measure_mode, sensor_info_ptr);
		if (status) {
			QM_PRINTF("Error: Getting sensor data failed %d\n",
				  status);
			break;
		} else {
			dev_hdc1000_print_sensor_data(measure_mode,
						      sensor_info_ptr);
			clk_sys_udelay(SENSOR_READ_INTERVAL);
		}
	} while (--loop_count);

	return status;
}

int main(void)
{
	int status;
	hdc1000_sensor_data_t sensor_info;
	hdc1000_measurement_mode_t measure_mode =
	    HDC1000_MEASUREMENT_MODE_COMBINED;

	QM_PUTS("Starting: Grove temperature/humidity sensor");

	status = bus_i2c_init();
	if (!status) {
		status =
		    sensor_init(measure_mode, HDC1000_RESOLUTION_14BIT,
				HDC1000_BATTERY_STATUS_LOW_INDICATION_DISABLE,
				HDC1000_DO_SOFT_RESET);
		if (!status) {
			status = sensor_read(&sensor_info, measure_mode,
					     NUM_SENSOR_READ);
			if (status) {
				QM_PRINTF("Error: Sensor read failed %d\n",
					  status);
			}
		} else {
			QM_PRINTF("Error: Sensor init failed %d\n", status);
		}
	} else {
		QM_PRINTF("Error: Bus init failed %d\n", status);
	}

	QM_PUTS("Finished: Grove temperature/humidity sensor");
	return status;
}
