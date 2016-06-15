/*
 Copyright (c) 2015 NewAE Technology Inc. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <asf.h>
#include "scard_usb.h"
#include "scard\at88sc102_lowlevel.h"
#include "scard\at88sc102_highlevel.h"
#include "scard\iso7816.h"

#define DATA_BUFFER_SIZE	256

/* Buffer holding APDU response */
uint8_t scard_rx_buffer[DATA_BUFFER_SIZE];
uint16_t scard_rx_buffer_size;

/* Buffer holding APDU to transmit */
uint8_t scard_tx_buffer[DATA_BUFFER_SIZE];
uint16_t scard_tx_buffer_size;

/* For smartcard testing */
#define ISO7816_BAUDRATE 9600
#define ISO7816_FI_DI 372

/* Buffer holding ATR */
uint8_t scard_atr[MAX_ATR_SIZE];
uint8_t scard_atr_size;
uint8_t scard_protocol;

#define SCARD_CFG_VERSION 0x00
#define SCARD_CFG_ATR 0x01
#define SCARD_CFG_PROTOCOL 0x02
#define SCARD_CFG_TXRX 0x05

/* AT88SC102 Mode Specific */
uint8_t at88sc102_status;

#define SCARD_MODE_AT88SC102 0x03
#define SCARD_MODE_AT88SC102_INIT 0x02
#define SCARD_MODE_AT88SC102_SENDPIN 0x03
#define SCARD_MODE_AT88SC102_CHANGEPIN 0x04
#define SCARD_MODE_AT88SC102_MOOLTIPASS 0x05
#define SCARD_MODE_AT88SC102_TESTPIN 0x06

/* Handle "Smartcard Data" request */
bool ctrl_scarddata_req(void)
{
	uint8_t offset = (udd_g_ctrlreq.req.wValue >> 8) & 0xFF;
	uint8_t datalen = (udd_g_ctrlreq.req.wValue) & 0xFF;
	uint16_t rxlen;
	
	if (offset > scard_rx_buffer_size){
		/* No data to send - point our buffer to memory actually used to prevent errors */
		rxlen = 0;
		offset = 0;
	} else {
		rxlen = scard_rx_buffer_size - offset;
		if (rxlen > datalen){
			rxlen = datalen;
		}
	}
	
	udd_g_ctrlreq.payload = scard_rx_buffer + offset;
	udd_g_ctrlreq.payload_size = rxlen;
	
	return true;
}

/* Handle "Smartcard Config" request */
bool ctrl_scardconfig_req(void)
{
	/* Version information, used to indicate to host computer what the firmware in the CW-Lite 
	   supports. */
	static uint8_t scard_protocol_version = 02;
	
	switch(udd_g_ctrlreq.req.wValue & 0xff){
		case SCARD_CFG_ATR:
			udd_g_ctrlreq.payload = scard_atr;
			udd_g_ctrlreq.payload_size = scard_atr_size;
			return true;
			break;
			
		case SCARD_CFG_PROTOCOL:
			udd_g_ctrlreq.payload = &scard_protocol;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		case SCARD_CFG_VERSION:
			udd_g_ctrlreq.payload = &scard_protocol_version;
			udd_g_ctrlreq.payload_size = 1;
			return true;
			break;
			
		default:
			return false;
			break;
	}
	
}

/* Handle "Smartcard Auxilary" request */
bool ctrl_scardaux_req(void)
{
	/*TODO: Temp hack for SC102 testing */
	udd_g_ctrlreq.payload = &at88sc102_status;
	udd_g_ctrlreq.payload_size = 1;
	return true;
}

/* Handle "Smartcard Data" Output from Computer */
void ctrl_scarddata_cb(void)
{
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size)	return;
	
	for(uint8_t i = 0; i < udd_g_ctrlreq.req.wLength; i++){		
		if ((udd_g_ctrlreq.req.wValue + i) < DATA_BUFFER_SIZE){
			scard_tx_buffer[udd_g_ctrlreq.req.wValue + i] = udd_g_ctrlreq.payload[i];
			scard_tx_buffer_size = udd_g_ctrlreq.req.wValue + i + 1;
		}
	}
}

