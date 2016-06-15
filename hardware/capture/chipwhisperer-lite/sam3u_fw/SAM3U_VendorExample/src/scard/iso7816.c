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

static uint8_t ta1, td1, tc3;

/**
 * \brief ISO7816 ICC power on.
 */
static void iso7816_icc_power_on(void)
{
	/* Set RESET Master Card. */
	gpio_set_pin_high(gs_ul_rst_pin_idx);
}

/**
 * \brief ISO7816 ICC power off.
 */
void iso7816_icc_power_off(void)
{
	/* Clear RESET Master Card. */
	gpio_set_pin_low(gs_ul_rst_pin_idx);
}


/**
 * \brief Update CRC value.
 *
 * \param data Data to be calculated.
 * \param crc CRC result.
 *
 * \return CRC value.
 */
static uint16_t iso7816_update_crc(uint8_t data, uint16_t crc)
{
	uint8_t idx;
	uint16_t tmp = (uint16_t)data << 8;

	for (idx = 0; idx < 8; idx++) {
		if ((crc ^ tmp) & 0x8000) {
			crc <<= 1;
			crc ^= (uint16_t)0x1021; /* X^12 + X^5 + 1 */
		} else {
			crc <<= 1;
		}

		tmp <<= 1;
	}

	return crc;
}

/**
 * \brief Update EDC value.
 *
 * \param data Data to be calculated.
 * \param edc EDC result.
 *
 * \return EDC value.
 */
static void iso7816_update_edc(uint8_t data, uint16_t *edc)
{
	/* Store the updated LRC/CRC in the EDC */
	if (tc3 & 0x1) {  /* type = CRC */
		*edc = iso7816_update_crc(data, *edc);
	} else { /* type = LRC */
		*edc = *edc ^ data;
	}
}

/**
 * \brief Get a character from ISO7816.
 *
 * \param p_usart Pointer to the usart device.
 * \param p_char_received Pointer for store the received char.
 *
 * \return 0xff: if timeout else status of US_CSR.
 */
uint32_t iso7816_get_char(Usart* p_usart, uint8_t *p_char_received)
{
	uint32_t ul_data;
	uint32_t ul_status;
	uint32_t ul_timeout = 0;

	if (gs_uc_state == USART_SEND) {		
		while ((usart_get_status(p_usart) & US_CSR_TXEMPTY) ==
				0) {
		}	
		/*
		usart_reset_status(p_usart);
		usart_reset_iterations(p_usart);
		usart_reset_nack(p_usart);
		*/
		gs_uc_state = USART_RCV;
		
		//iso7816_icc_power_off();
		//iso7816_icc_power_on();	
	}

	/* Wait USART ready for reception. */
	while (((usart_get_status(p_usart) & US_CSR_RXRDY) == 0)) {
		if (ul_timeout++ > RX_TIMEOUT * (g_ul_clk / 1000000)) {
			return (0xff);
		}
	}

	/* At least one complete character has been received and US_RHR has not
	 * yet been read. */
	usart_read(p_usart, &ul_data);
	/* ISO7816 only has 8 bits data. */
	*p_char_received = 0xFF & ul_data;

	ul_status = usart_get_status(p_usart) & (US_CSR_OVRE |
			US_CSR_FRAME | US_CSR_PARE | US_CSR_TIMEOUT |
			US_CSR_NACK | US_CSR_ITER);

	if (ul_status != 0) {
		usart_reset_status(p_usart);
	}

	/* Return status. */
	return (ul_status);
}

/**
 * \brief Send a char to ISO7816.
 *
 * \param p_usart Pointer to the usart device.
 * \param uc_char Char to be sent.
 *
 * \return status of US_CSR.
 */
