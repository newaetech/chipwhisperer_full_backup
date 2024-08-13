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
#include "clk.h"
#include "hdc1000_device.h"
#include "hdc1000_register.h"
#include "utils.h"
#include "hdc1000_bus.h"

int reg_generate_bit_mask(hdc1000_measurement_mode_t measure_mode,
			  hdc1000_resolution_t resolution,
			  hdc1000_battery_status_t battery,
			  hdc1000_soft_reset_t reset,
			  uint16_t *word_register_ptr)
{
	uint16_t config_bitmap = 0;

	QM_CHECK((NULL != word_register_ptr), -EINVAL);
	QM_CHECK((measure_mode <= HDC1000_MEASUREMENT_MODE_COMBINED), -EINVAL);
	QM_CHECK((resolution <= HDC1000_RESOLUTION_14BIT), -EINVAL);
	QM_CHECK((battery <= HDC1000_BATTERY_STATUS_LOW_INDICATION_DISABLE),
		 -EINVAL);
	QM_CHECK((reset <= HDC1000_DO_SOFT_RESET), -EINVAL);

	/* Select the measurement mode. */
	switch (measure_mode) {
	case HDC1000_MEASUREMENT_MODE_TEMPERATURE:
		config_bitmap |= CONFIG_REG_SET_HEATER_ENABLE;
		break;

	case HDC1000_MEASUREMENT_MODE_HUMIDITY:
		break;

	/* Default measurement mode is combined. */
	case HDC1000_MEASUREMENT_MODE_COMBINED:
	default:
		config_bitmap |= (CONFIG_REG_SET_HEATER_ENABLE |
				  CONFIG_REG_ACQUISITION_MODE);
	}

	/* Select the resolution. */
	switch (resolution) {
	case HDC1000_RESOLUTION_8BIT:
		if (HDC1000_MEASUREMENT_MODE_HUMIDITY == measure_mode) {
			config_bitmap |= CONFIG_REG_SET_HUMI_RESOLUTION_8_BIT;
		}
		break;

	case HDC1000_RESOLUTION_11BIT:
		switch (measure_mode) {
		case HDC1000_MEASUREMENT_MODE_TEMPERATURE:
			config_bitmap |= CONFIG_REG_SET_TEMP_RESOLUTION_11_BIT;
			break;

		case HDC1000_MEASUREMENT_MODE_HUMIDITY:
			config_bitmap |= CONFIG_REG_SET_HUMI_RESOLUTION_11_BIT;
			break;

		case HDC1000_MEASUREMENT_MODE_COMBINED:
		default:
			config_bitmap |=
			    (CONFIG_REG_SET_TEMP_RESOLUTION_11_BIT |
			     CONFIG_REG_SET_HUMI_RESOLUTION_11_BIT);
		}
		break;

	/* Default resolution is 14 bit. */
	case HDC1000_RESOLUTION_14BIT:
	default:
		switch (measure_mode) {
		case HDC1000_MEASUREMENT_MODE_TEMPERATURE:
			config_bitmap &=
			    (~CONFIG_REG_SET_TEMP_RESOLUTION_11_BIT);
			break;

		case HDC1000_MEASUREMENT_MODE_HUMIDITY:
			config_bitmap &=
			    (~(CONFIG_REG_SET_HUMI_RESOLUTION_11_BIT |
			       CONFIG_REG_SET_HUMI_RESOLUTION_8_BIT));
			break;

		case HDC1000_MEASUREMENT_MODE_COMBINED:
		default:
			config_bitmap &=
			    ((~CONFIG_REG_SET_TEMP_RESOLUTION_11_BIT) &
			     (~(CONFIG_REG_SET_HUMI_RESOLUTION_11_BIT |
				CONFIG_REG_SET_HUMI_RESOLUTION_8_BIT)));
		}
	}

	if (HDC1000_BATTERY_STATUS_LOW_INDICATION_ENABLE == battery) {
		config_bitmap |= CONFIG_REG_SET_BAT_LOW_INDICATION;
	} else {
		config_bitmap &= (~CONFIG_REG_SET_BAT_LOW_INDICATION);
	}

	if (HDC1000_DO_SOFT_RESET == reset) {
		config_bitmap |= CONFIG_REG_SET_SOFT_RESET;
	}
	*word_register_ptr = config_bitmap;
	return 0;
}

int reg_read_word(uint8_t register_address, uint16_t *register_value_ptr,
		  uint32_t acess_flags)
{
	int status;
	uint16_t reg_val = 0;

	QM_CHECK((NULL != register_value_ptr), -EINVAL);

	if (NEED_ADDRESS_REG_UPDATE(acess_flags)) {
		status = reg_update_address_register(register_address);
		if (status) {
			QM_PRINTF("Error: %s() fail (0x%x) (%d)\n",
				  __FUNCTION__, register_address, status);
			return status;
		}
	}

	if (NEED_DELAY_FOR_READ(acess_flags)) {
		clk_sys_udelay(WAIT_FOR_DATA_READY);
	}

	status = bus_read_data((uint8_t *)&reg_val, HDC1000_DATA_REG_WIDTH);
	if (status) {
		QM_PRINTF("Error: %s() fail (0x%x) (%d)\n", __FUNCTION__,
			  register_address, status);
		return status;
	}
	*register_value_ptr = BYTE_SWAP(reg_val);

	return status;
}

int reg_update_address_register(uint8_t register_address)
{
	int status;

	status =
	    bus_write_data_command(&register_address, HDC1000_CMD_REG_WIDTH);
	if (status) {
		QM_PRINTF("Error: %s() failed (0x%x) (%d)\n", __FUNCTION__,
			  register_address, status);
	}

	return status;
}

int reg_write_high_byte(uint8_t register_address, uint8_t value)
{
	int status;

	status = reg_update_address_register(register_address);
	if (status) {
		QM_PRINTF("Error: %s()  (0x%x) (%d)\n", __FUNCTION__,
			  register_address, status);
		return status;
	}
	status = bus_write_data_command(&value, sizeof(value));
	if (status) {
		QM_PRINTF("Error: %s() Write Failed (0x%x) (%d)\n",
			  __FUNCTION__, register_address, status);
		return status;
	}

	return status;
}
