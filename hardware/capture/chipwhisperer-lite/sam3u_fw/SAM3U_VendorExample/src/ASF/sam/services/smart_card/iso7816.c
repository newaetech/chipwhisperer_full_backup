/**
 * \file
 *
 * \brief Smart Card Standard ISO7816 driver.
 *
 * Copyright (c) 2011-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "iso7816.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup sam_services_smart_card_group Smart Card Service
 *
 * The smart card service provides functions for cards supporting ISO7816 protocol.
 *
 * @{
 */

//! Checking board configuration of the ISO7816 handler.
#if !defined(ISO7816_USART)
# warning The ISO7816 instance has not been defined. Using default settings.
# define ISO7816_USART  USART0
#endif

/** Case for APDU commands. */
#define CASE1           1
#define CASE2           2
#define CASE3           3

/** Flip flop for send and receive char. */
#define USART_SEND      0
#define USART_RCV       1

/** NULL byte to restart byte procedure. */
#define ISO_NULL_VAL    0x60

/** Receive timeout. */
#define RX_TIMEOUT      12000

/** Reset wait time. */
#define RST_WAIT_TIME   120

/** Variable for state of send and receive from USART. */
static uint8_t gs_uc_state = USART_RCV;

/** Card reset PIN. */
uint32_t gs_ul_rst_pin_idx;

/** Define the main clock of the system. */
volatile uint32_t g_ul_clk;

/**
 * \brief Get a character from ISO7816.
 *
 * \param p_char_received Pointer for store the received char.
 *
 * \return 0: if timeout else status of US_CSR.
 */
static uint32_t iso7816_get_char(uint8_t *p_char_received)
{
	uint32_t ul_data;
	uint32_t ul_status;
	uint32_t ul_timeout = 0;

	if (gs_uc_state == USART_SEND) {
		while ((usart_get_status(ISO7816_USART) & US_CSR_TXEMPTY) == 0) {
		}
		usart_reset_status(ISO7816_USART);
		usart_reset_iterations(ISO7816_USART);
		usart_reset_nack(ISO7816_USART);
		gs_uc_state = USART_RCV;
	}

	/* Wait USART ready for reception. */
	while (((usart_get_status(ISO7816_USART) & US_CSR_RXRDY) == 0)) {
		if (ul_timeout++ > RX_TIMEOUT * (g_ul_clk / 1000000)) {
			return (0);
		}
	}

	/* At least one complete character has been received and US_RHR has not yet been read. */
	usart_read(ISO7816_USART, &ul_data);
	/* ISO7816 only has 8 bits data. */
	*p_char_received = 0xFF & ul_data;

	ul_status = usart_get_status(ISO7816_USART) & (US_CSR_OVRE |
			US_CSR_FRAME | US_CSR_PARE | US_CSR_TIMEOUT |
			US_CSR_NACK | US_CSR_ITER);

	if (ul_status != 0) {
		usart_reset_status(ISO7816_USART);
	}

	/* Return status. */
	return (ul_status);
}


/**
 * \brief Send a char to ISO7816.
 *
 * \param uc_char Char to be sent.
 *
 * \return status of US_CSR.
 */
static uint32_t iso7816_send_char(uint8_t uc_char)
{
	uint32_t ul_status;

	if (USART_RCV == gs_uc_state) {
		usart_reset_status(ISO7816_USART);
		usart_reset_iterations(ISO7816_USART);
		usart_reset_nack(ISO7816_USART);
		gs_uc_state = USART_SEND;
	}

	/* Wait USART ready for transmit. */
	while ((usart_get_status(ISO7816_USART) & US_CSR_TXRDY) == 0) {
	}

	/* There is no character in the US_THR, transmit a char. */
	usart_write(ISO7816_USART, uc_char);

	ul_status = usart_get_status(ISO7816_USART) & (US_CSR_OVRE |
			US_CSR_FRAME | US_CSR_PARE | US_CSR_TIMEOUT |
			US_CSR_NACK | US_CSR_ITER);

	if (ul_status != 0) {
		/** There are errors happened, reset the status bit. */
		usart_reset_status(ISO7816_USART);
	}

	/* Return status. */
	return (ul_status);
}


/**
 * \brief ISO7816 ICC power on.
 */
static void iso7816_icc_power_on(void)
{
	/* Set RESET Master Card. */
#if defined(SMART_CARD_USING_GPIO)
	gpio_set_pin_high(gs_ul_rst_pin_idx);
#elif defined(SMART_CARD_USING_IOPORT)
	ioport_set_pin_level(gs_ul_rst_pin_idx, IOPORT_PIN_LEVEL_HIGH);
#endif
}

/**
 * \brief ISO7816 ICC power off.
 */
