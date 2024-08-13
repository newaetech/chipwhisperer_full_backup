/**
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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

#include <string.h>

#include "fw-manager_utils.h"
#include "xmodem.h"
#include "xmodem_io.h"

/* The maximum number of times XMODEM tries to send a packet / control byte */
#define MAX_RETRANSMIT (8)
/* The maximum number of consecutive RX errors XMODEM tolerates */
#define MAX_RX_ERRORS (5)

/* Custom value, not transferred via XMODEM, but used as return codes */
#define ERR (0xFF)
#define DUP (0xFE)

/* XMODEM control bytes */
#define SOH (0x01)
#define EOT (0x04)
#define ACK (0x06)
#define NAK (0x15)
#define CAN (0x18)

/* XMODEM block size */
#define PACKET_PAYLOAD_SIZE (XMODEM_BLOCK_SIZE)

/* Activate debug messages by defining DEBUG_MSG to 1 */
#define DEBUG_MSG (0)

#if DEBUG_MSG
#define printd(...) QM_PRINTF(__VA_ARGS__)
#else
#define printd(...)
#endif

/**
 * The XMODEM packet buffer.
 *
 * This buffer is used for both incoming and outgoing packets.
 */
static struct __attribute__((__packed__)) xmodem_packet {
	uint8_t soh;
	uint8_t seq_no;
	uint8_t seq_no_inv;
	uint8_t data[PACKET_PAYLOAD_SIZE];
	uint8_t crc_u8[2];
} pkt_buf;

/**
 * Send a single XMODEM packet.
 *
 * @param[in] data     The payload of the packet.
 * @param[in] data_len The length of the payload. Must be at most 128 bytes.
 *		       If less, (random) padding is automatically added.
 * @param[in] pkt_no   The desired packet sequence number.
 *
 * @return Resulting status code.
 * @retval 0 Success (only possible retval for now).
 */
static int xmodem_send_pkt(const uint8_t *data, size_t data_len, uint8_t pkt_no)
{
	size_t i;
	uint8_t *buf;
	uint16_t crc;

	printd("xmodem_send_pkt(): pkt_no: %d\n", pkt_no);
	pkt_buf.soh = SOH;
	memcpy(pkt_buf.data, data, data_len);
	crc = fm_crc16_ccitt(pkt_buf.data, PACKET_PAYLOAD_SIZE);
	pkt_buf.crc_u8[0] = (crc >> 8) & 0xFF;
	pkt_buf.crc_u8[1] = crc & 0xFF;
	pkt_buf.seq_no = pkt_no;
	pkt_buf.seq_no_inv = ~pkt_no;
	buf = (uint8_t *)&pkt_buf;
	/* Send the packet */
	for (i = 0; i < sizeof(pkt_buf); i++) {
		xmodem_io_putc(&buf[i]);
	}

	return 0;
}

/**
 * Try to send an XMODEM packet for MAX_RETRANSMIT times.
 *
 * This function sends an XMODEM packet and checks if an ACK is received. If no
 * ACK is received, the packet is retransmitted. This is done until
 * 'MAX_RETRANSMIT' is exceeded.
 *
 * @param[in] data     The payload of the packet.
 * @param[in] data_len The length of the payload. Must be at most 128 bytes.
 *		       If less, (random) padding is automatically added.
 * @param[in] pkt_no   The packet sequence number.
 *
 * @return Exit status.
 * @retval 0  Success, the packet has been transmitted and an ACK received.
 * @retval -1 Error, retransmit count exceeded.
 */
static int xmodem_send_pkt_with_retry(const uint8_t *data, size_t data_len,
				      uint8_t pkt_no)
{
	uint8_t retransmit = MAX_RETRANSMIT;
	uint8_t rsp;

	printd("xmodem_send_pkt_with_retry(): pkt_no: %d\n", pkt_no);
	while (retransmit--) {
		xmodem_send_pkt(data, data_len, pkt_no);
		rsp = ERR;
		xmodem_io_getc(&rsp);
		if (rsp == ACK) {
			printd("xmodem_send_pkt_with_retry(): done\n");
			return 0;
		}
		printd("xmodem_send_pkt_with_retry(): failure (%d)\n",
		       retransmit);
	}

	return -1;
}

