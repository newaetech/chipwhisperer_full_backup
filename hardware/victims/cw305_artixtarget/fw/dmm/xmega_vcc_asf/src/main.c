/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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

#define AVG_READINGS 64

int main(void)
{
	board_init();
	sysclk_init();
	adc_init();

	
    PORTE_DIRSET = MASK_DIGIT012;
	PORTD_DIRSET = 0xFF;
	
	int i = 0;	
	uint8_t disp = 0;
	uint8_t adccnt = 0;
	
	unsigned int adc_readings[AVG_READINGS];
	int adc_offset = 4;
	
	adc_init();
	
	while(1){	
		// Vref = 2.048V
		// 4096 count ADC (12-bit)
		// 2.048V / 4096count = 0.0005 V/Count
		// We want i to be result in mV
		// = 0.5 mV/count
		//So divide count by 2 to get mV reading		
		if (adccnt < AVG_READINGS){
			adc_readings[adccnt] = adc_get_result(&MY_ADC, MY_ADC_CH);	
		
			if (adc_readings[adccnt] < 0){
				adc_readings[adccnt] = 0;
			}			
			adccnt++;
		} else {
			uint32_t adctemp = 0;
			for (adccnt = 0; adccnt < AVG_READINGS; adccnt++){
				adctemp += adc_readings[adccnt] + adc_offset;
			}
			i = adctemp / AVG_READINGS;
			adccnt = 0;
		}
		
		if (i < 0) i = 0;
		
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
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12, ADC_REF_AREFA);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 62500UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN4, ADCCH_NEG_PAD_GND, 1);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_enable(&MY_ADC);
	
	return 0;
}
