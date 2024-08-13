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
 * Sensor Subsystem (SS) Inter-Integrated Circuit (I2C)
 *
 * This example uses a Microchip 24FC256-I/P I2C EEPROM to demonstrate
 * both polled and interrupt based master I2C transfers on I2C_SS_0.
 *
 * EEPROM pin 2, 3, 4 and 7 are connected to ground.
 * EEPROM pin 1, 8 is connected to 3.3V.
 * EEPROM pin 5 is connected to I2C_0 SDA pin with pull-up resistor.
 * EEPROM pin 6 is connected to I2C_0 SCL pin with pull-up resistor.
 */

#include "clk.h"
#include "qm_ss_i2c.h"
#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_uart.h"
#include "ss_clk.h"

#define EEPROM_PAGE_SIZE_BYTES (64)
#define EEPROM_ADDR_SIZE_BYTES (2)
#define EEPROM_SLAVE_ADDR (0x51)
#define EEPROM_DATA_ADDR (0x0000)
#define EEPROM_WRITE_WAIT_TIME_US (50000)
#define EEPROM_ADDR_FIRST_PAGE_LO (0)
#define EEPROM_ADDR_FIRST_PAGE_HI (0)
#define NULL_TERMINATOR (1)

/*
 * Request data for I2C asynchronous operation (transfer descriptor) needs to
 * be kept alive during request processing. It is safer when it is globally
 * accessible within the file - this ensures that they will always be in
 * scope when the IRQ is triggered.
 */
static qm_ss_i2c_transfer_t async_irq_write_xfer;
static qm_ss_i2c_transfer_t async_irq_read_xfer;

volatile bool ss_i2c_0_irq_complete = false;
volatile bool ss_i2c_0_irq_rc_error = false;
static uint8_t id_write = 0;
static uint8_t id_read = 1;

/* Reserve two bytes for EEPROM address. */
uint8_t eeprom_pio_write_data[EEPROM_ADDR_SIZE_BYTES + EEPROM_PAGE_SIZE_BYTES] =
    "  ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKL";
uint8_t eeprom_irq_write_data[EEPROM_ADDR_SIZE_BYTES + EEPROM_PAGE_SIZE_BYTES] =
    "  1234567890123456789012345678901234567890123456789012345678901234";
uint8_t eeprom_read_addr[EEPROM_ADDR_SIZE_BYTES] = {EEPROM_ADDR_FIRST_PAGE_LO,
						    EEPROM_ADDR_FIRST_PAGE_HI};
uint8_t eeprom_read_buf[EEPROM_PAGE_SIZE_BYTES + NULL_TERMINATOR] = {0};

/* Helper function to compare two buffers of size EEPROM_PAGE_SIZE_BYTES. */
void eeprom_compare_page(uint8_t *write_data, uint8_t *read_data)
{
	uint32_t i;
	for (i = 0; i < EEPROM_PAGE_SIZE_BYTES; i++) {
		/*
		 * Write_data contains the address in first 2 bytes, so offset
		 * comparison by 2 bytes.
		 */
		if (write_data[i + EEPROM_ADDR_SIZE_BYTES] != read_data[i]) {
			QM_PUTS("Error: Data compare failed");
			return;
		}
	}

	QM_PRINTF("Data compare OK:\n%s \n", (const char *)read_data);
}

/* IRQ based transfer callback. */
static void i2c_0_irq_callback(void *data, int rc, qm_ss_i2c_status_t status,
			       uint32_t len)
{
	if (rc != 0) {
		ss_i2c_0_irq_rc_error = true;
	}

	ss_i2c_0_irq_complete = true;
}

/* Writes a page to the EEPROM using PIO SS I2C transfer. */
void ss_i2c_pio_write_example()
{
	qm_ss_i2c_status_t status;

	QM_PUTS("PIO write");

	/* Write EEPROM page address then the data. */
	if (qm_ss_i2c_master_write(
		QM_SS_I2C_0, EEPROM_SLAVE_ADDR, eeprom_pio_write_data,
		sizeof(eeprom_pio_write_data), true, &status)) {
		QM_PUTS("Error: PIO write failed");
	} else {
		QM_PUTS("SS I2C PIO TX Transfer complete");
	}

	clk_sys_udelay(EEPROM_WRITE_WAIT_TIME_US);
}

/* Reads a page from the EEPROM using combined PIO SS I2C transfer. */
void ss_i2c_pio_combined_transaction_example()
{
	qm_ss_i2c_status_t status;

	QM_PUTS("PIO combined write + read");

	/* Write EEPROM page address then the data. */
	if (qm_ss_i2c_master_write(QM_SS_I2C_0, EEPROM_SLAVE_ADDR,
				   eeprom_read_addr, EEPROM_ADDR_SIZE_BYTES,
				   false, &status)) {
		QM_PUTS("Error: PIO write failed");
	} else {

		QM_PUTS("SS I2C PIO TX Transfer complete");
	}

	/* Read EEPROM page. */
	if (qm_ss_i2c_master_read(QM_SS_I2C_0, EEPROM_SLAVE_ADDR,
				  eeprom_read_buf, EEPROM_PAGE_SIZE_BYTES, true,
				  &status)) {
		QM_PUTS("Error: PIO read failed");
	} else {
		QM_PUTS("SS I2C PIO RX Transfer complete");
	}
}

