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
 * Memory to Memory Direct Memory Access (DMA) Transfer
 *
 * This app demonstrates the basic functionality of the QMSI DMA API by doing a
 * memory to memory copy.
 */

#include <string.h>
#if (!QM_SENSOR)
#include <x86intrin.h>
#endif
#include "clk.h"
#include "qm_dma.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "get_ticks.h"

#define SYS_TICKS_PER_S_32MHZ (SYS_TICKS_PER_US_32MHZ * 1000000)
#define NUM_TRANSFERS (5)
#define RX_BUFF_SIZE (100)
#define MULTIBLOCK_NUM_BUFFERS (2)
#define MULTIBLOCK_NUM_LLI_PER_BUFFER (2)

typedef struct {
	qm_dma_t controller_id;		/* Controller ID. */
	qm_dma_channel_id_t channel_id; /* Channel ID. */
} dma_channel_desc_t;

/* Data to be copied. */
static char tx_data[] = "The DMA example application shows how to initiate a "
			"memory to memory transfer.";
static char tx_data_multiblock[] =
    "[<---- DMA block A --->][<---- DMA block B --->]"
    "[<---- DMA block C --->][<---- DMA block D --->]";

/* Copy destinations. */
static char rx_data[NUM_TRANSFERS][RX_BUFF_SIZE] = {{0}};

volatile bool irq_fired = false;
volatile uint32_t irq_len;
volatile int irq_error_code;
volatile int transfer_count = 0;

/* DMA transfer callback. */
static void transfer_callback(void *callback_context, uint32_t len,
			      int error_code)
{
	irq_len = len;
	irq_error_code = error_code;
	irq_fired = true;
}

/* Start a DMA transfer. */
static void start_transfer(dma_channel_desc_t *p_chan_desc, uint32_t *src,
			   uint32_t *dst, uint32_t size)
{
	int return_code;
	qm_dma_transfer_t dma_trans;

	dma_trans.block_size = size;
	dma_trans.source_address = src;
	dma_trans.destination_address = dst;

	return_code = qm_dma_transfer_mem_to_mem(
	    p_chan_desc->controller_id, p_chan_desc->channel_id, &dma_trans);
	if (return_code) {
		QM_PUTS("Error: qm_dma_mem_to_mem_transfer");
	}
}

/* Check last error code and start a new transfer. */
void start_another_transfer(dma_channel_desc_t *p_chan_desc)
{
	/*
	 * Check last error code. If last transfer ended with no
	 * error, another transfer can be started.
	 */
	if (irq_error_code) {
		QM_PRINTF("Error: Transfer with error Code: %u\n",
			  irq_error_code);
	} else {
		QM_PRINTF("Transfer Loop %d Complete with Data Length: %u\n",
			  transfer_count, irq_len);

		transfer_count++;

		/* Start a new transfer. */
		if (transfer_count < NUM_TRANSFERS) {
			start_transfer(p_chan_desc, (uint32_t *)tx_data,
				       (uint32_t *)rx_data[transfer_count],
				       strlen(tx_data));
		}
	}
}

/* Do NUM_TRANSFERS DMA transfers. */
static void do_transfer(dma_channel_desc_t *p_chan_desc)
{
#if (QM_SENSOR)
	uint32_t t_init;
#else
	uint64_t t_init;
#endif
	int return_code;

	QM_PUTS("Starting the transfer and waiting for 1 second.");

	/* Get system tick time. */
	t_init = get_ticks();

	/* Start first transfer. */
	start_transfer(p_chan_desc, (uint32_t *)tx_data,
		       (uint32_t *)rx_data[transfer_count], strlen(tx_data));

	/*
	 * Loop until 1 second has elapsed from start or
	 * NUM_TRANSFERS transfers are finished.
	 */
	while (((get_ticks() - t_init) < SYS_TICKS_PER_S_32MHZ) &&
	       (transfer_count < NUM_TRANSFERS)) {

		if (irq_fired) {
			irq_fired = false;

			start_another_transfer(p_chan_desc);
		}
	}

	/* If the transfer has not finished then stop the channel. */
	if (transfer_count < NUM_TRANSFERS) {
		return_code = qm_dma_transfer_terminate(
		    p_chan_desc->controller_id, p_chan_desc->channel_id);
		if (return_code) {
			QM_PUTS("ERROR: qm_dma_transfer_stop");
		}
	}
}

