/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * QMSI I2C EEPROM simulation.
 *
 * This example simulates an I2C EEPROM to demonstrate how to use I2C slave API.
 * This application uses Microchip 24FC256-I/P I2C EEPROM's protocol.
 * You can use I2C example for the master I2C for reading/writing into this
 * simulated EEPROM.
 * After 6 transfers, this application will stop.
 *
 * Instruction:
 * - Flash one board (Intel(R) Quark(TM) development boards) with examples/i2c.
 * - Flash an another board with this application.
 * - connect ground of the first board to the second one.
 * - connect I2C_0 SDA of the first board to I2C_0 SDA of the second one.
 * - connect I2C_0 SCL of the first board to I2C_0 SCL of the second one.
 */

#include "clk.h"
#include "qm_common.h"
#include "qm_i2c.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"

/* EEPROM parameters. */
#define EEPROM_PAGE_SIZE (64)
#define EEPROM_ADDR_SIZE (2)
#define EEPROM_NB_PAGES (8)
#define EEPROM_INVALID_PAGE (0xFF)
#define EEPROM_ADDRESS (0x51)

/* Number of transfers before stopping. */
#define NB_I2C_TRANSFERS (6)

/*
 * Request data for I2C asynchronous operation (transfer descriptor) needs to
 * be kept alive during request processing. It is safer when it is globally
 * accessible within the file - this ensures that they will always be in
 * scope when the IRQ is triggered.
 */
static qm_i2c_transfer_t irq_combined_xfer;

static uint8_t eeprom_pages[EEPROM_NB_PAGES][EEPROM_PAGE_SIZE];
static uint8_t rx_buf[EEPROM_ADDR_SIZE];
static uint8_t addressed_page = EEPROM_INVALID_PAGE;

/* Remaining number of tranfers. */
static volatile int nb_i2c_transfers = NB_I2C_TRANSFERS;

/*
 * Prepare next transaction.
 * This function is called when we are done with current transaction, so we
 * reset some internal data and wait for a new transaction.
 */
static void prepare_for_next_transaction(void)
{
	/*
	 * Reset addressed_page which will be updated when getting the first 2
	 * bytes of next transaction.
	 */
	addressed_page = EEPROM_INVALID_PAGE;
	/*
	 * A transaction starts with reception of 2 bytes (ie: slot address).
	 */
	irq_combined_xfer.rx = rx_buf;
	irq_combined_xfer.rx_len = EEPROM_ADDR_SIZE;

	qm_i2c_slave_irq_transfer_update(QM_I2C_0, &irq_combined_xfer);
}

/*
 * Process first bytes of the transaction: extract address (ie: slot number of
 * EEPROM).
 */
static void process_address(uint32_t len)
{
	uint8_t *data = irq_combined_xfer.rx;

	QM_ASSERT(len == EEPROM_ADDR_SIZE);
	QM_ASSERT(addressed_page == EEPROM_INVALID_PAGE);

	/*
	 * Address is 2 bytes in little endian. Because this EEPROM
	 * simulation can accept only address value smaller than 8, then
	 * MSB of this address is expected to be zero.
	 */
	addressed_page = data[0];
	QM_ASSERT(data[1] == 0);
	QM_ASSERT(addressed_page < EEPROM_NB_PAGES);

	/*
	 * Update tx/rx buffer to point into the correct EEPROM slot.
	 * So if the next transaction is to transmit, then the buffer is
	 * already prepared.
	 * Note: for reception case, we could receive all bytes in one
	 * interruption, meaning address and the data, but we don't do
	 * that in order to avoid using memcpy function.
	 */
	irq_combined_xfer.tx = eeprom_pages[addressed_page];
	irq_combined_xfer.rx = eeprom_pages[addressed_page];

	irq_combined_xfer.rx_len = EEPROM_PAGE_SIZE;
	/* Notify the driver that we have updated some data.  */
	qm_i2c_slave_irq_transfer_update(QM_I2C_0, &irq_combined_xfer);
}

