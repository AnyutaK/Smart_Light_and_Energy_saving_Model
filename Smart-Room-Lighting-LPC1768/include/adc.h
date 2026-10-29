#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void     adc_init(void);
uint16_t adc_read_raw(void);          /* one 12-bit reading, 0..4095      */
uint16_t adc_read_avg(void);          /* average of ADC_SAMPLES_AVG reads */
uint16_t adc_to_percent(uint16_t raw);/* 0..100 normalized                */

#endif