/* Handle "Smartcard Config" Output from Computer */
void ctrl_scardconfig_cb(void)
{
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size)	return;
	
	switch(udd_g_ctrlreq.req.wValue & 0xff){
		case SCARD_CFG_ATR:
			scard_reset_init();
			break;
		
		case SCARD_CFG_TXRX:
			scard_rx_buffer_size = 0;
			if (scard_protocol){
				scard_rx_buffer_size = iso7816_xfr_block_apdu_t1(SCARD_USART, scard_tx_buffer, scard_rx_buffer, scard_tx_buffer_size);
			} else {
				scard_rx_buffer_size = iso7816_xfr_block_apdu_t0(SCARD_USART, scard_tx_buffer, scard_rx_buffer, scard_tx_buffer_size);
			}
			break;
		default:
		break;
	}
}

/* Handle "Smartcard Aux" Output from Computer */
void ctrl_scardaux_cb(void)
{
	if (udd_g_ctrlreq.req.wLength > udd_g_ctrlreq.payload_size)	return;
	
	if((udd_g_ctrlreq.req.wValue & 0xFF) == SCARD_MODE_AT88SC102)
	{
		at88sc102_status = 0;
		if (udd_g_ctrlreq.req.wLength < 1) return;
		
		udd_g_ctrlreq.req.wLength--;
		switch(udd_g_ctrlreq.payload[0]) {
			case SCARD_MODE_AT88SC102_INIT:
			/* FPGA must have been switched into proper mode as well */
			initPortSMC();
			at88sc102_status = firstDetectFunctionSMC();
			break;
			
			case SCARD_MODE_AT88SC102_SENDPIN:
			if (udd_g_ctrlreq.req.wLength < 2) return;
			at88sc102_status = securityValidationSMC(*((uint16_t *)(udd_g_ctrlreq.payload+1)), FALSE);
			break;
			
			case SCARD_MODE_AT88SC102_CHANGEPIN:
			if (udd_g_ctrlreq.req.wLength < 2) return;
			writeSecurityCode(((uint16_t *)(udd_g_ctrlreq.payload+1)));
			at88sc102_status = 1;
			break;
			
			case SCARD_MODE_AT88SC102_MOOLTIPASS:
			if (transformBlankCardIntoMooltipass() == RETURN_OK){
				at88sc102_status = 1;
			}
			break;
			
			case SCARD_MODE_AT88SC102_TESTPIN:
			if (udd_g_ctrlreq.req.wLength < 3) return;
			at88sc102_status = securityValidationSMC(*((uint16_t *)(udd_g_ctrlreq.payload+2)), *(udd_g_ctrlreq.payload+1));
			break;
			
		}
		
	}
	return;
}

/* Reset Smart-Card Interface on SAM3U Device */
void scard_reset_init(void)
{
	usart_iso7816_opt_t conf_iso7816_t;
	conf_iso7816_t.iso7816_hz = ISO7816_BAUDRATE *ISO7816_FI_DI; //4000000UL;
	conf_iso7816_t.fidi_ratio = ISO7816_FI_DI;
	conf_iso7816_t.parity_type= US_MR_PAR_EVEN;
	conf_iso7816_t.inhibit_nack = 0x00;
	conf_iso7816_t.dis_suc_nack = 0x00;
	conf_iso7816_t.max_iterations = 0x03;
	conf_iso7816_t.bit_order = 0x00;
	conf_iso7816_t.protocol_type = 0x00;	/* T=0 Protocol */
	
	sysclk_enable_peripheral_clock(SCARD_USART_ID);
	gpio_configure_pin(PIN_USART2_TXD, PIN_USART2_TXD_FLAGS);
	gpio_configure_pin(PIN_USART2_SCK, PIN_USART2_SCK_FLAGS);
	gpio_configure_pin(PIN_ISO7816_RST_IDX, PIN_ISO7816_RST_FLAG);
	iso7816_init(SCARD_USART, &conf_iso7816_t, sysclk_get_cpu_hz(), PIN_ISO7816_RST_IDX);
	
	/* Reset (includes power off/on */
	iso7816_warm_reset(SCARD_USART);
	
	iso7816_data_block_atr(SCARD_USART, scard_atr, &scard_atr_size);
	scard_protocol = iso7816_decode_atr(SCARD_USART, scard_atr);
	
	printf("ATR (%d len): ", scard_atr_size);
	for(unsigned int i = 0; i < scard_atr_size; i++){
		printf("%02x ", scard_atr[i]);
	}
	printf("\n");
	
	if (scard_protocol == 1) {
		conf_iso7816_t.protocol_type  = 1;
		conf_iso7816_t.max_iterations = 0;

		usart_init_iso7816(SCARD_USART, &conf_iso7816_t, sysclk_get_cpu_hz());

		/* Enable TX and RX. */
		usart_enable_rx(SCARD_USART);
		usart_enable_tx(SCARD_USART);
	}

}