/* IRQ based transfer callback. */
static void eeprom_fsm_cb(void *cb_data, int rc, qm_i2c_status_t status,
			  uint32_t len)
{
	switch (status) {
	case QM_I2C_STOP_DETECTED:
		/*
		 * RX transaction done.
		 * If len is not null, that means RX done and not TX done.
		 */
		if (len != 0) {
			/*
			 * If we are here, that means we have received data
			 * which was copied to our EEPROM.
			*/
			QM_ASSERT(len == EEPROM_PAGE_SIZE);

			/* Prepare next transaction. */
			prepare_for_next_transaction();
		}
		/*
		 * Note: in case of TX transaction, preparing next transaction
		 * is not needed because it's already done in TX_EMPTY
		 * interruption.
		 */

		/* Current transfer is done, so let's update remaining number of
		 * transfers. */
		if (nb_i2c_transfers) {
			nb_i2c_transfers--;
		}

		break;

	case QM_I2C_START_DETECTED:
		break;

	case QM_I2C_RX_FULL:
		/*
		 * RX buffer is full.
		 * This interruption happens when receiving the 2 bytes of the
		 * header. (ie: slot address).
		 */
		process_address(len);
		break;

	case QM_I2C_TX_EMPTY:
		/*
		 * TX buffer is empty.
		 * The transmission is done. So let's prepare the next
		 * transaction.
		 */
		prepare_for_next_transaction();
		break;

	case QM_I2C_GEN_CALL_DETECTED:
		break;

	default:
		break;
	}
}

int main(void)
{
	qm_i2c_config_t cfg;

	QM_PUTS("Starting: I2C-slave");

	qm_irq_request(QM_IRQ_I2C_0_INT, qm_i2c_0_isr);

	/* Enable I2C 0. */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_M0_REGISTER);

#if (QUARK_D2000)
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_2); /* I2C_SCL. */
	qm_pmux_select(QM_PIN_ID_7, QM_PMUX_FN_2); /* I2C_SDA. */
#elif(QUARK_SE)
	qm_pmux_select(QM_PIN_ID_20, QM_PMUX_FN_0); /* I2C0_SCL. */
	qm_pmux_select(QM_PIN_ID_21, QM_PMUX_FN_0); /* I2C0_SDA. */
#endif

	/* Configure I2C. */
	cfg.address_mode = QM_I2C_7_BIT; /* Set 7-bit mode. */
	cfg.mode = QM_I2C_SLAVE;	 /* Set slave. */
	cfg.speed = 0;			 /* Not used on slave mode. */
	cfg.slave_addr = EEPROM_ADDRESS; /* Set slave address. */

	if (qm_i2c_set_config(QM_I2C_0, &cfg)) {
		QM_PUTS("Error: I2C config");
		return -1;
	}

	/*
	 * All transaction start by the reception of 2 bytes which specify the
	 * slot address to read/write from/to the EEPROM.
	 */
	irq_combined_xfer.rx = rx_buf;
	irq_combined_xfer.rx_len = EEPROM_ADDR_SIZE;

	/*
	 * Tx buffer is not yet needed, but if the master wants to read directly
	 * from EEPROM without providing a slot address, then let's send the
	 * first page.
	 */
	irq_combined_xfer.tx = eeprom_pages[0];
	irq_combined_xfer.tx_len = EEPROM_PAGE_SIZE;

	/*
	 * Set callback which will be called when 2 bytes of slot address will
	 * be received.
	 */
	irq_combined_xfer.callback = eeprom_fsm_cb;
	irq_combined_xfer.callback_data = NULL;

	/* Set continuous transfer, do not stop after a transfer.  */
	irq_combined_xfer.stop = false;

	if (qm_i2c_slave_irq_transfer(QM_I2C_0, &irq_combined_xfer)) {
		QM_PUTS("Error: I2C read/write\n");
		return -1;
	}

	/*
	 * Process NB_I2C_TRANSFERS full EEPROM transfers to let the I2C master
	 * example to work with this slave example.
	 * A full EEPROM transfer consists in 1 Write, followed by 1
	 * Write-Restart-Read.
	 */
	while (nb_i2c_transfers) {
	}

	qm_irq_mask(QM_IRQ_I2C_0_INT);

	QM_PUTS("Finished: I2C slave");

	return 0;
}