void iso7816_icc_power_off(void)
{
	/* Clear RESET Master Card. */
#if defined(SMART_CARD_USING_GPIO)
	gpio_set_pin_low(gs_ul_rst_pin_idx);
#elif defined(SMART_CARD_USING_IOPORT)
	ioport_set_pin_level(gs_ul_rst_pin_idx, IOPORT_PIN_LEVEL_LOW);
#endif
}

/**
 * \brief Transfer Block TPDU T = 0.
 *
 * \param p_apdu     Pointer to APDU buffer.
 * \param p_message  Pointer to message buffer.
 * \param us_length  Block length.
 *
 * \return           Message index.
 */
uint16_t iso7816_xfr_block_tpdu_t0(const uint8_t *p_apdu,
		uint8_t *p_message, uint16_t us_length)
{
	uint16_t us_ne_nc;
	uint16_t us_apdu_index = 4;
	uint16_t us_message_index = 0;
	uint8_t uc_sw1 = 0;
	uint8_t uc_proc_byte;
	uint8_t uc_cmd_case;

	iso7816_send_char(p_apdu[0]); /* CLA */
	iso7816_send_char(p_apdu[1]); /* INS */
	iso7816_send_char(p_apdu[2]); /* P1 */
	iso7816_send_char(p_apdu[3]); /* P2 */
	iso7816_send_char(p_apdu[4]); /* P3 */

	/* Handle the four structures of command APDU. */
	us_apdu_index = 4;

	if (us_length == 4) {
		uc_cmd_case = CASE1;
		us_ne_nc = 0;
	} else if (us_length == 5) {
		uc_cmd_case = CASE2;
		us_ne_nc = p_apdu[4]; /* C5 */
		if (us_ne_nc == 0) {
			us_ne_nc = 256;
		}
	} else if (us_length == 6) {
		us_ne_nc = p_apdu[4]; /* C5 */
		uc_cmd_case = CASE3;
	} else if (us_length == 7) {
		us_ne_nc = p_apdu[4]; /* C5 */
		if (us_ne_nc == 0) {
			uc_cmd_case = CASE2;
			us_ne_nc = (p_apdu[5] << 8) + p_apdu[6];
		} else {
			uc_cmd_case = CASE3;
		}
	} else {
		us_ne_nc = p_apdu[4]; /* C5 */
		if (us_ne_nc == 0) {
			uc_cmd_case = CASE3;
			us_ne_nc = (p_apdu[5] << 8) + p_apdu[6];
		} else {
			uc_cmd_case = CASE3;
		}
	}

	/* Handle Procedure Bytes. */
	do {
		iso7816_get_char(&uc_proc_byte);
		uint8_t uc_proc_byte_x = (uc_proc_byte ^ 0xff);
		/* Handle NULL. */
		if (ISO_NULL_VAL == uc_proc_byte) {
			continue;
		}
		/* Handle sw1. */
		else if (((uc_proc_byte & 0xF0) == 0x60) ||
				((uc_proc_byte & 0xF0) == 0x90)) {
			uc_sw1 = 1;
		}
		/* Handle INS. */
		else if (p_apdu[1] == uc_proc_byte) {
			if (uc_cmd_case == CASE2) {
				/* Receive data from card. */
				do {
					iso7816_get_char(&p_message[us_message_index++]);
				} while (0 != --us_ne_nc);
			} else {
				/* Send data. */
				do {
					iso7816_send_char(p_apdu[us_apdu_index++]);
				} while (0 != --us_ne_nc);
			}
		}
		/* Handle INS ^ 0xff. */
		else if (p_apdu[1] == uc_proc_byte_x) {
			if (uc_cmd_case == CASE2) {
				/* receive data from card. */
				iso7816_get_char(&p_message[us_message_index++]);
			} else {
				iso7816_send_char(p_apdu[us_apdu_index++]);
			}
			us_ne_nc--;
		} else {
			break;
		}
	} while (us_ne_nc != 0);

	/* Status Bytes. */
	if (uc_sw1 == 0) {
		iso7816_get_char(&p_message[us_message_index++]); /* uc_sw1 */
	} else {
		p_message[us_message_index++] = uc_proc_byte;
	}
	iso7816_get_char(&p_message[us_message_index++]); /* SW2 */

	return (us_message_index);
}

/**
 * \brief Answer To Reset (ATR).
 *
 * \param p_atr    Pointer to ATR buffer.
 * \param p_length Pointer for store the ATR length.
 */