static void do_transfer_multi(dma_channel_desc_t *p_chan_desc)
{
	int return_code;
	qm_dma_multi_transfer_t multi_transfer = {0};

	/*
	 * We own the memory where the driver will set the linked lists. 2 LLIs
	 * are needed for each DMA transfer configuration call.
	 */
	qm_dma_linked_list_item_t
	    lli_buf[MULTIBLOCK_NUM_BUFFERS * MULTIBLOCK_NUM_LLI_PER_BUFFER];

	/* Clear RX buffer. */
	for (unsigned int i = 0; i < RX_BUFF_SIZE; i++) {
		rx_data[0][i] = '.';
	}

	/*
	 * Linked list multiblock transfer with 4 blocks, using 2 calls to the
	 * DMA transfer configuration function.
	 *
	 *  tx_data:
	 *  <------+ TX Buffer 2 +------><-------+ TX Buffer 1 +------>
	 *  +---------------------------------------------------------+
	 *  |   Block A   |   Block B   |   Block C   |   Block D     |
	 *  +---------------------------------------------------------+
	 *
	 *  RX Buffer:
	 *  +--------------------------+     +------------------------------+
	 *  |  Block C   |   Block D   |.....|  Block A     |   Block B     |
	 *  +--------------------------+     +------------------------------+
	 */

	/* Add LLIs for second half of tx_data (blocks C and D). */
	multi_transfer.block_size =
	    strlen(tx_data_multiblock) /
	    (MULTIBLOCK_NUM_BUFFERS * MULTIBLOCK_NUM_LLI_PER_BUFFER);
	multi_transfer.num_blocks = MULTIBLOCK_NUM_LLI_PER_BUFFER;
	multi_transfer.source_address =
	    (uint32_t *)&tx_data_multiblock[strlen(tx_data_multiblock) /
					    MULTIBLOCK_NUM_BUFFERS];
	multi_transfer.destination_address = (uint32_t *)&rx_data[0][0];
	multi_transfer.linked_list_first = &lli_buf[0];
	return_code = qm_dma_multi_transfer_set_config(
	    p_chan_desc->controller_id, p_chan_desc->channel_id,
	    &multi_transfer);
	if (return_code) {
		QM_PRINTF("ERROR: qm_dma_mem_to_mem_transfer\n");
	}

	/* Add LLIs for first half of tx_data (blocks A and B). */
	multi_transfer.source_address = (uint32_t *)&tx_data_multiblock[0];
	multi_transfer.destination_address =
	    (uint32_t *)&rx_data[0][RX_BUFF_SIZE - (strlen(tx_data_multiblock) /
						    MULTIBLOCK_NUM_BUFFERS)];
	multi_transfer.linked_list_first =
	    &lli_buf[MULTIBLOCK_NUM_LLI_PER_BUFFER];
	return_code = qm_dma_multi_transfer_set_config(
	    p_chan_desc->controller_id, p_chan_desc->channel_id,
	    &multi_transfer);
	if (return_code) {
		QM_PRINTF("ERROR: qm_dma_mem_to_mem_transfer\n");
	}

	irq_fired = false;
	return_code = qm_dma_transfer_start(p_chan_desc->controller_id,
					    p_chan_desc->channel_id);
	if (return_code) {
		QM_PRINTF("ERROR: qm_dma_transfer_start\n");
	}

	/* Wait for completion callback. */
	while (!irq_fired)
		;
}

int main(void)
{
	qm_dma_channel_config_t cfg = {0};
	static dma_channel_desc_t chan_desc;
	int return_code, i;

	QM_PUTS("Starting: DMA");

	/*
	 * Request the required interrupts. Depending on the channel used a
	 * different isr is set:
	 *     qm_irq_request(QM_IRQ_DMA_0_INT_<channel>,
	 * qm_dma_0_isr_<channel>)
	 */
	qm_irq_request(QM_IRQ_DMA_0_INT_0, qm_dma_0_isr_0);
	qm_irq_request(QM_IRQ_DMA_0_ERROR_INT, qm_dma_0_error_isr);

	/* Set the controller and channel IDs. */
	chan_desc.controller_id = QM_DMA_0;
	chan_desc.channel_id = QM_DMA_CHANNEL_0;

	return_code = qm_dma_init(chan_desc.controller_id);
	if (return_code) {
		QM_PUTS("ERROR: qm_dma_init");
	}

	/* Configure DMA channel. */
	cfg.channel_direction = QM_DMA_MEMORY_TO_MEMORY;
	cfg.source_transfer_width = QM_DMA_TRANS_WIDTH_8;
	cfg.destination_transfer_width = QM_DMA_TRANS_WIDTH_8;
	cfg.source_burst_length = QM_DMA_BURST_TRANS_LENGTH_1;
	cfg.destination_burst_length = QM_DMA_BURST_TRANS_LENGTH_1;
	cfg.client_callback = transfer_callback;
	cfg.transfer_type = QM_DMA_TYPE_SINGLE;

	/*
	 * Set the context as the channel descriptor. This will allow the
	 * descriptor to be available in the callback.
	 * The callback context is not actually used in this app. It is
	 * provided as an example.
	 */
	cfg.callback_context = (void *)&chan_desc;

	return_code = qm_dma_channel_set_config(chan_desc.controller_id,
						chan_desc.channel_id, &cfg);
	if (return_code) {
		QM_PUTS("ERROR: qm_dma_channel_set_config");
	}

	/* Do the transfers. */
	do_transfer(&chan_desc);

	QM_PUTS("Each RX buffer should contain the full TX buffer string.");
	QM_PRINTF("TX data: %s\n", tx_data);

	/* Print copied data. */
	for (i = 0; i < NUM_TRANSFERS; i++) {
		QM_PRINTF("RX data Loop %d: %s\n", i, rx_data[i]);
	}

	/* Configure DMA channel for multiblock usage. */
	cfg.transfer_type = QM_DMA_TYPE_MULTI_LL;
	return_code = qm_dma_channel_set_config(chan_desc.controller_id,
						chan_desc.channel_id, &cfg);
	if (return_code) {
		QM_PUTS("ERROR: qm_dma_channel_set_config");
	}

	/* Do the multiblock transfer. */
	do_transfer_multi(&chan_desc);

	QM_PRINTF("RX data (multiblock transfer):\n");
	rx_data[1][0] = '\0';
	printf("%s\n", (char *)&rx_data[0][0]);

	QM_PUTS("Finished: DMA");

	return 0;
}
