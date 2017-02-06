/*
 * ADC.c
 *
 * Created: 8.12.2016 14:33:07
 *  Author: jbbja
 */

#include "ADC.h" 

/* A global Analog to Digital Controller initialization function */
void adc_initialize(void)
{
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(), ADC_FREQ_MIN, ADC_STARTUP_TIME_4);
	adc_configure_timing(ADC, 1, ADC_SETTLING_TIME_3, 1);
	adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_10);
	adc_configure_trigger(ADC, ADC_TRIG_SW, ADC_MR_FREERUN_OFF);
	adc_enable_tag(ADC);

	// Initialize data arrays
	for (uint8_t i = 0; i < MAX_NR_OF_ADC_CHANNELS; i++)
	{
		if (i < NR_OF_ACTIVE_ADC_CHANNELS)
			AdcChannels[i] = 0;
		for (uint16_t j = 0; j < ADC_MEDIAN_FILTER_LENGTH; j++)
			AdcData[i][j] = 0;
		AdcMedianCounter[i] = 0;
	}

	
}
/* A global turn on ADC channels function, remember to define how many channels are used */
void adc_turn_on_multiple_channels (uint8_t ChannelNumber[NR_OF_ACTIVE_ADC_CHANNELS], uint8_t EnableAdcInterrupt, uint8_t AdcInterruptPriority)
{
	for (uint8_t i = 0; i < NR_OF_ACTIVE_ADC_CHANNELS; i++){
		// Turn on each described channel
		adc_enable_channel(ADC, ChannelNumber[i]);
		// Store the channel numbers for interrupts
		AdcChannels[i] = ChannelNumber[i];	
		//uart_print_string("Channel: "); uart_print_int(channel_number[i]);
	}
	if (EnableAdcInterrupt)
	{
		adc_enable_interrupt(ADC, ADC_IER_DRDY);
		interrupts_enable_interrupt_vector(ADC_IRQn, AdcInterruptPriority);
	}
}

void ADC_Handler(void)
{
	/************************************************************************/
	/* Register ADC_LCDR 
	15 14 13 12		11 10 9 8
	CHNB			LDATA
	7 6 5 4 3 2 1 0
	     LDATA   
	CHNB: Channel number
	LDATA: Last Data Converted */
	/************************************************************************/
	uint16_t Result = ADC->ADC_LCDR; //Last converted data register
	uint16_t ChannelNumber = ((Result & 0xF000) >> 12);
	AdcData[ChannelNumber][AdcMedianCounter[ChannelNumber]] = (0x0FFF & Result); // Discard channel number from results
	AdcMedianCounter[ChannelNumber] = (AdcMedianCounter[ChannelNumber] + 1) % ADC_MEDIAN_FILTER_LENGTH;
	AdcFlag = TRUE;
}