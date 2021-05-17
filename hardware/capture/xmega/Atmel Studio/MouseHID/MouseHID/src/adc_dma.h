#ifndef ADC_DMA_H
#define ADC_DMA_H

#define ADC_DATA_LENGTH 1024
extern int16_t adc_data[ADC_DATA_LENGTH];
extern volatile bool adc_data_ready;
extern volatile bool adc_data_dlbusy;
extern volatile bool adc_data_busy;

void dma_init(void);
void adc_init(void);
void timer_setup(void);
void adc_go(void);
void adc_stop(void);
void interrupt_setup(void);

//! This function is called when a HID report is received
//!
void main_hid_outreport(uint8_t *report);

#endif //ADC_DMA_H