/**
 * Try to send a byte for MAX_RETRANSMIT times.
 *
 * This function sends a byte (typically an XMODEM control byte) and checks if
 * an ACK is received. If no ACK is received, the byte is retransmitted. This is
 * done until 'MAX_RETRANSMIT' is exceeded.
 *
 * @param[in] cmd The byte to send.
 *
 * @return Exit status.
 * @retval 0  Success, the byte has been transmitted and an ACK received.
 * @retval -1 Error, retransmit count exceeded.
 */
static int xmodem_send_byte_with_retry(uint8_t cmd)
{
	uint8_t retransmit = MAX_RETRANSMIT;
	uint8_t rsp;

	while (retransmit--) {
		xmodem_io_putc(&cmd);
		rsp = ERR;
		xmodem_io_getc(&rsp);
		if (rsp == ACK) {
			return 0;
		}
	}

	return -1;
}

/*
 * Receive an XMODEM packet.
 *
 * @param[in] exp_seq_no The expected sequence number of the packet to be
 * 			 received.
 * @param[in] data       The buffer where to store the packet payload.
 * @param[in] len        The size of the buffer.
 *
 * @return Status code.
 * @retval SOH The packet has been successful received.
 * @retval DUP The received packet is a duplicate of the previous one (based on
 *             the expected sequence number); nothing has not been written to
 *             the data buffer.
 * @retval ERR An error has occurred (either a timeout or the reception of
 * 	       invalid / corrupted data), but the XMODEM session is not
 *	       compromised.
 * @retval CAN An unrecoverable error has occurred (either the sender and
 *	       receiver have lost sync or the passed buffer is too small).
 * @retval EOT The sender notified the end of transmission (i.e., there are no
 *	       more packets to receive).
 */
static int xmodem_read_pkt(uint8_t exp_seq_no, uint8_t *data, size_t len)
{
	uint8_t cmd;
	uint16_t crc_recv; /* received CRC */
	uint16_t crc_comp; /* computed CRC */
	uint8_t *buf;
	uint8_t *buf_end;

	cmd = ERR;

	if (xmodem_io_getc(&cmd) < 0) {
		return ERR;
	}

	switch (cmd) {
	case SOH:
		printd("xmodem_read_pkt(): cmd: SOH\n");
		break;
	case EOT:
		printd("xmodem_read_pkt(): cmd: EOT\n");
		return EOT;
	default:
		/*
		 * Unexpected cmd case.
		 *
		 * This includes the case of a corrupted/lost SOH; therefore,
		 * we should check if other bytes are arriving and, in case,
		 * discard them before returning (and replying with a NAK).
		 *
		 * This is the purpose of the following loop.
		 */
		printd("xmodem_read_pkt(): cmd: unexpected ctrl byte (0x%x)\n",
		       cmd);
		/* Wait until the sender stops sending bytes */
		while (xmodem_io_getc(&cmd) >= 0) {
			/*
			 * Loop until we timeout
			 *
			 * NOTE: a special small timeout value should actually
			 * be used; however, we do not do that in the target
			 * for footprint minimization purposes.
			 *
			 * IMPORTANT: This choice forces the target to use a
			 * timeout value smaller than the one used by the host,
			 * otherwise we may end up in a communication loop if
			 * for some reason both the target and the host enters
			 * reception mode (because both will send and discard
			 * NAKs).
			 */
		}
		return ERR;
	}

	/* Read the rest of the packet (seq_no, ~seq_no, data, and CRC) */
	/* Start from seq_no, since we have already read soh */
	buf = (uint8_t *)&pkt_buf.seq_no;
	/* Compute end of buffer */
	buf_end = (uint8_t *)(&pkt_buf + 1);
	while (buf < buf_end) {
		if (xmodem_io_getc(buf++) < 0) {
			printd("xmodem_read_pkt(): pkt: ERROR: timeout\n");
			printd("----\n");
			/* This is a timeout error */
			return ERR;
		}
	}

	/* Check sequence number fields and CRC */
	crc_comp = fm_crc16_ccitt(pkt_buf.data, PACKET_PAYLOAD_SIZE);
	crc_recv = (pkt_buf.crc_u8[0] << 8) | pkt_buf.crc_u8[1];
	/*
	 * NOTE: Using 'a == (~a &FF)' instead of 'a == ~a', since the latter
	 * leads to a compilation error due to the following GCC bug:
	 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=38341
	 */
	if ((pkt_buf.seq_no != (~pkt_buf.seq_no_inv & 0xFF)) ||
	    (crc_recv != crc_comp)) {
		printd("xmodem_read_pkt(): pkt: ERROR: corrupted packet\n");
		return ERR;
	}
	/* Check packet numbers. */
	if ((pkt_buf.seq_no == (exp_seq_no - 1))) {
		printd("xmodem_read_pkt(): pkt: WARNING duplicated packet\n");
		return DUP;
	}
	if (pkt_buf.seq_no != exp_seq_no) {
		printd("xmodem_read_pkt(): pkt: ERROR: wrong seq number\n");
		return CAN;
	}

	/*
	 * If we reach this point, the packet is the expected one and it has
	 * been correctly received: now we can check that the buffer is big
	 * enough to hold the payload (NOTE: this check should not be
	 * anticipated, otherwise we risk to return a CAN in case of a simple
	 * EOT from the sender).
	 */
	if (len < sizeof(pkt_buf.data)) {
		printd("xmodem_read_pkt(): pkt: "
		       "ERROR: user buffer out of space\n");
		return CAN;
	}
	memcpy(data, pkt_buf.data, sizeof(pkt_buf.data));
	printd("xmodem_read_pkt(): pkt: received correctly\n");

	return SOH;
}

