#ifndef __ADC_H__
#define __ADC_H__

void adc_buffered_init(uint32_t adc_ch);
int adc_buffered_deinit(uint32_t adc_ch);
int adc_init(uint32_t adc_ch, uint32_t flags);
uint16_t Get_ConversionVal(uint8_t ch);

#endif //__ADC_H__
