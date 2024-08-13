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

#ifndef __BMX1XX_H__
#define __BMX1XX_H__

#include "qm_common.h"
#include "qm_soc_regs.h"

#if (QUARK_D2000)
#define BMC150_SENSOR (1)
#define BMX1XX_I2C_LINK (1)
#elif(QM_SENSOR)
#define BMI160_SENSOR (1)
#define BMX1XX_SPI_SS_LINK (1)
#endif

/**
 * State of the J14 jumper on the Intel(R) Quark(TM) Microcontroller D2000
 * Development Platform.
 */
#if (QUARK_D2000)
typedef enum {
	BMC150_J14_POS_0,
	BMC150_J14_POS_1,
} bmx1xx_j14_pos_t;
#endif

typedef struct {
#if (QUARK_D2000)
	bmx1xx_j14_pos_t pos;
#endif
} bmx1xx_setup_config_t;

/**
 * Range of the accelerometer measures.
 */
typedef enum {
	BMX1XX_MODE_2G = 0x3,
	BMX1XX_MODE_4G = 0x5,
	BMX1XX_MODE_8G = 0x8,
	BMX1XX_MODE_16G = 0xC,
} bmx1xx_accel_mode_t;

/**
 * Size of the data filter.
 */
typedef enum {
#if (BMC150_SENSOR)
	BMC150_BANDWIDTH_64MS = 0x8,
	BMC150_BANDWIDTH_32MS = 0x9,
	BMC150_BANDWIDTH_16MS = 0xA,
	BMC150_BANDWIDTH_8MS = 0xB,
	BMC150_BANDWIDTH_4MS = 0xC,
	BMC150_BANDWIDTH_2MS = 0xD,
	BMC150_BANDWIDTH_1MS = 0xE,
	BMC150_BANDWIDTH_500US = 0xF,
#elif(BMI160_SENSOR)
	BMI160_BANDWIDTH_10MS = 0x8,
	BMI160_BANDWIDTH_5MS = 0x9,
	BMI160_BANDWIDTH_2500US = 0xA,
	BMI160_BANDWIDTH_1250US = 0xB,
	BMI160_BANDWIDTH_625US = 0xC,
#else
	BMX1XX_BANDWIDTH_NULL = 0x0,
#endif
} bmx1xx_bandwidth_t;

#if (BMC150_SENSOR)
typedef enum {
	BMX1XX_MAG_POWER_SUSPEND = 0x0,
	BMX1XX_MAG_POWER_ACTIVE = 0x1,
} bmx1xx_mag_power_t;
#elif(BMI160_SENSOR)
typedef enum {
	BMX1XX_MAG_POWER_SUSPEND = 0x0,
	BMX1XX_MAG_POWER_ACTIVE = 0x3,
} bmx1xx_mag_power_t;
#else
typedef enum {
	BMX1XX_MAG_POWER_NULL = 0x0,
} bmx1xx_mag_power_t;
#endif

typedef enum {
	BMX1XX_ACCEL_MODE_NORMAL = 0x11,
	BMX1XX_ACCEL_LOWPOWER = 0x12,
	BMX1XX_ACCEL_SUSPEND = 0x10,
	BMX1XX_GYRO_MODE_SUSPEND = 0x14,
	BMX1XX_GYRO_MODE_NORMAL = 0x15,
	BMX1XX_GYRO_MODE_FASTSTARTUP = 0x17,
	BMX1XX_MAG_MODE_SUSPEND = 0x18,
	BMX1XX_MAG_MODE_NORMAL = 0x19,
	BMX1XX_MAG_MODE_LOWPOWER = 0x1A,
} bmx1xx_command_mode_t;

typedef enum {
	BMX1XX_MAG_PRESET_REGULAR,
	BMX1XX_MAG_PRESET_LOW_POWER,
	BMX1XX_MAG_PRESET_ENHANCED,
	BMX1XX_MAG_PRESET_HIGH_ACCURACY,
} bmx1xx_mag_preset_t;

typedef struct {
	int16_t x, y, z;
} bmx1xx_gyro_t;

typedef struct {
	int16_t x, y, z;
} bmx1xx_accel_t;

typedef struct {
	int x, y, z;
} bmx1xx_mag_t;

/**
 * Initialize sensor parameters based on model and board.
 *
 * @brief Initialize sensor parameters.
 * @param [in] cfg Configuration descriptor.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_init(bmx1xx_setup_config_t cfg);

/**
 * Obtain readout of gyro data from sensor.
 * This feature is available for BMI160 sensor.
 *
 * @brief Obtain readout of gyro data.
 * @param [out] gyro Gyro components read.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_read_gyro(bmx1xx_gyro_t *const gyro);

/**
 * Obtain readout of accelerometer data from sensor.
 *
 * @brief Obtain readout of accelerometer.
 * @param [out] accel Accelerometer components read.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_read_accel(bmx1xx_accel_t *const accel);

/**
 * Obtain readout of magnetometer data from sensor.
 * This feature is available for BMC150 sensor.
 *
 * @brief Obtain readout of magnetometer data.
 * @param [out] mag Magnetometer components read.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_read_mag(bmx1xx_mag_t *const mag);

/**
 * Set a resolution mode for accelerometer. Supported ranges are
 * in the range of 2 to 16G.
 *
 * @brief Set a resolution mode for accelerometer.
 * @param [in] mode Accelerometer mode
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_accel_set_mode(bmx1xx_accel_mode_t mode);

/**
 * Set sensor internal sampling period. Supported values differ
 * for BMC150 and BMI160 sensors.
 *
 * @brief Set sensor internal sampling period.
 * @param [in] Bandwitdth period
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_set_bandwidth(bmx1xx_bandwidth_t bw);

/**
 * Set magnetometer sensor power mode.
 * This feature is available for BMC150 sensor.
 *
 * @brief Set magnetometer sensor power mode.
 * @param [in] Power mode
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_mag_set_power(bmx1xx_mag_power_t power);

/**
 * Set magnetometer mode of operation.
 * This feature is available for BMC150 sensor.
 *
 * @brief Set magnetometer mode of operation.
 * @param [in] Operation mode
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_mag_set_preset(bmx1xx_mag_preset_t preset);

/**
 * Send command to sensor.
 * This feature is available for BMI160 sensor.
 *
 * @brief Send command to sensor.
 * @param [in] Command mode
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int bmx1xx_set_command(bmx1xx_command_mode_t mode);

#endif /* __BMX1XX_H__ */
