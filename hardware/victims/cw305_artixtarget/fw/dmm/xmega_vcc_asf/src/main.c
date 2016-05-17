/*
 Copyright (c) 2015-2016 NewAE Technology Inc. All rights reserved.

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
#define F_CPU 2000000UL
#include <util/delay.h>

#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define MASK_DIGIT0 (1<<3)
#define MASK_DIGIT1 (1<<1)
#define MASK_DIGIT2 (1<<0)
#define MASK_DIGIT012 (1<<0) | (1<<1) | (1<<3)

#define MASK_DP (1<<3)
#define MASK_A (1<<5)
#define MASK_B (1<<4)
#define MASK_C (1<<2)
#define MASK_D (1<<1)
#define MASK_E (1<<0)
#define MASK_F (1<<6)
#define MASK_G (1<<7)

#define SEG_CA 0
//#define SEG_CA 1

void display(uint8_t numeral, uint8_t dp_on, uint8_t diginum);
int adc_init(void);

#define EEPROM_ADDR_ID 2
#define EEPROM_ADDR_OFFSET_NEG 3
#define EEPROM_ADDR_OFFSET_POS 4
#define EEPROM_ID 0xBE

#define AVG_READINGS 64

int main(void)
{
	board_init();
	sysclk_init();
	adc_init();
	
	//Offset is stored in EEPROM
	int adc_offset = 0;
	if (nvm_eeprom_read_byte(EEPROM_ADDR_ID) == EEPROM_ID){
		adc_offset = nvm_eeprom_read_byte(EEPROM_ADDR_OFFSET_POS) - nvm_eeprom_read_byte(EEPROM_ADDR_OFFSET_NEG);
	}

	
    PORTE_DIRSET = MASK_DIGIT012;
	PORTD_DIRSET = 0xFF;
	
	int32_t i = 0;	
	uint8_t disp = 0;
	uint8_t adccnt = 0;
	
	unsigned int adc_readings[AVG_READINGS];
	
	//Setup ADC hardware
	adc_init();
	
	//Main loop...measure VCC-INT
	while(1){	
		// Vref = 2.048V
		// 4096 count ADC (12-bit)
		// 2.048V / 4096count = 0.0005 V/Count
		// We want i to be result in mV
		// = 0.5 mV/count
		//So divide count by 2 to get mV reading		
		if (adccnt < AVG_READINGS){
			adc_readings[adccnt] = adc_get_unsigned_result(&MY_ADC, MY_ADC_CH);
		
			if (adc_readings[adccnt] < 0){
				adc_readings[adccnt] = 0;
			}			
			adccnt++;
		} else {
			int32_t adctemp = 0;
			for (adccnt = 0; adccnt < AVG_READINGS; adccnt++){
				adctemp += (int32_t)(adc_readings[adccnt] + adc_offset);
			}
			i = adctemp / AVG_READINGS;
			//Limit negative values to 0
			//i = i - 2048;
			if (i < 0) i = 0;
			i = i / 2;
			
			adccnt = 0;
		}
		
		//Switch between mV and V ranges
		if (i > 999){
			if (disp == 0){
				display((i / 10)%10, 0, 2);
				_delay_ms(2);
			} else if (disp == 1){
				display((i / 100) % 10, 0, 1);
				_delay_ms(2);
			} else {
				display(i / 1000, 1, 0);
				_delay_ms(2);
			}
		} else {
			if (disp == 0){
				display(i % 10, 0, 2);
				_delay_ms(2);
			} else if (disp == 1){
				display((i / 10) % 10, 0, 1);
				_delay_ms(2);
			} else {
				display(i / 100, 0, 0);
				_delay_ms(2);
			}
		}
		
		disp++;
		if (disp > 2){
			disp = 0;		
		}
		
		adc_start_conversion(&MY_ADC, MY_ADC_CH);
	}
}

/* Update the numeral */
void display(uint8_t numeral, uint8_t dp_on, uint8_t diginum)
{
	PORTE_OUTCLR = MASK_DIGIT012;
	if (diginum == 0){
		PORTE_OUTSET = MASK_DIGIT0;
		} else if (diginum == 1){
		PORTE_OUTSET = MASK_DIGIT1;
		} else {
		PORTE_OUTSET = MASK_DIGIT2;
	}
	
	switch (numeral){
		case 0:
		PORTD_OUT = MASK_A | MASK_B | MASK_C | MASK_D | MASK_E | MASK_F;
		break;
		
		case 1:
		PORTD_OUT = MASK_B | MASK_C;
		break;
		
		case 2:
		PORTD_OUT = MASK_A | MASK_B | MASK_G | MASK_E | MASK_D;
		break;
		
		case 3:
		PORTD_OUT = MASK_A | MASK_B | MASK_G | MASK_C | MASK_D;
		break;
		
		case 4:
		PORTD_OUT = MASK_F | MASK_G | MASK_B | MASK_C;
		break;
		
		case 5:
		PORTD_OUT = MASK_A | MASK_F | MASK_G | MASK_C | MASK_D;
		break;
		
		case 6:
		PORTD_OUT = MASK_A | MASK_F | MASK_G | MASK_C | MASK_D | MASK_E;
		break;
		
		case 7:
		PORTD_OUT = MASK_A | MASK_B | MASK_C;
		break;
		
		case 8:
		PORTD_OUT = MASK_A | MASK_B | MASK_C | MASK_D | MASK_E | MASK_F | MASK_G;
		break;
		
		case 9:
		PORTD_OUT = MASK_A | MASK_B | MASK_G | MASK_F | MASK_C | MASK_D;
		break;
		
		default:
		PORTD_OUT = MASK_F;
	}
	
	if (dp_on){
		PORTD_OUT |= MASK_DP;
		} else {
		PORTD_OUT &= ~(MASK_DP);
	}
	
	if (SEG_CA){
		PORTD_OUT = ~PORTD_OUT;
	}
	
}

/* Setup the external ADC */

int adc_init(void)
{
	/* Offset Measurement*/
	/*
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_AREFA);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 62500UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN4, ADCCH_NEG_PIN4, 1);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_enable(&MY_ADC);
	*/
	
	/* Normal Measurement*/
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_AREFA);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 62500UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN4, ADCCH_NEG_NONE, 1); //PAD_GND
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_enable(&MY_ADC);
	
	return 0;
}
