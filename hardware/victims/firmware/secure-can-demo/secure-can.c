/*
 This file is part of the ChipWhisperer Example Targets
 Copyright (C) 2012-2017 NewAE Technology Inc.

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


#include "hal.h"
#include "seccan.h"
#include "simpleserial.h"
#include <stdint.h>
#include <string.h>
#include "aes-independant.h"

//#define CAN_TWO_WAY 1

//sends the info in packet over the CAN bus
void send_can_packet(seccan_packet *packet)
{
	int rval = 0;
	if (rval = write_can(packet->ext_id, packet->payload, 8), rval < 0) {
	}
}

//reads a message from the CAN bus and moves the data into packet
int read_can_packet(seccan_packet *packet)
{
	int rval = 0;
	if (rval = read_can(packet->payload, &packet->ext_id, 8), rval < 0) {
		return -1;
	} else {
		if (rval != 8) {
			return -1;
		} else {

			return 0;
		}
	}
}

void setup(void)
{
	platform_init();
	init_uart();
	init_can();

	trigger_setup();
	aes_indep_init();
	
#ifndef STM_ADC
	init_pwm();
#endif
#ifdef STM_ADC
	init_adc();
#endif
}

//loop for the STM that controls the motor
void master_stm_loop(void)
{
	can_input their_data;
	seccan_packet packet;
	while (1) {
			if (!read_can_packet(&packet)) {
				if (decrypt_can_packet(&their_data, &packet))
					continue;
				uint16_t voltage = their_data.data[0] | (their_data.data[1] << 8);
				change_pwm(voltage << 4);
			}

	}
}

//loop for the ADC STM32
void adc_stm_loop(void)
{
	int adcerr;
	can_input my_data = {
				.msgnum = 0x0,
				.baseid = 0x200,
				.data = {0x12, 0x34, 0x56, 0x78}
	};

	seccan_packet packet;

	while(1) {
		//packet all good, so start doing adc
		uint16_t adc_value = 0;
		if (adcerr = read_adc(&adc_value), adcerr == 0) {
			for (volatile unsigned int i = 0; i < 5000; i++);
			my_data.data[0] = (adc_value) & 0xFF;
			my_data.data[1] = (adc_value >> 8);
			encrypt_can_packet(&packet, &my_data);
			send_can_packet(&packet);
			my_data.msgnum++;
		} else {
			
		}

	}
}

int main(void) {
	setup();
#ifdef STM_ADC
	adc_stm_loop();
#else
	master_stm_loop();
#endif
}

/*
static void print_can_error(char *pstring, can_return_t canError) {
	switch (canError) {
	case CAN_RET_TIMEOUT:
		send_string("CAN_RET_TIMEOUT \n");
		break;
	case CAN_RET_BUSY:
		send_string("CAN_RET_BUSY\n");
		break;
	case CAN_RET_ERROR:
		send_string("CAN_RET_ERROR\n");
		break;
	default:
		send_string("CAN_RET_ERROR_UNKNOWN\n");
	}
	return;
}

static void print_adc_error(char *str, adc_return_t err) {
	switch(err) {
	case ADC_RET_ADC_INIT:
			send_string("ADC Init");
			break;
	case ADC_RET_CHANNEL_INIT:
		send_string("ADC channel Init");
			break;
	case ADC_RET_ADC_START:
		send_string("ADC Start");
		break;
	case ADC_RET_ADC_TIMEOUT:
		send_string("ADC Timeout");
		break;
	case ADC_RET_ADC_STOP:
		send_string("ADC stop");
		break;
	default:
		send_string("Unknown adc error");

	}
}
*/
