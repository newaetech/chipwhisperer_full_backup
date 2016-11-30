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

/* Select the I2C instance. */
#if (QUARK_SE)
qm_i2c_t g_i2c_controller_instance = QM_I2C_1;
#elif(QUARK_D2000)
qm_i2c_t g_i2c_controller_instance = QM_I2C_0;
#endif

int bus_write_data_command(uint8_t *buf_ptr, uint32_t byes_to_write)
{
	bool stop = false;
	qm_i2c_status_t i2c_status = QM_I2C_IDLE;

	QM_CHECK((NULL != buf_ptr), -EINVAL);
	QM_CHECK((HDC1000_CMD_REG_WIDTH == byes_to_write), -EINVAL);

	return qm_i2c_master_write(g_i2c_controller_instance,
				   HDC1000_DEVICE_ADDR, buf_ptr, byes_to_write,
				   stop, &i2c_status);
}

int bus_read_data(uint8_t *buf_ptr, uint32_t bytes_to_read)
{

	bool stop = true;
	int retry = MAX_NUM_RETRIES, status;
	qm_i2c_status_t i2c_status;

	QM_CHECK((NULL != buf_ptr), -EINVAL);
	QM_CHECK((0 != bytes_to_read), -EINVAL);

	do {
		status = qm_i2c_master_read(g_i2c_controller_instance,
					    HDC1000_DEVICE_ADDR, buf_ptr,
					    bytes_to_read, stop, &i2c_status);
		if (status) {
			--retry;
			clk_sys_udelay(RETRY_INTERVAL);
		} else {
			break;
		}
	} while (retry);

	return status;
}
