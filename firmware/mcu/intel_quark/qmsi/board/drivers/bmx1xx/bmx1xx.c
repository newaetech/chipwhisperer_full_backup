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

#if (QM_SENSOR)

#include "qm_ss_spi.h"
#include "ss_clk.h"

#else

#include <x86intrin.h>
#include "qm_i2c.h"
#include "qm_pinmux.h"
#include "clk.h"

#endif /*QM_SENSOR*/

#include "bmx1xx.h"

/**
 * Driver provides support of Bosch BMX1XX drivers for Quark SE and D2000.
 * Supported devices: Bosch BMC150, BMI160
 * Supported configurations:
 *  1. Quark D2000. Supports measurement of acceleration and magnetometer
 *     data from a BMC150 sensor connected over I2C_0 (see Quark D2000
 *     eval board schematics).
 *  2. Quark SE. Supports measurmenet of acceleration and angular rate
 *     (gyro) data from a BMI160 sensor connected over SPI_0 from SS (see
 *     Quark SE eval board schematics).
 */

#if (BMC150_SENSOR || BMI160_SENSOR)

/* BMC150 registers */
#if (BMC150_SENSOR)

#define BMC150_I2C_ACCEL_ADDR (0x10)
#define BMC150_I2C_MAG_ADDR (0x12)

#define BMX1XX_REG_ACCEL_CHIPID (0x00)
#define BMX1XX_REG_ACCEL_X_LSB (0x02)
#define BMX1XX_REG_ACCEL_X_MSB (0x03)
#define BMX1XX_REG_ACCEL_Y_LSB (0x04)
#define BMX1XX_REG_ACCEL_Y_MSB (0x05)
#define BMX1XX_REG_ACCEL_Z_LSB (0x06)
#define BMX1XX_REG_ACCEL_Z_MSB (0x07)
#define BMX1XX_REG_ACCEL_PMU_RANGE (0x0F)
#define BMX1XX_REG_ACCEL_PMU_BW (0x10)

#define BMX1XX_REG_MAG_X_LSB (0x42)
#define BMX1XX_REG_MAG_X_MSB (0x43)
#define BMX1XX_REG_MAG_Y_LSB (0x44)
#define BMX1XX_REG_MAG_Y_MSB (0x45)
#define BMX1XX_REG_MAG_Z_LSB (0x46)
#define BMX1XX_REG_MAG_Z_MSB (0x47)
#define BMX1XX_REG_MAG_RHALL_LSB (0x48)
#define BMX1XX_REG_MAG_RHALL_MSB (0x49)
#define BMX1XX_REG_MAG_POWER_MODES (0x4B)
#define BMX1XX_REG_MAG_OPERATION_MODES (0x4C)
#define BMX1XX_REG_MAG_REPETION_XY (0x51)
#define BMX1XX_REG_MAG_REPETION_Z (0x52)
#define BMX1XX_CHIP_ID_MAJOR (0xFA)

/* BMI160 registers */
#elif(BMI160_SENSOR)

#define BMI160_I2C_ADDR (0xD0)

#define BMX1XX_REG_ACCEL_CHIPID (0x00)
#define BMX1XX_REG_ACCEL_X_LSB (0x12)
#define BMX1XX_REG_ACCEL_X_MSB (0x13)
#define BMX1XX_REG_ACCEL_Y_LSB (0x14)
#define BMX1XX_REG_ACCEL_Y_MSB (0x15)
#define BMX1XX_REG_ACCEL_Z_LSB (0x16)
#define BMX1XX_REG_ACCEL_Z_MSB (0x17)
#define BMX1XX_REG_ACCEL_PMU_RANGE (0x41)
#define BMX1XX_REG_ACCEL_PMU_BW (0x40)

#define BMX1XX_REG_MAG_X_LSB (0x04)
#define BMX1XX_REG_MAG_X_MSB (0x05)
#define BMX1XX_REG_MAG_Y_LSB (0x06)
#define BMX1XX_REG_MAG_Y_MSB (0x07)
#define BMX1XX_REG_MAG_Z_LSB (0x08)
#define BMX1XX_REG_MAG_Z_MSB (0x09)
#define BMX1XX_REG_MAG_RHALL_LSB (0x0A)
#define BMX1XX_REG_MAG_RHALL_MSB (0x0B)
#define BMX1XX_REG_GYR_X_LSB (0x0C)
#define BMX1XX_REG_GYR_X_MSB (0x0D)
#define BMX1XX_REG_GYR_Y_LSB (0x0E)
#define BMX1XX_REG_GYR_Y_MSB (0x0F)
#define BMX1XX_REG_GYR_Z_LSB (0x10)
#define BMX1XX_REG_GYR_Z_MSB (0x11)
#define BMX1XX_REG_MAG_POWER_MODES (0x03)
#define BMX1XX_REG_MAG_OPERATION_MODES (0x4C)
#define BMX1XX_REG_COMMAND (0x7E)
#define BMX1XX_CHIP_ID_MAJOR (0xD1)

