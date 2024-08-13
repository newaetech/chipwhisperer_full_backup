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

#include "qm_common.h"
#include "qm_i2c.h"
#include "hdc1000_device.h"
#include "hdc1000_register.h"
#include "utils.h"

int dev_hdc1000_get_temperature(float *temp_ptr)
{
	int status;
	uint16_t temp_data_value = 0;
	uint32_t flag = (REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER |
			 REG_ACCESS_FLAG_DELAY_FOR_READ);

	QM_CHECK((NULL != temp_ptr), -EINVAL);

	status = reg_read_word(REG_TEMP_DATA, &temp_data_value, flag);
	if (!status) {
		*temp_ptr =
		    (((float)temp_data_value / FRACTION_COVERSION_FACTOR) *
		     CURRENT_AT_MEASUREMENT) -
		    CONVERSION_DELTA;
	}

	return status;
}

int dev_hdc1000_get_humidity(float *humidity_ptr)
{

	int status;
	uint16_t humi_data_value = 0;
	uint32_t flag = (REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER |
			 REG_ACCESS_FLAG_DELAY_FOR_READ);

	QM_CHECK((NULL != humidity_ptr), -EINVAL);

	status = reg_read_word(REG_HUMI_DATA, &humi_data_value, flag);
	if (!status) {
		*humidity_ptr =
		    (((float)humi_data_value) / FRACTION_COVERSION_FACTOR) *
		    PERCENTAGE_CONVERSION_FACTOR;
	}

	return status;
}

int dev_hdc1000_get_sensor_data(hdc1000_measurement_mode_t measure_mode,
				hdc1000_sensor_data_t *sensor_data_ptr)
{
	int status;

	QM_CHECK((NULL != sensor_data_ptr), -EINVAL);
	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);

	switch (measure_mode) {
	case HDC1000_MEASUREMENT_MODE_TEMPERATURE:
		status =
		    dev_hdc1000_get_temperature(&sensor_data_ptr->temperature);
		break;

	case HDC1000_MEASUREMENT_MODE_HUMIDITY:
		status = dev_hdc1000_get_humidity(&sensor_data_ptr->humidity);
		break;

	case HDC1000_MEASUREMENT_MODE_COMBINED:
	default:
		status =
		    dev_hdc1000_get_temperature(&sensor_data_ptr->temperature);
		if (!status) {
			status = dev_hdc1000_get_humidity(
			    &sensor_data_ptr->humidity);
		}
	}

	return status;
}

int dev_hdc1000_configure(hdc1000_measurement_mode_t measure_mode,
			  hdc1000_resolution_t resolution,
			  hdc1000_battery_status_t battery,
			  hdc1000_soft_reset_t reset)
{

	uint16_t config_reg_value;
	int status;

	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);
	QM_CHECK((resolution <= HDC1000_RESOLUTION_14BIT), -EINVAL);
	QM_CHECK((battery <= HDC1000_BATTERY_STATUS_LOW_INDICATION_DISABLE),
		 -EINVAL);
	QM_CHECK((reset <= HDC1000_DO_SOFT_RESET), -EINVAL);

	status = reg_generate_bit_mask(measure_mode, resolution, battery, reset,
				       &config_reg_value);

	return ((status) ? status
			 : reg_write_high_byte(
			       REG_CONFIG, (uint8_t)(config_reg_value >> 8)));
}

int dev_hdc1000_get_serial_id(hdc1000_serial_id_t *serial_id_ptr)
{
	uint8_t register_adddress = REG_SERIAL_ID_FIRST_2_BYTE;
	uint16_t id_reg_value;
	int status, numreads = 3;
	uint32_t flag = (REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER |
			 REG_ACCESS_FLAG_DELAY_FOR_READ);

	QM_CHECK((NULL != serial_id_ptr), -EINVAL);

	do {
		status = reg_read_word(register_adddress, &id_reg_value, flag);
		if (status) {
			QM_PRINTF("Error: %s()  (0x%x) (%d)\n", __FUNCTION__,
				  register_adddress, status);
			break;
		}

		switch (register_adddress) {
		case REG_SERIAL_ID_FIRST_2_BYTE:
			serial_id_ptr->first_word = id_reg_value;
			break;

		case REG_SERIAL_ID_MIDDLE_2BYTE:
			serial_id_ptr->second_word = id_reg_value;
			break;

		case REG_SERIAL_ID_LAST_BYTE:
			serial_id_ptr->last_byte =
			    (uint8_t)(HIGH_BYTE(id_reg_value));
			break;
		}
		++register_adddress;
	} while (--numreads);

	return status;
}

int dev_hdc1000_read_id(hdc1000_id_t id)
{
	uint16_t id_value;
	int status;
	hdc1000_serial_id_t serial_id;
	uint32_t flag = (REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER |
			 REG_ACCESS_FLAG_DELAY_FOR_READ);

	QM_CHECK((id <= HDC1000_ID_SERIAL), -EINVAL);

	switch (id) {
	case HDC1000_ID_MANUFACTURER:
		status = reg_read_word(REG_MANUFACTURER_ID, &id_value, flag);
		if ((!status) && (HDC1000_MANUFACTURER_ID == id_value)) {
			QM_PRINTF("MANUFACTURER ID - [0x%X]\n", id_value);
		}
		break;

	case HDC1000_ID_DEVICE:
		status = reg_read_word(REG_DEVICE_ID, &id_value, flag);
		if ((!status) && (HDC1000_DEVICE_ID == id_value)) {
			QM_PRINTF("DEVICE ID - [0x%X]\n", id_value);
		}
		break;

	case HDC1000_ID_SERIAL:
		status = dev_hdc1000_get_serial_id(&serial_id);
		if (!status) {
			QM_PRINTF("SERIAL ID - [0x%X 0x%X 0x%X]\n",
				  serial_id.first_word, serial_id.second_word,
				  serial_id.last_byte);
		}
		break;

	default:
		QM_PRINTF("Error: %s() defaulted : 0x%x\n", __FUNCTION__, id);
		status = -EINVAL;
	}

	return status;
}

int dev_hdc1000_print_sensor_data(hdc1000_measurement_mode_t measure_mode,
				  hdc1000_sensor_data_t *sensor_data_ptr)
{

	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);
	QM_CHECK((sensor_data_ptr != NULL), -EINVAL);

	switch (measure_mode) {
	case HDC1000_MEASUREMENT_MODE_TEMPERATURE:
		QM_PRINTF("Temperature - ");
		util_print_float(sensor_data_ptr->temperature);
		break;

	case HDC1000_MEASUREMENT_MODE_HUMIDITY:
		QM_PRINTF("Humidity - ");
		util_print_float(sensor_data_ptr->humidity);
		break;

	case HDC1000_MEASUREMENT_MODE_COMBINED:
	default:
		QM_PRINTF("Temperature - ");
		util_print_float(sensor_data_ptr->temperature);
		QM_PRINTF(" Humidity - ");
		util_print_float(sensor_data_ptr->humidity);
	}
	QM_PRINTF("\n");
	return 0;
}