void iso7816_data_block_atr(uint8_t *p_atr, uint8_t *p_length)
{
	uint32_t i;
	uint32_t j;
	uint8_t uc_value;

	*p_length = 0;

	/* Read ATR TS. */
	iso7816_get_char(&p_atr[0]);
	/* Read ATR T0. */
	iso7816_get_char(&p_atr[1]);
	uc_value = p_atr[1] & 0xF0;
	i = 2;

	/* Read ATR Ti. */
	while (uc_value) {
		if (uc_value & 0x10) { /* TA[i] */
			iso7816_get_char(&p_atr[i++]);
		}

		if (uc_value & 0x20) { /* TB[i] */
			iso7816_get_char(&p_atr[i++]);
		}

		if (uc_value & 0x40) { /* TC[i] */
			iso7816_get_char(&p_atr[i++]);
		}

		if (uc_value & 0x80) { /* TD[i] */
			iso7816_get_char(&p_atr[i]);
			uc_value = p_atr[i++] & 0xF0;
		} else {
			uc_value = 0;
		}
	}

	/* Historical Bytes. */
	uc_value = p_atr[1] & 0x0F;
	for (j = 0; j < uc_value; j++) {
		iso7816_get_char(&p_atr[i++]);
	}

	*p_length = i;
}

/**
 * \brief Pin status for ISO7816 RESET PIN.
 *
 * \return 1 if the Pin RESET is high; otherwise 0.
 */
uint8_t iso7816_get_reset_statuts(void)
{
#if defined(SMART_CARD_USING_GPIO)
	return gpio_pin_is_high(gs_ul_rst_pin_idx);
#elif defined(SMART_CARD_USING_IOPORT)
	return (ioport_get_pin_level(gs_ul_rst_pin_idx) == IOPORT_PIN_LEVEL_HIGH);
#endif
}

/**
 * \brief Cold reset.
 */
void iso7816_cold_reset(void)
{
	uint32_t i;
	uint32_t ul_data;

	/* tb: wait 400 cycles */
	for (i = 0; i < (RST_WAIT_TIME * (g_ul_clk / 1000000)); i++) {
	}

	usart_read(ISO7816_USART, &ul_data);
	usart_reset_status(ISO7816_USART);
	usart_reset_iterations(ISO7816_USART);
	usart_reset_nack(ISO7816_USART);

	iso7816_icc_power_on();
}

/**
 * \brief Warm reset.
 */
void iso7816_warm_reset(void)
{
	uint32_t i;
	uint32_t ul_data;

	iso7816_icc_power_off();

	/* tb: wait 400 cycles. */
	for (i = 0; i < (RST_WAIT_TIME * (g_ul_clk / 1000000)); i++) {
	}

	usart_read(ISO7816_USART, &ul_data);
	usart_reset_status(ISO7816_USART);
	usart_reset_iterations(ISO7816_USART);
	usart_reset_nack(ISO7816_USART);

	iso7816_icc_power_on();
}

/**
 * \brief Decode ATR trace.
 *
 * \param p_atr Pinter on ATR buffer.
 */
void iso7816_decode_atr(uint8_t *p_atr)
{
	uint32_t i;
	uint32_t j;
	uint32_t y;
	uint8_t uc_offset;

	i = 2;
	y = p_atr[1] & 0xF0;

	/* Read ATR Ti. */
	uc_offset = 1;
	while (y) {
		if (y & 0x10) { /* TA[i] */
			i++;
		}
		if (y & 0x20) { /* TB[i] */
			i++;
		}
		if (y & 0x40) { /* TC[i] */
			i++;
		}
		if (y & 0x80) { /* TD[i] */
			y = p_atr[i++] & 0xF0;
		} else {
			y = 0;
		}
		uc_offset++;
	}

	y = p_atr[1] & 0x0F;
	for (j = 0; j < y; j++) {
		i++;
	}
}

/**
 * \brief Initializes a ISO7816 interface device.
 *
 * \param p_usart_opt     Pointer to an ISO7816 instance.
 * \param ul_mck          USART module input clock frequency.
 * \param ul_rst_pin_idx  Control smart card RST pin index.
 */
unsigned int iso7816_init(const usart_iso7816_opt_t *p_usart_opt,
		uint32_t ul_mck, uint32_t ul_rst_pin_idx)
{
	/* Pin RST of ISO7816 initialize. */
	gs_ul_rst_pin_idx = ul_rst_pin_idx;
#if defined(SMART_CARD_USING_GPIO)
	gpio_set_pin_low(gs_ul_rst_pin_idx);
#elif defined(SMART_CARD_USING_IOPORT)
	ioport_set_pin_level(gs_ul_rst_pin_idx, IOPORT_PIN_LEVEL_LOW);
#endif
	/* Init the global variable for ISO7816. */
	g_ul_clk = ul_mck;

	if(usart_init_iso7816(ISO7816_USART, p_usart_opt, g_ul_clk)){
		return 1;
	}

	/* Disable interrupts. */
	usart_disable_interrupt(ISO7816_USART, 0xffffffff);

	/* Write the Timeguard Register. */
	usart_set_tx_timeguard(ISO7816_USART, 5);

	/* Enable TX and RX. */
	usart_enable_rx(ISO7816_USART);
	//usart_enable_tx(ISO7816_USART);
	
	return 0;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
