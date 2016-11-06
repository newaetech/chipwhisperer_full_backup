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

#ifndef __HDC1000_REGISTER_H__
#define __HDC1000_REGISTER_H__

/** Temperature data register address (read only). */
#define REG_TEMP_DATA (0x00)
/** Humidity data register address (read only). */
#define REG_HUMI_DATA (0x01)
/** Config register address (read write). */
#define REG_CONFIG (0x02)
/** Serial ID register address to read first 2 bytes (read only). */
#define REG_SERIAL_ID_FIRST_2_BYTE (0XFB)
/** Serial ID register address to read middle 2 bytes (read only). */
#define REG_SERIAL_ID_MIDDLE_2BYTE (0XFC)
/** Serial ID register address to read last byte (read only). */
#define REG_SERIAL_ID_LAST_BYTE (0XFD)
/** Manufacturer ID register address (read only). */
#define REG_MANUFACTURER_ID (0XFE)
/** Device ID register address (read only). */
#define REG_DEVICE_ID (0XFF)
/** Texas Instruments ID. */
#define HDC1000_MANUFACTURER_ID (0x5449)
/** Device ID for hdc1000. */
#define HDC1000_DEVICE_ID (0X1000)
/** Macros for address register width in the sensor. */
#define HDC1000_CMD_REG_WIDTH (1)
/** Macros for  data register width in the sensor. */
#define HDC1000_DATA_REG_WIDTH (2)

/**
 * The reg_access_flag_t enum for register access flags.
 */
typedef enum {
	REG_ACCESS_FLAG_INVALID = 0x0, /**< Invalid enum value. */
	/** Do pointer register update. */
	REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER = 0x1,
	/** Add delay before read. */
	REG_ACCESS_FLAG_DELAY_FOR_READ = BIT(1)
} reg_access_flag_t;

/** Macro checks address register update selected. */
#define NEED_ADDRESS_REG_UPDATE(flag)                                          \
	(flag & REG_ACCESS_FLAG_UPDATE_ADDRESS_REGISTER)
/** Macro checks delay for register read selected. */
#define NEED_DELAY_FOR_READ(flag) (flag & REG_ACCESS_FLAG_DELAY_FOR_READ)

/**
 * The config_reg_values_t enum for set/clear bits CONFIG register.
 */
typedef enum {
	CONFIG_REG_INVALID,			/**< Invalid enum value. */
	CONFIG_REG_SET_SOFT_RESET = BIT(15),    /**< Do soft reset. */
	CONFIG_REG_SET_HEATER_ENABLE = BIT(13), /**< Heater enable. */
	/** Enable acquisition mode. */
	CONFIG_REG_ACQUISITION_MODE = BIT(12),
	/** Enable subscription for battery low indication. */
	CONFIG_REG_SET_BAT_LOW_INDICATION = BIT(11),
	/** Select 11 bit for temperature measurement. */
	CONFIG_REG_SET_TEMP_RESOLUTION_11_BIT = BIT(10),
	/** Select 11 bit for humidity measurement. */
	CONFIG_REG_SET_HUMI_RESOLUTION_11_BIT = BIT(8),
	/** Select 8 bit for humidity measurement. */
	CONFIG_REG_SET_HUMI_RESOLUTION_8_BIT = BIT(9)
} config_reg_values_t;

/**
 * Update pointer or address register.
 *
 * Update 8bit pointer register: The pointer register hold the
 * value of the register which needs to be read/written.
 * Each register write shall be preceded by update of the
 * pointer register.
 * A read operation always read the value of the register held
 * by pointer register.
 *
 * @param[in] register_address Address of register.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int reg_update_address_register(uint8_t register_address);

/**
 * Read data register.
 *
 * Read the word (16bit) size register.from device.
 *
 * @param[in] register_address Address of register.
 * @param[in] register_value_ptr Require delay between pointer.
 * register updation and read of the register.
 * @param[in] acess_flags Pass the flags for accessing the register.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int reg_read_word(uint8_t register_address, uint16_t *register_value_ptr,
		  uint32_t acess_flags);

/**
 * Write high byte of data register.
 *
 * Update the high byte of the register. Write operation is
 * performs to update the config register for which lower 8 bits
 * are reserved. This function helps to update the MSB of the
 * config register.
 *
 * @param[in] register_address Address of register.
 * @param[in] value Value to write.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int reg_write_high_byte(uint8_t register_address, uint8_t value);

/**
 * Generate bitmap for CONFIG register.
 *
 * Generate the values(bitmap) to be programmed to the CONFIG
 * register such as to control the behavior of the device.
 * example: we  can configure the device to operate in 8 bit,
 * 12 bit or 14bit mode.
 * Depending on the input enum values function generates
 * the bitmap for the config register.
 *
 * @param[in] measure_mode Mode of measurement.
 * @param[in] resolution Resolution to be used (8bit, 12bit, 14bit etc.).
 * @param[in] battery Option to chose the battery low indication.
 * @param[in] reset Input decided to do a soft reset during configuration.
 * @param[out] word_register_ptr Pointer to the memory to hold the final bitmap.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int reg_generate_bit_mask(hdc1000_measurement_mode_t measure_mode,
			  hdc1000_resolution_t resolution,
			  hdc1000_battery_status_t battery,
			  hdc1000_soft_reset_t reset,
			  uint16_t *word_register_ptr);

#endif /* __HDC1000_REGISTER_H__ */
