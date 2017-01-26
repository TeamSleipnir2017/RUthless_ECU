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
#include "include/global.h"

void waste_of_time_delay(uint32_t delay)
{
	for (uint32_t i = 0; i < delay; i++)
	{
		for (uint32_t j = 0; j < 1978; j++)
		{
			__asm__("nop");
		}
	}
}

#define TEST 13



int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();
	
	// Initialize UART communication
	uart_init();
	// Initialize interrupts for UART
	uart_rx_interrupt_init();
	uart_tx_interrupt_init();
	
	
	// Initialize Debug pins
	/************************************************************************/
	/* pio_set(PIOx, Pin)
	pio_clear(PIOx, Pin)*/
	/************************************************************************/
	pmc_enable_periph_clk(ID_PIOC);
	// Configure pin as output
	pio_set_output(PIOC, IGN1_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, IGN2_OUT, LOW, FALSE, FALSE);
	pio_clear(PIOC, IGN1_OUT);
	pio_clear(PIOC, IGN2_OUT);
	
	// Initialize necessary global parameters
	global_init();
	
	// Initialize Analog to Digital Converter
	adc_initialize();
	uint8_t channel_number[NR_OF_ACTIVE_ADC_CHANNELS] = {4, 6, 1, 2, 12};
	adc_turn_on_multiple_channels(channel_number, TRUE, ADC_PRIORITY);
	adc_start(ADC);
	// Initialize ADC timer frequency, how often ADC values are measured
	// CLOCK3 = MCK/32, TC_CMR_WAVE is to disable Capture mode, Enable Overflow and compare reg A
	timer_init(GLOBAL_TIMER, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE, TC_IER_COVFS | TC_IER_CPAS, TC8_PRIORITY);
	tc_write_ra(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
	
	// Initialize TWI communication for EEPROM
	eeprom_init();
	
	// TEST 
	// Test tunerstudio comms
	table_init();
	tunerstudio_init();

	uart_print_string("Init done\n");

	while (1)
	{
		if (RxFlag)
		{
			uart_rx_read_buffer();
			RxFlag = LOW;
		}
		//waste_of_time_delay(10000);
		

		//waste_of_time_delay(10000);
		//uart_print_string("RxStringHead "); uart_print_int(RxStringHead); uart_new_line();
		//uart_print_string("RxStringTail "); uart_print_int(RxStringTail); uart_new_line();
		
		if (AdcFlag)
		{
			AdcFlag = FALSE;
			for (uint8_t i = 0; i < NR_OF_ACTIVE_ADC_CHANNELS; i++)
			{
				//uart_print_string("Channel"); uart_print_int(channel_number[i]);uart_print_string(" :");uart_print_int(AdcData[AdcChannels[i]]); uart_new_line();
				__asm__("nop");
			}
		}
// 		waste_of_time_delay(10000);
// 		uart_print_string("Channel"); uart_print_int(channel_number[cnt]);uart_print_string(" :");uart_print_int(AdcData[AdcChannels[cnt]]); uart_new_line();
// 		cnt = (cnt + 1) % NR_OF_ACTIVE_ADC_CHANNELS;
// 		adc_start(ADC);
	}
	
}
