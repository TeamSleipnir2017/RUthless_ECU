/*
 * eeprom.c
 *
 * Created: 21.1.2017 14:14:22
 *  Author: jbbja
 */ 

 #include "eeprom.h"

 void eeprom_init(void)
 {
	pmc_enable_periph_clk(ID_TWI1);				// Enable peripheral clock
	#define TWI_DATA_PIN PIO_PB12A_TWD1			// Define TWI data pin
	#define TWI_CLOCK_PIN PIO_PB13A_TWCK1		// Define TWI clock pin
	#define TWI_PERIPHERAL PIOB					// Define TWI peripheral (either A(TWI0) or B(TWI1))
	TWI_PERIPHERAL->PIO_IDR = TWI_DATA_PIN;		// Interrupt disable register
	TWI_PERIPHERAL->PIO_IDR = TWI_CLOCK_PIN;	
	TWI_PERIPHERAL->PIO_PDR = TWI_DATA_PIN;		// Disable input/output which enables peripheral mode
	TWI_PERIPHERAL->PIO_PDR = TWI_CLOCK_PIN;
	TWI_PERIPHERAL->PIO_ABSR &= ~TWI_DATA_PIN;	// Enable Peripheral A (TWI)
	TWI_PERIPHERAL->PIO_ABSR &= ~TWI_CLOCK_PIN;
	// Initialize TWI 
	twi_options_t opt;
	opt.master_clk = sysclk_get_cpu_hz(); opt.speed = 400000; // 400KHz clock frequency
	twi_master_init(TWI1, &opt);
 }