#endif

/* Undocumented registers necessary for temperature compensation. */
#define BMX1XX_REG_MAG_DIG_X1 (0x5D)
#define BMX1XX_REG_MAG_DIG_Y1 (0x5E)
#define BMX1XX_REG_MAG_DIG_Z4_LSB (0x62)
#define BMX1XX_REG_MAG_DIG_Z4_MSB (0x63)
#define BMX1XX_REG_MAG_DIG_X2 (0x64)
#define BMX1XX_REG_MAG_DIG_Y2 (0x65)
#define BMX1XX_REG_MAG_DIG_Z2_LSB (0x68)
#define BMX1XX_REG_MAG_DIG_Z2_MSB (0x69)
#define BMX1XX_REG_MAG_DIG_Z1_LSB (0x6A)
#define BMX1XX_REG_MAG_DIG_Z1_MSB (0x6B)
#define BMX1XX_REG_MAG_DIG_XYZ1_LSB (0x6C)
#define BMX1XX_REG_MAG_DIG_XYZ1_MSB (0x6D)
#define BMX1XX_REG_MAG_DIG_Z3_LSB (0x6E)
#define BMX1XX_REG_MAG_DIG_Z3_MSB (0x6F)
#define BMX1XX_REG_MAG_DIG_XY2 (0x70)
#define BMX1XX_REG_MAG_DIG_XY1 (0x71)

/* Overflow */
#define BMX1XX_FLIP_OVERFLOW_ADCVAL (-4096)
#define BMX1XX_HALL_OVERFLOW_ADCVAL (-16384)
#define BMX1XX_OVERFLOW_OUTPUT (0x8000)

#define BMX1XX_WRITE_WAIT_USECS 450

#define SS_LOOPS_PER_US 4

/* Raw data conversion */
#if (BMC150_SENSOR)
#define EXTRACT_ACCEL(msb, lsb) ((int16_t)((lsb) | (msb) << 8) >> 4)
#elif(BMI160_SENSOR)
#define EXTRACT_ACCEL(msb, lsb) ((int16_t)((lsb) | (msb) << 8))
#endif

#define EXTRACT_GYRO(msb, lsb) ((int16_t)((lsb) | (msb) << 8) >> 4)

#define EXTRACT_MAG_XY(msb, lsb) ((int16_t)((lsb) | (msb) << 8) >> 3)
#define EXTRACT_MAG_Z(msb, lsb) ((int16_t)((lsb) | (msb) << 8) >> 1)
#define EXTRACT_RHALL(msb, lsb) ((int16_t)((lsb) | (msb) << 8) >> 2)
#define EXTRACT_UINT16(msb, lsb) ((uint16_t)((lsb) | (msb) << 8))

static struct compensation {
	int8_t x1, y1, x2, y2, xy1, xy2;
	uint16_t z1, z2, z3, z4, xyz1;
	bool init;
} digital_comp;

#if (BMC150_SENSOR)
/*
 * Values used for presets are from Table 6 (page 24) of the BMC150 datasheet.
 */
static const uint8_t operation_modes[] = {
    0,      /* 10 Hz */
    0,      /* 10 Hz */
    0,      /* 10 Hz */
    5 << 3, /* 20 Hz */
};

/* The formula here is repetions = 1 + 2*(mode) */
static const uint8_t repetion_modes_xy[] = {
    1, 3, 7, 23,
};

/* The formula here is repetions = 1 + (mode) */
static const uint8_t repetion_modes_z[] = {
    2, 14, 26, 82,
};

static uint16_t accel_addr = BMC150_I2C_ACCEL_ADDR;
static uint16_t magneto_addr = BMC150_I2C_MAG_ADDR;
#elif(BMI160_SENSOR)
static uint16_t accel_addr = BMI160_I2C_ADDR;
static uint16_t magneto_addr = BMI160_I2C_ADDR;
#endif /*(BMC150_SENSOR)*/