uint32_t iso7816_send_char(Usart* p_usart, uint8_t uc_char)
{
	uint32_t ul_status;

	if (USART_RCV == gs_uc_state) {
		usart_reset_status(p_usart);
		usart_reset_iterations(p_usart);
		usart_reset_nack(p_usart);
		gs_uc_state = USART_SEND;
	}

	/* Wait USART ready for transmit. */
	while ((usart_get_status(p_usart) & US_CSR_TXRDY) == 0) {
	}

	/* There is no character in the US_THR, transmit a char. */
	usart_write(p_usart, uc_char);

	ul_status = usart_get_status(p_usart) & (US_CSR_OVRE |
			US_CSR_FRAME | US_CSR_PARE | US_CSR_TIMEOUT |
			US_CSR_NACK | US_CSR_ITER);

	if (ul_status != 0) {
		/** There are errors happened, reset the status bit. */
		usart_reset_status(p_usart);
	}

	/* Return status. */
	return (ul_status);
}

/**
 * \brief Transfer Block APDU T = 0.
 *
 * \param p_usart Pointer to the usart device.
 * \param p_apdu     Pointer to APDU buffer.
 * \param p_message  Pointer to message buffer.
 * \param us_length  Block length.
 *
 * \return           Message index.
 */
uint16_t iso7816_xfr_block_apdu_t0(Usart* p_usart, const uint8_t *p_apdu,
		uint8_t *p_message, uint16_t us_length)
{
	uint16_t us_ne_nc;
	uint16_t us_apdu_index;
	uint16_t us_message_index = 0;
	uint8_t uc_sw1 = 0;
	uint8_t uc_proc_byte;
	uint8_t uc_cmd_case;
	
	uint32_t status = 0;

	status |= iso7816_send_char(p_usart, p_apdu[0]); /* CLA */
	status |= iso7816_send_char(p_usart, p_apdu[1]); /* INS */	
	status |= iso7816_send_char(p_usart, p_apdu[2]); /* P1 */
	status |= iso7816_send_char(p_usart, p_apdu[3]); /* P2 */
	status |= iso7816_send_char(p_usart, p_apdu[4]); /* P3 */

	/* Handle the four structures of command APDU. */
	us_apdu_index = 5;

	/* Check transfer cases */
	if (us_length == 4) {
		uc_cmd_case = CASE1;
		us_ne_nc = 0;
	} else if (us_length == 5) {
		uc_cmd_case = CASE2;
		us_ne_nc = p_apdu[4];
		if (us_ne_nc == 0) {
			us_ne_nc = 256;
		}
	} else if (us_length == 6) {
		us_ne_nc = p_apdu[4];
		uc_cmd_case = CASE3;
	} else if (us_length == 7) {
		us_ne_nc = p_apdu[4];
		if (us_ne_nc == 0) {
			uc_cmd_case = CASE2;
			us_ne_nc = (p_apdu[5] << 8) + p_apdu[6];
		} else {
			uc_cmd_case = CASE3;
		}
	} else {
		us_ne_nc = p_apdu[4];
		if (us_ne_nc == 0) {
			uc_cmd_case = CASE3;
			us_ne_nc = (p_apdu[5] << 8) + p_apdu[6];
		} else {
			uc_cmd_case = CASE3;
		}
	}

	/* Handle procedure bytes. */
	do {
		status |= iso7816_get_char(p_usart, &uc_proc_byte);
		
		//uc_proc_byte = p_apdu[1];
		
		uint8_t uc_proc_byte_x = (uc_proc_byte ^ 0xff);
		/* Handle NULL. */
		if (ISO_NULL_VAL == uc_proc_byte) {
			continue;
		}
		/* Handle SW1. */
		else if (((uc_proc_byte & 0xF0) == 0x60) ||
				((uc_proc_byte & 0xF0) == 0x90)) {
			uc_sw1 = 1;
		}
		/* Handle INS. */
		else if (p_apdu[1] == uc_proc_byte) {
			if (uc_cmd_case == CASE2) {
				/* Receive data from card. */
				do {
					status |= iso7816_get_char(p_usart, &p_message[us_message_index++]);
				} while (0 != --us_ne_nc);
			} else {
				/* Send data. */
				do {
					status |= iso7816_send_char(p_usart,
							p_apdu[us_apdu_index++]);
				} while (0 != --us_ne_nc);
			}
		}
		/* Handle INS ^ 0xff. */
		else if (p_apdu[1] == uc_proc_byte_x) {
			if (uc_cmd_case == CASE2) {
				/* receive data from card. */
				status |= iso7816_get_char(p_usart, &p_message[us_message_index++]);
			} else {
				status |= iso7816_send_char(p_usart, p_apdu[us_apdu_index++]);
			}

			us_ne_nc--;
		} else {
			break;
		}
	} while (us_ne_nc != 0);

	/* Status Bytes. */
	if (uc_sw1 == 0) {
		status |= iso7816_get_char(p_usart, &p_message[us_message_index++]); /* SW1 */
	} else {
		p_message[us_message_index++] = uc_proc_byte;
	}

	status |= iso7816_get_char(p_usart, &p_message[us_message_index++]); /* SW2 */
	
	printf("%x\n", (unsigned int)status);

	return (us_message_index);
}

