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
 * Mailbox
 *
 * This example app is exclusive to the Intel(R) Quark(TM) SE SoC. This app
 * shows the use of mailboxes with interrupts. The sensor subsystem echoes back
 * all messages sent by the x86 processor.
 *
 * This example must be compiled for x86 and sensor subsystem targets, and both
 * binary objects must be loaded on the target.
 */

#if (!QM_SENSOR)
#include <x86intrin.h>
#endif
#include "clk.h"
#include "get_ticks.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_mailbox.h"

#define SYS_TICKS_PER_S_32MHZ (SYS_TICKS_PER_US_32MHZ * 1000000)
#define NUM_TRANSFERS (5)

/*
 * Disable Sensor printf output.
 * UART for printf is shared between x86 processor and sensor subsystem. To
 * avoid conflicts, initialisation of UART by sensor subsystem is disabled in
 * the Makefile and printf and puts calls are disabled hereafter.
 */
#define DISABLE_SENSOR_PRINTF 1
#if (DISABLE_SENSOR_PRINTF && QM_SENSOR)
#undef QM_PUTS
#undef QM_PRINTF
#define QM_PUTS(...)
#define QM_PRINTF(...)
#endif

volatile bool cb_fired = false;
volatile int transfer_count = 0;
qm_mbox_msg_t tx_data, rx_data;

qm_mbox_ch_t mbox_pair[2] = {QM_MBOX_CH_0, QM_MBOX_CH_1};

qm_mbox_config_t mbox_rx_config;

/* Mailbox callback on available data. */
void mailbox_example_cb(void *callback_data)
{
	cb_fired = true;
}

/* Send a message. */
int send_data(qm_mbox_ch_t mbox_tx, uint32_t value)
{
#if (QM_SENSOR)
	/* For sensor, copy received data. */
	tx_data = rx_data;
#else
	/* On x86 core , write an initial value in transfered data registers. */
	tx_data.ctrl = value;
	tx_data.data[0] = value + 1;
	tx_data.data[1] = value + 2;
	tx_data.data[2] = value + 3;
	tx_data.data[3] = value + 4;
#endif /* QM_SENSOR */

	/* Actually send message here. */
	if (0 != qm_mbox_ch_write(mbox_tx, &tx_data)) {
		QM_PRINTF("Error: mbox %d write\n", mbox_tx);
		return 1;
	}

	return 0;
}

/* Check received data and send a new message. */
int check_rx_data_and_send(qm_mbox_ch_t mbox_rx, qm_mbox_ch_t mbox_tx)
{
	/* Read the mailbox. */
	if (0 != qm_mbox_ch_read(mbox_rx, &rx_data)) {
		QM_PRINTF("Error: Reading failed on mbox=%d, "
			  "ctrl=%d.\n",
			  mbox_rx, (int)rx_data.ctrl);
		return 1;
	} else {
#if (!QM_SENSOR)
		/*
		 * On x86 core, compare data sent and received.
		 * They should be the same as sensor subsystem
		 * echoes back data.
		 */
		if (tx_data.ctrl == rx_data.ctrl &&
		    tx_data.data[0] == rx_data.data[0] &&
		    tx_data.data[1] == rx_data.data[1] &&
		    tx_data.data[2] == rx_data.data[2] &&
		    tx_data.data[3] == rx_data.data[3]) {
			QM_PRINTF("Message %d OK !\n", transfer_count);
		}
#endif
		/* Send a new message. */
		if (transfer_count < NUM_TRANSFERS) {
			++(transfer_count);

			if (0 != send_data(mbox_tx, transfer_count)) {
				return 1;
			}
		}
	}

	return 0;
}

int main(void)
{
#if (QM_SENSOR)
	uint32_t t_init;
#else
	uint64_t t_init;
#endif
	int retcode = 0;
	qm_mbox_ch_t mbox_tx, mbox_rx;

	QM_PUTS("Starting: Mailbox");

#if (QM_SENSOR)
	/* Choose mailbox channels for RX and TX. */
	mbox_rx = mbox_pair[0];
	mbox_tx = mbox_pair[1];

	mbox_rx_config.dest = QM_MBOX_TO_SS;

#else
	/*
	 * Choose mailbox channels for RX and TX.
	 * If one mailbox is chosen as a RX or TX on sensor, they must
	 * be configured respectively as TX or RX on x86 core.
	 */
	mbox_rx = mbox_pair[1];
	mbox_tx = mbox_pair[0];
	QM_PRINTF("Mailbox: TX channel %d -> RX channel %d\n", mbox_tx,
		  mbox_rx);

	mbox_rx_config.dest = QM_MBOX_TO_LMT;

#endif

	mbox_rx_config.mode = QM_MBOX_INTERRUPT_MODE;
	mbox_rx_config.callback = mailbox_example_cb;
	mbox_rx_config.callback_data = NULL;

	/* Register the interrupt handler. */
	qm_irq_request(QM_IRQ_MAILBOX_0_INT, qm_mailbox_0_isr);

	/* Configure RX channel. */
	qm_mbox_ch_set_config(mbox_rx, &mbox_rx_config);

#if (!QM_SENSOR)
	/* x86 core sends data first. */
	if (0 != send_data(mbox_tx, transfer_count)) {
		retcode = 1;
	}

#endif
	/* Get system tick time before starting the loop. */
	t_init = get_ticks();

	/*
	 * Wait for 1 second to be elapsed or NUM_TRANSFERS transfers to
	 * succeed.
	 */
	while (((get_ticks() - t_init) < SYS_TICKS_PER_S_32MHZ) &&
	       (transfer_count < NUM_TRANSFERS)) {

		if (cb_fired) {
			cb_fired = false;

			if (check_rx_data_and_send(mbox_rx, mbox_tx)) {
				retcode = 1;
			}
		}
	}

	QM_PUTS("Finished: Mailbox");

	return retcode;
}