/* SPI protocol */
#define SPI_CLOCK_DIV (32) /* 1Mhz */
#define SPI_READ_SIZE (2)
#define SPI_WRITE_SIZE (2)
#define SPI_READ_REQ (0x80)
#define SPI_WRITE_REQ (0x0)
#define SPI_REG_OFFSET (0)
#define SPI_DATA_OFFSET (1)

static void busy_wait(uint32_t usecs_wait)
{
#if (QM_SENSOR)
	volatile uint32_t count = usecs_wait * SS_LOOPS_PER_US;
	while (count--)
		;
#else
	clk_sys_udelay(usecs_wait);
#endif /*(QM_SENSOR)*/
}

/* Interface constants and functions for SPI connected device to SS */
#if (BMX1XX_SPI_SS_LINK)

#define BMX1XX_SPI QM_SS_SPI_0
#define BMX1XX_SPI_SLAVE QM_SS_SPI_SS_0

static qm_ss_spi_config_t spi_config;

static int spi_read_register(uint8_t reg, uint8_t *const data, uint32_t len)
{
	qm_ss_spi_transfer_t trans;
	qm_ss_spi_status_t status;
	uint8_t tx_buffer[SPI_READ_SIZE];
	uint8_t rx_buffer[SPI_READ_SIZE];
	int rc;
	uint32_t i;

	for (i = 0; i < len; i++) {

		tx_buffer[SPI_REG_OFFSET] = SPI_READ_REQ + reg + i;

		trans.rx = rx_buffer;
		trans.tx = tx_buffer;
		trans.rx_len = SPI_READ_SIZE;
		trans.tx_len = SPI_READ_SIZE;

		/* Enable clock for SPI */
		rc = ss_clk_spi_enable(BMX1XX_SPI);
		if (rc != 0) {
			return rc;
		}

		/* Do the actual SPI transfer */
		rc = qm_ss_spi_set_config(BMX1XX_SPI, &spi_config);
		if (rc != 0) {
			return rc;
		}

		rc = qm_ss_spi_slave_select(BMX1XX_SPI, BMX1XX_SPI_SLAVE);
		if (rc != 0) {
			return rc;
		}

		rc = qm_ss_spi_transfer(BMX1XX_SPI, &trans, &status);
		if (rc != 0) {
			return rc;
		}

		/* Disable clock for SPI */
		rc = ss_clk_spi_disable(BMX1XX_SPI);
		if (rc != 0) {
			return rc;
		}

		*(data + i) = rx_buffer[SPI_DATA_OFFSET];
	}
	return 0;
}

static int spi_write_register(uint8_t reg, uint8_t const data)
{
	qm_ss_spi_transfer_t trans;
	qm_ss_spi_status_t status;
	uint8_t tx_buffer[SPI_WRITE_SIZE];
	uint8_t rx_buffer[SPI_WRITE_SIZE];
	int rc;

	tx_buffer[SPI_REG_OFFSET] = SPI_WRITE_REQ + reg;
	tx_buffer[SPI_DATA_OFFSET] = data;

	trans.rx = rx_buffer;
	trans.tx = tx_buffer;
	trans.rx_len = SPI_WRITE_SIZE;
	trans.tx_len = SPI_WRITE_SIZE;

	/* Enable clock for SPI */
	rc = ss_clk_spi_enable(BMX1XX_SPI);
	if (rc != 0) {
		return rc;
	}

	/* Do the actual SPI transfer */
	rc = qm_ss_spi_set_config(BMX1XX_SPI, &spi_config);
	if (rc != 0) {
		return rc;
	}

	rc = qm_ss_spi_slave_select(BMX1XX_SPI, BMX1XX_SPI_SLAVE);
	if (rc != 0) {
		return rc;
	}

	rc = qm_ss_spi_transfer(BMX1XX_SPI, &trans, &status);
	if (rc != 0) {
		return rc;
	}

	/* Disable clock for SPI */
	rc = ss_clk_spi_disable(BMX1XX_SPI);
	if (rc != 0) {
		return rc;
	}

	busy_wait(BMX1XX_WRITE_WAIT_USECS);

	return 0;
}

static int spi_init(void)
{
	spi_config.frame_size = QM_SS_SPI_FRAME_SIZE_8_BIT;
	spi_config.transfer_mode = QM_SS_SPI_TMOD_TX_RX;
	spi_config.clk_divider = SPI_CLOCK_DIV;
	spi_config.bus_mode = QM_SS_SPI_BMODE_0;

	return 0;
}