/**
 * \brief Transfer Block APDU T = 1.
 *
 * \param p_usart Pointer to the usart device.
 * \param p_apdu     Pointer to APDU buffer.
 * \param p_message  Pointer to message buffer.
 * \param us_length  Block length.
 *
 * \return           Message index.
 */
uint16_t iso7816_xfr_block_apdu_t1(Usart* p_usart, const uint8_t *p_apdu,
		uint8_t *p_message, uint16_t us_length)
{
	uint8_t index;
	uint16_t edc;
	uint32_t rx_len;
	uint16_t us_message_index = 0;

	if (us_length <= 0) {
		return false;
	}

	if (tc3 & 0x1) {
		edc = 0xFFFF;
	} else {
		edc = 0;
	}

	/* Update the edc for the data to be transmitted */
	for (index = 0; index < (p_apdu[2] + 3); index++) {
		iso7816_update_edc(p_apdu[index], &edc);
	}

	/* Transmit Data Bytes */
	for (index = 0; index < (p_apdu[2] + 3); index++) {
		iso7816_send_char(p_usart, p_apdu[index]);
	}

	/* Transmit EDC */
	iso7816_send_char(p_usart, (uint8_t)edc);

	if (tc3 & 0x1) {
		iso7816_send_char(p_usart, (uint8_t)edc >> 8);
	}

	if (iso7816_get_char(p_usart, &p_message[0]) == 0xff) {
		return us_message_index;
	}

	us_message_index++;

	if (iso7816_get_char(p_usart, &p_message[1]) == 0xff) {
		return us_message_index;
	}

	us_message_index++;

	if (iso7816_get_char(p_usart, &p_message[2]) == 0xff) {
		return us_message_index;
	}

	us_message_index++;

	rx_len = p_message[2] + 1;

	if (tc3 & 0x1) {
		rx_len++;
	}

	for (index = 3; index < (rx_len + 3); index++) {
		if (iso7816_get_char(p_usart, &p_message[index]) == 0xff) {
			return us_message_index;
		}
	}

	/* Check for the CRC Error */
	if (tc3 & 0x1) {
		edc = 0xFFFF;
		edc = iso7816_update_crc(p_message[0], edc);
		edc = iso7816_update_crc(p_message[1], edc);
		edc = iso7816_update_crc(p_message[2], edc);

		for (index = 3; index < (rx_len + 3); index++) {
			edc = iso7816_update_crc(p_message[index], edc);
		}

		if (((edc >> 8) != p_message[rx_len - 2]) ||
				((edc & 0xFF) != p_message[rx_len - 1])) {
			return us_message_index;
		}
	} else {
		edc = 0;
		edc = edc ^ p_message[0];
		edc = edc ^ p_message[1];
		edc = edc ^ p_message[2];

		for (index = 3; index < (rx_len + 3); index++) {
			edc = edc ^ p_message[index];
		}

		if (edc != p_message[index + 1]) {
			return us_message_index;
		}
	}

	us_message_index += rx_len;
	return us_message_index;
}



/**
 * \brief Answer To Reset (ATR).
 *
 * \param p_usart Pointer to the usart device.
 * \param p_atr    Pointer to ATR buffer.
 * \param p_length Pointer for store the ATR length.
 */
