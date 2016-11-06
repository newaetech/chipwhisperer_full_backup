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

#ifndef __ITG3200_GYRO_H__
#define __ITG3200_GYRO_H__

#include "qm_i2c.h"
#include "qm_pinmux.h"

/** Maximum number of axis. */
#define ITG3200_MAX_AXIS (3)

/**
 * I2C configuration values based on SoC.
 */
typedef enum {
#if (QUARK_SE)
	QM_I2C_X = QM_I2C_1,	 /**< Configures I2C controller 1. */
	QM_PMUX_FN_X = QM_PMUX_FN_0, /**< Configure GPIO pin function. */
	QM_PIN_ID_XX = QM_PIN_ID_22, /**< Configure pin id 22. */
	QM_PIN_ID_YY = QM_PIN_ID_23, /**< Configure pin id 23. */
	/** I2C master 1 clock gate enable. */
	CLK_PERIPH_I2C_MX_REGISTER = CLK_PERIPH_I2C_M1_REGISTER
#elif(QUARK_D2000)
	QM_I2C_X = QM_I2C_0,	 /**< Configures I2C controller 0. */
	QM_PMUX_FN_X = QM_PMUX_FN_2, /**< Configure GPIO pin function. */
	QM_PIN_ID_XX = QM_PIN_ID_6,  /**< Configure pin id 6. */
	QM_PIN_ID_YY = QM_PIN_ID_7,  /**< Configure pin id 7. */
	/** I2C master 0 clock gate enable. */
	CLK_PERIPH_I2C_MX_REGISTER = CLK_PERIPH_I2C_M0_REGISTER
#endif
} itg3200_gyro_i2c_platform_config;

/**
 * Set initial configuration for device registers.
 *
 * This routine initializes the device registers with default values at
 * power-up.
 *
 * @return Standard errno return type.
 * @retval 0 on success
 * @retval Negative @ref errno for possible error codes.
 */
int itg3200_gyro_cfg_init(void);

/**
 * Calculate the angular velocity of the device.
 *
 * This routine gets the x, y, z axis values of ITG-3200 and
 * calculates the angular velocity based on the sensitivity
 * of the device and print the calculated values as float.
 * The unit of angular velocity is degrees/s.
 *
 * @param[out] ax Angular velocity of x.
 * @param[out] ay Angular velocity of y.
 * @param[out] az Angular velocity of z.
 * @param[out] axis Raw axis of x, y, z.
 *
 * @return Standard errno return type.
 * @retval 0 on success
 * @retval Negative @ref errno for possible error codes.
 */
int itg3200_gyro_get_angular_vel(float *ax, float *ay, float *az,
				 int16_t *axis);

/**
 * Print the value in float format.
 *
 * This routine helps to print the values in float format as the
 * format specifier for float is not available with QMSI.
 *
 * @param[in] ax Angular velocity of x.
 * @param[in] ay Angular velocity of y.
 * @param[in] az Angular velocity of z.
 * @param[in] axis Raw axis of x, y, z.
 */
void itg3200_gyro_print_float(float ax, float ay, float az, int16_t *axis);

#endif /*__ITG3200_GYRO_H__*/