/* Interface constants and functions for I2C connected device */
#elif(BMX1XX_I2C_LINK)

/* I2C bus */
#if (QUARK_D2000)
#define BMX1XX_I2C QM_I2C_0
#endif /*(QUARK_D2000)*/

static int i2c_init(void)
{
	int rc;
	static const qm_i2c_config_t i2c_config = {
	    .address_mode = QM_I2C_7_BIT,
	    .mode = QM_I2C_MASTER,
	    .speed = QM_I2C_SPEED_STD,
	};

	rc = clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_M0_REGISTER);
	if (rc != 0) {
		return rc;
	}

#if (QUARK_D2000)
	rc = qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_2);
	if (rc != 0) {
		return rc;
	}

	rc = qm_pmux_select(QM_PIN_ID_7, QM_PMUX_FN_2);
	if (rc != 0) {
		return rc;
	}
#endif /*(QUARK_D2000)*/

	rc = qm_i2c_set_config(BMX1XX_I2C, &i2c_config);
	if (rc != 0) {
		return rc;
	}

	return 0;
}

/*
 * From the BMC150 datasheet, page 125, and BMI160 datasheet in page 92:
 * "The register address is automatically incremented and, therefore, more
 * than one byte can be sequentially read out. Once a new data read
 * transmission starts, the start address will be set to the register address
 * specified in the latest I2C write command."
 */
static int i2c_read_register(uint16_t addr, uint8_t reg, uint8_t *const data,
			     uint32_t len)
{
	int rc;
	qm_i2c_status_t status;

	rc = qm_i2c_master_write(BMX1XX_I2C, addr, &reg, sizeof(reg), false,
				 &status);
	if (rc != 0) {
		return rc;
	}

	rc = qm_i2c_master_read(BMX1XX_I2C, addr, data, len, true, &status);
	if (rc != 0) {
		return rc;
	}

	return 0;
}

static int i2c_write_register(uint16_t addr, uint8_t reg, uint8_t data)
{
	int rc;
	qm_i2c_status_t status;

	rc = qm_i2c_master_write(BMX1XX_I2C, addr, &reg, sizeof(reg), false,
				 &status);
	if (rc != 0) {
		return rc;
	}

	rc = qm_i2c_master_write(BMX1XX_I2C, addr, &data, sizeof(data), true,
				 &status);
	if (rc != 0) {
		return rc;
	}

	busy_wait(BMX1XX_WRITE_WAIT_USECS);

	return 0;
}

#endif /*(BMX1XX_SPI_SS_LINK)*/

static int read_register(uint16_t addr, uint8_t reg, uint8_t *const data,
			 uint32_t len)
{
#if (BMX1XX_SPI_SS_LINK)
	return spi_read_register(reg, data, len);
#elif(BMX1XX_I2C_LINK)
	return i2c_read_register(addr, reg, data, len);
#else
	return -ENODEV;
#endif /*(BMX1XX_SPI_SS_LINK)*/
}

static int write_register(uint16_t addr, uint8_t reg, uint8_t data)
{
#if (BMX1XX_SPI_SS_LINK)
	return spi_write_register(reg, data);
#elif(BMX1XX_I2C_LINK)
	return i2c_write_register(addr, reg, data);
#else
	return -ENODEV;
#endif /*(BMX1XX_SPI_SS_LINK)*/
}

static int hw_link_init(void)
{
#if (BMX1XX_SPI_SS_LINK)
	return spi_init();
#elif(BMX1XX_I2C_LINK)
	return i2c_init();
#else
	return -ENODEV;
#endif /*(BMX1XX_SPI_SS_LINK)*/
}

int bmx1xx_read_gyro(bmx1xx_gyro_t *const gyro)
{
#if (BMI160_SENSOR)
	int rc;
	uint8_t raw_gyro[6];

	/* Reading the 6 registers at once. */
	rc = read_register(accel_addr, BMX1XX_REG_GYR_X_LSB, raw_gyro,
			   sizeof(raw_gyro));
	if (rc != 0) {
		return rc;
	}

	gyro->x = EXTRACT_GYRO(raw_gyro[1], raw_gyro[0]);
	gyro->y = EXTRACT_GYRO(raw_gyro[3], raw_gyro[2]);
	gyro->z = EXTRACT_GYRO(raw_gyro[5], raw_gyro[4]);

	return 0;
#else
	return -ENODEV;
#endif /*(BMI160_SENSOR)*/
}

