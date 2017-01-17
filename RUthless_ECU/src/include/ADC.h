/*
 * ADC.h
 *
 * Created: 8.12.2016 14:33:23
 *  Author: jbbja
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "global.h"


#define ADC_CH0 0
#define ADC_CH1 1
#define ADC_CH2 2
#define ADC_CH3 3
#define ADC_CH4 4
#define ADC_CH5 5
#define ADC_CH6 6
#define ADC_CH7 7
#define ADC_CH8 8
#define ADC_CH9 9
#define ADC_CH10 10
#define ADC_CH11 11
#define ADC_CH12 12
#define ADC_CH13 13
#define ADC_CH14 14
#define ADC_CH15 15
// Channel 15 is temperature sensor

/* Define active ADC channels, used for interrupts */
#define NR_OF_ACTIVE_ADC_CHANNELS 5
#define MAX_NR_OF_ADC_CHANNELS 15
/* Array for the ADC channel numbers */
volatile uint8_t AdcChannels[NR_OF_ACTIVE_ADC_CHANNELS];

/* Data array for the configured active ADC channels */
volatile uint16_t AdcData[MAX_NR_OF_ADC_CHANNELS];

/* Global flag to indicate new results from interrupt */
// NOT USED AT THE MOMENT, maybe unnecessary
volatile uint8_t AdcFlag;

void adc_initialize(void); 
void adc_turn_on_multiple_channels (uint8_t channel_number[NR_OF_ACTIVE_ADC_CHANNELS], uint8_t enable_adc_interrupt, uint8_t adc_interrupt_priority);

#endif /* ADC_H_ */