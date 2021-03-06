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

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();

	// Initialize pins
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);
	// Configure pins as output MOVED TO CYLINDER INIT 17.4.17 JBB
 	pio_set_output(PIOC, IGN1_OUT, HIGH, FALSE, FALSE); // debug pin
	pio_set_output(PIOC, IGN2_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, IGN3_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, IGN4_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, IGN5_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, IGN6_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, IGN7_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, IGN8_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, IGN_AUX_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, INJ1_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOD, INJ2_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, INJ3_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, INJ4_OUT, HIGH, FALSE, FALSE);
	pio_set_output(PIOC, INJ5_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, INJ6_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, INJ7_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, INJ8_OUT, LOW, FALSE, FALSE);
	pio_set_output(PIOC, INJ_AUX_OUT, LOW, FALSE, FALSE);
	
	// Initialize UART communication
	uart_init();
	// Initialize interrupts for UART
	uart_enable_rx_interrupt();
	uart_tx_interrupt_init();
	uart_print_string("Init begin"); uart_new_line();

	// Initialize TWI communication for EEPROM
	eeprom_init();

	// Initialize necessary global parameters
	global_init();
	
	// Initialize Analog to Digital Converter
	adc_initialize();
	uint8_t channel_number[NR_OF_ACTIVE_ADC_CHANNELS] = {ADC_CLT_CH, ADC_IAT_CH, ADC_AFR_CH, ADC_MAP_CH, ADC_TPS_CH, ADC_BATT_CH, ADC_AFR_CH};
	adc_turn_on_multiple_channels(channel_number, TRUE, ADC_PRIORITY);
	adc_start(ADC);
	// Initialize ADC timer frequency, how often ADC values are measured
	timer_init(GLOBAL_TIMER, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC8_PRIORITY);
	tc_write_ra(TC2, 2, GLOBAL_TIMER_FREQ/GlobalTimerFreqADCScaler);
	tc_write_rc(TC2, 2, GLOBAL_TIMER_FREQ/MILLI_SEC);
	
	// Initialize sensors look up vectors by reading them from EEPROM
	sensors_init();

	// Initialize communication with tunerstudio
	tunerstudio_init();	
	
	/************* RJR **************/
	
	// Enable external interrupt for PIOA register, crank and cam
	interrupts_enable_pio(ID_PIOA, CRANK_SIGNAL, PIOA_PRIORITY, INTERRUPT_FALLING_EDGE_MODE);
	interrupts_enable_pio(ID_PIOA, CAM_SIGNAL, PIOA_PRIORITY, INTERRUPT_FALLING_EDGE_MODE);
	
	// Initiate timer interrupt, Moved to global.c 22.4.17 JBB
// 	timer_init(CYLINDER_1_TIMER, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC0_PRIORITY);
// 	timer_init(CYLINDER_2_TIMER, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC1_PRIORITY);
// 	timer_init(CYLINDER_3_TIMER, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC2_PRIORITY);
// 	timer_init(CYLINDER_4_TIMER, TC_CMR_WAVE | TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_EEVT_XC0, TC_IER_COVFS | TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS, TC3_PRIORITY);
	
	uart_print_string("Init done"); uart_new_line();

	
	
	debug_init_usart(&myDebug, USART0, 500000);
	
	// DEBUGGGGGGGGGGGGGGGG !!!!!!!!!!!!!!
	
	while (1)
 	{		
		if (AdcFlag)
		{
 			AdcFlag = FALSE;
			// TODO: Maybe disable interrupts
			sensors_read_adc();
			// TODO: Enable interrupts
// 			for (uint8_t i = 0; i < NR_OF_ACTIVE_ADC_CHANNELS; i++)
// 			{
// 			
// 				uart_print_string("Channel"); uart_print_int(channel_number[i]);uart_print_string(" :");uart_print_int(math_find_median(AdcData[AdcChannels[i]], ADC_MEDIAN_FILTER_LENGTH)); uart_new_line();
// 				__asm__("nop");
// 			}
		}
		
		if (RxFlag)
		{
			RxFlag = FALSE;
			uart_rx_read_buffer();
		}
		
		if (CrankSignalFlag)
		{
			CrankSignalFlag = FALSE;
			decoders_crank_primary();
			CrankSignalIntervalMillis = millis;
		}
		if (CrankNewCycleFlag)
		{
			CrankNewCycleFlag = FALSE;
			if (isDebug)
			{
// 				debug_cylinder[0].RealTimeLastRevCounts = LastCrankRevCounts;
// 				debug_cylinder[1].RealTimeLastRevCounts = LastCrankRevCounts;
// 				debug_cylinder[2].RealTimeLastRevCounts = LastCrankRevCounts;
// 				debug_cylinder[3].RealTimeLastRevCounts = LastCrankRevCounts;
				//uart_load_pdc_tx_buffer(&debug_cylinder, sizeof(debug_cylinder));
				//uart_print_string("PIOAHandlerTimeInCounts"); uart_print_int(PIOAHandlerTimeInCounts); uart_new_line();
			}
			debug_transfer_struct(&myDebug, &debug_cylinders, sizeof(debug_cylinders));
		}
		if ((millis > CrankSignalIntervalMillis + MILLI_SEC) || engine_realtime.Rpm < 100) // Motor not running (last crank signal was last received a second ago)
		{
			engine_realtime.Rpm = 0;
		}
		if (engine_realtime.Rpm < 100)
		{
			global_set_inj_and_ign_output_off();
			engine_realtime.EngineStatus |= ENGINE_RUNNING;
		}
		
		if (CrankCycleCounter % 2 == 0)
		{
			
		}
		else
		{
			engine_realtime.EngineStatus &= ~ENGINE_RUNNING;
		}
		
 	}
	
}

