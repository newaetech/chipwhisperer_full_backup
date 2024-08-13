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

#include "itg3200_gyro.h"

/* Slave address macros based pin 9 logic. */
#define ITG3200_GYRO_SLAVE_ADDR_PIN9_LOW (0x68)
#define ITG3200_GYRO_SLAVE_ADDR_PIN9_HIGH (0x69)
/* The sensitivity of the sensor to calculate the angular velocity. */
#define SENSITIVITY (14.375)
/* Only 2 digits to be printed in exponent part of float. */
#define PRECISION (100)
/* Total Size of sensor data registers. */
#define ITG3200_DATA_REGS_SIZE (6)

/* ITG3200 gyro sensor register maps. */
typedef enum {
	ITG3200_GYRO_WHO = 0x00,
	ITG3200_GYRO_SMP_RD = 0x15,
	ITG3200_GYRO_DLPF_FS = 0x16,
	ITG3200_GYRO_INT_CFG = 0x17,
	ITG3200_GYRO_INT_STATUS = 0x1A,
	ITG3200_GYRO_TMP_HIGH = 0x1B,
	ITG3200_GYRO_TMP_LOW = 0x1C,
	ITG3200_GYRO_GX_HIGH = 0x1D,
	ITG3200_GYRO_GX_LOW = 0x1E,
	ITG3200_GYRO_GY_HIGH = 0x1F,
	ITG3200_GYRO_GY_LOW = 0x20,
	ITG3200_GYRO_GZ_HIGH = 0x21,
	ITG3200_GYRO_GZ_LOW = 0x22,
	ITG3200_GYRO_PWR_MGM = 0x3E
} itg3200_gyro_regs;

/* ITG3200 gyro sensor register values. */
typedef enum {
	ITG3200_H_RST = 0x80,
	ITG3200_FS_SEL = 0x18,
} itg3200_gyro_reg_values;

/* ITG3200 gyro x, y, z axis. */
typedef enum { X_AXIS = 0, Y_AXIS, Z_AXIS } itg3200_3_axis;

/* Slave address of the device is set in the variable. */
static uint16_t itg3200_slave_addr = ITG3200_GYRO_SLAVE_ADDR_PIN9_LOW;

/*
 * Read the data from the registers.
 *
 * This routine reads the lower and higher byte of the x, y and
 * z axes and stores value to xyz_axis pointer.
 */
static int itg3200_gyro_read_xyzaxis(int16_t *xyz_axis)
{
	int rc;
	qm_i2c_status_t status;
	uint8_t reg = ITG3200_GYRO_GX_HIGH;

	rc = qm_i2c_master_write(QM_I2C_X, itg3200_slave_addr, &reg,
				 sizeof(reg), false, &status);
	if (rc) {
		QM_PRINTF(
		    "Error: qm_i2c_master_write failed for reg = 0x%x with "
		    "\n error = 0x%x \ni2c_status = 0x%x\n",
		    reg, rc, status);
		return rc;
	}

	rc = qm_i2c_master_read(QM_I2C_X, itg3200_slave_addr,
				(uint8_t *)xyz_axis, ITG3200_DATA_REGS_SIZE,
				true, &status);
	if (rc) {
		QM_PRINTF(
		    "Error: qm_i2c_master_read failed for reg = 0x%x with \n"
		    "error = 0x%x \ni2c_status = 0x%x\n",
		    reg, rc, status);
		return rc;
	}

	return 0;
}

/*
 * Write data to register.
 *
 * This routine writes a value into the register.
 */
static int itg3200_gyro_reg_write(uint8_t reg, uint8_t wdata)
{
	int rc;
	qm_i2c_status_t status;

	rc = qm_i2c_master_write(QM_I2C_X, itg3200_slave_addr, &reg,
				 sizeof(reg), false, &status);
	if (rc) {
		QM_PRINTF(
		    "Error: qm_i2c_master_write failed! for reg = 0x%x with \n"
		    "error = 0x%x \ni2c_status = 0x%x \n",
		    reg, rc, status);
		return rc;
	}

	rc = qm_i2c_master_write(QM_I2C_X, itg3200_slave_addr, &wdata,
				 sizeof(wdata), true, &status);
	if (rc) {
		QM_PRINTF(
		    "Error: qm_i2c_master_write failed for reg = 0x%x with"
		    "\n error = 0x%x \ni2c_status = 0x%x \n",
		    reg, rc, status);
		return rc;
	}

	return 0;
}

/*
 * Print the value in float format.
 *
 * This routine helps to print the values in float format as the
 * format specifier for float is not available with QMSI.
 */
void itg3200_gyro_print_float(float ax, float ay, float az, int16_t *axis)
{
	float man_ax, man_ay, man_az;
	int16_t exp_ax = (int16_t)ax, exp_ay = (int16_t)ay,
		exp_az = (int16_t)az;

	man_ax = ax - (float)exp_ax;
	man_ay = ay - (float)exp_ay;
	man_az = az - (float)exp_az;

	/* The unit of angular velocity is degrees/s */
	QM_PRINTF("Raw x = %d y = %d z = %d  Angular vel ax = %d.%d "
		  "ay = %d.%d az = %d.%d\n",
		  axis[X_AXIS], axis[Y_AXIS], axis[Z_AXIS], exp_ax,
		  (uint16_t)(man_ax * PRECISION), exp_ay,
		  (uint16_t)(man_ay * PRECISION), exp_az,
		  (uint16_t)(man_az * PRECISION));
}

/*
 * Calculate the angular velocity of the device.
 *
 * This routine gets the x, y, z axis values of ITG-3200 and
 * calculates the angular velocity based on the sensitivity
 * of the device and print the calculated values as float.
 * The unit of angular velocity is degrees/s.
 */
int itg3200_gyro_get_angular_vel(float *ax, float *ay, float *az, int16_t *axis)
{
	uint16_t i = 0;

	/* Get the x, y, z axis of gyro-sensor. */
	if (itg3200_gyro_read_xyzaxis(axis)) {
		return -EIO;
	}

	/* Swap bytes to form proper x, y, z axis. */
	while (i < ITG3200_MAX_AXIS) {
		*(axis + i) = (((*(axis + i) & 0x00ff) << 8) |
			       ((*(axis + i) & 0xff00) >> 8));
		i++;
	}

	*ax = (axis[X_AXIS] / SENSITIVITY);
	*ay = (axis[Y_AXIS] / SENSITIVITY);
	*az = (axis[Z_AXIS] / SENSITIVITY);

	return 0;
}

/*
 * Set initial configuration for device registers.
 *
 * This routine initializes the device registers with default
 * values at power-up.
 */
int itg3200_gyro_cfg_init(void)
{
	int rc;

	/* Reset device and internal registers to the default settings. */
	rc = itg3200_gyro_reg_write(ITG3200_GYRO_PWR_MGM, ITG3200_H_RST);
	if (rc) {
		return rc;
	}
	/* Set sampling rate default value to zero. */
	rc = itg3200_gyro_reg_write(ITG3200_GYRO_SMP_RD, 0x00);
	if (rc) {
		return rc;
	}
	/* Configure the device to full scale range of =/- 2000 rad/s. */
	rc = itg3200_gyro_reg_write(ITG3200_GYRO_DLPF_FS, ITG3200_FS_SEL);
	if (rc) {
		return rc;
	}

	return 0;
}