int xmodem_receive_package(uint8_t *buf, size_t buf_len)
{
	int status;
	uint8_t exp_seq_no;
	uint8_t nak;
	uint8_t cmd;
	int retv;
	int data_cnt;
	int err_cnt;

	/* XMODEM sequence number starts from 1 */
	exp_seq_no = 1;
	/* Reception is started by sending a 'C' */
	cmd = 'C';
	/*
	 * Until the first packet is received (i.e., the XMODEM transfer is
	 * started), we must nak with a 'C' instead of a regular NAK
	 * [this is an XMODEM-CRC peculiarity]
	 */
	nak = 'C';

	err_cnt = 0;
	data_cnt = 0;
	retv = -1;
	while (err_cnt < MAX_RX_ERRORS) {
		printd("xmodem_receive(): sending cmd: %x\n", cmd);
		xmodem_io_putc(&cmd);
		status = xmodem_read_pkt(exp_seq_no, &buf[data_cnt], buf_len);
		switch (status) {
		case SOH:
			nak = NAK;
			data_cnt += sizeof(pkt_buf.data);
			buf_len -= sizeof(pkt_buf.data);
			exp_seq_no++;
			err_cnt = 0;
		/* no 'break' on purpose */
		case DUP:
			/*
			 * We must acknowledge duplicated packets to have the
			 * sender transmit the next packet
			 */
			cmd = ACK;
			break;
		case EOT:
			cmd = ACK;
			retv = data_cnt;
			goto exit;
		case CAN:
			cmd = CAN;
			goto exit;
		default:
			err_cnt++;
			cmd = nak;
		}
	}
exit:
	if (retv < 0) {
		printd("xmodem_receive(): ERROR: reception failed\n");
	}
	xmodem_io_putc(&cmd);

	return retv;
}

int xmodem_transmit_package(uint8_t *data, size_t len)
{
	int mlen;
	uint8_t retransmit;
	uint8_t rsp;
	uint8_t pkt_no;

	retransmit = MAX_RETRANSMIT;

	while (retransmit--) {
		printd("xmodem_transmit(): waiting for 'C' (%d)\n", retransmit);
		rsp = ERR;
		xmodem_io_getc(&rsp);
		if (rsp == 'C') {
			goto start_transmit;
		}
	}

	return -1;

start_transmit:
	printd("xmodem_transmit(): starting transmission\n");
	pkt_no = 1;
	/* Send packets as long data */
	while (len) {
		mlen =
		    (len >= sizeof(pkt_buf.data)) ? sizeof(pkt_buf.data) : len;
		if (xmodem_send_pkt_with_retry(data, mlen, pkt_no) < 0) {
			return -1;
		}
		data += mlen;
		len -= mlen;
		pkt_no++;
	}
	if (xmodem_send_byte_with_retry(EOT) < 0) {
		return -1;
	}

	return 0;
}