int bmx1xx_read_accel(bmx1xx_accel_t *const accel)
{
	int rc;
	uint8_t raw_accel[6];

	/* Reading the 6 registers at once. */
	rc = read_register(accel_addr, BMX1XX_REG_ACCEL_X_LSB, raw_accel,
			   sizeof(raw_accel));
	if (rc != 0) {
		return rc;
	}

	accel->x = EXTRACT_ACCEL(raw_accel[1], raw_accel[0]);
	accel->y = EXTRACT_ACCEL(raw_accel[3], raw_accel[2]);
	accel->z = EXTRACT_ACCEL(raw_accel[5], raw_accel[4]);

	return 0;
}

int bmx1xx_accel_set_mode(bmx1xx_accel_mode_t mode)
{
	return write_register(accel_addr, BMX1XX_REG_ACCEL_PMU_RANGE, mode);
}

int bmx1xx_set_bandwidth(bmx1xx_bandwidth_t bw)
{
	return write_register(accel_addr, BMX1XX_REG_ACCEL_PMU_BW, bw);
}

static int read_dig_comp(struct compensation *const comp)
{
	uint8_t raw_comp[21];
	int rc;

	rc = read_register(magneto_addr, BMX1XX_REG_MAG_DIG_X1, raw_comp,
			   sizeof(raw_comp));
	if (rc != 0) {
		return rc;
	}

	comp->x1 = raw_comp[0];
	comp->y1 = raw_comp[1];
	comp->z4 = EXTRACT_UINT16(raw_comp[6], raw_comp[5]);
	comp->x2 = raw_comp[7];
	comp->y2 = raw_comp[8];
	comp->z2 = EXTRACT_UINT16(raw_comp[12], raw_comp[11]);
	comp->z1 = EXTRACT_UINT16(raw_comp[14], raw_comp[13]);
	comp->xyz1 = EXTRACT_UINT16(raw_comp[16], raw_comp[15]);
	comp->z3 = EXTRACT_UINT16(raw_comp[18], raw_comp[17]);
	comp->xy2 = raw_comp[19];
	comp->xy1 = raw_comp[20];

	return 0;
}

typedef enum {
	AXIS_X,
	AXIS_Y,
} axis_t;

static int compensate_xy(struct compensation *const comp, int16_t rhall,
			 int16_t raw, axis_t axis)
{
	int inter = 0;
	int value;

	if (rhall != 0) {
		inter = ((int)comp->xyz1 << 14) / rhall - (1 << 14);
	}

	if (BMX1XX_FLIP_OVERFLOW_ADCVAL == raw) {
		return BMX1XX_OVERFLOW_OUTPUT;
	}

	value = comp->xy2 * ((inter * inter) >> 7);
	value += inter * ((int)comp->xy1 << 7);
	value >>= 9;
	value += 1 << (8 + 12);
	value *= (int)(AXIS_X == axis ? comp->x2 : comp->y2) + 160;
	value >>= 12;
	value *= raw;
	value >>= 13;
	value += (int)(AXIS_X == axis ? comp->x1 : comp->y1) << 3;

	return value;
}

static int compensate_z(struct compensation *const comp, int rhall, int16_t raw)
{
	int dividend, divisor;
	int value;

	if (BMX1XX_HALL_OVERFLOW_ADCVAL == raw) {
		return BMX1XX_OVERFLOW_OUTPUT;
	}

	dividend = (raw - (int)comp->z4) << 15;
	dividend -= (comp->z3 * (rhall - (int)comp->xyz1)) >> 2;
	/* add 1 << 15 to round to next integer. */
	divisor = (int)comp->z1 * (rhall << 1) + (1 << 15);
	divisor >>= 16;
	divisor += (int)comp->z2;
	value = dividend / divisor;
	if (value > (1 << 15) || value < -(1 << 15)) {
		value = BMX1XX_OVERFLOW_OUTPUT;
	}

	return value;
}

