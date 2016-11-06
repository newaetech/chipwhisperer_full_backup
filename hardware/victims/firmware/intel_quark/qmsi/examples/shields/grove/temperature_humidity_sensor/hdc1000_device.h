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

#ifndef __HDC1000_DEVICE_H__
#define __HDC1000_DEVICE_H__

/** Default I2C device address for the hdc1000 device. */
#define HDC1000_DEVICE_ADDR (0x40)
/** Common sleep time for data available in milliseconds. */
#define WAIT_FOR_DATA_READY (20000)
/** Macro for converting in to percentage. */
#define PERCENTAGE_CONVERSION_FACTOR (100.0)
/** Macro for fraction conversion. */
#define FRACTION_COVERSION_FACTOR (65536.0)
/** Macro for currernt drained during measurement. */
#define CURRENT_AT_MEASUREMENT (165.0)
/** Macro for conversion delta. */
#define CONVERSION_DELTA (40.0)

/**
 * The hdc1000_measurement_mode_t enums.
 */
typedef enum {
	HDC1000_MEASUREMENT_MODE_INVALID,     /**< Invalid mode. */
	HDC1000_MEASUREMENT_MODE_TEMPERATURE, /**< Temperature only. */
	HDC1000_MEASUREMENT_MODE_HUMIDITY,    /**< Humidity only. */
	/** Humidity & tempearture. */
	HDC1000_MEASUREMENT_MODE_COMBINED
} hdc1000_measurement_mode_t;

/**
 * The hdc1000_id_t enums are used for device identifiers.
 */
typedef enum {
	HDC1000_ID_INVALID,      /**< Invalid ID. */
	HDC1000_ID_MANUFACTURER, /**< Manufacturer ID. */
	HDC1000_ID_DEVICE,       /**< Device ID. */
	HDC1000_ID_SERIAL	/**< Serial ID. */
} hdc1000_id_t;

/**
 * The hdc1000_battery_status_t enum used for battery status.
 */
typedef enum {
	HDC1000_BATTERY_INDICATION_INVALID, /**< Invalid Indication. */
	/** Subscribe battery low indication. */
	HDC1000_BATTERY_STATUS_LOW_INDICATION_ENABLE,
	/** No subscription for battery low indication. */
	HDC1000_BATTERY_STATUS_LOW_INDICATION_DISABLE
} hdc1000_battery_status_t;

/**
 * The hdc1000_soft_reset_t enum used to for soft reset.
 */
typedef enum {
	HDC1000_SOFT_RESET_INVALID, /**< Invalid soft reset option. */
	HDC1000_DO_SOFT_RESET       /**< Do Soft Reset. */
} hdc1000_soft_reset_t;

/**
 * The hdc1000_resolution_t enums used.
 */
typedef enum {
	HDC1000_RESOLUTION_INVALID, /**< Invalid resolution. */
	HDC1000_RESOLUTION_8BIT,    /**< 8bit support for humidity alone. */
	HDC1000_RESOLUTION_11BIT,   /**< 11bit support. */
	HDC1000_RESOLUTION_14BIT    /**< 14bit support. */
} hdc1000_resolution_t;

/**
 * The hdc1000_serial_id_t structure used to store serial ID.
 */
typedef struct {
	uint16_t first_word;  /**< first 16bit [read register-0xFB]. */
	uint16_t second_word; /**< middle 16bit [read register-0xFC]. */
	uint8_t last_byte;    /**< Last 8bit [read register-0xFD]. */
} hdc1000_serial_id_t;

/**
 * The hdc1000_sensor_data_t structure used to store sensor values.
 */
typedef struct {
	float temperature; /**< Hold the final temperature. */
	float humidity;    /**< Hold the final humidity. */
} hdc1000_sensor_data_t;

/**
 * Get the sensor data for temperature and humidity.
 *
 * @param[in] measure_mode Mode of measurement.
 * @param[in,out] sensor_data_ptr Pointer to the hdc1000_sensor_data_t.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_get_sensor_data(hdc1000_measurement_mode_t measure_mode,
				hdc1000_sensor_data_t *sensor_data_ptr);

/**
 * Read temperature from device.
 *
 * Read temperature from the device register and
 * convert to degree Celcius.
 *
 * @param[out] temp_ptr Pointer to buffer to hold the temperature.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_get_temperature(float *temp_ptr);

/**
 * Read humidity from sensor.
 *
 * Read humidity from the device register and convert to
 * relative humidity.
 *
 * @param[out] humidity_ptr Pointer to buffer to hold the humidity.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_get_humidity(float *humidity_ptr);

/**
 * Read Serial ID.
 *
 * Read serial ID which is a 40 bit identifier which unique
 * for each device. Serial ID is spanned across three read only
 * registers in the device.
 *
 * @param[in,out] serial_id_ptr Pointer to the buffer.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_get_serial_id(hdc1000_serial_id_t *serial_id_ptr);

/**
 * Read read only identifiers of device.
 *
 * Read different identification values like Manufacturer,
 * Device and Serial from the device.
 *
 * @param[in] id Enum value which selects the ID to be read.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_read_id(hdc1000_id_t id);

/**
 * Configure the sensor for measurement.
 *
 * Uses QMSI exported API to pass the data to the controller.
 *
 * @param[in] measure_mode Mode of measurement.
 * @param[in] resolution Resolution to be used (8bit, 12bit, 14bit etc.).
 * @param[in] battery Option to chose the battery low indication.
 * @param[in] reset Input decided to do a soft reset during configuration.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_configure(hdc1000_measurement_mode_t measure_mode,
			  hdc1000_resolution_t resolution,
			  hdc1000_battery_status_t battery,
			  hdc1000_soft_reset_t reset);

/**
 * Print the sensor data.
 *
 * @param[in] measure_mode Mode of measurement.
 * @param[in,out] sensor_data_ptr Pointer to the hdc1000_sensor_data_t.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int dev_hdc1000_print_sensor_data(hdc1000_measurement_mode_t measure_mode,
				  hdc1000_sensor_data_t *sensor_data_ptr);
#endif /* __HDC1000_DEVICE_H__ */
