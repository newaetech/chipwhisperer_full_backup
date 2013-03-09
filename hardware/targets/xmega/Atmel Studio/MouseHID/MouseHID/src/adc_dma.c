#include <asf.h>
#include "adc_dma.h"
int16_t adc_data[ADC_DATA_LENGTH];
#define DMA_CHANNEL     0

static void dma_transfer_done(enum dma_channel_status status)
{
	dma_channel_disable(DMA_CHANNEL);	
	adc_stop();	
	PORTC.OUTCLR = 1<<6;
	dma_init();
	adc_data_dlbusy = true;
}

void dma_init(void)
{
	struct dma_channel_config dmach_conf;
	memset(&dmach_conf, 0, sizeof(dmach_conf));

	dma_channel_set_burst_length(&dmach_conf, DMA_CH_BURSTLEN_2BYTE_gc);
	dma_channel_set_transfer_count(&dmach_conf, ADC_DATA_LENGTH*2);

	dma_channel_set_src_reload_mode(&dmach_conf,
	DMA_CH_SRCRELOAD_BURST_gc);
	dma_channel_set_dest_reload_mode(&dmach_conf,
	DMA_CH_DESTRELOAD_TRANSACTION_gc);

	dma_channel_set_src_dir_mode(&dmach_conf, DMA_CH_SRCDIR_INC_gc);
	dma_channel_set_dest_dir_mode(&dmach_conf, DMA_CH_DESTDIR_INC_gc);

	dma_channel_set_source_address(&dmach_conf,
	(uint16_t)(uintptr_t)&ADCA.CH0RES);
	dma_channel_set_destination_address(&dmach_conf,
	(uint16_t)(uintptr_t)adc_data);

	dma_channel_set_trigger_source(&dmach_conf, DMA_CH_TRIGSRC_ADCA_CH0_gc);
	dma_channel_set_single_shot(&dmach_conf);

	dma_enable();

	dma_set_callback(DMA_CHANNEL, dma_transfer_done);
	dma_channel_set_interrupt_level(&dmach_conf, DMA_INT_LVL_LO);

	dma_channel_write_config(DMA_CHANNEL, &dmach_conf);
}

/** Differential Example **/
//Check Datasheet: this can only be certain pins
#define POS_CHANNEL	ADCCH_POS_PIN4
//Check Datasheet: this can only be certain pins
#define NEG_CHANNEL ADCCH_NEG_PIN5
//Gain can take: 1,2,4,8,16,32,64 
#define GAIN		1

/** Single-Ended Example **/
//define POS_CHANNEL	ADCCH_POS_PIN0
//#define NEG_CHANNEL	ADCCH_NEG_NONE
//#define GAIN			1

void adc_init(void)
{
	struct adc_config adc_conf;	
	adc_read_configuration(&ADCA, &adc_conf);	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_BANDGAP);
	adc_set_clock_rate(&adc_conf, 2000000UL);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_FREERUN, 1, 0);	
	adc_write_configuration(&ADCA, &adc_conf);
	
	struct adc_channel_config adcch_conf;
	memset(&adcch_conf, 0, sizeof(struct adc_channel_config));
	
	adcch_set_interrupt_mode(&adcch_conf, ADCCH_MODE_COMPLETE);
	adcch_disable_interrupt(&adcch_conf);
		
	adcch_set_input(&adcch_conf, POS_CHANNEL, NEG_CHANNEL, GAIN);	
	adcch_write_configuration(&ADCA, 1, &adcch_conf);
	
	adc_enable(&ADCA);
}

void adc_stop(void)
{
	struct adc_config adc_conf;
	adc_read_configuration(&ADCA, &adc_conf);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_write_configuration(&ADCA, &adc_conf);	
	adc_disable(&ADCA);
}

void timer_setup(void)
{
	 ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 0), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	 tc_enable(&TCC0);
	 tc_set_wgm(&TCC0, TC_WG_FRQ);
	 tc_write_period(&TCC0, 5);
	 tc_write_cc(&TCC0, TC_CCA, 2);
	 tc_enable_cc_channels(&TCC0,TC_CCAEN);
	 tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}

#define TRIGPORT PORTC
#define TRIGPIN 1
#define PREF IOPORT_CREATE_PIN(PORTC, 1)

void interrupt_setup(void)
{
	ioport_configure_pin(PREF, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PREF, IOPORT_MODE_PULLDOWN);
	ioport_set_pin_sense_mode(PREF, IOPORT_SENSE_RISING);	
	TRIGPORT.INTCTRL = (TRIGPORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_HI_gc;
	TRIGPORT.INT0MASK = 1<<1;
	PMIC.CTRL |= PMIC_HILVLEN_bm;
	
	
	//For Debug
	PORTC.DIRSET = 1<<6;
	PORTC.OUTCLR = 1<<6;
}

ISR(PORTC_INT0_vect)
{	
	//adc_flush(&ADCA);
	irqflags_t flags = cpu_irq_save();
	
	memset(adc_data, 0, ADC_DATA_LENGTH*2);
	
	dma_channel_enable(DMA_CHANNEL);
	adc_init();
	
	PORTC.OUTSET = 1<<6;
	
	LED_Toggle(LED1_GPIO);	
	adc_data_busy = true;
	
	cpu_irq_restore(flags);
}


void main_hid_outreport(uint8_t *report)
{
	if (report[0]=='1') {
		// A led must be on
		switch(report[1]) {
			case '2':
			LED_On(LED0_GPIO);
			break;
			case '3':
			LED_On(LED1_GPIO);
			break;
		}
		} else {
		// A led must be off
		switch(report[1]) {
			case '2':
			LED_Off(LED0_GPIO);
			break;
			case '3':
			LED_Off(LED1_GPIO);
			break;
		}
	}
}