int bmx1xx_read_mag(bmx1xx_mag_t *const mag)
{
	int rc = 0;
	uint8_t raw_mag[8];
	int16_t x, y, z, rhall;

	if (!digital_comp.init) {
		rc = read_dig_comp(&digital_comp);
		if (rc == 0) {
			digital_comp.init = true;
		}
	}

	if (rc != 0) {
		return rc;
	}

	rc = read_register(magneto_addr, BMX1XX_REG_MAG_X_LSB, raw_mag,
			   sizeof(raw_mag));
	if (rc != 0) {
		return rc;
	}

	x = EXTRACT_MAG_XY(raw_mag[1], raw_mag[0]);
	y = EXTRACT_MAG_XY(raw_mag[3], raw_mag[2]);
	z = EXTRACT_MAG_Z(raw_mag[5], raw_mag[4]);
	rhall = EXTRACT_RHALL(raw_mag[7], raw_mag[6]);

	mag->x = compensate_xy(&digital_comp, rhall, x, AXIS_X);
	mag->y = compensate_xy(&digital_comp, rhall, y, AXIS_Y);
	mag->z = compensate_z(&digital_comp, rhall, z);

	return 0;
}

int bmx1xx_mag_set_power(bmx1xx_mag_power_t power)
{
	return write_register(magneto_addr, BMX1XX_REG_MAG_POWER_MODES, power);
}

int bmx1xx_mag_set_preset(bmx1xx_mag_preset_t preset)
{
#if (BMC150_SENSOR)
	int rc;

	rc = write_register(magneto_addr, BMX1XX_REG_MAG_OPERATION_MODES,
			    operation_modes[preset]);
	if (rc != 0) {
		return rc;
	}

	rc = write_register(magneto_addr, BMX1XX_REG_MAG_REPETION_XY,
			    repetion_modes_xy[preset]);
	if (rc != 0) {
		return rc;
	}

	rc = write_register(magneto_addr, BMX1XX_REG_MAG_REPETION_Z,
			    repetion_modes_z[preset]);
	if (rc != 0) {
		return rc;
	}
	return 0;
#else
	return -ENODEV;
#endif /*(BMC150_SENSOR)*/
}

int bmx1xx_set_command(bmx1xx_command_mode_t mode)
{
#if (BMI160_SENSOR)
	return write_register(magneto_addr, BMX1XX_REG_COMMAND, mode);
#else
	return -ENODEV;
#endif /*(BMI160_SENSOR)*/
}

int bmx1xx_init(bmx1xx_setup_config_t cfg)
{
	int rc;
	uint8_t data;

	hw_link_init();

#if (QUARK_D2000)
	if (cfg.pos == BMC150_J14_POS_1) {
		accel_addr = 0x11;
		magneto_addr = 0x13;
	}
#endif /*(QUARK_D2000)*/

	rc = read_register(accel_addr, BMX1XX_REG_ACCEL_CHIPID, &data,
			   sizeof(data));
	if (rc != 0) {
		return rc;
	}

	if (data != BMX1XX_CHIP_ID_MAJOR) {
		return -ENODEV;
	}

#if (BMI160_SENSOR)
	/* Secondary magnetometer attached to BMI160 not available */
	rc = bmx1xx_set_command(BMX1XX_MAG_MODE_SUSPEND);
	if (rc != 0) {
		return rc;
	}

	rc = bmx1xx_set_command(BMX1XX_ACCEL_MODE_NORMAL);
	if (rc != 0) {
		return rc;
	}

	rc = bmx1xx_set_command(BMX1XX_GYRO_MODE_NORMAL);
	if (rc != 0) {
		return rc;
	}
#endif /*(BMI160_SENSOR)*/

	return 0;
}

#else

int bmx1xx_read_gyro(bmx1xx_gyro_t *const gyro)
{
	return -ENODEV;
}

int bmx1xx_read_accel(bmx1xx_accel_t *const accel)
{
	return -ENODEV;
}

int bmx1xx_accel_set_mode(bmx1xx_accel_mode_t mode)
{
	return -ENODEV;
}

int bmx1xx_set_bandwidth(bmx1xx_bandwidth_t bw)
{
	return -ENODEV;
}

int bmx1xx_read_mag(bmx1xx_mag_t *const mag)
{
	return -ENODEV;
}

int bmx1xx_mag_set_power(bmx1xx_mag_power_t power)
{
	return -ENODEV;
}

int bmx1xx_mag_set_preset(bmx1xx_mag_preset_t preset)
{
	return -ENODEV;
}

int bmx1xx_set_command(bmx1xx_command_mode_t mode)
{
	return -ENODEV;
}

int bmx1xx_init(bmx1xx_setup_config_t cfg)
{
	return -ENODEV;
}

#endif /*(BMC150_SENSOR || BMI160_SENSOR)*/