void iso7816_data_block_atr(Usart* p_usart, uint8_t *p_atr, uint8_t *p_length)
{
	uint32_t i;
	uint32_t j;
	uint8_t uc_value;

	*p_length = 0;

	ta1=0;
	td1=0;
	tc3=0;

	/* Read ATR TS. */
	iso7816_get_char(p_usart, &p_atr[0]);

	/* Read ATR T0. */
	iso7816_get_char(p_usart, &p_atr[1]);

	uc_value = p_atr[1] & 0xF0;
	i = 2;

	/* Read ATR T1. */
	if (uc_value & 0x10) { /* TA[1] */
		iso7816_get_char(p_usart, &p_atr[i++]);
		ta1 = p_atr[2];
	}

	if (uc_value & 0x20) { /* TB[1] */
		iso7816_get_char(p_usart, &p_atr[i++]);
	}

	if (uc_value & 0x40) { /* TC[1] */
		iso7816_get_char(p_usart, &p_atr[i++]);
	}

	if (uc_value & 0x80) { /* TD[1] */
		iso7816_get_char(p_usart, &p_atr[i]);
		td1 = p_atr[i];
		uc_value = p_atr[i++] & 0xF0;
	} else {
		uc_value = 0;
	}

	if (uc_value) {
		if (uc_value & 0x10) { /* TA[2] */
			iso7816_get_char(p_usart, &p_atr[i++]);
		}

		if (uc_value & 0x20) { /* TB[2] */
			iso7816_get_char(p_usart, &p_atr[i++]);
		}

		if (uc_value & 0x40) { /* TC[2] */
			iso7816_get_char(p_usart, &p_atr[i++]);
		}

		if (uc_value & 0x80) { /* TD[2] */
			iso7816_get_char(p_usart, &p_atr[i]);
			uc_value = p_atr[i++] & 0xF0;
		} else {
			uc_value = 0;
		}
	}

	if (uc_value) {
		if (uc_value & 0x10) { /* TA[3] */
			iso7816_get_char(p_usart, &p_atr[i++]);
		}

		if (uc_value & 0x20) { /* TB[3] */
			iso7816_get_char(p_usart, &p_atr[i++]);
		}

		if (uc_value & 0x40) { /* TC[3] */
			iso7816_get_char(p_usart, &p_atr[i++]);
			tc3 = p_atr[i - 1];
		}

		if (uc_value & 0x80) { /* TD[3] */
			iso7816_get_char(p_usart, &p_atr[i]);
			uc_value = p_atr[i++] & 0xF0;
		} else {
			uc_value = 0;
		}
	}

	/* Historical Bytes. */
	uc_value = p_atr[1] & 0x0F;
	for (j = 0; j < uc_value; j++) {
		iso7816_get_char(p_usart, &p_atr[i++]);
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
	return gpio_pin_is_high(gs_ul_rst_pin_idx);
}

/**
 * \brief Cold reset.
 *
 * \param p_usart Pointer to the usart device.
 */
void iso7816_cold_reset(Usart* p_usart)
{
	uint32_t i;
	uint32_t ul_data;

	/* tb: wait 400 cycles */
	for (i = 0; i < (RST_WAIT_TIME * (g_ul_clk / 1000000)); i++) {
	}

	usart_read(p_usart, &ul_data);
	usart_reset_status(p_usart);
	usart_reset_iterations(p_usart);
	usart_reset_nack(p_usart);

	iso7816_icc_power_on();
}

/**
 * \brief Warm reset.
 *
 * \param p_usart Pointer to the usart device.
 */
void iso7816_warm_reset(Usart* p_usart)
{
	uint32_t i;
	uint32_t ul_data;

	iso7816_icc_power_off();

	/* tb: wait 400 cycles. */
	for (i = 0; i < (RST_WAIT_TIME * (g_ul_clk / 1000000)); i++) {
	}

	usart_read(p_usart, &ul_data);
	usart_reset_status(p_usart);
	usart_reset_iterations(p_usart);
	usart_reset_nack(p_usart);

	iso7816_icc_power_on();
}

/**
 * \brief Decode ATR trace and do PPS.
 *
 * \param p_usart Pointer to the usart device.
 * \param p_atr Pinter on ATR buffer.
 */
uint8_t iso7816_decode_atr(Usart* p_usart, uint8_t *p_atr)
{
	uint32_t ftable[16] = {0, 372, 558, 744, 1116, 1488, 1860, 0,
			       0, 512, 768, 1024, 1536, 2048, 0, 0};
	uint32_t dtable[10] = {0, 1, 2, 4, 8, 16, 32, 64, 12, 20};
	uint8_t ppss[4] = {0,0,0,0};

	uint32_t i;
	uint32_t j;
	uint32_t y;
	uint8_t uc_offset;
	uint32_t fidi = 0;

	i = 2;
	y = p_atr[1] & 0xF0;

	/* Read ATR Ti. */
	uc_offset = 1;

	if (y & 0x10) {
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
	}

	y = p_atr[1] & 0x0F;
	for (j = 0; j < y; j++) {
		i++;
	}

	if (td1 & 0x10) {
		return 0;
	}

	if (ta1 && ((ta1 & 0xf) != 0)) {
		fidi = ftable[(ta1 >> 4) & 0xf] / dtable[ta1 & 0xf];
	}

	if (td1 & 0xf) {
		return (td1 & 0xf);
	}
	
	// Propose speeds to card
	iso7816_send_char(p_usart, 0xff);
	iso7816_send_char(p_usart, 0x10);
	iso7816_send_char(p_usart, ta1);
	iso7816_send_char(p_usart, 0xff ^ 0x10 ^ ta1);

	iso7816_get_char(p_usart, &ppss[0]);
	iso7816_get_char(p_usart, &ppss[1]);
	iso7816_get_char(p_usart, &ppss[2]);
	iso7816_get_char(p_usart, &ppss[3]);

	if ((ppss[0] != 0xff) || (ppss[1] != 0x10)) {
		// Fail - if card doesn't support this we reset card again
		iso7816_warm_reset(p_usart);
		// Read ATR to clear buffer
		uint8_t plength;
		iso7816_data_block_atr(p_usart, p_atr, &plength);
		return 0;
	}

	if (ppss[2] != ta1) {
		if (ppss[3] == (0xff ^ 0x10 ^ ppss[2])) {
			fidi = ftable[(ppss[3] >>
					4) & 0xf] / dtable[ppss[3] & 0xf];
		} else {
			// Fail - if card doesn't support this we reset card again
			iso7816_warm_reset(p_usart);
			// Read ATR to clear buffer
			uint8_t plength;
			iso7816_data_block_atr(p_usart, p_atr, &plength);
			return 0;
		}
	}

	/* Enable TX and RX. */
	usart_disable_rx(p_usart);
	usart_disable_tx(p_usart);

	p_usart->US_FIDI = fidi;

	usart_enable_rx(p_usart);
	usart_enable_tx(p_usart);

	return 0;
}

/**
 * \brief Initializes a ISO7816 interface device.
 *
 * \param p_usart Pointer to the usart device.
 * \param p_usart_opt     Pointer to an ISO7816 instance.
 * \param ul_mck          USART module input clock frequency.
 * \param ul_rst_pin_idx  Control smart card RST pin index.
 */
void iso7816_init(Usart* p_usart, const usart_iso7816_opt_t *p_usart_opt,
		uint32_t ul_mck, uint32_t ul_rst_pin_idx)
{
	/* Pin RST of ISO7816 initialize. */
	gs_ul_rst_pin_idx = ul_rst_pin_idx;
	gpio_set_pin_low(gs_ul_rst_pin_idx);

	/* Init the global variable for ISO7816. */
	g_ul_clk = ul_mck;

	usart_init_iso7816(p_usart, p_usart_opt, g_ul_clk);

	/* Disable interrupts. */
	usart_disable_interrupt(p_usart, 0xffffffff);

	/* Write the Timeguard Register. */
	usart_set_tx_timeguard(p_usart, 5);

	/* Enable TX and RX. */
	usart_enable_rx(p_usart);
	usart_enable_tx(p_usart);
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