/* Writes a page to the EEPROM using IRQ SS I2C transfer. */
void ss_i2c_irq_write_example()
{
	QM_PUTS("IRQ write");

	async_irq_write_xfer.tx = eeprom_irq_write_data;
	async_irq_write_xfer.tx_len =
	    EEPROM_PAGE_SIZE_BYTES + EEPROM_ADDR_SIZE_BYTES;
	async_irq_write_xfer.callback = i2c_0_irq_callback;
	async_irq_write_xfer.callback_data = &id_write;
	async_irq_write_xfer.rx = NULL;
	async_irq_write_xfer.rx_len = 0;
	async_irq_write_xfer.stop = true;

	if (qm_ss_i2c_master_irq_transfer(QM_SS_I2C_0, &async_irq_write_xfer,
					  EEPROM_SLAVE_ADDR)) {
		QM_PUTS("Error: IRQ write failed");
	}

	while (!ss_i2c_0_irq_complete)
		;

	if (ss_i2c_0_irq_rc_error) {
		QM_PUTS("Error: SS I2C IRQ Transfer failed");
	} else {
		QM_PUTS("SS I2C IRQ Transfer complete");
	}

	clk_sys_udelay(EEPROM_WRITE_WAIT_TIME_US);
}

/* Reads a page from the EEPROM using combined IRQ SS I2C transfer. */
void ss_i2c_irq_combined_transaction_example()
{
	QM_PUTS("IRQ combined write + read");

	async_irq_read_xfer.tx = eeprom_read_addr;
	async_irq_read_xfer.tx_len = EEPROM_ADDR_SIZE_BYTES;
	async_irq_read_xfer.callback = i2c_0_irq_callback;
	async_irq_read_xfer.callback_data = &id_read;
	async_irq_read_xfer.rx = eeprom_read_buf;
	async_irq_read_xfer.rx_len = EEPROM_PAGE_SIZE_BYTES;
	async_irq_read_xfer.stop = true;

	ss_i2c_0_irq_complete = false;
	ss_i2c_0_irq_rc_error = false;

	if (qm_ss_i2c_master_irq_transfer(QM_I2C_0, &async_irq_read_xfer,
					  EEPROM_SLAVE_ADDR)) {
		QM_PUTS("Error: IRQ read failed");
	}

	while (!ss_i2c_0_irq_complete)
		;

	if (ss_i2c_0_irq_rc_error) {
		QM_PUTS("Error: SS I2C IRQ Transfer failed");
	} else {
		QM_PUTS("SS I2C IRQ Transfer complete");
	}
}

int main(void)
{
	qm_ss_i2c_config_t cfg;

	QM_PUTS("Starting: SS I2C");

	/*  Enable I2C 0. */
	ss_clk_i2c_enable(QM_SS_I2C_0);

	qm_ss_irq_request(QM_SS_IRQ_I2C_0_ERROR_INT, qm_ss_i2c_0_isr);
	qm_ss_irq_request(QM_SS_IRQ_I2C_0_RX_AVAIL_INT, qm_ss_i2c_0_isr);
	qm_ss_irq_request(QM_SS_IRQ_I2C_0_TX_REQ_INT, qm_ss_i2c_0_isr);
	qm_ss_irq_request(QM_SS_IRQ_I2C_0_STOP_DET_INT, qm_ss_i2c_0_isr);

	/* Configure I2C. */
	cfg.address_mode = QM_SS_I2C_7_BIT;
	cfg.speed = QM_SS_I2C_SPEED_STD;

	if (qm_ss_i2c_set_config(QM_SS_I2C_0, &cfg)) {
		QM_PUTS("Error: I2C_SS_0 config failed");
	}

	/* Add eeprom data address to the beginning of each message. */
	eeprom_pio_write_data[0] = EEPROM_ADDR_FIRST_PAGE_LO;
	eeprom_pio_write_data[1] = EEPROM_ADDR_FIRST_PAGE_HI;
	eeprom_irq_write_data[0] = EEPROM_ADDR_FIRST_PAGE_LO;
	eeprom_irq_write_data[1] = EEPROM_ADDR_FIRST_PAGE_HI;

	/* PIO example. */
	ss_i2c_pio_write_example();
	ss_i2c_pio_combined_transaction_example();
	eeprom_compare_page(eeprom_pio_write_data, eeprom_read_buf);

	/* IRQ example. */
	ss_i2c_irq_write_example();
	ss_i2c_irq_combined_transaction_example();
	eeprom_compare_page(eeprom_irq_write_data, eeprom_read_buf);

	QM_PUTS("Finished: SS I2C");

	return 0;
}
